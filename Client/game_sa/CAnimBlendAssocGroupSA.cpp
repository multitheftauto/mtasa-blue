/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendAssocGroupSA.cpp
 *  PURPOSE:     Animation blend association group
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CAnimBlendAssocGroupSA::CAnimBlendAssocGroupSA(CAnimBlendAssocGroupSAInterface* pInterface)
{
    m_pInterface = pInterface;
    m_pAnimBlock = NULL;
    SetupAnimBlock();
}

CAnimBlendAssociationSAInterface* CAnimBlendAssocGroupSA::CopyAnimation(unsigned int AnimID)
{
    CAnimBlendAssociationSAInterface* pAnimAssociationReturn = nullptr;

    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendAssocGroup_CopyAnimation;
    _asm
    {
        mov     ecx, dwThis
        push    AnimID
        call    dwFunc
        mov     pAnimAssociationReturn, eax
    }
    return pAnimAssociationReturn;
}

void CAnimBlendAssocGroupSA::InitEmptyAssociations(RpClump* pClump)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendAssocGroup_InitEmptyAssociations;
    _asm
    {
        mov     ecx, dwThis
        push    pClump
        call    dwFunc
    }
}

bool CAnimBlendAssocGroupSA::IsCreated()
{
    bool  bReturn;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendAssocGroup_IsCreated;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

int CAnimBlendAssocGroupSA::GetNumAnimations()
{
    int   iReturn;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendAssocGroup_GetNumAnimations;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     iReturn, eax
    }
    return iReturn;
}

CAnimBlock* CAnimBlendAssocGroupSA::GetAnimBlock()
{
    SetupAnimBlock();

    return m_pAnimBlock;
}

CAnimBlendStaticAssociation* CAnimBlendAssocGroupSA::GetAnimation(unsigned int ID)
{
    // ppAssociations [ ID - this->iIDOffset ] ??
    CAnimBlendStaticAssociation* pReturn;
    DWORD                        dwThis = (DWORD)m_pInterface;
    DWORD                        dwFunc = FUNC_CAnimBlendAssocGroup_GetAnimation;
    _asm
    {
        mov     ecx, dwThis
        push    ID
        call    dwFunc
        mov     pReturn, eax
    }
    return pReturn;
}

eAnimGroup CAnimBlendAssocGroupSA::GetGroupID()
{
    if ((DWORD)m_pInterface < 0x250)
    {
        g_pCore->LogEvent(543, "CAnimBlendAssocGroupSA::GetGroupID", "Incorrect Group Interface",
            SString("pAnimAssocGroupInterface = %p", m_pInterface), 543);
    }
    return static_cast<eAnimGroup>(m_pInterface->groupID);
};

bool CAnimBlendAssocGroupSA::IsLoaded()
{
    if (m_pInterface->pAnimBlock)
    {
        return m_pInterface->pAnimBlock->bLoaded;
    }
    return false;
}

void CAnimBlendAssocGroupSA::CreateAssociations(const char* szBlockName)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendAssocGroup_CreateAssociations;
    _asm
    {
        mov     ecx, dwThis
        push    szBlockName
        call    dwFunc
    }
}

void CAnimBlendAssocGroupSA::SetupAnimBlock()
{
    // Make sure our AnimBlock matches up with our interface's
    CAnimBlockSAInterface* pCurrent = (m_pAnimBlock) ? m_pAnimBlock->m_pInterface : NULL;
    CAnimBlockSAInterface* pActual = m_pInterface->pAnimBlock;
    if (pCurrent != pActual)
    {
        if (m_pAnimBlock)
        {
            delete m_pAnimBlock;
            m_pAnimBlock = NULL;
        }
        if (pActual)
            m_pAnimBlock = new CAnimBlockSA(pActual);
    }
}
