/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CProjectileInfoSA.cpp
*  PURPOSE:     Projectile type information
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA * pGame;
/**
 * This destroys all the projectiles in the world
 */
void CProjectileInfoSA::RemoveAllProjectiles (  )
{
	DEBUG_TRACE("void CProjectileInfoSA::RemoveAllProjectiles (  )");
	DWORD dwFunction = FUNC_RemoveAllProjectiles;
	_asm
	{
		call	dwFunction
	}
}

/**
 * Gets a projectile class for a specific projectile ID
 * @param ID valid projectile ID (0-31)
 * @return CProjectile * for the requested projectile, or NULL if the projectile slot is empty or the ID is invalid
 * \todo Check this CProjectile array is how it says in the function
 */
CProjectile	* CProjectileInfoSA::GetProjectile ( DWORD ID )
{
	DEBUG_TRACE("CProjectile * CProjectileInfoSA::GetProjectile ( DWORD ID )");
	if(ID >= 0 && ID < 32)
		return (CProjectile *)(ARRAY_CProjectile + ID * sizeof(CProjectile *));
	else
		return NULL;
}

CProjectile * CProjectileInfoSA::GetProjectile ( void * projectilePointer )
{
    // This must be destroyed later
    return new CProjectileSA((CProjectileSAInterface*)projectilePointer);
}

CProjectileInfo * CProjectileInfoSA::GetProjectileInfo ( void * projectileInfoInterface )
{
    return projectileInfo[((DWORD)projectileInfoInterface - ARRAY_CProjectileInfo) / sizeof(CProjectileInfoSAInterface)];
}

void CProjectileInfoSA::RemoveProjectile ( CProjectileInfo * pProjectileInfo, CProjectile * pProjectile )
{
    DWORD dwFunc = FUNC_RemoveProjectile;
    CProjectileInfoSAInterface * projectileInfoInterface = ((CProjectileInfoSA*)pProjectileInfo)->internalInterface;

    CProjectileSA* pProjectileSA = dynamic_cast < CProjectileSA* > ( pProjectile );
	if ( !pProjectileSA ) return;

    CEntitySAInterface * projectileInterface = pProjectileSA->GetInterface();

    // Check that this infact is a CProjectile
    // This is perhaps the fix for a crash where it jumps to 0x42480000
    // The proper cause should be figured out instead though as this is a rather unsafe fix.
    if ( (DWORD) projectileInterface->vtbl == VTBL_CProjectile )
    {
        // Has it not already been removed by GTA?
        if ( pProjectileInfo->IsActive () )
        {
            _asm
            {
                push    projectileInterface
                push    projectileInfoInterface
                call    dwFunc
                add     esp, 8
            }
        }
    }
}

CProjectileInfo * CProjectileInfoSA::GetNextFreeProjectileInfo ( )
{
    for ( int i = 0; i < PROJECTILE_INFO_COUNT; i++ )
    {
        if ( projectileInfo[i]->internalInterface->dwProjectileType == 0 )
            return projectileInfo[i];
    }
    return NULL;
}

CProjectileInfo * CProjectileInfoSA::GetProjectileInfo ( DWORD dwIndex )
{
    return projectileInfo[dwIndex];
}

/**
 * Creates a projectile based on an entity's position at a specified offset from the entity's position (VC)
 * @param creator The entity that is emmitting the projectile
 * @param eWeapon The type of projectile
 * @param vecOffset How far away from the creator entity the projectile is created
 * @param fForce How fast the projectile is moving when it is created
 * \note The projectile is created at the same angle as the creator entity. If the creator entity is the player
 * and the camera is in standard 3rd person mode, then the camera's rotation is used as the basis for the angle.
 * SA: public: static bool __cdecl CProjectileInfo::AddProjectile(class CEntity *,enum eWeaponType,class CVector,float,class CVector *,class CEntity *)
 */


bool CProjectileInfoSA::AddProjectile ( CEntity * creator, eWeaponType eWeapon, CVector vecOrigin, float fForce, CVector * target, CEntity * targetEntity )
{
	DEBUG_TRACE("bool CProjectileInfoSA::AddProjectile ( CEntity * creator, eWeaponType eWeapon, CVector vecOffset, float fForce )");
	
	DWORD dwFunction = FUNC_AddProjectile;
	DWORD dwReturn = 0;
    CEntitySAInterface * creatorVC = NULL;
    if ( creator != NULL )   
    {
        CEntitySA* pCreatorSA = dynamic_cast < CEntitySA* > ( creator );
		if ( pCreatorSA )
		{
			creatorVC = pCreatorSA->GetInterface();
			pGame->GetWorld()->IgnoreEntity(creator);
		}
    }

    CEntitySAInterface * targetVC = NULL;
    
    if ( targetEntity != NULL )
    {
        CEntitySA* pTargetEntitySA = dynamic_cast < CEntitySA* > ( targetEntity );
		if ( pTargetEntitySA )
			targetVC = pTargetEntitySA->GetInterface ();
    }

    
	_asm
	{
		push	eax
		    
        push    targetVC
        push    target
		push	fForce
		lea		eax, vecOrigin
		push	[eax+8]
		push	[eax+4]
		push	[eax]
		push	eWeapon
		push	creatorVC
		call	dwFunction
        add     esp, 32
		mov		dwReturn, eax

		pop		eax
	}
	pGame->GetWorld()->IgnoreEntity(NULL);
	return dwReturn != 0;
}

CEntity* CProjectileInfoSA::GetTarget ( void )
{
    CEntitySAInterface* pTargetInterface = internalInterface->pEntProjectileTarget;
    CEntity* pTarget = NULL;
    if ( pTargetInterface )
    {
        switch ( pTargetInterface->nType )
        {
            case ENTITY_TYPE_PED:
            {
                pTarget = pGame->GetPools ()->GetPed ( (DWORD*) pTargetInterface );
                break;
            }

            case ENTITY_TYPE_VEHICLE:
            {
                pTarget = pGame->GetPools ()->GetVehicle ( (CVehicleSAInterface*) pTargetInterface );
                break;
            }

            case ENTITY_TYPE_OBJECT:
            {
                //pTarget = pGame->GetPools ()->GetObject ( (CObjectSAInterface*) pExplodingEntityInterface );
                break;
            }
        }
    }
    return pTarget;
}


void CProjectileInfoSA::SetTarget ( CEntity* pEntity )
{
    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );
	if ( pEntitySA )
		internalInterface->pEntProjectileTarget = pEntitySA->GetInterface ();
}


bool CProjectileInfoSA::IsActive ( void )
{
    return ( internalInterface->bProjectileActive == 1 );
}