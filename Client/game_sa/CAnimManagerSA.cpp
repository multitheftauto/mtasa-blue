/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CAnimManagerSA.cpp
 *  PURPOSE:     Animation manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAnimBlendAssocGroupSA.h"
#include "CAnimBlendAssociationSA.h"
#include "CAnimBlendHierarchySA.h"
#include "CAnimBlendSequenceSA.h"
#include "CAnimBlockSA.h"
#include "CAnimManagerSA.h"
#include "CGameSA.h"
#include "CKeyGenSA.h"

extern CGameSA* pGame;

// This "gateway" animation will allow us to play custom animations by simply playing this animation
// and then in AddAnimation and AddAnimationAndSync hook, we can return our custom animation in the
// hook instead of run_wuzi. This will trick GTA SA into thinking that it is playing run_wuzi from
// ped block, but in reality, it's playing our custom animation, and Of course, we can return run_wuzi
// animation within the hook if we want to play it instead. Why run_wuzi? We can also use another animation,
// but I've tested with this one mostly, so let's stick to this.
static const char* const kGateWayBlockName = "ped";
static const char* const kGateWayAnimationName = "run_wuzi";

CAnimManagerSA::CAnimManagerSA()
{
}

CAnimManagerSA::~CAnimManagerSA()
{
}

void CAnimManagerSA::Initialize()
{
    DWORD dwFunc = FUNC_CAnimManager_Initialize;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::Shutdown()
{
    DWORD dwFunc = FUNC_CAnimManager_Shutdown;
    _asm
    {
        call    dwFunc
    }
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
    DWORD                           dwFunc = FUNC_CAnimManager_GetAnimation_int;
    _asm
    {
        push    ID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendHierarchySA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetAnimation(const char* szName, std::unique_ptr<CAnimBlock>& pBlock)
{
    CAnimBlendHierarchySAInterface* pInterface = nullptr;
    DWORD                           dwFunc = FUNC_CAnimManager_GetAnimation_str_block;
    CAnimBlockSAInterface*          pBlockInterface = pBlock->GetInterface();
    _asm
    {
        push    pBlockInterface
        push    szName
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendHierarchySA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetAnimation(unsigned int uiIndex, std::unique_ptr<CAnimBlock>& pBlock)
{
    CAnimBlendHierarchySAInterface* pInterface = nullptr;
    ;
    DWORD                  dwFunc = FUNC_CAnimManager_GetAnimation_int_block;
    CAnimBlockSAInterface* pBlockInterface = pBlock->GetInterface();
    _asm
    {
        push    pBlockInterface
        push    uiIndex
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendHierarchySA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlock> CAnimManagerSA::GetAnimationBlock(int ID)
{
    CAnimBlockSAInterface* pInterface = nullptr;
    DWORD                  dwFunc = FUNC_CAnimManager_GetAnimationBlock_int;
    _asm
    {
        push    ID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlockSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlock> CAnimManagerSA::GetAnimationBlock(const char* szName)
{
    CAnimBlockSAInterface* pInterface = nullptr;
    DWORD                  dwFunc = FUNC_CAnimManager_GetAnimationBlock_str;
    _asm
    {
        push    szName
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlockSA>(pInterface);
    }
    return nullptr;
}

int CAnimManagerSA::GetAnimationBlockIndex(const char* szName)
{
    int   iReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimationBlockIndex;
    _asm
    {
        push    szName
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}

int CAnimManagerSA::RegisterAnimBlock(const char* szName)
{
    int   iReturn;
    DWORD dwFunc = FUNC_CAnimManager_RegisterAnimBlock;
    _asm
    {
        push    szName
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}

std::unique_ptr<CAnimBlendAssocGroup> CAnimManagerSA::GetAnimBlendAssoc(AssocGroupId groupID) const
{
    CAnimBlendAssocGroupSAInterface* pInterface = nullptr;
    DWORD                            dwFunc = FUNC_CAnimManager_GetAnimBlendAssoc;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssocGroupSA>(pInterface);
    }
    return nullptr;
}

AssocGroupId CAnimManagerSA::GetFirstAssocGroup(const char* szName)
{
    AssocGroupId groupReturn;
    DWORD        dwFunc = FUNC_CAnimManager_GetFirstAssocGroup;
    _asm
    {
        push    szName
        call    dwFunc
        mov     groupReturn, eax
        add     esp, 0x4
    }
    return groupReturn;
}

const char* CAnimManagerSA::GetAnimGroupName(AssocGroupId groupID)
{
    const char* szReturn;
    DWORD       dwFunc = FUNC_CAnimManager_GetAnimGroupName;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     szReturn, eax
        add     esp, 0x4
    }
    return szReturn;
}

const char* CAnimManagerSA::GetAnimBlockName(AssocGroupId groupID)
{
    const char* szReturn;
    DWORD       dwFunc = FUNC_CAnimManager_GetAnimBlockName;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     szReturn, eax
        add     esp, 0x4
    }
    return szReturn;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::CreateAnimAssociation(AssocGroupId animGroup, AnimationId animID)
{
    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = FUNC_CAnimManager_CreateAnimAssociation;
    _asm
    {
        push    animID
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

CAnimManagerSA::StaticAssocIntface_type CAnimManagerSA::GetAnimStaticAssociation(eAnimGroup animGroup, eAnimID animID) const
{
    // We check the validity of the group, avoid crashes due to an invalid group
    if (!IsValidGroup(static_cast<std::uint32_t>(animGroup)))
        return nullptr;

    CAnimBlendStaticAssociationSAInterface* pInterface = nullptr;
    DWORD                                   dwFunc = FUNC_CAnimManager_GetAnimAssociation;
    _asm
    {
        push    animID
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendStaticAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::GetAnimAssociation(AssocGroupId animGroup, const char* szAnimName)
{
    // We check the validity of the group, avoid crashes due to an invalid group
    if (!IsValidGroup(animGroup))
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = FUNC_CAnimManager_GetAnimAssociation_str;
    _asm
    {
        push    szAnimName
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
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
    DWORD                             dwFunc = FUNC_CAnimManager_AddAnimation;
    _asm
    {
        push    animID
        push    animGroup
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0xC
    }
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
    DWORD                             dwFunc = FUNC_CAnimManager_AddAnimation_hier;
    CAnimBlendHierarchySAInterface*   pHierarchyInterface = pHierarchy->GetInterface();
    _asm
    {
        push    ID
        push    pHierarchyInterface
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0xC
    }
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
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = FUNC_CAnimManager_AddAnimationAndSync;
    CAnimBlendAssociationSAInterface* pAssociationInterface = pAssociation->GetInterface();
    _asm
    {
        push    animID
        push    animGroup
        push    pAssociationInterface
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::BlendAnimation(RpClump* pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta)
{
    if (!pClump || !IsValidAnim(animGroup, animID))
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = FUNC_CAnimManager_BlendAnimation;
    _asm
    {
        push    fBlendDelta
        push    animID
        push    animGroup
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
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
    DWORD                             dwFunc = FUNC_CAnimManager_BlendAnimation_hier;
    CAnimBlendHierarchySAInterface*   pHierarchyInterface = pHierarchy->GetInterface();
    _asm
    {
        push    fBlendDelta
        push    ID
        push    pHierarchyInterface
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

void CAnimManagerSA::AddAnimBlockRef(int ID)
{
    DWORD dwFunc = FUNC_CAnimManager_AddAnimBlockRef;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::RemoveAnimBlockRef(int ID)
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlockRef;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::RemoveAnimBlockRefWithoutDelete(int ID)
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlockRefWithoutDelete;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}

int CAnimManagerSA::GetNumRefsToAnimBlock(int ID)
{
    int   iReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetNumRefsToAnimBlock;
    _asm
    {
        push    ID
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}

void CAnimManagerSA::RemoveAnimBlock(int ID)
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlock;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}

AnimAssocDefinition* CAnimManagerSA::AddAnimAssocDefinition(const char* szBlockName, const char* szAnimName, AssocGroupId animGroup, AnimationId animID,
                                                            AnimDescriptor* pDescriptor)
{
    if (!IsValidAnim(animGroup, animID))
        return nullptr;

    AnimAssocDefinition* pReturn{};
    DWORD                dwFunc = FUNC_CAnimManager_AddAnimAssocDefinition;
    _asm
    {
        push    pDescriptor
        push    animID
        push    animGroup
        push    szAnimName
        push    szBlockName
        call    dwFunc
        mov     pReturn, eax
        add     esp, 0x14
    }
    return pReturn;
}

void CAnimManagerSA::ReadAnimAssociationDefinitions()
{
    DWORD dwFunc = FUNC_CAnimManager_ReadAnimAssociationDefinitions;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::CreateAnimAssocGroups()
{
    DWORD dwFunc = FUNC_CAnimManager_CreateAnimAssocGroups;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::UncompressAnimation(CAnimBlendHierarchy* pHierarchy)
{
    DWORD                           dwFunc = FUNC_CAnimManager_UncompressAnimation;
    CAnimBlendHierarchySAInterface* pHierarchyInterface = pHierarchy->GetInterface();
    _asm
    {
        push    pHierarchyInterface
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::RemoveFromUncompressedCache(CAnimBlendHierarchy* pHierarchy)
{
    DWORD                           dwFunc = FUNC_CAnimManager_RemoveFromUncompressedCache;
    CAnimBlendHierarchySAInterface* pHierarchyInterface = pHierarchy->GetInterface();
    _asm
    {
        push    pHierarchyInterface
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::RemoveFromUncompressedCache(CAnimBlendHierarchySAInterface* pHierarchyInterface)
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveFromUncompressedCache;
    _asm
    {
        push    pHierarchyInterface
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::LoadAnimFile(const char* szFile)
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFile;
    _asm
    {
        push    szFile
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::LoadAnimFile(RwStream* pStream, bool b1, const char* sz1)
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFile_stream;
    _asm
    {
        push    sz1
        push    b1
        push    pStream
        call    dwFunc
        add     esp, 0xC
    }
}

void CAnimManagerSA::LoadAnimFiles()
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFiles;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::RemoveLastAnimFile()
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveLastAnimFile;
    _asm
    {
        call    dwFunc
    }
}

BYTE* CAnimManagerSA::AllocateKeyFramesMemory(uint32_t u32BytesToAllocate)
{
    BYTE* pKeyFrames = nullptr;
    DWORD dwFunc = FUNC_CAnimManager_AllocateKeyFramesMemory;
    _asm
    {
        push    u32BytesToAllocate
        call    dwFunc
        add     esp, 0x4
        mov     pKeyFrames, eax
    }
    return pKeyFrames;
}

void CAnimManagerSA::FreeKeyFramesMemory(void* pKeyFrames)
{
    DWORD dwFunc = FUNC_CAnimManager_FreeKeyFramesMemory;
    _asm
    {
        push    pKeyFrames
        call    dwFunc
        add     esp, 0x4
    }
}

bool CAnimManagerSA::HasAnimGroupLoaded(AssocGroupId groupID)
{
    bool  bReturn;
    DWORD dwFunc = FUNC_HasAnimGroupLoaded;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x4
    }
    return bReturn;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::RpAnimBlendClumpGetFirstAssociation(RpClump* pClump)
{
    if (!pClump)
        return NULL;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = FUNC_RpAnimBlendClumpGetFirstAssociation;
    _asm
    {
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
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
    DWORD                             dwFunc = FUNC_RpAnimBlendClumpGetAssociation_str;
    _asm
    {
        push    szAnimName
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
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
    DWORD                             dwFunc = FUNC_RpAnimBlendClumpGetAssociation_int;
    _asm
    {
        push    animID
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    if (pInterface)
    {
        return std::make_unique<CAnimBlendAssociationSA>(pInterface);
    }
    return nullptr;
}

std::unique_ptr<CAnimBlendAssociation> CAnimManagerSA::RpAnimBlendGetNextAssociation(std::unique_ptr<CAnimBlendAssociation>& pAssociation)
{
    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = FUNC_RpAnimBlendGetNextAssociation;
    CAnimBlendAssociationSAInterface* pAssociationInterface = pAssociation->GetInterface();
    _asm
    {
        push    pAssociationInterface
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
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

    int   iReturn;
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetNumAssociations;
    _asm
    {
        push    pClump
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}

void CAnimManagerSA::RpAnimBlendClumpUpdateAnimations(RpClump* pClump, float f1, bool b1)
{
    if (!pClump)
        return;

    DWORD dwFunc = FUNC_RpAnimBlendClumpUpdateAnimations;
    _asm
    {
        push    b1
        push    f1
        push    pClump
        call    dwFunc
        add     esp, 0xC
    }
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
    return pGame->GetKeyGen()->GetUppercaseKey(kGateWayAnimationName) == pInterface->uiHashKey;
}

const char* CAnimManagerSA::GetGateWayBlockName() const
{
    return kGateWayBlockName;
}

const char* CAnimManagerSA::GetGateWayAnimationName() const
{
    return kGateWayAnimationName;
}

bool CAnimManagerSA::IsValidGroup(std::uint32_t uiAnimGroup) const
{
    const auto pGroup = GetAnimBlendAssoc(uiAnimGroup);
    return pGroup && pGroup->IsCreated();
}

bool CAnimManagerSA::IsValidAnim(std::uint32_t uiAnimGroup, std::uint32_t uiAnimID) const
{
    // We get an animation for the checks
    const auto pAnim = GetAnimStaticAssociation((eAnimGroup)uiAnimGroup, (eAnimID)uiAnimID);
    if (!pAnim)
        return false;

    // We check the interface and sAnimID, if AnimID is not in GTA:SA, it will differ from our indicators in sAnimID
    const auto pInterface = pAnim->GetInterface();
    return pInterface && pInterface->sAnimID == uiAnimID;
}
