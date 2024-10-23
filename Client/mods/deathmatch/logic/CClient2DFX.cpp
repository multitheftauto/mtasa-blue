/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClient2DFX.cpp
 *  PURPOSE:     Handling 2DFX effects
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClient2DFX.h"
#include "CClient2DFXManager.h"

CClient2DFX::CClient2DFX(class CClientManager* manager, ElementID ID)
    : ClassInit(this), CClientEntity(ID), m_2DFXManager(manager->Get2DFXManager()), m_effect(nullptr), m_effectType(e2dEffectType::NONE), m_model(0)
{
    m_pManager = manager;
    SetTypeName("2dfx");

    m_2DFXManager->AddToList(this);
}

CClient2DFX::~CClient2DFX()
{
    m_2DFXManager->RemoveFromList(this);

    // Destroy effect
    g_pGame->Get2DEffects()->Destroy(m_effect);
}

bool CClient2DFX::Create(std::uint32_t model, const CVector& position, const e2dEffectType& type, const effectDataMap& effectData)
{
    CModelInfo* modelInfo = g_pGame->GetModelInfo(static_cast<DWORD>(model));
    if (!modelInfo)
        return false;

    C2DEffect* effect = g_pGame->Get2DEffects()->Create(model, position, type);
    if (!effect)
        return false;

    m_2DFXManager->Set2DFXProperties(effect, effectData);

    m_effect = effect;
    m_effectType = type;
    m_model = model;

    return true;
}
