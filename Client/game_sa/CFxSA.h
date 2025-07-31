/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFxSA.h
 *  PURPOSE:     Game effects handling
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CFx.h>

struct RwColor;
class FxSystem_c;

#define FUNC_CFx_AddBlood                  0x49eb00
#define FUNC_CFx_AddWood                   0x49ee10
#define FUNC_CFx_AddSparks                 0x49f040
#define FUNC_CFx_AddTyreBurst              0x49f300
#define FUNC_CFx_AddBulletImpact           0x49f3d0
#define FUNC_CFx_AddPunchImpact            0x49f670
#define FUNC_CFx_AddDebris                 0x49f750
#define FUNC_CFx_AddGlass                  0x49f970
#define FUNC_CFx_TriggerWaterHydrant       0x4a0d70
#define FUNC_CFx_TriggerGunshot            0x4a0de0
#define FUNC_CFx_TriggerTankFire           0x4a0fa0
#define FUNC_CFx_TriggerWaterSplash        0x4a1070
#define FUNC_CFx_TriggerBulletSplash       0x4a10e0
#define FUNC_CFx_TriggerFootSplash         0x4a1150
#define FUNC_FXSystem_c_AddParticle        0x4AA440

class CFxSAInterface
{
public:
    FxSystem_c* m_fxSysBlood;
    FxSystem_c* m_fxSysBoatSplash;
    FxSystem_c* m_fxSysBubble;
    FxSystem_c* m_fxSysDebris;
    FxSystem_c* m_fxSysSmoke;
    FxSystem_c* m_fxSysGunshell;
    FxSystem_c* m_fxSysSand;
    FxSystem_c* m_fxSysSand2;
    FxSystem_c* m_fxSysSmokeHuge;
    FxSystem_c* m_fxSysSmoke2;
    FxSystem_c* m_fxSysSpark;
    FxSystem_c* m_fxSysSpark2;
    FxSystem_c* m_fxSysSplash;
    FxSystem_c* m_fxSysWake;
    FxSystem_c* m_fxSysWaterSplash;
    FxSystem_c* m_fxSysWheelDirt;
    FxSystem_c* m_fxSysGlass;

private:
};

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
    void AddParticle(FxParticleSystems eFxParticle, const CVector& vecPosition, const CVector& vecDirection, float fR, float fG, float fB, float fA, bool bRandomizeColors, std::uint32_t iCount, float fBrightness, float fSize, bool bRandomizeSizes, float fLife);

private:
    CFxSAInterface* m_pInterface;

    struct FxPrtMult_c
    {
        struct
        {
            float red;
            float green;
            float blue;
            float alpha;
        } m_color;

        float m_fSize;
        float unk;
        float m_fLife;
    };
};
