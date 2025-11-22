/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/sdk/game/CAnimManager.h
 *  PURPOSE:     Animation manager interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <memory>

// Get correct values
#define MAX_ANIM_GROUPS 200
#define MAX_ANIMATIONS 500
#define MAX_ANIM_BLOCKS 200

typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;

class CAnimBlendAssocGroup;
class CAnimBlendAssocGroupSAInterface;
class CAnimBlendAssociation;
class CAnimBlendAssociationSAInterface;
class CAnimBlendHierarchy;
class CAnimBlendHierarchySAInterface;
class CAnimBlendSequence;
class CAnimBlendSequenceSAInterface;
class CAnimBlendStaticAssociation;
class CAnimBlendStaticAssociationSAInterface;
class CAnimBlock;
class CAnimBlockSAInterface;
class CClientPed;
class SString;
enum class eAnimGroup;
enum class eAnimID;
struct AnimAssocDefinition;
struct AnimDescriptor;
struct RpClump;
struct RwStream;

class CAnimManager
{
    friend class CAnimBlendAssociation;

public:
    typedef std::unique_ptr<CAnimBlendAssociation>       AnimBlendAssoc_type;
    typedef std::unique_ptr<CAnimBlendStaticAssociation> StaticAssocIntface_type;
    typedef std::unique_ptr<CAnimBlendAssocGroup>        AnimAssocGroup_type;
    virtual void                                         Initialize() = 0;
    virtual void                                         Shutdown() = 0;

    virtual int GetNumAnimations() = 0;
    virtual int GetNumAnimBlocks() = 0;
    virtual int GetNumAnimAssocDefinitions() = 0;

    virtual std::unique_ptr<CAnimBlendHierarchy> GetAnimation(int ID) = 0;
    virtual std::unique_ptr<CAnimBlendHierarchy> GetAnimation(const char* szName, std::unique_ptr<CAnimBlock>& pBlock) = 0;
    virtual std::unique_ptr<CAnimBlendHierarchy> GetAnimation(unsigned int uiIndex, std::unique_ptr<CAnimBlock>& pBlock) = 0;

    virtual std::unique_ptr<CAnimBlock> GetAnimationBlock(int ID) = 0;
    virtual std::unique_ptr<CAnimBlock> GetAnimationBlock(const char* szName) = 0;
    virtual int                         GetAnimationBlockIndex(const char* szName) = 0;
    virtual int                         RegisterAnimBlock(const char* szName) = 0;

    virtual AnimAssocGroup_type GetAnimBlendAssoc(AssocGroupId groupID) const = 0;
    virtual AssocGroupId        GetFirstAssocGroup(const char* szName) = 0;

    virtual const char* GetAnimGroupName(AssocGroupId groupID) = 0;
    virtual const char* GetAnimBlockName(AssocGroupId groupID) = 0;

    virtual AnimBlendAssoc_type     CreateAnimAssociation(AssocGroupId animGroup, AnimationId animID) = 0;
    virtual StaticAssocIntface_type GetAnimStaticAssociation(eAnimGroup animGroup, eAnimID animID) const = 0;
    virtual AnimBlendAssoc_type     GetAnimAssociation(AssocGroupId animGroup, const char* szAnimName) = 0;
    virtual AnimBlendAssoc_type     AddAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID) = 0;
    virtual AnimBlendAssoc_type     AddAnimation(RpClump* pClump, CAnimBlendHierarchy*, int ID) = 0;
    virtual AnimBlendAssoc_type     AddAnimationAndSync(RpClump* pClump, CAnimBlendAssociation* pAssociation, AssocGroupId animGroup, AnimationId animID) = 0;
    virtual AnimBlendAssoc_type     BlendAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta) = 0;
    virtual AnimBlendAssoc_type     BlendAnimation(RpClump* pClump, CAnimBlendHierarchy* pHierarchy, int ID, float fBlendDelta) = 0;
    virtual AnimBlendAssoc_type     BlendAnimation(RpClump* pClump, CAnimBlendHierarchySAInterface* pHierarchyInterface, int ID, float fBlendDelta) = 0;

    virtual void AddAnimBlockRef(int ID) = 0;
    virtual void RemoveAnimBlockRef(int ID) = 0;
    virtual void RemoveAnimBlockRefWithoutDelete(int ID) = 0;
    virtual int  GetNumRefsToAnimBlock(int ID) = 0;
    virtual void RemoveAnimBlock(int ID) = 0;

    virtual AnimAssocDefinition* AddAnimAssocDefinition(const char* szBlockName, const char* szAnimName, AssocGroupId animGroup, AnimationId animID,
                                                        AnimDescriptor* pDescriptor) = 0;
    virtual void                 ReadAnimAssociationDefinitions() = 0;
    virtual void                 CreateAnimAssocGroups() = 0;

    virtual void UncompressAnimation(CAnimBlendHierarchy* pHierarchy) = 0;
    virtual void RemoveFromUncompressedCache(CAnimBlendHierarchy* pHierarchy) = 0;
    virtual void RemoveFromUncompressedCache(CAnimBlendHierarchySAInterface* pInterface) = 0;

    virtual void  LoadAnimFile(const char* szFile) = 0;
    virtual void  LoadAnimFile(RwStream* pStream, bool b1, const char* sz1) = 0;
    virtual void  LoadAnimFiles() = 0;
    virtual void  RemoveLastAnimFile() = 0;
    virtual BYTE* AllocateKeyFramesMemory(uint32_t u32BytesToAllocate) = 0;
    virtual void  FreeKeyFramesMemory(void* pKeyFrames) = 0;

    // Non members
    virtual bool                HasAnimGroupLoaded(AssocGroupId groupID) = 0;
    virtual AnimBlendAssoc_type RpAnimBlendClumpGetFirstAssociation(RpClump* pClump) = 0;
    virtual AnimBlendAssoc_type RpAnimBlendClumpGetAssociation(RpClump* pClump, const char* szAnimName) = 0;
    virtual AnimBlendAssoc_type RpAnimBlendClumpGetAssociation(RpClump* pClump, AnimationId animID) = 0;
    virtual AnimBlendAssoc_type RpAnimBlendGetNextAssociation(std::unique_ptr<CAnimBlendAssociation>& pAssociation) = 0;
    virtual int                 RpAnimBlendClumpGetNumAssociations(RpClump* pClump) = 0;
    virtual void                RpAnimBlendClumpUpdateAnimations(RpClump* pClump, float f1, bool b1) = 0;

    // MTA members
    virtual AnimBlendAssoc_type                  GetAnimBlendAssociation(CAnimBlendAssociationSAInterface* pInterface) = 0;
    virtual AnimAssocGroup_type                  GetAnimBlendAssocGroup(CAnimBlendAssocGroupSAInterface* pInterface) = 0;
    virtual std::unique_ptr<CAnimBlock>          GetAnimBlock(CAnimBlockSAInterface* pInterface) = 0;
    virtual std::unique_ptr<CAnimBlendHierarchy> GetAnimBlendHierarchy(CAnimBlendHierarchySAInterface* pInterface) = 0;

    virtual StaticAssocIntface_type GetAnimStaticAssociation(CAnimBlendStaticAssociationSAInterface* pInterface) = 0;

    // MTA members, but use this strictly for custom animations only
    virtual std::unique_ptr<CAnimBlendHierarchy> GetCustomAnimBlendHierarchy(CAnimBlendHierarchySAInterface* pInterface) = 0;
    virtual std::unique_ptr<CAnimBlendSequence>  GetCustomAnimBlendSequence(CAnimBlendSequenceSAInterface* pInterface) = 0;
    virtual std::unique_ptr<CAnimBlendHierarchy> GetCustomAnimBlendHierarchy() = 0;
    virtual std::unique_ptr<CAnimBlendSequence>  GetCustomAnimBlendSequence() = 0;
    virtual void                                 DeleteCustomAnimHierarchyInterface(CAnimBlendHierarchySAInterface* pInterface) = 0;
    virtual void                                 DeleteCustomAnimSequenceInterface(CAnimBlendSequenceSAInterface* pInterface) = 0;

    virtual bool              isGateWayAnimationHierarchy(CAnimBlendHierarchySAInterface* pInterface) = 0;
    virtual const char* GetGateWayBlockName() const = 0;
    virtual const char* GetGateWayAnimationName() const = 0;

    virtual bool IsValidGroup(std::uint32_t uiAnimGroup) const = 0;
    virtual bool IsValidAnim(std::uint32_t uiAnimGroup, std::uint32_t uiAnimID) const = 0;
};
