/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFxSA.cpp
 *  PURPOSE:     Game effects handling
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFxSA.h"
#include "CEntitySA.h"

void CFxSA::AddBlood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddBlood;
    _asm
    {
        mov     ecx, dwThis
        push    fBrightness
        push    iCount
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::AddWood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddWood;
    _asm
    {
        mov     ecx, dwThis
        push    fBrightness
        push    iCount
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::AddSparks(CVector& vecPosition, CVector& vecDirection, float fForce, int iCount, CVector vecAcrossLine, unsigned char ucBlurIf0, float fSpread,
                      float fLife)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    float    fX = vecAcrossLine.fX, fY = vecAcrossLine.fY, fZ = vecAcrossLine.fZ;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddSparks;
    _asm
    {
        mov     ecx, dwThis
        push    fLife
        push    fSpread
        push    ucBlurIf0
        push    fZ
        push    fY
        push    fX
        push    iCount
        push    fForce
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::AddTyreBurst(CVector& vecPosition, CVector& vecDirection)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddTyreBurst;
    _asm
    {
        mov     ecx, dwThis
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::AddBulletImpact(CVector& vecPosition, CVector& vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddBulletImpact;
    _asm
    {
        mov     ecx, dwThis
        push    fSmokeIntensity
        push    iSparkCount
        push    iSmokeSize
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::AddPunchImpact(CVector& vecPosition, CVector& vecDirection, int i)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddPunchImpact;
    _asm
    {
        mov     ecx, dwThis
        push    i
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::AddDebris(CVector& vecPosition, RwColor& rwColor, float fDebrisScale, int iCount)
{
    CVector* pvecPosition = &vecPosition;
    RwColor* pColor = &rwColor;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddDebris;
    _asm
    {
        mov     ecx, dwThis
        push    iCount
        push    fDebrisScale
        push    pColor
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::AddGlass(CVector& vecPosition, RwColor& rwColor, float fDebrisScale, int iCount)
{
    CVector* pvecPosition = &vecPosition;
    RwColor* pColor = &rwColor;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddGlass;
    _asm
    {
        mov     ecx, dwThis
        push    iCount
        push    fDebrisScale
        push    pColor
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::TriggerWaterHydrant(CVector& vecPosition)
{
    CVector* pvecPosition = &vecPosition;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerWaterHydrant;
    _asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::TriggerGunshot(CEntity* pEntity, CVector& vecPosition, CVector& vecDirection, bool bIncludeSparks)
{
    DWORD    dwEntity = (pEntity) ? (DWORD)pEntity->GetInterface() : NULL;
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerGunshot;
        _asm
    {
        mov     ecx, dwThis
        push    bIncludeSparks
        push    pvecDirection
        push    pvecPosition
        push    dwEntity
        call    dwFunc
    }
}

void CFxSA::TriggerTankFire(CVector& vecPosition, CVector& vecDirection)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerTankFire;
    _asm
    {
        mov     ecx, dwThis
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::TriggerWaterSplash(CVector& vecPosition)
{
    CVector* pvecPosition = &vecPosition;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerWaterSplash;
    _asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::TriggerBulletSplash(CVector& vecPosition)
{
    CVector* pvecPosition = &vecPosition;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerBulletSplash;
    _asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::TriggerFootSplash(CVector& vecPosition)
{
    CVector* pvecPosition = &vecPosition;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerFootSplash;
    _asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
}

void CFxSA::AddParticle(FxParticleSystems eFxParticle, const CVector& vecPosition, const CVector& vecDirection, float fR, float fG, float fB, float fA, bool bRandomizeColors, std::uint32_t iCount, float fBrightness, float fSize, bool bRandomizeSizes, float fLife)
{
    // Init our own FxPrtMult struct
    FxPrtMult_c fxPrt{{fR,fG,fB,fA}, fSize, 0, fLife};
    CVector     newDirection;

    FxSystem_c* fxParticleSystem;

    switch (eFxParticle)
    {
        case FxParticleSystems::PRT_BLOOD:
            fxParticleSystem = m_pInterface->m_fxSysBlood;
            break;
        case FxParticleSystems::PRT_BOATSPLASH:
            fxParticleSystem = m_pInterface->m_fxSysBoatSplash;
            break;
        case FxParticleSystems::PRT_BUBBLE:
            fxParticleSystem = m_pInterface->m_fxSysBubble;
            break;
        case FxParticleSystems::PRT_DEBRIS:
            fxParticleSystem = m_pInterface->m_fxSysDebris;
            break;
        case FxParticleSystems::PRT_GUNSHELL:
            fxParticleSystem = m_pInterface->m_fxSysGunshell;
            break;
        case FxParticleSystems::PRT_SAND:
            fxParticleSystem = m_pInterface->m_fxSysSand;
            break;
        case FxParticleSystems::PRT_SAND2:
            fxParticleSystem = m_pInterface->m_fxSysSand2;
            break;
        case FxParticleSystems::PRT_SMOKE:
            fxParticleSystem = m_pInterface->m_fxSysSmoke;
            break;
        case FxParticleSystems::PRT_SMOKEHUGE:
            fxParticleSystem = m_pInterface->m_fxSysSmokeHuge;
            break;
        case FxParticleSystems::PRT_SMOKE2:
            fxParticleSystem = m_pInterface->m_fxSysSmoke2;
            break;
        case FxParticleSystems::PRT_SPARK:
            fxParticleSystem = m_pInterface->m_fxSysSpark;
            break;
        case FxParticleSystems::PRT_SPARK2:
            fxParticleSystem = m_pInterface->m_fxSysSpark2;
            break;
        case FxParticleSystems::PRT_SPLASH:
            fxParticleSystem = m_pInterface->m_fxSysSplash;
            break;
        case FxParticleSystems::PRT_WAKE:
            fxParticleSystem = m_pInterface->m_fxSysWake;
            break;
        case FxParticleSystems::PRT_WATERSPLASH:
            fxParticleSystem = m_pInterface->m_fxSysWaterSplash;
            break;
        case FxParticleSystems::PRT_WHEELDIRT:
            fxParticleSystem = m_pInterface->m_fxSysWheelDirt;
            break;
        case FxParticleSystems::PRT_GLASS:
            fxParticleSystem = m_pInterface->m_fxSysGlass;
            break;
        default:
            fxParticleSystem = m_pInterface->m_fxSysBlood;
    }

    for (size_t i = 0; i < iCount; i++)
    {
        if (bRandomizeColors)
        {
            // 0x49EECB
            fxPrt.m_color.red = (rand() % 10000) * 0.0001f * fR + 0.13f;
            fxPrt.m_color.green = (rand() % 10000) * 0.0001f * fG + 0.12f;
            fxPrt.m_color.blue = (rand() % 10000) * 0.0001f * fB + 0.04f;
        }

        if (bRandomizeSizes)
            // 0x49EF21 - Calculate random size for each particle
            fxPrt.m_fSize = (rand() % 10000) * 0.0001f * fSize + 0.3f;

        // 0x49EF4C - Calculate random direction for each particle
        newDirection = CVector(vecDirection.fX * 4, vecDirection.fY * 4, vecDirection.fZ * 4);
        newDirection.fX = (rand() % 10000) * 0.0001f * 4 - 2 + newDirection.fX;
        newDirection.fY = (rand() % 10000) * 0.0001f * 4 - 2 + newDirection.fY;
        newDirection.fZ = (rand() % 10000) * 0.0001f * 4 - 2 + newDirection.fZ;

        // Call FxSystem_c::AddParticle
        ((int(__thiscall*)(FxSystem_c*, const CVector*, const CVector*, float, FxPrtMult_c*, float, float, float, int))FUNC_FXSystem_c_AddParticle)(fxParticleSystem, &vecPosition, &newDirection, 0, &fxPrt, -1.0f, fBrightness, 0, 0);
    }
}
