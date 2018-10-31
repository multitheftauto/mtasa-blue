/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAnimBlendAssocGroup.h
 *  PURPOSE:     Animation blend association group interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

typedef unsigned char BYTE;
typedef unsigned long AssocGroupId;
class CAnimBlendAssociation;
class CAnimBlendStaticAssociation;
struct RpClump;
class CAnimBlock;

class CAnimBlendAssocGroup
{
public:
    virtual CAnimBlendAssociationSAInterface* CopyAnimation(unsigned int AnimID) = 0;
    virtual void                              InitEmptyAssociations(RpClump* pClump) = 0;
    virtual bool                              IsCreated(void) = 0;
    virtual int                               GetNumAnimations(void) = 0;
    virtual CAnimBlock*                       GetAnimBlock(void) = 0;
    virtual CAnimBlendStaticAssociation*      GetAnimation(unsigned int ID) = 0;
    virtual AssocGroupId                      GetGroupID(void) = 0;
    virtual void                              CreateAssociations(const char* szBlockName) = 0;
    virtual bool                              IsLoaded(void) = 0;
    virtual void                              SetIDOffset(int iOffset) = 0;
};
