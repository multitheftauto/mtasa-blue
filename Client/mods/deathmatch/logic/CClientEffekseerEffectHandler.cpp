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

CClientEffekseerEffectHandler::CClientEffekseerEffectHandler(CClientManager* pManager, ElementID ID, CEffekseerEffectHandler *handle)
    : ClassInit(this), CClientEntity(ID), m_pHandle(handle)
{
    SetTypeName("effekseer-handler");
    m_pManager = pManager;
}

CClientEffekseerEffectHandler::~CClientEffekseerEffectHandler()
{
    if (m_pHandle)
    {
        Stop();
    }
}

void CClientEffekseerEffectHandler::SetPosition(const CVector& vecPosition)
{
    m_vecPosition = vecPosition;
    m_pHandle->SetPosition(vecPosition);
}

void CClientEffekseerEffectHandler::SetRotation(const CVector& vecRotation)
{
    m_pHandle->SetRotation(vecRotation);
}

void CClientEffekseerEffectHandler::SetScale(const CVector& vecScale)
{
    m_pHandle->SetScale(vecScale);
}

void CClientEffekseerEffectHandler::Stop()
{
    m_pHandle->StopEffect();
}

void CClientEffekseerEffectHandler::StopRoot()
{
    m_pHandle->StopRoot();
}

void CClientEffekseerEffectHandler::SetSpeed(float fSpeed)
{
    m_pHandle->SetSpeed(fSpeed);
}

float CClientEffekseerEffectHandler::GetSpeed()
{
    return m_pHandle->GetSpeed();
}

void CClientEffekseerEffectHandler::SetDynamicInput(int32_t index, float fValue)
{
    m_pHandle->SetDynamicInput(index, fValue);
}

float CClientEffekseerEffectHandler::GetDynamicInput(int32_t index)
{
    return m_pHandle->GetDynamicInput(index);
}
