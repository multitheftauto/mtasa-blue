
#include "StdInc.h"
#include "CEffekseerEffectHandlerImpl.h"

// m_Handle can be pointing to removed effect handle
// Effekseer handle this situation in API
// But this causes collision after 2_147_483_647 played effcts
// This is too much to fix this behavior

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
    m_pInterface->SetLocation(m_Handle, reinterpret_cast<const Effekseer::Vector3D&>(vecPosition));
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

void CEffekseerEffectHandlerImpl::SetRandomSeed(int32_t seed)
{
    m_pInterface->SetRandomSeed(m_Handle, seed);
}

void CEffekseerEffectHandlerImpl::SetLayer(int32_t layer)
{
    m_pInterface->SetLayer(m_Handle, layer);
}

int32_t CEffekseerEffectHandlerImpl::GetLayer()
{
    return m_pInterface->GetLayer(m_Handle);
}

bool CEffekseerEffectHandlerImpl::GetSpawnDisabled()
{
    return m_pInterface->GetSpawnDisabled(m_Handle);
}

void CEffekseerEffectHandlerImpl::SetSpawnDisabled(bool status)
{
    m_pInterface->SetSpawnDisabled(m_Handle, status);
}

void CEffekseerEffectHandlerImpl::SetPaused(bool status)
{
    m_pInterface->SetPaused(m_Handle, status);
}

bool CEffekseerEffectHandlerImpl::GetPaused()
{
    return m_pInterface->GetPaused(m_Handle);
}

void CEffekseerEffectHandlerImpl::SetShown(bool status)
{
    m_pInterface->SetShown(m_Handle, status);
}

bool CEffekseerEffectHandlerImpl::GetShown()
{
    return m_pInterface->GetShown(m_Handle);
}

void CEffekseerEffectHandlerImpl::SendTrigger(int32_t index)
{
    m_pInterface->SendTrigger(m_Handle, index);
}

void CEffekseerEffectHandlerImpl::SetTargetLocation(const CVector& location)
{
    m_pInterface->SetLocation(m_Handle, reinterpret_cast<const Effekseer::Vector3D&>(location));
}

void CEffekseerEffectHandlerImpl::SetAllColor(SColor color)
{
    m_pInterface->SetAllColor(m_Handle, Effekseer::Color(color.R, color.G, color.B, color.A));
}

bool CEffekseerEffectHandlerImpl::Exists()
{
    return m_pInterface->Exists(m_Handle);
}
