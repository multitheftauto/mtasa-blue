/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_HookDestructors.cpp
*  PURPOSE:     Game destructor hooks and entity lifecycle tracking
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <atomic>
#include <mutex>

namespace
{
    // Handler pointers for game destructors
    CAnimBlendAssocDestructorHandler*  m_pCAnimBlendAssocDestructorHandler = nullptr;
    GameObjectDestructHandler*         pGameObjectDestructHandler = nullptr;
    GameVehicleDestructHandler*        pGameVehicleDestructHandler = nullptr;
    GamePlayerDestructHandler*         pGamePlayerDestructHandler = nullptr;
    GameProjectileDestructHandler*     pGameProjectileDestructHandler = nullptr;
    std::atomic<GameModelRemoveHandler*> pGameModelRemoveHandler{nullptr};
    GameRunNamedAnimDestructorHandler* pRunNamedAnimDestructorHandler = nullptr;

    // Reentrancy protection for CStreamingRemoveModel
    static std::atomic<bool> g_bStreamingRemoveModelInProgress{false};
    static std::atomic_flag  g_streamingRemoveModelLock{};

    #define FUNC_CPtrListSingleLink_Remove      0x0533610
    #define FUNC_CPtrListDoubleLink_Remove      0x05336B0
    #define FUNC_CPhysical_RemoveFromMovingList 0x542860

    struct SStreamSectorEntrySingle
    {
        CEntitySAInterface*       pEntity;
        SStreamSectorEntrySingle* pNext;
    };

    struct SStreamSectorEntryDouble
    {
        CEntitySAInterface*       pEntity;
        SStreamSectorEntryDouble* pNext;
        SStreamSectorEntryDouble* pPrev;
    };

    // Info about what sectors an entity actually used
    struct SEntitySAInterfaceExtraInfo
    {
        std::vector<SStreamSectorEntrySingle**> AddedSectorSingleList;
        std::vector<SStreamSectorEntryDouble**> AddedSectorDoubleList;
    };

    CFastHashMap<CEntitySAInterface*, SEntitySAInterfaceExtraInfo> ms_EntitySAInterfaceExtraInfoMap;
    std::mutex ms_EntityMapMutex;  // Protects ms_EntitySAInterfaceExtraInfoMap

    void RemoveEntitySAInterfaceExtraInfo(CEntitySAInterface* pEntitySAInterface)
    {
        const std::lock_guard<std::mutex> lock(ms_EntityMapMutex);
        MapRemove(ms_EntitySAInterfaceExtraInfoMap, pEntitySAInterface);
    }    //
    // CPtrListSingleLink contains item
    //
    [[nodiscard]] bool CPtrListSingleLink_Contains(SStreamSectorEntrySingle* pStreamEntry, CEntitySAInterface* pCheckEntity) noexcept
    {
        for (; pStreamEntry; pStreamEntry = pStreamEntry->pNext)
            if (pStreamEntry->pEntity == pCheckEntity)
                return true;
        return false;
    }

    //
    // CPtrListSingleLink remove item
    //
    void CPtrListSingleLink_Remove(SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pCheckEntity)
    {
        DWORD dwFunc = FUNC_CPtrListSingleLink_Remove;
        __asm
        {
            pushfd                  // Preserve flags (including Direction Flag)
            mov     ecx, ppStreamEntryList
            push    pCheckEntity
            call    dwFunc
            popfd                   // Restore flags
        }
    }

    //
    // CPtrListDoubleLink contains item
    //
    [[nodiscard]] bool CPtrListDoubleLink_Contains(SStreamSectorEntryDouble* pStreamEntry, CEntitySAInterface* pCheckEntity) noexcept
    {
        for (; pStreamEntry; pStreamEntry = pStreamEntry->pNext)
            if (pStreamEntry->pEntity == pCheckEntity)
                return true;
        return false;
    }

    //
    // CPtrListDoubleLink remove item
    //
    void CPtrListDoubleLink_Remove(SStreamSectorEntryDouble** ppStreamEntryList, CEntitySAInterface* pCheckEntity)
    {
        DWORD dwFunc = FUNC_CPtrListDoubleLink_Remove;
        __asm
        {
            pushfd                  // Preserve flags (including Direction Flag)
            mov     ecx, ppStreamEntryList
            push    pCheckEntity
            call    dwFunc
            popfd                   // Restore flags
        }
    }

    //
    // Ensure entity is removed from previously added stream sectors
    // NOTE: Called from naked asm hooks - MUST be noexcept to prevent stack corruption
    //
    void OnCEntityRemoveFromStreamSectorLists(CEntitySAInterface* pEntity) noexcept
    {
        try
        {
            // Copy sector lists while holding lock to prevent dangling pointers
            std::vector<SStreamSectorEntrySingle**> sectorSingleList;
            std::vector<SStreamSectorEntryDouble**> sectorDoubleList;
            
            {
                const std::lock_guard<std::mutex> lock(ms_EntityMapMutex);  // Can throw std::system_error
                auto* const pInfo = MapFind(ms_EntitySAInterfaceExtraInfoMap, pEntity);
                
                if (!pInfo) [[unlikely]]
                    return;
                
                // Remove map entry FIRST, before moving vectors, to prevent leak if move throws.
                // Use std::move for exception safety (move hardly throws).
                sectorSingleList = std::move(pInfo->AddedSectorSingleList);  // Usually noexcept
                sectorDoubleList = std::move(pInfo->AddedSectorDoubleList);  // Usually noexcept
                
                // Now safe to remove - entry has been emptied
                MapRemove(ms_EntitySAInterfaceExtraInfoMap, pEntity);
            }

            // Remove from single link sectors (lock released - safe to call game functions)
            for (auto* pSectorList : sectorSingleList)
            {
                if (CPtrListSingleLink_Contains(*pSectorList, pEntity)) [[likely]]
                    CPtrListSingleLink_Remove(pSectorList, pEntity);
            }

            // Remove from double link sectors
            for (auto* pSectorList : sectorDoubleList)
            {
                if (CPtrListDoubleLink_Contains(*pSectorList, pEntity)) [[likely]]
                    CPtrListDoubleLink_Remove(pSectorList, pEntity);
            }
        }
        catch (...)
        {
            // Called from naked asm hooks - MUST NOT throw or stack is corrupted
            // If mutex.lock() or move fails, entity may remain in sector lists (leak in GTA's tracking)
            // This is acceptable as entity destructor will eventually clean up
        }
    }
}            // namespace

////////////////////////////////////////////////////////////////////////////////////////////////
//
void __cdecl CAnimBlendAssoc_destructor(CAnimBlendAssociationSAInterface* pThis)
{
    if (m_pCAnimBlendAssocDestructorHandler)
    {
        m_pCAnimBlendAssocDestructorHandler(pThis);
    }
}

#define HOOKPOS_CAnimBlendAssoc_destructor       0x4CECF0
static DWORD RETURN_CAnimBlendAssoc_destructor = 0x4CECF6;
static void __declspec(naked) HOOK_CAnimBlendAssoc_destructor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        // Cache this pointer from ECX before pushad
        mov     eax, ecx
        
        // Preserve registers and flags
        pushad
        pushfd
        
        // Call handler (__cdecl convention)
        push    eax
        call    CAnimBlendAssoc_destructor
        add     esp, 4
        
        // Restore registers and flags
        popfd
        popad

        // Replay original prologue (6 bytes overwritten)
        push    esi
        mov     esi, ecx
        mov     eax, [esi + 10h]
        jmp     RETURN_CAnimBlendAssoc_destructor
    }
}

void _cdecl OnCObjectDestructor(DWORD calledFrom, CObjectSAInterface* pObject)
{
    if (!pObject)
        return;

    if (pGameObjectDestructHandler)
        pGameObjectDestructHandler(pObject);
}

// Hook info
#define HOOKPOS_CObjectDestructor        0x59F660
#define HOOKSIZE_CObjectDestructor       7
DWORD RETURN_CObjectDestructor = 0x59F667;
static void __declspec(naked) HOOK_CObjectDestructor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        // Cache parameters before pushad
        mov     eax, ecx
        mov     edx, [esp]
        
        // Preserve registers and flags
        pushad
        pushfd
        
        // Call handler (__cdecl convention)
        push    eax
        push    edx
        call    OnCObjectDestructor
        add     esp, 4*2
        
        // Restore registers and flags
        popfd
        popad

        // Replay original prologue (7 bytes overwritten)
        // SEH frame setup
        push    0FFFFFFFFh
        push    0x83D228
        jmp     RETURN_CObjectDestructor
    }
}

void _cdecl OnVehicleDestructor(DWORD calledFrom, CVehicleSAInterface* pVehicle)
{
    if (!pVehicle)
        return;

    if (pGameVehicleDestructHandler)
        pGameVehicleDestructHandler(pVehicle);
}

// Hook info
#define HOOKPOS_CVehicleDestructor           0x6E2B40
#define HOOKSIZE_CVehicleDestructor          7
DWORD RETURN_CVehicleDestructor = 0x6E2B47;  // Avoid SA's anti-disasm obfuscation at 0x401355 (which had been the return address for years), jump directly to real destructor body
static void __declspec(naked) HOOK_CVehicleDestructor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        mov     eax, ecx
        mov     edx, [esp]
        
        pushad
        pushfd
        
        push    eax
        push    edx
        call    OnVehicleDestructor
        add     esp, 4*2
        
        popfd
        popad

        // Original code at 0x6E2B40: push 0xFFFFFFFF; jmp 0x401355 (obfuscation chunk)
        // The obfuscation at 0x401355 pushes TWO values before jumping to 0x6E2B47:
        //   1. A garbage value (result of mov+add+xchg)
        //   2. The original 0xFFFFFFFF
        // We must replicate this stack layout to maintain compatibility
        push    0               // Garbage value placeholder (obfusscation result doesn't matter)
        push    0FFFFFFFFh      // SEH marker
        jmp     RETURN_CVehicleDestructor
    }
}

void _cdecl OnCPlayerPedDestructor(DWORD calledFrom, CPedSAInterface* pPlayerPed)
{
    if (!pPlayerPed)
        return;

    if (pGamePlayerDestructHandler)
        pGamePlayerDestructHandler(pPlayerPed);
}

// Hook info
#define HOOKPOS_CPlayerPedDestructor        0x6093B0
#define HOOKSIZE_CPlayerPedDestructor       7
DWORD RETURN_CPlayerPedDestructor = 0x6093B7;
static void __declspec(naked) HOOK_CPlayerPedDestructor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        mov     eax, ecx
        mov     edx, [esp]
        
        pushad
        pushfd
        
        push    eax
        push    edx
        call    OnCPlayerPedDestructor
        add     esp, 4*2
        
        popfd
        popad

        push    0FFFFFFFFh
        push    0x83EC18
        jmp     RETURN_CPlayerPedDestructor
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCProjectileDestructor(DWORD calledFrom, CEntitySAInterface* pProjectile)
{
    if (!pProjectile)
        return;

    if (pGameProjectileDestructHandler)
        pGameProjectileDestructHandler(pProjectile);
}

// Hook info
#define HOOKPOS_CProjectileDestructor        0x5A40E0
#define HOOKSIZE_CProjectileDestructor       6
DWORD RETURN_CProjectileDestructor = 0x5A40E6;
static void __declspec(naked) HOOK_CProjectileDestructor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        mov     eax, ecx
        mov     edx, [esp]
        
        pushad
        pushfd
        
        push    eax
        push    edx
        call    OnCProjectileDestructor
        add     esp, 4*2
        
        popfd
        popad

        mov     dword ptr [ecx], 867030h
        jmp     RETURN_CProjectileDestructor
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCPhysicalDestructor(DWORD calledFrom, CPhysicalSAInterface* pEntity) noexcept
{
    if (!pEntity)
        return;

    if (pEntity->m_pMovingList)
    {
        // Log the removal (SString can throw, so wrap in try/catch)
        try
        {
            AddReportLog(8640, SString("Removing CPhysical type %d from moving list", pEntity->nType));
        }
        catch (...)
        {
            // Called from naked asm hook - MUST NOT throw or stack is corrupted
            // If SString alloc fails, skip logging but continue cleanup
        }
        
        // Always perform the actual removal, even if logging failed
        DWORD dwFunc = FUNC_CPhysical_RemoveFromMovingList;
        __asm
        {
            mov     ecx, pEntity
            call    dwFunc
        }
    }
}

// Hook info
#define HOOKPOS_CPhysicalDestructor        0x542450
#define HOOKSIZE_CPhysicalDestructor       7
DWORD RETURN_CPhysicalDestructor = 0x542457;
static void __declspec(naked) HOOK_CPhysicalDestructor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        mov     eax, ecx
        mov     edx, [esp]
        
        pushad
        pushfd
        
        push    eax
        push    edx
        call    OnCPhysicalDestructor
        add     esp, 4*2
        
        popfd
        popad

        push    0FFFFFFFFh
        push    0x83C996
        jmp     RETURN_CPhysicalDestructor
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCEntityDestructor(DWORD calledFrom, CEntitySAInterface* pEntity)
{
    if (!pEntity)
        return;

    OnCEntityRemoveFromStreamSectorLists(pEntity);
}

// Hook info
#define HOOKPOS_CEntityDestructor        0x535E97
#define HOOKSIZE_CEntityDestructor       6
DWORD RETURN_CEntityDestructor = 0x535E9D;
static void __declspec(naked) HOOK_CEntityDestructor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        mov     eax, ecx
        mov     edx, [esp]
        
        pushad
        pushfd
        
        push    eax
        push    edx
        call    OnCEntityDestructor
        add     esp, 4*2
        
        popfd
        popad

        mov     eax, dword ptr fs:[00000000h]
        jmp     RETURN_CEntityDestructor
    }
}

void cdecl OnCEntityAddMid1(SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface) noexcept
{
    if (!ppStreamEntryList || !pEntitySAInterface) [[unlikely]]
        return;

    try
    {
        const std::lock_guard<std::mutex> lock(ms_EntityMapMutex);
        
        // Find or create entry - use find() first to prevent empty entry leak if push_back throws
        auto it = ms_EntitySAInterfaceExtraInfoMap.find(pEntitySAInterface);
        if (it != ms_EntitySAInterfaceExtraInfoMap.end())
        {
            // Entry exists - directly push_back (may throw, but no leak)
            it->second.AddedSectorSingleList.push_back(ppStreamEntryList);
        }
        else
        {
            // Entry doesn't exist - create with initial value to avoid empty entry leak
            SEntitySAInterfaceExtraInfo info;
            info.AddedSectorSingleList.push_back(ppStreamEntryList);  // May throw - info is local, no leak
            ms_EntitySAInterfaceExtraInfoMap[pEntitySAInterface] = std::move(info);  // Move assignment (noexcept)
        }
    }
    catch (...)
    {
        // Called from naked asm hook - MUST NOT throw or stack is corrupted
        // Silently ignore allocation failures (no memory leak - exception-safe)
    }
}

// Hook info
#define HOOKPOS_CEntityAddMid1        0x5348FB
#define HOOKSIZE_CEntityAddMid1       5
#define HOOKCHECK_CEntityAddMid1      0xE8
DWORD RETURN_CEntityAddMid1 = 0x534900;
static void __declspec(naked) HOOK_CEntityAddMid1()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        // At hook entry: ECX = list pointer (thiscall this), [esp] = entity pointer (pushed arg)
        mov     eax, [esp]          // Entity pointer from stack arg
        mov     edx, ecx            // List pointer from ECX
        
        pushad
        pushfd
        
        // cdecl: push args right-to-left, so arg2 first, arg1 second
        // Handler: OnCEntityAddMid1(list, entity)
        push    eax                 // arg2: entity
        push    edx                 // arg1: list
        call    OnCEntityAddMid1
        add     esp, 4*2
        
        popfd
        popad

        mov     eax, 0x5335E0
        call    eax
        jmp     RETURN_CEntityAddMid1
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
void cdecl OnCEntityAddMid2(SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface) noexcept
{
    if (!ppStreamEntryList || !pEntitySAInterface) [[unlikely]]
        return;

    try
    {
        const std::lock_guard<std::mutex> lock(ms_EntityMapMutex);
        
        // Find or create entry - use find() first to prevent empty entry leak if push_back throws
        auto it = ms_EntitySAInterfaceExtraInfoMap.find(pEntitySAInterface);
        if (it != ms_EntitySAInterfaceExtraInfoMap.end())
        {
            // Entry exists - directly push_back (may throw, but no leak)
            it->second.AddedSectorSingleList.push_back(ppStreamEntryList);
        }
        else
        {
            // Entry doesn't exist - create with initial value to avoid empty entry leak
            SEntitySAInterfaceExtraInfo info;
            info.AddedSectorSingleList.push_back(ppStreamEntryList);  // May throw - info is local, no leak
            ms_EntitySAInterfaceExtraInfoMap[pEntitySAInterface] = std::move(info);  // Move assignment (noexcept)
        }
    }
    catch (...)
    {
        // Called from naked asm hook - MUST NOT throw or stack is corrupted
        // Silently ignore allocation failures (no memory leak - exception-safe)
    }
}

// Hook info
#define HOOKPOS_CEntityAddMid2        0x534A10
#define HOOKSIZE_CEntityAddMid2       5
#define HOOKCHECK_CEntityAddMid2      0xE8
DWORD RETURN_CEntityAddMid2 = 0x534A15;
static void __declspec(naked) HOOK_CEntityAddMid2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        // At hook entry: ECX = list pointer (thiscall this), [esp] = entity pointer (pushed arg)
        mov     eax, [esp]          // Entity pointer from stack arg
        mov     edx, ecx            // List pointer from ECX
        
        pushad
        pushfd
        
        // cdecl: push args right-to-left, so arg2 first, arg1 second
        // Handler: OnCEntityAddMid2(list, entity)
        push    eax                 // arg2: entity
        push    edx                 // arg1: list
        call    OnCEntityAddMid2
        add     esp, 4*2
        
        popfd
        popad

        mov     eax, 0x5335E0
        call    eax
        jmp     RETURN_CEntityAddMid2
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
void cdecl OnCEntityAddMid3(SStreamSectorEntryDouble** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface) noexcept
{
    if (!ppStreamEntryList || !pEntitySAInterface) [[unlikely]] 
        return;

    try
    {
        const std::lock_guard<std::mutex> lock(ms_EntityMapMutex);
        
        // Find or create entry - use find() first to prevent empty entry leak if push_back throws
        auto it = ms_EntitySAInterfaceExtraInfoMap.find(pEntitySAInterface);
        if (it != ms_EntitySAInterfaceExtraInfoMap.end())
        {
            // Entry exists - directly push_back (may throw, but no leak)
            it->second.AddedSectorDoubleList.push_back(ppStreamEntryList);
        }
        else
        {
            // Entry doesn't exist - create with initial value to avoid empty entry leak
            SEntitySAInterfaceExtraInfo info;
            info.AddedSectorDoubleList.push_back(ppStreamEntryList);  // May throw - info is local, no leak
            ms_EntitySAInterfaceExtraInfoMap[pEntitySAInterface] = std::move(info);  // Move assignment (noexcept)
        }
    }
    catch (...)
    {
        // Called from naked asm hook - MUST NOT throw or stack is corrupted
        // Silently ignore allocation failures (no memory leak - exception-safe)
    }
}

// Hook info
#define HOOKPOS_CEntityAddMid3        0x534AA2
#define HOOKSIZE_CEntityAddMid3       5
#define HOOKCHECK_CEntityAddMid3      0xE8
DWORD RETURN_CEntityAddMid3 = 0x534AA7;
static void __declspec(naked) HOOK_CEntityAddMid3()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        // At hook entry: ECX = list pointer (thiscall this), [esp] = entity pointer (pushed arg)
        mov     eax, [esp]          // Entity pointer from stack arg
        mov     edx, ecx            // List pointer from ECX
        
        pushad
        pushfd
        
        // cdecl: push args right-to-left, so arg2 first, arg1 second
        // Handler: OnCEntityAddMid3(list, entity)
        push    eax                 // arg2: entity
        push    edx                 // arg1: list
        call    OnCEntityAddMid3
        add     esp, 4*2
        
        popfd
        popad

        mov     eax, 0x533670
        call    eax
        jmp     RETURN_CEntityAddMid3
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
void cdecl OnCEntityRemovePost(CEntitySAInterface* pEntity)
{
    if (!pEntity)
        return;

    OnCEntityRemoveFromStreamSectorLists(pEntity);
}

// Hook info
#define HOOKPOS_CEntityRemove        0x534AE0
#define HOOKSIZE_CEntityRemove       5
#define HOOKCHECK_CEntityRemove      0x83
DWORD RETURN_CEntityRemove = 0x534AE5;
static void __declspec(naked) HOOK_CEntityRemove()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        // Save original ESI (callee-saved)
        push    esi
        
        // Get entity pointer from ECX (it's __thiscall!)
        mov     esi, ecx            // Entity is in ECX, not on stack!
        
        // Inner function expects entity in ECX (not on stack!)
        mov     ecx, esi            // Ensure ECX has entity
        call    inner

        // Call post-removal handler
        pushad
        pushfd
        push    esi                 // Push cached entity
        call    OnCEntityRemovePost
        add     esp, 4*1
        popfd
        popad
        
        // Restore original ESI
        pop     esi
        ret                         

inner:
        // Original code (expects entity in ECX)
        sub     esp, 30h
        push    ebx
        push    ebp
        jmp     RETURN_CEntityRemove
    }
}


static void CallGameModelRemoveHandlerSafe(GameModelRemoveHandler* handler, ushort modelId)
{
    __try
    {
        handler(modelId);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }
}

void _cdecl OnCStreamingRemoveModel(DWORD calledFrom, int modelId) noexcept
{
    if (g_bStreamingRemoveModelInProgress.exchange(true, std::memory_order_acquire))
        return;

    bool bLockAcquired = false;
    bool bTimeout = false;
    int attemptCount = 0;
    
    // Acquire spinlock with timeout
    constexpr int MAX_SPIN_ATTEMPTS = 50000;  // ~50ms on most CPU's
    int spinCount = 0;
    
    while (g_streamingRemoveModelLock.test_and_set(std::memory_order_acquire))
    {
        if (++spinCount > MAX_SPIN_ATTEMPTS)
        {
            bTimeout = true;
            attemptCount = spinCount;
            break;
        }
        SwitchToThread();
    }
    
    if (!bTimeout)
    {
        bLockAcquired = true;
        
        // Call handler with exception protection
        auto* const handler = pGameModelRemoveHandler.load(std::memory_order_acquire);
        if (handler) [[likely]]
        {
            CallGameModelRemoveHandlerSafe(handler, static_cast<ushort>(modelId));
        }
        
        // Release spinlock
        g_streamingRemoveModelLock.clear(std::memory_order_release);
    }
    
    // Always release reentrancy flag
    g_bStreamingRemoveModelInProgress.store(false, std::memory_order_release);
    
    // Log timeout (SString can throw, so wrap in try/catch)
    if (bTimeout)
    {
        try
        {
            AddReportLog(8641, SString("Failed to acquire CStreamingRemoveModel lock after %d attempts (modelId=%d)", 
                                       attemptCount, modelId));
        }
        catch (...)
        {
            // Called from naked asm hook - MUST NOT throw or stack is corrupted
            // If SString allocation fails, skip logging (reentrancy flag already released above)
        }
    }
}

// Hook info
#define HOOKPOS_CStreamingRemoveModel        0x4089A0
#define HOOKSIZE_CStreamingRemoveModel       6
DWORD RETURN_CStreamingRemoveModel = 0x4089A6;
static void __declspec(naked) HOOK_CStreamingRemoveModel()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        mov     eax, [esp+4]
        mov     edx, [esp]
        
        pushad
        pushfd

        push    eax
        push    edx
        call    OnCStreamingRemoveModel
        add     esp, 4*2
        
        popfd
        popad

        push    esi
        mov     esi, [esp+8]
        push    edi
        jmp     RETURN_CStreamingRemoveModel
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCTaskSimpleRunNamedAnimDestructor(class CTaskSimpleRunNamedAnimSAInterface* pTask)
{
    if (!pTask)
        return;

    if (pRunNamedAnimDestructorHandler)
        pRunNamedAnimDestructorHandler(pTask);
}

// Hook info
#define HOOKPOS_CTaskSimpleRunNamedAnimDestructor        0x61BEF0
#define HOOKSIZE_CTaskSimpleRunNamedAnimDestructor       8
DWORD RETURN_CTaskSimpleRunNamedAnim = 0x61BEF8;
static void __declspec(naked) HOOK_CTaskSimpleRunNamedAnimDestructor()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
    {
        mov     eax, ecx
        
        pushad
        pushfd
        
        push    eax
        call    OnCTaskSimpleRunNamedAnimDestructor
        add     esp, 4
        
        popfd
        popad

        push    esi
        mov     esi, ecx

        mov     eax, 0x61BF10
        call    eax
        jmp     RETURN_CTaskSimpleRunNamedAnim
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Handler setters
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetCAnimBlendAssocDestructorHandler(CAnimBlendAssocDestructorHandler* pHandler)
{
    m_pCAnimBlendAssocDestructorHandler = pHandler;
}

void CMultiplayerSA::SetGameObjectDestructHandler(GameObjectDestructHandler* pHandler)
{
    pGameObjectDestructHandler = pHandler;
}

void CMultiplayerSA::SetGameVehicleDestructHandler(GameVehicleDestructHandler* pHandler)
{
    pGameVehicleDestructHandler = pHandler;
}

void CMultiplayerSA::SetGamePlayerDestructHandler(GamePlayerDestructHandler* pHandler)
{
    pGamePlayerDestructHandler = pHandler;
}

void CMultiplayerSA::SetGameProjectileDestructHandler(GameProjectileDestructHandler* pHandler)
{
    pGameProjectileDestructHandler = pHandler;
}

void CMultiplayerSA::SetGameModelRemoveHandler(GameModelRemoveHandler* pHandler)
{
    pGameModelRemoveHandler.store(pHandler, std::memory_order_release);
}

void CMultiplayerSA::SetGameRunNamedAnimDestructorHandler(GameRunNamedAnimDestructorHandler* pHandler)
{
    pRunNamedAnimDestructorHandler = pHandler;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_HookDestructors()
{
    HookInstall(HOOKPOS_CAnimBlendAssoc_destructor, (DWORD)HOOK_CAnimBlendAssoc_destructor, 6);
    EZHookInstall(CTaskSimpleRunNamedAnimDestructor);
    EZHookInstall(CObjectDestructor);
    EZHookInstall(CVehicleDestructor);
    EZHookInstall(CProjectileDestructor);
    EZHookInstall(CPlayerPedDestructor);
    EZHookInstall(CPhysicalDestructor);
    EZHookInstall(CEntityDestructor);
    EZHookInstall(CStreamingRemoveModel);
    EZHookInstall(CEntityAddMid1);
    EZHookInstall(CEntityAddMid2);
    EZHookInstall(CEntityAddMid3);
    EZHookInstall(CEntityRemove);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Cleanup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::CleanupHooks_HookDestructors()
{
    m_pCAnimBlendAssocDestructorHandler = nullptr;
    pGameObjectDestructHandler = nullptr;
    pGameVehicleDestructHandler = nullptr;
    pGamePlayerDestructHandler = nullptr;
    pGameProjectileDestructHandler = nullptr;
    pGameModelRemoveHandler.store(nullptr, std::memory_order_release);
    pRunNamedAnimDestructorHandler = nullptr;

    {
        const std::lock_guard<std::mutex> lock(ms_EntityMapMutex);
        ms_EntitySAInterfaceExtraInfoMap.clear();
    }
}
