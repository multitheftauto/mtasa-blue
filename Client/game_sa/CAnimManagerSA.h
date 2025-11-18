/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAnimManagerSA.h
 *  PURPOSE:     Header file for animation manager class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CAnimManager.h>
#include <game/CAnimBlendAssociation.h>
#include <game/CAnimBlendHierarchy.h>
#include <game/CAnimBlock.h>
#include <game/CAnimBlendAssocGroup.h>

#define FUNC_CAnimManager_Initialize                        0x5bf6b0
#define FUNC_CAnimManager_Shutdown                          0x4d4130
#define FUNC_CAnimManager_GetAnimationBlock_int             0x406f90
#define FUNC_CAnimManager_GetAnimationBlock_str             0x4d3940
#define FUNC_CAnimManager_GetAnimationBlockIndex            0x4d3990
#define FUNC_CAnimManager_RegisterAnimBlock                 0x4d3e50
#define FUNC_CAnimManager_GetAnimBlendAssoc                 0x45b0e0
#define FUNC_CAnimManager_GetFirstAssocGroup                0x4d39b0
#define FUNC_CAnimManager_GetAnimation_int                  0x4cde50
#define FUNC_CAnimManager_GetAnimation_str_block            0x4d42f0
#define FUNC_CAnimManager_GetAnimation_int_block            0x4d39f0
#define FUNC_CAnimManager_GetAnimGroupName                  0x4d3a20
#define FUNC_CAnimManager_GetAnimBlockName                  0x4d3a30
#define FUNC_CAnimManager_AddAnimBlockRef                   0x4d3fb0
#define FUNC_CAnimManager_RemoveAnimBlockRef                0x4d3fd0
#define FUNC_CAnimManager_RemoveAnimBlockRefWithoutDelete   0x4d3ff0
#define FUNC_CAnimManager_GetNumRefsToAnimBlock             0x4d4010
#define FUNC_CAnimManager_RemoveAnimBlock                   0x4d3f40
#define FUNC_CAnimManager_RemoveLastAnimFile                0x4d3ed0
#define FUNC_CAnimManager_AddAnimation                      0x4d3aa0
#define FUNC_CAnimManager_AddAnimation_hier                 0x4d4330
#define FUNC_CAnimManager_BlendAnimation                    0x4d4610
#define FUNC_CAnimManager_BlendAnimation_hier               0x4d4410
#define FUNC_CAnimManager_AddAnimationAndSync               0x4d3b30
#define FUNC_CAnimManager_CreateAnimAssociation             0x4d3a40
#define FUNC_CAnimManager_GetAnimAssociation                0x4d3a60
#define FUNC_CAnimManager_GetAnimAssociation_str            0x4d3a80
#define FUNC_CAnimManager_AddAnimAssocDefinition            0x4d3ba0
#define FUNC_CAnimManager_ReadAnimAssociationDefinitions    0x5bc910
#define FUNC_CAnimManager_CreateAnimAssocGroups             0x4d3cc0
#define FUNC_CAnimManager_UncompressAnimation               0x4d41c0
#define FUNC_CAnimManager_RemoveFromUncompressedCache       0x4d42a0
#define FUNC_CAnimManager_LoadAnimFile                      0x4d55d0
#define FUNC_CAnimManager_LoadAnimFile_stream               0x4d47f0
#define FUNC_CAnimManager_LoadAnimFiles                     0x4d5620
#define FUNC_CAnimManager_AllocateKeyFramesMemory           0x72F420
#define FUNC_CAnimManager_FreeKeyFramesMemory               0x72F430
#define ARRAY_CAnimManager_AnimAssocGroups                  0xb4ea34
#define ARRAY_CAnimManager_Animations                       0xb4ea40
#define ARRAY_CAnimManager_AnimBlocks                       0xb5d4a0
#define VAR_CAnimManager_NumAnimAssocDefinitions            0xb4ea28
#define VAR_CAnimManager_NumAnimations                      0xb4ea2c
#define VAR_CAnimManager_NumAnimBlocks                      0xb4ea30

// Non members
#define FUNC_HasAnimGroupLoaded                             0x45b130
#define FUNC_RpAnimBlendClumpGetFirstAssociation            0x4d15e0
#define FUNC_RpAnimBlendClumpGetAssociation_str             0x4d6870
#define FUNC_RpAnimBlendClumpGetAssociation_int             0x4d68b0
#define FUNC_RpAnimBlendGetNextAssociation                  0x4d6ab0
#define FUNC_RpAnimBlendClumpGetNumAssociations             0x4d6b60
#define FUNC_RpAnimBlendClumpUpdateAnimations               0x4d34f0

class CAnimManagerSA : public CAnimManager
{
    typedef std::unique_ptr<CAnimBlendStaticAssociation> StaticAssocIntface_type;

public:
    CAnimManagerSA();
    ~CAnimManagerSA();

    void Initialize();
    void Shutdown();

    int GetNumAnimations();
    int GetNumAnimBlocks();
    int GetNumAnimAssocDefinitions();

    std::unique_ptr<CAnimBlendHierarchy> GetAnimation(int ID);
    std::unique_ptr<CAnimBlendHierarchy> GetAnimation(const char* szName, std::unique_ptr<CAnimBlock>& pBlock);
    std::unique_ptr<CAnimBlendHierarchy> GetAnimation(unsigned int uiIndex, std::unique_ptr<CAnimBlock>& pBlock);

    std::unique_ptr<CAnimBlock> GetAnimationBlock(int ID);
    std::unique_ptr<CAnimBlock> GetAnimationBlock(const char* szName);
    int                         GetAnimationBlockIndex(const char* szName);
    int                         RegisterAnimBlock(const char* szName);

    std::unique_ptr<CAnimBlendAssocGroup> GetAnimBlendAssoc(AssocGroupId groupID) const;
    AssocGroupId                          GetFirstAssocGroup(const char* szName);

    const char* GetAnimGroupName(AssocGroupId groupID);
    const char* GetAnimBlockName(AssocGroupId groupID);

    std::unique_ptr<CAnimBlendAssociation> CreateAnimAssociation(AssocGroupId animGroup, AnimationId animID);
    StaticAssocIntface_type                GetAnimStaticAssociation(eAnimGroup animGroup, eAnimID animID) const;
    std::unique_ptr<CAnimBlendAssociation> GetAnimAssociation(AssocGroupId animGroup, const char* szAnimName);
    std::unique_ptr<CAnimBlendAssociation> AddAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID);
    std::unique_ptr<CAnimBlendAssociation> AddAnimation(RpClump* pClump, CAnimBlendHierarchy*, int ID);
    std::unique_ptr<CAnimBlendAssociation> AddAnimationAndSync(RpClump* pClump, CAnimBlendAssociation* pAssociation, AssocGroupId animGroup,
                                                               AnimationId animID);
    std::unique_ptr<CAnimBlendAssociation> BlendAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta);
    std::unique_ptr<CAnimBlendAssociation> BlendAnimation(RpClump* pClump, CAnimBlendHierarchy* pHierarchy, int ID, float fBlendDelta);
    std::unique_ptr<CAnimBlendAssociation> BlendAnimation(RpClump* pClump, CAnimBlendHierarchySAInterface* pHierarchyInterface, int ID, float fBlendDelta);

    void AddAnimBlockRef(int ID);
    void RemoveAnimBlockRef(int ID);
    void RemoveAnimBlockRefWithoutDelete(int ID);
    int  GetNumRefsToAnimBlock(int ID);
    void RemoveAnimBlock(int ID);

    AnimAssocDefinition* AddAnimAssocDefinition(const char* szBlockName, const char* szAnimName, AssocGroupId animGroup, AnimationId animID,
                                                AnimDescriptor* pDescriptor);
    void                 ReadAnimAssociationDefinitions();
    void                 CreateAnimAssocGroups();

    void  UncompressAnimation(CAnimBlendHierarchy* pHierarchy);
    void  RemoveFromUncompressedCache(CAnimBlendHierarchy* pHierarchy);
    void  RemoveFromUncompressedCache(CAnimBlendHierarchySAInterface* pInterface);
    void  LoadAnimFile(const char* szFile);
    void  LoadAnimFile(RwStream* pStream, bool b1, const char* sz1);
    void  LoadAnimFiles();
    void  RemoveLastAnimFile();
    BYTE* AllocateKeyFramesMemory(uint32_t u32BytesToAllocate);
    void  FreeKeyFramesMemory(void* pKeyFrames);

    // Non members
    bool                                   HasAnimGroupLoaded(AssocGroupId groupID);
    std::unique_ptr<CAnimBlendAssociation> RpAnimBlendClumpGetFirstAssociation(RpClump* pClump);
    std::unique_ptr<CAnimBlendAssociation> RpAnimBlendClumpGetAssociation(RpClump* pClump, const char* szAnimName);
    std::unique_ptr<CAnimBlendAssociation> RpAnimBlendClumpGetAssociation(RpClump* pClump, AnimationId animID);
    std::unique_ptr<CAnimBlendAssociation> RpAnimBlendGetNextAssociation(std::unique_ptr<CAnimBlendAssociation>& pAssociation);
    int                                    RpAnimBlendClumpGetNumAssociations(RpClump* pClump);
    void                                   RpAnimBlendClumpUpdateAnimations(RpClump* pClump, float f1, bool b1);

    // MTA members
    std::unique_ptr<CAnimBlendAssociation> GetAnimBlendAssociation(CAnimBlendAssociationSAInterface* pInterface);
    std::unique_ptr<CAnimBlendAssocGroup>  GetAnimBlendAssocGroup(CAnimBlendAssocGroupSAInterface* pInterface);
    std::unique_ptr<CAnimBlock>            GetAnimBlock(CAnimBlockSAInterface* pInterface);
    std::unique_ptr<CAnimBlendHierarchy>   GetAnimBlendHierarchy(CAnimBlendHierarchySAInterface* pInterface);

    StaticAssocIntface_type GetAnimStaticAssociation(CAnimBlendStaticAssociationSAInterface* pInterface);

    // MTA members, but use this strictly for custom animations only
    std::unique_ptr<CAnimBlendHierarchy> GetCustomAnimBlendHierarchy(CAnimBlendHierarchySAInterface* pInterface);
    std::unique_ptr<CAnimBlendSequence>  GetCustomAnimBlendSequence(CAnimBlendSequenceSAInterface* pInterface);

    // Warning! These two functions will create a new interface ( dynamic memory memory allocation )
    std::unique_ptr<CAnimBlendHierarchy> GetCustomAnimBlendHierarchy();
    std::unique_ptr<CAnimBlendSequence>  GetCustomAnimBlendSequence();

    void DeleteCustomAnimHierarchyInterface(CAnimBlendHierarchySAInterface* pInterface);
    void DeleteCustomAnimSequenceInterface(CAnimBlendSequenceSAInterface* pInterface);

    bool        isGateWayAnimationHierarchy(CAnimBlendHierarchySAInterface* pInterface);
    const char* GetGateWayBlockName() const;
    const char* GetGateWayAnimationName() const;

    bool IsValidGroup(std::uint32_t uiAnimGroup) const;
    bool IsValidAnim(std::uint32_t uiAnimGroup, std::uint32_t uiAnimID) const;
};
