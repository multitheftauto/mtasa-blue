
#include "StdInc.h"
#include "CEffekseerEffectHandlerImpl.h"

void CEffekseerEffectHandlerImpl::GetPosition(CVector& vecPosition)
{
    auto pos = m_pInterface->GetLocation(m_Handle);
    vecPosition = CVector(pos.X, pos.Y, pos.Z);
}

void CEffekseerEffectHandlerImpl::GetRotation(CVector& vecRotation)
{
    // TODO
}

void CEffekseerEffectHandlerImpl::SetPosition(const CVector& vecPosition)
{
    m_pInterface->SetLocation(m_Handle, ::Effekseer::Vector3D(vecPosition.fX, vecPosition.fY, vecPosition.fZ));
}

// ZXY rotation is used
void CEffekseerEffectHandlerImpl::SetRotation(const CVector& vecRotation)
{
    m_pInterface->SetRotation(m_Handle, vecRotation.fX, vecRotation.fY, vecRotation.fZ);
}

void CEffekseerEffectHandlerImpl::GetMatrix(CMatrix& matrix)
{
    matrix = reinterpret_cast<CMatrix&>(m_pInterface->GetMatrix(m_Handle));
}

void CEffekseerEffectHandlerImpl::SetMatrix(const CMatrix& matrix)
{
    m_pInterface->SetMatrix(m_Handle, reinterpret_cast<const Effekseer::Matrix43&>(matrix));
}

void CEffekseerEffectHandlerImpl::SetScale(const CVector& vecScale)
{
    m_pInterface->SetScale(m_Handle, vecScale.fX, vecScale.fY, vecScale.fZ);
}

void CEffekseerEffectHandlerImpl::StopEffect()
{
    // Effect will be removed here
    m_pInterface->StopEffect(m_Handle);
    // Free handler too
    delete this;
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
