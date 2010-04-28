/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFireSA.cpp
*  PURPOSE:     Fire
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/**
 * Put the fire out
 */
VOID CFireSA::Extinguish (  )
{
    DEBUG_TRACE("VOID CFireSA::Extinguish (  )");
    DWORD dwFunction = FUNC_Extinguish;
    DWORD dwPointer = (DWORD)this->internalInterface;
    _asm
    {
        mov     ecx, dwPointer
        call    dwFunction
    }
    this->internalInterface->bActive = FALSE;
}

/**
 * Gets the position the fire is burning at
 * @return CVector * containing the fire's position
 */
CVector * CFireSA::GetPosition ( )
{
    DEBUG_TRACE("CVector * CFireSA::GetPosition ( )");
    return &internalInterface->vecPosition;
}

/**
 * Set the position the fire is burning at. This won't have any effect if this fire has a target set
 * @param vecPosition CVector * containing the desired position for the fire.
 * @see CFireSA::SetTarget
 */
VOID CFireSA::SetPosition ( CVector & vecPosition )
{
    DEBUG_TRACE("VOID CFireSA::SetPosition ( CVector & vecPosition )");
    this->internalInterface->entityTarget = 0;
    memcpy(&internalInterface->vecPosition, &vecPosition, sizeof(CVector));
}

/**
 * Set the time that the fire will be extinguished.
 * @param dwTime DWORD containing the time that the fire will be extinguished.  This is in game-time units which can be got from CGame::GetSystemTime;
 */
VOID CFireSA::SetTimeToBurnOut ( DWORD dwTime )
{
    DEBUG_TRACE("VOID CFireSA::SetTimeToBurnOut ( DWORD dwTime )");
    internalInterface->nTimeToBurn = dwTime;
}

DWORD CFireSA::GetTimeToBurnOut (  )
{
    DEBUG_TRACE("DWORD CFireSA::GetTimeToBurnOut (  )");
    return internalInterface->nTimeToBurn;
}

CEntity * CFireSA::GetCreator (  )
{
    DEBUG_TRACE("CEntity * CFireSA::GetCreator (  )");
    CEntity * creatorEntity = NULL;
    CEntitySAInterface * createEntitySA = internalInterface->entityCreator;
    CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
    if ( pPools && createEntitySA )
    {
        switch ( createEntitySA->nType )
        {
        case ENTITY_TYPE_PED:
            creatorEntity = pPools->GetPed((DWORD *)createEntitySA);
            break;
        case ENTITY_TYPE_VEHICLE:
            creatorEntity = pPools->GetVehicle((DWORD *)createEntitySA);
            break;
        default:
            creatorEntity = NULL;
        }
    }
    return creatorEntity;
}

CEntity * CFireSA::GetEntityOnFire (  )
{
    DEBUG_TRACE("CEntity * CFireSA::GetEntityOnFire (  )");
    CEntity * TargetEntity = NULL;
    CEntitySAInterface * TargetEntitySA = internalInterface->entityTarget;
    CPoolsSA * pPools = ((CPoolsSA *)pGame->GetPools());
    if ( pPools && TargetEntitySA )
    {
        switch ( TargetEntitySA->nType )
        {
        case ENTITY_TYPE_PED:
            TargetEntity = pPools->GetPed((DWORD *)TargetEntitySA);
            break;
        case ENTITY_TYPE_VEHICLE:
            TargetEntity = pPools->GetVehicle((DWORD *)TargetEntitySA);
            break;
        default:
            TargetEntity = NULL;
        }
    }
    return TargetEntity;
}

VOID CFireSA::SetTarget ( CEntity * entity  )
{
    DEBUG_TRACE("VOID CFireSA::SetTarget ( CEntity * entity  )");

    if ( entity )
    {
        CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( entity );
        if ( pEntitySA )
            internalInterface->entityTarget = pEntitySA->GetInterface();
    }
    else
    {
        internalInterface->entityTarget = NULL;
    }
}

BOOL CFireSA::IsIgnited (  )
{
    DEBUG_TRACE("BOOL CFireSA::IsIgnited (  )");
    return internalInterface->bActive;
}

BOOL CFireSA::IsFree (  )
{
    DEBUG_TRACE("BOOL CFireSA::IsFree (  )");
    if(!internalInterface->bActive && !internalInterface->bCreatedByScript)
        return TRUE;
    else
        return FALSE;
}

VOID CFireSA::SetSilent ( BOOL bSilent )
{
    DEBUG_TRACE("VOID CFireSA::SetSilent ( BOOL bSilent )");
    internalInterface->bMakesNoise = !bSilent;
}

BOOL CFireSA::IsBeingExtinguished (  )
{
    DEBUG_TRACE("BOOL CFireSA::IsBeingExtinguished (  )");
    return internalInterface->bBeingExtinguished;
}

VOID CFireSA::Ignite( )
{
    DEBUG_TRACE("VOID CFireSA::Ignite( )");
    this->internalInterface->bActive = TRUE;

    CVector * vecPosition = this->GetPosition();
    DWORD dwFunc = FUNC_CreateFxSysForStrength;
    DWORD dwThis = (DWORD)this->internalInterface;
    _asm
    {
        mov     ecx, dwThis
        push    0
        push    vecPosition
        call    dwFunc
    }

    this->internalInterface->bBeingExtinguished = 0;
    this->internalInterface->bFirstGeneration = 1;
    this->internalInterface->nNumGenerationsAllowed = 100;
}

FLOAT CFireSA::GetStrength (  )
{
    DEBUG_TRACE("FLOAT CFireSA::GetStrength (  )");
    return this->internalInterface->Strength;
}

VOID CFireSA::SetStrength ( FLOAT fStrength )
{
    DEBUG_TRACE("VOID CFireSA::SetStrength ( FLOAT fStrength )");
    this->internalInterface->Strength = fStrength;
}


VOID CFireSA::SetNumGenerationsAllowed ( char generations )
{
    DEBUG_TRACE("VOID CFireSA::SetNumGenerationsAllowed ( char generations )");
    this->internalInterface->nNumGenerationsAllowed = generations;
}
