/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_VehicleDummies.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <enums/VehicleType.h>

static const uint32_t CModelInfo__ms_modelInfoPtrs = 0xA9B0C8;

static CVector* vehicleDummiesPositionArray = nullptr;

static void __cdecl UpdateVehicleDummiesPositionArray(CVehicleSAInterface* vehicleInterface)
{
    SClientEntity<CVehicleSA>* vehicle = pGameInterface->GetPools()->GetVehicle(reinterpret_cast<DWORD*>(vehicleInterface));
    vehicleDummiesPositionArray = (vehicle != nullptr) ? vehicle->pEntity->GetDummyPositions() : nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::AddExhaustParticles (1 of 2)
//
// Required for: Position for vehicle exhaust particles (eVehicleDummies::EXHAUST)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6DE2EA | 8B 3C 8D C8 B0 A9 00 | mov  edi, CModelInfo::ms_modelInfoPtrs
// >>> 0x6DE2F1 | 8B 57 5C             | mov  edx, [edi+5Ch]
// >>> 0x6DE2F4 | 83 C2 48             | add  edx, 48h
// >>> 0x6DE2F7 | 8B 02                | mov  eax, [edx]
// >>> 0x6DE2F9 | 89 44 24 64          | mov  [esp+0D0h+var_70.y], eax
// >>> 0x6DE2FD | 8B 4A 04             | mov  ecx, [edx+4]
// >>> 0x6DE300 | D9 44 24 64          | fld  [esp+0D0h+var_70.y]
// >>> 0x6DE304 | 89 4C 24 68          | mov  [esp+0D0h+var_70.z], ecx
// >>> 0x6DE308 | 8B 52 08             | mov  edx, [edx+8]
// >>> 0x6DE30B | D8 0D 1C 8C 85 00    | fmul 1-.0f
// >>> 0x6DE311 | 89 4C 24 40          | mov  [esp+0D0h+var_98.z], ecx
// >>> 0x6DE315 | 89 44 24 3C          | mov  [esp+0D0h+var_98.y], eax
// >>> 0x6DE319 | 53                   | push ebx
// >>> 0x6DE31A | 8D 8C 24 80 00 00 00 | lea  ecx, [esp+0D4h+var_58.right.y]
// >>> 0x6DE321 | D9 5C 24 40          | fstp [esp+0D4h+var_98.y]
// >>> 0x6DE325 | 89 54 24 70          | mov  [esp+0D4h+var_64.x], edx
// >>> 0x6DE329 | 89 54 24 48          | mov  [esp+0D4h+var_8C], edx
//     0x6DE32D | E8 BE D9 EB FF       | call CMatrix::CMatrix(CMatrix const &)
#define HOOKPOS_CVehicle_AddExhaustParticles_1               0x6DE2F1
#define HOOKSIZE_CVehicle_AddExhaustParticles_1              59
static const DWORD CONTINUE_CVehicle_AddExhaustParticles_1 = 0x6DE32D;

static void __cdecl ApplyExhaustParticlesPosition(CVehicleSAInterface* vehicleInterface, CVector* mainPosition, CVector* secondaryPosition)
{
    UpdateVehicleDummiesPositionArray(vehicleInterface);

    if (vehicleDummiesPositionArray != nullptr)
    {
        *mainPosition = vehicleDummiesPositionArray[static_cast<std::size_t>(VehicleDummies::EXHAUST)];
        *secondaryPosition = vehicleDummiesPositionArray[static_cast<std::size_t>(VehicleDummies::EXHAUST_SECONDARY)];

        // NOTE(botder): Certain bike models (NRG-500, BF-400, FCR-900) actually use the secondary exhaust position
        //               and we can't abuse it for these models to position double exhausts here.
        //               Bikes use the secondary exhaust position in case they have extras (read: exhausts) installed
        bool    applyNegativeMainPosition = false;
        int16_t extras = vehicleInterface->m_upgrades[0];

        if (static_cast<VehicleType>(vehicleInterface->m_nModelIndex) == VehicleType::VT_NRG500)
        {
            applyNegativeMainPosition = extras != 0 && extras != 1;
        }
        else if (static_cast<VehicleType>(vehicleInterface->m_nModelIndex) == VehicleType::VT_BF400)
        {
            applyNegativeMainPosition = extras != 2;
        }
        else if (static_cast<VehicleType>(vehicleInterface->m_nModelIndex) == VehicleType::VT_FCR900)
        {
            applyNegativeMainPosition = extras != 1;
        }

        if (applyNegativeMainPosition || secondaryPosition->fX == 0.0 && secondaryPosition->fY == 0.0 && secondaryPosition->fZ == 0.0)
        {
            *secondaryPosition = *mainPosition;
            secondaryPosition->fX *= -1.0f;
        }
    }
    else
    {
        mainPosition->fX = 0.0f;
        mainPosition->fY = 0.0f;
        mainPosition->fZ = 0.0f;
        secondaryPosition->fX = 0.0f;
        secondaryPosition->fY = 0.0f;
        secondaryPosition->fZ = 0.0f;
    }
}

static void _declspec(naked) HOOK_CVehicle_AddExhaustParticles_1()
{
    _asm
    {
        pushad
        lea     eax, [esp+5Ch]
        lea     ebx, [esp+84h]
        push    eax             // CVector*
        push    ebx             // CVector*
        push    esi             // CVehicleSAInterface*
        call    ApplyExhaustParticlesPosition
        add     esp, 12
        popad

        mov     edx, vehicleDummiesPositionArray
        add     edx, 48h
        mov     edx, [edx+8]
        push    ebx
        lea     ecx, [esp+0D4h-54h]
        mov     [esp+0D4h-64h], edx
        mov     [esp+0D4h-8Ch], edx
        jmp     CONTINUE_CVehicle_AddExhaustParticles_1
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::AddExhaustParticles (2 of 2)
//
// Required for: Position for NRG-500 secondary exhaust particles (eVehicleDummies::EXHAUST_SECONDARY)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6DE39C | 75 31             | jnz short loc_6DE3CF
// >>> 0x6DE39E | 8B 57 5C          | mov edx, [edi+5Ch]
// >>> 0x6DE3A1 | 81 C2 84 00 00 00 | add edx, 84h
//     0x6DE3A7 | 8B 02             | mov eax, [edx]
#define HOOKPOS_CVehicle_AddExhaustParticles_2               0x6DE39E
#define HOOKSIZE_CVehicle_AddExhaustParticles_2              9
static const DWORD CONTINUE_CVehicle_AddExhaustParticles_2 = 0x6DE3A7;

static void _declspec(naked) HOOK_CVehicle_AddExhaustParticles_2()
{
    _asm
    {
        pushad
        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     edx, vehicleDummiesPositionArray
        add     edx, 84h
        jmp     CONTINUE_CVehicle_AddExhaustParticles_2

        continueWithOriginalCode:
        popad
        mov     edx, [edi+5Ch]
        add     edx, 84h
        jmp     CONTINUE_CVehicle_AddExhaustParticles_2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::AddDamagedVehicleParticles
//
// Required for: Position for vehicle engine damage particles (eVehicleDummies::ENGINE)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6D2B02 | 8B 0C 8D C8 B0 A9 00 | mov  ecx, CModelInfo::ms_modelInfoPtrs
// >>> 0x6D2B09 | 8B 49 5C             | mov  ecx, [ecx+5Ch]
// >>> 0x6D2B0C | 83 C1 54             | add  ecx, 54h
//     0x6D2B0F | 84 D2                | test dl, dl
#define HOOKPOS_CVehicle_AddDamagedVehicleParticles               0x6D2B09
#define HOOKSIZE_CVehicle_AddDamagedVehicleParticles              6
static const DWORD CONTINUE_CVehicle_AddDamagedVehicleParticles = 0x6D2B0F;

static void _declspec(naked) HOOK_CVehicle_AddDamagedVehicleParticles()
{
    _asm
    {
        pushad
        push    esi // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     ecx, vehicleDummiesPositionArray
        add     ecx, 54h
        jmp     CONTINUE_CVehicle_AddDamagedVehicleParticles

        continueWithOriginalCode:
        popad
        mov     ecx, [edi+5Ch]
        add     ecx, 54h
        jmp     CONTINUE_CVehicle_AddDamagedVehicleParticles
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CFire::ProcessFire
//
// Required for: Update vehicle fire position
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x53A70D | 8B 04 95 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x53A714 | 8B 40 5C             | mov eax, [eax+5Ch]
// >>> 0x53A717 | 8B 10                | mov edx, [eax]
//     0x53A719 | 89 54 24 18          | mov [esp+3Ch+var_24.x], edx
#define HOOKPOS_CFire_ProcessFire               0x53A714
#define HOOKSIZE_CFire_ProcessFire              5
static const DWORD CONTINUE_CFire_ProcessFire = 0x53A719;

static void _declspec(naked) HOOK_CFire_ProcessFire()
{
    _asm
    {
        pushad
        push    ecx            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     eax, vehicleDummiesPositionArray
        mov     edx, [eax]
        jmp     CONTINUE_CFire_ProcessFire

        continueWithOriginalCode:
        popad
        mov     eax, [eax+5Ch]
        mov     edx, [eax]
        jmp     CONTINUE_CFire_ProcessFire
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::DoNitroEffect (1 of 2)
//
// Required for: Position for vehicle nitro (+ recharge) effect particles
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6A3BDB | 8B 04 85 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6A3BE2 | 8B 48 5C             | mov ecx, [eax+5Ch]
// >>> 0x6A3BE5 | 83 C1 48             | add ecx, 48h
//     0x6A3BE8 | 8B 11                | mov edx, [ecx]
#define HOOKPOS_CAutomobile_DoNitroEffect_1               0x6A3BE2
#define HOOKSIZE_CAutomobile_DoNitroEffect_1              6
static const DWORD CONTINUE_CAutomobile_DoNitroEffect_1 = 0x6A3BE8;

static void _declspec(naked) HOOK_CAutomobile_DoNitroEffect_1()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     ecx, vehicleDummiesPositionArray
        add     ecx, 48h
        jmp     CONTINUE_CAutomobile_DoNitroEffect_1

        continueWithOriginalCode:
        popad
        mov     ecx, [eax+5Ch]
        add     ecx, 48h
        jmp     CONTINUE_CAutomobile_DoNitroEffect_1
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::DoNitroEffect (2 of 2)
//
// Required for: Position for vehicle secondary nitro (+ recharge) effect particles
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6A3C66 | 74 76             | jz   short loc_6A3CDE
// >>> 0x6A3C68 | 8B 44 24 1C       | mov  eax, [esp+38h+position.z]
// >>> 0x6A3C6C | D9 44 24 14       | fld  [esp+38h+position.x]
// >>> 0x6A3C70 | D8 0D 1C 8C 85 00 | fmul -1.0f
// >>> 0x6A3C76 | 8B 4C 24 14       | mov  ecx, [esp+38h+position.x]
// >>> 0x6A3C7A | 8B 54 24 18       | mov  edx, [esp+38h+position.y]
// >>> 0x6A3C7E | 89 44 24 28       | mov  [esp+38h+secondaryNitroPos.z], eax
// >>> 0x6A3C82 | 85 7E 40          | test [esi+40h], edi
// >>> 0x6A3C85 | 89 4C 24 20       | mov  [esp+38h+secondaryNitroPos.x], ecx
// >>> 0x6A3C89 | D9 5C 24 20       | fstp [esp+38h+secondaryNitroPos.x]
// >>> 0x6A3C8D | 89 54 24 24       | mov  [esp+38h+secondaryNitroPos.y], edx
//     0x6A3C91 | 74 4B             | jz   short loc_6A3CDE
#define HOOKPOS_CAutomobile_DoNitroEffect_2               0x6A3C68
#define HOOKSIZE_CAutomobile_DoNitroEffect_2              41
static const DWORD CONTINUE_CAutomobile_DoNitroEffect_2 = 0x6A3C91;

static void __cdecl ApplySecondaryExhaustNitroPosition(CVehicleSAInterface* vehicleInterface, CVector* secondaryExhaustPosition)
{
    UpdateVehicleDummiesPositionArray(vehicleInterface);

    if (vehicleDummiesPositionArray != nullptr)
    {
        *secondaryExhaustPosition = vehicleDummiesPositionArray[(std::size_t)VehicleDummies::EXHAUST_SECONDARY];

        if (secondaryExhaustPosition->fX == 0.0 && secondaryExhaustPosition->fY == 0.0 && secondaryExhaustPosition->fZ == 0.0)
        {
            *secondaryExhaustPosition = vehicleDummiesPositionArray[(std::size_t)VehicleDummies::EXHAUST];
            secondaryExhaustPosition->fX *= -1.0f;
        }
    }
    else
    {
        secondaryExhaustPosition->fX = 0.0f;
        secondaryExhaustPosition->fY = 0.0f;
        secondaryExhaustPosition->fZ = 0.0f;
    }
}

static void _declspec(naked) HOOK_CAutomobile_DoNitroEffect_2()
{
    _asm
    {
        pushad
        lea     eax, [esp+40h]
        push    eax             // CVector*
        push    esi             // CVehicleSAInterface*
        call    ApplySecondaryExhaustNitroPosition
        add     esp, 8
        popad

        test    [esi+40h], edi
        jmp     CONTINUE_CAutomobile_DoNitroEffect_2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoVehicleLights
//
// Note: _1 is for the light exactly on the dummy position,
//       _2 is for the light on the mirrored dummy position.
// Required for: Position for a secondary front light (eVehicleDummies::LIGHT_FRONT_SECONDARY)
//               The secondary front light only appears if the position is non-zero
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6E1F35 | 8B 04 95 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6E1F3C | 8B 40 5C             | mov eax, [eax+5Ch]
// >>> 0x6E1F3F | 83 C0 18             | add eax, 18h
//     0x6E1F42 | 8B 08                | mov ecx, [eax]
#define HOOKPOS_CVehicle_DoVehicleLights_1               0x6E1F3C
#define HOOKSIZE_CVehicle_DoVehicleLights_1              6
static const DWORD CONTINUE_CVehicle_DoVehicleLights_1 = 0x6E1F42;

static void _declspec(naked) HOOK_CVehicle_DoVehicleLights_1()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     eax, vehicleDummiesPositionArray
        add     eax, 18h
        jmp     CONTINUE_CVehicle_DoVehicleLights_1

        continueWithOriginalCode:
        popad
        mov     eax, [eax+5Ch]
        add     eax, 18h
        jmp     CONTINUE_CVehicle_DoVehicleLights_1
    }
}

//     0x6E22C6 | 8B 04 85 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6E22CD | 8B 48 5C             | mov ecx, [eax+5Ch]
// >>> 0x6E22D0 | 83 C1 18             | add ecx, 18h
//     0x6E22D3 | 8B 11                | mov edx, [ecx]
#define HOOKPOS_CVehicle_DoVehicleLights_2               0x6E22CD
#define HOOKSIZE_CVehicle_DoVehicleLights_2              6
static const DWORD CONTINUE_CVehicle_DoVehicleLights_2 = 0x6E22D3;

static void _declspec(naked) HOOK_CVehicle_DoVehicleLights_2()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     ecx, vehicleDummiesPositionArray
        add     ecx, 18h
        jmp     CONTINUE_CVehicle_DoVehicleLights_2

        continueWithOriginalCode:
        popad
        mov     ecx, [eax+5Ch]
        add     ecx, 18h
        jmp     CONTINUE_CVehicle_DoVehicleLights_2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::ProcessCarOnFireAndExplode
//
// Required for: Position for the fire of the burning vehicle engine (eVehicleDummies::ENGINE)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6A7178 | 8B 04 95 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6A717F | 8B 40 5C             | mov eax, [eax+5Ch]
// >>> 0x6A7182 | 83 C0 54             | add eax, 54h
//     0x6A7185 | 8B 08                | mov ecx, [eax]
#define HOOKPOS_CAutomobile_ProcessCarOnFireAndExplode               0x6A717F
#define HOOKSIZE_CAutomobile_ProcessCarOnFireAndExplode              6
static const DWORD CONTINUE_CAutomobile_ProcessCarOnFireAndExplode = 0x6A7185;

static void _declspec(naked) HOOK_CAutomobile_ProcessCarOnFireAndExplode()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     eax, vehicleDummiesPositionArray
        add     eax, 54h
        jmp     CONTINUE_CAutomobile_ProcessCarOnFireAndExplode

        continueWithOriginalCode:
        popad
        mov     eax, [eax+5Ch]
        add     eax, 54h
        jmp     CONTINUE_CAutomobile_ProcessCarOnFireAndExplode
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CBike::FixHandsToBars
//
// Required for: Position for ped hands on a bike (eVehicleDummies::HAND_REST)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6B804C | 8B 04 95 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6B8053 | 8B 40 5C             | mov eax, [eax+92]
// >>> 0x6B8056 | 83 C0 78             | add eax, 78h
//     0x6B8059 | 8B 10                | mov edx, [eax]
#define HOOKPOS_CBike_FixHandsToBars               0x6B8053
#define HOOKSIZE_CBike_FixHandsToBars              6
static const DWORD CONTINUE_CBike_FixHandsToBars = 0x6B8059;

static void _declspec(naked) HOOK_CBike_FixHandsToBars()
{
    _asm
    {
        pushad
        push    ebx            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     eax, vehicleDummiesPositionArray
        add     eax, 78h
        jmp     CONTINUE_CBike_FixHandsToBars

        continueWithOriginalCode:
        popad
        mov     eax, [eax+5Ch]
        add     eax, 78h
        jmp     CONTINUE_CBike_FixHandsToBars
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::SetPedPositionInCar (1 of 4)
//
// Required for: Seat position for ped in vehicle (eVehicleDummies::SEAT_FRONT)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x5DF989 | 75 5B       | jnz short loc_5DF9E6
// >>> 0x5DF98B | 83 7F 3C 05 | cmp dword ptr [edi+3Ch], 5
// >>> 0x5DF98F | 8B 7F 5C    | mov edi, [edi+5Ch]
//     0x5DF992 | 74 03       | jz  short loc_5DF997
#define HOOKPOS_CPed_SetPedPositionInCar_1               0x5DF98B
#define HOOKSIZE_CPed_SetPedPositionInCar_1              7
static const DWORD CONTINUE_CPed_SetPedPositionInCar_1 = 0x5DF992;

static void _declspec(naked) HOOK_CPed_SetPedPositionInCar_1()
{
    _asm
    {
        pushad
        mov     eax, [esi+58Ch]
        push    eax            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        cmp     dword ptr [edi+3Ch], 5
        mov     edi, vehicleDummiesPositionArray
        jmp     CONTINUE_CPed_SetPedPositionInCar_1

        continueWithOriginalCode:
        popad
        cmp     dword ptr [edi+3Ch], 5
        mov     edi, [edi+5Ch]
        jmp     CONTINUE_CPed_SetPedPositionInCar_1
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::SetPedPositionInCar (2 of 4)
//
// Required for: Seat position for ped in vehicle (eVehicleDummies::SEAT_REAR)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x5DFA54 | 75 22       | jnz short loc_5DFA78
// >>> 0x5DFA56 | 8B 47 5C    | mov eax, [edi+5Ch]
// >>> 0x5DFA59 | 83 C0 3C    | add eax, 3Ch
//     0x5DFA5C | 8B 10       | mov edx, [eax]
#define HOOKPOS_CPed_SetPedPositionInCar_2               0x5DFA56
#define HOOKSIZE_CPed_SetPedPositionInCar_2              6
static const DWORD CONTINUE_CPed_SetPedPositionInCar_2 = 0x5DFA5C;

static void _declspec(naked) HOOK_CPed_SetPedPositionInCar_2()
{
    _asm
    {
        pushad
        mov     eax, [esi+58Ch]
        push    eax            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     eax, vehicleDummiesPositionArray
        add     eax, 3Ch
        jmp     CONTINUE_CPed_SetPedPositionInCar_2

        continueWithOriginalCode:
        popad
        mov     eax, [edi+5Ch]
        add     eax, 3Ch
        jmp     CONTINUE_CPed_SetPedPositionInCar_2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::SetPedPositionInCar (3 of 4)
//
// Required for: Seat position for ped in vehicle (eVehicleDummies::SEAT_FRONT and ::LIGHT_FRONT_MAIN for boats???)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x5DFA02 | 74 7C       | jz  short loc_5DFA80
// >>> 0x5DFA04 | 83 7F 3C 05 | cmp dword ptr [edi+3Ch], 5
// >>> 0x5DFA08 | 8B 7F 5C    | mov edi, [edi+5Ch]
//     0x5DFA0B | 74 03       | jz  short loc_5DFA10
#define HOOKPOS_CPed_SetPedPositionInCar_3               0x5DFA04
#define HOOKSIZE_CPed_SetPedPositionInCar_3              7
static const DWORD CONTINUE_CPed_SetPedPositionInCar_3 = 0x5DFA0B;

static void _declspec(naked) HOOK_CPed_SetPedPositionInCar_3()
{
    _asm
    {
        pushad
        mov     eax, [esi+58Ch]
        push    eax            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        cmp     dword ptr [edi+3Ch], 5
        mov     edi, vehicleDummiesPositionArray
        jmp     CONTINUE_CPed_SetPedPositionInCar_3

        continueWithOriginalCode:
        popad
        cmp     dword ptr [edi+3Ch], 5
        mov     edi, [edi+5Ch]
        jmp     CONTINUE_CPed_SetPedPositionInCar_3
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPed::SetPedPositionInCar (4 of 4)
//
// Required for: Seat position for ped in vehicle (eVehicleDummies::SEAT_REAR)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x5DFA7E | 75 84       | jnz short loc_5DFA04
// >>> 0x5DFA80 | 8B 57 5C    | mov edx, [edi+5Ch]
// >>> 0x5DFA83 | 83 C2 3C    | add edx, 3Ch
//     0x5DFA86 | 8B 02       | mov eax, [edx]
#define HOOKPOS_CPed_SetPedPositionInCar_4               0x5DFA80
#define HOOKSIZE_CPed_SetPedPositionInCar_4              6
static const DWORD CONTINUE_CPed_SetPedPositionInCar_4 = 0x5DFA86;

static void _declspec(naked) HOOK_CPed_SetPedPositionInCar_4()
{
    _asm
    {
        pushad
        mov     eax, [esi+58Ch]
        push    eax            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     edx, vehicleDummiesPositionArray
        add     edx, 3Ch
        jmp     CONTINUE_CPed_SetPedPositionInCar_4

        continueWithOriginalCode:
        popad
        mov     edx, [edi+5Ch]
        add     edx, 3Ch
        jmp     CONTINUE_CPed_SetPedPositionInCar_4
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoHeadLightEffect
//
// Required for: Position for vehicle head light coronas (eVehicleDummies::LIGHT_FRONT_MAIN)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6E0A62 | 8B 04 85 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6E0A69 | 8B 50 5C             | mov edx, [eax+5Ch]
// >>> 0x6E0A6C | 8D 0C 5B             | lea ecx, [ebx+ebx*2]
//     0x6E0A6F | 8D 04 CA             | lea eax, [edx+ecx*8]
#define HOOKPOS_CVehicle_DoHeadLightEffect               0x6E0A69
#define HOOKSIZE_CVehicle_DoHeadLightEffect              6
static const DWORD CONTINUE_CVehicle_DoHeadLightEffect = 0x6E0A6F;

static void _declspec(naked) HOOK_CVehicle_DoHeadLightEffect()
{
    _asm
    {
        pushad
        push    ecx            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     edx, vehicleDummiesPositionArray
        lea     ecx, [ebx+ebx*2]
        jmp     CONTINUE_CVehicle_DoHeadLightEffect

        continueWithOriginalCode:
        popad
        mov     edx, [eax+5Ch]
        lea     ecx, [ebx+ebx*2]
        jmp     CONTINUE_CVehicle_DoHeadLightEffect
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoTailLightEffect
//
// Required for: Position for vehicle tail light coronas (eVehicleDummies::LIGHT_REAR_MAIN)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6E17B9 | 8B 04 85 C8 B0 A9 00 | mov  eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6E17C0 | 8B 50 5C             | mov  edx, [eax+5Ch]
// >>> 0x6E17C3 | 53                   | push ebx
// >>> 0x6E17C4 | 8B 5C 24 34          | mov  ebx, [esp+4+30h]
//     0x6E17C8 | 83 FB 01             | cmp  ebx, 1
#define HOOKPOS_CVehicle_DoTailLightEffect               0x6E17C0
#define HOOKSIZE_CVehicle_DoTailLightEffect              8
static const DWORD CONTINUE_CVehicle_DoTailLightEffect = 0x6E17C8;

static void _declspec(naked) HOOK_CVehicle_DoTailLightEffect()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     edx, vehicleDummiesPositionArray
        push    ebx
        mov     ebx, [esp+4+30h]
        jmp     CONTINUE_CVehicle_DoTailLightEffect

        continueWithOriginalCode:
        popad
        mov     edx, [eax+5Ch]
        push    ebx
        mov     ebx, [esp+4+30h]
        jmp     CONTINUE_CVehicle_DoTailLightEffect
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoHeadLightReflectionSingle
//
// Required for: Position for car light shadow (eVehicleDummies::LIGHT_FRONT_MAIN)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6E144B | 8B 04 85 C8 B0 A9 00 | mov  eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6E1452 | 8B 50 5C             | mov  edx, [eax+5Ch]
// >>> 0x6E1455 | 8B 02                | mov  eax, [edx]
//     0x6E1457 | 89 44 24 18          | mov  [esp+24h+var_C], eax
#define HOOKPOS_CVehicle_DoHeadLightReflectionSingle               0x6E1452
#define HOOKSIZE_CVehicle_DoHeadLightReflectionSingle              5
static const DWORD CONTINUE_CVehicle_DoHeadLightReflectionSingle = 0x6E1457;

static void _declspec(naked) HOOK_CVehicle_DoHeadLightReflectionSingle()
{
    _asm
    {
        pushad
        push    ecx            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     edx, vehicleDummiesPositionArray
        mov     eax, [edx]
        jmp     CONTINUE_CVehicle_DoHeadLightReflectionSingle

        continueWithOriginalCode:
        popad
        mov     edx, [eax+5Ch]
        mov     eax, [edx]
        jmp     CONTINUE_CVehicle_DoHeadLightReflectionSingle
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoHeadLightReflectionTwin
//
// Required for: Position for car light shadow (eVehicleDummies::LIGHT_FRONT_MAIN)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6E1607 | 8B 04 85 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6E160E | 8B 50 5C             | mov edx, [eax+5Ch]
// >>> 0x6E1611 | 8B 02                | mov eax, [edx]
//     0x6E1613 | 89 44 24 10          | mov [esp+1Ch+var_C], eax
#define HOOKPOS_CVehicle_DoHeadLightReflectionTwin               0x6E160E
#define HOOKSIZE_CVehicle_DoHeadLightReflectionTwin              5
static const DWORD CONTINUE_CVehicle_DoHeadLightReflectionTwin = 0x6E1613;

static void _declspec(naked) HOOK_CVehicle_DoHeadLightReflectionTwin()
{
    _asm
    {
        pushad
        push    ecx            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     edx, vehicleDummiesPositionArray
        mov     eax, [edx]
        jmp     CONTINUE_CVehicle_DoHeadLightReflectionTwin

        continueWithOriginalCode:
        popad
        mov     edx, [eax+5Ch]
        mov     eax, [edx]
        jmp     CONTINUE_CVehicle_DoHeadLightReflectionTwin
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::GetPlaneGunsPosition
//
// Required for: Plane gun position (eVehicleDummies::VEH_GUN)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6D4293 | 66 8B 51 22          | mov   dx, [ecx+22h]
// >>> 0x6D4297 | 0F BF CA             | movsx ecx, dx
// >>> 0x6D429A | 8B 04 8D C8 B0 A9 00 | mov   eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6D42A1 | 8B 40 5C             | mov   eax, [eax+5Ch]
//     0x6D42A4 | 05 9C 00 00 00       | add   eax, 9Ch
#define HOOKPOS_CVehicle_GetPlaneGunsPosition               0x6D4297
#define HOOKSIZE_CVehicle_GetPlaneGunsPosition              13
static const DWORD CONTINUE_CVehicle_GetPlaneGunsPosition = 0x6D42A4;

static void _declspec(naked) HOOK_CVehicle_GetPlaneGunsPosition()
{
    _asm
    {
        pushad
        push    ecx            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        movsx   ecx, dx
        mov     eax, CModelInfo__ms_modelInfoPtrs
        mov     eax, vehicleDummiesPositionArray
        jmp     CONTINUE_CVehicle_GetPlaneGunsPosition

        continueWithOriginalCode:
        popad
        movsx   ecx, dx
        mov     eax, CModelInfo__ms_modelInfoPtrs
        mov     eax, [eax+5Ch]
        jmp     CONTINUE_CVehicle_GetPlaneGunsPosition
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::GetPlaneOrdnancePosition
//
// Required for: Plane ordnance position (eVehicleDummies::VEH_GUN)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6D46E9 | 8B 04 BD C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6D46F0 | 8B 40 5C             | mov eax, [eax+5Ch]
// >>> 0x6D46F3 | 05 9C 00 00 00       | add eax, 9Ch
//     0x6D46F8 | 8B 08                | mov ecx, [eax]
#define HOOKPOS_CVehicle_GetPlaneOrdnancePosition               0x6D46F0
#define HOOKSIZE_CVehicle_GetPlaneOrdnancePosition              8
static const DWORD CONTINUE_CVehicle_GetPlaneOrdnancePosition = 0x6D46F8;

static void _declspec(naked) HOOK_CVehicle_GetPlaneOrdnancePosition()
{
    _asm
    {
        pushad
        push    ecx            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     eax, vehicleDummiesPositionArray
        add     eax, 9Ch
        jmp     CONTINUE_CVehicle_GetPlaneOrdnancePosition

        continueWithOriginalCode:
        popad
        mov     eax, [eax+5Ch]
        add     eax, 9Ch
        jmp     CONTINUE_CVehicle_GetPlaneOrdnancePosition
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::CanBeDriven
//
// Required for: Unknown
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6D5431 | 8B 04 85 C8 B0 A9 00 | mov eax, CModelInfo::ms_modelInfoPtrs
// >>> 0x6D5438 | 83 78 3C 05          | cmp dword ptr [eax+3Ch], 5
// >>> 0x6D543C | 8B 40 5C             | mov eax, [eax+5Ch]
//     0x6D543F | 74 03                | jz  short loc_6D5444
#define HOOKPOS_CVehicle_CanBeDriven               0x6D5438
#define HOOKSIZE_CVehicle_CanBeDriven              7
static const DWORD CONTINUE_CVehicle_CanBeDriven = 0x6D543F;

static void _declspec(naked) HOOK_CVehicle_CanBeDriven()
{
    _asm
    {
        pushad
        push    ecx            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        cmp     dword ptr [eax+3Ch], 5
        mov     eax, vehicleDummiesPositionArray
        jmp     CONTINUE_CVehicle_CanBeDriven

        continueWithOriginalCode:
        popad
        cmp     dword ptr [eax+3Ch], 5
        mov     eax, [eax+5Ch]
        jmp     CONTINUE_CVehicle_CanBeDriven
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPlane::PreRender (1 of 3)
//
// Required for: Unknown (CPlane might be using a different enum for dummies)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6C9716 | E8 95 24 ED FF    | call CMatrix::UpdateRW
// >>> 0x6C971B | 8B 4D 5C          | mov  ecx, [ebp+5Ch]
// >>> 0x6C971E | 81 C1 84 00 00 00 | add  ecx, 84h
//     0x6C9724 | 8B 11             | mov  edx, [ecx]
#define HOOKPOS_CPlane_PreRender_1               0x6C971B
#define HOOKSIZE_CPlane_PreRender_1              9
static const DWORD CONTINUE_CPlane_PreRender_1 = 0x6C9724;

static void _declspec(naked) HOOK_CPlane_PreRender_1()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     ecx, vehicleDummiesPositionArray
        add     ecx, 84h
        jmp     CONTINUE_CPlane_PreRender_1

        continueWithOriginalCode:
        popad
        mov     ecx, [ebp+5Ch]
        add     ecx, 84h
        jmp     CONTINUE_CPlane_PreRender_1
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPlane::PreRender (2 of 3)
//
// Required for: Unknown (CPlane might be using a different enum for dummies)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6C98C4 | E8 E7 22 ED FF | call CMatrix::UpdateRW
// >>> 0x6C98C9 | 8B 45 5C       | mov  eax, [ebp+5Ch]
// >>> 0x6C98CC | 83 C0 6C       | add  eax, 6Ch
//     0x6C98CF | 8B 08          | mov  ecx, [eax]
#define HOOKPOS_CPlane_PreRender_2               0x6C98C9
#define HOOKSIZE_CPlane_PreRender_2              6
static const DWORD CONTINUE_CPlane_PreRender_2 = 0x6C98CF;

static void _declspec(naked) HOOK_CPlane_PreRender_2()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     eax, vehicleDummiesPositionArray
        add     eax, 6Ch
        jmp     CONTINUE_CPlane_PreRender_2

        continueWithOriginalCode:
        popad
        mov     eax, [ebp+5Ch]
        add     eax, 6Ch
        jmp     CONTINUE_CPlane_PreRender_2
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CPlane::PreRender (3 of 3)
//
// Required for: Unknown (CPlane might be using a different enum for dummies)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6C9B51 | E8 5A 20 ED FF | call CMatrix::UpdateRW
// >>> 0x6C9B56 | 8B 45 5C       | mov  eax, [ebp+5Ch]
// >>> 0x6C9B59 | 83 C0 78       | add  eax, 78h
//     0x6C9B5C | 8B 08          | mov  ecx, [eax]
#define HOOKPOS_CPlane_PreRender_3               0x6C9B56
#define HOOKSIZE_CPlane_PreRender_3              6
static const DWORD CONTINUE_CPlane_PreRender_3 = 0x6C9B5C;

static void _declspec(naked) HOOK_CPlane_PreRender_3()
{
    _asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     eax, vehicleDummiesPositionArray
        add     eax, 78h
        jmp     CONTINUE_CPlane_PreRender_3

        continueWithOriginalCode:
        popad
        mov     eax, [ebp+5Ch]
        add     eax, 78h
        jmp     CONTINUE_CPlane_PreRender_3
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoHeadLightBeam
//
// Required for: Vehicle light beam start position (eVehicleDummies::LIGHT_FRONT_MAIN and ::LIGHT_FRONT_SECONDARY)
//
//////////////////////////////////////////////////////////////////////////////////////////
//     0x6E0E2D | 8B 0C 85 C8 B0 A9 00 | mov ecx, CModelInfo::ms_modelInfoPtrs
// >>> 0x6E0E34 | 8B 49 5C             | mov ecx, [ecx+5Ch]
// >>> 0x6E0E37 | 8B 84 24 9C 00 00 00 | mov eax, [esp+98h+arg_0]
//     0x6E0E3E | 83 F8 01             | cmp eax, 1
#define HOOKPOS_CVehicle_DoHeadLightBeam               0x6E0E34
#define HOOKSIZE_CVehicle_DoHeadLightBeam              10
static const DWORD CONTINUE_CVehicle_DoHeadLightBeam = 0x6E0E3E;

static void _declspec(naked) HOOK_CVehicle_DoHeadLightBeam()
{
    _asm
    {
        pushad
        push    edi            // CVehicleSAInterface*
        call    UpdateVehicleDummiesPositionArray
        add     esp, 4

        mov     eax, vehicleDummiesPositionArray
        test    eax, eax
        jz      continueWithOriginalCode

        popad
        mov     ecx, vehicleDummiesPositionArray
        mov     eax, [esp+98h+4h]
        jmp     CONTINUE_CVehicle_DoHeadLightBeam

        continueWithOriginalCode:
        popad
        mov     ecx, [ecx+5Ch]
        mov     eax, [esp+98h+4h]
        jmp     CONTINUE_CVehicle_DoHeadLightBeam
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_VehicleDummies
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_VehicleDummies()
{
    EZHookInstall(CVehicle_DoVehicleLights_1);
    EZHookInstall(CVehicle_DoVehicleLights_2);
    EZHookInstall(CVehicle_DoHeadLightBeam);
    EZHookInstall(CVehicle_DoHeadLightEffect);
    EZHookInstall(CVehicle_DoTailLightEffect);
    EZHookInstall(CVehicle_DoHeadLightReflectionSingle);
    EZHookInstall(CVehicle_DoHeadLightReflectionTwin);
    EZHookInstall(CVehicle_GetPlaneGunsPosition);
    EZHookInstall(CVehicle_GetPlaneOrdnancePosition);
    EZHookInstall(CVehicle_CanBeDriven);
    EZHookInstall(CVehicle_AddExhaustParticles_1);
    EZHookInstall(CVehicle_AddExhaustParticles_2);
    EZHookInstall(CVehicle_AddDamagedVehicleParticles);
    EZHookInstall(CAutomobile_DoNitroEffect_1);
    EZHookInstall(CAutomobile_DoNitroEffect_2);
    EZHookInstall(CAutomobile_ProcessCarOnFireAndExplode);
    EZHookInstall(CBike_FixHandsToBars);
    // EZHookInstall(CPlane_PreRender_1); Only changes plane wing orientation in weird ways
    // EZHookInstall(CPlane_PreRender_2);
    // EZHookInstall(CPlane_PreRender_3);
    // EZHookInstall(CFire_ProcessFire); // No visible effect
    EZHookInstall(CPed_SetPedPositionInCar_1);
    EZHookInstall(CPed_SetPedPositionInCar_2);
    EZHookInstall(CPed_SetPedPositionInCar_3);
    EZHookInstall(CPed_SetPedPositionInCar_4);
}
