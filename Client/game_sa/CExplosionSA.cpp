/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CExplosionSA.cpp
 *  PURPOSE:     Explosion
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

eExplosionType CExplosionSA::GetExplosionType()
{
    DEBUG_TRACE("eExplosionType CExplosionSA::GetExplosionType (  )");
    return internalInterface->m_ExplosionType;
}

CVector* CExplosionSA::GetExplosionPosition()
{
    DEBUG_TRACE("CVector * CExplosionSA::GetExplosionPosition (  )");
    return &internalInterface->m_vecPosition;
}

void CExplosionSA::SetExplosionPosition(const CVector* vecPosition)
{
    internalInterface->m_vecPosition = *vecPosition;
}

/**
 * \todo Handle objects creating explosions
 */
CEntity* CExplosionSA::GetExplosionCreator()
{
    DEBUG_TRACE("CEntity * CExplosionSA::GetExplosionCreator (  )");
    // we create a temporary entity to take care of finding the type of entity it is
    CEntitySA* entity = new CEntitySA;
    entity->m_pInterface = this->GetInterface()->m_pEntExplosionOwner;
    eEntityType entityType = entity->GetEntityType();
    delete entity;

    CPools* pools = pGame->GetPools();
    switch (entityType)
    {
        case ENTITY_TYPE_PED:
        {
            SClientEntity<CPedSA>* pPedClientEntity = pools->GetPed((DWORD*)this->GetInterface()->m_pEntExplosionOwner);
            if (pPedClientEntity)
            {
                return pPedClientEntity->pEntity;
            }
            break;
        }
        case ENTITY_TYPE_VEHICLE:
        {
            SClientEntity<CVehicleSA>* pVehicleClientEntity = pools->GetVehicle((DWORD*)this->GetInterface()->m_pEntExplosionOwner);
            if (pVehicleClientEntity)
            {
                return pVehicleClientEntity->pEntity;
            }
            break;
        }
        case ENTITY_TYPE_OBJECT:
        {
            break;
        }
    }
    return NULL;
}

CEntity* CExplosionSA::GetExplodingEntity()
{
    return NULL;
}

BOOL CExplosionSA::IsActive()
{
    DEBUG_TRACE("BOOL CExplosionSA::IsActive (  )");
    return internalInterface->m_cExplosionActive;
}

VOID CExplosionSA::Remove()
{
    DEBUG_TRACE("VOID CExplosionSA::Remove (  )");
    internalInterface->m_cExplosionActive = 0;
}

float CExplosionSA::GetExplosionForce()
{
    DEBUG_TRACE("FLOAT CExplosionSA::GetExplosionForce()");
    return this->GetInterface()->m_fExplosionForce;
}

void CExplosionSA::SetExplosionForce(float fForce)
{
    DEBUG_TRACE("VOID CExplosionSA::SetExplosionForce(FLOAT fForce)");
    this->GetInterface()->m_fExplosionForce = fForce;
}

void CExplosionSA::SetSilent(bool bSilent)
{
    DEBUG_TRACE("VOID CExplosionSA::SetSilent(BOOL bSilent)");
    this->GetInterface()->m_bMakeSound = !bSilent;
}

unsigned long CExplosionSA::GetActivationTimer()
{
    DEBUG_TRACE("DWORD CExplosionSA::GetActivationTimer()");
    return this->GetInterface()->m_ActivationTime;
}

void CExplosionSA::SetActivationTimer(unsigned long ulActivationTime)
{
    DEBUG_TRACE("VOID CExplosionSA::SetActivationTimer( DWORD dwActivationTime )");
    this->GetInterface()->m_ActivationTime = ulActivationTime;
}

DWORD CExplosionSA::GetExpiryTime()
{
    DEBUG_TRACE("DWORD CExplosionSA::GetExpiryTime()");
    return this->GetInterface()->m_TimeExpires;
}

void CExplosionSA::SetExpiryTime(DWORD dwExpiryTime)
{
    DEBUG_TRACE("VOID CExplosionSA::SetExpiryTime( DWORD dwExpiryTime )");
    this->GetInterface()->m_TimeExpires = dwExpiryTime;
}

float CExplosionSA::GetExplosionRadius()
{
    DEBUG_TRACE("FLOAT CExplosionSA::GetExplosionRadius()");
    return this->GetInterface()->m_fExplosionRadius;
}

void CExplosionSA::SetExplosionRadius(float fRadius)
{
    DEBUG_TRACE("VOID CExplosionSA::SetExplosionRadius( FLOAT fRadius )");
    this->GetInterface()->m_fExplosionRadius = fRadius;
}
