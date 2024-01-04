
#include "StdInc.h"

#include "CEffekseerEffectImpl.h"
#include "CEffekseerEffectHandlerImpl.h"

CEffekseerEffectHandler* CEffekseerEffectImpl::Play(const CVector &pos)
{
    auto handle = m_pInternalInterface->Play(m_pEffect, pos.fX, pos.fY, pos.fZ);
    if (handle == -1)
        return nullptr;

    m_pInternalInterface->SetAutoDrawing(handle, true);

    CEffekseerEffectHandlerImpl* pHandler = new CEffekseerEffectHandlerImpl(m_pInternalInterface, handle);

    return pHandler;
}
