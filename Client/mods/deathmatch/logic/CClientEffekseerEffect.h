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
#include "Effekseer.h"

class CClientEffekseerEffect final : public CClientEntity
{
    DECLARE_CLASS(CClientEffekseerEffect, CClientEntity)
public:
    CClientEffekseerEffect(class CClientManager* pManager, ElementID ID);
    ~CClientEffekseerEffect();

    void Unlink(){};

    eClientEntityType GetType() const { return CCLIENTEFFEKSEERFX; }
    const CVector&    GetPosition() { return m_vecPosition; };
    void              GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    void              SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };
    bool Load(std::string strPath);

    CClientEffekseerEffectHandler* Play(const CVector& vecPosition);

private:
    CVector               m_vecPosition;

    std::list<CClientEffekseerEffectHandler*> m_Handles;

    Effekseer::ManagerRef m_pInternalInterface;
    Effekseer::EffectRef  m_pEffect;
};
