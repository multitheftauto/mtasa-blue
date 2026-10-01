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
#include <game/RenderWare.h>
#include <game/RenderWareD3D.h>
#include "gamesa_renderware.h"
#include "CVector2D.h"
#include "CFxSA.h"
#include "CEntitySA.h"

void CFxSA::AddBlood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddBlood;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    fBrightness
        push    iCount
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::AddWood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddWood;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    fBrightness
        push    iCount
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::AddSparks(CVector& vecPosition, CVector& vecDirection, float fForce, int iCount, CVector vecAcrossLine, unsigned char ucBlurIf0, float fSpread,
                      float fLife)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    float    fX = vecAcrossLine.fX, fY = vecAcrossLine.fY, fZ = vecAcrossLine.fZ;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddSparks;
    // clang-format off
    __asm
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
    // clang-format on
}

void CFxSA::AddTyreBurst(CVector& vecPosition, CVector& vecDirection)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddTyreBurst;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::AddBulletImpact(CVector& vecPosition, CVector& vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddBulletImpact;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    fSmokeIntensity
        push    iSparkCount
        push    iSmokeSize
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::AddPunchImpact(CVector& vecPosition, CVector& vecDirection, int i)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddPunchImpact;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    i
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::AddDebris(CVector& vecPosition, RwColor& rwColor, float fDebrisScale, int iCount)
{
    CVector* pvecPosition = &vecPosition;
    RwColor* pColor = &rwColor;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddDebris;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    iCount
        push    fDebrisScale
        push    pColor
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::AddGlass(CVector& vecPosition, RwColor& rwColor, float fDebrisScale, int iCount)
{
    CVector* pvecPosition = &vecPosition;
    RwColor* pColor = &rwColor;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_AddGlass;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    iCount
        push    fDebrisScale
        push    pColor
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::TriggerWaterHydrant(CVector& vecPosition)
{
    CVector* pvecPosition = &vecPosition;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerWaterHydrant;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::TriggerGunshot(CEntity* pEntity, CVector& vecPosition, CVector& vecDirection, bool bIncludeSparks)
{
    DWORD    dwEntity = (pEntity) ? (DWORD)pEntity->GetInterface() : NULL;
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerGunshot;
    // clang-format off
        __asm
    {
        mov     ecx, dwThis
        push    bIncludeSparks
        push    pvecDirection
        push    pvecPosition
        push    dwEntity
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::TriggerTankFire(CVector& vecPosition, CVector& vecDirection)
{
    CVector* pvecPosition = &vecPosition;
    CVector* pvecDirection = &vecDirection;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerTankFire;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::TriggerWaterSplash(CVector& vecPosition)
{
    CVector* pvecPosition = &vecPosition;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerWaterSplash;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::TriggerBulletSplash(CVector& vecPosition)
{
    CVector* pvecPosition = &vecPosition;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerBulletSplash;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::TriggerFootSplash(CVector& vecPosition)
{
    CVector* pvecPosition = &vecPosition;
    DWORD    dwThis = (DWORD)m_pInterface;
    DWORD    dwFunc = FUNC_CFx_TriggerFootSplash;
    // clang-format off
    __asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
    // clang-format on
}

void CFxSA::AddParticle(FxParticleSystems::Enum eFxParticle, const CVector& vecPosition, const CVector& vecDirection, float fR, float fG, float fB, float fA,
                        bool bRandomizeColors, std::uint32_t iCount, float fBrightness, float fSize, bool bRandomizeSizes, float fLife)
{
    // Init our own FxPrtMult struct
    FxPrtMult_c fxPrt{{fR, fG, fB, fA}, fSize, 0, fLife};
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
        ((int(__thiscall*)(FxSystem_c*, const CVector*, const CVector*, float, FxPrtMult_c*, float, float, float, int))FUNC_FXSystem_c_AddParticle)(
            fxParticleSystem, &vecPosition, &newDirection, 0, &fxPrt, -1.0f, fBrightness, 0, 0);
    }
}

namespace
{
    constexpr unsigned int MAX_SCRIPT_SHADOWS = 48;
    RwTexture*             customShadowTextures[MAX_SCRIPT_SHADOWS] = {};

    void DestroyCustomShadowTexture(RwTexture* texture)
    {
        if (!texture)
            return;
        auto native = reinterpret_cast<RwD3D9Raster*>(&texture->raster->renderResource);
        if (native->texture)
            native->texture->Release();
        native->texture = nullptr;
        RwTextureDestroy(texture);
    }

    unsigned short& StoredShadowCount()
    {
        return *reinterpret_cast<unsigned short*>(VAR_FXSystem_StoreShadows);
    }
}

CFxSA::~CFxSA()
{
    ClearCustomShadows(true);
}

void CFxSA::ClearCustomShadows(bool force)
{
    // GTA consumes the queue before the pre-FX callback. Keep textures alive if
    // rendering was skipped; device invalidation instead discards the queue.
    if (force)
        StoredShadowCount() = 0;
    else if (StoredShadowCount() != 0)
        return;

    for (auto& texture : customShadowTextures)
    {
        DestroyCustomShadowTexture(texture);
        texture = nullptr;
    }
}

bool CFxSA::IsShadowsLimitReached()
{
    return StoredShadowCount() >= MAX_SCRIPT_SHADOWS;
}

bool CFxSA::AddShadow(eShadowTextureType shadowTextureType, const CVector& vecPosition, const CVector2D& vecOffset1, const CVector2D& vecOffset2, SColor color,
                      eShadowType shadowType, float fZDistance, bool bDrawOnWater, bool bDrawOnBuildings, IDirect3DBaseTexture9* customTexture)
{
    if (IsShadowsLimitReached() || shadowTextureType < eShadowTextureType::CAR || shadowTextureType >= eShadowTextureType::COUNT)
        return false;

    const auto index = StoredShadowCount();
    RwTexture* texture = reinterpret_cast<RwTexture**>(TEXTURE_FXSystem_Shadow)[static_cast<unsigned int>(shadowTextureType)];
    if (customTexture)
    {
        if (customTexture->GetType() != D3DRTYPE_TEXTURE)
            return false;
        D3DSURFACE_DESC desc;
        auto            d3dTexture = static_cast<IDirect3DTexture9*>(customTexture);
        if (FAILED(d3dTexture->GetLevelDesc(0, &desc)))
            return false;

        // A real RW raster without pixel allocation. The queue holds its own
        // COM reference, so destroying the Lua element cannot invalidate it.
        RwRaster* raster = RwRasterCreate(desc.Width, desc.Height, 32, 0x0500 | 0x04 | 0x80);  // 8888 | TEXTURE | DONTALLOCATE
        if (!raster)
            return false;
        texture = RwTextureCreate(raster);
        if (!texture)
        {
            reinterpret_cast<int(__cdecl*)(RwRaster*)>(0x7FB020)(raster);  // RwRasterDestroy
            return false;
        }
        raster->width = desc.Width;
        raster->height = desc.Height;
        raster->depth = 32;
        raster->format = 0x05;  // 8888
        auto native = reinterpret_cast<RwD3D9Raster*>(&raster->renderResource);
        d3dTexture->AddRef();
        native->texture = d3dTexture;
        native->alpha = true;
        native->format = desc.Format;
        if (desc.Format >= D3DFMT_DXT1 && desc.Format <= D3DFMT_DXT5)
            native->textureFlags |= 0x10;
        texture->flags = 0x3302;  // linear filtering, clamp U/V

        // A reused queue index can only refer to an already consumed frame.
        DestroyCustomShadowTexture(customShadowTextures[index]);
        customShadowTextures[index] = texture;
    }
    if (!texture || !texture->raster)
        return false;

    using StoreShadow = void(__cdecl*)(unsigned char, RwTexture*, const CVector*, float, float, float, float, short, unsigned char, unsigned char,
                                       unsigned char, float, bool, float, void*, bool);
    reinterpret_cast<StoreShadow>(FUNC_FXSystem_StoreShadows)(static_cast<unsigned char>(shadowType), texture, &vecPosition, vecOffset1.fX, vecOffset1.fY,
                                                              vecOffset2.fX, vecOffset2.fY, color.A, color.R, color.G, color.B, fZDistance, bDrawOnWater, 1.0f,
                                                              nullptr, bDrawOnBuildings);
    return StoredShadowCount() > index;
}
