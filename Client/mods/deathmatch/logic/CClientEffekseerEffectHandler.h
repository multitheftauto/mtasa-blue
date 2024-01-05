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
#include <effekseer/CEffekseerEffectHandler.h>

class CClientEffekseerEffectHandler final : public CClientEntity
{
    DECLARE_CLASS(CClientEffekseerEffectHandler, CClientEntity)
public:
    CClientEffekseerEffectHandler(class CClientManager* pManager, ElementID ID, CEffekseerEffectHandler* handle);
    ~CClientEffekseerEffectHandler();

    void Unlink(){};

    eClientEntityType GetType() const override { return CCLIENTEFFEKSEERFXHANDLER; }
    void              GetPosition(CVector& vecPosition) const override;
    void              SetPosition(const CVector& vecPosition) override;
    void              GetRotationRadians(CVector& vecRotation) const override;
    void              SetRotationRadians(const CVector& vecRotation) override;
    bool              GetMatrix(CMatrix& matrix) const override;
    bool              SetMatrix(const CMatrix& matrix) override;

    void              SetScale(const CVector& vecScale);
    void              Stop() ;
    void              StopRoot();
    void              SetSpeed(float fSpeed);
    float             GetSpeed();
    void              SetDynamicInput(int32_t index, float fValue);
    float             GetDynamicInput(int32_t index);

private:
    CVector m_vecRotation;
    CEffekseerEffectHandler* m_pHandle;
};
