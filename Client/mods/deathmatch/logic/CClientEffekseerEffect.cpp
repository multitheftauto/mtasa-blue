/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientEffekseerEffect.cpp
 *  PURPOSE:     Dummy entity class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <codecvt>

CClientEffekseerEffect::CClientEffekseerEffect(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    SetTypeName("effekseer-effect");

    m_pManager = pManager;
    m_pEffect = nullptr;
}

CClientEffekseerEffect::~CClientEffekseerEffect()
{
}

bool CClientEffekseerEffect::Load(std::string strPath)
{
    if (!FileExists(strPath))
        return false;

    if (m_pEffect)
        return false;

    m_pEffect = m_pManager->GetEffekseerManagerImpl()->Create(strPath.c_str());

    return (m_pEffect != nullptr) ? true : false;
}

CClientEffekseerEffectHandler* CClientEffekseerEffect::Play(const CVector& pos)
{
    auto handle = m_pEffect->Play(pos);

    CClientEffekseerEffectHandler* pEffect = new CClientEffekseerEffectHandler(m_pManager, INVALID_ELEMENT_ID, handle);
    pEffect->SetParent(this);
    m_Handles.push_back(pEffect);

    return pEffect;
}
