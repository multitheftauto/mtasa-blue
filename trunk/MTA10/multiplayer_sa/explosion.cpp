/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/explosion.cpp
*  PURPOSE:     explosion modification
*  DEVELOPERS:  eAi <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define HOOKPOS_CExplosion_AddExplosion     0x736A50

void HOOK_CExplosion_AddExplosion();

bool m_bExplosionsDisabled;
ExplosionHandler * m_pExplosionHandler; // stores our handler

void explosion_init ( void )
{
    m_bExplosionsDisabled = false;
    m_pExplosionHandler = NULL;

    HookInstall(HOOKPOS_CExplosion_AddExplosion, (DWORD)HOOK_CExplosion_AddExplosion, 6);
}

eExplosionType explosionType;
CVector vecExplosionLocation;
DWORD explosionCreator = 0;
DWORD explosionEntity = 0;

bool CallExplosionHandler ( void )
{
    // Find out who the creator is
    CEntity* pExplosionCreator = NULL;
    CEntity* pExplodingEntity = NULL;
    CEntitySAInterface* pInterface = (CEntitySAInterface*) explosionCreator;
    CEntitySAInterface* pExplodingEntityInterface = (CEntitySAInterface*) explosionEntity;

    if ( pInterface )
    {
        // See what type it is and grab the SA interface depending on type
        switch ( pInterface->nType )
        {
            case ENTITY_TYPE_PED:
            {
                pExplosionCreator = pGameInterface->GetPools ()->GetPed ( (DWORD*) pInterface );
                break;
            }

            case ENTITY_TYPE_VEHICLE:
            {
                pExplosionCreator = pGameInterface->GetPools ()->GetVehicle ( (DWORD*) pInterface );
                break;
            }

            case ENTITY_TYPE_OBJECT:
            {
                //pExplosionCreator = pGameInterface->GetPools ()->GetObject ( (DWORD*) pInterface );
                break;
            }
        }
    }

    if ( pExplodingEntityInterface )
    {
        // See what type it is and grab the SA interface depending on type
        switch ( pExplodingEntityInterface->nType )
        {
            case ENTITY_TYPE_PED:
            {
                pExplodingEntity = dynamic_cast < CEntity * > ( pGameInterface->GetPools ()->GetPed ( (DWORD *) pExplodingEntityInterface ) );
                break;
            }

            case ENTITY_TYPE_VEHICLE:
            {
                pExplodingEntity = dynamic_cast < CEntity * > ( pGameInterface->GetPools ()->GetVehicle ( (DWORD *) pExplodingEntityInterface ) );
                break;
            }

            case ENTITY_TYPE_OBJECT:
            {
                //pExplodingEntity = pGameInterface->GetPools ()->GetObject ( (CObjectSAInterface*) pExplodingEntityInterface );
                break;
            }
        }
    }

	return m_pExplosionHandler ( pExplodingEntity, pExplosionCreator, vecExplosionLocation, explosionType );
}

void _declspec(naked) HOOK_CExplosion_AddExplosion()
{
    _asm
    {
        // Check if explosions are disabled.
        push        eax
        mov         al, m_bExplosionsDisabled
        test        al, al
        pop         eax
        jz          checkexplosionhandler

        // If they are, just return now
        retn
        
        // Check the explosion handler. So we can call it if it exists. Jump over the explosion
        // handler part if we have none
        checkexplosionhandler:
        push        eax
        mov         eax, m_pExplosionHandler
        test        eax, eax
        pop         eax
        jz          noexplosionhandler

        // Extract arguments....
		push	esi
		push	edi

        mov     esi, [esp+12]
        mov     explosionEntity, esi

        mov     esi, [esp+16]
        mov     explosionCreator, esi

		mov		esi, [esp+20]
		mov		explosionType, esi

		lea		edi, vecExplosionLocation
		mov		esi, esp
		add		esi, 24 // 3 DWORDS and RETURN address and 2 STORED REGISTERS
		movsd
		movsd
		movsd

		pop		edi
		pop		esi

        // Store registers for calling this handler
		pushad
    }

    // Call the explosion handler
	if ( !CallExplosionHandler () )
	{
		_asm	popad
		_asm	retn // if they return false from the handler, they don't want the explosion to show
	}
    else
    {
		_asm popad
	}

	_asm
	{
        noexplosionhandler:

        // Replaced code
		sub		esp, 0x1C
		push	ebx
		push	ebp
		push	esi

        // Return to the calling function and resume (do the explosion)
		mov		edx, HOOKPOS_CExplosion_AddExplosion
		add		edx, 6
		jmp		edx
	}
}