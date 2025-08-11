/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileInfoSA.cpp
 *  PURPOSE:     Projectile type information
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CProjectileInfoSA.h"
#include "CVehicleSA.h"
#include "CWorldSA.h"
#include "enums/ExplosionType.h"
#include "CMatrixSA.h"
#include "CMatrixLinkSA.h"
#include "game/CCamera.h"
#include "CCamSA.h"
#include "CColModelSA.h"

#include "CExplosionManagerSA.h"
#include "CFxSA.h"

extern CGameSA* pGame;

ProjectileHandler              CProjectileInfoSA::m_projectileCreationHandler;
GameProjectileDestructHandler*  CProjectileInfoSA::m_projectileDestructionHandler;

std::array<CProjectileInfoSA, MAX_PROJECTILES_COUNT> CProjectileInfoSA::ms_projectileInfo{};

void CProjectileInfoSA::Initialise()
{
    for (auto& info : ms_projectileInfo)
    {
        info.m_weaponType = eWeaponType::WEAPONTYPE_GRENADE;
        info.m_creator = nullptr;
        info.m_target = nullptr;
        info.m_particle = nullptr;
        info.m_projectile = nullptr;
        info.m_counter = 0;
    }
}

void CProjectileInfoSA::Shutdown()
{
    for (auto& info : ms_projectileInfo)
    {
        if (!info.m_particle)
            continue;

        RemoveParticle(&info, true);
    }
}

CObjectSAInterface* CProjectileInfoSA::CreateProjectileObject(std::uint32_t modelIndex, CProjectileInfoSA* info)
{
    auto* mem = ((CObjectSAInterface * (__cdecl*)(int))0x5A1EE0)(sizeof(CObjectSAInterface));
    auto* object = ((CObjectSAInterface * (__thiscall*)(CObjectSAInterface*, std::uint32_t))0x5A4030)(mem, modelIndex);

    info->m_projectile = new CProjectileSA(object, info);
    object->bStreamingDontDelete = true;
    object->bDontStream = true;
    object->bRemoveFromWorld = false;

    return object;
}

void CProjectileInfoSA::DestroyProjectileObject(CObjectSAInterface* object)
{
    if (!object)
        return;

    if (m_projectileDestructionHandler)
        m_projectileDestructionHandler(object);

    for (auto& info : ms_projectileInfo)
    {
        if (!info.m_projectile)
            continue;

        if (info.m_projectile->m_object == object)
        {
            delete info.m_projectile;
            info.m_projectile = nullptr;
            break;
        }
    }

    // CRadar::ClearBlipForEntity
    ((void(__cdecl*)(int, int))0x587C60)(3, pGame->GetPools()->GetObjectHandle(object));

    pGame->GetWorld()->Remove(object, eDebugCaller::CObject_Destructor);

    // CProjectile destructor
    ((void(__thiscall*)(CObjectSAInterface*, bool))0x5A40F0)(object, true);
}

void CProjectileInfoSA::RemoveNotAdd(CEntitySAInterface* entity, eWeaponType weaponType, CVector pos)
{
    ExplosionType::Enum explosionType{};

    switch (weaponType)
    {
        case WEAPONTYPE_GRENADE:
        case WEAPONTYPE_REMOTE_SATCHEL_CHARGE:
            explosionType = ExplosionType::Enum::EXPLOSION_GRENADE;
            break;
        case WEAPONTYPE_MOLOTOV:
            explosionType = ExplosionType::Enum::EXPLOSION_MOLOTOV;
            break;
        case WEAPONTYPE_ROCKET:
        case WEAPONTYPE_ROCKET_HS:
            explosionType = ExplosionType::Enum::EXPLOSION_ROCKET;
            break;
        default:
            explosionType = ExplosionType::Enum::EXPLOSION_SMALL;
            break;
    }

    // Call CExplosion::AddExplosion
    ((void(__cdecl*)(CEntitySAInterface*, CEntitySAInterface*, ExplosionType::Enum, CVector, bool, float, bool))0x736A50)(nullptr, entity, explosionType, pos, true, -1.0f, false);
}

void CProjectileInfoSA::RemoveDetonatorProjectiles()
{
    for (std::size_t i = 0; i < MAX_PROJECTILES_COUNT; i++)
    {
        CProjectileInfoSA& info = ms_projectileInfo[i];

        if (!info.m_isActive || info.m_weaponType != eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
            continue;

        CObjectSAInterface* object = info.m_projectile->m_object;
        CVector             pos = object->matrix ? object->matrix->vPos : object->m_transform.m_translate;

        object->bRemoveFromWorld = true;

        // Call CExplosion::AddExplosion
        ((void(__cdecl*)(CEntitySAInterface*, CEntitySAInterface*, ExplosionType::Enum, CVector, bool, float, bool))0x736A50)(nullptr, info.m_creator, ExplosionType::Enum::EXPLOSION_GRENADE, pos, true, -1.0f, false);

        info.m_isActive = false;

        if (info.m_particle)
        {
            // Call FxManager_c::DestroyFxSystem
            ((void(__thiscall*)(void*, void*))0x4A9810)((void*)0xA9AE80, info.m_particle);
            info.m_particle = nullptr;
        }
    }
}

void CProjectileInfoSA::RemoveIfThisIsAProjectile(CObjectSAInterface* object)
{
    for (auto& info : ms_projectileInfo)
    {
        if (!info.m_projectile || !info.m_projectile->m_object)
            continue;

        RemoveParticle(&info, false);
        DestroyProjectileObject(info.m_projectile->m_object);

        delete info.m_projectile;
        info.m_projectile = nullptr;
    }
}

void CProjectileInfoSA::RemoveAllProjectiles()
{
    for (auto& info : ms_projectileInfo)
    {
        if (!info.m_isActive)
            continue;

        if (!info.m_projectile || !info.m_projectile->m_object)
            continue;

        RemoveParticle(&info, true);
        DestroyProjectileObject(info.m_projectile->m_object);

        info.m_isActive = false;

        delete info.m_projectile;
        info.m_projectile = nullptr;
    }
}

void CProjectileInfoSA::StaticRemoveProjectile(CProjectileInfoSA* info, CProjectileSA* projectile, bool blow)
{
    CObjectSAInterface* object = projectile->m_object;
    if (!object)
        return;

    if (!blow)
    {
        RemoveIfThisIsAProjectile(object);
        return;
    }

    CVector objectPosition = object->matrix ? object->matrix->vPos : object->m_transform.m_translate;

    switch (info->m_weaponType)
    {
        case eWeaponType::WEAPONTYPE_MOLOTOV:
        {
            // Call CExplosion::AddExplosion
            ((void(__cdecl*)(CEntitySAInterface*, CEntitySAInterface*, int, CVector, bool, float, bool))0x736A50)(nullptr, info->m_creator, 1, objectPosition, true, -1.0f, false);

            // Call CAudioEngine::ReportObjectDestruction
            ((void(__thiscall*)(CAudioEngineSAInterface*, CObjectSAInterface*))0x506ED0)((CAudioEngineSAInterface*)0xB6BC90, object);
            break;
        }
        case eWeaponType::WEAPONTYPE_ROCKET:
        {
            if (info->m_creator->nType == ENTITY_TYPE_VEHICLE)
            {
                // Call CExplosion::AddExplosion
                ((void(__cdecl*)(CEntitySAInterface*, CEntitySAInterface*, int, CVector, bool, float, bool))0x736A50)(nullptr, static_cast<CVehicleSAInterface*>(info->m_creator)->pDriver, 2, objectPosition, true, -1.0f, false);
            }
            else
            {
                // Call CExplosion::AddExplosion
                ((void(__cdecl*)(CEntitySAInterface*, CEntitySAInterface*, int, CVector, bool, float, bool))0x736A50)(nullptr, info->m_creator, 2, objectPosition, true, -1.0f, false);
            }
            break;
        }
        case eWeaponType::WEAPONTYPE_ROCKET_HS:
        {
            CPedSAInterface* localPlayer = pGame->GetPedContext()->GetPedInterface();
            // Call CExplosion::AddExplosion
            ((void(__cdecl*)(CEntitySAInterface*, CEntitySAInterface*, int, CVector, bool, float, bool))0x736A50)(nullptr, info->m_creator, info->m_creator == localPlayer ? 2 : 3, objectPosition, true, -1.0f, false);

            break;
        }
        case eWeaponType::WEAPONTYPE_FREEFALL_BOMB:
        case eWeaponType::WEAPONTYPE_GRENADE:
        {
            // Call CExplosion::AddExplosion
            ((void(__cdecl*)(CEntitySAInterface*, CEntitySAInterface*, int, CVector, bool, float, bool))0x736A50)(nullptr, info->m_creator, 0, objectPosition, true, -1.0f, false);
            break;
        }
        default:
            break;
    }

    RemoveParticle(info, false);
    DestroyProjectileObject(object);

    delete projectile;
    info->m_isActive = false;
}

void CProjectileInfoSA::RemoveParticle(CProjectileInfoSA* info, bool instantly)
{
    if (!info->m_particle)
        return;

    if (instantly)
        // Call FxManager_c::DestroyFxSystem
        ((void(__thiscall*)(void*, void*))0x4A9810)((void*)0xA9AE80, info->m_particle);
    else
        // Call FxSystem_c::Kill
        ((void(__thiscall*)(void*))0x4AA3F0)(info->m_particle);

    info->m_particle = nullptr;
}

bool CProjectileInfoSA::IsProjectileInRange(float x1, float x2, float y1, float y2, float z1, float z2, bool destroy)
{
    bool isInRange = false;

    for (std::size_t i = 0; i < MAX_PROJECTILES_COUNT; i++)
    {
        if (!ms_projectileInfo[i].m_isActive)
            continue;

        auto projectileType = ms_projectileInfo[i].m_weaponType;
        if (projectileType == eWeaponType::WEAPONTYPE_ROCKET || projectileType == eWeaponType::WEAPONTYPE_ROCKET_HS || projectileType == eWeaponType::WEAPONTYPE_MOLOTOV || projectileType == eWeaponType::WEAPONTYPE_TEARGAS || projectileType == eWeaponType::WEAPONTYPE_GRENADE)
        {
            CVector pos = ms_projectileInfo[i].m_projectile->m_object->matrix ? ms_projectileInfo[i].m_projectile->m_object->matrix->vPos : ms_projectileInfo[i].m_projectile->m_object->m_transform.m_translate;

            if ((pos.fX >= x1 && pos.fX <= x2) && (pos.fY >= y1 && pos.fY <= y2) && (pos.fZ >= z1 && pos.fZ <= z2))
            {
                isInRange = true;

                if (destroy)
                {
                    RemoveParticle(&ms_projectileInfo[i], false);
                    DestroyProjectileObject(ms_projectileInfo[i].m_projectile->m_object);
                }

                break;
            }
        }
    }

    return isInRange;
}

void CProjectileInfoSA::Update()
{
    for (std::size_t i = 0; i < MAX_PROJECTILES_COUNT; i++)
    {
        if (!ms_projectileInfo[i].m_isActive)
            continue;

        CProjectileInfoSA*      projectileInfo = &ms_projectileInfo[i];
        CProjectileSA*          projectileInstance = projectileInfo->m_projectile;
        if (!projectileInstance)
            continue;

        CObjectSAInterface* projectile = projectileInstance->m_object;
        if (!projectile)
            continue;

        if (projectile->bSubmergedInWater && projectileInfo->m_particle)
            RemoveParticle(projectileInfo, false);

        // Call CPed::IsPointerValid
        if (projectileInfo->m_creator && projectileInfo->m_creator->nType == ENTITY_TYPE_PED && !((bool(__thiscall*)(CPedSAInterface*))0x5E4220)((CPedSAInterface*)projectileInfo->m_creator))
            projectileInfo->m_creator = nullptr;

        if (projectileInfo->m_weaponType == eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE || projectileInfo->m_weaponType == eWeaponType::WEAPONTYPE_GRENADE || projectileInfo->m_weaponType == eWeaponType::WEAPONTYPE_TEARGAS)
        {
            if (projectile->m_fElasticity > 0.1f && (std::fabs(projectile->m_vecLinearVelocity.fX) < 0.05f && std::fabs(projectile->m_vecLinearVelocity.fY) < 0.05f && std::fabs(projectile->m_vecLinearVelocity.fZ) < 0.05f))
                projectile->m_fElasticity = 0.03f;

            if (projectileInfo->m_weaponType == eWeaponType::WEAPONTYPE_TEARGAS && pGame->GetSystemTime() > (projectileInfo->m_counter - 17500) && GetRandomNumberInRange(0, 100) < 10)
            {
                // Call CWorld::SetPedsChoking
                const CVector& pos = projectile->matrix ? projectile->matrix->vPos : projectile->m_transform.m_translate;
                ((void(__cdecl*)(CVector, float, CPedSAInterface*))0x565800)(pos, 6.0f, (CPedSAInterface*)projectileInfo->m_creator);
            }
        }

        if (projectileInfo->m_weaponType == eWeaponType::WEAPONTYPE_ROCKET || projectileInfo->m_weaponType == eWeaponType::WEAPONTYPE_ROCKET_HS)
        {
            FxPrtMult_c fxPrt{{0.3f, 0.3f, 0.3f, 0.3f}, 0.5f, 1.0f, 0.08f};
            const CVector& pos = projectile->matrix ? projectile->matrix->vPos : projectile->m_transform.m_translate;

            CVector velocity = projectile->m_vecLinearVelocity * pGame->GetTimeStep();
            float   len = velocity.Length();
            int     particleCount = 1;
            if (len >= 1.0f)
                particleCount = static_cast<int>(len);

            for (std::size_t i = 0; i < particleCount; i++)
            {
                fxPrt.m_color.red = rand() * 0.000030518509f * 0.25f + 0.25f;
                fxPrt.m_color.green = fxPrt.m_color.red;
                fxPrt.m_color.blue = fxPrt.m_color.red;

                fxPrt.m_fLife = rand() * 0.000030518509f * 0.04f + 0.08f;

                float t = 1.0f - (static_cast<float>(i) / static_cast<float>(particleCount));

                CVector prtPos = pos - velocity * t;

                int     rand1 = rand();
                int     rand2 = rand();
                int     rand3 = rand();
                CVector prtVel = CVector(rand1 * 0.000030518509f + rand1 * 0.000030518509f - 1.0f, rand2 * 0.000030518509f + rand2 * 0.000030518509f - 1.0f, rand3 * 0.000030518509f + rand3 * 0.000030518509f - 1.0f);
                prtVel.Normalize();

                CVector objectVelocity = projectile->m_vecLinearVelocity;
                objectVelocity.Normalize();

                objectVelocity.CrossProduct(&prtVel);
                objectVelocity *= 1.5f;

                // Call FxSystem_c::AddParticle
                ((int(__thiscall*)(FxSystem_c*, const CVector*, const CVector*, float, FxPrtMult_c*, float, float, float, int))FUNC_FXSystem_c_AddParticle)(pGame->GetFx()->GetInterface()->m_fxSysSmokeHuge, &prtPos, &objectVelocity, 0, &fxPrt, -1.0f, 1.2f, 0.6f, 0);
            }
        }

        if (projectileInfo->m_counter > 0 && pGame->GetSystemTime() > projectileInfo->m_counter)
        {
            if (projectileInfo->m_weaponType == eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
            {
                if (projectileInfo->m_creator->nType == ENTITY_TYPE_PED && static_cast<CPedSAInterface*>(projectileInfo->m_creator)->bPedType < 2)
                {
                    CPedSAInterface* ped = static_cast<CPedSAInterface*>(projectileInfo->m_creator);
                    if (ped->Weapons[12].m_eWeaponType != eWeaponType::WEAPONTYPE_DETONATOR || ped->Weapons[12].m_ammoTotal == 0)
                        projectileInfo->m_counter = 0;
                }

                UpdateLastPos(projectileInfo);
                continue;
            }

            UpdateLastPos(projectileInfo);
            StaticRemoveProjectile(projectileInfo, projectileInfo->m_projectile);
            continue;
        }

        if (projectileInfo->m_weaponType == eWeaponType::WEAPONTYPE_ROCKET)
        {
            float time = pGame->GetTimeStep() * 0.008f;
            projectile->m_vecLinearVelocity += (projectile->matrix ? projectile->matrix->vFront : CVector(0, 0 ,0)) * time;

            float length = projectile->m_vecLinearVelocity.Length();
            if (length > 9.9)
            {
                projectile->m_vecLinearVelocity.Normalize();
                projectile->m_vecLinearVelocity *= 9.9f;
            }

            if (CheckIsLineOfSightClear(projectileInfo))
                continue;

            UpdateLastPos(projectileInfo);
            StaticRemoveProjectile(projectileInfo, projectileInfo->m_projectile);
            continue;
        }

        switch (projectileInfo->m_weaponType)
        {
            case eWeaponType::WEAPONTYPE_FLARE:
            {
                CVector pos = projectile->matrix ? projectile->matrix->vPos : projectile->m_transform.m_translate;

                *(void**)0xB7CD68 = projectileInfo->m_creator;
                projectile->bUsesCollision = false;

                // Call CWorld::GetIsLineOfSightClear
                bool clear = ((bool(__cdecl*)(CVector*, CVector*, bool, bool, bool, bool, bool, bool, bool))0x56A490)(&projectileInfo->m_lastPos, &pos, true, true, true, true, false, false, false);

                projectile->bUsesCollision = true;
                *(void**)0xB7CD68 = nullptr;

                if (!clear)
                {
                    projectile->m_vecLinearVelocity = CVector(0, 0, 0);

                    if (projectile->matrix)
                        projectile->matrix->vPos = projectileInfo->m_lastPos;
                    else
                        projectile->m_transform.m_translate = projectileInfo->m_lastPos;
                }

                projectileInfo->m_lastPos = pos;
                continue;
            }
            case eWeaponType::WEAPONTYPE_FREEFALL_BOMB:
            case eWeaponType::WEAPONTYPE_MOLOTOV:
            {
                CVector pos = projectile->matrix ? projectile->matrix->vPos : projectile->m_transform.m_translate;

                *(void**)0xB7CD68 = projectileInfo->m_creator;
                projectile->bUsesCollision = false;

                // Call CWorld::GetIsLineOfSightClear
                bool clear = ((bool(__cdecl*)(CVector*, CVector*, bool, bool, bool, bool, bool, bool, bool))0x56A490)(&projectileInfo->m_lastPos, &pos, true, true, true, true, false, false, false);

                CVector diff = projectileInfo->m_lastPos - pos;
                if ((!projectileInfo->m_creator || diff.LengthSquared() >= 2.0f) && (projectile->bOnSolidSurface || !clear))
                    StaticRemoveProjectile(projectileInfo, projectileInfo->m_projectile);

                if (projectile)
                    projectile->bUsesCollision = true;

                *(void**)0xB7CD68 = nullptr;
                break;
            }
            case eWeaponType::WEAPONTYPE_ROCKET_HS:
            {
                if (projectileInfo->m_target)
                {
                    CPedSAInterface* localPlayer = pGame->GetPedContext()->GetPedInterface();
                    if (projectileInfo->m_target == localPlayer->pVehicle)
                        // CAudioEngine::ReportFrontendAudioEvent
                        ((void(__thiscall*)(CAudioEngineSAInterface*, int, float, float))0x506EA0)((CAudioEngineSAInterface*)0xB6BC90, 101, 0, 1.0f);

                    const CVector& pos = projectile->matrix ? projectile->matrix->vPos : projectile->m_transform.m_translate;
                    const CVector& forward = projectile->matrix ? projectile->matrix->vFront : CVector(0,0,0);

                    CVector startPoint = pos + forward;
                    // CWeapon::EvaluateTargetForHeatSeekingMissile
                    double evaluate1 = ((double(__cdecl*)(CEntitySAInterface*, CVector*, const CVector*, float, bool, CEntitySAInterface*))0x73E560)(projectileInfo->m_target, &startPoint, &pos, 1.2f, true, nullptr);
                    double evaluate2 = 0.0;

                    CPhysicalSAInterface* targetProjectile = nullptr;

                    for (std::size_t j = 0; j < MAX_PROJECTILES_COUNT; j++)
                    {
                        if (ms_projectileInfo[j].m_isActive && ms_projectileInfo[j].m_weaponType == eWeaponType::WEAPONTYPE_FLARE && ms_projectileInfo[j].m_projectile && ms_projectileInfo[j].m_projectile->m_object)
                        {
                            // CWeapon::EvaluateTargetForHeatSeekingMissile
                            double ev = ((double(__cdecl*)(CEntitySAInterface*, CVector*, const CVector*, float, bool, CEntitySAInterface*))0x73E560)(ms_projectileInfo[j].m_projectile->m_object, &startPoint, &pos, 1.2f, true, nullptr);
                            if (ev > evaluate2)
                            {
                                evaluate2 = ev;
                                targetProjectile = ms_projectileInfo[j].m_projectile->m_object;
                            }
                        }
                    }

                    if (!targetProjectile || evaluate2 <= evaluate1)
                        targetProjectile = (CPhysicalSAInterface*)projectileInfo->m_target;

                    bool isPlaneTargetFromLocalPlayer = false;

                    if (targetProjectile->nType == ENTITY_TYPE_VEHICLE)
                    {
                        if ((projectileInfo->m_creator == localPlayer || projectileInfo->m_creator == localPlayer->pVehicle) && static_cast<CVehicleSAInterface*>(targetProjectile)->m_vehicleSubClass == (uint32_t)VehicleClass::PLANE)
                            isPlaneTargetFromLocalPlayer = true;
                    }

                    CVector startPos = projectile->m_vecLinearVelocity * 100.0f + pos;

                    if (isPlaneTargetFromLocalPlayer)
                        startPos = pos;

                    CVector targetProjectilePos = targetProjectile->matrix ? targetProjectile->matrix->vPos : targetProjectile->m_transform.m_translate;

                    CVector diff = pos - targetProjectilePos;
                    float   vecLen = diff.Length();
                    float   maxDist = isPlaneTargetFromLocalPlayer ? std::min(vecLen, 1.5f) : std::min(vecLen, 50.0f);

                    CVector targetVelocityCompensation = targetProjectile->m_vecLinearVelocity * maxDist;
                    CVector projectileDir = projectile->m_vecLinearVelocity;
                    projectileDir.Normalize();

                    CVector aimAdjustment  = CVector((targetVelocityCompensation .fX + targetProjectilePos.fX) - startPos.fX, (targetVelocityCompensation .fY + targetProjectilePos.fY) - startPos.fY, (targetVelocityCompensation .fZ + targetProjectilePos.fZ) - startPos.fZ);

                    float dotToAimDir = aimAdjustment.fX * projectileDir.fX + aimAdjustment.fY * projectileDir.fY + aimAdjustment.fZ * projectileDir.fZ;
                    if (dotToAimDir < 0.0f)
                        aimAdjustment -= projectileDir * dotToAimDir;

                    aimAdjustment.Normalize();

                    float steeringStrength = 0.0f;
                    float velocityScale = 1.0f;

                    if (projectileInfo->m_creator == localPlayer || projectileInfo->m_creator == localPlayer->pVehicle)
                        steeringStrength = 0.011f;
                    else
                        steeringStrength = 0.009f;

                    if (targetProjectile->m_vecLinearVelocity.Length() > 0.8f)
                        steeringStrength *= 1.2f;

                    if (isPlaneTargetFromLocalPlayer)
                    {
                        steeringStrength = 0.15f;
                        velocityScale = pGame->GetTimeStep() * 0.95f;
                    }

                    projectile->m_vecLinearVelocity *= velocityScale;
                    projectile->m_vecLinearVelocity += aimAdjustment * (pGame->GetTimeStep() * steeringStrength);

                    if (projectile->m_vecLinearVelocity.Length() > 9.9f)
                    {
                        projectile->m_vecLinearVelocity.Normalize();
                        projectile->m_vecLinearVelocity *= 9.9f;
                    }

                    projectile->matrix->vFront = projectileDir;

                    if (CheckIsLineOfSightClear(projectileInfo))
                        continue;

                    projectileInfo->m_lastPos = pos;
                    StaticRemoveProjectile(projectileInfo, projectileInfo->m_projectile);
                    continue;
                }

                break;
            }
            default:
            {
                if (projectileInfo->m_weaponType != eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE || projectile->m_fDamageImpulseMagnitude <= 0.0f || !projectile->m_pCollidedEntity || projectile->m_pAttachedEntity)
                {
                    UpdateLastPos(projectileInfo);
                    continue;
                }

                // CPhysical::AttachEntityToEntity
                ((void(__thiscall*)(CPhysicalSAInterface*, CPhysicalSAInterface*, float, float))0x54D690)(projectile, (CPhysicalSAInterface*)projectile->m_pCollidedEntity, 0, 0);
                projectile->bUsesCollision = false;
                break;
            }
        }
    }
}

int CProjectileInfoSA::FindFreeIndex()
{
    int freeIndex = -1;

    for (std::size_t i = 0; i < MAX_PROJECTILES_COUNT; i++)
    {
        if (ms_projectileInfo[i].m_isActive)
            continue;

        freeIndex = i;
        break;
    }

    return freeIndex;
}

void CProjectileInfoSA::InitCollision(CObjectSAInterface* projectile)
{
    CModelInfo* modelInfo = pGame->GetModelInfo(projectile->m_nModelIndex);
    if (modelInfo)
    {
        CColModelSAInterface* col = modelInfo->GetInterface()->pColModel;
        if (CColDataSA* colData = col->m_data)
        {
            if (!colData->m_spheres && colData->m_numSpheres == 0)
            {
                colData->m_numSpheres = 1;
                colData->m_spheres = ((CColSphereSA * (__cdecl*)(int))0x72F420)(20);            // CMemoryMgr::Malloc

                // CColSphere::Set
                ((void(__thiscall*)(CColSphereSA*, float, CVector&, unsigned char, char, unsigned char))(0x40FD10))(colData->m_spheres, col->m_sphere.m_radius * 0.75f, col->m_sphere.m_center, 56, 0, 255);
            }
        }
        else
        {
            // CColModel::AllocateData
            ((void(__thiscall*)(CColModelSAInterface*, int, int, int, int, int, int))(0x40F870))(col, 1, 0, 0, 0, 0, 0);

            if (col->m_data)
                // CColSphere::Set
                ((void(__thiscall*)(CColSphereSA*, float, CVector&, unsigned char, char, unsigned char))(0x40FD10))(col->m_data->m_spheres, col->m_sphere.m_radius * 0.75f, col->m_sphere.m_center, 56, 0, 255);
        }
    }
}

void CProjectileInfoSA::UpdateLastPos(CProjectileInfoSA* info)
{
    if (!info)
        return;

    CObjectSAInterface* object = info->m_projectile ? info->m_projectile->m_object : nullptr;
    if (!object)
        return;

    info->m_lastPos = object->matrix ? object->matrix->vPos : object->m_transform.m_translate;
}

bool CProjectileInfoSA::CheckIsLineOfSightClear(CProjectileInfoSA* info)
{
    if (!info)
        return false;

    CObjectSAInterface* object = info->m_projectile ? info->m_projectile->m_object : nullptr;
    if (!object)
        return false;

    if (!object->bOnSolidSurface)
    {
        CVector pos = object->matrix ? object->matrix->vPos : object->m_transform.m_translate;

        *(void**)0xB7CD68 = info->m_creator;
        object->bUsesCollision = false;

        // CWorld::GetIsLineOfSightClear
        bool clear = ((bool(__cdecl*)(CVector*, CVector*, bool, bool, bool, bool, bool, bool, bool))0x56A490)(&info->m_lastPos, &pos, true, true, true, true, false, false, false);

        object->bUsesCollision = true;
        *(void**)0xB7CD68 = nullptr;

        object->m_entityIgnoredCollision = info->m_creator;
        if (clear)
        {
            UpdateLastPos(info);
            return true;
        }
    }

    if (object->m_ucCollisionState > 0)
    {
        if (object->pLastContactedEntity[0] && (object->pLastContactedEntity[0]->GetInterface() == info->m_creator || object->pLastContactedEntity[0]->GetModelIndex() == 345))
        {
            UpdateLastPos(info);
            return true;
        }
    }

    return false;
}

CProjectileInfo* CProjectileInfoSA::StaticAddProjectile(CEntitySAInterface* creator, eWeaponType projectileType, CVector pos, float force, CVector* target, CEntitySAInterface* targetEntity)
{
    int freeIndex = FindFreeIndex();
    if (freeIndex == -1)
        return nullptr;

    CProjectileInfoSA* projectileInfo = &ms_projectileInfo[freeIndex];
    projectileInfo->m_weaponType = projectileType;
    projectileInfo->m_creator = creator;
    projectileInfo->m_target = targetEntity;

    std::uint32_t timeToDestroy = 2000;
    auto*         matrix = creator->matrix;
    float         heading = matrix ? std::atan2(-matrix->vFront.fX, matrix->vFront.fY) : creator->m_transform.m_heading;
    float         elasticity = 1.0f;

    float offset = force == 0.0f ? 0.0f : force * 0.22f + 0.15f;

    CMatrixSAInterface projectileMatrix;
    projectileMatrix.SetTranslate(CVector(0, 0, 0));
    projectileMatrix.RotateZ(heading);

    CVector matrixPos = projectileMatrix.GetPosition();
    projectileMatrix.SetTranslateOnly(matrixPos + pos);

    auto ComputeOffsetVector = [](float heading, float offset, float force) -> CVector {
        return CVector(
            std::sin(heading) * offset * -1.0f,
            std::cos(heading) * offset,
            (force + 1.0f) * 0.4f * offset
        );
    };

    CVector projectileVelocity = ComputeOffsetVector(heading, offset, force);

    bool useStaticObjectInfo = false;
    bool applyGravity = true;

    std::uint32_t model;

    if (projectileType == eWeaponType::WEAPONTYPE_FLARE)
    {
        pGame->GetStreaming()->RequestModel(354, 0); // 354 = MI_FLARE
        model = 354;
    }
    else // GetWeaponInfo returns null for flare
        model = pGame->GetWeaponInfo(projectileType, WEAPONSKILL_STD)->GetModel();

    CObjectSAInterface* projectile = CreateProjectileObject(model, projectileInfo);
    if (!projectile)
        return nullptr;

    switch (projectileType)
    {
        case eWeaponType::WEAPONTYPE_GRENADE:
        case eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE:
        {
            elasticity = 0.5f;

            if (projectileType == eWeaponType::WEAPONTYPE_REMOTE_SATCHEL_CHARGE)
            {
                offset *= 0.5f;
                elasticity = 0.03f;
            }

            if (creator->nType == ENTITY_TYPE_VEHICLE) // Call CGeneral::LimitRadianAngle
                heading = ((float(__cdecl*)(float))0x53CB50)(heading + PI);

            projectileVelocity = ComputeOffsetVector(heading, offset, force);

            if (creator->m_nModelIndex == 405) // ???
                projectileVelocity += static_cast<CVehicleSAInterface*>(creator)->m_vecLinearVelocity;

            InitCollision(projectile);
            useStaticObjectInfo = true;
            break;
        }
        case eWeaponType::WEAPONTYPE_TEARGAS:
        {
            useStaticObjectInfo = true;
            elasticity = 0.5f;
            timeToDestroy = 20000;

            InitCollision(projectile);
            break;
        }
        case eWeaponType::WEAPONTYPE_MOLOTOV:
        {
            if (offset < 0.2f)
                offset = 0.2f;

            InitCollision(projectile);

            projectileVelocity = ComputeOffsetVector(heading, offset, force);
            projectileVelocity.fZ = (force * 0.2f + 0.4f) * offset;
            break;
        }
        case eWeaponType::WEAPONTYPE_ROCKET:
        case eWeaponType::WEAPONTYPE_ROCKET_HS:
        {
            timeToDestroy = projectileType == eWeaponType::WEAPONTYPE_ROCKET ? 3000 : 100000;

            CVector in = CVector(0, projectileType == eWeaponType::WEAPONTYPE_ROCKET ? 0.4f : 0.2f, 0);

            if (creator->nType == ENTITY_TYPE_VEHICLE)
            {
                projectileMatrix = creator->GetMatrix();
                projectileMatrix.SetTranslateOnly(pos);

                in.fY += static_cast<CVehicleSAInterface*>(creator)->m_vecLinearVelocity.Length();
            }
            else if (creator->nType == ENTITY_TYPE_PED && static_cast<CPedSAInterface*>(creator)->bPedType < 2)
            {
                CCamera* camera = pGame->GetCamera();
                CCam*    cam = camera->GetCam(camera->GetActiveCam());

                CVector* camUp = cam->GetUp();
                CVector* camFront = cam->GetFront();

                CVector right = camUp->Clone();
                right.CrossProduct(camFront);

                projectileMatrix.SetMatrix(right, *camFront, *camUp, pos);
            }
            else if (target)
            {
                CMatrixSAInterface* matrix = creator->GetMatrix();

                CVector right = matrix->GetRight();
                CVector up = matrix->GetRight();
                up.CrossProduct(target);

                projectileMatrix.SetMatrix(right, *target, up, pos);
            }
            else
                projectileMatrix = creator->GetMatrix();

            projectileVelocity = CVector::Multiply3x3(projectileMatrix.GetRight(), projectileMatrix.GetForward(), projectileMatrix.GetUp(), in);
            applyGravity = false;
            break;
        }
        case eWeaponType::WEAPONTYPE_FREEFALL_BOMB:
        case eWeaponType::WEAPONTYPE_FLARE:
        {
            timeToDestroy = projectileType == eWeaponType::WEAPONTYPE_FREEFALL_BOMB ? 2000000 : 10000;

            if (projectileType == eWeaponType::WEAPONTYPE_FLARE)
                projectile->m_fAirResistance = 0.9f;

            projectileMatrix = creator->GetMatrix();
            projectileMatrix.SetTranslateOnly(pos);

            if (creator->nType > ENTITY_TYPE_BUILDING && creator->nType < ENTITY_TYPE_DUMMY)
                projectileVelocity = static_cast<CPhysicalSAInterface*>(creator)->m_vecLinearVelocity;
            else
                projectileVelocity = CVector(0, 0, 0);

            break;
        }
        default:
            break;
    }

    projectileInfo->m_counter = pGame->GetSystemTime() + timeToDestroy;
    projectileInfo->m_lastPos = projectileMatrix.GetPosition();

    projectile->SetMatrix(&projectileMatrix);
    projectile->m_vecLinearVelocity = projectileVelocity;
    projectile->m_fElasticity = elasticity;
    projectile->m_entityIgnoredCollision = creator;
    projectile->bEnableCollision = true;
    projectile->bApplyGravity = applyGravity;

    // CObjectData::Initialise (0x5B5420)
    if (useStaticObjectInfo)
        projectile->pObjectInfo = (CObjectInfo*)0xBB4BD0;

    pGame->GetWorld()->Add(projectile, eDebugCaller::CObject_Constructor);

    // CEntity::RegisterReference
    ((void(__thiscall*)(CEntitySAInterface*, CEntitySAInterface**))0x571B70)(creator, &projectileInfo->m_creator);
    ((void(__thiscall*)(CEntitySAInterface*, CObjectSAInterface**))0x571B70)(projectile, &projectileInfo->m_projectile->m_object);

    if (targetEntity) // CEntity::RegisterReference
        ((void(__thiscall*)(CEntitySAInterface*, CEntitySAInterface**))0x571B70)(targetEntity, &projectileInfo->m_target);

    if (creator && (creator->nType == ENTITY_TYPE_VEHICLE || creator->nType == ENTITY_TYPE_PED || creator->nType == ENTITY_TYPE_OBJECT) && !static_cast<CPhysicalSAInterface*>(creator)->m_entityIgnoredCollision)
        static_cast<CPhysicalSAInterface*>(creator)->m_entityIgnoredCollision = creator;

    if (projectileType == eWeaponType::WEAPONTYPE_TEARGAS)
    {
        CVector point = CVector(0, 0, 0);
        // CEntity::getModellingMatrix
        void* modellingMatrix = ((void*(__thiscall*)(CEntitySAInterface*))0x46A2D0)(projectile); // RwMatrixTag*
        if (modellingMatrix)
        {
            // FxManager_c::CreateFxSystem
            void* fx = ((void*(__thiscall*)(void*, const char*, CVector*, void*, bool))0x4A9BE0)((void*)0xA9AE80, "teargasAD", &point, modellingMatrix, false);

            if (fx)
            {
                projectileInfo->m_particle = fx;
                ((void(__thiscall*)(void*))0x4AA2F0)(fx); // FxSystem_c::Play
            }
        }
    }

    if (projectileType == eWeaponType::WEAPONTYPE_ROCKET_HS)
    {
        // CRadar::SetEntityBlip
        int blipIndex = ((int(__cdecl*)(int, int, int, int))0x5839A0)(3, pGame->GetPools()->GetObjectHandle(projectile), 0xFF0000FF, 2);
        ((void(__cdecl*)(int, std::int16_t))0x583CC0)(blipIndex, 1);            // CRadar::ChangeBlipScale

        CPedSAInterface* localPlayer = pGame->GetPedContext()->GetPedInterface();
        // CRadar::ChangeBlipColour
        if (creator == localPlayer || static_cast<CPedSAInterface*>(creator)->pVehicle == localPlayer->pVehicle)
            ((void(__cdecl*)(int, int))0x583AB0)(blipIndex, -1);
        else
            ((void(__cdecl*)(int, int))0x583AB0)(blipIndex, 0xFF0000FF);
    }

    // CAudioEngine::ReportWeaponEvent
    ((void(__thiscall*)(CAudioEngineSAInterface*, int, eWeaponType, CEntitySAInterface*))0x506F40)((CAudioEngineSAInterface*)0xB6BC90, 0x94, projectileType, projectile);

    projectileInfo->m_isActive = true;

    if (m_projectileCreationHandler)
    {
        CPools* pools = pGame->GetPools();

        CEntity* creatorEntity = pools->GetEntity((DWORD*)creator);
        CEntity* targetGameEntity = targetEntity ? pools->GetEntity((DWORD*)targetEntity) : nullptr;

        if (!m_projectileCreationHandler(creatorEntity, projectileInfo->m_projectile, projectileInfo, projectileType, &pos, force, target, targetGameEntity))
            return nullptr;
    }

    return projectileInfo;
}

CProjectileInfo* CProjectileInfoSA::AddProjectile(CEntity* creator, eWeaponType projectileType, CVector pos, float force, CVector* target, CEntity* targetEntity) const
{
    return CProjectileInfoSA::StaticAddProjectile(creator->GetInterface(), projectileType, pos, force, target, targetEntity ? targetEntity->GetInterface() : nullptr);
}

void CProjectileInfoSA::RemoveProjectile(CProjectileInfo* info, CProjectile* object, bool blow) const
{
    CProjectileInfoSA::StaticRemoveProjectile(dynamic_cast<CProjectileInfoSA*>(info), dynamic_cast<CProjectileSA*>(object), blow);
}

CEntity* CProjectileInfoSA::GetTarget()
{
    CEntitySAInterface* pTargetInterface = m_target;
    CEntity*            pTarget = nullptr;
    if (pTargetInterface)
    {
        switch (pTargetInterface->nType)
        {
            case ENTITY_TYPE_PED:
            {
                SClientEntity<CPedSA>* pPedClientEntity = pGame->GetPools()->GetPed((DWORD*)pTargetInterface);
                pTarget = pPedClientEntity ? pPedClientEntity->pEntity : nullptr;
                break;
            }
            case ENTITY_TYPE_VEHICLE:
            {
                SClientEntity<CVehicleSA>* pVehicleClientEntity = pGame->GetPools()->GetVehicle((DWORD*)pTargetInterface);
                pTarget = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
                break;
            }
        }
    }
    return pTarget;
}

void CProjectileInfoSA::SetTarget(CEntity* pEntity)
{
    CEntitySA* pEntitySA = dynamic_cast<CEntitySA*>(pEntity);
    if (pEntitySA)
        m_target = pEntitySA->GetInterface();
}

bool CProjectileInfoSA::IsActive()
{
    return (m_isActive == 1 && m_weaponType != 0);
}

void CProjectileInfoSA::SetCounter(DWORD dwCounter)
{
    m_counter = dwCounter + pGame->GetSystemTime();
}

DWORD CProjectileInfoSA::GetCounter()
{
    return m_counter - pGame->GetSystemTime();
}

void CProjectileInfoSA::RemoveEntityReferences(CEntity* entity)
{
    const CEntitySAInterface* entityInterface = entity->GetInterface();
    for (int i = 0; i < PROJECTILE_INFO_COUNT; i++)
    {
        auto projectileInterface = ms_projectileInfo[i];

        if (projectileInterface.m_creator == entityInterface)
            projectileInterface.m_creator = nullptr;

        if (projectileInterface.m_creator == entityInterface)
            projectileInterface.m_creator = nullptr;
    }
}

void CProjectileInfoSA::StaticSetHooks()
{
    HookInstallCall(0x73A30F, (DWORD)CProjectileInfoSA::Initialise);
    HookInstallCall(0x73A33F, (DWORD)CProjectileInfoSA::Shutdown);

    HookInstallCall(0x429763, (DWORD)CProjectileInfoSA::StaticAddProjectile);
    HookInstallCall(0x42B45B, (DWORD)CProjectileInfoSA::StaticAddProjectile);
    HookInstallCall(0x5A0B3B, (DWORD)CProjectileInfoSA::StaticAddProjectile);
    HookInstallCall(0x6E0749, (DWORD)CProjectileInfoSA::StaticAddProjectile);
    HookInstallCall(0x6E08BF, (DWORD)CProjectileInfoSA::StaticAddProjectile);
    HookInstallCall(0x6E35D7, (DWORD)CProjectileInfoSA::StaticAddProjectile);
    HookInstallCall(0x741A53, (DWORD)CProjectileInfoSA::StaticAddProjectile);

    HookInstallCall(0x4808C0, (DWORD)CProjectileInfoSA::IsProjectileInRange);

    HookInstall(0x56612E, (void*)CProjectileInfoSA::RemoveDetonatorProjectiles, 5);

    HookInstallCall(0x73A36A, (DWORD)CProjectileInfoSA::Update);

    HookInstallCall(0x741928, (DWORD)CProjectileInfoSA::RemoveNotAdd);
    HookInstallCall(0x741A11, (DWORD)CProjectileInfoSA::RemoveNotAdd);

    HookInstallCall(0x5500E8, (DWORD)CProjectileInfoSA::RemoveIfThisIsAProjectile);

    HookInstallCall(0x564372, (DWORD)CProjectileInfoSA::RemoveAllProjectiles);
    HookInstallCall(0X56A46E, (DWORD)CProjectileInfoSA::RemoveAllProjectiles);
    HookInstallCall(0X56E975, (DWORD)CProjectileInfoSA::RemoveAllProjectiles);
}
