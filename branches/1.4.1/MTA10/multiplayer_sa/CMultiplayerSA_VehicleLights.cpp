/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_VehicleLights.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    // Used to save state between CVehicle_DoTailLightEffect_Mid and CVehicle_DoTailLightEffect_Mid2
    uint    bCameraFacingCorona = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoTailLightEffect hook 1
//
// Save result of (camera dir).(corona dir)
// Also stops DoTailLightEffect returning false when the camera is to the side
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_CVehicle_DoTailLightEffect_Mid          0x006E18E5
#define HOOKSIZE_CVehicle_DoTailLightEffect_Mid         6
#define HOOKCHECK_CVehicle_DoTailLightEffect_Mid        0x0F
DWORD RETURN_CVehicle_DoTailLightEffect_Mid =           0x006E18EB;
void _declspec(naked) HOOK_CVehicle_DoTailLightEffect_Mid()
{
    _asm
    {
        // Save result of comparing camera and corona direction
        mov     eax, 0
        jnz     behind_corona
        mov     eax, 1

behind_corona:
        mov     bCameraFacingCorona, eax
        jmp     RETURN_CVehicle_DoTailLightEffect_Mid
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoTailLightEffect hook 2
//
// Figure out if the tail light corona should be added
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_CVehicle_DoTailLightEffect_Mid2         0x006E19E6
#define HOOKSIZE_CVehicle_DoTailLightEffect_Mid2        10
#define HOOKCHECK_CVehicle_DoTailLightEffect_Mid2       0x8B
DWORD RETURN_CVehicle_DoTailLightEffect_Mid2           = 0x006E19F0;
DWORD RETURN_CVehicle_DoTailLightEffect_Mid2_NoCorona  = 0x006E1A32;
void _declspec(naked) HOOK_CVehicle_DoTailLightEffect_Mid2()
{
    _asm
    {
        movzx   eax, byte ptr [esp+0Fh]
        test    al, al
        jz      no_corona           // Tail light off

        mov     eax, bCameraFacingCorona
        test    al, al
        jz      no_corona           // Camera looking behind corona

        // Add corona
        mov     eax, [esp+38h]
        fld     dword ptr ds:[0xB6F118]
        jmp     RETURN_CVehicle_DoTailLightEffect_Mid2

no_corona:
        sub     esp, 54h
        jmp     RETURN_CVehicle_DoTailLightEffect_Mid2_NoCorona
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_VehicleDamage
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_VehicleLights( void )
{
   EZHookInstallChecked( CVehicle_DoTailLightEffect_Mid );
   EZHookInstallChecked( CVehicle_DoTailLightEffect_Mid2 );
}
