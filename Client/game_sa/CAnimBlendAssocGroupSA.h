/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendAssocGroupSA.h
 *  PURPOSE:     Header file for animation blend association group class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAnimBlendAssocGroup.h>
#include "CAnimBlendStaticAssociationSA.h"

class CAnimBlendAssociationSAInterface;
class CAnimBlockSA;
class CAnimBlockSAInterface;
class CAnimManagerSA;

#define FUNC_CAnimBlendAssocGroup_InitEmptyAssociations 0x4cdfb0
#define FUNC_CAnimBlendAssocGroup_CopyAnimation         0x4ce130
#define FUNC_CAnimBlendAssocGroup_IsCreated             0x4d37a0
#define FUNC_CAnimBlendAssocGroup_GetNumAnimations      0x45b050
#define FUNC_CAnimBlendAssocGroup_GetAnimation          0x4ce090
#define FUNC_CAnimBlendAssocGroup_CreateAssociations    0x4ce220

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

    CAnimBlendAssociationSAInterface* CopyAnimation(unsigned int AnimID);
    void                              InitEmptyAssociations(RpClump* pClump);
    bool                              IsCreated();
    int                               GetNumAnimations();
    CAnimBlock*                       GetAnimBlock();
    CAnimBlendStaticAssociation*      GetAnimation(unsigned int ID);
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
