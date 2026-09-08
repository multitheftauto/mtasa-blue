/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendAssociationSA.cpp
 *  PURPOSE:     Animation blend association
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAnimBlendAssociationSA.h"
#include "CAnimBlendHierarchySA.h"
#include "CAnimBlendStaticAssociationSA.h"
#include "CAnimManagerSA.h"
#include "CGameSA.h"

extern CGameSA* pGame;

AnimBlendFrameData* CAnimBlendClumpDataSAInterface::GetFrameDataByNodeId(unsigned int nodeId)
{
    for (int i = 0; i < m_dwNumBones; i++)
    {
        AnimBlendFrameData& frameData = m_frames[i];
        if (frameData.m_nNodeId == nodeId)
            return &frameData;
    }
    return nullptr;
}

CAnimBlendAssociationSAInterface* CAnimBlendAssociationSA::Constructor(CAnimBlendStaticAssociationSAInterface& staticAssociationByReference)
{
    DWORD DwFunc = 0x4CF080;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    // clang-format off
    __asm
    {
        mov     ecx, DwThisInterface
        push    staticAssociationByReference
        call    DwFunc
    }
    // clang-format on;
}

CAnimBlendAssociationSAInterface* CAnimBlendAssociationSA::Constructor(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy)
{
    DWORD DwFunc = 0x4CEFC0;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    // clang-format off
    __asm
    {
        mov     ecx, DwThisInterface
        push    pAnimHierarchy
        push    pClump
        call    DwFunc
    }
    // clang-format on;
}

CAnimBlendAssociationSAInterface* CAnimBlendAssociationSA::InitializeForCustomAnimation(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy)
{
    m_pInterface->fBlendAmount = 1.0;
    m_pInterface->fSpeed = 1.0;
    m_pInterface->pAnimBlendNodeArray = 0;
    m_pInterface->fBlendDelta = 0.0;
    m_pInterface->fCurrentTime = 0.0;
    m_pInterface->fTimeStep = 0.0;
    m_pInterface->m_nFlags = 0;
    m_pInterface->uiCallbackType = 0;
    m_pInterface->vTable = reinterpret_cast<DWORD*>(0x85C6D0);
    m_pInterface->sAnimGroup = -1;
    m_pInterface->sAnimID = -1;
    m_pInterface->listEntry.prev = 0;
    m_pInterface->listEntry.next = 0;
    Init(pClump, pAnimHierarchy);
    return m_pInterface;
}

void CAnimBlendAssociationSA::Init(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy)
{
    DWORD DwFunc = 0x4CED50;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    // clang-format off
    __asm
    {
        mov     ecx, DwThisInterface
        push    pAnimHierarchy
        push    pClump
        call    DwFunc
    }
    // clang-format on;
}

void CAnimBlendAssociationSA::AllocateAnimBlendNodeArray(int iCount)
{
    DWORD DwFunc = 0x4CE9F0;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    // clang-format off
    __asm
    {
        mov     ecx, DwThisInterface
        push    iCount
        call    DwFunc
    }
    // clang-format on;
}

void CAnimBlendAssociationSA::FreeAnimBlendNodeArray()
{
    DWORD DwFunc = 0x4CEA40;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    // clang-format off
    __asm
    {
        mov     ecx, DwThisInterface
        call    DwFunc
    }
    // clang-format on;
}

std::unique_ptr<CAnimBlendHierarchy> CAnimBlendAssociationSA::GetAnimHierarchy()
{
    return pGame->GetAnimManager()->GetAnimBlendHierarchy(m_pInterface->pAnimHierarchy);
}

const std::unique_ptr<CAnimBlendHierarchy> CAnimBlendAssociationSA::GetAnimHierarchy() const noexcept
{
    return pGame->GetAnimManager()->GetAnimBlendHierarchy(m_pInterface->pAnimHierarchy);
}

void CAnimBlendAssociationSA::RestrictToBonesOf(const CAnimBlendStaticAssociationSAInterface* pOriginalAssoc)
{
    if (!pOriginalAssoc || !pOriginalAssoc->pAnimBlendNodesSequenceArray)
        return;

    // Custom animations are padded out to the full 32 bone skeleton when their IFP is loaded, with a
    // fixed pose standing in for every bone the animation doesn't define. That padding is what a full
    // body animation needs, but for a partial one it would drive the root, pelvis and legs the original
    // never touched, leaving the ped rigid. Keep only the bones the original animation itself animates.
    void* const*         ppOriginalSequences = reinterpret_cast<void* const*>(pOriginalAssoc->pAnimBlendNodesSequenceArray);
    const unsigned short numOriginalNodes = pOriginalAssoc->nNumBlendNodes;

    for (unsigned short i = 0; i < m_pInterface->cNumBlendNodes; i++)
    {
        const bool isAnimatedByOriginal = i < numOriginalNodes && ppOriginalSequences[i] != nullptr;
        if (!isAnimatedByOriginal)
            m_pInterface->pAnimBlendNodeArray[i].pAnimSequence = nullptr;
    }
}

void CAnimBlendAssociationSA::RestrictToBones(std::bitset<32> animatedBonesMask)
{
    // Same padding problem as RestrictToBonesOf, but for animations played directly from a custom IFP
    // bank rather than replacing a built-in one. There's no built-in association to compare against, so
    // the caller passes the set of bones the source IFP animation itself defines (see CClientIFP).
    for (unsigned short i = 0; i < m_pInterface->cNumBlendNodes; i++)
    {
        const bool isAnimated = i < animatedBonesMask.size() && animatedBonesMask.test(i);
        if (!isAnimated)
            m_pInterface->pAnimBlendNodeArray[i].pAnimSequence = nullptr;
    }
}

void CAnimBlendAssociationSA::SetCurrentProgress(float fProgress)
{
    float fTime = m_pInterface->pAnimHierarchy->fTotalTime * fProgress;

    DWORD DwFunc = 0x4CEA80;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    // clang-format off
    __asm
    {
        mov     ecx, DwThisInterface
        push    fTime
        call    DwFunc
    }
    // clang-format on;
}
