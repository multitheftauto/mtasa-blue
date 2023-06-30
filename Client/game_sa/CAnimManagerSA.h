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

#define ARRAY_CAnimManager_Animations                       0xb4ea40

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

    std::unique_ptr<CAnimBlendAssocGroup> GetAnimBlendAssoc(AssocGroupId groupID);
    AssocGroupId                          GetFirstAssocGroup(const char* szName);

    const char* GetAnimGroupName(AssocGroupId groupID);
    const char* GetAnimBlockName(AssocGroupId groupID);

    std::unique_ptr<CAnimBlendAssociation> CreateAnimAssociation(AssocGroupId animGroup, AnimationId animID);
    StaticAssocIntface_type                GetAnimStaticAssociation(eAnimGroup animGroup, eAnimID animID);
    std::unique_ptr<CAnimBlendAssociation> GetAnimAssociation(AssocGroupId animGroup, const char* szAnimName);
    std::unique_ptr<CAnimBlendAssociation> AddAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID);
    std::unique_ptr<CAnimBlendAssociation> AddAnimation(RpClump* pClump, CAnimBlendHierarchy*, int ID);
    std::unique_ptr<CAnimBlendAssociation> AddAnimationAndSync(RpClump* pClump, CAnimBlendAssociation* pAssociation, AssocGroupId animGroup,
                                                               AnimationId animID);
    std::unique_ptr<CAnimBlendAssociation> BlendAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta);
    std::unique_ptr<CAnimBlendAssociation> BlendAnimation(RpClump* pClump, CAnimBlendHierarchy* pHierarchy, int ID, float fBlendDelta);

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

    bool           isGateWayAnimationHierarchy(CAnimBlendHierarchySAInterface* pInterface);
    const SString& GetGateWayBlockName() { return m_kGateWayBlockName; };
    const SString& GetGateWayAnimationName() { return m_kGateWayAnimationName; };

private:
    CAnimBlendAssocGroup* m_pAnimAssocGroups[MAX_ANIM_GROUPS];
    CAnimBlock*           m_pAnimBlocks[MAX_ANIM_BLOCKS];

    // This "gateway" animation will allow us to play custom animations by simply playing this animation
    // and then in AddAnimation and AddAnimationAndSync hook, we can return our custom animation in the
    // hook instead of run_wuzi. This will trick GTA SA into thinking that it is playing run_wuzi from
    // ped block, but in reality, it's playing our custom animation, and Of course, we can return run_wuzi
    // animation within the hook if we want to play it instead. Why run_wuzi? We can also use another animation,
    // but I've tested with this one mostly, so let's stick to this.
    const SString m_kGateWayBlockName = "ped";
    const SString m_kGateWayAnimationName = "run_wuzi";
};
