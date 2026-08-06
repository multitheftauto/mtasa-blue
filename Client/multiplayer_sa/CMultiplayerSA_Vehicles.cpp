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
#include <algorithm>
#include <cmath>

static bool __fastcall AreVehicleDoorsUndamageable(CVehicleSAInterface* vehicle)
{
    SClientEntity<CVehicleSA>* pair = pGameInterface->GetPools()->GetVehicle((DWORD*)vehicle);

    if (!pair)
        return false;

    return pair->pEntity->AreDoorsUndamageable();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CDamageManager::ProgressDoorDamage
//
// This hook checks if our CVehicleSA instance prevents door damage
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6C2320 | 53             | push    ebx
//     0x6C2321 | 56             | push    esi
//     0x6C2322 | 0F B6 74 24 0C | movzx   esi, [esp + doorId]
//     0x6C2327 | 85 F6          | test    esi, esi
#define HOOKPOS_CDamageManager__ProgressDoorDamage  0x6C2320
#define HOOKSIZE_CDamageManager__ProgressDoorDamage 7
static DWORD CONTINUE_CDamageManager__ProgressDoorDamage = 0x6C2327;

static void __declspec(naked) HOOK_CDamageManager__ProgressDoorDamage()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        mov     ecx, [esp + 08h]        // CAutomobileSAInterface*
        call    AreVehicleDoorsUndamageable
        test    al, al
        jz      continueGameCodeLocation

        popad
        mov     al, 0
        retn    8

        continueGameCodeLocation:
        popad
        push    ebx
        push    esi
        movzx   esi, [esp + 0Ch]
        jmp     CONTINUE_CDamageManager__ProgressDoorDamage
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::HydraulicControl
//
// This hook gives monster trucks working hydraulics instead of letting the native control wreck
// their suspension.
//
// HydraulicControl runs every tick for any vehicle carrying the hydraulics upgrade, monster trucks
// included, and it knows nothing about them. It rewrites the suspension spring and line lengths for
// an ordinary car's suspension range, derived from the wheel radius and the handling limits, while
// CMonsterTruck keeps a suspension model entirely of its own. CMonsterTruck::PreRender then clamps
// its wheel position against the very spring length field the native control just overwrote, so the
// two fight over it every frame and the wheels settle wherever that unrelated math left them.
//
// Monster trucks are told apart by their vtable pointer rather than a handling bitfield, since MTA
// maps none of CMonsterTruck's own suspension fields to read one from.
//
//////////////////////////////////////////////////////////////////////////////////////////
// >>> 0x6A07A0 | 83 EC 60             | sub     esp, 0x60
//     0x6A07A3 | 55                   | push    ebp
//     0x6A07A4 | 8B E9                | mov     ebp, ecx
//     0x6A07A6 | 0F BF 45 22          | movsx   eax, word ptr [ebp + 0x22]
#define HOOKPOS_CAutomobile__HydraulicControl  0x6A07A0
#define HOOKSIZE_CAutomobile__HydraulicControl 6
static DWORD CONTINUE_CAutomobile__HydraulicControl = 0x6A07A6;

#define MONSTERTRUCK_VTABLE 0x8717D8

// CAutomobile's m_aSuspensionSpringLength and m_aSuspensionLineLength, four floats each.
// CMonsterTruck repurposes them as the travel limits of m_wheelPosition: SetupSuspensionLines fills
// them with the wheel's Z plus the handling suspension upper and lower limit respectively, and
// ExtendSuspension clamps m_wheelPosition into the range they describe. MTA's CAutomobileSAInterface
// splits that region into field_878, wheelOffsetZ and field_88C without recognising the two arrays,
// so they are addressed here by offset.
#define MONSTERTRUCK_SUSPENSION_UPPERLIMIT 0x878
#define MONSTERTRUCK_SUSPENSION_LOWERLIMIT 0x888

// Value m_wMiscComponentAngle carries while the hydraulics are raised, matching what the native
// control parks there for the same purpose. The only monster truck that puts the field to another
// use is the dumper, and CAutomobile::ProcessControl sends that one to UpdateMovingCollision instead
// of ever reaching hydraulics.
static constexpr std::uint16_t MONSTERTRUCK_HYDRAULICS_RAISED = 500;

// Fraction of the truck's own suspension travel a fully raised wheel gives up. The native control
// shifts a car by a flat 0.2, roughly half the travel of a stock car; taking a proportion instead
// keeps the lift in scale with a monster truck's far longer suspension.
static constexpr float MONSTERTRUCK_HYDRAULICS_RAISE_RATIO = 0.5f;

static const std::uint32_t CModelInfo__ms_modelInfoPtrs = 0xA9B0C8;
static const std::uint32_t FUNC_CVehicleModelInfo__GetWheelPosn = 0x4C7D20;

// Recreates the native control scheme against CMonsterTruck's own suspension, so the horn raises and
// lowers the truck, the hydraulic jump button hops every wheel at once and the right stick tilts it.
// Only ever reached for the vehicle the local player is driving, matching the STATUS_PLAYER gate the
// native control applies before it reads any pad input.
static void __fastcall MonsterTruckHydraulicControl(CAutomobileSAInterface* vehicle)
{
    if (!vehicle || vehicle->nStatus != STATUS_PLAYER)
        return;

    const auto* handling = vehicle->pHandlingData;
    if (!handling)
        return;

    auto* modelInfo = reinterpret_cast<void**>(CModelInfo__ms_modelInfoPtrs)[vehicle->m_nModelIndex];
    if (!modelInfo)
        return;

    auto*            pad = pGameInterface->GetPad();
    CControllerState state, lastState;
    pad->GetCurrentControllerState(&state);
    pad->GetLastControllerState(&lastState);

    // The horn is the raise and lower toggle, which is what the native control reads through
    // CPad::HornJustDown; in the default control mode that button is ShockButtonL, the one MTA maps
    // its horn control to. The game already silences the horn itself on any vehicle carrying the
    // upgrade, so without this the key would do nothing at all.
    if (state.ShockButtonL && !lastState.ShockButtonL)
        vehicle->m_wMiscComponentAngle = vehicle->m_wMiscComponentAngle < MONSTERTRUCK_HYDRAULICS_RAISED ? MONSTERTRUCK_HYDRAULICS_RAISED : 0;

    const bool raised = vehicle->m_wMiscComponentAngle >= MONSTERTRUCK_HYDRAULICS_RAISED;

    float suspensionChange[MAX_WHEELS] = {};

    if (state.ShockButtonR)
        for (float& change : suspensionChange)
            change = 1.0f;
    else if (state.RightStickX != 0 || state.RightStickY != 0)
    {
        // The diagonal tilt the native control derives from this same stick, read straight from
        // CControllerState rather than through CPad::GetCarGunLeftRight and GetCarGunUpDown, which
        // return these two fields anyway in every common control mode.
        const float leftRight = static_cast<float>(state.RightStickX);
        const float upDown = static_cast<float>(state.RightStickY);
        const float factor = std::sqrt(upDown * upDown + leftRight * leftRight) * 1.5f / 128.0f;
        const float angle = std::atan2(leftRight, upDown) - PI / 4.0f;
        const float rearRight = std::cos(angle) * factor;
        const float frontRight = std::sin(angle) * factor;

        suspensionChange[FRONT_LEFT_WHEEL] = std::max(0.0f, -rearRight);
        suspensionChange[REAR_LEFT_WHEEL] = std::max(0.0f, -frontRight);
        suspensionChange[FRONT_RIGHT_WHEEL] = std::max(0.0f, frontRight);
        suspensionChange[REAR_RIGHT_WHEEL] = std::max(0.0f, rearRight);
    }

    // Raising slides the whole travel window down relative to the chassis, the same move the native
    // control makes when it drops both of its limits by an equal amount. The spring keeps its length
    // and only the window it works in relocates, so the wheel settles that much further from the
    // chassis and the body rides that much higher. ExtendSuspension clamps m_wheelPosition into the
    // new window before the compression is recomputed from it, and PreRender clamps it once more for
    // rendering, so the physics and the visible wheel both follow the window down.
    const auto  GetWheelPosn = reinterpret_cast<void(__thiscall*)(void*, int, CVector*, bool)>(FUNC_CVehicleModelInfo__GetWheelPosn);
    const float travel = handling->fSuspensionUpperLimit - handling->fSuspensionLowerLimit;
    auto*       base = reinterpret_cast<std::uint8_t*>(vehicle);

    for (int i = 0; i < MAX_WHEELS; i++)
    {
        // Being raised inverts what an input asks for, exactly as it does natively. Lowered, a wheel
        // rests on its own suspension until an input lifts it; raised, every wheel is held up until
        // an input releases one, which is what lets the stick tilt the truck from a raised stance.
        const float change = std::min(suspensionChange[i], 1.0f);
        const float drop = travel * MONSTERTRUCK_HYDRAULICS_RAISE_RATIO * (raised ? 1.0f - change : change);

        // Rebuilding both limits the way SetupSuspensionLines first derived them, from the wheel's Z
        // plus the matching handling limit, keeps this idempotent: a wheel with nothing asked of it
        // is handed back exactly what the game set up, so nothing needs restoring when the player
        // leaves the truck or it gets destroyed.
        CVector wheelPosition;
        GetWheelPosn(modelInfo, i, &wheelPosition, false);

        *reinterpret_cast<float*>(base + MONSTERTRUCK_SUSPENSION_UPPERLIMIT + i * sizeof(float)) = wheelPosition.fZ + handling->fSuspensionUpperLimit - drop;
        *reinterpret_cast<float*>(base + MONSTERTRUCK_SUSPENSION_LOWERLIMIT + i * sizeof(float)) = wheelPosition.fZ + handling->fSuspensionLowerLimit - drop;
    }
}

static void __declspec(naked) HOOK_CAutomobile__HydraulicControl()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     dword ptr [ecx], MONSTERTRUCK_VTABLE
        jne     continueGameCodeLocation

        pushad
        call    MonsterTruckHydraulicControl
        popad
        retn

        continueGameCodeLocation:
        sub     esp, 0x60
        push    ebp
        mov     ebp, ecx
        jmp     CONTINUE_CAutomobile__HydraulicControl
    }
    // clang-format on
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
    EZHookInstall(CDamageManager__ProgressDoorDamage);
    EZHookInstall(CAutomobile__HydraulicControl);
}
