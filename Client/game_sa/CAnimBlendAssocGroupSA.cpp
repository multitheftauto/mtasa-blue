/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendAssocGroupSA.cpp
 *  PURPOSE:     Animation blend association group
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CCoreInterface.h>
#include "CAnimBlendAssocGroupSA.h"
#include "CAnimBlockSA.h"

extern CCoreInterface* g_pCore;

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
    // clang-format off
    pAnimAssociationReturn = gta_thiscall_address<decltype(pAnimAssociationReturn)>(dwFunc, dwThis, AnimID);
    // clang-format on
    return pAnimAssociationReturn;
}

void CAnimBlendAssocGroupSA::InitEmptyAssociations(RpClump* pClump)
{
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendAssocGroup_InitEmptyAssociations;
    // clang-format off
    gta_thiscall_address(dwFunc, dwThis, pClump);
    // clang-format on
}

bool CAnimBlendAssocGroupSA::IsCreated()
{
    bool  bReturn;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendAssocGroup_IsCreated;
    // clang-format off
    bReturn = gta_thiscall_address<decltype(bReturn)>(dwFunc, dwThis);
    // clang-format on
    return bReturn;
}

int CAnimBlendAssocGroupSA::GetNumAnimations()
{
    int   iReturn;
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CAnimBlendAssocGroup_GetNumAnimations;
    // clang-format off
    iReturn = gta_thiscall_address<decltype(iReturn)>(dwFunc, dwThis);
    // clang-format on
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
    // clang-format off
    pReturn = gta_thiscall_address<decltype(pReturn)>(dwFunc, dwThis, ID);
    // clang-format on
    return pReturn;
}

eAnimGroup CAnimBlendAssocGroupSA::GetGroupID()
{
    if ((DWORD)m_pInterface < 0x250)
    {
        g_pCore->LogEvent(543, "CAnimBlendAssocGroupSA::GetGroupID", "Incorrect Group Interface", SString("pAnimAssocGroupInterface = %p", m_pInterface), 543);
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
    // clang-format off
    gta_thiscall_address(dwFunc, dwThis, szBlockName);
    // clang-format on
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
