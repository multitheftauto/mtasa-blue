/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFxSA.h
*  PURPOSE:     Game effects handling
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFXSA_H
#define __CFXSA_H

#include <game/CFx.h>

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


/**
General Info:
    effects.fxp loading procedures
        005C2420 CParticleData::getEffectsFxp
            005C1F50 CParticleData::ReadEffectsFile
                005C05F0 ReadEffectsFxp_1
                005C2010 CParticleEmitter::LoadData
                005C25F0 CParticleEmitter::LoadData_LOD
                005C0B70 ReadEffectsFxp_2
                (virtual: load CParticleEmitters by calling their VTBLs at + 0x04 ( CParticleEmitter::LoadData )
                          which in turns calls its CParticleActivator VTBLs at + 0x04 ( CParticleActivator::LoadData ) )
                    
// Particle Info Data
//      ID<->Name map

 [ID]         [NAME]
0x1001 = FX_INFO_EMRATE_DATA
0x1004 = FX_INFO_EMSIZE_DATA
0x1008 = FX_INFO_EMSPEED_DATA
0x1010 = FX_INFO_EMDIR_DATA
0x1020 = FX_INFO_EMANGLE_DATA
0x1040 = FX_INFO_EMLIFE_DATA
0x1080 = FX_INFO_EMPOS_DATA
0x1100 = FX_INFO_EMWEATHER_DATA
0x1200 = FX_INFO_EMROTATION_DATA
0x2001 = FX_INFO_NOISE_DATA
0x2002 = FX_INFO_FORCE_DATA
0x2004 = FX_INFO_FRICTION_DATA
0x2008 = FX_INFO_ATTRACTPT_DATA
0x2010 = FX_INFO_ATTRACTLINE_DATA
0x2020 = FX_INFO_GROUNDCOLLIDE_DATA
0x2040 = FX_INFO_WIND_DATA
0x2080 = FX_INFO_JITTER_DATA
0x2100 = FX_INFO_ROTSPEED_DATA
0x2200 = FX_INFO_FLOAT_DATA
0x2400 = FX_INFO_UNDERWATER_DATA
0x4001 = FX_INFO_COLOUR_DATA
0x4002 = FX_INFO_SIZE_DATA
0x4004 = FX_INFO_SPRITERECT_DATA
0x4008 = FX_INFO_HEATHAZE_DATA
0x4010 = FX_INFO_TRAIL_DATA
0x4020 = FX_INFO_FLAT_DATA
0x4040 = FX_INFO_DIR_DATA
0x4080 = FX_INFO_ANIMTEX_DATA
0x4100 = FX_INFO_COLOURRANGE_DATA
0x4200 = FX_INFO_SELFLIT_DATA
0x4400 = FX_INFO_COLOURBRIGHT_DATA
0x8001 = FX_INFO_SMOKE_DATA
-------------------------------------
// Particle System activation
// CFx::AddSparks -> CFxSystem::AddParticle -> (virtual by array)CParticleEmitterManager::AddParticleEffect -> CParticleManager::ActivateParticleEmitter -> (virtual by array)CParticleEmitter::Activate

*/

/**
 *  File-scope limited string array mapping of common particle fx systems from IDs to names. 
 */
static SString saCommonFxSystemMap[17] =
{
    "prt_blood",            "prt_boatsplash",       "prt_bubble",
    "prt_carderbis",        "prt_collisionsmoke",   "prt_gunshell",
    "prt_sand",             "prt_sand2",            "prt_smoke_huge", 
    "prt_smokeII_3_expand", "prt_spark",            "prt_spark_2", 
    "prt_splash",           "prt_wake",             "prt_watersplash",
    "prt_wheeldirt",        "prt_glass"
};

class CFxSAInterface
{
public:
    /**
     *  Common Particle Fx Systems that are initialised because of their often use.
     *  List:
     *
     *   [0] = "prt_blood"
     *   [1] = "prt_boatsplash"
     *   [2] = "prt_bubble"
     *   [3] = "prt_carderbis"
     *   [4] = "prt_collisionsmoke"
     *   [5] = "prt_gunshell"
     *   [6] = "prt_sand"
     *   [7] = "prt_sand2"
     *   [8] = "prt_smoke_huge"
     *   [9] = "prt_smokeII_3_expand"
     *   [10] = "prt_spark"
     *   [11] = "prt_spark_2" (blurred)
     *   [12] = "prt_splash"
     *   [13] = "prt_wake"
     *   [14] = "prt_watersplash"
     *   [15] = "prt_wheeldirt"
     *   [16] = "prt_glass"
     */
    class CFxSystem* pCommonFxSystemArray[17];
    /**
     *  Unknown.
     */
    uint32 pad1[4];
    /**
     *  Effects draw distance which is read from and written to local player settings file.
     */
    uint32 uiDrawDistance;
    uint32 verticesCount2;
    uint32 verticesCount;
    /**
     *  Transform flags which specify options available for controlling execution of the 3D immediate mode pipeline.
     *  Refer to Renderware SDK documentation for possible options.(file: rwcore.h)
     */
    uint32 transformRenderFlags; // RwIm3DTransform flags
    /**
     *  Particle texture raster that is being rendered.
     */
    RwRaster* pRasterToRender;
    /**
     *  Lower Triangular Matrix that transform particles from object space to world space.
     */
    RwMatrix* transformLTM; // RwIm3DTransform LTM
    /**
     *  Vertices that are used as temporary storage for ordered rendering primitives that will be rendered.
     */
    class RxObjSpace3DVertex* pVerts;
    /**
     *  Unknown.
     */
    uint32 pad2[4];
};
C_ASSERT(sizeof(CFxSAInterface) == 0x80); /** Safeguard */

class CFxSA : public CFx
{
public:
                        CFxSA                  ( CFxSAInterface * pInterface )    { m_pInterface = pInterface; }

    void                AddBlood                ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness );
    void                AddWood                 ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness );
    void                AddSparks               ( CVector & vecPosition, CVector & vecDirection, float fForce, int iCount, CVector vecAcrossLine, unsigned char ucBlurIf0, float fSpread, float fLife );
    void                AddTyreBurst            ( CVector & vecPosition, CVector & vecDirection );
    void                AddBulletImpact         ( CVector & vecPosition, CVector & vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity );
    void                AddPunchImpact          ( CVector & vecPosition, CVector & vecDirection, int );
    void                AddDebris               ( CVector & vecPosition, RwColor & rwColor, float fScale, int iCount );
    void                AddGlass                ( CVector & vecPosition, RwColor & rwColor, float fScale, int iCount );
    void                TriggerWaterHydrant     ( CVector & vecPosition );
    void                TriggerGunshot          ( CEntity * pEntity, CVector & vecPosition, CVector & vecDirection, bool bIncludeSparks );
    void                TriggerTankFire         ( CVector & vecPosition, CVector & vecDirection );
    void                TriggerWaterSplash      ( CVector & vecPosition );
    void                TriggerBulletSplash     ( CVector & vecPosition );
    void                TriggerFootSplash       ( CVector & vecPosition );
    CFxSystem*          GetCommonFxSystem       ( const SString & strName );
private:
    CFxSAInterface *   m_pInterface;
};

#endif
