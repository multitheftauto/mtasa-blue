/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFireManagerSA.h
 *  PURPOSE:     Header file for fire manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CFireManager.h>
#include <vector>

class CEntity;
class CFireSA;
class CFireSAInterface;

class CFireManagerSA : public CFireManager
{
    friend class CFireSA;

private:
    std::vector<std::unique_ptr<CFireSA>> m_Fires;
    std::vector<bool>                     m_Visited;

    FireDestructionHandler m_destructionHandler{nullptr};

public:
    CFireManagerSA();
    ~CFireManagerSA();

    CFire* StartFire(const CVector& position, float size, CEntity* creator, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed = 100, bool makeNoise = true) override;
    CFire* StartFire(CEntity* target, CEntity* creator, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed = 100, bool makeNoise = true) override;
    CFire* FindNearestFire(CVector* position, bool checkExtinguished, bool checkScript) override;
    CFireSA* GetRandomFire() const noexcept;

    static CFireSA* GetFire(CFireSAInterface* iface) noexcept;

    void ExtinguishPoint(const CVector& position, float radius) override;
    bool ExtinguishPointWithWater(const CVector& position, float radius, float waterStrength) override;
    void ExtinguishAllFires() override;

    bool          PlentyFiresAvailable() override;
    std::uint32_t GetNumFiresInRange(const CVector& position, float radius) const override;
    std::size_t   GetNumFires() const noexcept { return m_Fires.size(); }

    void Update();

    void        SetFireDestructionHandler(FireDestructionHandler destructionHandler) noexcept override { m_destructionHandler = destructionHandler; }

    static void StaticSetHooks();

private:
    void ClearExtinguishedFires();
    CFire* GetStrongestFire() const;
};
