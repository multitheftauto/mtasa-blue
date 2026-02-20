/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFire.h
 *  PURPOSE:     Fire class header
 *
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <game/CFire.h>

class CClientFire : public CClientEntity
{
    DECLARE_CLASS(CClientFire, CClientEntity)
    friend class CClientFireManager;

public:
    CClientFire(CClientFireManager* fireManager, const CVector& position, float strength, std::uint32_t lifetime, bool makeNoise);
    ~CClientFire();

    // CClientEntity stuff
    eClientEntityType GetType() const override { return CCLIENTFIRE; }
    CEntity*          GetGameEntity() override { return nullptr; }
    void              Unlink() override;

    CFire* GetGameFire() const noexcept { return m_fire; }

    void Extinguish();

    void     GetPosition(CVector& position) const override;
    void     SetPosition(const CVector& position);

    void SetStrength(float strength);
    float GetStrength() const noexcept;

    void SetLifetime(std::uint32_t lifetime);
    std::uint32_t GetLifetime() const noexcept;

    static void OnGameFireDestroyed(CFire* fire);

private:
    CFire* m_fire;

    CClientFireManager* m_fireManager{nullptr};
};
