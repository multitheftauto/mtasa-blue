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
    // CAnimBlendAssocGroup::CopyAnimation
    return ((CAnimBlendAssociationSAInterface * (__thiscall*)(CAnimBlendAssocGroupSAInterface*, unsigned int)) FUNC_CAnimBlendAssocGroup_CopyAnimation)(
        m_pInterface, AnimID);
}

void CAnimBlendAssocGroupSA::InitEmptyAssociations(RpClump* pClump)
{
    // CAnimBlendAssocGroup::InitEmptyAssociations
    return ((void(__thiscall*)(CAnimBlendAssocGroupSAInterface*, RpClump*))FUNC_CAnimBlendAssocGroup_InitEmptyAssociations)(m_pInterface, pClump);
}

bool CAnimBlendAssocGroupSA::IsCreated()
{
    // CAnimBlendAssocGroup::IsCreated
    return ((bool(__thiscall*)(CAnimBlendAssocGroupSAInterface*))FUNC_CAnimBlendAssocGroup_IsCreated)(m_pInterface);
}

int CAnimBlendAssocGroupSA::GetNumAnimations()
{
    // CAnimBlendAssocGroup::GetNumAnimations
    return ((int(__thiscall*)(CAnimBlendAssocGroupSAInterface*))FUNC_CAnimBlendAssocGroup_GetNumAnimations)(m_pInterface);
}

CAnimBlock* CAnimBlendAssocGroupSA::GetAnimBlock()
{
    SetupAnimBlock();

    return m_pAnimBlock;
}

CAnimBlendStaticAssociation* CAnimBlendAssocGroupSA::GetAnimation(unsigned int ID)
{
    // CAnimBlendAssocGroup::GetAnimation
    return ((CAnimBlendStaticAssociation * (__thiscall*)(CAnimBlendAssocGroupSAInterface*, unsigned int)) FUNC_CAnimBlendAssocGroup_GetAnimation)(m_pInterface,
                                                                                                                                                  ID);
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
    // CAnimBlendAssocGroup::CreateAssociations
    ((void(__thiscall*)(CAnimBlendAssocGroupSAInterface*, const char*))FUNC_CAnimBlendAssocGroup_CreateAssociations)(m_pInterface, szBlockName);
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
