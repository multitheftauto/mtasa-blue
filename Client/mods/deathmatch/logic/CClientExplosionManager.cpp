/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientExplosionManager.cpp
 *  PURPOSE:     Explosion event manager class
 *
 *****************************************************************************/

#include <StdInc.h>

extern CClientGame* g_pClientGame;

CClientExplosionManager* g_pExplosionManager = NULL;

CClientExplosionManager::CClientExplosionManager(CClientManager* pManager)
{
    CClientEntityRefManager::AddEntityRefs(ENTITY_REF_DEBUG(this, "CClientExplosionManager"), &m_pLastCreator, NULL);

    g_pExplosionManager = this;
    m_pManager = pManager;
    m_LastWeaponType = WEAPONTYPE_UNARMED;
    m_pLastCreator = NULL;
}

CClientExplosionManager::~CClientExplosionManager()
{
    if (g_pExplosionManager == this)
        g_pExplosionManager = NULL;

    CClientEntityRefManager::RemoveEntityRefs(0, &m_pLastCreator, NULL);
}

bool CClientExplosionManager::Hook_StaticExplosionCreation(CEntity* pGameExplodingEntity, CEntity* pGameCreator, const CVector& vecPosition,
                                                           eExplosionType explosionType)
{
    return g_pExplosionManager->Hook_ExplosionCreation(pGameExplodingEntity, pGameCreator, vecPosition, explosionType);
}

bool CClientExplosionManager::Hook_ExplosionCreation(CEntity* pGameExplodingEntity, CEntity* pGameCreator, const CVector& vecPosition,
                                                     eExplosionType explosionType)
{
    CPools* const pPools = g_pGame->GetPools();

    // Grab the entity responsible
    CEntity* const pResponsibleGameEntity = pGameExplodingEntity ? pGameExplodingEntity : pGameCreator;

    if (!pResponsibleGameEntity)
        return false;

    CClientEntity* const pResponsible = pPools->GetClientEntity(reinterpret_cast<DWORD*>(pResponsibleGameEntity->GetInterface()));;

    if (!pResponsible)
        return false;

    // Determine the used weapon
    eWeaponType explosionWeaponType = WEAPONTYPE_EXPLOSION;

    switch (explosionType)
    {
    case EXP_TYPE_GRENADE:
    {
        // Grenade type explosions from vehicles should only be freefall bombs
        // TODO: need a way to check if its a freefall bomb if creator is a ped
        if (pGameCreator && pGameCreator->GetEntityType() == ENTITY_TYPE_VEHICLE)
            explosionWeaponType = WEAPONTYPE_FREEFALL_BOMB;
        else
            explosionWeaponType = WEAPONTYPE_GRENADE;
        break;
    }
    case EXP_TYPE_MOLOTOV:
        explosionWeaponType = WEAPONTYPE_MOLOTOV;
        break;
    case EXP_TYPE_ROCKET:
    case EXP_TYPE_ROCKET_WEAK:
        explosionWeaponType = WEAPONTYPE_ROCKET;
        break;
    case EXP_TYPE_TANK_GRENADE:
        explosionWeaponType = WEAPONTYPE_TANK_GRENADE;
        break;
    default:
        break;
    }

    // Handle this explosion client side only if entity is local or breakable (i.e. barrel)
    unsigned short usModel;
    const bool     bHasModel = CStaticFunctionDefinitions::GetElementModel(*pResponsible, usModel);

    if (pResponsible->IsLocalEntity() || (bHasModel && CClientObjectManager::IsBreakableModel(usModel)))
    {
        CLuaArguments Arguments;
        Arguments.PushNumber(vecPosition.fX);
        Arguments.PushNumber(vecPosition.fY);
        Arguments.PushNumber(vecPosition.fZ);
        Arguments.PushNumber(explosionWeaponType);
        const bool bAllowExplosion = pResponsible->CallEvent("onClientExplosion", Arguments, true);
        return bAllowExplosion;
    }

    // All explosions are handled server side (ATTENTION: always 'return false;' below)
    CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer();

    // Is the local player responsible for this?
    const bool bIsLocalPlayer = pResponsible == pLocalPlayer;
    const bool bIsLocalPlayerVehicle = pResponsible == pLocalPlayer->GetOccupiedVehicle();
    const bool bIsUnoccupiedVehicleSynced = g_pClientGame->GetUnoccupiedVehicleSync()->Exists(static_cast<CDeathmatchVehicle*>(pResponsible));

    if (!bIsLocalPlayer && !bIsLocalPlayerVehicle && !bIsUnoccupiedVehicleSynced)
        return false;

    CClientEntity* pOriginSource = nullptr;

    // Is this an exploding vehicle?
    if (pGameExplodingEntity && pGameExplodingEntity->GetEntityType() == ENTITY_TYPE_VEHICLE)
    {
        // Set our origin-source to the vehicle
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pPools->GetVehicle(reinterpret_cast<DWORD*>(pGameExplodingEntity->GetInterface()));

        if (pVehicleClientEntity)
        {
            pOriginSource = pVehicleClientEntity->pClientEntity;
        }
    }
    // If theres other players, sync it relative to the closest (lag compensation)
    else if (m_pManager->GetPlayerManager()->Count() > 1)
    {
        switch (explosionWeaponType)
        {
            case WEAPONTYPE_ROCKET:
            case WEAPONTYPE_ROCKET_HS:
            {
                CClientPlayer* pPlayer = g_pClientGame->GetClosestRemotePlayer(vecPosition, 200.0f);

                if (pPlayer)
                {
                    pOriginSource = pPlayer;
                }

                break;
            }
        }
    }

    // Request a new explosion
    g_pClientGame->SendExplosionSync(vecPosition, explosionType, pOriginSource);
    return false;
}

CExplosion* CClientExplosionManager::Create(eExplosionType explosionType, CVector& vecPosition, CClientEntity* pCreator, bool bMakeSound, float fCamShake,
                                            bool bNoDamage, eWeaponType responsibleWeapon)
{
    CEntity* pGameCreator = NULL;
    if (pCreator)
        pGameCreator = pCreator->GetGameEntity();

    // Update our records first?
    m_pLastCreator = pCreator;
    if (responsibleWeapon != WEAPONTYPE_UNARMED)
        m_LastWeaponType = responsibleWeapon;
    else
    {
        switch (explosionType)
        {
            case EXP_TYPE_GRENADE:
                m_LastWeaponType = WEAPONTYPE_GRENADE;
                break;
            case EXP_TYPE_MOLOTOV:
                m_LastWeaponType = WEAPONTYPE_MOLOTOV;
                break;
            case EXP_TYPE_ROCKET:
            case EXP_TYPE_ROCKET_WEAK:
                m_LastWeaponType = WEAPONTYPE_ROCKET;
                break;
            case EXP_TYPE_TANK_GRENADE:
                m_LastWeaponType = WEAPONTYPE_TANK_GRENADE;
                break;
            default:
                m_LastWeaponType = WEAPONTYPE_EXPLOSION;
                break;
        }
    }

    CExplosion* pExplosion = g_pGame->GetExplosionManager()->AddExplosion(NULL, pGameCreator, explosionType, vecPosition, 0, bMakeSound, fCamShake, bNoDamage);
    return pExplosion;
}
