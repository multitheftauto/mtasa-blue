/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPointLightsSA.h
 *  PURPOSE:     Header file for PointLights entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPointLights.h>

#define FUNC_CPointLights_AddLight      0x7000e0
#define FUNC_CHeli_Pre_SearchLightCone  0x6C4650
#define FUNC_CHeli_Post_SearchLightCone 0x6C46E0
#define FUNC_CHeli_SearchLightCone      0x6C58E0

class CPointLightsSA : public CPointLights
{
public:
    virtual void AddLight(int iMode, const CVector vecPosition, CVector vecDirection, float fRadius, SharedUtil::SColor color, unsigned char uc_8,
                          bool bCreatesShadow, CEntity* pAffected) override;

    virtual void PreRenderHeliLights() override;
    virtual void PostRenderHeliLights() override;
    virtual void RenderHeliLight(const CVector& vecStart, const CVector& vecEnd, float startRadius, float endRadius, bool renderSpot) override;
    ;
};
