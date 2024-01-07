
#include "StdInc.h"

#include "CEffekseerEffectImpl.h"
#include "CEffekseerEffectHandlerImpl.h"

constexpr Effekseer::Handle INVALID_HANDLE = -1;

CEffekseerEffectHandler* CEffekseerEffectImpl::Play(const CVector &pos)
{
    auto handle = m_pInternalInterface->Play(m_pEffect, reinterpret_cast<const Effekseer::Vector3D&>(pos), 0);
    if (handle == INVALID_HANDLE)
        return nullptr;

    m_pInternalInterface->SetAutoDrawing(handle, true);

    CEffekseerEffectHandlerImpl* pHandler = new CEffekseerEffectHandlerImpl(m_pInternalInterface, handle);
    return pHandler;
}
