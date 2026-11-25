/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientWaterCannon.cpp
 *  PURPOSE:     Water cannon entity class
 *
 *****************************************************************************/

#include <StdInc.h>

CClientWaterCannon::CClientWaterCannon(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    m_pWaterCannonManager = pManager->GetWaterCannonManager();

    m_vecDirection = CVector(0.0f, 1.0f, 0.0f);
    m_fForce = 1.0f;
    m_bEnabled = true;
    m_bKnockdownEnabled = true;
    m_bHasCustomColor = false;
    m_Color = SColor(0xFFC8C8FF);  // the native jet's light blue (R200 G200 B255), opaque
    m_pNativeCannon = g_pMultiplayer->CreateCustomWaterCannon();

    SetTypeName("water-cannon");

    m_pWaterCannonManager->AddToList(this);
}

CClientWaterCannon::~CClientWaterCannon()
{
    Unlink();
    g_pMultiplayer->DestroyCustomWaterCannon(m_pNativeCannon);
}

void CClientWaterCannon::Unlink()
{
    m_pWaterCannonManager->RemoveFromList(this);
}

void CClientWaterCannon::SetColor(const SColor& color)
{
    m_Color = color;
    m_bHasCustomColor = true;
    if (m_pNativeCannon)
        g_pMultiplayer->SetCustomWaterCannonColor(m_pNativeCannon, color.R, color.G, color.B, color.A);
}

void CClientWaterCannon::ResetColor()
{
    m_bHasCustomColor = false;
    m_Color = SColor(0xFFC8C8FF);
    if (m_pNativeCannon)
        g_pMultiplayer->ResetCustomWaterCannonColor(m_pNativeCannon);
}

void CClientWaterCannon::DoPulse()
{
    // Update our position/rotation if we're attached, and let anything attached to us follow ours
    DoAttaching();

    if (!m_bEnabled || !m_pNativeCannon)
        return;

    // m_vecDirection is always normalized (see SetDirection), so this is purely "orientation
    // times force", not the direction's own magnitude bleeding into the velocity.
    const CVector vecVelocity = m_vecDirection * m_fForce;
    g_pMultiplayer->UpdateCustomWaterCannon(m_pNativeCannon, m_vecPosition, vecVelocity);
}

// Same heading/pitch-from-direction convention the native water cannons use
// (CAutomobile::FireTruckControl/TankControl), so a model attached to us via attachElements
// aims consistently with how the game's own nozzles look. atan2f's second argument here is
// never negative, so pitch always comes back between -90 and 90 degrees for a normalized
// direction, which is exactly the range SetRotationRadians below expects back.
void CClientWaterCannon::GetRotationRadians(CVector& vecOutRadians) const
{
    const float fHorizontalMagnitude = sqrtf(m_vecDirection.fX * m_vecDirection.fX + m_vecDirection.fY * m_vecDirection.fY);
    const float fHeading = atan2f(-m_vecDirection.fX, m_vecDirection.fY);
    const float fPitch = atan2f(m_vecDirection.fZ, fHorizontalMagnitude);
    vecOutRadians = CVector(fPitch, 0.0f, fHeading);
}

void CClientWaterCannon::SetRotationRadians(const CVector& vecRadians)
{
    const float fPitch = vecRadians.fX;
    const float fHeading = vecRadians.fZ;
    SetDirection(CVector(-sinf(fHeading) * cosf(fPitch), cosf(fHeading) * cosf(fPitch), sinf(fPitch)));
}
