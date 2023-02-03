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
#include "CFxSA.h"
#include "CEntitySA.h"

using StoreShadowToBeRendered_t = int(__cdecl*)(char type, RwTexture* texture, CVector* pos, float x1, float y1, float x2, float y2, __int16 intensity, char r,
                                                char g, char b, float zDistance, char bDrawOnWater, float scale, void* shadowData, char bDrawOnBuildings);
auto StoreShadowToBeRendered = (StoreShadowToBeRendered_t)0x707390;

CFxSA::CFxSA(CFxSAInterface* pInterface) : m_pInterface(pInterface)
{
    m_textureMap[eShadowType::PLANE] = new RwTexture();
}

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

bool CFxSA::IsShadowsLimitReached()
{
    unsigned short& CShadows_ShadowsStoredToBeRendered = *(unsigned short*)0xC403DC;
    return CShadows_ShadowsStoredToBeRendered >= 48;
}

bool CFxSA::AddShadow(eShadowType shadowType, CVector& vecPosition, CVector2D& vecOffset1, CVector2D& vecOffset2, SColor color, float fZDistance,
                      bool bDrawOnWater, bool bDrawOnBuildings)
{
    if (IsShadowsLimitReached())
        return false;


    void*      textureAddress = *(void**)(SHADOW_BASE_TEXTURE_OFFSET + (int)shadowType * 4);
    RwTexture* pRwTexture = reinterpret_cast<RwTexture*>(textureAddress);
    char       type = 1;

    switch (shadowType)
    {
        case eShadowType::HEADLIGHT1:
        case eShadowType::HEADLIGHT2:
        case eShadowType::EXPLOSION:
            type = 2;
    }

    if (pRwTexture == nullptr)
        return false;

    StoreShadowToBeRendered(type, pRwTexture, &vecPosition, vecOffset1.fX, vecOffset1.fY, vecOffset2.fX, vecOffset2.fY, color.A, color.R, color.G, color.B,
                            fZDistance, bDrawOnWater, 1, 0, bDrawOnBuildings);

    return true;
}
