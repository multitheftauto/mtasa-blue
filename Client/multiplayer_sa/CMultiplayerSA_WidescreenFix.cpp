/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_WidescreenFix.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// NOTE: This code is based on ThirteenAG's fix, which is licensed under the MIT license.
#include "StdInc.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

static constexpr float CameraWidth = 0.01403292f;
static constexpr float SkyMultiFix = 10.0f;

static float* ScreenAspectRatio = reinterpret_cast<float*>(0xC3EFA4);
static float* ScreenFieldOfView = reinterpret_cast<float*>(0x8D5038);

static float AdjustFOV(float factor, float aspectRatio) noexcept
{
    return std::round((2.0f * std::atan(((aspectRatio) / (4.0f / 3.0f)) *
        std::tan(factor / 2.0f * ((float)M_PI / 180.0f)))) *
        (180.0f / (float)M_PI) * 100.0f) / 100.0f;
}

static void SetFOV(float factor)
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

static void InstallAspectRatioFixes()
{
    // Disables jump instructions in GetVideoModeList
    MemSet((void*)0x745BD1, 0x90, 2);
    MemSet((void*)0x745BD9, 0x90, 2);

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
}

static void InstallFieldOfViewFixes()
{
    // Fix sky blurring white in ultrawide screens
    MemPut<const float*>(0x714843, &SkyMultiFix);
    MemPut<const float*>(0x714860, &SkyMultiFix);

    HookInstall(0x6FF410, reinterpret_cast<DWORD>(SetFOV), 9);
    HookInstall(0x51499E, reinterpret_cast<DWORD>(CalculateAimingPoint), 6);

    // Skips division by CDraw::ms_fAspectRatio
    MemSet((void*)0x50AD79, 0x90, 6);
    MemPut<const float*>(0x50AD5B, &CameraWidth);            // CCamera::Find3rdPersonQuickAimPitch
    MemPut<const float*>(0x51498F, &CameraWidth);             // CCamera::Find3rdPersonCamTargetVector
}

void CMultiplayerSA::InitHooks_WidescreenFix()
{
    InstallAspectRatioFixes();
    InstallFieldOfViewFixes();
}
