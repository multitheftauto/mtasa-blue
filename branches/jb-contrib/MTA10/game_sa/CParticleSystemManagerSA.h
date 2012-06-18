/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSystemManagerSA.h
*  PURPOSE:     Header file for particle system manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PARTICLESYSTEMMANAGER
#define __CGAMESA_PARTICLESYSTEMMANAGER

#include <game/CParticleSystemManager.h>

/* <TEMP DOCS> */
/*
// TODO: Find more sensible names for some classes(CParticleManager)
// TODO: Cleanup old unused CParticle stuff

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
                    
// Particle Emitters & Activators
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
// Particle System layout
// CParticleSystemManager::AddSparks -> CParticleFx::Add -> (virtual by array)CParticleEmitterManager::AddParticleEffect -> CParticleManager::ActivateParticleEmitter -> (virtual by array)CParticleEmitter::Activate

*/
/* </TEMP DOCS> */

#define CLASS_CParticleSystemMgr                                0xA9AE00

// <Particle System Root>
class CParticleSystemManagerSAInterface
{
public:
    // Basic particle types
    /*
        [0] = "prt_blood"
        [1] = "prt_boatsplash"
        [2] = "prt_bubble"
        [3] = "prt_carderbis"
        [4] = "prt_collisionsmoke"
        [5] = "prt_gunshell"
        [6] = "prt_sand"
        [7] = "prt_sand2"
        [8] = "prt_smoke_huge"
        [9] = "prt_smokeII_3_expand"
        [10] = "prt_spark"
        [11] = "prt_spark_2" (blurred)
        [12] = "prt_splash"
        [13] = "prt_wake"
        [14] = "prt_watersplash"
        [15] = "prt_wheeldirt"
        [16] = "prt_glass"
    */
    class CParticleFxSAInterface* pParticleFxArray[16];
    uint32 pad1[4];
    // Render stuff
    uint32 uiDrawDistance;
    uint32 verticesCount2;
    uint32 verticesCount;
    uint32 transformRenderFlags; // RwIm3DTransform flags
    RwRaster* pRasterToRender;
    RwMatrix* transformLTM; // RwIm3DTransform LTM
    class RxObjSpace3DVertex* pVerts; // vertices to render

    uint32_t pad2[4];
};
C_ASSERT(sizeof(CParticleSystemManagerSAInterface) == 0x80);

class CParticleSystemManagerSA : public CParticleSystemManager
{

};

#endif