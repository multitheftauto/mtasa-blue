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
    uint isCameraFacingCorona = false;

    CVehicleSAInterface* lightsVehicleInterface = nullptr;
    CVehicleSAInterface* headLightBeamVehicleInterface = nullptr;
    bool                 headLightBeamIsRight = false;
    std::uint8_t         headLightEffectSide = 0;
    std::uint8_t         reflectionSide = 0;
    RwVertex*            headLightVerts = nullptr;
    unsigned int         headLightNumVerts = 0;
    unsigned long        headLightColorR = 0;
    unsigned long        headLightColorG = 0;
    unsigned long        headLightColorB = 0;
}  // namespace

static void __cdecl GetVehicleHeadLightColor(CVehicleSAInterface* vehicleInterface, unsigned int sideValue, float redFactor, float greenFactor,
                                             float blueFactor)
{
    SColor                     color = SColorRGBA(255, 255, 255, 255);
    SClientEntity<CVehicleSA>* vehicleClientEntity = pGameInterface->GetPools()->GetVehicle(reinterpret_cast<DWORD*>(vehicleInterface));
    CVehicle*                  vehicle = vehicleClientEntity ? vehicleClientEntity->pEntity : nullptr;
    if (vehicle)
    {
        color = vehicle->GetHeadLightColor(sideValue ? HeadlightSide::Right : HeadlightSide::Left);
    }

    // Scale color values to the defaults
    headLightColorR = static_cast<unsigned char>(std::min(255.0f, color.R * (1.0f / 255.0f) * redFactor));
    headLightColorG = static_cast<unsigned char>(std::min(255.0f, color.G * (1.0f / 255.0f) * greenFactor));
    headLightColorB = static_cast<unsigned char>(std::min(255.0f, color.B * (1.0f / 255.0f) * blueFactor));
}

static void ProcessHeadLightBeam()
{
    GetVehicleHeadLightColor(headLightBeamVehicleInterface, headLightBeamIsRight ? 1 : 0, 255.0f, 255.0f, 255.0f);

    for (unsigned int i = 0; i < headLightNumVerts; i++)
    {
        unsigned char alpha = COLOR_ARGB_A(headLightVerts[i].color);
        headLightVerts[i].color = COLOR_ARGB(alpha, static_cast<unsigned char>(headLightColorR), static_cast<unsigned char>(headLightColorG),
                                             static_cast<unsigned char>(headLightColorB));
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
        mov     isCameraFacingCorona, eax
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

        mov     eax, isCameraFacingCorona
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
        mov     lightsVehicleInterface, ecx
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
// Captures headlight beam vehicle interface, side (left/right), and applies custom vertex colors
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
        mov     headLightBeamVehicleInterface, ecx
        mov     al, byte ptr [esp+0Ch]
        mov     headLightBeamIsRight, al
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
        mov     headLightVerts, eax
        mov     eax, [esp+4]
        mov     headLightNumVerts, eax
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
// Captures light side at entry and applies custom vehicle headlight corona colors for individual left/right sides
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CVehicle__DoHeadLightEffect_Entry  0x6E0A50
#define HOOKSIZE_CVehicle__DoHeadLightEffect_Entry 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightEffect_Entry = 0x6E0A58;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightEffect_Entry()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     lightsVehicleInterface, ecx
        mov     al, byte ptr [esp+0Ch]      // a4: lightId (0 = Left, 1 = Right)
        mov     headLightEffectSide, al
        sub     esp, 24h
        push    ebx
        mov     ebx, [esp+2Ch]
        jmp     CONTINUE_CVehicle__DoHeadLightEffect_Entry
    }
    // clang-format on
}

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
        push    430C0000h                   // 140.0f
        push    43200000h                   // 160.0f
        push    43200000h                   // 160.0f
        movzx   eax, headLightEffectSide
        push    eax                         // sideValue (0 = left, 1 = right)
        push    lightsVehicleInterface
        call    GetVehicleHeadLightColor
        add     esp, 14h
        popad

        mov     eax, headLightColorR
        mov     [esp+8], eax
        mov     eax, headLightColorG
        mov     [esp+12], eax
        mov     eax, headLightColorB
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
        push    43430000h                   // 195.0f
        push    43520000h                   // 210.0f
        push    43520000h                   // 210.0f
        movzx   eax, headLightEffectSide
        push    eax                         // sideValue (0 = left, 1 = right)
        push    lightsVehicleInterface
        call    GetVehicleHeadLightColor
        add     esp, 14h
        popad

        mov     eax, headLightColorR
        mov     [esp+8], eax
        mov     eax, headLightColorG
        mov     [esp+12], eax
        mov     eax, headLightColorB
        mov     [esp+16], eax

        call    CALL_CCoronas__RegisterCorona
        add     esp, 54h
        jmp     CONTINUE_CVehicle__DoHeadLightEffect_2
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoVehicleLights front headlamp bulbs
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CVehicle__DoVehicleLights_RightCorona_1  0x6E219F
#define HOOKSIZE_CVehicle__DoVehicleLights_RightCorona_1 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoVehicleLights_RightCorona_1 = 0x6E21A7;
static void __declspec(naked)   HOOK_CVehicle__DoVehicleLights_RightCorona_1()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    430C0000h                   // 140.0f
        push    43200000h                   // 160.0f
        push    43200000h                   // 160.0f
        push    1                           // sideValue = 1 (Right)
        push    lightsVehicleInterface
        call    GetVehicleHeadLightColor
        add     esp, 14h
        popad

        mov     eax, headLightColorR
        mov     [esp+8], eax
        mov     eax, headLightColorG
        mov     [esp+12], eax
        mov     eax, headLightColorB
        mov     [esp+16], eax

        call    CALL_CCoronas__RegisterCorona
        add     esp, 54h
        jmp     CONTINUE_CVehicle__DoVehicleLights_RightCorona_1
    }
    // clang-format on
}

#define HOOKPOS_CVehicle__DoVehicleLights_RightCorona_2  0x6E2297
#define HOOKSIZE_CVehicle__DoVehicleLights_RightCorona_2 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoVehicleLights_RightCorona_2 = 0x6E229F;
static void __declspec(naked)   HOOK_CVehicle__DoVehicleLights_RightCorona_2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    43430000h                   // 195.0f
        push    43520000h                   // 210.0f
        push    43520000h                   // 210.0f
        push    1                           // sideValue = 1 (Right)
        push    lightsVehicleInterface
        call    GetVehicleHeadLightColor
        add     esp, 14h
        popad

        mov     eax, headLightColorR
        mov     [esp+8], eax
        mov     eax, headLightColorG
        mov     [esp+12], eax
        mov     eax, headLightColorB
        mov     [esp+16], eax

        call    CALL_CCoronas__RegisterCorona
        add     esp, 54h
        jmp     CONTINUE_CVehicle__DoVehicleLights_RightCorona_2
    }
    // clang-format on
}

#define HOOKPOS_CVehicle__DoVehicleLights_LeftCorona_1  0x6E253E
#define HOOKSIZE_CVehicle__DoVehicleLights_LeftCorona_1 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoVehicleLights_LeftCorona_1 = 0x6E2546;
static void __declspec(naked)   HOOK_CVehicle__DoVehicleLights_LeftCorona_1()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    430C0000h                   // 140.0f
        push    43200000h                   // 160.0f
        push    43200000h                   // 160.0f
        push    0                           // sideValue = 0 (Left)
        push    lightsVehicleInterface
        call    GetVehicleHeadLightColor
        add     esp, 14h
        popad

        mov     eax, headLightColorR
        mov     [esp+8], eax
        mov     eax, headLightColorG
        mov     [esp+12], eax
        mov     eax, headLightColorB
        mov     [esp+16], eax

        call    CALL_CCoronas__RegisterCorona
        add     esp, 54h
        jmp     CONTINUE_CVehicle__DoVehicleLights_LeftCorona_1
    }
    // clang-format on
}

#define HOOKPOS_CVehicle__DoVehicleLights_LeftCorona_2  0x6E2633
#define HOOKSIZE_CVehicle__DoVehicleLights_LeftCorona_2 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoVehicleLights_LeftCorona_2 = 0x6E263B;
static void __declspec(naked)   HOOK_CVehicle__DoVehicleLights_LeftCorona_2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    43430000h                   // 195.0f
        push    43520000h                   // 210.0f
        push    43520000h                   // 210.0f
        push    0                           // sideValue = 0 (Left)
        push    lightsVehicleInterface
        call    GetVehicleHeadLightColor
        add     esp, 14h
        popad

        mov     eax, headLightColorR
        mov     [esp+8], eax
        mov     eax, headLightColorG
        mov     [esp+12], eax
        mov     eax, headLightColorB
        mov     [esp+16], eax

        call    CALL_CCoronas__RegisterCorona
        add     esp, 54h
        jmp     CONTINUE_CVehicle__DoVehicleLights_LeftCorona_2
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicle::DoHeadLightReflection
//
// Applies custom vehicle headlight shadow reflection colors for individual left and right lights.
// Intercepts twin reflections to render two separate left/right ground reflections.
//
//////////////////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CVehicle__DoHeadLightReflectionTwin  0x6E1600
#define HOOKSIZE_CVehicle__DoHeadLightReflectionTwin 7
static constexpr std::uintptr_t CALL_CVehicle__DoHeadLightReflectionSingle = 0x6E1440;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightReflectionTwin()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    esi
        push    edi
        mov     esi, ecx                    // this (CVehicle*)
        mov     edi, [esp+0Ch]              // matrix (RwMatrixTag*)

        // 1. Draw Left Headlight Reflection
        push    0                           // bRight = 0 (Left)
        push    edi                         // matrix
        mov     ecx, esi                    // this
        call    CALL_CVehicle__DoHeadLightReflectionSingle

        // 2. Draw Right Headlight Reflection
        push    1                           // bRight = 1 (Right)
        push    edi                         // matrix
        mov     ecx, esi                    // this
        call    CALL_CVehicle__DoHeadLightReflectionSingle

        pop     edi
        pop     esi
        retn    4
    }
    // clang-format on
}

#define HOOKPOS_CVehicle__DoHeadLightReflectionSingle_Entry  0x6E1440
#define HOOKSIZE_CVehicle__DoHeadLightReflectionSingle_Entry 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightReflectionSingle_Entry = 0x6E1448;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightReflectionSingle_Entry()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     lightsVehicleInterface, ecx
        mov     al, byte ptr [esp+8]        // bRight (0 = Left, 1 = Right)
        mov     reflectionSide, al
        sub     esp, 20h
        push    esi
        mov     si, word ptr [ecx+22h]
        jmp     CONTINUE_CVehicle__DoHeadLightReflectionSingle_Entry
    }
    // clang-format on
}

#define HOOKPOS_CVehicle__DoHeadLightReflectionSingle  0x6E15E2
#define HOOKSIZE_CVehicle__DoHeadLightReflectionSingle 8
static constexpr std::uintptr_t CONTINUE_CVehicle__DoHeadLightReflectionSingle = 0x6E15EA;
static constexpr std::uintptr_t CALL_CShadows__StoreCarLightShadow = 0x70C500;
static void __declspec(naked)   HOOK_CVehicle__DoHeadLightReflectionSingle()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    42340000h                   // 45.0f
        push    42340000h                   // 45.0f
        push    42340000h                   // 45.0f
        movzx   eax, reflectionSide
        push    eax                         // sideValue (0 = left, 1 = right)
        push    lightsVehicleInterface
        call    GetVehicleHeadLightColor
        add     esp, 14h
        popad

        // If Right side (reflectionSide == 1), differentiate shadow ID at [esp+4] so CShadows doesn't overwrite Left shadow!
        mov     al, reflectionSide
        test    al, al
        jz      left_shadow
        inc     dword ptr [esp+4]           // Change shadow ID for right light: &m_matrix + 3 instead of + 2

left_shadow:
        mov     eax, headLightColorR
        mov     [esp+32], eax
        mov     eax, headLightColorG
        mov     [esp+36], eax
        mov     eax, headLightColorB
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
    EZHookInstall(CVehicle__DoHeadLightEffect_Entry);
    EZHookInstall(CVehicle__DoHeadLightEffect_1);
    EZHookInstall(CVehicle__DoHeadLightEffect_2);
    EZHookInstall(CVehicle__DoVehicleLights_RightCorona_1);
    EZHookInstall(CVehicle__DoVehicleLights_RightCorona_2);
    EZHookInstall(CVehicle__DoVehicleLights_LeftCorona_1);
    EZHookInstall(CVehicle__DoVehicleLights_LeftCorona_2);
    EZHookInstall(CVehicle__DoHeadLightReflectionTwin);
    EZHookInstall(CVehicle__DoHeadLightReflectionSingle_Entry);
    EZHookInstall(CVehicle__DoHeadLightReflectionSingle);

    // Allow turning on vehicle lights even if the engine is off
    MemSet(reinterpret_cast<void*>(0x6E1DBC), 0x90, 8);

    // Fix vehicle back lights both using light state 3 (SA bug)
    MemPut<BYTE>(0x6E1D4F, 2);
}
