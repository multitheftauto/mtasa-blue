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

#include <bitset>
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
    virtual bool  IsPartial() const = 0;
    // Drops any blend node for a bone the original animation doesn't animate, so a custom replacement
    // for a partial anim can't drive the bones (root, pelvis, legs) the original left to the movement anim.
    virtual void RestrictToBonesOf(const CAnimBlendStaticAssociationSAInterface* pOriginalAssoc) = 0;
    // Same idea as RestrictToBonesOf, but driven directly by a bone mask (bit i set means bone i is
    // animated) instead of comparing against another association. Used for custom animations played
    // from a custom IFP bank, which have no built-in association to compare against.
    virtual void  RestrictToBones(std::bitset<32> animatedBonesMask) = 0;
    virtual void  SetCurrentProgress(float fProgress) = 0;
    virtual float GetCurrentProgress() const noexcept = 0;
    virtual void  SetCurrentSpeed(float fSpeed) = 0;
    virtual float GetCurrentSpeed() const noexcept = 0;
    virtual float GetLength() const noexcept = 0;
    virtual void  SetAnimID(short sAnimID) = 0;
    virtual void  SetAnimGroup(short sAnimGroup) = 0;
    virtual void  SetFlags(short sFlags) = 0;
};
