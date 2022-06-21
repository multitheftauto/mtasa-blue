/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimManagerSA.cpp
 *  PURPOSE:     Animation manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CAnimManagerSA::CAnimManagerSA()
{
    MemSetFast(m_pAnimAssocGroups, 0, sizeof(m_pAnimAssocGroups));
    MemSetFast(m_pAnimBlocks, 0, sizeof(m_pAnimBlocks));
}

CAnimManagerSA::~CAnimManagerSA()
{
    for (unsigned int i = 0; i < MAX_ANIM_GROUPS; i++)
        if (m_pAnimAssocGroups[i])
            delete m_pAnimAssocGroups[i];
    for (unsigned int i = 0; i < MAX_ANIM_BLOCKS; i++)
        if (m_pAnimBlocks[i])
            delete m_pAnimBlocks[i];
}

void CAnimManagerSA::Initialize()
{
    // CAnimManager::Initialize
    ((void(__cdecl*)())FUNC_CAnimManager_Initialize)();
}

void CAnimManagerSA::Shutdown()
{
    // CAnimManager::Shutdown
    ((void(__cdecl*)())FUNC_CAnimManager_Shutdown)();
}

int CAnimManagerSA::GetNumAnimations()
{
    return *(int*)(VAR_CAnimManager_NumAnimations);
}

int CAnimManagerSA::GetNumAnimBlocks()
{
    return *(int*)(VAR_CAnimManager_NumAnimBlocks);
}

int CAnimManagerSA::GetNumAnimAssocDefinitions()
{
    return *(int*)(VAR_CAnimManager_NumAnimAssocDefinitions);
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetAnimation(int ID)
{
    CAnimBlendHierarchySAInterface* pInterface = nullptr;

    // CAnimManager::GetAnimation
    pInterface = ((CAnimBlendHierarchySAInterface * (__cdecl*)(int)) FUNC_CAnimManager_GetAnimation_int)(ID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendHierarchySA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetAnimation(const char* szName, std::unique_ptr<CAnimBlock>& pBlock)
{
    CAnimBlendHierarchySAInterface* pInterface = nullptr;

    // CAnimManager::GetAnimation
    pInterface = ((CAnimBlendHierarchySAInterface * (__cdecl*)(const char*, CAnimBlockSAInterface*)) FUNC_CAnimManager_GetAnimation_str_block)(
        szName, pBlock->GetInterface());

    if (pInterface)
    {
        return std::make_unique<CAnimBlendHierarchySA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetAnimation(unsigned int uiIndex, std::unique_ptr<CAnimBlock>& pBlock)
{
    CAnimBlendHierarchySAInterface* pInterface = nullptr;

    // CAnimManager::GetAnimation
    pInterface = ((CAnimBlendHierarchySAInterface * (__cdecl*)(unsigned int, CAnimBlockSAInterface*)) FUNC_CAnimManager_GetAnimation_int_block)(
        uiIndex, pBlock->GetInterface());

    if (pInterface)
    {
        return std::make_unique<CAnimBlendHierarchySA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlock> CAnimManagerSA::GetAnimationBlock(int ID)
{
    CAnimBlockSAInterface* pInterface = nullptr;

    // CAnimManager::GetAnimationBlock
    pInterface = ((CAnimBlockSAInterface * (__cdecl*)(int)) FUNC_CAnimManager_GetAnimationBlock_int)(ID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlockSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlock> CAnimManagerSA::GetAnimationBlock(const char* szName)
{
    CAnimBlockSAInterface* pInterface = nullptr;

    // CAnimManager::GetAnimationBlock
    pInterface = ((CAnimBlockSAInterface * (__cdecl*)(const char*)) FUNC_CAnimManager_GetAnimationBlock_str)(szName);

    if (pInterface)
    {
        return std::make_unique<CAnimBlockSA>(pInterface);
    }
    return nullptr;
}

int CAnimManagerSA::GetAnimationBlockIndex(const char* szName)
{
    // CAnimManager::GetAnimationBlockIndex
    return ((int(__cdecl*)(const char*))FUNC_CAnimManager_GetAnimationBlockIndex)(szName);
}

int CAnimManagerSA::RegisterAnimBlock(const char* szName)
{
    // CAnimManager::RegisterAnimBlock
    return ((int(__cdecl*)(const char*))FUNC_CAnimManager_RegisterAnimBlock)(szName);
}

std::unique_ptr<CAnimBlendAssocGroup> CAnimManagerSA::GetAnimBlendAssoc(AssocGroupId groupID)
{
    CAnimBlendAssocGroupSAInterface* pInterface = nullptr;

    // CAnimManager::GetAnimBlendAssoc
    pInterface = ((CAnimBlendAssocGroupSAInterface * (__cdecl*)(AssocGroupId)) FUNC_CAnimManager_GetAnimBlendAssoc)(groupID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssocGroupSA>(pInterface);
    }
    return nullptr;
}

AssocGroupId CAnimManagerSA::GetFirstAssocGroup(const char* szName)
{
    // CAnimManager::GetFirstAssocGroup
    return ((AssocGroupId(__cdecl*)(const char*))FUNC_CAnimManager_GetFirstAssocGroup)(szName);
}

const char* CAnimManagerSA::GetAnimGroupName(AssocGroupId groupID)
{
    // CAnimManager::GetAnimGroupName
    return ((const char*(__cdecl*)(AssocGroupId))FUNC_CAnimManager_GetAnimGroupName)(groupID);
}

const char* CAnimManagerSA::GetAnimBlockName(AssocGroupId groupID)
{
    // CAnimManager::GetAnimBlockName
    return ((const char*(__cdecl*)(AssocGroupId))FUNC_CAnimManager_GetAnimBlockName)(groupID);
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::CreateAnimAssociation(AssocGroupId animGroup, AnimationId animID)
{
    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // CAnimManager::CreateAnimAssociation
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(AssocGroupId, AnimationId)) FUNC_CAnimManager_CreateAnimAssociation)(animGroup, animID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

CAnimManagerSA::StaticAssocIntface_type CAnimManagerSA::GetAnimStaticAssociation(eAnimGroup animGroup, eAnimID animID)
{
    CAnimBlendStaticAssociationSAInterface* pInterface = nullptr;

    // CAnimManager::GetAnimAssociation
    pInterface = ((CAnimBlendStaticAssociationSAInterface * (__cdecl*)(eAnimGroup, eAnimID)) FUNC_CAnimManager_GetAnimAssociation)(animGroup, animID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendStaticAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::GetAnimAssociation(AssocGroupId animGroup, const char* szAnimName)
{
    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // CAnimManager::GetAnimAssociation
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(AssocGroupId, const char*)) FUNC_CAnimManager_GetAnimAssociation_str)(animGroup, szAnimName);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::AddAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // CAnimManager::AddAnimation
    pInterface =
        ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*, AssocGroupId, AnimationId)) FUNC_CAnimManager_AddAnimation)(pClump, animGroup, animID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::AddAnimation(RpClump* pClump, CAnimBlendHierarchy* pHierarchy, int ID)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // CAnimManager::AddAnimation
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*, CAnimBlendHierarchySAInterface*, int)) FUNC_CAnimManager_AddAnimation_hier)(
        pClump, pHierarchy->GetInterface(), ID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::AddAnimationAndSync(RpClump* pClump, CAnimBlendAssociation* pAssociation, AssocGroupId animGroup,
                                                                           AnimationId animID)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // CAnimManager::AddAnimationAndSync
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*, CAnimBlendAssociationSAInterface*, AssocGroupId, AnimationId))
                      FUNC_CAnimManager_AddAnimationAndSync)(pClump, pAssociation->GetInterface(), animGroup, animID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::BlendAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // CAnimManager::BlendAnimation
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*, AssocGroupId, AnimationId, float)) FUNC_CAnimManager_BlendAnimation)(
        pClump, animGroup, animID, fBlendDelta);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::BlendAnimation(RpClump* pClump, CAnimBlendHierarchy* pHierarchy, int ID, float fBlendDelta)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // CAnimManager::BlendAnimation
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*, CAnimBlendHierarchySAInterface*, int, float)) FUNC_CAnimManager_BlendAnimation_hier)(
        pClump, pHierarchy->GetInterface(), ID, fBlendDelta);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

void CAnimManagerSA::AddAnimBlockRef(int ID)
{
    // CAnimManager::AddAnimBlockRef
    ((void(__cdecl*)(int))FUNC_CAnimManager_AddAnimBlockRef)(ID);
}

void CAnimManagerSA::RemoveAnimBlockRef(int ID)
{
    // CAnimManager::RemoveAnimBlockRef
    ((void(__cdecl*)(int))FUNC_CAnimManager_RemoveAnimBlockRef)(ID);
}

void CAnimManagerSA::RemoveAnimBlockRefWithoutDelete(int ID)
{
    // CAnimManager::RemoveAnimBlockRefWithoutDelete
    ((void(__cdecl*)(int))FUNC_CAnimManager_RemoveAnimBlockRefWithoutDelete)(ID);
}

int CAnimManagerSA::GetNumRefsToAnimBlock(int ID)
{
    // CAnimManager::GetNumRefsToAnimBlock
    return ((int(__cdecl*)(int))FUNC_CAnimManager_GetNumRefsToAnimBlock)(ID);
}

void CAnimManagerSA::RemoveAnimBlock(int ID)
{
    // CAnimManager::RemoveAnimBlock
    ((void(__cdecl*)(int))FUNC_CAnimManager_RemoveAnimBlock)(ID);
}

AnimAssocDefinition* CAnimManagerSA::AddAnimAssocDefinition(const char* szBlockName, const char* szAnimName, AssocGroupId animGroup, AnimationId animID,
                                                            AnimDescriptor* pDescriptor)
{
    // CAnimManager::AddAnimAssocDefinition
    ((AnimAssocDefinition * (__cdecl*)(const char*, const char*, AssocGroupId, AnimationId, AnimDescriptor*)) FUNC_CAnimManager_AddAnimAssocDefinition)(
        szBlockName, szAnimName, animGroup, animID, pDescriptor);
    return nullptr;
}

void CAnimManagerSA::ReadAnimAssociationDefinitions()
{
    // CAnimManager::ReadAnimAssociationDefinitions
    ((void(__cdecl*)())FUNC_CAnimManager_ReadAnimAssociationDefinitions)();
}

void CAnimManagerSA::CreateAnimAssocGroups()
{
    // CAnimManager::CreateAnimAssocGroups
    ((void(__cdecl*)())FUNC_CAnimManager_CreateAnimAssocGroups)();
}

void CAnimManagerSA::UncompressAnimation(CAnimBlendHierarchy* pHierarchy)
{
    // CAnimManager::UncompressAnimation
    ((void(__cdecl*)(CAnimBlendHierarchySAInterface*))FUNC_CAnimManager_UncompressAnimation)(pHierarchy->GetInterface());
}

void CAnimManagerSA::RemoveFromUncompressedCache(CAnimBlendHierarchy* pHierarchy)
{
    CAnimManagerSA::RemoveFromUncompressedCache(pHierarchy->GetInterface());
}

void CAnimManagerSA::RemoveFromUncompressedCache(CAnimBlendHierarchySAInterface* pHierarchyInterface)
{
    // CAnimManager::RemoveFromUncompressedCache
    ((void(__cdecl*)(CAnimBlendHierarchySAInterface*))FUNC_CAnimManager_RemoveFromUncompressedCache)(pHierarchyInterface);
}

void CAnimManagerSA::LoadAnimFile(const char* szFile)
{
    // CAnimManager::LoadAnimFile
    ((void(__cdecl*)(const char*))FUNC_CAnimManager_LoadAnimFile)(szFile);
}

void CAnimManagerSA::LoadAnimFile(RwStream* pStream, bool b1, const char* sz1)
{
    // CAnimManager::LoadAnimFile
    ((void(__cdecl*)(RwStream*, bool, const char*))FUNC_CAnimManager_LoadAnimFile_stream)(pStream, b1, sz1);
}

void CAnimManagerSA::LoadAnimFiles()
{
    // CAnimManager::LoadAnimFiles
    ((void(__cdecl*)())FUNC_CAnimManager_LoadAnimFiles)();
}

void CAnimManagerSA::RemoveLastAnimFile()
{
    // CAnimManager::RemoveLastAnimFile
    ((void(__cdecl*)())FUNC_CAnimManager_RemoveLastAnimFile)();
}

BYTE* CAnimManagerSA::AllocateKeyFramesMemory(uint32_t u32BytesToAllocate)
{
    // CAnimManager::AllocateKeyFramesMemory
    return ((BYTE * (__cdecl*)(unsigned int)) FUNC_CAnimManager_AllocateKeyFramesMemory)(u32BytesToAllocate);
}

void CAnimManagerSA::FreeKeyFramesMemory(void* pKeyFrames)
{
    // CAnimManager::FreeKeyFramesMemory
    ((void(__cdecl*)(void*))FUNC_CAnimManager_FreeKeyFramesMemory)(pKeyFrames);
}

bool CAnimManagerSA::HasAnimGroupLoaded(AssocGroupId groupID)
{
    // CAnimManager::AllocateKeyFramesMemory
    return ((bool(__cdecl*)(AssocGroupId))FUNC_HasAnimGroupLoaded)(groupID);
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::RpAnimBlendClumpGetFirstAssociation(RpClump* pClump)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // RpAnimBlendClumpGetFirstAssociation
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*)) FUNC_RpAnimBlendClumpGetFirstAssociation)(pClump);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::RpAnimBlendClumpGetAssociation(RpClump* pClump, const char* szAnimName)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // RpAnimBlendClumpGetAssociation
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*, const char*)) FUNC_RpAnimBlendClumpGetAssociation_str)(pClump, szAnimName);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::RpAnimBlendClumpGetAssociation(RpClump* pClump, AnimationId animID)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // RpAnimBlendClumpGetAssociation
    pInterface = ((CAnimBlendAssociationSAInterface * (__cdecl*)(RpClump*, AnimationId)) FUNC_RpAnimBlendClumpGetAssociation_int)(pClump, animID);

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::RpAnimBlendGetNextAssociation(std::unique_ptr<CAnimBlendAssociation>& pAssociation)
{
    CAnimBlendAssociationSAInterface* pInterface = nullptr;

    // RpAnimBlendGetNextAssociation
    pInterface =
        ((CAnimBlendAssociationSAInterface * (__cdecl*)(CAnimBlendAssociationSAInterface*)) FUNC_RpAnimBlendGetNextAssociation)(pAssociation->GetInterface());

    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

int CAnimManagerSA::RpAnimBlendClumpGetNumAssociations(RpClump* pClump)
{
    if (!pClump)
        return 0;

    // RpAnimBlendClumpGetNumAssociations
    return ((int(__cdecl*)(RpClump*))FUNC_RpAnimBlendClumpGetNumAssociations)(pClump);
}

void CAnimManagerSA::RpAnimBlendClumpUpdateAnimations(RpClump* pClump, float f1, bool b1)
{
    if (!pClump)
        return;

    // RpAnimBlendClumpUpdateAnimations
    ((void(__cdecl*)(RpClump*, float, bool))FUNC_RpAnimBlendClumpUpdateAnimations)(pClump, f1, b1);
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::GetAnimBlendAssociation(CAnimBlendAssociationSAInterface* pInterface)
{
    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssocGroup> CAnimManagerSA::GetAnimBlendAssocGroup(CAnimBlendAssocGroupSAInterface* pInterface)
{
    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssocGroupSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlock> CAnimManagerSA::GetAnimBlock(CAnimBlockSAInterface* pInterface)
{
    if (pInterface)
    {
        return std::make_unique<CAnimBlockSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetAnimBlendHierarchy(CAnimBlendHierarchySAInterface* pInterface)
{
    if (pInterface)
    {
        return std::make_unique<CAnimBlendHierarchySA>(pInterface);
    }
    return nullptr;
}

CAnimManagerSA::StaticAssocIntface_type CAnimManagerSA::GetAnimStaticAssociation(CAnimBlendStaticAssociationSAInterface* pInterface)
{
    if (pInterface)
    {
        return std::make_unique<CAnimBlendStaticAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetCustomAnimBlendHierarchy(CAnimBlendHierarchySAInterface* pInterface)
{
    if (pInterface)
    {
        return std::make_unique<CAnimBlendHierarchySA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendSequence> CAnimManagerSA::GetCustomAnimBlendSequence(CAnimBlendSequenceSAInterface* pInterface)
{
    if (pInterface)
    {
        return std::make_unique<CAnimBlendSequenceSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetCustomAnimBlendHierarchy()
{
    auto pInterface = new CAnimBlendHierarchySAInterface;
    return std::make_unique<CAnimBlendHierarchySA>(pInterface);
}

std::unique_ptr<CAnimBlendSequence> CAnimManagerSA::GetCustomAnimBlendSequence()
{
    auto pInterface = new CAnimBlendSequenceSAInterface;
    return std::make_unique<CAnimBlendSequenceSA>(pInterface);
}

void CAnimManagerSA::DeleteCustomAnimHierarchyInterface(CAnimBlendHierarchySAInterface* pInterface)
{
    delete pInterface;
}

void CAnimManagerSA::DeleteCustomAnimSequenceInterface(CAnimBlendSequenceSAInterface* pInterface)
{
    delete pInterface;
}

bool CAnimManagerSA::isGateWayAnimationHierarchy(CAnimBlendHierarchySAInterface* pInterface)
{
    return pGame->GetKeyGen()->GetUppercaseKey(m_kGateWayAnimationName.c_str()) == pInterface->uiHashKey;
}
