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


namespace
{
    // Handler pointers for game destructors
    CAnimBlendAssocDestructorHandler*  m_pCAnimBlendAssocDestructorHandler = nullptr;
    GameObjectDestructHandler*         pGameObjectDestructHandler = nullptr;
    GameVehicleDestructHandler*        pGameVehicleDestructHandler = nullptr;
    GamePlayerDestructHandler*         pGamePlayerDestructHandler = nullptr;
    GameProjectileDestructHandler*     pGameProjectileDestructHandler = nullptr;
    GameModelRemoveHandler*            pGameModelRemoveHandler = nullptr;
    GameRunNamedAnimDestructorHandler* pRunNamedAnimDestructorHandler = nullptr;

    // Reentrancy protection for CStreamingRemoveModel
    static volatile LONG g_lStreamingRemoveModelInProgress = 0;
    static CRITICAL_SECTION g_csStreamingRemoveModel;
    static volatile LONG g_lStreamingRemoveModelCriticalSectionInitialized = 0;

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

    SEntitySAInterfaceExtraInfo& GetEntitySAInterfaceExtraInfo(CEntitySAInterface* pEntitySAInterface)
    {
        if (!MapContains(ms_EntitySAInterfaceExtraInfoMap, pEntitySAInterface))
            ms_EntitySAInterfaceExtraInfoMap[pEntitySAInterface] = SEntitySAInterfaceExtraInfo();

        return MapGet(ms_EntitySAInterfaceExtraInfoMap, pEntitySAInterface);
    }

    void RemoveEntitySAInterfaceExtraInfo(CEntitySAInterface* pEntitySAInterface) 
    { 
        MapRemove(ms_EntitySAInterfaceExtraInfoMap, pEntitySAInterface); 
    }

    //
    // CPtrListSingleLink contains item
    //
    bool CPtrListSingleLink_Contains(SStreamSectorEntrySingle* pStreamEntry, CEntitySAInterface* pCheckEntity)
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
            pushfd
            mov     ecx, ppStreamEntryList
            push    pCheckEntity
            call    dwFunc
            popfd
        }
    }

    //
    // CPtrListDoubleLink contains item
    //
    bool CPtrListDoubleLink_Contains(SStreamSectorEntryDouble* pStreamEntry, CEntitySAInterface* pCheckEntity)
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
            pushfd
            mov     ecx, ppStreamEntryList
            push    pCheckEntity
            call    dwFunc
            popfd
        }
    }

    //
    // Ensure entity is removed from previously added stream sectors
    //
    void RemoveEntityFromStreamSectors(CEntitySAInterface* pEntity)
    {
        SEntitySAInterfaceExtraInfo* pInfo = MapFind(ms_EntitySAInterfaceExtraInfoMap, pEntity);
        if (!pInfo)
            return;

        // Remove from single link sectors
        for (uint i = 0; i < pInfo->AddedSectorSingleList.size(); i++)
        {
            if (CPtrListSingleLink_Contains(*pInfo->AddedSectorSingleList[i], pEntity))
                CPtrListSingleLink_Remove(pInfo->AddedSectorSingleList[i], pEntity);
        }
        pInfo->AddedSectorSingleList.clear();

        // Remove from double link sectors
        for (uint i = 0; i < pInfo->AddedSectorDoubleList.size(); i++)
        {
            if (CPtrListDoubleLink_Contains(*pInfo->AddedSectorDoubleList[i], pEntity))
                CPtrListDoubleLink_Remove(pInfo->AddedSectorDoubleList[i], pEntity);
        }
        pInfo->AddedSectorDoubleList.clear();

        // Remove map entry to prevent memory leak
        // Entry will be auto-created if entity is re-added to sectors
        RemoveEntitySAInterfaceExtraInfo(pEntity);
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
DWORD RETURN_CVehicleDestructor = 0x401355;
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

        push    0FFFFFFFFh
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
void _cdecl OnCPhysicalDestructor(DWORD calledFrom, CPhysicalSAInterface* pEntity)
{
    if (!pEntity)
        return;

    if (pEntity->m_pMovingList)
    {
        AddReportLog(8640, SString("Removing CPhysical type %d from moving list", pEntity->nType));
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

    RemoveEntityFromStreamSectors(pEntity);
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

void cdecl OnCEntityAddMid1(SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface)
{
    if (!ppStreamEntryList || !pEntitySAInterface)
        return;

    SEntitySAInterfaceExtraInfo& info = GetEntitySAInterfaceExtraInfo(pEntitySAInterface);
    info.AddedSectorSingleList.push_back(ppStreamEntryList);
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
        mov     eax, [esp+4]
        mov     edx, ecx
        
        pushad
        pushfd
        
        push    edx
        push    eax
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
void cdecl OnCEntityAddMid2(SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface)
{
    if (!ppStreamEntryList || !pEntitySAInterface)
        return;

    SEntitySAInterfaceExtraInfo& info = GetEntitySAInterfaceExtraInfo(pEntitySAInterface);
    info.AddedSectorSingleList.push_back(ppStreamEntryList);
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
        mov     eax, [esp+4]
        mov     edx, ecx
        
        pushad
        pushfd
        
        push    edx
        push    eax
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
void cdecl OnCEntityAddMid3(SStreamSectorEntryDouble** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface)
{
    if (!ppStreamEntryList || !pEntitySAInterface)
        return;

    SEntitySAInterfaceExtraInfo& info = GetEntitySAInterfaceExtraInfo(pEntitySAInterface);
    info.AddedSectorDoubleList.push_back(ppStreamEntryList);
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
        mov     eax, [esp+4]
        mov     edx, ecx
        
        pushad
        pushfd
        
        push    edx
        push    eax
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

    RemoveEntityFromStreamSectors(pEntity);
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
        push    esi
        mov     esi, ecx
        mov     ecx, esi
        call    inner

        pushad
        pushfd
        push    esi
        call    OnCEntityRemovePost
        add     esp, 4*1
        popfd
        popad
        
        pop     esi
        ret

inner:
        sub     esp, 30h
        push    ebx
        push    ebp
        jmp     RETURN_CEntityRemove
    }
}

void _cdecl OnCStreamingRemoveModel(DWORD calledFrom, int modelId)
{
    if (InterlockedCompareExchange(&g_lStreamingRemoveModelInProgress, 1, 0) != 0)
        return;

    __try
    {
        if (g_lStreamingRemoveModelCriticalSectionInitialized)
            EnterCriticalSection(&g_csStreamingRemoveModel);

        if (pGameModelRemoveHandler)
        {
            __try
            {
                pGameModelRemoveHandler(static_cast<ushort>(modelId));
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }
    }
    __finally
    {
        if (g_lStreamingRemoveModelCriticalSectionInitialized)
            LeaveCriticalSection(&g_csStreamingRemoveModel);
        
        InterlockedExchange(&g_lStreamingRemoveModelInProgress, 0);
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
    pGameModelRemoveHandler = pHandler;
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
    if (InterlockedCompareExchange(&g_lStreamingRemoveModelCriticalSectionInitialized, 1, 0) == 0)
    {
        InitializeCriticalSection(&g_csStreamingRemoveModel);
    }

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
    pGameModelRemoveHandler = nullptr;
    pRunNamedAnimDestructorHandler = nullptr;

    if (InterlockedCompareExchange(&g_lStreamingRemoveModelCriticalSectionInitialized, 0, 1) == 1)
    {
        DeleteCriticalSection(&g_csStreamingRemoveModel);
    }

    ms_EntitySAInterfaceExtraInfoMap.clear();
}
