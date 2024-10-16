/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C2DEffectsSA.cpp
 *  PURPOSE:     2dfx class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "C2DEffectsSA.h"
#include "C2DEffectSA.h"
#include "CGameSA.h"

extern CGameSA* pGame;

C2DEffect* C2DEffectsSA::Create(std::uint32_t model, const CVector& position, const e2dEffectType& type)
{
    CModelInfo* modelInfo = pGame->GetModelInfo(model);
    if (!modelInfo)
        return nullptr;

    C2DEffectSAInterface* effectInterface = modelInfo->Add2DFXEffect(position, type);
    if (!effectInterface)
        return nullptr;

    C2DEffectSA* effect = new C2DEffectSA(effectInterface, model);
    return effect;
}

void C2DEffectsSA::Destroy(C2DEffect* effect)
{
    if (!effect)
        return;

    auto* effectInterface = dynamic_cast<C2DEffectSA*>(effect)->GetInterface();
    for (auto it = m_effects.begin(); it != m_effects.end(); ++it)
    {
        if ((*it)->GetInterface() == effectInterface)
        {
            (*it)->Destroy();

            delete *it;
            m_effects.erase(it);
            break;
        }
    }
}

C2DEffectSA* C2DEffectsSA::Get(C2DEffectSAInterface* effectInterface) const
{
    for (const auto& effect : m_effects)
        if (effect->GetInterface() == effectInterface)
            return effect;

    return nullptr;
}

void C2DEffectsSA::RemoveFromList(C2DEffectSA* effect)
{
    auto& it = std::find(m_effects.begin(), m_effects.end(), effect);
    if (it != m_effects.end())
        m_effects.erase(it);
}
