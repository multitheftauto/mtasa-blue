/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAnimBlendAssociation.h
 *  PURPOSE:     Animation blend association interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <memory>

typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;

class CAnimBlendAssociationSAInterface;
class CAnimBlendHierarchy;
class CAnimBlendHierarchySAInterface;
class CAnimBlendStaticAssociationSAInterface;
enum class eAnimGroup;
enum class eAnimID;
struct Rpclump;

enum eAnimationFlags
{
    ANIMATION_DEFAULT = 0,
    ANIMATION_IS_PLAYING = 0x1,  // (0x5DED30) CPed::StopNonPartialAnims()
    ANIMATION_IS_LOOPED = 0x2,   // (0x5B042F) CCutsceneMgr::SetCutsceneAnimToLoop
    ANIMATION_IS_BLEND_AUTO_REMOVE =
        0x4,  // (0x5E4B12) CPed::SetMoveAnim() - Automatically `delete this` once faded out (`m_BlendAmount <= 0 && m_BlendDelta <= 0`)
    ANIMATION_IS_FINISH_AUTO_REMOVE = 0x8,   // (0x5E4B00) CPed::SetMoveAnim() - Animation will be stuck on last frame, if not set
    ANIMATION_IS_PARTIAL = 0x10,             // (0x5DED28) CPed::StopNonPartialAnims()
    ANIMATION_IS_MOVING = 0x20,              // (0x4D4651) CAnimManager::BlendAnimation()
    ANIMATION_TRANSLATE_Y = 0x40,            // (0x4D1773)
    ANIMATION_TRANSLATE_X = 0x80,            // (0x4D17A2)
    ANIMATION_PED_WALK = 0x100,              // PEDFLAG (0x5E58A5) CPed::PlayFootSteps()
    ANIMATION_PED_UPPERBODYONLY = 0x400,     // PEDFLAG (0x5E58BB) CPed::PlayFootSteps()
    ANIMATION_FORWARD_ANIM = 0x800,          // PEDFLAG (0x678579) CTaskSimpleFall::ProcessFall()
    ANIMATION_SECONDARY_TASK_ANIM = 0x1000,  // (0x64F688) CCarEnterExit::RemoveCarSitAnim()
    ANIMATION_FREEZE_TRANSLATION = 0x2000,   // (0x4D21EB)
    ANIMATION_BLOCK_REFERENCED = 0x4000,     // (0x4CEA59) CAnimBlendAssociation::ReferenceAnimBlock()
    ANIMATION_IS_INDESTRUCTIBLE = 0x8000,    // (0x4D4687) CAnimManager::BlendAnimation() - The animation is never destroyed if this flag is set
};

class CAnimBlendAssociation
{
public:
    virtual CAnimBlendAssociationSAInterface*    Constructor(CAnimBlendStaticAssociationSAInterface& StaticAssociationByReference) = 0;
    virtual CAnimBlendAssociationSAInterface*    Constructor(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy) = 0;
    virtual void                                 Init(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy) = 0;
    virtual CAnimBlendAssociationSAInterface*    InitializeForCustomAnimation(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy) = 0;
    virtual void                                 FreeAnimBlendNodeArray() = 0;
    virtual CAnimBlendAssociationSAInterface*    GetInterface() = 0;
    virtual eAnimGroup                           GetAnimGroup() = 0;
    virtual eAnimID                              GetAnimID() = 0;
    virtual std::unique_ptr<CAnimBlendHierarchy> GetAnimHierarchy() = 0;

    virtual float GetBlendAmount() = 0;
    virtual void  SetBlendAmount(float fAmount) = 0;
    virtual void  SetCurrentProgress(float fProgress) = 0;
    virtual float GetCurrentProgress() const noexcept = 0;
    virtual void  SetCurrentSpeed(float fSpeed) = 0;
    virtual float GetCurrentSpeed() const noexcept = 0;
    virtual float GetLength() const noexcept = 0;
    virtual void  SetAnimID(short sAnimID) = 0;
    virtual void  SetAnimGroup(short sAnimGroup) = 0;
    virtual short GetFlags() const = 0;
    virtual void  SetFlags(short sFlags) = 0;
};
