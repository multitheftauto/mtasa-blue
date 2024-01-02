
#include "StdInc.h"

#include "CEffekseerEffectImpl.h"



CEffekseerEffectHandler* CEffekseerEffectImpl::Play(const CVector &pos)
{
    auto handle = m_pInternalInterface->Play(m_pEffect, pos.fX, pos.fY, pos.fZ);
    m_pInternalInterface->SetAutoDrawing(handle, true);

    return nullptr;
}
