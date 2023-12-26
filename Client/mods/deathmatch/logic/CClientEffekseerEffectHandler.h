/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientEffekseerEffect.h
 *  PURPOSE:     Effekseer effect class
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"
#include "Effekseer.h"

class CClientEffekseerEffectHandler final : public CClientEntity
{
    DECLARE_CLASS(CClientEffekseerEffectHandler, CClientEntity)
public:
    CClientEffekseerEffectHandler(class CClientManager* pManager, ElementID ID, Effekseer::Handle handle);
    ~CClientEffekseerEffectHandler();

    void Unlink(){};

    eClientEntityType GetType() const { return CCLIENTEFFEKSEERFXHANDLER; }
    const CVector&    GetPosition() { return m_vecPosition; };
    void              GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    void              SetPosition(const CVector& vecPosition);
    void              SetRotation(const CVector& vecRotation);
    void              SetScale(const CVector& vecScale);
    void              Stop();
    void              StopRoot();
    void              SetSpeed(float fSpeed);
    float             GetSpeed();
    void              SetDynamicInput(int32_t index, float fValue);
    float             GetDynamicInput(int32_t index);

private:
    CVector               m_vecPosition;
    Effekseer::ManagerRef m_pInternalInterface;
    Effekseer::Handle     m_Handle;
};
