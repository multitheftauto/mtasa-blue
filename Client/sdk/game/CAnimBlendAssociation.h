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

#ifndef __CAnimBlendAssociation_H
#define __CAnimBlendAssociation_H

class CAnimBlendAssociationSAInterface;
class CAnimBlendStaticAssociationSAInterface;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;
class CAnimBlendHierarchy;
class CAnimBlendHierarchySAInterface;
struct RpClump;

class CAnimBlendAssociation
{
public:
    virtual CAnimBlendAssociationSAInterface* Constructor(CAnimBlendStaticAssociationSAInterface& StaticAssociationByReference) = 0;
    virtual CAnimBlendAssociationSAInterface* Constructor(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy) = 0;
    virtual CAnimBlendAssociationSAInterface* GetInterface(void) = 0;
    virtual AssocGroupId                      GetAnimGroup(void) = 0;
    virtual AnimationId                       GetAnimID(void) = 0;
    virtual CAnimBlendHierarchy*              GetAnimHierarchy(void) = 0;

    virtual float GetBlendAmount(void) = 0;
    virtual void  SetBlendAmount(float fAmount) = 0;
    virtual void  SetCurrentProgress(float fProgress) = 0;
    virtual void  SetAnimID(short sAnimID) = 0;
    virtual void  SetAnimGroup(short sAnimGroup) = 0;
    virtual void  SetFlags(short sFlags) = 0;
};

#endif
