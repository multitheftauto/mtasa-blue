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

extern CGameSA* pGame;

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
    m_pInterface->sFlags = 0;
    m_pInterface->uiCallbackType = 0;
    m_pInterface->vTable = reinterpret_cast<DWORD*>(0x85C6D0);
    m_pInterface->sAnimGroup = -1;
    m_pInterface->sAnimID = -1;
    m_pInterface->listEntry.prev = 0;
    m_pInterface->listEntry.next = 0;
    InitializeWithHierarchy(pClump, pAnimHierarchy);
    return m_pInterface;
}

void CAnimBlendAssociationSA::InitializeWithHierarchy(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimHierarchy)
{
    const unsigned short cBlendNodes = 32;
    m_pInterface->cNumBlendNodes = cBlendNodes;
    AllocateAnimBlendNodeArray(cBlendNodes);
    m_pInterface->pAnimHierarchy = pAnimHierarchy;
    for (size_t i = 0; i < cBlendNodes; i++)
    {
        m_pInterface->pAnimBlendNodeArray[i].pAnimBlendAssociation = m_pInterface;
        CAnimBlendSequenceSAInterface& sequence = pAnimHierarchy->pSequences[i];
        if (sequence.sNumKeyFrames > 0)
        {
            m_pInterface->pAnimBlendNodeArray[i].pAnimSequence = &sequence;
        }
    }
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

std::unique_ptr<CAnimBlendHierarchy> CAnimBlendAssociationSA::GetAnimHierarchy(void)
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
