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
#include "CClientEffekseerEffectHandler.h"
#include <effekseer/CEffekseerEffect.h>

class CClientEffekseerEffect final : public CClientEntity
{
    DECLARE_CLASS(CClientEffekseerEffect, CClientEntity)
public:
    CClientEffekseerEffect(class CClientManager* pManager, ElementID ID);
    ~CClientEffekseerEffect();

    // CClientEntity interface
    void              Unlink() override {};
    eClientEntityType GetType() const override { return CCLIENTEFFEKSEERFX; }
    void              GetPosition(CVector& vecPosition) const override {};
    void              SetPosition(const CVector& vecPosition) override {};
    //

    bool                           Load(const std::string& strPath);
    CClientEffekseerEffectHandler* Play(const CVector& vecPosition);

private:
    CEffekseerEffect* m_pEffect;
};
