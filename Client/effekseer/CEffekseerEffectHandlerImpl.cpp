#include "CEffekseerEffectHandlerImpl.h"

void CEffekseerEffectHandlerImpl::GetPosition(CVector& vecPosition)
{
}

void CEffekseerEffectHandlerImpl::GetRotation(CVector& vecPosition)
{

}

void CEffekseerEffectHandlerImpl::SetPosition(const CVector& vecPosition)
{
    m_pInterface->SetLocation(m_Handle, ::Effekseer::Vector3D(vecPosition.fX, vecPosition.fY, vecPosition.fZ));
}

void CEffekseerEffectHandlerImpl::SetRotation(const CVector& vecRotation)
{
    m_pInterface->SetRotation(m_Handle, vecRotation.fX, vecRotation.fY, vecRotation.fZ);
}

void CEffekseerEffectHandlerImpl::SetScale(const CVector& vecScale)
{
    m_pInterface->SetScale(m_Handle, vecScale.fX, vecScale.fY, vecScale.fZ);
}

void CEffekseerEffectHandlerImpl::StopEffect()
{
    m_pInterface->StopEffect(m_Handle);
}

void CEffekseerEffectHandlerImpl::StopRoot()
{
    m_pInterface->StopRoot(m_Handle);
}

void CEffekseerEffectHandlerImpl::SetSpeed(float fSpeed)
{
    m_pInterface->SetSpeed(m_Handle, fSpeed);
}

float CEffekseerEffectHandlerImpl::GetSpeed()
{
    return m_pInterface->GetSpeed(m_Handle);
}

void CEffekseerEffectHandlerImpl::SetDynamicInput(int32_t index, float fValue)
{
    m_pInterface->SetDynamicInput(m_Handle, index, fValue);
}

float CEffekseerEffectHandlerImpl::GetDynamicInput(int32_t index)
{
    return m_pInterface->GetDynamicInput(m_Handle, index);
}
