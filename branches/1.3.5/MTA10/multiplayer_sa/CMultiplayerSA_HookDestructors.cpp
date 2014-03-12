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

    #define ARRAY_StreamSectors                                 0xB7D0B8
    #define NUM_StreamSectorRows                                120
    #define NUM_StreamSectorCols                                120
    #define ARRAY_StreamRepeatSectors                           0xB992B8
    #define NUM_StreamRepeatSectorRows                          16
    #define NUM_StreamRepeatSectorCols                          16

    #define ARRAY_BuildingSectors   ((SStreamSectorEntrySingle**)( (ARRAY_StreamSectors) + 0 ))
    #define ARRAY_VehicleSectors    ((SStreamSectorEntryDouble**)( (ARRAY_StreamRepeatSectors) + 0 ))
    #define ARRAY_PedSectors        ((SStreamSectorEntryDouble**)( (ARRAY_StreamRepeatSectors) + 4 ))
    #define ARRAY_ObjectSectors     ((SStreamSectorEntryDouble**)( (ARRAY_StreamRepeatSectors) + 8 ))
    #define ARRAY_DummySectors      ((SStreamSectorEntryDouble**)( (ARRAY_StreamSectors) + 4 ))

    #define NUM_BuildingSectors     ( NUM_StreamSectorRows * NUM_StreamSectorCols )
    #define NUM_VehicleSectors      ( NUM_StreamRepeatSectorRows * NUM_StreamRepeatSectorCols )
    #define NUM_PedSectors          ( NUM_StreamRepeatSectorRows * NUM_StreamRepeatSectorCols )
    #define NUM_ObjectSectors       ( NUM_StreamRepeatSectorRows * NUM_StreamRepeatSectorCols )
    #define NUM_DummySectors        ( NUM_StreamSectorRows * NUM_StreamSectorCols )

    #define FUNC_CPtrListSingleLink_Remove  0x0533610
    #define FUNC_CPtrListDoubleLink_Remove  0x05336B0

    struct SStreamSectorEntrySingle
    {
        CEntitySAInterface*     pEntity;
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
    // Log problem
    //
    void LogSectorMessage( uint uiId, const char* szContext, CEntitySAInterface* pCheckEntity, CEntitySAInterface* pEntity, uint n )
    {
        SString strMsg( "n:%-5d  vtbl:%08x  ", n, pEntity->vtbl );
        if ( pCheckEntity )
        {
            CVector vPos;
            bool bHasMatrix = ( pCheckEntity->Placeable.matrix != NULL );
            if ( bHasMatrix )
                vPos = pCheckEntity->Placeable.matrix->vPos;
            else
                vPos = pCheckEntity->Placeable.m_transform.m_translate; 
            strMsg += SString( "Type:%d  Model:%-5d  HasMatrix:%d  Pos:%0.1f,%0.1f,%0.1f  ", pCheckEntity->nType, pCheckEntity->m_nModelIndex, bHasMatrix, vPos.fX, vPos.fY, vPos.fZ );
        }
        LogEvent ( uiId, "CheckSectors", szContext, strMsg, 8000 + uiId );
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
    void CPtrListSingleLink_Remove( SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pCheckEntity, const char* szContext )
            {
        LogSectorMessage( 903, szContext, pCheckEntity, pCheckEntity, pCheckEntity->nType );
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
    void CPtrListDoubleLink_Remove( SStreamSectorEntryDouble** ppStreamEntryList, CEntitySAInterface* pCheckEntity, const char* szContext )
        {
        LogSectorMessage( 904, szContext, pCheckEntity, pCheckEntity, pCheckEntity->nType );
        DWORD dwFunc = FUNC_CPtrListDoubleLink_Remove;
        _asm
                {
            mov     ecx, ppStreamEntryList
            push    pCheckEntity
            call    dwFunc
        }
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
void _cdecl OnCBuildingDestructor ( DWORD calledFrom, CEntitySAInterface* pBuilding )
{
}


// Hook info
#define HOOKPOS_CBuildingDestructor        0x404134
#define HOOKSIZE_CBuildingDestructor       5
DWORD RETURN_CBuildingDestructor =         0x404139;
void _declspec(naked) HOOK_CBuildingDestructor()
{
    _asm
    {
        pushad
        push    ecx
        push    [esp+32+4*1]
        call    OnCBuildingDestructor
        add     esp, 4*2
        popad

        mov     eax, 0x404180       // CBuilding::~CBuilding()
        call    eax 
        jmp     RETURN_CBuildingDestructor
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
void _cdecl OnCEntityDestructor ( DWORD calledFrom, CEntitySAInterface* pEntity )
{
    if ( pEntity->nType == 1 )
    {
        // Type 1 - CBuilding
        for ( unsigned int n = 0 ; n < NUM_BuildingSectors; n++ )
        {
            SStreamSectorEntrySingle** ppStreamEntryList = &ARRAY_BuildingSectors[ n * 2 ];

            if ( CPtrListSingleLink_Contains( *ppStreamEntryList, pEntity ) )
            {
                CPtrListSingleLink_Remove( ppStreamEntryList, pEntity, "(old) Entity in sectors at delete" );
                n--;
            }
        }
    }
    else
    if ( pEntity->nType == 2 )
    {
        // Type 2 - CVehicle
        for ( unsigned int n = 0 ; n < NUM_VehicleSectors; n++ )
        {
            SStreamSectorEntryDouble** ppStreamEntryList = &ARRAY_VehicleSectors[ n * 3 ];

            if ( CPtrListDoubleLink_Contains( *ppStreamEntryList, pEntity ) )
            {
                CPtrListDoubleLink_Remove( ppStreamEntryList, pEntity, "(old) Entity in sectors at delete" );
                n--;
            }
        }
    }
    if ( pEntity->nType == 3 )
    {
        // Type 3 - CPed
        for ( unsigned int n = 0 ; n < NUM_PedSectors; n++ )
        {
            SStreamSectorEntryDouble** ppStreamEntryList = &ARRAY_PedSectors[ n * 3 ];

            if ( CPtrListDoubleLink_Contains( *ppStreamEntryList, pEntity ) )
            {
                CPtrListDoubleLink_Remove( ppStreamEntryList, pEntity, "(old) Entity in sectors at delete" );
                n--;
            }
        }
    }
    if ( pEntity->nType == 4 )
    {
        // Type 4 - CObject
        for ( unsigned int n = 0 ; n < NUM_ObjectSectors; n++ )
        {
            SStreamSectorEntryDouble** ppStreamEntryList = &ARRAY_ObjectSectors[ n * 3 ];

            if ( CPtrListDoubleLink_Contains( *ppStreamEntryList, pEntity ) )
            {
                CPtrListDoubleLink_Remove( ppStreamEntryList, pEntity, "(old) Entity in sectors at delete" );
                n--;
            }
        }
    }
    else
    if ( pEntity->nType == 5 )
    {
        // Type 5 - CDummy
        for ( unsigned int n = 0 ; n < NUM_DummySectors; n++ )
        {
            SStreamSectorEntryDouble** ppStreamEntryList = &ARRAY_DummySectors[ n * 2 ];

            if ( CPtrListDoubleLink_Contains( *ppStreamEntryList, pEntity ) )
            {
                CPtrListDoubleLink_Remove( ppStreamEntryList, pEntity, "(old) Entity in sectors at delete" );
                n--;
            }
        }
    }

    if ( HasEntitySAInterfaceExtraInfo( pEntity ) )
    {
        SEntitySAInterfaceExtraInfo& info = GetEntitySAInterfaceExtraInfo( pEntity );
        if ( !info.AddedSectorSingleList.empty() )
            LogSectorMessage( 905, "Entity AddedSectorSingleList not empty at delete", pEntity, pEntity, pEntity->nType );
        if ( !info.AddedSectorDoubleList.empty() )
            LogSectorMessage( 906, "Entity AddedSectorDoubleList not empty at delete", pEntity, pEntity, pEntity->nType );

        RemoveEntitySAInterfaceExtraInfo( pEntity );
    }
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
void cdecl OnCEntityRemovePost ( CEntitySAInterface* pEntitySAInterface )
{
    // Ensure entity has been removed from previously added sectors
    SEntitySAInterfaceExtraInfo& info = GetEntitySAInterfaceExtraInfo( pEntitySAInterface );

    // Check single link sectors
    for( uint i = 0 ; i < info.AddedSectorSingleList.size() ; i++ )
    {
        if ( CPtrListSingleLink_Contains( *info.AddedSectorSingleList[i], pEntitySAInterface ) )
        {
            CPtrListSingleLink_Remove( info.AddedSectorSingleList[i], pEntitySAInterface, "(new) Entity in sectors at delete" );
        }
    }
    info.AddedSectorSingleList.clear();

    // Check double link sectors
    for( uint i = 0 ; i < info.AddedSectorDoubleList.size() ; i++ )
    {
        if ( CPtrListDoubleLink_Contains( *info.AddedSectorDoubleList[i], pEntitySAInterface ) )
        {
            CPtrListDoubleLink_Remove( info.AddedSectorDoubleList[i], pEntitySAInterface, "(new) Entity in sectors at delete" );
        }
    }
    info.AddedSectorDoubleList.clear();
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
   EZHookInstall ( CBuildingDestructor );
   EZHookInstall ( CEntityDestructor );
   EZHookInstall ( CStreamingRemoveModel );
   EZHookInstall ( CEntityAddMid1 );
   EZHookInstall ( CEntityAddMid2 );
   EZHookInstall ( CEntityAddMid3 );
   EZHookInstall ( CEntityRemove );
}
