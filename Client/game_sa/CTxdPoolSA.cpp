/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTxdPoolSA.h"
#include "CGameSA.h"
#include "CKeyGenSA.h"
#include "CModelInfoSA.h"
#include "CStreamingSA.h"
#include "MemSA.h"
#include <cstring>
#include <limits>
#include <unordered_set>

// Slots in this set are shielded from SA's streaming eviction.
// Hook_CTxdStore_RemoveRef will NOT call CStreaming::RemoveModel when a
// protected slot's ref count drops to zero, allowing MTA to orphan textures
// from the RwTexDictionary before it is destroyed.
static std::unordered_set<unsigned short> g_StreamingProtectedTxdSlots;

#define VAR_CTxdStore_ms_pTxdPool   0xC8800C
#define FUNC_CTxdStore__RemoveRef   0x731A30
#define FUNC_CStreaming__requestTxd 0x407100
#define FUNC_CTxdStore__RemoveSlot  0x731E90

// CMemoryMgr::MallocAlign / FreeAlign wrappers (hooked by MTA in
// CMultiplayerSA_FixMallocAlign.cpp > SafeMallocAlign / SafeFreeAlign).
// SafeMallocAlign uses MTA's CRT malloc with alignment metadata at ptr-4/ptr-8.
// SafeFreeAlign validates the metadata magic before freeing; if the magic is
// absent (e.g. memory from SA's CRT), it silently returns without freeing.
//
// We use these for Resize()'s replacement pool arrays. SA's original CPool
// constructor allocates via operator new (SA's static CRT), but MTA's hooked
// allocator provides controlled lifetime and avoids cross-CRT issues.
namespace CMemoryMgr
{
    inline void* MallocAlign(int size, int alignment = 4, int hint = 0)
    {
        using fn_t = void*(__cdecl*)(int, int, int);
        return reinterpret_cast<fn_t>(0x72F4C0)(size, alignment, hint);
    }

    inline void FreeAlign(void* ptr)
    {
        using fn_t = void(__cdecl*)(void*);
        reinterpret_cast<fn_t>(0x72F4F0)(ptr);
    }
}

// GetNextFileOnCd TXD dependency check hook:
// At 0x408ECE, SA reads the TXD streaming entry loadState for DFF dependency:
//   movzx edx, byte ptr [edx*4+0x946750]   ; 8 bytes: 0F B6 14 95 50 67 94 00
// For overflow TXD slots (txdIndex >= 5000), this reads COL/IPL streaming
// entries instead. For overflow slots, always report LOADED so SA never
// calls CStreaming::RequestModel with an out-of-range streaming ID.
#define HOOKPOS_GetNextFileOnCd_TxdCheck  0x408ECE
#define HOOKSIZE_GetNextFileOnCd_TxdCheck 8
static const DWORD RETURN_GetNextFileOnCd_TxdCheck = 0x408ED6;

// RequestModelStream TXD dependency check hook:
// At 0x40CD9F, SA reads the TXD streaming entry loadState before packing a
// DFF into a streaming request:
//   mov al, byte ptr [ecx*4+0x8E4CD0]   ; 7 bytes: 8A 04 8D D0 4C 8E 00
// Same overflow problem as GetNextFileOnCd. For overflow TXD slots, always
// report LOADED so the DFF is not indefinitely deferred.
#define HOOKPOS_RequestModelStream_TxdCheck  0x40CD9F
#define HOOKSIZE_RequestModelStream_TxdCheck 7
static const DWORD RETURN_RequestModelStream_TxdCheck = 0x40CDA6;

extern CGameSA* pGame;

// Hook for GetNextFileOnCd's TXD dependency check (0x408ECE).
// Replaces:  movzx edx, byte ptr [edx*4+0x946750]  (8 bytes)
// At entry: eax = txdIndex (from movsx eax, [ecx+0Ah])
//           edx = txdIndex*5 (from lea edx, [eax+eax*4])
// At exit:  edx = loadState byte, execution continues at 0x408ED6 (cmp edx, 1)
//
// Post-hook, SA checks: cmp edx, 1 (LOADED) > jz loc_408F01 (IFP dep check);
//                        cmp edx, 3 (READING) > jz loc_408F01 (same);
//                        otherwise > fallthrough to RequestModel(txdIndex+20000).
// For standard TXDs (txdIndex < 5000): executes the original loadState read.
// For overflow TXDs (>= 5000 or negative from movsx): always reports LOADED (1).
// This prevents SA from falling through to its direct
//   push edx; add eax, 4E20h; push eax; call CStreaming::RequestModel (0x4087E0)
// call, which is NOT guarded by Hook_CStreaming_requestTxd and would use
// an out-of-range streaming ID (>= 25000), corrupting COL/IPL streaming
// entries or writing out of bounds of ms_aInfoForModel[26316].
// Only EDX is live after return; eax and ecx are preserved (never touched
// in the overflow path).
static void __declspec(naked) HOOK_GetNextFileOnCd_TxdCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        // eax = txdIndex, edx = txdIndex*5
        test    eax, eax
        js      overflow_loaded         // Negative txdIndex from movsx of >= 32768
        cmp     eax, 5000               // SA_TXD_POOL_CAPACITY
        jge     overflow_loaded

        // Standard path: execute original instruction
        movzx   edx, byte ptr [edx*4+0x946750]
        jmp     RETURN_GetNextFileOnCd_TxdCheck

    overflow_loaded:
        // MTA manages overflow TXDs outside SA's streaming system.
        // Reporting LOADED is always safe:
        //  - Slot occupied: TXD data is present, DFF renders correctly.
        //  - Slot empty:    DFF may get white textures (recoverable),
        //                   vs. NOT_LOADED which triggers the unguarded
        //                   RequestModel call above (freeze/corruption).
        mov     edx, 1                  // LOADSTATE_LOADED
        jmp     RETURN_GetNextFileOnCd_TxdCheck
    }
    // clang-format on
}

// Hook for RequestModelStream's TXD dependency check (0x40CD9F).
// Replaces:  mov al, byte ptr [ecx*4+0x8E4CD0]  (7 bytes)
// At entry: eax = txdIndex+20000 (from movsx + add), ecx = (txdIndex+20000)*5
// At exit:  al = loadState byte, execution continues at 0x40CDA6 (cmp al, 1)
//
// Post-hook, SA checks: cmp al, 1 (LOADED) > jz proceed;
//                        cmp al, 3 (READING) > jz proceed;
//                        otherwise > jnz loc_40CEF8 (defer DFF).
// For standard TXDs (streaming ID < 25000): executes the original loadState read.
// For overflow TXDs (streaming ID >= 25000): always reports LOADED (1).
// Returning NOT_LOADED would prevent the DFF from ever being packed into
// a streaming request when its overflow TXD is managed by MTA.
// Only AL is live after return; ecx, edx, and eax upper bytes are dead.
static void __declspec(naked) HOOK_RequestModelStream_TxdCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        // eax = txdIndex+20000, ecx = (txdIndex+20000)*5
        cmp     eax, 25000                 // 20000 + SA_TXD_POOL_CAPACITY
        jge     overflow_loaded

        // Standard path: execute original instruction
        mov     al, byte ptr [ecx*4+0x8E4CD0]
        jmp     RETURN_RequestModelStream_TxdCheck

    overflow_loaded:
        mov     al, 1                      // LOADSTATE_LOADED
        jmp     RETURN_RequestModelStream_TxdCheck
    }
    // clang-format on
}

// Replaces CTxdStore::RemoveRef (0x731A30).
// The original calls CStreaming::RemoveModel(index + 20000) when the ref count
// drops to zero. For overflow pool slots (index >= 5000) that streaming ID
// falls outside ms_aInfoForModel. Skip CStreaming for overflow slots.
static void __cdecl Hook_CTxdStore_RemoveRef(int index)
{
    auto* pool = *reinterpret_cast<CPoolSAInterface<CTextureDictonarySAInterface>**>(VAR_CTxdStore_ms_pTxdPool);
    if (!pool)
        return;

    if (index < 0 || index >= pool->m_nSize)
        return;

    if (pool->m_byteMap[index].bEmpty)
        return;

    auto* entry = &pool->m_pObjects[index];
    --entry->usUsagesCount;

    // Signed comparison matches the original jg (jump if greater) at 0x731A4C
    if (static_cast<short>(entry->usUsagesCount) <= 0)
    {
        if (index < CTxdPoolSA::SA_TXD_POOL_CAPACITY && g_StreamingProtectedTxdSlots.count(static_cast<unsigned short>(index)) == 0)
        {
            using RemoveModel_t = void(__cdecl*)(int);
            reinterpret_cast<RemoveModel_t>(FUNC_RemoveModel)(index + pGame->GetBaseIDforTXD());
        }
    }
}

// Replaces CStreaming::requestTxd (0x407100).
// The original converts a TXD pool index to a stream ID (index + 20000) and
// calls CStreaming::RequestModel. For standard-range slots (< 5000) forward
// to RequestModel. For overflow slots (>= 5000) the stream ID falls in
// COL/IPL/DAT/IFP territory, so skip - the GetNextFileOnCd and
// RequestModelStream hooks always report LOADED for overflow TXDs.
static void __cdecl Hook_CStreaming_requestTxd(int txdIndex, int flags)
{
    if (txdIndex >= 0 && txdIndex < CTxdPoolSA::SA_TXD_POOL_CAPACITY)
    {
        using RequestModel_t = void(__cdecl*)(int, int);
        reinterpret_cast<RequestModel_t>(FUNC_CStreaming__RequestModel)(txdIndex + pGame->GetBaseIDforTXD(), flags);
        return;
    }

    // Overflow slots (>= 5000): skip. GetNextFileOnCd and RequestModelStream
    // hooks always report LOADED for overflow TXDs, so SA never tries to
    // stream them.
}

CTxdPoolSA::CTxdPoolSA()
{
    m_ppTxdPoolInterface = (CPoolSAInterface<CTextureDictonarySAInterface>**)VAR_CTxdStore_ms_pTxdPool;
    InstallPoolHooks();

    // Pool expansion deferred to InitialisePool() - called from
    // CGameSA::Initialize() after SA's CTxdStore::Initialise has
    // created the pool at 0xC8800C.
}

void CTxdPoolSA::InitialisePool()
{
    if (!m_ppTxdPoolInterface || !*m_ppTxdPoolInterface)
        return;

    const int currentSize = (*m_ppTxdPoolInterface)->m_nSize;
    if (currentSize >= TXD_POOL_MAX_CAPACITY)
        return;

    Resize(TXD_POOL_MAX_CAPACITY);
}

void CTxdPoolSA::InstallPoolHooks()
{
    // First two instructions = 4 + 6 = 10 bytes
    HookInstall(FUNC_CTxdStore__RemoveRef, reinterpret_cast<DWORD>(Hook_CTxdStore_RemoveRef), 10);

    // Small 2-instruction wrapper, 5-byte overwrite
    HookInstall(FUNC_CStreaming__requestTxd, reinterpret_cast<DWORD>(Hook_CStreaming_requestTxd), 5);

    // GetNextFileOnCd TXD loadState hook: overflow slots always report
    // LOADED to prevent SA from issuing RequestModel with invalid IDs.
    HookInstall(HOOKPOS_GetNextFileOnCd_TxdCheck, reinterpret_cast<DWORD>(HOOK_GetNextFileOnCd_TxdCheck), HOOKSIZE_GetNextFileOnCd_TxdCheck);

    // RequestModelStream TXD dependency check when packing streaming requests.
    HookInstall(HOOKPOS_RequestModelStream_TxdCheck, reinterpret_cast<DWORD>(HOOK_RequestModelStream_TxdCheck), HOOKSIZE_RequestModelStream_TxdCheck);
}

std::uint32_t CTxdPoolSA::AllocateTextureDictonarySlot(std::uint32_t uiSlotId, std::string& strTxdName)
{
    CTextureDictonarySAInterface* pTxd = (*m_ppTxdPoolInterface)->AllocateAt(uiSlotId);
    if (!pTxd)
        return -1;

    strTxdName.resize(24);

    pTxd->usUsagesCount = 0;
    pTxd->hash = pGame->GetKeyGen()->GetUppercaseKey(strTxdName.c_str());
    pTxd->rwTexDictonary = nullptr;
    pTxd->usParentIndex = -1;

    return (*m_ppTxdPoolInterface)->GetObjectIndex(pTxd);
}

void CTxdPoolSA::RemoveTextureDictonarySlot(std::uint32_t uiTxdId)
{
    if (!(*m_ppTxdPoolInterface)->IsContains(uiTxdId))
        return;

    // Clear streaming protection so any future re-use of this pool index
    // starts unprotected. Must happen before RemoveTxd in case RemoveTxd's
    // parent cascade triggers Hook_CTxdStore_RemoveRef on related slots.
    g_StreamingProtectedTxdSlots.erase(static_cast<unsigned short>(uiTxdId));

    const DWORD dwStreamingId = uiTxdId + pGame->GetBaseIDforTXD();

    // Standard-range TXD slots tracked by SA's streaming system must be
    // removed via CStreaming::RemoveModel before the pool slot is freed.
    // RemoveModel properly unlinks from the loaded doubly-linked list,
    // sets loadState to NOT_LOADED, and calls RemoveTxd internally.
    // Without this, the stale entry stays in the loaded list and causes
    // a crash when RemoveLeastUsedModel later tries to access the freed slot.
    if (uiTxdId < SA_TXD_POOL_CAPACITY && pGame->GetStreaming())
    {
        CStreamingInfo* pStreamInfo = pGame->GetStreaming()->GetStreamingInfo(dwStreamingId);
        if (pStreamInfo && pStreamInfo->loadState != eModelLoadState::LOADSTATE_NOT_LOADED)
        {
            pGame->GetStreaming()->RemoveModel(dwStreamingId);

            // RemoveTxd (called by RemoveModel) doesn't clear parentIndex.
            // The direct RemoveTxd call below would re-process the parent
            // chain, double-decrementing the parent's usUsagesCount.
            CTextureDictonarySAInterface* pEntry = (*m_ppTxdPoolInterface)->GetObject(uiTxdId);
            if (pEntry)
                pEntry->usParentIndex = static_cast<unsigned short>(-1);
        }
    }

    // Call RemoveTxd for dictionary cleanup. When RemoveModel already ran
    // above, dictionary is NULL and parentIndex is -1, making this a no-op.
    // For overflow slots or non-loaded standard slots, this performs the
    // actual dictionary destruction and parent ref cascade.
    // ShaderSupport hooks 0x731E90 and reads ESI as the streaming ID
    // (pool index + 20000) for texture/shader tracking.
    const DWORD dwFuncAddr = FUNC_CTxdStore__RemoveSlot;
    // clang-format off
    __asm
    {
        mov     esi, dwStreamingId
        push    uiTxdId
        call    dwFuncAddr
        add     esp, 4
    }
    // clang-format on

    (*m_ppTxdPoolInterface)->Release(uiTxdId);
}

bool CTxdPoolSA::IsFreeTextureDictonarySlot(std::uint32_t uiTxdId)
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return false;

    return (*m_ppTxdPoolInterface)->IsEmpty(uiTxdId);
}

std::uint32_t CTxdPoolSA::GetFreeTextureDictonarySlot()
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return static_cast<std::uint32_t>(-1);

    return (*m_ppTxdPoolInterface)->GetFreeSlot();
}

std::uint32_t CTxdPoolSA::GetFreeTextureDictonarySlotInRange(std::uint32_t maxExclusive)
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return static_cast<std::uint32_t>(-1);

    auto* pool = *m_ppTxdPoolInterface;
    if (!pool->m_byteMap || pool->m_nSize <= 0)
        return static_cast<std::uint32_t>(-1);

    const std::uint32_t limit = std::min(maxExclusive, static_cast<std::uint32_t>(pool->m_nSize));
    for (std::uint32_t i = 0; i < limit; ++i)
    {
        if (pool->m_byteMap[i].bEmpty)
            return i;
    }

    return static_cast<std::uint32_t>(-1);
}

std::uint32_t CTxdPoolSA::GetFreeTextureDictonarySlotAbove(std::uint32_t minInclusive)
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return static_cast<std::uint32_t>(-1);

    auto* pool = *m_ppTxdPoolInterface;

    if (!pool->m_byteMap || pool->m_nSize <= static_cast<int>(minInclusive))
        return static_cast<std::uint32_t>(-1);

    for (std::uint32_t i = minInclusive; i < static_cast<std::uint32_t>(pool->m_nSize); ++i)
    {
        if (pool->m_byteMap[i].bEmpty)
            return i;
    }

    return static_cast<std::uint32_t>(-1);
}

int CTxdPoolSA::GetUsedSlotCountInRange(std::uint32_t maxExclusive) const
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return -1;

    auto* pool = *m_ppTxdPoolInterface;
    if (!pool->m_byteMap || pool->m_nSize <= 0)
        return -1;

    const std::uint32_t limit = std::min(maxExclusive, static_cast<std::uint32_t>(pool->m_nSize));
    int                 count = 0;
    for (std::uint32_t i = 0; i < limit; ++i)
    {
        if (!pool->m_byteMap[i].bEmpty)
            ++count;
    }

    return count;
}

int CTxdPoolSA::GetPoolSize() const noexcept
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return 0;

    return (*m_ppTxdPoolInterface)->m_nSize;
}

int CTxdPoolSA::GetUsedSlotCount() const noexcept
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return -1;

    CPoolSAInterface<CTextureDictonarySAInterface>* pPool = *m_ppTxdPoolInterface;
    if (!pPool->m_byteMap || pPool->m_nSize <= 0)
        return -1;

    int iUsedCount = 0;
    for (int i = 0; i < pPool->m_nSize; ++i)
    {
        if (!pPool->m_byteMap[i].bEmpty)
            ++iUsedCount;
    }

    return iUsedCount;
}

CTextureDictonarySAInterface* CTxdPoolSA::GetTextureDictonarySlot(std::uint32_t uiTxdId) noexcept
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return nullptr;

    if (!(*m_ppTxdPoolInterface)->IsContains(uiTxdId))
        return nullptr;

    return (*m_ppTxdPoolInterface)->GetObject(uiTxdId);
}

bool CTxdPoolSA::SetTextureDictonarySlot(std::uint32_t uiTxdId, RwTexDictionary* pTxd, std::uint16_t usParentIndex) noexcept
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
        return false;

    CTextureDictonarySAInterface* pSlot = GetTextureDictonarySlot(uiTxdId);
    if (!pSlot)
        return false;

    pSlot->rwTexDictonary = pTxd;
    pSlot->usParentIndex = usParentIndex;
    return true;
}

// Resizes the TXD pool, preserving existing slot data.
// Safe to call after SA creates the pool: SA stores TXD references
// as 16-bit indices and re-reads pool->m_pObjects on every access,
// so swapping the backing arrays doesn't invalidate anything as
// long as no pool-accessing code is on the call stack.
bool CTxdPoolSA::Resize(int newCapacity)
{
    if (!m_ppTxdPoolInterface || !(*m_ppTxdPoolInterface))
    {
        AddReportLog(9401, "CTxdPoolSA::Resize: Pool pointer is NULL");
        return false;
    }

    auto*     pool = *m_ppTxdPoolInterface;
    const int oldCapacity = pool->m_nSize;

    if (newCapacity <= oldCapacity)
    {
        AddReportLog(9401, SString("CTxdPoolSA::Resize: newCapacity(%d) <= oldCapacity(%d)", newCapacity, oldCapacity));
        return false;
    }

    // SA reads CBaseModelInfo::usTextureDictionary with movsx (signed extend);
    // indices >= 32768 become negative, crashing SA pool accesses.
    constexpr int maxPoolIndex = 32767;
    if (newCapacity > maxPoolIndex + 1)
        newCapacity = maxPoolIndex + 1;
    if (newCapacity <= oldCapacity)
    {
        AddReportLog(9401, SString("CTxdPoolSA::Resize: clamped newCapacity(%d) <= oldCapacity(%d)", newCapacity, oldCapacity));
        return false;
    }

    // Allocate replacement arrays via CMemoryMgr::MallocAlign (hooked to
    // SafeMallocAlign). SA's original CPool arrays were allocated with
    // operator new (SA's static CRT), but we intentionally use MTA's hooked
    // allocator for the replacement arrays. SafeFreeAlign below will detect
    // the old SA-CRT arrays lack the metadata magic and silently skip freeing
    // them (~65 KB one-time leak, negligible for a 32 KB-slot pool resize).
    auto* newObjects = static_cast<CTextureDictonarySAInterface*>(CMemoryMgr::MallocAlign(newCapacity * sizeof(CTextureDictonarySAInterface)));
    if (!newObjects)
    {
        AddReportLog(9401, SString("CTxdPoolSA::Resize: MallocAlign failed for objects (size=%d)", newCapacity * sizeof(CTextureDictonarySAInterface)));
        return false;
    }

    auto* newByteMap = static_cast<tPoolObjectFlags*>(CMemoryMgr::MallocAlign(newCapacity * sizeof(tPoolObjectFlags)));
    if (!newByteMap)
    {
        AddReportLog(9401, SString("CTxdPoolSA::Resize: MallocAlign failed for bytemap (size=%d)", newCapacity * sizeof(tPoolObjectFlags)));
        CMemoryMgr::FreeAlign(newObjects);
        return false;
    }

    // Preserve all existing entries
    std::memcpy(newObjects, pool->m_pObjects, oldCapacity * sizeof(CTextureDictonarySAInterface));
    std::memcpy(newByteMap, pool->m_byteMap, oldCapacity * sizeof(tPoolObjectFlags));

    // Zero-init new slots to avoid stale pointers in empty entries
    std::memset(&newObjects[oldCapacity], 0, static_cast<std::size_t>(newCapacity - oldCapacity) * sizeof(CTextureDictonarySAInterface));

    // Mark new slots as empty
    for (int i = oldCapacity; i < newCapacity; ++i)
    {
        newByteMap[i].bEmpty = true;
        newByteMap[i].nId = 0;
    }

    // Swap arrays (pool is only accessed from the main thread)
    auto* oldObjects = pool->m_pObjects;
    auto* oldByteMap = pool->m_byteMap;

    pool->m_pObjects = newObjects;
    pool->m_byteMap = newByteMap;
    pool->m_nSize = newCapacity;

    // Prevent SA's CPool destructor from calling operator delete on our
    // SafeMallocAlign'd arrays. MTA exits via TerminateProcess() so the
    // destructor normally never runs, but this is a zero-cost safeguard.
    pool->m_bOwnsAllocations = false;

    // Don't override m_nFirstFree - SA's value points within [0, 5000)
    // where its standard TXD allocations belong. Overriding it to
    // oldCapacity-1 would make SA's Allocate()/GetFreeSlot() skip the
    // standard range and hand out slots in the overflow zone (>= 5000)
    // where no streaming entries exist.

    CMemoryMgr::FreeAlign(oldObjects);
    CMemoryMgr::FreeAlign(oldByteMap);

    return true;
}

void CTxdPoolSA::ProtectSlotFromStreaming(unsigned short usSlotId)
{
    g_StreamingProtectedTxdSlots.insert(usSlotId);
}

void CTxdPoolSA::UnprotectSlotFromStreaming(unsigned short usSlotId)
{
    g_StreamingProtectedTxdSlots.erase(usSlotId);
}

bool CTxdPoolSA::IsSlotProtectedFromStreaming(unsigned short usSlotId) const
{
    return g_StreamingProtectedTxdSlots.count(usSlotId) > 0;
}
