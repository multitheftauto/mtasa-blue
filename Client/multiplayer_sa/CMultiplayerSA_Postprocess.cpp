/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_Postprocess.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define HOOKPOS_GrainEffect_NightModifier 0x704EE8
#define HOOKPOS_GrainEffect_InfraredModifier 0x704F59
#define HOOKPOS_GrainEffect_RainModifier 0x705078
#define HOOKPOS_GrainEffect_OverlayModifier 0x705091

namespace GrainEffect
{

static BYTE ucGrainEnabled = FALSE;
static DWORD dwGrainStrength = 1;

struct MasterModifier
{
    static float fMultiplier;

    static int ApplyEffect(BYTE ucLevel, BYTE ucUpdate)
    {
        return ((int(__cdecl*)(char, char))0x7037C0)(ucLevel * fMultiplier, ucUpdate);
    }
};

struct InfraredModifier
{
    static float fMultiplier;

    static int ApplyEffect(BYTE ucLevel, BYTE ucUpdate)
    {
        return MasterModifier::ApplyEffect(ucLevel * fMultiplier, ucUpdate);
    }
};

struct NightModifier
{
    static float fMultiplier;

    static int ApplyEffect(BYTE ucLevel, BYTE ucUpdate)
    {
        return MasterModifier::ApplyEffect(ucLevel * fMultiplier, ucUpdate);
    }
};

struct RainModifier
{
    static float fMultiplier;

    static int ApplyEffect(BYTE ucLevel, BYTE ucUpdate)
    {
        return MasterModifier::ApplyEffect(ucLevel * fMultiplier, ucUpdate);
    }
};

struct OverlayModifier
{
    static float fMultiplier;

    static int ApplyEffect(BYTE ucLevel, BYTE ucUpdate)
    {
        return MasterModifier::ApplyEffect(ucLevel * fMultiplier, ucUpdate);
    }
};

float MasterModifier::fMultiplier = 1.0f;
float InfraredModifier::fMultiplier = 1.0f;
float NightModifier::fMultiplier = 1.0f;
float RainModifier::fMultiplier = 1.0f;
float OverlayModifier::fMultiplier = 1.0f;

}

void CMultiplayerSA::SetGrainMultiplier(eGrainMultiplierType type, float fMultiplier)
{
    using namespace GrainEffect;

    fMultiplier = Clamp(0.0f, fMultiplier, 1.0f);

    switch (type)
    {
    case eGrainMultiplierType::MASTER:
        MasterModifier::fMultiplier = fMultiplier;
        break;
    case eGrainMultiplierType::INFRARED:
        InfraredModifier::fMultiplier = fMultiplier;
        break;
    case eGrainMultiplierType::NIGHT:
        NightModifier::fMultiplier = fMultiplier;
        break;
    case eGrainMultiplierType::RAIN:
        RainModifier::fMultiplier = fMultiplier;
        break;
    case eGrainMultiplierType::OVERLAY:
        OverlayModifier::fMultiplier = fMultiplier;
        break;
    case eGrainMultiplierType::ALL:
        MasterModifier::fMultiplier = InfraredModifier::fMultiplier = NightModifier::fMultiplier = RainModifier::fMultiplier = OverlayModifier::fMultiplier = fMultiplier;
        break;
    default:
        break;
    }
}

void CMultiplayerSA::SetGrainLevel(BYTE ucLevel)
{
    const bool bOverridden = *(DWORD*)0x705081 != (DWORD)0x00C402B4;
    const bool bEnable = ucLevel > 0;

    GrainEffect::ucGrainEnabled = static_cast<BYTE>(bEnable);
    GrainEffect::dwGrainStrength = static_cast<DWORD>(ucLevel);

    if (bEnable == bOverridden)
        return;    

    if (bEnable)
    {
        MemPut<DWORD>(0x705081, (DWORD)&GrainEffect::ucGrainEnabled);
        MemPut<DWORD>(0x70508A, (DWORD)&GrainEffect::dwGrainStrength);
    }
    else
    {
        // Restore the original address
        MemPut<DWORD>(0x705081, (DWORD)0x00C402B4);
        MemPut<DWORD>(0x70508A, (DWORD)0x008D5094);
    }
}

void CMultiplayerSA::SetNightVisionEnabled(bool bEnabled, bool bNoiseEnabled)
{
    if (bEnabled)
    {
        MemPutFast<BYTE>(0xC402B8, 1);
    }
    else
    {
        MemPutFast<BYTE>(0xC402B8, 0);
    }
    if (bNoiseEnabled)
    {
        HookInstallCall(0x704EE8, (DWORD)GrainEffect::NightModifier::ApplyEffect);
    }
    else
    {
        MemSet((void*)0x704EE8, 0x90, 5);
    }
}

void CMultiplayerSA::SetThermalVisionEnabled(bool bEnabled, bool bNoiseEnabled)
{
    if (bEnabled)
    {
        MemPutFast<BYTE>(0xC402B9, 1);
    }
    else
    {
        MemPutFast<BYTE>(0xC402B9, 0);
    }
    if (bNoiseEnabled)
    {
        HookInstallCall(0x704F59, (DWORD)GrainEffect::InfraredModifier::ApplyEffect);
    }
    else
    {
        MemSet((void*)0x704F59, 0x90, 5);
    }
}

bool CMultiplayerSA::IsNightVisionEnabled()
{
    return (*(BYTE*)0xC402B8 == 1);
}

bool CMultiplayerSA::IsThermalVisionEnabled()
{
    return (*(BYTE*)0xC402B9 == 1);
}

void CMultiplayerSA::InitHooks_Postprocess()
{
    HookInstallCall(HOOKPOS_GrainEffect_NightModifier, (DWORD)GrainEffect::NightModifier::ApplyEffect);
    HookInstallCall(HOOKPOS_GrainEffect_InfraredModifier, (DWORD)GrainEffect::InfraredModifier::ApplyEffect);
    HookInstallCall(HOOKPOS_GrainEffect_RainModifier, (DWORD)GrainEffect::RainModifier::ApplyEffect);
    HookInstallCall(HOOKPOS_GrainEffect_OverlayModifier, (DWORD)GrainEffect::OverlayModifier::ApplyEffect);
}
