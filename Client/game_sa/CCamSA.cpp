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

extern CGameSA* pGame;

CEntity* CCamSA::GetTargetEntity() const
{

    if (!m_pInterface)
        return nullptr;
        
    CEntitySAInterface* pInterface = m_pInterface->CamTargetEntity;
    if (pInterface)
    {           
        CPools* pPools = pGame->GetPools();
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

    float fHoriz = m_pInterface->m_fHorizontalAngle;
    float fVert = m_pInterface->m_fVerticalAngle;
    
    if (!std::isfinite(fHoriz) || !std::isfinite(fVert))
    {
        fHorizontal = 0.0f;
        fVertical = 0.0f;
        return;
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
        
    // Clamp angle values to prevent overflow
    constexpr float MIN_ANGLE = -360.0f;
    constexpr float MAX_ANGLE = 360.0f;
    
    if (fHorizontal < MIN_ANGLE || fHorizontal > MAX_ANGLE ||
        fVertical < MIN_ANGLE || fVertical > MAX_ANGLE)
        return;

    // Calculation @ sub 0x50F970
    m_pInterface->m_fHorizontalAngle = fHorizontal;
    m_pInterface->m_fVerticalAngle = fVertical;
}