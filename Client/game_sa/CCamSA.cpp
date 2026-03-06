/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CCamSA.cpp
 *  PURPOSE:     Camera entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCamSA.h"
#include "CGameSA.h"
#include <cmath>
#include <cfloat>

namespace
{
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kTwoPi = 6.28318530717958647692f;

    inline float WrapAngleRad(float angle) noexcept
    {
        angle -= kTwoPi * std::floor((angle + kPi) / kTwoPi);
        if (angle <= -kPi)
            angle += kTwoPi;
        else if (angle > kPi)
            angle -= kTwoPi;
        return angle;
    }
}

extern CGameSA* pGame;

CEntity* CCamSA::GetTargetEntity() const
{
    if (!m_pInterface)
        return nullptr;

    if (!pGame)
        return nullptr;

    CEntitySAInterface* pInterface = m_pInterface->CamTargetEntity;
    if (pInterface)
    {
        CPools* pPools = pGame->GetPools();
        if (pPools)
            return pPools->GetEntity((DWORD*)pInterface);
    }
    return nullptr;
}

void CCamSA::SetTargetEntity(CEntity* pEntity)
{
    if (!m_pInterface)
        return;

    if (pEntity)
    {
        auto pEntityInterface = pEntity->GetInterface();
        if (!pEntityInterface)
            return;

        m_pInterface->CamTargetEntity = pEntityInterface;
    }
    else
    {
        m_pInterface->CamTargetEntity = nullptr;
    }
}

void CCamSA::GetDirection(float& fHorizontal, float& fVertical)
{
    if (!m_pInterface)
    {
        fHorizontal = 0.0f;
        fVertical = 0.0f;
        return;
    }

    const float fHoriz = std::isfinite(m_pInterface->m_fHorizontalAngle) ? WrapAngleRad(m_pInterface->m_fHorizontalAngle) : 0.0f;
    const float fVert = std::isfinite(m_pInterface->m_fVerticalAngle) ? WrapAngleRad(m_pInterface->m_fVerticalAngle) : 0.0f;

    if (!std::isfinite(m_pInterface->m_fHorizontalAngle) || !std::isfinite(m_pInterface->m_fVerticalAngle))
    {
        m_pInterface->m_fHorizontalAngle = fHoriz;
        m_pInterface->m_fVerticalAngle = fVert;
    }

    fHorizontal = fHoriz;
    fVertical = fVert;
}

void CCamSA::SetDirection(float fHorizontal, float fVertical)
{
    if (!m_pInterface)
        return;

    // Validate input float values
    if (!std::isfinite(fHorizontal) || !std::isfinite(fVertical))
        return;

    m_pInterface->m_fHorizontalAngle = WrapAngleRad(fHorizontal);
    m_pInterface->m_fVerticalAngle = WrapAngleRad(fVertical);
}
