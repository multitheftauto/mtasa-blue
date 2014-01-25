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
   EZHookInstall ( CStreamingRemoveModel );
}
