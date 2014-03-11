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
    void CPtrListSingleLink_Remove( SStreamSectorEntrySingle** ppStreamEntryList, CEntitySAInterface* pCheckEntity )
            {
        LogSectorMessage( 903, "Entity in sectors at delete", pCheckEntity, pCheckEntity, pCheckEntity->nType );
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
        LogSectorMessage( 904, "Entity in sectors at delete", pCheckEntity, pCheckEntity, pCheckEntity->nType );
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
                CPtrListSingleLink_Remove( ppStreamEntryList, pEntity );
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
                CPtrListDoubleLink_Remove( ppStreamEntryList, pEntity );
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
                CPtrListDoubleLink_Remove( ppStreamEntryList, pEntity );
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
                CPtrListDoubleLink_Remove( ppStreamEntryList, pEntity );
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
                CPtrListDoubleLink_Remove( ppStreamEntryList, pEntity );
                n--;
            }
        }
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
}
