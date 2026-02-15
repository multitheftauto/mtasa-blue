/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFireManagerSA.cpp
 *  PURPOSE:     Fire manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CFireManagerSA.h"
#include "CFireSA.h"
#include "CGameSA.h"
#include "CPoolsSA.h"
#include "game/CCamera.h"

extern CGameSA* pGame;

// Reserve space to avoid reallocations
// This is not a hard limit - m_Fires can grow beyond MAX_FIRES if needed
static constexpr const std::uint32_t MAX_FIRES = 100;

CFireManagerSA::CFireManagerSA()
{
    m_Fires.reserve(MAX_FIRES);
    m_Visited.reserve(MAX_FIRES);
}

CFireManagerSA::~CFireManagerSA()
{
    m_Fires.clear();
}

CFire* CFireManagerSA::StartFire(const CVector& position, float size, CEntity* creator, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed, bool makeNoise)
{
    // Call CWaterLevel::GetWaterLevelNoWaves
    float waterLevel = 0.0f;
    ((float(__cdecl*)(CVector, float*, float*, float*))0x6E8580)(position, &waterLevel, nullptr, nullptr);
    if (waterLevel > position.fZ)
        return nullptr;

    m_Fires.push_back(std::make_unique<CFireSA>(this, creator, position, lifetime, numGenerationsAllowed, makeNoise));
    return m_Fires.back().get();
}

CFire* CFireManagerSA::StartFire(CEntity* target, CEntity* creator, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed, bool makeNoise)
{
    if (target->IsOnFire() || target->IsFireProof())
        return nullptr;

    switch (target->GetEntityType())
    {
        case eEntityType::ENTITY_TYPE_PED:
        {
            auto* ped = dynamic_cast<CPed*>(target);
            if (!ped->IsPedInControl())
                return nullptr;

            break;
        }
        case eEntityType::ENTITY_TYPE_VEHICLE:
        {
            auto* vehicle = dynamic_cast<CVehicle*>(target);
            if (vehicle->GetVehicleInterface()->m_vehicleClass == VehicleClass::AUTOMOBILE && vehicle->GetDamageManager()->GetEngineStatus() >= 225)
                return nullptr;

            break;
        }
        default:
            break;
    }

    m_Fires.push_back(std::make_unique<CFireSA>(this, creator, target, lifetime, numGenerationsAllowed, makeNoise));
    return m_Fires.back().get();
}

CFire* CFireManagerSA::FindNearestFire(CVector* position, bool checkExtinguished, bool checkScript)
{
    float nearestDistance2DSquared = std::numeric_limits<float>::infinity();
    CFireSA* nearestFire = nullptr;

    for (const auto& fire : m_Fires)
    {
        if (checkExtinguished && fire->IsBeingExtinguished())
            continue;

        if (CEntitySAInterface* entityOnFire = fire->GetEntityOnFire())
        {
            if (entityOnFire->nType == eEntityType::ENTITY_TYPE_PED)
                continue;
        }

        float distance2DSquared = (fire->GetPosition() - *position).Length2DSquared();
        if (distance2DSquared < nearestDistance2DSquared)
        {
            nearestDistance2DSquared = distance2DSquared;
            nearestFire = fire.get();
        }
    }

    return nearestFire;
}

CFireSA* CFireManagerSA::GetRandomFire() const noexcept
{
    if (m_Fires.empty())
        return nullptr;

    return m_Fires[std::rand() % m_Fires.size()].get();
}

CFire* CFireManagerSA::GetStrongestFire() const
{
    CFireSA* strongestFire = nullptr;
    for (std::size_t i = 0; i < m_Fires.size(); i++)
    {
        if (m_Visited[i])
            continue;

        if (!strongestFire || m_Fires[i]->GetStrength() > strongestFire->GetStrength())
            strongestFire = m_Fires[i].get();
    }

    return strongestFire;
}

CFireSA* CFireManagerSA::GetFire(CFireSAInterface* iface) noexcept
{
    return reinterpret_cast<CFireSA*>(reinterpret_cast<char*>(iface) - offsetof(CFireSA, m_interface));
}

void CFireManagerSA::ExtinguishPoint(const CVector& position, float radius)
{
    float radiusSquared = radius * radius;

    for (const auto& fire : m_Fires)
    {
        if ((fire->GetPosition() - position).LengthSquared() > radiusSquared)
            continue;

        fire->Extinguish();
    }
}

bool CFireManagerSA::ExtinguishPointWithWater(const CVector& position, float radius, float waterStrength)
{
    float radiusSquared = radius * radius;
    bool  anyFireExtinguished = false;

    for (const auto& fire : m_Fires)
    {
        if ((fire->GetPosition() - position).LengthSquared() > radiusSquared)
            continue;

        fire->ExtinguishWithWater(waterStrength);
        anyFireExtinguished = true;
    }

    return anyFireExtinguished;
}

void CFireManagerSA::ExtinguishAllFires()
{
    for (auto& fire : m_Fires)
        fire->Extinguish();
}

bool CFireManagerSA::PlentyFiresAvailable()
{
    return m_Fires.size() >= 6;
}

std::uint32_t CFireManagerSA::GetNumFiresInRange(const CVector& position, float radius) const
{
    std::uint32_t firesCount = 0;
    float         radiusSquared = radius * radius;

    for (const auto& fire : m_Fires)
    {
        float dist = (fire->GetPosition() - position).Length2DSquared();
        if (dist <= radiusSquared)
            firesCount++;
    }

    return firesCount;
}

void CFireManagerSA::ClearExtinguishedFires()
{
    m_Fires.erase(std::remove_if(m_Fires.begin(), m_Fires.end(), [](const std::unique_ptr<CFireSA>& fire) { return !fire->IsActive(); }), m_Fires.end());
}

void CFireManagerSA::Update()
{
    for (auto& fire : m_Fires)
    {
        if (!fire->IsActive())
            continue;

        fire->ProcessFire();
    }

    std::size_t numFires = m_Fires.size();
    m_Visited.assign(numFires, false);

    // Find strongest un-visited fire, and sum of the strength of all fires within 6.0 units of it
    // Based on this strength possibly create a shadow (if combined strength > 4), and coronas (combined strength > 6)
    while (numFires > 0)
    {
        CFire* strongestFire = GetStrongestFire();
        if (!strongestFire)
            break;

        const CVector& strongestFirePos = strongestFire->GetPosition();
        float          totalStrength = 0.0f;
        int            totalCeilStrength = 0;

        for (std::size_t i = 0; i < m_Fires.size(); i++)
        {
            if (m_Visited[i])
                continue;

            const CVector& pos = m_Fires[i]->GetPosition();

            float dx = pos.fX - strongestFirePos.fX;
            float dy = pos.fY - strongestFirePos.fY;
            if (std::abs(dx) > 6.0f || std::abs(dy) > 6.0f)
                continue;

            if (dx * dx + dy * dy < 36.0f)
            {
                float strength = m_Fires[i]->GetStrength();

                totalStrength += strength;
                totalCeilStrength += static_cast<int>(std::ceil(strength));

                m_Visited[i] = true;
                --numFires;
            }
        }

        if (totalStrength > 4.0f && totalCeilStrength > 0)
        {
            float direction = std::min(7.0f, totalStrength - 6.0f + 3.0f);
            auto  colorMult = GetRandomNumberInRange(0.6f, 1.0f);

            // The shadow is buggy, which is why R* set its intensity to 0 here

            // This corona also seems to not achieve what R* intended
            // It is barely visible, positioned above the particle, and practically produces no visible effect
            if (totalStrength > 6.0f)
            {
                int strongestId = reinterpret_cast<int>(strongestFire);

                CMatrix cameraMatrix;
                pGame->GetCamera()->GetMatrix(&cameraMatrix);

                CVector point = strongestFire->GetPosition() + CVector(0.0f, 0.0f, 2.6f);
                CVector diffVec = (cameraMatrix.GetPosition() - point);
                diffVec.Normalize();
                point += diffVec * 3.5f;

                const auto RegisterCorona = [&](auto idx, CVector pos, char flare = 0)
                {
                    ((void(__cdecl*)(int, void*, char, char, char, char, CVector*, float, float, int, char, bool, bool, int, float, bool, float, bool, float,
                                     bool, bool))0x6FC580)(strongestId, nullptr, 40 * colorMult * 0.8f, 32 * colorMult * 0.8f, 20 * colorMult * 0.8f, 255, &pos,
                                                           direction * 0.5f, 70.0f, 1, flare, 0, 0, 0, 0.0f, false, 1.5f, 0, 15.0f, false, false);
                };

                RegisterCorona(strongestId, point, 2);
                point.fZ += 2.0f;

                RegisterCorona(strongestId + 1, point);

                point.fZ -= 2.0f;

                CVector camRightNorm = cameraMatrix.vRight;
                camRightNorm.fZ = 0.0f;
                camRightNorm.Normalize();
                point += camRightNorm * 2.0f;

                RegisterCorona(strongestId + 2, point);

                point -= camRightNorm * 4.0f;
                RegisterCorona(strongestId + 3, point);
            }
        }
    }

    if (pGame->GetSystemFrameCounter() % 30 == 0)
        ClearExtinguishedFires();
}

static void __fastcall StaticStartFireAtCoors(void* gFireManagerSA, void* edx, CVector position, float size, int unused_1, CEntitySAInterface* creator, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed, int unused_2)
{
    CEntity* creatorEntity = creator ? pGame->GetPools()->GetEntity((DWORD*)creator) : nullptr;
    pGame->GetFireManager()->StartFire(position, size, creatorEntity, lifetime, numGenerationsAllowed);
}

static void __fastcall StaticStartFireAtEntity(void* gFireManagerSA, void* edx, CEntitySAInterface* target, CEntitySAInterface* creator, float size_unused, int unused_1, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed)
{
    auto pools = pGame->GetPools();

    CEntity* targetEntity = target ? pools->GetEntity((DWORD*)target) : nullptr;
    CEntity* creatorEntity = creator ? pools->GetEntity((DWORD*)creator) : nullptr;
    pGame->GetFireManager()->StartFire(targetEntity, creatorEntity, lifetime, numGenerationsAllowed);
}

static CFireSAInterface* __fastcall StaticFindNearestFire(void* gFireManagerSA, void* edx, CVector* position, bool checkExtinguished, bool checkScript)
{
    CFire* fire = pGame->GetFireManager()->FindNearestFire(position, checkExtinguished, checkScript);
    return fire != nullptr ? fire->GetInterface() : nullptr;
}

static void __fastcall StaticExtinguishPoint(void* gFireManagerSA, void* edx, CVector position, float radius)
{
    pGame->GetFireManager()->ExtinguishPoint(position, radius);
}

static bool __fastcall StaticExtinguishPointWithWater(void* gFireManagerSA, void* edx, CVector position, float radius, float waterStrength)
{
    return pGame->GetFireManager()->ExtinguishPointWithWater(position, radius, waterStrength);
}

static std::uint32_t __fastcall StaticGetNumFiresInRange(void* gFireManagerSA, void* edx, const CVector& position, float range)
{
    return pGame->GetFireManager()->GetNumFiresInRange(position, range);
}

static void __fastcall StaticUpdate(void* gFireManagerSA)
{
    pGame->GetFireManager()->Update();
}

void CFireManagerSA::StaticSetHooks()
{
    // Patch calls to CFireManager::StartFire (point)
    HookInstallCall(0x53A544, (DWORD)StaticStartFireAtCoors);  // CCreepingFire::TryToStartFireAtCoors
    HookInstallCall(0x56B9AA, (DWORD)StaticStartFireAtCoors);  // CWorld::SetWorldOnFire
    HookInstallCall(0x70765D, (DWORD)StaticStartFireAtCoors);  // CShadows::GunShotSetsOilOnFire
    HookInstallCall(0x70CB87, (DWORD)StaticStartFireAtCoors);  // CShadows::UpdatePermanentShadows
    HookInstallCall(0x70CC7C, (DWORD)StaticStartFireAtCoors);  // CShadows::UpdatePermanentShadows
    HookInstallCall(0x7373DA, (DWORD)StaticStartFireAtCoors);  // CExplosion::AddExplosion

    // Patch calls to CFireManager::StartFire (entity)
    HookInstallCall(0x4621D3, (DWORD)StaticStartFireAtEntity); // CRoadBlocks::CreateRoadBlockBetween2Points
    HookInstallCall(0x4624A9, (DWORD)StaticStartFireAtEntity); // CRoadBlocks::CreateRoadBlockBetween2Points
    HookInstallCall(0x5657DE, (DWORD)StaticStartFireAtEntity);  // CWorld::SetPedsOnFire
    HookInstallCall(0x565B5A, (DWORD)StaticStartFireAtEntity); // CWorld::SetCarsOnFire
    HookInstallCall(0x6B39DF, (DWORD)StaticStartFireAtEntity); // CAutomobile::BlowUpCar
    HookInstallCall(0x6B3E08, (DWORD)StaticStartFireAtEntity);  // CAutomobile::BlowUpCarCutSceneNoExtras
    HookInstallCall(0x6C6FCC, (DWORD)StaticStartFireAtEntity);  // CHeli::BlowUpCar
    HookInstallCall(0x6CD011, (DWORD)StaticStartFireAtEntity);  // CPlane::BlowUpCar
    HookInstallCall(0x7364A5, (DWORD)StaticStartFireAtEntity);  // CBulletInfo::Update
    HookInstallCall(0x73A063, (DWORD)StaticStartFireAtEntity);  // CShotInfo::Update

    // Patch calls to CFireManager::FindNearestFire
    HookInstallCall(0x603F49, (DWORD)StaticFindNearestFire); // CNearbyFireScanner::ScanForNearbyFires
    HookInstallCall(0x6599AE, (DWORD)StaticFindNearestFire);  // sub_659990 called by CTaskComplexExtinguishFireOnFoot::CreateFirstSubTask
    HookInstallCall(0x65B180, (DWORD)StaticFindNearestFire);  // CTaskComplexDriveFireTruck::CreateFirstSubTask
    HookInstallCall(0x65B24A, (DWORD)StaticFindNearestFire);  // CTaskComplexDriveFireTruck::ControlSubTask
    HookInstallCall(0x65B2C3, (DWORD)StaticFindNearestFire);  // CTaskComplexDriveFireTruck::ControlSubTask
    HookInstallCall(0x69761E, (DWORD)StaticFindNearestFire);  // sub_697600 called by CTaskComplexExtinguishFires::CreateNextSubTask

    // Patch calls to CFireManager::ExtinguishPoint
    HookInstallCall(0x56A421, (DWORD)StaticExtinguishPoint); // CWorld::ClearExcitingStuffFromArea
    HookInstallCall(0x56E945, (DWORD)StaticExtinguishPoint);  // CPlayerInfo::MakePlayerSafe

    // Patch calls to CFireManager::ExtinguishPointWithWater
    HookInstallCall(0x72A36B, (DWORD)StaticExtinguishPointWithWater); // CWaterCannon::Update_OncePerFrame
    HookInstallCall(0x73A1E1, (DWORD)StaticExtinguishPointWithWater);  // CShotInfo::Update

    // Patch call to CFireManager::GetNumFiresInRange
    HookInstallCall(0x56B972, (DWORD)StaticGetNumFiresInRange); // CWorld::SetWorldOnFire

    // Patch call to CFireManager::Update
    HookInstallCall(0x53C00A, (DWORD)StaticUpdate); // CGame::Process

    // Ignoring 0x539340 (CFireManager::PlentyFiresAvailable) because it’s inlined and has no xrefs
    // Ignoring all functions/calls related to scripts & replay stuff
}
