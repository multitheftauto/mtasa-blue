/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientEffekseerEffectHandler.cpp
 *  PURPOSE:     Effekseer handle class
 *
 *****************************************************************************/

#include "StdInc.h"

CClientEffekseerEffectHandler::CClientEffekseerEffectHandler(CClientManager* pManager, ElementID ID, Effekseer::Handle handle) : ClassInit(this), CClientEntity(ID)
{
    SetTypeName("effekseer-handler");

    m_Handle = handle;
    m_pManager = pManager;
    m_pInternalInterface = g_pCore->GetGraphics()->GetEffekseerManager()->GetInterface();
}

CClientEffekseerEffectHandler::~CClientEffekseerEffectHandler()
{
    Stop();
}

void CClientEffekseerEffectHandler::SetPosition(const CVector& vecPosition)
{
    m_vecPosition = vecPosition;
    m_pInternalInterface->SetLocation(m_Handle, ::Effekseer::Vector3D(vecPosition.fX, vecPosition.fY, vecPosition.fZ));
}

void CClientEffekseerEffectHandler::SetRotation(const CVector& vecRotation)
{
    m_pInternalInterface->SetRotation(m_Handle, vecRotation.fX, vecRotation.fY, vecRotation.fZ);
}

void CClientEffekseerEffectHandler::SetScale(const CVector& vecScale)
{
    m_pInternalInterface->SetScale(m_Handle, vecScale.fX, vecScale.fY, vecScale.fZ);
}

void CClientEffekseerEffectHandler::Stop()
{
    m_pInternalInterface->StopEffect(m_Handle);
}

void CClientEffekseerEffectHandler::StopRoot()
{
    m_pInternalInterface->StopRoot(m_Handle);
}

void CClientEffekseerEffectHandler::SetSpeed(float fSpeed)
{
    m_pInternalInterface->SetSpeed(m_Handle, fSpeed);
}

float CClientEffekseerEffectHandler::GetSpeed()
{
    return m_pInternalInterface->GetSpeed(m_Handle);
}

void CClientEffekseerEffectHandler::SetDynamicInput(int32_t index, float fValue)
{
    m_pInternalInterface->SetDynamicInput(m_Handle, index, fValue);
}

float CClientEffekseerEffectHandler::GetDynamicInput(int32_t index)
{
    return m_pInternalInterface->GetDynamicInput(m_Handle, index);
}
