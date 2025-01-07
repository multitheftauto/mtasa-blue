/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_WidescreenFix.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

static float RadarWidth[23], RadarHeight[23];
static float HUDWidth[39], HUDHeight[34];
static float MiscWidth, MiscHeight;
static float CameraWidth;

static float           ScreenWidthScale = 1.0f;
static constexpr float ScreenHeightScale = 1.0f;

static constexpr float HudWidthScale = 1.0f;
static constexpr float HudHeightScale = 1.0f;
static constexpr float RadarWidthScale = 1.0f;
static constexpr float RadarHeightScale = 1.0f;
/* NOTE: Changing hud and radar scales may misalign elements */

static constexpr float WidthMult = 0.0015625f;
static constexpr float HeightMult = 0.002232143f;

static float* ScreenAspectRatio = reinterpret_cast<float*>(0xC3EFA4);
static float* ScreenFieldOfView = reinterpret_cast<float*>(0x8D5038);

float AdjustFOV(float f, float ar)
{
    return std::round((2.0f * atan(((ar) / (4.0f / 3.0f)) * tan(f / 2.0f * ((float)M_PI / 180.0f)))) * (180.0f / (float)M_PI) * 100.0f) / 100.0f;
}

void SetFOV(float factor)
{
    *ScreenFieldOfView = AdjustFOV(factor, *ScreenAspectRatio);
}

static constexpr std::uintptr_t RETURN_CCamera_Find3rdPersonCamTargetVector = 0x5149A6;
static void __declspec(naked) CalculateAimingPoint()
{
    _asm
    {
        fstp    st
        mov     edx, [ScreenAspectRatio]
        fmul    [edx]
        mov     edi, [esp + 1Ch + 14h]
        mov     edx, edi
        jmp     RETURN_CCamera_Find3rdPersonCamTargetVector
    }
}

void UpdateMiscFixes()
{
    CameraWidth = 0.01403292f;

    MiscWidth = WidthMult * ScreenWidthScale;            // StretchX
    MiscHeight = HeightMult * ScreenHeightScale;         // StretchY
}

void UpdateHUDFixes()
{
    for (std::uint8_t i = 0; i < std::size(RadarWidth); i++)
        RadarWidth[i] = (i == 6 ? 0.0015f : WidthMult) * ScreenWidthScale * HudWidthScale;

    for (std::uint8_t i = 0; i < std::size(RadarHeight); i++)
        RadarHeight[i] = HeightMult * ScreenHeightScale * HudHeightScale;

    for (std::uint8_t i = 0; i < std::size(HUDWidth); i++)
    {
        if (i < 38)
        {
            float mult = (i == 17 || i == 22) ? 0.17343046f : ((i == 30 || i == 33) ? 0.00195f : WidthMult);
            HUDWidth[i] = mult * ScreenWidthScale * HudWidthScale;
        }
        else
            HUDWidth[i] = 1920.0f * 0.17343046f * ScreenWidthScale * HudWidthScale;
    }

    for (std::uint8_t i = 0; i < std::size(HUDHeight); i++)
        HUDHeight[i] = (i == 18 ? 0.002f : HeightMult) * ScreenHeightScale * HudHeightScale;

    HUDHeight[14] /= 1.125f;
    HUDHeight[15] /= 1.125f;
    HUDHeight[16] /= 1.125f;
}

void UpdateScreenAspectRatio()
{
    ScreenWidthScale = 640.0f / (*(ScreenAspectRatio) * 448.0f);
    
    UpdateMiscFixes();
    UpdateHUDFixes();
}

void InstallAspectRatioFixes()
{
    MemPut<BYTE>(0x745BD1, 0x7D);
    MemPut<BYTE>(0x745BD9, 0x7C);

    // Proportional coronas, skips multiplication by ScreenY in CCoronas::Render
    MemSet((void*)0x6FB2C9, 0x90, 4);

    // Load ScreenY value from the stack
    MemPut<BYTE>(0x6FB2BD, 0x6C);            // CCoronas::Render
    MemPut<BYTE>(0x6FB2DC, 0x78);            // CCoronas::Render
    MemPut<BYTE>(0x713BE5, 0x20);            // CClouds::Render
    MemPut<BYTE>(0x713B6D, 0x38);            // CClouds::Render
    MemPut<BYTE>(0x713CFB, 0x38);            // CClouds::Render
    MemPut<BYTE>(0x713EFC, 0x30);            // CClouds::Render
    MemPut<BYTE>(0x714004, 0x38);            // CClouds::Render

    DWORD callFunc = reinterpret_cast<DWORD>(UpdateScreenAspectRatio);
    HookInstallCall(0x0053D694, callFunc);
    HookInstallCall(0x0053D7B1, callFunc);
    HookInstallCall(0x0053D966, callFunc);
    HookInstallCall(0x0053E770, callFunc);
    HookInstallCall(0x0053EB19, callFunc);
}

void InstallHUDFixes()
{
    #pragma region crosshair and radar
    std::uintptr_t crosshairWidthAddresses[10] = {
        0x58E7CE, 0x58E7F8, 0x58E2FA, 0x58E4ED, 0x58E75B, 0x58E28B, 0x58E2AC, 0x58E2BA, 0x53E472, 0x53E4AE,
    };

    std::uintptr_t crosshairHeightAddresses[7] = {
        0x58E7E4, 0x58E80E, 0x58E319, 0x58E527, 0x58E2C8, 0x53E3E7, 0x53E409,
    };

    for (std::uint8_t i = 0; i < std::size(crosshairWidthAddresses); i++)
    {
        MemPut<float*>(crosshairWidthAddresses[i] + 0x2, &MiscWidth);
    }

    for (std::uint8_t i = 0; i < std::size(crosshairHeightAddresses); i++)
    {
        MemPut<float*>(crosshairHeightAddresses[i] + 0x2, &MiscHeight);
    }

    std::uintptr_t radarWidthAddresses[23] = {
        0x58A441,                                                                        // Radar plane
        0x58A791,                                                                        // Radar disc
        0x58A82E,                                                                        // Radar disc
        0x58A8DF,                                                                        // Radar disc
        0x58A982,                                                                        // Radar disc
        0x58A5D8, 0x58A6DE,                                                              // Radar altimeter
        0x5834BA,                                                                        // Radar point
        0x58603F,                                                                        // Radar point
        0x5886CC,                                                                        // Radar centre
        0x58439C,                                                                        // Radar Trace 0
        0x584434,                                                                        // Radar Trace 0
        0x58410B,                                                                        // Radar Trace 2
        0x584190,                                                                        // Radar Trace 2
        0x584249,                                                                        // Radar Trace 1
        0x5842E6,                                                                        // Radar Trace 1
        0x5876D4, 0x58774B, 0x58780A, 0x58788F, 0x58792E, 0x587A1A, 0x587AAA,            // Radar blips
    };

    std::uintptr_t radarHeightAddresses[21] = {
        0x58A473,                                                    // Radar plane
        0x58A600,                                                    // Radar disc
        0x58A69E,                                                    // Radar disc
        0x58A704,                                                    // Radar disc
        0x58A7B9,                                                    // Radar disc
        0x58A85A, 0x58A909,                                          // Radar altimeter
        0x58A9BD,                                                    // Radar point
        0x5834EC,                                                    // Radar point
        0x586058,                                                    // Radar centre
        0x584346,                                                    // Radar Trace 0
        0x58440C,                                                    // Radar Trace 0
        0x58412B,                                                    // Radar Trace 2
        0x5841B0,                                                    // Radar Trace 2
        0x584207,                                                    // Radar Trace 1
        0x5842C6,                                                    // Radar Trace 1
        0x5876BC, 0x587733, 0x587916, 0x587A02, 0x587A92,            // Radar blips
    };

    for (std::uint8_t i = 0; i < std::size(radarWidthAddresses); i++)
    {
        MemPut<float*>(radarWidthAddresses[i] + 0x2, &RadarWidth[i]);
    }

    for (std::uint8_t i = 0; i < std::size(radarHeightAddresses); i++)
    {
        MemPut<float*>(radarHeightAddresses[i] + 0x2, &RadarHeight[i]);
    }
    #pragma endregion

   std::uintptr_t hudWidthAddresses[38] = {
        0x58EB3F,            // 0 Clock
        0x58EC0C,            // 1 Clock
        0x58F55C,            // 2 Money
        0x58F5F4,            // 3 Money
        0x5892CA,            // 4 Info bars
        0x58937E,            // 5 Info bars
        0x58EE7E,            // 6 Info bars (health)
        0x58EEF4,            // 7 Info bars
        0x589155,            // 8 Info bars
        0x58EF50,            // 9 Info bars
        0x58EFC5,            // 10 Info bars
        0x58922D,            // 11 Info bars
        0x58F116,            // 12 Info bars
        0x58F194,            // 13 Info bars
        0x58D92D,            // 14 Weapon icons fist scale
        0x58D8C3,            // 15 Weapon icons weapon scale
        0x58F91C,            // 16 Weapon icons pos?
        0x58F92D,            // 17 Weapon icons pos!
        0x5894C5,            // 18 Ammo
        0x5894E9,            // 19
        0x58F9D0,            // 20 Ammo
        0x58FA5D,            // 21 Ammo
        0x58F9F5,            // 22 font?
        0x58FA8E,            // 23 Ammo x pos
        0x58DCB8,            // 24 Wanted
        0x58DD00,            // 25 Wanted
        0x58DD7E,            // 26 Wanted
        0x58DF71,            // 27 Wanted
        0x58DFE5,            // 28 Wanted
        0x58B09F,            // 29 Vehicle names
        0x58B13F,            // 30 Vehicle names x pos
        0x58AD3A,            // 31 Area names scale
        0x58AD65,            // 32 Area names ?
        0x58AE4A,            // 33 Area names x pos
        0x58C395,            // 34 Subs
        0x58C41D,            // 35 Subs
        0x58C4DC,            // 36 Subs
        0x4E9F30,            // 37 RadioStation
    };

    std::uintptr_t hudHeightAddresses[34] = {
        0x58EB29,            // 0 Clock
        0x58EBF9,            // 1 Clock
        0x58F546,            // 2 Money
        0x58F5CE,            // 3 Money
        0x589346,            // 4 Info bars
        0x58EE60,            // 5 Info bars
        0x588B9C,            // 6 Info bars
        0x58EEC8,            // 7 Info bars
        0x58913E,            // 8 Info bars
        0x58EF32,            // 9 Info bars
        0x58EF99,            // 10 Info bars
        0x589216,            // 11 Info bars
        0x58F0F8,            // 12 Info bars
        0x58F168,            // 13 Info bars
        0x58D945,            // 14 Weapon icons
        0x58D882,            // 15 Weapon icon with firegun scale
        0x58F90B,            // 16 Weapon icon y pos
        0x5894AF,            // 17 Ammo
        0x58F9C0,            // 18 Ammo y position
        0x58FA4A,            // 19 Ammo
        0x58DCA2,            // 20 Wanted
        0x58DD68,            // 21 Wanted
        0x58DDF4,            // 22 Wanted
        0x58DF55,            // 23 Wanted
        0x58DF9B,            // 24 Wanted
        0x58DEE4,            // 25 Wanted
        0x58B089,            // 26 Vehicle names
        0x58B12D,            // 27 Vehicle names
        0x58AD24,            // 28 Area names
        0x58AE0D,            // 29 Area names
        0x58C37F,            // 30 Subs
        0x58C407,            // 31 Subs
        0x58C4C6,            // 32 Subs
        0x4E9F1A,            // 33 RadioStation
    };

                                                               /*
    for (std::uint8_t i = 0; i < std::size(hudWidthAddresses); i++)
    {
        MemPut<float*>(hudWidthAddresses[i] + 0x2, &HUDWidth[i]);
    }

    for (std::uint8_t i = 0; i < std::size(hudHeightAddresses); i++)
    {
        MemPut<float*>(hudHeightAddresses[i] + 0x2, &HUDHeight[i]);
    }

    // Second player fix, these addresses points to CHud::DrawPlayerInfo
    MemPut<float*>(0x58F9A2, &HUDWidth[38]);                   // Weapon icon X
    MemPut<float*>(0x58F995, &HUDWidth[16]);                   // Weapon icon X
    MemPut<float*>(0x58F974, &HUDHeight[16]);                  // Weapon icon Y
    MemPut<float*>(0x58FA90, &HUDWidth[17]);                   // Ammo x
                                                                */
}

void InstallFieldOfViewFixes()
{
    HookInstall(0x6FF410, reinterpret_cast<DWORD>(SetFOV), 9);
    HookInstall(0x51499E, reinterpret_cast<DWORD>(CalculateAimingPoint), 6);

    MemSet((void*)0x50AD79, 0x90, 6); // Skips division by CDraw::ms_fAspectRatio

    MemPut<float*>(0x50AD5B, &CameraWidth);
    MemPut<float*>(0x51498F, &CameraWidth);
}

void CMultiplayerSA::InitHooks_WidescreenFix()
{
    InstallAspectRatioFixes();
    InstallFieldOfViewFixes();
    InstallHUDFixes();
}
