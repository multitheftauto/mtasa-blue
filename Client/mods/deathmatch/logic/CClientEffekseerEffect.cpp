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
    m_pInternalInterface = g_pCore->GetGraphics()->GetEffekseerManager()->GetInterface();
    m_pEffect = nullptr;
}

CClientEffekseerEffect::~CClientEffekseerEffect()
{
}

bool CClientEffekseerEffect::Load(std::string strPath)
{
    if (!FileExists(strPath))
        return false;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::u16string utf8str = utf16conv.from_bytes(strPath.c_str());

    m_pEffect = Effekseer::Effect::Create(m_pInternalInterface, utf8str.c_str());

    return (m_pEffect != nullptr) ? true : false;
}

CClientEffekseerEffectHandler* CClientEffekseerEffect::Play(const CVector& pos)
{
    auto handle = m_pInternalInterface->Play(m_pEffect, pos.fX, pos.fY, pos.fZ);
    m_pInternalInterface->SetAutoDrawing(handle, true);

    CClientEffekseerEffectHandler* pEffect = new CClientEffekseerEffectHandler(m_pManager, INVALID_ELEMENT_ID, handle);
    pEffect->SetParent(this);
    m_Handles.push_back(pEffect);

    return pEffect;
}
