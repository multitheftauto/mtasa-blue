/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_VehicleLights.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

namespace
{
    // Used to save state between CVehicle_DoTailLightEffect_Mid and CVehicle_DoTailLightEffect_Mid2
    uint bCameraFacingCorona = false;

    CVehicleSAInterface* s_pLightsVehicleInterface = nullptr;
    CVehicleSAInterface* s_pHeadLightBeamVehicleInterface = nullptr;
    RwVertex*            s_pHeadLightVerts = nullptr;
    unsigned int         s_uiHeadLightNumVerts = 0;
    unsigned long        s_ulHeadLightR = 0;
    unsigned long        s_ulHeadLightG = 0;
    unsigned long        s_ulHeadLightB = 0;
}  // namespace

static void GetVehicleHeadLightColor(CVehicleSAInterface* vehicleInterface, float r, float g, float b)
{
    SColor                     color = SColorRGBA(255, 255, 255, 255);
    SClientEntity<CVehicleSA>* vehicleClientEntity = pGameInterface->GetPools()->GetVehicle(reinterpret_cast<DWORD*>(vehicleInterface));
    CVehicle*                  vehicle = vehicleClientEntity ? vehicleClientEntity->pEntity : nullptr;
    if (vehicle)
    {
        color = vehicle->GetHeadLightColor();
    }

    // Scale our color values to the defaults
    s_ulHeadLightR = static_cast<unsigned char>(std::min(255.0f, color.R * (1.0f / 255.0f) * r));
    s_ulHeadLightG = static_cast<unsigned char>(std::min(255.0f, color.G * (1.0f / 255.0f) * g));
    s_ulHeadLightB = static_cast<unsigned char>(std::min(255.0f, color.B * (1.0f / 255.0f) * b));
}

static void ProcessHeadLightBeam()
{
    GetVehicleHeadLightColor(s_pHeadLightBeamVehicleInterface, 255.0f, 255.0f, 255.0f);

    for (unsigned int i = 0; i < s_uiHeadLightNumVerts; i++)
    {
        unsigned char alpha = COLOR_ARGB_A(s_pHeadLightVerts[i].color);
        s_pHeadLightVerts[i].color = COLOR_ARGB(alpha, static_cast<unsigned char>(s_ulHeadLightR), static_cast<unsigned char>(s_ulHeadLightG),
                                                static_cast<unsigned char>(s_ulHeadLightB));
    }
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
#define HOOKPOS_CVehicle_DoTailLightEffect_Mid   0x006E18E5
#define HOOKSIZE_CVehicle_DoTailLightEffect_Mid  6
#define HOOKCHECK_CVehicle_DoTailLightEffect_Mid 0x0F
static constexpr std::uintptr_t CONTINUE_CVehicle_DoTailLightEffect_Mid = 0x006E18EB;
static void __declspec(naked)   HOOK_CVehicle_DoTailLightEffect_Mid()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Save result of comparing camera and corona direction
        mov     eax, 0
        jnz     behind_corona
        mov     eax, 1

behind_corona:
        mov     bCameraFacingCorona, eax
        jmp     CONTINUE_CVehicle_DoTailLightEffect_Mid
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoTailLightEffect hook 2
//
// Figure out if the tail light corona should be added
//
//////////////////////////////////////////////////////////////////////////////////////////
// Hook info
#define HOOKPOS_CVehicle_DoTailLightEffect_Mid2   0x006E19E6
#define HOOKSIZE_CVehicle_DoTailLightEffect_Mid2  10
#define HOOKCHECK_CVehicle_DoTailLightEffect_Mid2 0x8B
static constexpr std::uintptr_t CONTINUE_CVehicle_DoTailLightEffect_Mid2 = 0x006E19F0;
static constexpr std::uintptr_t CONTINUE_CVehicle_DoTailLightEffect_Mid2_NoCorona = 0x006E1A32;
static void __declspec(naked)   HOOK_CVehicle_DoTailLightEffect_Mid2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
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
        jmp     CONTINUE_CVehicle_DoTailLightEffect_Mid2

no_corona:
        sub     esp, 54h
        jmp     CONTINUE_CVehicle_DoTailLightEffect_Mid2_NoCorona
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoVehicleLights
//
// Intercepts CVehicle::DoVehicleLights to capture the vehicle interface
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CVehicle__DoVehicleLights  0x6E1A60
#define HOOKSIZE_CVehicle__DoVehicleLights 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoVehicleLights = 0x6E1A68;
static void __declspec(naked)   HOOK_CVehicle__DoVehicleLights()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     s_pLightsVehicleInterface, ecx
        mov     al, byte ptr ds:[00C1CC18h]
        sub     esp, 3Ch
        jmp     CONTINUE_CVehicle__DoVehicleLights
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoHeadLightBeam
//
// Captures headlight beam vehicle interface and applies custom vertex colors
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CVehicle__DoHeadLightBeam_1  0x6E0E20
#define HOOKSIZE_CVehicle__DoHeadLightBeam_1 6
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightBeam_1 = 0x6E0E26;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightBeam_1()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     s_pHeadLightBeamVehicleInterface, ecx
        sub     esp, 94h
        jmp     CONTINUE_CVehicle__DoHeadLightBeam_1
    }
    // clang-format on
}

#define HOOKPOS_CVehicle__DoHeadLightBeam_2  0x6E13A4
#define HOOKSIZE_CVehicle__DoHeadLightBeam_2 10
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightBeam_2 = 0x6E13AE;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightBeam_2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, [esp]
        mov     s_pHeadLightVerts, eax
        mov     eax, [esp+4]
        mov     s_uiHeadLightNumVerts, eax
        pushad
    }
    // clang-format on

    ProcessHeadLightBeam();

    // clang-format off
    __asm
    {
        popad
        mov     dword ptr ds:[0C4B950h], 5
        jmp     CONTINUE_CVehicle__DoHeadLightBeam_2
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoHeadLightEffect
//
// Applies custom vehicle headlight corona colors
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CVehicle__DoHeadLightEffect_1  0x6E0D01
#define HOOKSIZE_CVehicle__DoHeadLightEffect_1 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightEffect_1 = 0x6E0D09;
static constexpr std::uintptr_t CALL_CCoronas__RegisterCorona = 0x6FC580;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightEffect_1()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
    }
    // clang-format on

    GetVehicleHeadLightColor(s_pLightsVehicleInterface, 160.0f, 160.0f, 140.0f);

    // clang-format off
    __asm
    {
        popad
        mov     eax, s_ulHeadLightR
        mov     [esp+8], eax
        mov     eax, s_ulHeadLightG
        mov     [esp+12], eax
        mov     eax, s_ulHeadLightB
        mov     [esp+16], eax

        call    CALL_CCoronas__RegisterCorona
        add     esp, 54h
        jmp     CONTINUE_CVehicle__DoHeadLightEffect_1
    }
    // clang-format on
}

#define HOOKPOS_CVehicle__DoHeadLightEffect_2  0x6E0DF7
#define HOOKSIZE_CVehicle__DoHeadLightEffect_2 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightEffect_2 = 0x6E0DFF;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightEffect_2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
    }
    // clang-format on

    GetVehicleHeadLightColor(s_pLightsVehicleInterface, 160.0f, 160.0f, 140.0f);

    // clang-format off
    __asm
    {
        popad
        mov     eax, s_ulHeadLightR
        mov     [esp+8], eax
        mov     eax, s_ulHeadLightG
        mov     [esp+12], eax
        mov     eax, s_ulHeadLightB
        mov     [esp+16], eax

        call    CALL_CCoronas__RegisterCorona
        add     esp, 54h
        jmp     CONTINUE_CVehicle__DoHeadLightEffect_2
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoHeadLightReflection
//
// Applies custom vehicle headlight shadow reflection colors for twin and single lights
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CVehicle__DoHeadLightReflectionTwin  0x6E170F
#define HOOKSIZE_CVehicle__DoHeadLightReflectionTwin 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightReflectionTwin = 0x6E1717;
static constexpr std::uintptr_t CALL_CShadows__StoreCarLightShadow = 0x70C500;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightReflectionTwin()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
    }
    // clang-format on

    GetVehicleHeadLightColor(s_pLightsVehicleInterface, 45.0f, 45.0f, 45.0f);

    // clang-format off
    __asm
    {
        popad
        mov     eax, s_ulHeadLightR
        mov     [esp+32], eax
        mov     eax, s_ulHeadLightG
        mov     [esp+36], eax
        mov     eax, s_ulHeadLightB
        mov     [esp+40], eax

        call    CALL_CShadows__StoreCarLightShadow
        add     esp, 4Ch
        jmp     CONTINUE_CVehicle__DoHeadLightReflectionTwin
    }
    // clang-format on
}

#define HOOKPOS_CVehicle__DoHeadLightReflectionSingle  0x6E15E2
#define HOOKSIZE_CVehicle__DoHeadLightReflectionSingle 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightReflectionSingle = 0x6E15EA;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightReflectionSingle()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
    }
    // clang-format on

    GetVehicleHeadLightColor(s_pLightsVehicleInterface, 45.0f, 45.0f, 45.0f);

    // clang-format off
    __asm
    {
        popad
        mov     eax, s_ulHeadLightR
        mov     [esp+32], eax
        mov     eax, s_ulHeadLightG
        mov     [esp+36], eax
        mov     eax, s_ulHeadLightB
        mov     [esp+40], eax

        call    CALL_CShadows__StoreCarLightShadow
        add     esp, 30h
        jmp     CONTINUE_CVehicle__DoHeadLightReflectionSingle
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_VehicleLights
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_VehicleLights()
{
    EZHookInstallChecked(CVehicle_DoTailLightEffect_Mid);
    EZHookInstallChecked(CVehicle_DoTailLightEffect_Mid2);
    EZHookInstall(CVehicle__DoVehicleLights);
    EZHookInstall(CVehicle__DoHeadLightBeam_1);
    EZHookInstall(CVehicle__DoHeadLightBeam_2);
    EZHookInstall(CVehicle__DoHeadLightEffect_1);
    EZHookInstall(CVehicle__DoHeadLightEffect_2);
    EZHookInstall(CVehicle__DoHeadLightReflectionTwin);
    EZHookInstall(CVehicle__DoHeadLightReflectionSingle);

    // Allow turning on vehicle lights even if the engine is off
    MemSet(reinterpret_cast<void*>(0x6E1DBC), 0x90, 8);

    // Fix vehicle back lights both using light state 3 (SA bug)
    MemPut<BYTE>(0x6E1D4F, 2);
}
