/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFireSA.cpp
 *  PURPOSE:     Fire
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntitySA.h"
#include "CFireSA.h"
#include "CFxSystemSA.h"
#include "CGameSA.h"
#include "CPoolsSA.h"
#include <game/CTaskManager.h>
#include <game/TaskTypes.h>
#include "CFxSA.h"
#include "CFireManagerSA.h"
#include "CCreepingFireSA.h"
#include <game/CCamera.h>
#include <game/CPointLights.h>

extern CGameSA* pGame;

////////////////////////////////////////////////////////////////////////
// CFire::Extinguish
//
// Fix GH #3249 (PLAYER_ON_FIRE task is not aborted after the fire is extinguished)
////////////////////////////////////////////////////////////////////////
static void AbortFireTask(CEntitySAInterface* entityOnFire, DWORD returnAddress)
{
    // We can't and shouldn't remove the task if we're in CTaskSimplePlayerOnFire::ProcessPed. Otherwise we will crash.
    if (returnAddress == 0x633783)
        return;

    auto* ped = pGame->GetPools()->GetPed(reinterpret_cast<DWORD*>(entityOnFire));
    if (!ped || !ped->pEntity)
        return;

    CTaskManager* taskManager = ped->pEntity->GetPedIntelligence()->GetTaskManager();
    if (!taskManager)
        return;

    taskManager->RemoveTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM, TASK_SIMPLE_PLAYER_ON_FIRE);
}

#define HOOKPOS_CFire_Extinguish  0x539429
#define HOOKSIZE_CFire_Extinguish 6
static constexpr intptr_t     CONTINUE_CFire_Extinguish = 0x53942F;
static void __declspec(naked) HOOK_CFire_Extinguish()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     [eax+730h], edi

        push    ebx
        mov     ebx, [esp+12]
        push    edi
        push    esi

        push    ebx     // returnAddress
        push    eax     // entityOnFire
        call    AbortFireTask
        add     esp, 8

        pop     esi
        pop     edi
        pop     ebx
        jmp     CONTINUE_CFire_Extinguish
    }
    // clang-format on
}

CFireSA::CFireSA(CFireManagerSA* fireMgr, CEntity* creator, CVector position, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed, bool makeNoise)
    : m_fireManager{fireMgr}, m_creator{creator}
{
    SetSilent(!makeNoise);
    SetActive(true);
    SetLifetime(pGame->GetSystemTime() + static_cast<std::uint32_t>(GetRandomNumberInRange(1.0f, 1.3f) * static_cast<float>(lifetime)));
    SetNumGenerationsAllowed(numGenerationsAllowed);
    SetCreator(creator);
    SetPosition(position);
    SetStrength(1.0f);
}

CFireSA::CFireSA(CFireManagerSA* fireMgr, CEntity* creator, CEntity* target, std::uint32_t lifetime, std::uint8_t numGenerationsAllowed, bool makeNoise)
    : m_fireManager{fireMgr}, m_creator{creator}, m_entityOnFire{target}
{
    SetSilent(!makeNoise);
    SetActive(true);
    SetNumGenerationsAllowed(numGenerationsAllowed);
    SetCreator(creator);
    SetEntityOnFire(target);
    SetPosition(*target->GetPosition());

    std::uint32_t new_lifetime = pGame->GetSystemTime() + GetRandomNumberInRange(0.0f, 1000.0f) + lifetime;

    switch (target->GetEntityType())
    {
        case eEntityType::ENTITY_TYPE_PED:
        {
            auto* pedInterface = dynamic_cast<CPedSA*>(target)->GetPedInterface();
            pedInterface->pFireOnPed = &m_interface;

            if (pedInterface->bPedType < 2)
                new_lifetime = pGame->GetSystemTime() + 2333;
            break;
        }
        case eEntityType::ENTITY_TYPE_VEHICLE:
        {
            dynamic_cast<CVehicleSA*>(target)->GetVehicleInterface()->m_pFire = &m_interface;
            new_lifetime = pGame->GetSystemTime() + GetRandomNumberInRange(0.0f, 1000.0f) + 3000;
            break;
        }
        case eEntityType::ENTITY_TYPE_OBJECT:
        {
            dynamic_cast<CObjectSA*>(target)->GetObjectInterface()->pFire = &m_interface;
            break;
        }
        default:
            break;
    }

    SetLifetime(new_lifetime);
    SetStrength(1.0f);
}

void CFireSA::Extinguish()
{
    if (!IsActive())
        return;

    // If the fire was created by a script, we need to remove CClientFire object
    if (IsCreatedByScript())
    {
        if (m_fireManager->m_destructionHandler)
            m_fireManager->m_destructionHandler(this);
    }

    SetLifetime(0);

    SetActive(false);
    SetBeingExtinguished(false);

    DestroyFxSys();

    if (CEntitySAInterface* entityOnFire = GetEntityOnFire())
    {
        switch (entityOnFire->nType)
        {
            case eEntityType::ENTITY_TYPE_PED:
            {
                reinterpret_cast<CPedSAInterface*>(entityOnFire)->pFireOnPed = nullptr;

                // task fix above TODO
                break;
            }
            case eEntityType::ENTITY_TYPE_VEHICLE:
            {
                reinterpret_cast<CVehicleSAInterface*>(entityOnFire)->m_pFire = nullptr;
                break;
            }
            case eEntityType::ENTITY_TYPE_OBJECT: // bugfix
            {
                reinterpret_cast<CObjectSAInterface*>(entityOnFire)->pFire = nullptr;
                break;
            }
            default:
                break;
        }

        entityOnFire->CleanUpOldReference(&m_interface.m_entityOnFire);
        SetEntityOnFire(nullptr);
    }

    if (CEntitySAInterface* creator = GetCreator())
    {
        creator->CleanUpOldReference(&m_interface.m_creator);
        SetCreator(nullptr);
    }
}

void CFireSA::ExtinguishWithWater(float waterStrength)
{
    float currentStrength = GetStrength();
    float newStrength = currentStrength - waterStrength * pGame->GetTimeStepInSeconds();
    SetStrength(newStrength, false);

    const CVector& firePos = GetPosition();
    CVector particlePos = firePos + CVector(GetRandomNumberInRange(-1.28f, 1.28f), GetRandomNumberInRange(-1.28f, 1.28f), GetRandomNumberInRange(-0.64f, 0.64f));
    FxPrtMult_c fxPrt{{1.0f, 1.0f, 1.0f, 0.6f}, 0.75f, 0.0f, 0.4f};

    static FxSystem_c* smokeII3expand = pGame->GetFx()->GetInterface()->m_fxSysSmoke2;

    const auto AddParticle = [&](CVector velocity)
    {
        // Call FxSystem_c::AddParticle
        ((int(__thiscall*)(FxSystem_c*, const CVector*, const CVector*, float, FxPrtMult_c*, float, float, float, int))0x4AA440)(
            smokeII3expand, &particlePos, &velocity, 0, &fxPrt, -1.0f, 1.2f, 0.6f, 0);
    };

    AddParticle(CVector(0.0f, 0.0f, 0.8f));
    AddParticle(CVector(0.0f, 0.0f, 1.4f));

    SetBeingExtinguished(true);

    if (newStrength >= 0.0f)
    {
        if (static_cast<std::intmax_t>(currentStrength) != static_cast<std::intmax_t>(newStrength))
            CreateFxSysForStrength(firePos, nullptr);
    }
    else
        Extinguish();
}

void CFireSA::SetPosition(const CVector& position, bool updateParticle)
{
    m_interface.m_position = position;

    if (m_interface.m_fxSystem && updateParticle)
        m_interface.m_fxSystem->SetOffsetPos(position);
}

void CFireSA::SetStrength(float strength, bool updateFX)
{
    m_interface.m_strength = strength;

    if (updateFX)
        CreateFxSysForStrength(GetPosition(), nullptr);
}

void CFireSA::SetCreator(CEntity* entity)
{
    CEntitySAInterface* entityInterface = entity ? entity->GetInterface() : nullptr;
    if (entityInterface)
        entityInterface->CleanUpOldReference(&m_interface.m_creator);

    m_interface.m_creator = entityInterface;
    m_creator = entity;

    if (entityInterface)
        entityInterface->RegisterReference(&m_interface.m_creator);
}

void CFireSA::SetEntityOnFire(CEntity* entity)
{
    CEntitySAInterface* entityInterface = entity ? entity->GetInterface() : nullptr;
    if (entityInterface)
        entityInterface->CleanUpOldReference(&m_interface.m_entityOnFire);

    m_interface.m_entityOnFire = entityInterface;
    m_entityOnFire = entity;

    if (entityInterface)
        entityInterface->RegisterReference(&m_interface.m_entityOnFire);
}

void CFireSA::ProcessFire()
{
    float currentStrength = GetStrength();
    float newStrength = std::min(3.0f, currentStrength + pGame->GetTimeStep() / 500.0f);
    SetStrength(newStrength, false);

    if (static_cast<int>(currentStrength) != static_cast<int>(newStrength))
        SetStrength(currentStrength, false);

    if (m_entityOnFire)
    {
        SetPosition(*m_entityOnFire->GetPosition());

        switch (m_entityOnFire->GetEntityType())
        {
            case eEntityType::ENTITY_TYPE_PED:
            {
                auto* pedInterface = reinterpret_cast<CPedSAInterface*>(m_interface.m_entityOnFire);
                if (pedInterface->pFireOnPed != &m_interface)
                {
                    Extinguish();
                    return;
                }

                if (pedInterface->pedState == 0x36 || pedInterface->pedState == 0x37)
                    m_interface.m_position.fZ -= 1.0f;

                if (pedInterface->pedFlags.bInVehicle && pedInterface->pVehicle)
                {
                    if (pedInterface->pVehicle->m_nModelIndex != 407 && pedInterface->pVehicle->m_vehicleClass == VehicleClass::AUTOMOBILE)
                        pedInterface->pVehicle->m_nHealth = 75.0f;
                }
                else if (pedInterface->bPedType > 0 && (pedInterface->pedState != 0x36 && pedInterface->pedState != 0x37))
                    reinterpret_cast<CPhysicalSAInterface*>(pedInterface)->bDestroyed = true;

                break;
            }
            case eEntityType::ENTITY_TYPE_VEHICLE:
            {
                auto* vehicleInterface = reinterpret_cast<CVehicleSAInterface*>(m_interface.m_entityOnFire);
                if (vehicleInterface->m_pFire != &m_interface)
                {
                    Extinguish();
                    return;
                }

                // CVehicle::InflictDamage
                ((void(__thiscall*)(CVehicleSAInterface*, CEntitySAInterface*, int, float, CVector))0x6D7C90)(
                    vehicleInterface, GetCreator(), WEAPONTYPE_FLAMETHROWER, pGame->GetTimeStep() * 1.2f, CVector{});

                if (vehicleInterface->m_vehicleClass == VehicleClass::AUTOMOBILE)
                {
                    CVector fireOnVehPos = 
                        reinterpret_cast<CVehicleModelInfoSAInterface*>(pGame->GetModelInfo(vehicleInterface->m_nModelIndex)->GetInterface())
                            ->pVisualInfo->vecDummies[0] + CVector(0, 0, 0.15f);

                    CMatrix vehMatrix;
                    dynamic_cast<CVehicle*>(m_entityOnFire)->GetMatrix(&vehMatrix);

                    SetPosition(vehMatrix.TransformVector(fireOnVehPos));
                }

                break;
            }
        }

        if (m_interface.m_fxSystem)
        {
            float   doubleStep = pGame->GetTimeStep() * 2.0f;
            CVector vec = GetPosition() + reinterpret_cast<CPhysicalSAInterface*>(m_interface.m_entityOnFire)->m_vecLinearVelocity * doubleStep;
            m_interface.m_fxSystem->SetOffsetPos(vec);
        }
    }

    CVector& firePosition = GetPosition();

    if (!m_entityOnFire || m_entityOnFire->GetEntityType() != eEntityType::ENTITY_TYPE_VEHICLE)
    {
        CPed* player = pGame->GetPedContext();
        if (!player->GetVehicle() && !player->IsOnFire() && !player->IsFireProof() && !player->GetPedInterface()->m_pAttachedEntity)
        {
            if ((*player->GetPosition() - firePosition).LengthSquared() < 1.2f)
            {
                // CPlayerPed::DoStuffToGoOnFire
                ((void(__thiscall*)(CPedSAInterface*))0x60A020)(player->GetPedInterface());
                m_fireManager->StartFire(player, m_creator, 7000, 100, !IsSilent());
            }
        }
    }

    if (rand() % 32 == 0)
    {
        SPoolData<CVehicleSA, CVehicleSAInterface, MAX_VEHICLES> vehiclesPool;
        pGame->GetPools()->GetVehiclesPool(&vehiclesPool);

        for (uint i = 0; i < vehiclesPool.ulCount; i++)
        {
            CEntitySA* pEntitySA = vehiclesPool.arrayOfClientEntities[i].pEntity;
            if (!pEntitySA)
                continue;

            if ((*pEntitySA->GetPosition() - firePosition).LengthSquared() >= 4.0f)
                continue;

            CVehicleSA* vehicle = dynamic_cast<CVehicleSA*>(pEntitySA);
            CVehicleSAInterface* vehicleInterface = vehicle->GetVehicleInterface();

            if (static_cast<VehicleClass>(vehicleInterface->m_vehicleSubClass) == VehicleClass::BMX)
            {
                CPed* player = pGame->GetPedContext();
                // CPlayerPed::DoStuffToGoOnFire
                ((void(__thiscall*)(CPedSAInterface*))0x60A020)(player->GetPedInterface());
                m_fireManager->StartFire(player, m_creator, 7000, 100, !IsSilent());

                // CVehicle::FindTyreNearestPoint
                int tyre = ((int(__thiscall*)(CVehicleSAInterface*, CVector2D))0x6D7BC0)(vehicleInterface,
                                                                                         CVector2D(firePosition.fX, firePosition.fY));
                vehicleInterface->BurstTyre(tyre + 13, false);
            }
            else
                m_fireManager->StartFire(pEntitySA, m_creator, 7000, 100, !IsSilent());
        }
    }

    if (rand() % 4 == 0)
    {
        SPoolData<CObjectSA, CObjectSAInterface, MAX_OBJECTS> objectsPool;
        pGame->GetPools()->GetObjectsPool(&objectsPool);

        for (uint i = 0; i < objectsPool.ulCount; i++)
        {
            CEntitySA* pEntitySA = objectsPool.arrayOfClientEntities[i].pEntity;
            if (!pEntitySA)
                continue;

            if ((*pEntitySA->GetPosition() - firePosition).LengthSquared() >= 9.0f)
                continue;

            // CObject::ObjectFireDamage
            ((void(__thiscall*)(CObjectSAInterface*, float, CEntitySAInterface*))0x5A1580)(dynamic_cast<CObjectSA*>(pEntitySA)->GetObjectInterface(),
                                                                                           pGame->GetTimeStep() * 8.0f, m_interface.m_creator);
        }
    }

    std::uint8_t numGens = GetNumGenerationsAllowed();

    if (numGens > 0 && rand() % 128 == 0 && m_fireManager->GetNumFires() < 25)
    {
        CVector direction(GetRandomNumberInRange(-1.0f, 1.0f), GetRandomNumberInRange(-1.0f, 1.0f), 0.0f);
        direction.Normalize();

        CVector newPos = firePosition + direction * GetRandomNumberInRange(2.0f, 2.1f); // default: 2-2.003
        newPos.fZ = firePosition.fZ + 2.0f;

        CCreepingFireSA::TryToStartFireAtCoors(newPos, numGens - 1, 10.0f);
    }

    if (GetStrength() <= 2.0f && numGens > 0 && rand() % 16 == 0)
    {
        // We don’t have a fire limit, but we want to preserve the original chance of fire spreading
        // as if there were only 60 slots
        float probabilityScale = std::min(static_cast<float>(m_fireManager->GetNumFires()) / 60.0f, 1.0f);

        if ((rand() / static_cast<float>(RAND_MAX)) < probabilityScale)
        {
            CFireSA* randomFire = m_fireManager->GetRandomFire();
            if (randomFire != this && randomFire->IsActive() && randomFire->GetStrength() <= 1.0f)
            {
                const CVector& randomFirePos = randomFire->GetPosition();
                if ((firePosition - randomFirePos).LengthSquared() < 12.25f)
                {
                    randomFire->SetPosition(randomFirePos * 0.3f + firePosition * 0.7f);
                    SetStrength(GetStrength() + 1.0f);

                    SetLifetime(std::max(GetLifetime(), static_cast<std::uint32_t>(pGame->GetSystemTime() + 7000)));
                    SetNumGenerationsAllowed(std::min(numGens, randomFire->GetNumGenerationsAllowed()));

                    randomFire->Extinguish();
                }
            }
        }
    }

    if (m_interface.m_fxSystem)
    {
        float unused_field;
        float fractionalPart = std::modf(GetStrength(), &unused_field);
        m_interface.m_fxSystem->SetConstTime(true, std::min(std::max(0.0f, (GetLifetime() - static_cast<float>(pGame->GetSystemTime())) / 3500.0f), fractionalPart));
    }

    CMatrix camMatrix;
    pGame->GetCamera()->GetMatrix(&camMatrix);

    if (GetLifetime() > pGame->GetSystemTime() && (camMatrix.GetPosition() - firePosition).Length2D() < m_interface.m_removalDistance)
    {
        // CPointLights::AddLight
        float color = (float)(rand() % 128) / 512.0f;
        ((void(__cdecl*)(char, const CVector&, const CVector&, float, float, float, float, char, bool, CEntitySAInterface*))0x7000E0)(
            0, firePosition, CVector(), 8.0f, color, color, 0.0f, 0, false, nullptr);
    }
    else
    {
        if (GetStrength() <= 1.0f)
            Extinguish();
        else
        {
            SetStrength(GetStrength() - 1.0f);
            SetLifetime(pGame->GetSystemTime() + 7000);
        }
    }
}

void CFireSA::DestroyFxSys()
{
    if (!m_interface.m_fxSystem)
        return;

    m_interface.m_fxSystem->Kill();
    m_interface.m_fxSystem = nullptr;
}

void CFireSA::CreateFxSysForStrength(const CVector& position, RwMatrix* matrix)
{
    DestroyFxSys();

    const char* particleName = "fire";
    float       strength = GetStrength();

    if (strength > 1.0f)
        particleName = strength > 2.0f ? "fire_large" : "fire_med";

    auto* fx = pGame->GetFxManagerSA()->CreateFxSystem(particleName, position, matrix, true, !IsSilent());
    if (!fx)
        return;

    if (auto* fxSystemInterface = fx->GetInterface())
        m_interface.m_fxSystem = static_cast<CFxSystemSAInterface*>(fxSystemInterface);

    if (m_interface.m_fxSystem)
        m_interface.m_fxSystem->Play();
}

static void __fastcall StaticExtinguish(CFireSAInterface* fireInterface)
{
    CFireSA* fire = CFireManagerSA::GetFire(fireInterface);
    if (!fire)
        return;

    fire->Extinguish();
}

void CFireSA::StaticSetHooks()
{
    // Patch calls to CFire::Extinguish
    HookInstallCall(0x442157, (DWORD)StaticExtinguish); // CGameLogic::RestorePlayerStuffDuringResurrection
    HookInstallCall(0x59F81B, (DWORD)StaticExtinguish); // CObject::~CObject
    HookInstallCall(0x5E8714, (DWORD)StaticExtinguish);  // CPed::~CPed
    HookInstallCall(0x60CD7F, (DWORD)StaticExtinguish);  // CPlayerPed::SetInitialState
    HookInstallCall(0x63377E, (DWORD)StaticExtinguish);  // CTaskSimplePlayerOnFire::ProcessPed
    HookInstallCall(0x639224, (DWORD)StaticExtinguish);  // CTaskComplexOnFire::ControlSubTask
    HookInstallCall(0x698261, (DWORD)StaticExtinguish);  // CTaskComplexExtinguishFires::CreateNextSubTask
    HookInstallCall(0x6D24A6, (DWORD)StaticExtinguish);  // CVehicle::ExtinguishCarFire
    HookInstallCall(0x6E2C0B, (DWORD)StaticExtinguish);  // CVehicle::~CVehicle

    // EZHookInstall(CFire_Extinguish);
}
