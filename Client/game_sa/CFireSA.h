/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFireSA.h
 *  PURPOSE:     Header file for fire class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CFire.h>

class CEntitySA;
class CFxSystemSAInterface;
class CEntitySAInterface;
class RwMatrix;

// Even though we have our own fire implementation, we still need to return
// a CFireSAInterface to ensure the correct memory layout is returned to GTA
// when performing operations on the fire object (e.g., FindNearestFire).
// We cannot return CFireSA directly, because its structure is offset by 4 bytes
// due to the vfptr at offset 0.
class CFireSAInterface
{
public:
    struct
    {
        std::uint8_t isActive : 1;
        std::uint8_t createdByScript : 1;  // unused in MTA
        std::uint8_t makeNoise : 1;
        std::uint8_t isBeingExtinguished : 1;
        std::uint8_t isFirstGeneration : 1; // unused in MTA
    } m_flags{};

    std::int16_t  scriptRefIndex{0};  // unused in MTA
    CVector       m_position;
    CEntitySAInterface*    m_entityOnFire{nullptr};
    CEntitySAInterface*    m_creator{nullptr};
    std::uint32_t m_lifetime{0};
    float         m_strength{1.0f};
    std::uint8_t  m_numGenerationsAllowed{100};
    std::uint8_t  m_removalDistance{60};
    CFxSystemSAInterface*  m_fxSystem{nullptr};
};

class CFireSA : public CFire
{
    friend class CFireManagerSA;

public:
    CFireSA(CFireManagerSA* fireMgr, CEntity* creator, CVector position, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed = 100, bool makeNoise = true);
    CFireSA(CFireManagerSA* fireMgr, CEntity* creator, CEntity* target, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed = 100, bool makeNoise = true);

    CFireSAInterface* GetInterface() noexcept { return &m_interface; }

    // Always pass ProcessPedCall = false, unless you know what you're doing. This is only used when extinguishing fire on a ped, and it will prevent the
    // PLAYER_ON_FIRE task from being aborted, which can cause crashes if the task is removed while it's being processed.
    void Extinguish(bool ProcessPedCall = false) override;
    void ExtinguishWithWater(float waterStrength);

    bool IsActive() const noexcept { return m_interface.m_flags.isActive; }
    void SetActive(bool active) noexcept { m_interface.m_flags.isActive = active; }

    bool IsBeingExtinguished() const noexcept { return m_interface.m_flags.isBeingExtinguished; }
    void SetBeingExtinguished(bool extinguished) noexcept { m_interface.m_flags.isBeingExtinguished = extinguished; }

    bool IsSilent() const noexcept { return !m_interface.m_flags.makeNoise; }
    void SetSilent(bool silent) noexcept { m_interface.m_flags.makeNoise = !silent; }

    std::uint8_t GetNumGenerationsAllowed() const noexcept { return m_interface.m_numGenerationsAllowed; }
    void         SetNumGenerationsAllowed(std::uint8_t numGenerationsAllowed) noexcept { m_interface.m_numGenerationsAllowed = numGenerationsAllowed; }

    std::uint32_t GetLifetime() const noexcept override { return m_interface.m_lifetime; }
    void SetLifetime(std::uint32_t lifetime) noexcept override { m_interface.m_lifetime = lifetime; }

    void     SetPosition(const CVector& position, bool updateParticle = false) override;
    CVector& GetPosition() noexcept override { return m_interface.m_position; }

    float GetStrength() const noexcept override { return m_interface.m_strength; }
    void  SetStrength(float strength, bool updateFX = true) override;

    void SetCreatedByScript(bool createdByScript) noexcept override { m_interface.m_flags.createdByScript = createdByScript; }
    bool IsCreatedByScript() const noexcept override { return m_interface.m_flags.createdByScript; }

    void ProcessFire();

    static void StaticSetHooks();

private:
    CFireSAInterface m_interface{};
    CEntity*         m_entityOnFire{nullptr};
    CEntity*         m_creator{nullptr};
    bool             m_createdByScript{false}; // created by createFire function

    CFireManagerSA* m_fireManager{nullptr};

private:
    CEntitySAInterface* GetCreator() const noexcept { return m_interface.m_creator; }
    void                SetCreator(CEntity* entity);

    CEntitySAInterface* GetEntityOnFire() const noexcept { return m_interface.m_entityOnFire; }
    void                SetEntityOnFire(CEntity* entity);

    void DestroyFxSys();
    void CreateFxSysForStrength(const CVector& position, RwMatrix* matrix);
};
