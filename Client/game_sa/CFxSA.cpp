/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFxSA.cpp
 *  PURPOSE:     Game effects handling
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CFxSA::AddBlood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness)
{
    // CFx::AddBlood
    ((void(__thiscall*)(CFxSAInterface*, CVector&, CVector&, int, float))FUNC_CFx_AddBlood)(m_pInterface, vecPosition, vecDirection, iCount, fBrightness);
}

void CFxSA::AddWood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness)
{
    // CFx::AddWood
    ((void(__thiscall*)(CFxSAInterface*, CVector&, CVector&, int, float))FUNC_CFx_AddWood)(m_pInterface, vecPosition, vecDirection, iCount, fBrightness);
}

void CFxSA::AddSparks(CVector& vecPosition, CVector& vecDirection, float fForce, int iCount, CVector vecAcrossLine, unsigned char ucBlurIf0, float fSpread,
                      float fLife)
{
    // CFx::AddSparks
    ((void(__thiscall*)(CFxSAInterface*, CVector&, CVector&, float, int, float, float, float, unsigned char, float, float))FUNC_CFx_AddSparks)(
        m_pInterface, vecPosition, vecDirection, fForce, iCount, vecAcrossLine.fX, vecAcrossLine.fY, vecAcrossLine.fZ, ucBlurIf0, fSpread, fLife);
}

void CFxSA::AddTyreBurst(CVector& vecPosition, CVector& vecDirection)
{
    // CFx::AddTyreBurst
    ((void(__thiscall*)(CFxSAInterface*, CVector&, CVector&))FUNC_CFx_AddTyreBurst)(m_pInterface, vecPosition, vecDirection);
}

void CFxSA::AddBulletImpact(CVector& vecPosition, CVector& vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity)
{
    // CFx::AddBulletImpact
    ((void(__thiscall*)(CFxSAInterface*, CVector&, CVector&, int, int, float))FUNC_CFx_AddBulletImpact)(m_pInterface, vecPosition, vecDirection, iSmokeSize,
                                                                                                        iSparkCount, fSmokeIntensity);
}

void CFxSA::AddPunchImpact(CVector& vecPosition, CVector& vecDirection, int i)
{
    // CFx::AddPunchImpact
    ((void(__thiscall*)(CFxSAInterface*, CVector&, CVector&, int))FUNC_CFx_AddPunchImpact)(m_pInterface, vecPosition, vecDirection, i);
}

void CFxSA::AddDebris(CVector& vecPosition, RwColor& rwColor, float fDebrisScale, int iCount)
{
    // CFx::AddDebris
    ((void(__thiscall*)(CFxSAInterface*, CVector&, RwColor&, float, int))FUNC_CFx_AddDebris)(m_pInterface, vecPosition, rwColor, fDebrisScale, iCount);
}

void CFxSA::AddGlass(CVector& vecPosition, RwColor& rwColor, float fDebrisScale, int iCount)
{
    // CFx::AddGlass
    ((void(__thiscall*)(CFxSAInterface*, CVector&, RwColor&, float, int))FUNC_CFx_AddGlass)(m_pInterface, vecPosition, rwColor, fDebrisScale, iCount);
}

void CFxSA::TriggerWaterHydrant(CVector& vecPosition)
{
    // CFx::TriggerWaterHydrant
    ((void(__thiscall*)(CFxSAInterface*, CVector&))FUNC_CFx_TriggerWaterHydrant)(m_pInterface, vecPosition);
}

void CFxSA::TriggerGunshot(CEntity* pEntity, CVector& vecPosition, CVector& vecDirection, bool bIncludeSparks)
{
    // CFx::TriggerGunshot
    ((void(__thiscall*)(CFxSAInterface*, CEntitySAInterface*, CVector&, CVector&, bool))FUNC_CFx_TriggerGunshot)(m_pInterface, pEntity? pEntity->GetInterface() : nullptr, vecPosition, vecDirection, bIncludeSparks);
}

void CFxSA::TriggerTankFire(CVector& vecPosition, CVector& vecDirection)
{
    // CFx::TriggerTankFire
    ((void(__thiscall*)(CFxSAInterface*, CVector&, CVector&))FUNC_CFx_TriggerTankFire)(m_pInterface, vecPosition, vecDirection);
}

void CFxSA::TriggerWaterSplash(CVector& vecPosition)
{
    // CFx::TriggerWaterSplash
    ((void(__thiscall*)(CFxSAInterface*, CVector&))FUNC_CFx_TriggerWaterSplash)(m_pInterface, vecPosition);
}

void CFxSA::TriggerBulletSplash(CVector& vecPosition)
{
    // CFx::TriggerBulletSplash
    ((void(__thiscall*)(CFxSAInterface*, CVector&))FUNC_CFx_TriggerBulletSplash)(m_pInterface, vecPosition);
}

void CFxSA::TriggerFootSplash(CVector& vecPosition)
{
    // CFx::TriggerFootSplash
    ((void(__thiscall*)(CFxSAInterface*, CVector&))FUNC_CFx_TriggerFootSplash)(m_pInterface, vecPosition);
}
