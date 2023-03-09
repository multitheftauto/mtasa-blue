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
#include "CEntitySA.h"
#include "CExplosionSA.h"
#include "CGameSA.h"
#include "CVehicleSA.h"

extern CGameSA* pGame;

eExplosionType CExplosionSA::GetExplosionType()
{
    return internalInterface->m_ExplosionType;
}

CVector* CExplosionSA::GetExplosionPosition()
{
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
    eEntityType entityType = (eEntityType)GetInterface()->m_pEntExplosionOwner->nType;

    CPools* pools = pGame->GetPools();
    switch (entityType)
    {
        case ENTITY_TYPE_PED:
        {
            SClientEntity<CPedSA>* pPedClientEntity = pools->GetPed((DWORD*)GetInterface()->m_pEntExplosionOwner);
            if (pPedClientEntity)
            {
                return pPedClientEntity->pEntity;
            }
            break;
        }
        case ENTITY_TYPE_VEHICLE:
        {
            SClientEntity<CVehicleSA>* pVehicleClientEntity = pools->GetVehicle((DWORD*)GetInterface()->m_pEntExplosionOwner);
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

bool CExplosionSA::IsActive()
{
    return internalInterface->m_cExplosionActive;
}

void CExplosionSA::Remove()
{
    internalInterface->m_cExplosionActive = 0;
}

float CExplosionSA::GetExplosionForce()
{
    return GetInterface()->m_fExplosionForce;
}

void CExplosionSA::SetExplosionForce(float fForce)
{
    GetInterface()->m_fExplosionForce = fForce;
}

void CExplosionSA::SetSilent(bool bSilent)
{
    GetInterface()->m_bMakeSound = !bSilent;
}

unsigned long CExplosionSA::GetActivationTimer()
{
    return GetInterface()->m_ActivationTime;
}

void CExplosionSA::SetActivationTimer(unsigned long ulActivationTime)
{
    GetInterface()->m_ActivationTime = ulActivationTime;
}

DWORD CExplosionSA::GetExpiryTime()
{
    return GetInterface()->m_TimeExpires;
}

void CExplosionSA::SetExpiryTime(DWORD dwExpiryTime)
{
    GetInterface()->m_TimeExpires = dwExpiryTime;
}

float CExplosionSA::GetExplosionRadius()
{
    return GetInterface()->m_fExplosionRadius;
}

void CExplosionSA::SetExplosionRadius(float fRadius)
{
    GetInterface()->m_fExplosionRadius = fRadius;
}
