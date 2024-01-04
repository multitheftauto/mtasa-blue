
#pragma once

#include <effekseer/CEffekseerEffect.h>

#include <Effekseer.h>

class CEffekseerEffectImpl : public CEffekseerEffect
{
public:
    CEffekseerEffectImpl(Effekseer::ManagerRef effMager, Effekseer::EffectRef effRef) : m_pInternalInterface(effMager), m_pEffect(effRef){};
    ~CEffekseerEffectImpl() = default;

    CEffekseerEffectHandler* Play(const CVector &pos) override;

private:
    Effekseer::ManagerRef m_pInternalInterface;
    Effekseer::EffectRef  m_pEffect;
};
