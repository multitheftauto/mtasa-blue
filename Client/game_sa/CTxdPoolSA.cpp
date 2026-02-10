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
// entries instead. Check the TXD pool bytemap directly for overflow slots.
#define HOOKPOS_GetNextFileOnCd_TxdCheck  0x408ECE
#define HOOKSIZE_GetNextFileOnCd_TxdCheck 8
static const DWORD RETURN_GetNextFileOnCd_TxdCheck = 0x408ED6;

// RequestModelStream TXD dependency check hook:
// At 0x40CD9F, SA reads the TXD streaming entry loadState before packing a
// DFF into a streaming request:
//   mov al, byte ptr [ecx*4+0x8E4CD0]   ; 7 bytes: 8A 04 8D D0 4C 8E 00
// Same overflow problem as GetNextFileOnCd. For overflow TXD slots, we check
// the pool bytemap directly.
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
// For standard TXDs (txdIndex < 5000): executes the original loadState read.
// For overflow TXDs (>= 5000): checks the TXD pool bytemap directly.
// If the pool slot is occupied, MTA has loaded the TXD data into memory,
// so we report LOADED (edx=1).  This avoids modifying the shared COL/IPL
// streaming entry, which would break collision loading.
static void __declspec(naked) HOOK_GetNextFileOnCd_TxdCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        // eax = txdIndex, edx = txdIndex*5
        test    eax, eax
        js      overflow_not_loaded     // Negative txdIndex (movsx of >= 32768), treat as not loaded
        cmp     eax, 5000               // SA_TXD_POOL_CAPACITY
        jge     overflow_check

        // Standard path: execute original instruction
        movzx   edx, byte ptr [edx*4+0x946750]
        jmp     RETURN_GetNextFileOnCd_TxdCheck

    overflow_check:
        // Overflow TXD slot: check if pool slot is occupied
        push    ecx
        push    eax
        mov     ecx, dword ptr [0xC8800C]  // VAR_CTxdStore_ms_pTxdPool -> pool*
        test    ecx, ecx
        jz      overflow_not_loaded

        // Check pool size
        cmp     eax, [ecx+8]              // pool->m_nSize (offset 8 in CPoolSAInterface)
        jge     overflow_not_loaded

        // Check bytemap: pool->m_byteMap[txdIndex].bEmpty  (bit 7 of byte)
        mov     ecx, [ecx+4]              // pool->m_byteMap (offset 4)
        test    ecx, ecx
        jz      overflow_not_loaded

        movzx   edx, byte ptr [ecx+eax]   // m_byteMap[txdIndex]
        test    dl, 0x80                   // bEmpty is the sign bit (bit 7)
        jnz     overflow_not_loaded        // bEmpty=1 means slot is free

        // Slot occupied = TXD loaded by MTA
        pop     eax
        pop     ecx
        mov     edx, 1                    // LOADSTATE_LOADED
        jmp     RETURN_GetNextFileOnCd_TxdCheck

    overflow_not_loaded:
        pop     eax
        pop     ecx
        // TXD not loaded by MTA: report NOT_LOADED so SA defers the DFF.
        // Do NOT fall back to the real streaming entry - it belongs to
        // COL/IPL and would give a semantically wrong (and possibly
        // corrupted, since edx may no longer hold txdIndex*5) result.
        xor     edx, edx                  // LOADSTATE_NOT_LOADED = 0
        jmp     RETURN_GetNextFileOnCd_TxdCheck
    }
    // clang-format on
}

// Hook for RequestModelStream's TXD dependency check (0x40CD9F).
// Replaces:  mov al, byte ptr [ecx*4+0x8E4CD0]  (7 bytes)
// At entry: eax = txdIndex+20000 (from movsx + add), ecx = (txdIndex+20000)*5
// At exit:  al = loadState byte, execution continues at 0x40CDA6 (cmp al, 1)
// Only AL is live after return; ecx, edx, and eax upper bytes are dead.
static void __declspec(naked) HOOK_RequestModelStream_TxdCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        // eax = txdIndex+20000, ecx = (txdIndex+20000)*5
        cmp     eax, 25000                 // 20000 + SA_TXD_POOL_CAPACITY
        jge     overflow_check

        // Standard path: execute original instruction
        mov     al, byte ptr [ecx*4+0x8E4CD0]
        jmp     RETURN_RequestModelStream_TxdCheck

    overflow_check:
        // Convert streaming ID back to raw pool index
        sub     eax, 0x4E20                // eax = txdIndex (raw pool index)
        test    eax, eax
        js      overflow_not_loaded        // Negative index, not loaded

        // Check pool bytemap
        mov     ecx, dword ptr [0xC8800C]  // VAR_CTxdStore_ms_pTxdPool -> pool*
        test    ecx, ecx
        jz      overflow_not_loaded

        cmp     eax, [ecx+8]              // pool->m_nSize
        jge     overflow_not_loaded

        mov     ecx, [ecx+4]              // pool->m_byteMap
        test    ecx, ecx
        jz      overflow_not_loaded

        test    byte ptr [ecx+eax], 0x80  // bEmpty? (bit 7)
        jnz     overflow_not_loaded

        // Slot occupied = TXD loaded by MTA
        mov     al, 1                      // LOADSTATE_LOADED
        jmp     RETURN_RequestModelStream_TxdCheck

    overflow_not_loaded:
        xor     eax, eax                   // al = LOADSTATE_NOT_LOADED = 0
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
// COL/IPL/DAT/IFP territory, so skip - GetNextFileOnCd handles the DFF
// dependency check via the TXD pool bytemap.
static void __cdecl Hook_CStreaming_requestTxd(int txdIndex, int flags)
{
    if (txdIndex >= 0 && txdIndex < CTxdPoolSA::SA_TXD_POOL_CAPACITY)
    {
        using RequestModel_t = void(__cdecl*)(int, int);
        reinterpret_cast<RequestModel_t>(FUNC_CStreaming__RequestModel)(txdIndex + pGame->GetBaseIDforTXD(), flags);
        return;
    }

    // Overflow slots (>= 5000): skip. GetNextFileOnCd and RequestModelStream
    // hooks handle the DFF dependency check via the TXD pool bytemap.
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
    {
        AddReportLog(9401, "CTxdPoolSA::InitialisePool: Pool pointer is NULL");
        return;
    }

    const int currentSize = (*m_ppTxdPoolInterface)->m_nSize;
    if (currentSize >= TXD_POOL_MAX_CAPACITY)
    {
        AddReportLog(9402, SString("CTxdPoolSA::InitialisePool: Already at capacity (%d)", currentSize));
        return;
    }

    AddReportLog(9402, SString("CTxdPoolSA::InitialisePool: Expanding from %d to %d", currentSize, TXD_POOL_MAX_CAPACITY));
    if (!Resize(TXD_POOL_MAX_CAPACITY))
    {
        AddReportLog(9401, SString("CTxdPoolSA::InitialisePool: Resize failed (currentSize=%d targetSize=%d)", currentSize, TXD_POOL_MAX_CAPACITY));
    }
    else
    {
        AddReportLog(9402, SString("CTxdPoolSA::InitialisePool: Success, new size=%d", (*m_ppTxdPoolInterface)->m_nSize));
    }
}

void CTxdPoolSA::InstallPoolHooks()
{
    // First two instructions = 4 + 6 = 10 bytes
    HookInstall(FUNC_CTxdStore__RemoveRef, reinterpret_cast<DWORD>(Hook_CTxdStore_RemoveRef), 10);

    // Small 2-instruction wrapper, 5-byte overwrite
    HookInstall(FUNC_CStreaming__requestTxd, reinterpret_cast<DWORD>(Hook_CStreaming_requestTxd), 5);

    // GetNextFileOnCd TXD loadState hook: overflow slots check pool bytemap
    // instead of reading the COL/IPL streaming entry.
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

    // ShaderSupport hooks 0x731E90 and reads ESI as the streaming ID
    // (pool index + 20000) for texture/shader tracking. A plain cdecl
    // call leaves ESI undefined, so set it to match what the hook expects.
    const DWORD dwStreamingId = uiTxdId + pGame->GetBaseIDforTXD();
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
