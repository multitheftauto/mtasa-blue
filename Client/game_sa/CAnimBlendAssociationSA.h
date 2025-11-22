/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendAssociationSA.h
 *  PURPOSE:     Header file for animation blend association class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

/*CAnimBlendAssociation: 1 per running animation?*/

#pragma once

#include <CVector.h>
#include <game/RenderWare.h>
#include <game/CAnimBlendAssociation.h>
#include <game/CAnimBlendHierarchy.h>
#include "CAnimBlendNodeSA.h"

class CAnimBlendAssocGroupSA;
class CAnimBlendHierarchySAInterface;

struct RpHAnimBlendInterpFrame
{
    RtQuat orientation;
    RwV3d  translation;
};
class AnimBlendFrameData
{
public:
    union
    {
        struct
        {
            unsigned char m_bf1 : 1;                                             // doesn't seem to be used
            unsigned char m_IsIFrameOrientationToAffectedByNodes : 1;            // m_IFrame orientation will be affected
            unsigned char m_IsIFrameTranslationToAffectedByNodes : 1;            // m_IFrame translation will be affected
            unsigned char m_bIsInitialized : 1;
            unsigned char m_bUpdateSkinnedWith3dVelocityExtraction : 1;
            unsigned char m_bCheckBlendNodeClumpKeyFrames : 1;            // key frames of CAninBlendNode bones will be checked
            unsigned char m_bIsCompressed : 1;
            unsigned char m_bUpdatingFrame : 1;            // doesn't seem to be used
        };
        unsigned char m_nFlags;
    };
    char                     pad[3];
    CVector                  m_vecOffset;
    RpHAnimBlendInterpFrame* m_pIFrame;
    unsigned int             m_nNodeId;
};

struct SClumpAnimAssocSAInterface
{
    SClumpAnimAssocSAInterface*      m_pNext;
    SClumpAnimAssocSAInterface*      m_pPrevious;
    std::int16_t                     m_nNumBlendNodes;
    std::int16_t                     m_nAnimGroup;
    class CAnimBlendNodeSAInterface* m_pNodeArray;
    CAnimBlendHierarchySAInterface*  m_pHierarchy;
    float                            m_fBlendAmount;
    float                            m_fBlendDelta;
    float                            m_fCurrentTime;
    float                            m_fSpeed;
    float                            fTimeStep;
    std::int16_t                     m_nAnimId;
    std::int16_t                     m_nFlags;
};

class CAnimBlendClumpDataSAInterface
{
public:
    SClumpAnimAssocSAInterface* m_pFirstAssociation;
    int                         field_4;
    int                         m_dwNumBones;
    int                         field_C;
    AnimBlendFrameData*         m_frames;

    AnimBlendFrameData* GetFrameDataByNodeId(unsigned int nodeId);
};

class CAnimBlendAssociationSAInterface
{
public:
    DWORD*                          vTable;
    RwListEntry                     listEntry;
    unsigned short                  cNumBlendNodes;
    short                           sAnimGroup;                     // 14
    CAnimBlendNodeSAInterface*      pAnimBlendNodeArray;            // CAnimBlendNode pp?          // 16
    CAnimBlendHierarchySAInterface* pAnimHierarchy;                 // 20
    float                           fBlendAmount;                   // 24
    float                           fBlendDelta;                    // 28
    float                           fCurrentTime;                   // 32
    float                           fSpeed;                         // 36
    float                           fTimeStep;                      // 40
    short                           sAnimID;                        // 44
    union
    {
        struct
        {
            unsigned short m_bPlaying : 1;                    // Anim will stop playing if flag is not set
            unsigned short m_bLooped : 1;                     // Anim will always restart when it completes
            unsigned short m_bFreezeLastFrame : 1;            // Anim will freeze on last frame

            // When this flag is Set, IF m_bLooped == false, m_bPlaying == true, and anim progress >= total anim time
            // THEN m_bFreezeLastFrame is set to TRUE and m_fBlendDelta is set to -4.0.
            // Result: https://i.imgur.com/idw0jsX.png
            unsigned short m_bLockLastFrame : 1;

            unsigned short m_bPartial : 1;                   // Partial anims run along other anims
            unsigned short m_bEnableMovement : 1;            // blends all playing anims together if set

            // Camera will not move with ped, and ped will go back to the initial position when anim is over
            unsigned short m_bLockLastX : 1;
            unsigned short m_bLockLastY : 1;            // only applies if m_bLockLastX is set

            unsigned short m_bf9 : 1;             // doesn't seem to be used
            unsigned short m_bf10 : 1;            // doesn't seem to be used

            // If set to TRUE, then result:
            // Before = https://i.imgur.com/c8T7xNK.png | AFTER = https://i.imgur.com/4gqlA4n.png
            unsigned short m_bAddAnimBlendToTotalBlend : 1;

            unsigned short m_bf12 : 1;                          // doesn't seem to be used
            unsigned short m_bSecondaryTaskAnim : 1;            // doesn't seem to be used

            // Anim will play. Translation values will be ignored for anim (m_bEnableTranslation
            // is ignored if set), and it only applies to some anims though
            unsigned short m_bFreezeTranslation : 1;
            unsigned short m_bBlockReferenced : 1;            // anim block can't be unloaded if it's referenced by an anim.

            // Anim will not be destroyed. It will be played simultaneously with other anims
            // (multiple anims at once) if you set this flag while sitting in a car, you'll still be
            // seated even if you exit the vehicle or play other anims, like walking, running, crouching,
            // etc, like this: https://i.imgur.com/VgWn5fl.png
            unsigned short m_bIndestructible : 1;
        };
        unsigned short m_nFlags;
    };
    unsigned int uiCallbackType;            // 48:  1 means finished and 2 means delete
    DWORD*       pCallbackFunc;             // 52
    DWORD*       pCallbackData;             // 56
                                            // Total: 60 bytes
};
static_assert(sizeof(CAnimBlendAssociationSAInterface) == 0x3C, "Incorrect class size: CAnimBlendAssociationSAInterface");

class CAnimBlendAssociationSA : public CAnimBlendAssociation
{
public:
    CAnimBlendAssociationSA(CAnimBlendAssociationSAInterface* pInterface) { m_pInterface = pInterface; }

    CAnimBlendAssociationSAInterface*    Constructor(CAnimBlendStaticAssociationSAInterface& StaticAssociationByReference);
    CAnimBlendAssociationSAInterface*    Constructor(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy);
    CAnimBlendAssociationSAInterface*    InitializeForCustomAnimation(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy);
    void                                 Init(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy);
    void                                 AllocateAnimBlendNodeArray(int count);
    void                                 FreeAnimBlendNodeArray();
    CAnimBlendAssociationSAInterface*    GetInterface() { return m_pInterface; }
    eAnimGroup                           GetAnimGroup() { return static_cast<eAnimGroup>(m_pInterface->sAnimGroup); }
    eAnimID                              GetAnimID() { return static_cast<eAnimID>(m_pInterface->sAnimID); }
    std::unique_ptr<CAnimBlendHierarchy> GetAnimHierarchy();
    const std::unique_ptr<CAnimBlendHierarchy> GetAnimHierarchy() const noexcept;

    float GetBlendAmount() { return m_pInterface->fBlendAmount; }
    void  SetBlendAmount(float fAmount) { m_pInterface->fBlendAmount = fAmount; }
    void  SetCurrentProgress(float fProgress);
    float GetCurrentProgress() const noexcept { return m_pInterface->fCurrentTime; }
    float GetCurrentSpeed() const noexcept { return m_pInterface->fSpeed; }
    void  SetCurrentSpeed(float fSpeed) { m_pInterface->fSpeed = fSpeed; }
    float GetLength() const noexcept { return GetAnimHierarchy()->GetTotalTime(); }
    void  SetAnimID(short sAnimID) { m_pInterface->sAnimID = sAnimID; }
    void  SetAnimGroup(short sAnimGroup) { m_pInterface->sAnimGroup = sAnimGroup; }

    short GetFlags() const override { return m_pInterface->m_nFlags; }
    void  SetFlags(short sFlags) { m_pInterface->m_nFlags = sFlags; }

protected:
    CAnimBlendAssociationSAInterface* m_pInterface;
};
