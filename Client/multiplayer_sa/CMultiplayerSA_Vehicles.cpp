/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Vehicles.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::SetDoorDamage
//
// This hook checks if our CVehicleSA instance prevents door damage
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6B1602 | 68 78 81 84 00    | push    offset CAutomobile::SetDoorDamage @ 0x848178
// >>> 0x6B1607 | 64 A1 00 00 00 00 | mov     eax, large fs:0
//     0x6B160D | 50                | push    eax
#define HOOKPOS_CAutomobile__SetDoorDamage         0x6B1607
#define HOOKSIZE_CAutomobile__SetDoorDamage        6
static DWORD CONTINUE_CAutomobile__SetDoorDamage = 0x6B160D;

static bool __fastcall CUSTOM_CAutomobile__SetDoorDamage(CVehicleSAInterface* vehicle)
{
    SClientEntity<CVehicleSA>* pair = pGameInterface->GetPools()->GetVehicle((DWORD*)vehicle);

    if (!pair)
        return true;

    return !pair->pEntity->AreDoorsUndamageable();
}

static void _declspec(naked) HOOK_CAutomobile__SetDoorDamage()
{
    _asm
    {
        pushad
        call    CUSTOM_CAutomobile__SetDoorDamage
        test    al, al
        jne     continueGameCodeLocation

        popad
        add     esp, 8
        retn    8

        continueGameCodeLocation:
        popad
        mov     eax, fs:[0]
        jmp     CONTINUE_CAutomobile__SetDoorDamage
    }
}

 //////////////////////////////////////////////////////////////////////////////////////////
 //
 // CMultiplayerSA::InitHooks_Vehicles
 //
 // Setup hooks
 //
 //////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Vehicles()
{
    EZHookInstall(CAutomobile__SetDoorDamage);
}
