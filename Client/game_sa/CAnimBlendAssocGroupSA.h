/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAnimBlendAssocGroupSA.h
 *  PURPOSE:     Header file for animation blend association group class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAnimBlendAssocGroup.h>
#include "CAnimBlendStaticAssociationSA.h"

class CAnimBlendAssociationSAInterface;
class CAnimBlockSA;
class CAnimBlockSAInterface;
class CAnimManagerSA;

class CAnimationStyleDescriptorSAInterface
{
public:
    char  groupName[16];
    char  blockName[16];
    int   field_20;
    int   animsCount;
    void* animNames;
    void* animDesc;
};

class CAnimBlendAssocGroupSAInterface
{
public:
    CAnimBlockSAInterface*                  pAnimBlock;
    CAnimBlendStaticAssociationSAInterface* pAssociationsArray;
    int                                     iNumAnimations;
    int                                     iIDOffset;
    AssocGroupId                            groupID;
};

class CAnimBlendAssocGroupSA : public CAnimBlendAssocGroup
{
    friend class CAnimManagerSA;

public:
    CAnimBlendAssocGroupSA(CAnimBlendAssocGroupSAInterface* pInterface);

    CAnimBlendAssociationSAInterface* CopyAnimation(uint AnimID);
    void                              InitEmptyAssociations(RpClump* pClump);
    bool                              IsCreated();
    int                               GetNumAnimations();
    CAnimBlock*                       GetAnimBlock();
    CAnimBlendStaticAssociation*      GetAnimation(uint ID);
    eAnimGroup                        GetGroupID();
    void                              CreateAssociations(const char* szBlockName);

    bool IsLoaded();
    void SetIDOffset(int iOffset) { m_pInterface->iIDOffset = iOffset; }

    CAnimBlendAssocGroupSAInterface* GetInterface() { return m_pInterface; }

protected:
    void SetupAnimBlock();

    CAnimBlendAssocGroupSAInterface* m_pInterface;
    CAnimBlockSA*                    m_pAnimBlock;
};
