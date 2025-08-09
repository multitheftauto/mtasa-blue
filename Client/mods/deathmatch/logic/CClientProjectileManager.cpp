/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientProjectileManager.cpp
 *  PURPOSE:     Projectile entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "game/CProjectileInfo.h"

using std::list;

CClientProjectileManager* g_pProjectileManager = NULL;

CClientProjectileManager::CClientProjectileManager(CClientManager* pManager)
{
    CClientEntityRefManager::AddEntityRefs(ENTITY_REF_DEBUG(this, "CClientProjectileManager"), &m_pCreator, &m_pLastCreated, NULL);

    g_pProjectileManager = this;
    m_pManager = pManager;

    m_bIsLocal = false;
    m_pCreator = NULL;

    m_bCreating = false;
    m_pLastCreated = NULL;
}

CClientProjectileManager::~CClientProjectileManager()
{
    RemoveAll();
    if (g_pProjectileManager == this)
        g_pProjectileManager = NULL;
    CClientEntityRefManager::RemoveEntityRefs(0, &m_pCreator, &m_pLastCreated, NULL);
}

void CClientProjectileManager::DoPulse()
{
    CElementDeleter*                   pElementDeleter = g_pClientGame->GetElementDeleter();
    CClientProjectile*                 pProjectile = NULL;
    list<CClientProjectile*>           cloneList = m_List;
    list<CClientProjectile*>::iterator iter = cloneList.begin();
    for (; iter != cloneList.end(); ++iter)
    {
        pProjectile = *iter;

        // Is this projectile still active?
        if (pProjectile->IsActive())
        {
            pProjectile->DoPulse();
        }
    }
}

void CClientProjectileManager::RemoveAll()
{
    list<CClientProjectile*>           cloneList = m_List;
    list<CClientProjectile*>::iterator iter = cloneList.begin();
    for (; iter != cloneList.end(); ++iter)
    {
        delete *iter;
    }
    m_List.clear();
}

bool CClientProjectileManager::Exists(CClientProjectile* pProjectile)
{
    list<CClientProjectile*>::iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        if (*iter == pProjectile)
        {
            return true;
        }
    }
    return false;
}

CClientProjectile* CClientProjectileManager::Get(CEntitySAInterface* projectileObject)
{
    int iCount = m_List.size();
    //assert(iCount <= 32);
    list<CClientProjectile*>::iterator iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        CEntity* projectile = (*iter)->GetGameEntity();
        if (!projectile)
            continue;

        if (projectile->GetInterface() == projectileObject)
            return (*iter);
    }
    return NULL;
}

void CClientProjectileManager::RemoveFromList(CClientProjectile* pProjectile)
{
    m_List.remove(pProjectile);
}

bool CClientProjectileManager::Hook_StaticProjectileCreation(CEntity* pGameCreator, CProjectile* pGameProjectile, CProjectileInfo* pProjectileInfo,
                                                             eWeaponType weaponType, CVector* origin, float fForce, CVector* target, CEntity* pGameTarget)
{
    return g_pProjectileManager->Hook_ProjectileCreation(pGameCreator, pGameProjectile, pProjectileInfo, weaponType, origin, fForce, target, pGameTarget);
}

bool CClientProjectileManager::Hook_ProjectileCreation(CEntity* pGameCreator, CProjectile* pGameProjectile, CProjectileInfo* pProjectileInfo,
                                                       eWeaponType weaponType, CVector* origin, float fForce, CVector* target, CEntity* pGameTarget)
{
    CPools*        pPools = g_pGame->GetPools();
    CClientEntity* pTarget = pGameTarget ? pPools->GetClientEntity((DWORD*)pGameTarget->GetInterface()) : nullptr;
    CClientEntity* creator = pGameCreator ? pPools->GetClientEntity((DWORD*)pGameCreator->GetInterface()) : nullptr;

    CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer();
    m_bIsLocal = (creator == pLocalPlayer || (pLocalPlayer->GetOccupiedVehicleSeat() == 0 && creator == pLocalPlayer->GetOccupiedVehicle()));

    m_pLastCreated = new CClientProjectile(m_pManager, pGameProjectile, pProjectileInfo, creator, pTarget, weaponType, origin, target, fForce, m_bIsLocal);

    // Validate the projectile for our element tree
    m_pLastCreated->SetParent(g_pClientGame->GetRootEntity());

    CLuaArguments Arguments;
    Arguments.PushElement(creator);
    Arguments.PushNumber(weaponType);
    Arguments.PushNumber(origin->fX);
    Arguments.PushNumber(origin->fY);
    Arguments.PushNumber(origin->fZ);
    Arguments.PushNumber(fForce);
    Arguments.PushNumber(target ? target->fX : 0);
    Arguments.PushNumber(target ? target->fY : 0);
    Arguments.PushNumber(target ? target->fZ : 0);
    if (pTarget)
        Arguments.PushElement(pTarget);
    else
        Arguments.PushNil();

    bool continueCreation = m_pLastCreated->CallEvent("onClientProjectileCreation", Arguments, true);
    if (!continueCreation)
        m_pLastCreated->Destroy(false);

    return continueCreation;
}

CClientProjectile* CClientProjectileManager::Create(CClientEntity* creator, eWeaponType weapon, CVector& origin, float force, CVector* target, CClientEntity* targetEntity)
{
    CEntity* creatorGameEntity = creator->GetGameEntity();
    if (!creatorGameEntity)
        return nullptr;

    CProjectileInfo* gameProjectile = g_pGame->GetProjectileInfo()->AddProjectile(creatorGameEntity, weapon, origin, force, target, targetEntity ? targetEntity->GetGameEntity() : nullptr);
    if (!gameProjectile)
        return nullptr;

    return m_pLastCreated;
}
