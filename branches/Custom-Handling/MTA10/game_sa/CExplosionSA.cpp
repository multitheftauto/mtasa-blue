/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CExplosionSA.cpp
*  PURPOSE:     Explosion
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

eExplosionType CExplosionSA::GetExplosionType ( void )
{
    DEBUG_TRACE("eExplosionType CExplosionSA::GetExplosionType (  )");
    return internalInterface->m_ExplosionType;
}

CVector* CExplosionSA::GetExplosionPosition ( void )
{
    DEBUG_TRACE("CVector * CExplosionSA::GetExplosionPosition (  )");
    return &internalInterface->m_vecPosition;
}

void CExplosionSA::SetExplosionPosition ( const CVector* vecPosition )
{
    internalInterface->m_vecPosition = *vecPosition;
}

/**
 * \todo Handle objects creating explosions
 */
CEntity* CExplosionSA::GetExplosionCreator ( void )
{
    DEBUG_TRACE("CEntity * CExplosionSA::GetExplosionCreator (  )");
    // we create a temporary entity to take care of finding the type of entity it is
    CEntitySA * entity = new CEntitySA;
    entity->m_pInterface = this->GetInterface()->m_pEntExplosionOwner;
    eEntityType entityType = entity->GetEntityType();
    delete entity;

    CPoolsSA * pools = (CPoolsSA*)pGame->GetPools();

    switch(entityType)
    {
    case ENTITY_TYPE_PED:
        return (CEntity *)(pools->GetPed((DWORD *)this->GetInterface()->m_pEntExplosionOwner));
        break;
    case ENTITY_TYPE_VEHICLE:
        return (CEntity *)(pools->GetVehicle((DWORD *)this->GetInterface()->m_pEntExplosionOwner));
        break;
    case ENTITY_TYPE_OBJECT:
        break;
    }
    return NULL;
}

CEntity* CExplosionSA::GetExplodingEntity ( void )
{
    return NULL;
}

BOOL CExplosionSA::IsActive ( void )
{
    DEBUG_TRACE("BOOL CExplosionSA::IsActive (  )");
    return internalInterface->m_cExplosionActive;
}

VOID CExplosionSA::Remove ( void )
{
    DEBUG_TRACE("VOID CExplosionSA::Remove (  )");
    internalInterface->m_cExplosionActive = 0;
}

float CExplosionSA::GetExplosionForce ( void )
{
    DEBUG_TRACE("FLOAT CExplosionSA::GetExplosionForce()");
    return this->GetInterface()->m_fExplosionForce;
}

void CExplosionSA::SetExplosionForce ( float fForce )
{
    DEBUG_TRACE("VOID CExplosionSA::SetExplosionForce(FLOAT fForce)");
    this->GetInterface()->m_fExplosionForce = fForce;
}

void CExplosionSA::SetSilent ( bool bSilent )
{
    DEBUG_TRACE("VOID CExplosionSA::SetSilent(BOOL bSilent)");
    this->GetInterface()->m_bMakeSound = !bSilent;
}

unsigned long CExplosionSA::GetActivationTimer()
{
    DEBUG_TRACE("DWORD CExplosionSA::GetActivationTimer()");
    return this->GetInterface()->m_ActivationTime;
}

void CExplosionSA::SetActivationTimer ( unsigned long ulActivationTime )
{
    DEBUG_TRACE("VOID CExplosionSA::SetActivationTimer( DWORD dwActivationTime )");
    this->GetInterface()->m_ActivationTime = ulActivationTime;
}

DWORD CExplosionSA::GetExpiryTime ( void )
{
    DEBUG_TRACE("DWORD CExplosionSA::GetExpiryTime()");
    return this->GetInterface()->m_TimeExpires;
}

void CExplosionSA::SetExpiryTime ( DWORD dwExpiryTime )
{
    DEBUG_TRACE("VOID CExplosionSA::SetExpiryTime( DWORD dwExpiryTime )");
    this->GetInterface()->m_TimeExpires = dwExpiryTime;
}

float CExplosionSA::GetExplosionRadius ( void )
{
    DEBUG_TRACE("FLOAT CExplosionSA::GetExplosionRadius()");
    return this->GetInterface()->m_fExplosionRadius;
}

void CExplosionSA::SetExplosionRadius ( float fRadius )
{
    DEBUG_TRACE("VOID CExplosionSA::SetExplosionRadius( FLOAT fRadius )");
    this->GetInterface()->m_fExplosionRadius = fRadius;
}
