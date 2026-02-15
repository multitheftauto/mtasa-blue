/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFire.cpp
 *  PURPOSE:     Fire class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientFire.h"
#include "game/CFireManager.h"

CClientFire::CClientFire(CClientFireManager* fireManager, const CVector& position, float strength, std::uint32_t lifetime, bool makeNoise)
    : ClassInit(this), CClientEntity(INVALID_ELEMENT_ID), m_fireManager{fireManager}
{
    SetTypeName("fire");

    m_fire = g_pGame->GetFireManager()->StartFire(position, strength, g_pGame->GetPedContext(), lifetime, 0, makeNoise);

    if (m_fire)
        m_fire->SetCreatedByScript(true);

    m_fireManager->AddToList(this);
}

CClientFire::~CClientFire()
{
    Unlink();
    Extinguish();
}

void CClientFire::Unlink()
{
    m_fireManager->RemoveFromList(this);
}

void CClientFire::Extinguish()
{
    if (!m_fire)
        return;

    m_fire->Extinguish();
}

void CClientFire::GetPosition(CVector& position) const
{
    position = m_fire ? m_fire->GetPosition() : CVector();
}

void CClientFire::SetPosition(const CVector& position)
{
    if (!m_fire)
        return;

    m_fire->SetPosition(position, true);
}

void CClientFire::SetStrength(float strength)
{
    if (!m_fire)
        return;

    m_fire->SetStrength(strength);
}

float CClientFire::GetStrength() const noexcept
{
    if (!m_fire)
        return 0.0f;

    return m_fire->GetStrength();
}

void CClientFire::SetLifetime(std::uint32_t lifetime)
{
    if (!m_fire)
        return;

    m_fire->SetLifetime(lifetime);
}

std::uint32_t CClientFire::GetLifetime() const noexcept
{
    if (!m_fire)
        return 0;

    return m_fire->GetLifetime();
}

void CClientFire::OnGameFireDestroyed(CFire* fire)
{
    if (!fire->IsCreatedByScript())
        return;

    CClientFire* clientFire = g_pClientGame->GetManager()->GetFireManager()->Get(fire);
    if (clientFire && !clientFire->IsBeingDeleted())
        CStaticFunctionDefinitions::DestroyElement(*clientFire);
}
