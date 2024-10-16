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
#include "CFxSystemBPSA.h"
#include "CVector.h"

struct RwColor;
struct RwRaster;
struct RwMatrix;

#define FUNC_CFx_AddBlood            0x49eb00
#define FUNC_CFx_AddWood             0x49ee10
#define FUNC_CFx_AddSparks           0x49f040
#define FUNC_CFx_AddTyreBurst        0x49f300
#define FUNC_CFx_AddBulletImpact     0x49f3d0
#define FUNC_CFx_AddPunchImpact      0x49f670
#define FUNC_CFx_AddDebris           0x49f750
#define FUNC_CFx_AddGlass            0x49f970
#define FUNC_CFx_TriggerWaterHydrant 0x4a0d70
#define FUNC_CFx_TriggerGunshot      0x4a0de0
#define FUNC_CFx_TriggerTankFire     0x4a0fa0
#define FUNC_CFx_TriggerWaterSplash  0x4a1070
#define FUNC_CFx_TriggerBulletSplash 0x4a10e0
#define FUNC_CFx_TriggerFootSplash   0x4a1150
#define FUNC_FxSystem_c_AddParticle  0x4AA440

enum class eFXQuality : std::uint32_t
{
    QUALITY_LOW = 0,
    QUALITY_MEDIUM,
    QUALITY_HIGH,
    QUALITY_VERY_HIGH,
};

enum class eFxSystemPlayState : std::uint8_t
{
    PLAYING = 0,
    STOPPED,
    UNKNOWN,
};

enum class eFxSystemKillState : std::uint8_t
{
    NOT_KILLED = 0,
    PLAY_AND_KILL,
    KILLED,
    UNKNOWN,
};

class FxSystem_cSAInterface
{
public:
    std::uint32_t           m_link[2];            // ListItem_c
    CFxSystemBPSAInterface* m_bluePrint;
    void*                   m_transformMatrix;            // RwMatrixTag*
    std::uint8_t            m_baseMatrix[64];             // RwMatrixTag
    eFxSystemPlayState      m_playState;
    eFxSystemKillState      m_killState;
    bool                    m_useConstTime;
    std::uint8_t            field_53[2];
    float                   m_cameraDistance;
    std::uint16_t           m_constTime;
    std::uint16_t           m_rateMult;
    std::uint16_t           m_timeMult;

    union
    {
        struct
        {
            std::uint8_t m_hasOwnMatrix : 1;
            std::uint8_t m_local : 1;
            std::uint8_t m_useZTest : 1;
            std::uint8_t m_stopParticleCreation : 1;
            std::uint8_t m_prevCulled : 1;
            std::uint8_t m_mustCreateParticles : 1;
        };

        std::uint8_t flags;
    };

    std::uint8_t field_63;
    float        m_loopInterval;
    CVector      m_velAdd;
    void*        m_boundingSphere;            // CParticleBounding* or FxSphere_c*
    void**       m_primsList;                 // FxPrim_c**
    std::uint8_t m_fireAE[136];               // CAEFireAudioEntity
};

class CFxSAInterface
{
public:
    FxSystem_cSAInterface* m_fxSysBlood;
    FxSystem_cSAInterface* m_fxSysBoatSplash;
    FxSystem_cSAInterface* m_fxSysBubble;
    FxSystem_cSAInterface* m_fxSysDebris;
    FxSystem_cSAInterface* m_fxSysSmoke;
    FxSystem_cSAInterface* m_fxSysGunshell;
    FxSystem_cSAInterface* m_fxSysSand;
    FxSystem_cSAInterface* m_fxSysSand2;
    FxSystem_cSAInterface* m_fxSysSmokeHuge;
    FxSystem_cSAInterface* m_fxSysSmoke2;
    FxSystem_cSAInterface* m_fxSysSpark;
    FxSystem_cSAInterface* m_fxSysSpark2;
    FxSystem_cSAInterface* m_fxSysSplash;
    FxSystem_cSAInterface* m_fxSysWake;
    FxSystem_cSAInterface* m_fxSysWaterSplash;
    FxSystem_cSAInterface* m_fxSysWheelDirt;
    FxSystem_cSAInterface* m_fxSysGlass;

    // List_c<??>
    void*         m_lastParticleEntity;
    void*         m_firstParticleEntity;
    std::uint32_t m_particleEntitiesCount;

    std::uint32_t m_numCreatedBloodPools;
    eFXQuality    m_fxQuality;
    std::uint32_t m_verticesCount2;
    std::uint32_t m_verticesCount;
    std::uint32_t m_transformRenderFlags;
    RwRaster*     m_rasterToRender;
    RwMatrix*     m_transformLTM;
    void*         m_verts;            // RxObjSpace3DVertex*
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
    void AddParticle(eFxParticleSystems eFxParticle, const CVector& vecPosition, const CVector& vecDirection, float fR, float fG, float fB, float fA,
                     bool bRandomizeColors, std::uint32_t iCount, float fBrightness, float fSize, bool bRandomizeSizes, float fLife);

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
