/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendAssociationSA.cpp
 *  PURPOSE:     Animation blend association
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAnimBlendAssociationSA.h"
#include "CAnimBlendHierarchySA.h"
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
    _asm
    {
        mov     ecx, DwThisInterface
        push    staticAssociationByReference
        call    DwFunc
    };
}

CAnimBlendAssociationSAInterface* CAnimBlendAssociationSA::Constructor(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy)
{
    DWORD DwFunc = 0x4CEFC0;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    _asm
    {
        mov     ecx, DwThisInterface
        push    pAnimHierarchy
        push    pClump
        call    DwFunc
    };
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
    _asm
    {
        mov     ecx, DwThisInterface
        push    pAnimHierarchy
        push    pClump
        call    DwFunc
    };
}

void CAnimBlendAssociationSA::AllocateAnimBlendNodeArray(int iCount)
{
    DWORD DwFunc = 0x4CE9F0;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    _asm
    {
        mov     ecx, DwThisInterface
        push    iCount
        call    DwFunc
    };
}

void CAnimBlendAssociationSA::FreeAnimBlendNodeArray()
{
    DWORD DwFunc = 0x4CEA40;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    _asm
    {
        mov     ecx, DwThisInterface
        call    DwFunc
    };
}

std::unique_ptr<CAnimBlendHierarchy> CAnimBlendAssociationSA::GetAnimHierarchy()
{
    return pGame->GetAnimManager()->GetAnimBlendHierarchy(m_pInterface->pAnimHierarchy);
}

const std::unique_ptr<CAnimBlendHierarchy> CAnimBlendAssociationSA::GetAnimHierarchy() const noexcept
{
    return pGame->GetAnimManager()->GetAnimBlendHierarchy(m_pInterface->pAnimHierarchy);
}

void CAnimBlendAssociationSA::SetCurrentProgress(float fProgress)
{
    float fTime = m_pInterface->pAnimHierarchy->fTotalTime * fProgress;

    DWORD DwFunc = 0x4CEA80;
    DWORD DwThisInterface = reinterpret_cast<DWORD>(m_pInterface);
    _asm
    {
        mov     ecx, DwThisInterface
        push    fTime
        call    DwFunc
    };
}
