
#pragma once

#include <effekseer/CEffekseerEffect.h>

#include <Effekseer.h>

class CEffekseerEffectImpl : public CEffekseerEffect
{
    CEffekseerEffectHandler* Play(const CVector &pos);

private:
    Effekseer::ManagerRef m_pInternalInterface;
    Effekseer::EffectRef  m_pEffect;
};
