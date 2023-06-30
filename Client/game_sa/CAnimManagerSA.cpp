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
    DWORD dwFunc = 0x5BF6B0;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::Shutdown()
{
    DWORD dwFunc = 0x4D4130;
    _asm
    {
        call    dwFunc
    }
}

int CAnimManagerSA::GetNumAnimations()
{
    return *(int*)(0xb4ea2c);
}

int CAnimManagerSA::GetNumAnimBlocks()
{
    return *(int*)(0xb4ea30);
}

int CAnimManagerSA::GetNumAnimAssocDefinitions()
{
    return *(int*)(0xb4ea28);
}

std::unique_ptr<CAnimBlendHierarchy> CAnimManagerSA::GetAnimation(int ID)
{
    CAnimBlendHierarchySAInterface* pInterface = nullptr;
    DWORD                           dwFunc = 0x4cde50;
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
    DWORD                           dwFunc = 0x4d42f0;
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
    DWORD                  dwFunc = 0x4d39f0;
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
    DWORD                  dwFunc = 0x406f90;
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
    DWORD                  dwFunc = 0x4d3940;
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
    DWORD dwFunc = 0x4d3990;
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
    DWORD dwFunc = 0x4d3e50;
    _asm
    {
        push    szName
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}

std::unique_ptr<CAnimBlendAssocGroup> CAnimManagerSA::GetAnimBlendAssoc(AssocGroupId groupID)
{
    CAnimBlendAssocGroupSAInterface* pInterface = nullptr;
    DWORD                            dwFunc = 0x45b0e0;
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
    DWORD        dwFunc = 0x4d39b0;
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
    DWORD       dwFunc = 0x4d3a20;
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
    DWORD       dwFunc = 0x4d3a30;
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
    DWORD                             dwFunc = 0x4d3a40;
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

CAnimManagerSA::StaticAssocIntface_type CAnimManagerSA::GetAnimStaticAssociation(eAnimGroup animGroup, eAnimID animID)
{
    CAnimBlendStaticAssociationSAInterface* pInterface = nullptr;
    DWORD                                   dwFunc = 0x4d3a60;
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
    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = 0x4d3a80;
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
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = 0x4d3aa0;
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
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = 0x4d4330;
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
    DWORD                             dwFunc = 0x4d3b30;
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
    if (!pClump)
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = 0x4d4610;
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
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = 0x4d4410;
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
    DWORD dwFunc = 0x4d3fb0;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::RemoveAnimBlockRef(int ID)
{
    DWORD dwFunc = 0x4d3fd0;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::RemoveAnimBlockRefWithoutDelete(int ID)
{
    DWORD dwFunc = 0x4d3ff0;
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
    DWORD dwFunc = 0x4d4010;
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
    DWORD dwFunc = 0x4d3f40;
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
    AnimAssocDefinition* pReturn;
    DWORD                dwFunc = 0x4d3ba0;
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
    return nullptr;
}

void CAnimManagerSA::ReadAnimAssociationDefinitions()
{
    DWORD dwFunc = 0x5bc910;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::CreateAnimAssocGroups()
{
    DWORD dwFunc = 0x4d3cc0;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::UncompressAnimation(CAnimBlendHierarchy* pHierarchy)
{
    DWORD                           dwFunc = 0x4d41c0;
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
    DWORD                           dwFunc = 0x4d42a0;
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
    DWORD dwFunc = 0x4d42a0;
    _asm
    {
        push    pHierarchyInterface
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::LoadAnimFile(const char* szFile)
{
    DWORD dwFunc = 0x4d55d0;
    _asm
    {
        push    szFile
        call    dwFunc
        add     esp, 0x4
    }
}

void CAnimManagerSA::LoadAnimFile(RwStream* pStream, bool b1, const char* sz1)
{
    DWORD dwFunc = 0x4d47f0;
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
    DWORD dwFunc = 0x4d5620;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::RemoveLastAnimFile()
{
    DWORD dwFunc = 0x4d3ed0;
    _asm
    {
        call    dwFunc
    }
}

BYTE* CAnimManagerSA::AllocateKeyFramesMemory(uint32_t u32BytesToAllocate)
{
    BYTE* pKeyFrames = nullptr;
    DWORD dwFunc = 0x72F420;
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
    DWORD dwFunc = 0x72F430;
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
    DWORD dwFunc = 0x45b130;
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
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = 0x4d15e0;
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
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = 0x4d6870;
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
        return nullptr;

    CAnimBlendAssociationSAInterface* pInterface = nullptr;
    DWORD                             dwFunc = 0x4d68b0;
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
    DWORD                             dwFunc = 0x4d6ab0;
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
    DWORD dwFunc = 0x4d6b60;
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

    DWORD dwFunc = 0x4d34f0;
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
    return pGame->GetKeyGen()->GetUppercaseKey(m_kGateWayAnimationName.c_str()) == pInterface->uiHashKey;
}
