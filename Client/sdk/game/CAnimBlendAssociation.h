/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAnimBlendAssociation.h
 *  PURPOSE:     Animation blend association interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
    virtual void  SetFlags(short sFlags) = 0;
};
