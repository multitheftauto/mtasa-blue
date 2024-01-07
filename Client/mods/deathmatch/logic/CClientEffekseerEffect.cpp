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
    m_pManager->GetEffekseerManager()->AddToList(this);
}

CClientEffekseerEffect::~CClientEffekseerEffect()
{
    m_pManager->GetEffekseerManager()->RemoveFromList(this);
    if (m_pEffect)
    {
        g_pCore->GetEffekseer()->Remove(m_pEffect);
    }
}

bool CClientEffekseerEffect::Load(const std::string &strPath)
{
    if (!FileExists(strPath))
        return false;

    if (m_pEffect)
        return false;

    m_pEffect = g_pCore->GetEffekseer()->Create(strPath.c_str());

    return m_pEffect != nullptr;
}

CClientEffekseerEffectHandler* CClientEffekseerEffect::Play(const CVector& pos)
{
    auto handle = m_pEffect->Play(pos);

    CClientEffekseerEffectHandler* pEffect = new CClientEffekseerEffectHandler(m_pManager, INVALID_ELEMENT_ID, handle);
    pEffect->SetParent(this);

    return pEffect;
}
