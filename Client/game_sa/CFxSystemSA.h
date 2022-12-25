/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFxSystemSA.h
 *  PURPOSE:     Game effects interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CFxSystem.h>
#include <game/RenderWare.h>

#define FUNC_FxSystem_c__PlayAndKill            		0x4AA3D0
#define VAR_FxSystemUpdateCullDistMultiplier            0x4AB032
#define VAR_FxCreateParticleCullDistMultiplierA         0x4A4247
#define VAR_FxCreateParticleCullDistMultiplierB         0x4A4255
#define FX_SYSTEM_UPDATE_CULL_DIST_MULTIPLIER_DEFAULT   ( 1 / 256.f )
#define FX_CREATE_PARTICLE_CULL_DIST_MULTIPLIER_DEFAULT ( 1 / 64.f )

class CAEFireAudioEntitySAInterface
{
public:
    // this derives from CAEAudioEntity
    // todo: reverse this structure and CAEAudioEntity
    std::int32_t pad1[0x21];

    void* audio;            // 0x84 // TODO: Reverse what this __exactly__ is
};
static_assert(sizeof(CAEFireAudioEntitySAInterface) == 0x88, "Invalid size for CAEFireAudioEntitySAInterface");

class FxInfoSAInterface;
class CFxSystemBPSAInterface;

class CFxSystemSAInterface            // Internal SA Name: FxSystem_c
{
public:
    // Based upon ListItem_c
    CFxSystemSAInterface* pPrevious;            // 0x00
    CFxSystemSAInterface* pNext;                // 0x04

    // Actual members
    CFxSystemBPSAInterface*       pBlueprint;                    // 0x08
    RwMatrix*                     pmatUnknown;                   // 0x10
    RwMatrix                      matPosition;                   // 0x0C
    char                          cPlayStatus;                   // 0x50
    char                          cUnknownFlag;                  // 0x51
    char                          cConstTime;                    // 0x52
    char                          pad;                           // 0x53
    int                           unk2[2];                       // 0x54
    short                         sConstTimeMult;                // 0x5C
    short                         sRateMult;                     // 0x5E (This controls how often the effect is renewed)
    short                         sTimeMult;                     // 0x60 (This controls how fast the effect plays)
    char                          cFlags;                        // 0x62
    char                          pad2;                          // 0x63
    int                           unk3;                          // 0x64
    CVector                       vecVelocity;                   // 0x68
    void*                         pSphere;                       // 0x74
    void**                        ppParticleEmitters;            // 0x78 (Array of particle emitters, amount is defined by the blueprint)
    CAEFireAudioEntitySAInterface audioEntity;                   // 0x7C
};
static_assert(sizeof(CFxSystemSAInterface) == 0x104, "Invalid size for CFxSystemSAInterface");

class CFxSystemSA : public CFxSystem
{
public:
    CFxSystemSA(CFxSystemSAInterface* pInterface);
    ~CFxSystemSA();

    void PlayAndKill();

    void  SetEffectSpeed(float fSpeed);
    float GetEffectSpeed();

    void  SetEffectDensity(float fDensity);
    float GetEffectDensity();

    void GetPosition(CVector& vecPos);
    void SetPosition(const CVector& vecPos);

    void GetMatrix(CMatrix& matrix);
    void SetMatrix(const CMatrix& matrix);

    void  SetDrawDistance(float fDrawDistance);
    float GetDrawDistance();
    bool  HasCustomDrawDistance();

    void* GetInterface() { return (void*)m_pInterface; }

    static void StaticSetHooks();

    static void SetPosition(CFxSystemSAInterface* fxSystem, const CVector& position);

protected:
    CFxSystemSAInterface* m_pInterface;
    float                 m_fDrawDistance;
};

class FxInfoManagerSAInterface
{
public:
    uint32_t           m_nNumInfos;                     // 0x00
    FxInfoSAInterface* m_pInfos;                        // 0x04
    uint8_t            m_nFirstMovementInfo;            // 0x08
    uint8_t            m_nFirstRenderInfo;              // 0x09
};
static_assert(sizeof(FxInfoManagerSAInterface) == 0xC, "Invalid size for FxInfoManagerSAInterface");

class FxPrimBPSAInterface
{
public:
    void*                    vtbl;                                  // 0x00
    uint8_t                  field_4;                               // 0x04
    uint8_t                  m_nSrcBlendId;                         // 0x05
    uint8_t                  m_nDstBlendId;                         // 0x06
    uint8_t                  m_bAlphaOn;                            // 0x07
    void*                    m_pCompressedInitialMatrix;            // 0x08
    RwTexture*               m_apTextures[4];                       // 0x0C
    void*                    field_1C;                              // 0x1C
    uint32_t                 particlesList[3];                      // 0x20 -- List_c
    FxInfoManagerSAInterface m_infoManager;                         // 0x2C
};
static_assert(sizeof(FxPrimBPSAInterface) == 0x38, "Invalid size for FxPrimBPSAInterface");

class CFxEmitterBPSAInterface : public FxPrimBPSAInterface
{
public:
    uint16_t m_nLodStart;                       // 0x38
    uint16_t m_nLodEnd;                         // 0x3A
    bool     m_bHasInfoFlatData;                // 0x3C
    bool     m_bHasInfoHeatHazeData;            // 0x3C
};
static_assert(sizeof(CFxEmitterBPSAInterface) == 0x40, "Invalid size for CFxEmitterBPSAInterface");

class CFxEmitterSAInterface
{
public:
    void*                    vtbl;                  // 0x00
    CFxEmitterBPSAInterface* pBlueprint;            // 0x04
    CFxSystemSAInterface*    pOwner;                // 0x08
    // TODO the rest
};
