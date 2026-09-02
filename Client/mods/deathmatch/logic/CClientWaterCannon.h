/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientWaterCannon.h
 *  PURPOSE:     Water cannon entity class header
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"

class CClientWaterCannonManager;

// A standalone water jet using the native CWaterCannons system (the same one the Firetruck and
// SWAT tank use), but not tied to any real vehicle. See CClientWaterCannon.cpp for how the native
// side treats the "owner" id as fully opaque, which is what makes that safe.
class CClientWaterCannon final : public CClientEntity
{
    DECLARE_CLASS(CClientWaterCannon, CClientEntity)
    friend class CClientWaterCannonManager;

public:
    CClientWaterCannon(CClientManager* pManager, ElementID ID);
    ~CClientWaterCannon();

    void Unlink();

    eClientEntityType GetType() const { return CCLIENTWATERCANNON; };

    void GetPosition(CVector& vecPosition) const { vecPosition = m_vecPosition; };
    void SetPosition(const CVector& vecPosition) { m_vecPosition = vecPosition; };

    // The generic element rotation, kept in sync with the direction below so that scripts can
    // attach a model to us with the standard attachElements and have it aim correctly; see
    // CClientWaterCannon.cpp for the angle convention shared with the direction accessors.
    void GetRotationRadians(CVector& vecOutRadians) const;
    void SetRotationRadians(const CVector& vecRadians);

    // Always stored normalized, so it's a pure orientation; SetForce's value is the only thing
    // that controls the velocity's actual magnitude.
    const CVector& GetDirection() const { return m_vecDirection; };
    void           SetDirection(CVector vecDirection)
    {
        vecDirection.Normalize();
        m_vecDirection = vecDirection;
    };

    float GetForce() const { return m_fForce; };
    void  SetForce(float fForce) { m_fForce = fForce; };

    bool IsEnabled() const { return m_bEnabled; };
    void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; };

    // A custom jet colour; when unset the native fading light-blue is used
    bool          HasCustomColor() const { return m_bHasCustomColor; };
    const SColor& GetColor() const { return m_Color; };
    void          SetColor(const SColor& color);
    void          ResetColor();

    // False if the shared custom-cannon pool (CMultiplayer::CreateCustomWaterCannon, capped
    // separately from the vehicle-owned one) was full at construction time.
    bool HasNativeCannon() const { return m_pNativeCannon != nullptr; };

protected:
    void DoPulse();

private:
    CClientWaterCannonManager* m_pWaterCannonManager;

    CVector m_vecPosition;
    CVector m_vecDirection;
    float   m_fForce;
    bool    m_bEnabled;

    bool   m_bHasCustomColor;
    SColor m_Color;

    // Opaque handle from CMultiplayer::CreateCustomWaterCannon; our own native CWaterCannon
    // instance, kept apart from the vehicle-owned ones (see CClientWaterCannon.cpp).
    void* m_pNativeCannon;
};
