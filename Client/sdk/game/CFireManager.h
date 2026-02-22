/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CFireManager.h
 *  PURPOSE:     Fire entity manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CFire;
class CVector;
class CEntity;

using FireCreationHandler = bool (*)(CEntity* target, CEntity* creator);
using FireDestructionHandler = void (*)(CFire* fire);

class CFireManager
{
public:
    virtual CFire* StartFire(const CVector& position, float size, CEntity* creator, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed = 100,
                             bool makeNoise = true) = 0;
    virtual CFire* StartFire(CEntity* target, CEntity* creator, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed = 100, bool makeNoise = true) = 0;
    virtual CFire* FindNearestFire(CVector* position, bool checkExtinguished, bool checkScript) = 0;

    virtual void ExtinguishPoint(const CVector& position, float radius) = 0;
    virtual bool ExtinguishPointWithWater(const CVector& position, float radius, float waterStrength) = 0;
    virtual void ExtinguishAllFires() = 0;

    virtual std::uint32_t GetNumFiresInRange(const CVector& position, float radius) const = 0;
    virtual bool          PlentyFiresAvailable() = 0;

    virtual void Update() = 0;

    virtual void SetFireCreationHandler(FireCreationHandler creationHandler) noexcept = 0;
    virtual void SetFireDestructionHandler(FireDestructionHandler destructionHandler) noexcept = 0;
};
