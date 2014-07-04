/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_HookDestructors.cpp
*  PORPOISE:    
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    GameObjectDestructHandler*      pGameObjectDestructHandler      = NULL;
    GameVehicleDestructHandler*     pGameVehicleDestructHandler     = NULL;
    GamePlayerDestructHandler*      pGamePlayerDestructHandler      = NULL;
    GameProjectileDestructHandler*  pGameProjectileDestructHandler  = NULL;
    GameModelRemoveHandler*         pGameModelRemoveHandler         = NULL;

    #define FUNC_CPtrListSingleLink_Remove  0x0533610
    #define FUNC_CPtrListDoubleLink_Remove  0x05336B0

    struct SStreamSectorEntrySingle
    {
        CEntitySAInterface*         pEntity;
        SStreamSectorEntrySingle*   pNext;
    };

    struct SStreamSectorEntryDouble
    {
        CEntitySAInterface*         pEntity;
        SStreamSectorEntryDouble*   pNext;
        SStreamSectorEntryDouble*   pPrev;
    };


    // Info about what sectors an entity actually used
    struct SEntitySAInterfaceExtraInfo
    {
        std::vector < SStreamSectorEntrySingle** > AddedSectorSingleList;
        std::vector < SStreamSectorEntryDouble** > AddedSectorDoubleList;
    };

    CFastHashMap < CEntitySAInterface*, SEntitySAInterfaceExtraInfo > ms_EntitySAInterfaceExtraInfoMap;

    bool HasEntitySAInterfaceExtraInfo( CEntitySAInterface* pEntitySAInterface )
    {
        return MapContains( ms_EntitySAInterfaceExtraInfoMap, pEntitySAInterface );
    }

    SEntitySAInterfaceExtraInfo& GetEntitySAInterfaceExtraInfo( CEntitySAInterface* pEntitySAInterface )
    {
        return MapGet( ms_EntitySAInterfaceExtraInfoMap, pEntitySAInterface );
    }

    void RemoveEntitySAInterfaceExtraInfo( CEntitySAInterface* pEntitySAInterface )
    {
        MapRemove( ms_EntitySAInterfaceExtraInfoMap, pEntitySAInterface );
    }


    //
    // CPtrListSingleLink contains item
    //
    bool CPtrListSingleLink_Contains( SStreamSectorEntrySingle* pStreamEntry, CEntitySAInterface* pCheckEntity )
    {
        for ( ; pStreamEntry ; pStreamEntry = pStreamEntry->pNext )
            if ( pStreamEntry->pEntity == pCheckEntity )
                return true;
        return false;
    }

    //
    // CPtrListSingleLink remove item
    //
    void CPtrListSingleLink_Remove( SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pCheckEntity )
    {
        DWORD dwFunc = FUNC_CPtrListSingleLink_Remove;
        _asm
        {
            mov     ecx, ppStreamEntryList
            push    pCheckEntity
            call    dwFunc
        }
    }

    //
    // CPtrListDoubleLink contains item
    //
    bool CPtrListDoubleLink_Contains( SStreamSectorEntryDouble* pStreamEntry, CEntitySAInterface* pCheckEntity )
    {
        for ( ; pStreamEntry ; pStreamEntry = pStreamEntry->pNext )
            if ( pStreamEntry->pEntity == pCheckEntity )
                return true;
        return false;
    }

    //
    // CPtrListDoubleLink remove item
    //
    void CPtrListDoubleLink_Remove( SStreamSectorEntryDouble** ppStreamEntryList, CEntitySAInterface* pCheckEntity )
    {
        DWORD dwFunc = FUNC_CPtrListDoubleLink_Remove;
        _asm
        {
            mov     ecx, ppStreamEntryList
            push    pCheckEntity
            call    dwFunc
        }
    }

    //
    // Ensure entity is removed from previously added stream sectors
    //
    void RemoveEntityFromStreamSectors( CEntitySAInterface* pEntity, bool bRemoveExtraInfo )
    {
        SEntitySAInterfaceExtraInfo* pInfo = MapFind( ms_EntitySAInterfaceExtraInfoMap, pEntity );
        if ( !pInfo )
            return;
        SEntitySAInterfaceExtraInfo& info = *pInfo;

        // Check single link sectors
        for( uint i = 0 ; i < info.AddedSectorSingleList.size() ; i++ )
        {
            if ( CPtrListSingleLink_Contains( *info.AddedSectorSingleList[i], pEntity ) )
            {
                CPtrListSingleLink_Remove( info.AddedSectorSingleList[i], pEntity );
            }
        }
        info.AddedSectorSingleList.clear();

        // Check double link sectors
        for( uint i = 0 ; i < info.AddedSectorDoubleList.size() ; i++ )
        {
            if ( CPtrListDoubleLink_Contains( *info.AddedSectorDoubleList[i], pEntity ) )
            {
                CPtrListDoubleLink_Remove( info.AddedSectorDoubleList[i], pEntity );
            }
        }
        info.AddedSectorDoubleList.clear();

        if ( bRemoveExtraInfo )
            RemoveEntitySAInterfaceExtraInfo( pEntity );
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCObjectDestructor ( DWORD calledFrom, CObjectSAInterface* pObject )
{
    // Tell client to check for things going away
    if ( pGameObjectDestructHandler )
        pGameObjectDestructHandler ( pObject );
}

// Hook info
#define HOOKPOS_CObjectDestructor        0x59F667
#define HOOKSIZE_CObjectDestructor       6
DWORD RETURN_CObjectDestructor =         0x59F66D;
void _declspec(naked) HOOK_CObjectDestructor()
{
    _asm
    {
        pushad
        push    ecx
        push    [esp+32+4*1+4*2]
        call    OnCObjectDestructor
        add     esp, 4*2
        popad

        mov     eax,dword ptr fs:[00000000h]
        jmp     RETURN_CObjectDestructor
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnVehicleDestructor ( DWORD calledFrom, CVehicleSAInterface* pVehicle )
{
    // Tell client to check for things going away
    if ( pGameVehicleDestructHandler )
        pGameVehicleDestructHandler ( pVehicle );
}

// Hook info
#define HOOKPOS_CVehicleDestructor_US        0x6E2B40
#define HOOKPOS_CVehicleDestructor_EU        0x6E2B40
#define HOOKSIZE_CVehicleDestructor_US       7
#define HOOKSIZE_CVehicleDestructor_EU       7
DWORD RETURN_CVehicleDestructor_US =         0x401355;
DWORD RETURN_CVehicleDestructor_EU =         0x401357;
DWORD RETURN_CVehicleDestructor_BOTH =       NULL;
void _declspec(naked) HOOK_CVehicleDestructor()
{
    _asm
    {
        pushad
        push    ecx
        push    [esp+32+4*1]
        call    OnVehicleDestructor
        add     esp, 4*2
        popad

        push    0FFFFFFFFh
        jmp     RETURN_CVehicleDestructor_BOTH
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCPlayerPedDestructor ( DWORD calledFrom, CPedSAInterface* pPlayerPed )
{
    // Tell client to check for things going away
    if ( pGamePlayerDestructHandler )
        pGamePlayerDestructHandler ( pPlayerPed );
}

// Hook info
#define HOOKPOS_CPlayerPedDestructor        0x6093B7
#define HOOKSIZE_CPlayerPedDestructor       6
DWORD RETURN_CPlayerPedDestructor =         0x6093BD;
void _declspec(naked) HOOK_CPlayerPedDestructor()
{
    _asm
    {
        pushad
        push    ecx
        push    [esp+32+4*1+4*2]
        call    OnCPlayerPedDestructor
        add     esp, 4*2
        popad

        mov     eax,dword ptr fs:[00000000h]
        jmp     RETURN_CPlayerPedDestructor
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCProjectileDestructor ( DWORD calledFrom, CEntitySAInterface* pProjectile )
{
    // Tell client to check for things going away
    if ( pGameProjectileDestructHandler )
        pGameProjectileDestructHandler ( pProjectile );
}

// Hook info
#define HOOKPOS_CProjectileDestructor        0x5A40E0
#define HOOKSIZE_CProjectileDestructor       6
DWORD RETURN_CProjectileDestructor =         0x5A40E6;
void _declspec(naked) HOOK_CProjectileDestructor()
{
    _asm
    {
        pushad
        push    ecx
        push    [esp+32+4*1]
        call    OnCProjectileDestructor
        add     esp, 4*2
        popad

        mov     dword ptr [ecx], 867030h
        jmp     RETURN_CProjectileDestructor
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCEntityDestructor ( DWORD calledFrom, CEntitySAInterface* pEntity )
{
    RemoveEntityFromStreamSectors( pEntity, true );
}


// Hook info
#define HOOKPOS_CEntityDestructor        0x535E97
#define HOOKSIZE_CEntityDestructor       6
DWORD RETURN_CEntityDestructor =         0x535E9D;
void _declspec(naked) HOOK_CEntityDestructor()
{
    _asm
    {
        pushad
        push    ecx
        push    [esp+32+4*1]
        call    OnCEntityDestructor
        add     esp, 4*2
        popad

        mov     eax, dword ptr fs:[00000000h]
        jmp     RETURN_CEntityDestructor
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void cdecl OnCEntityAddMid1 ( SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface )
{
    // ARRAY_LodSectors
    SEntitySAInterfaceExtraInfo& info = GetEntitySAInterfaceExtraInfo( pEntitySAInterface );
    info.AddedSectorSingleList.push_back( ppStreamEntryList );
}


// Hook info
#define HOOKPOS_CEntityAddMid1        0x5348FB
#define HOOKSIZE_CEntityAddMid1       5
#define HOOKCHECK_CEntityAddMid1      0xE8
DWORD RETURN_CEntityAddMid1 =         0x534900;
void _declspec(naked) HOOK_CEntityAddMid1()
{
    _asm
    {
        pushad
        push    [esp+32+4*0]
        push    ecx
        call    OnCEntityAddMid1
        add     esp, 4*2
        popad

        mov     eax, 0x5335E0   // CPtrListSingleLink::Add
        call    eax
        jmp     RETURN_CEntityAddMid1
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void cdecl OnCEntityAddMid2 ( SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface )
{
    // ARRAY_BuildingSectors
    SEntitySAInterfaceExtraInfo& info = GetEntitySAInterfaceExtraInfo( pEntitySAInterface );
    info.AddedSectorSingleList.push_back( ppStreamEntryList );
}


// Hook info
#define HOOKPOS_CEntityAddMid2        0x534A10
#define HOOKSIZE_CEntityAddMid2       5
#define HOOKCHECK_CEntityAddMid2      0xE8
DWORD RETURN_CEntityAddMid2 =         0x534A15;
void _declspec(naked) HOOK_CEntityAddMid2()
{
    _asm
    {
        pushad
        push    [esp+32+4*0]
        push    ecx
        call    OnCEntityAddMid2
        add     esp, 4*2
        popad

        mov     eax, 0x5335E0   // CPtrListSingleLink::Add
        call    eax
        jmp     RETURN_CEntityAddMid2
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void cdecl OnCEntityAddMid3 ( SStreamSectorEntryDouble** ppStreamEntryList, CEntitySAInterface* pEntitySAInterface )
{
    // ARRAY_VehicleSectors
    // ARRAY_PedSectors
    // ARRAY_ObjectSectors
    // ARRAY_DummySectors
    SEntitySAInterfaceExtraInfo& info = GetEntitySAInterfaceExtraInfo( pEntitySAInterface );
    info.AddedSectorDoubleList.push_back( ppStreamEntryList );
}


// Hook info
#define HOOKPOS_CEntityAddMid3        0x534AA2
#define HOOKSIZE_CEntityAddMid3       5
#define HOOKCHECK_CEntityAddMid3      0xE8
DWORD RETURN_CEntityAddMid3 =         0x534AA7;
void _declspec(naked) HOOK_CEntityAddMid3()
{
    _asm
    {
        pushad
        push    [esp+32+4*0]
        push    ecx
        call    OnCEntityAddMid3
        add     esp, 4*2
        popad

        mov     eax, 0x533670   // CPtrListDoubleLink::Add
        call    eax
        jmp     RETURN_CEntityAddMid3
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void cdecl OnCEntityRemovePost ( CEntitySAInterface* pEntity )
{
    RemoveEntityFromStreamSectors( pEntity, false );
}


// Hook info
#define HOOKPOS_CEntityRemove        0x534AE0
#define HOOKSIZE_CEntityRemove       5
#define HOOKCHECK_CEntityRemove      0x83
DWORD RETURN_CEntityRemove =         0x534AE5;
void _declspec(naked) HOOK_CEntityRemove()
{
    _asm
    {
        push    [esp+4*1]
        call inner
        add     esp, 4*1

        pushad
        push    [esp+32+4*1]
        call    OnCEntityRemovePost
        add     esp, 4*1
        popad
        retn

inner:
        // Original code
        sub     esp, 30h
        push    ebx
        push    ebp
        jmp     RETURN_CEntityRemove
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCStreamingRemoveModel ( DWORD calledFrom, ushort usModelId )
{
    // Tell client to check for things going away
    if ( pGameModelRemoveHandler )
        pGameModelRemoveHandler ( usModelId );
}

// Hook info
#define HOOKPOS_CStreamingRemoveModel        0x4089A0
#define HOOKSIZE_CStreamingRemoveModel       5
DWORD RETURN_CStreamingRemoveModel =         0x4089A5;
void _declspec(naked) HOOK_CStreamingRemoveModel()
{
    _asm
    {
        pushad
        push    [esp+32+4*1]
        push    [esp+32+4*1]
        call    OnCStreamingRemoveModel
        add     esp, 4*2
        popad

        push    esi
        mov     esi, [esp+8]
        jmp     RETURN_CStreamingRemoveModel
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Set handlers
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::SetGameObjectDestructHandler ( GameObjectDestructHandler * pHandler )
{
    pGameObjectDestructHandler = pHandler;
}

void CMultiplayerSA::SetGameVehicleDestructHandler ( GameVehicleDestructHandler * pHandler )
{
    pGameVehicleDestructHandler = pHandler;
}

void CMultiplayerSA::SetGamePlayerDestructHandler ( GamePlayerDestructHandler * pHandler )
{
    pGamePlayerDestructHandler = pHandler;
}

void CMultiplayerSA::SetGameProjectileDestructHandler ( GameProjectileDestructHandler * pHandler )
{
    pGameProjectileDestructHandler = pHandler;
}

void CMultiplayerSA::SetGameModelRemoveHandler ( GameModelRemoveHandler * pHandler )
{
    pGameModelRemoveHandler = pHandler;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for HookDestructors
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_HookDestructors ( void )
{
   EZHookInstall ( CObjectDestructor );
   EZHookInstall ( CVehicleDestructor );
   EZHookInstall ( CProjectileDestructor );
   EZHookInstall ( CPlayerPedDestructor );
   EZHookInstall ( CEntityDestructor );
   EZHookInstall ( CStreamingRemoveModel );
   EZHookInstall ( CEntityAddMid1 );
   EZHookInstall ( CEntityAddMid2 );
   EZHookInstall ( CEntityAddMid3 );
   EZHookInstall ( CEntityRemove );
}
