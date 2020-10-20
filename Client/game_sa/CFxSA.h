/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFxSA.h
 *  PURPOSE:     Game effects handling
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CFx.h>
#include "CFxSystemSA.h"
#include "List_c.h"

#define FUNC_CFx_AddBlood                  0x49eb00
#define FUNC_CFx_AddWood                   0x49ee10
#define FUNC_CFx_AddSparks                 0x49f040
#define FUNC_CFx_AddTyreBurst              0x49f300
#define FUNC_CFx_AddBulletImpact           0x49f3d0
#define FUNC_CFx_AddPunchImpact            0x49f670
#define FUNC_CFx_AddDebris                 0x49f750
#define FUNC_CFx_AddGlass                  0x49f970
#define FUNC_CFx_AddWheelSpray             0x49fb30
#define FUNC_CFx_AddWheelGrass             0x49ff20
#define FUNC_CFx_AddWheelGravel            0x4a0170
#define FUNC_CFx_AddWheelMud               0x4a03c0
#define FUNC_CFx_AddWheelSand              0x4a0610
#define FUNC_CFx_AddWheelDust              0x4a09c0
#define FUNC_CFx_TriggerWaterHydrant       0x4a0d70
#define FUNC_CFx_TriggerGunshot            0x4a0de0
#define FUNC_CFx_TriggerTankFire           0x4a0fa0
#define FUNC_CFx_TriggerWaterSplash        0x4a1070
#define FUNC_CFx_TriggerBulletSplash       0x4a10e0
#define FUNC_CFx_TriggerFootSplash         0x4a1150

enum class eFxQuality : unsigned int
{
    LOW,
    MEDIUM,
    HIGH,
    VERY_HIGH
};

class CFxPrtMult
{
public:
    RwRGBAReal m_color;
    float      m_fSize;
    float      field_14;
    float      m_fLife;

    CFxPrtMult();
    CFxPrtMult(float red, float green, float blue, float alpha, float size, float arg5, float lastFactor);
    void SetUp(float red, float green, float blue, float alpha, float size, float arg5, float lastFactor);
};
static_assert(sizeof(CFxPrtMult) == 0x1C, "Invalid size for CFxPrtMult");

class CFxSAInterface
{
public:
    CFxSystemSAInterface* m_pPrtBlood;
    CFxSystemSAInterface* m_pPrtBoatsplash;
    CFxSystemSAInterface* m_pPrtBubble;
    CFxSystemSAInterface* m_pPrtCardebris;
    CFxSystemSAInterface* m_pPrtCollisionsmoke;
    CFxSystemSAInterface* m_pPrtGunshell;
    CFxSystemSAInterface* m_pPrtSand;
    CFxSystemSAInterface* m_pPrtSand2;
    CFxSystemSAInterface* m_pPrtSmoke_huge;
    CFxSystemSAInterface* m_pPrtSmokeII3expand;
    CFxSystemSAInterface* m_pPrtSpark;
    CFxSystemSAInterface* m_pPrtSpark2;
    CFxSystemSAInterface* m_pPrtSplash;
    CFxSystemSAInterface* m_pPrtWake;
    CFxSystemSAInterface* m_pPrtWatersplash;
    CFxSystemSAInterface* m_pPrtWheeldirt;
    CFxSystemSAInterface* m_pPrtGlass;
    TList_c<ListItem_c>   m_entityFxList;
    unsigned int          m_nBloodPoolsCount;
    eFxQuality            m_fxQuality;
    unsigned int          m_nVerticesCount2;
    unsigned int          m_nVerticesCount;
    unsigned int          m_nTransformRenderFlags;
    RwRaster*             m_pRasterToRender;
    RwMatrix*             m_pTransformLTM;
    void*                 m_pVerts;
};
static_assert(sizeof(CFxSAInterface) == 0x70, "Invalid size for CFxSAInterface");

class CFxSA : public CFx
{
public:
    CFxSA(CFxSAInterface* pInterface) { m_pInterface = pInterface; }

    void AddBlood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness);
    void AddWood(CVector& vecPosition, CVector& vecDirection, int iCount, float fBrightness);
    void AddSparks(CVector& vecPosition, CVector& vecDirection, float fForce, int iCount, CVector vecAcrossLine, unsigned char ucBlurIf0, float fSpread,
                   float fLife);
    void AddTyreBurst(CVector& vecPosition, CVector& vecDirection);
    void AddBulletImpact(CVector& vecPosition, CVector& vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity);
    void AddPunchImpact(CVector& vecPosition, CVector& vecDirection, int);
    void AddDebris(CVector& vecPosition, RwColor& rwColor, float fScale, int iCount);
    void AddGlass(CVector& vecPosition, RwColor& rwColor, float fScale, int iCount);
    void TriggerWaterHydrant(CVector& vecPosition);
    void TriggerGunshot(CEntity* pEntity, CVector& vecPosition, CVector& vecDirection, bool bIncludeSparks);
    void TriggerTankFire(CVector& vecPosition, CVector& vecDirection);
    void TriggerWaterSplash(CVector& vecPosition);
    void TriggerBulletSplash(CVector& vecPosition);
    void TriggerFootSplash(CVector& vecPosition);

    static CFxSAInterface& g_fx;
private:
    CFxSAInterface* m_pInterface;
};
