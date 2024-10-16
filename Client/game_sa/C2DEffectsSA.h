/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/C2DEffectsSA.h
 *  PURPOSE:     Header file for 2dfx class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "C2DEffectSA.h"

class C2DEffectsSA : public C2DEffects
{
public:
    C2DEffect* Create(std::uint32_t model, const CVector& position, const e2dEffectType& type) override;
    void       Destroy(C2DEffect* effect) override;
    C2DEffectSA* Get(C2DEffectSAInterface* effectInterface) const override;
    void         AddToList(C2DEffectSA* effect) { m_effects.push_back(effect); }
    void         RemoveFromList(C2DEffectSA* effect);

private:
    std::list<C2DEffectSA*> m_effects;
};
