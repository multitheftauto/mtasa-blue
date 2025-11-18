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
    ANIMATION_DEFAULT = 0,                               // 0x0,
    ANIMATION_IS_PLAYING = 1 << 0,                       // 0x1,
    ANIMATION_IS_LOOPED = 1 << 1,                        // 0x2,
    ANIMATION_IS_BLEND_AUTO_REMOVE = 1 << 2,             //!< (0x4) Automatically `delete this` once faded out (`m_BlendAmount <= 0 && m_BlendDelta <= 0`)
    ANIMATION_IS_FINISH_AUTO_REMOVE = 1 << 3,            // 0x8,  // Animation will be stuck on last frame, if not set
    ANIMATION_IS_PARTIAL = 1 << 4,                      // 0x10,
    ANIMATION_IS_SYNCRONISED = 1 << 5,                    // 0x20,
    ANIMATION_CAN_EXTRACT_VELOCITY = 1 << 6,              // 0x40,
    ANIMATION_CAN_EXTRACT_X_VELOCITY = 1 << 7,            // 0x80,

    // ** User defined flags **
    ANIMATION_WALK = 1 << 8,                                  // 0x100,
    ANIMATION_200 = 1 << 9,                                   // 0x200,
    ANIMATION_DONT_ADD_TO_PARTIAL_BLEND = 1 << 10,            // 0x400,
    ANIMATION_IS_FRONT = 1 << 11,                             // 0x800,
    ANIMATION_SECONDARY_TASK_ANIM = 1 << 12,                  // 0x1000,
    // **

    ANIMATION_IGNORE_ROOT_TRANSLATION = 1 << 13,            // 0x2000,
    ANIMATION_REFERENCE_BLOCK = 1 << 14,                    // 0x4000,
    ANIMATION_FACIAL = 1 << 15,                             // 0x8000 // The animation is never destroyed if this flag is set, NO MATTER WHAT
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
