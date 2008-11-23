/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CExplosionManagerSA.cpp
*  PURPOSE:		Explosion manager
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CExplosionManagerSA::CExplosionManagerSA()
{
	DEBUG_TRACE("CExplosionManagerSA::CExplosionManagerSA()");
	for(int i = 0; i<MAX_EXPLOSIONS;i++)
		Explosions[i] = new CExplosionSA((CExplosionSAInterface *)(ARRAY_Explosions + i * sizeof(CExplosionSAInterface)));
}


CExplosionManagerSA::~CExplosionManagerSA()
{
    for ( int i = 0; i < MAX_EXPLOSIONS; i++ )
    {
        delete Explosions [i];
    }
}


/**
 * \todo Test this, replace with CExplosion::AddExplosion code if possible in order to ensure correct pointer
 */
CExplosion * CExplosionManagerSA::AddExplosion ( eExplosionType explosiontype, CVector * vecPosition, CEntity * creator, bool bMakeSound, float fCamShake, bool bNoDamage )
{
	DEBUG_TRACE("CExplosion * CExplosionManagerSA::AddExplosion ( eExplosionType explosiontype, CVector * vecPosition, CEntity * creator = NULL)");
	CEntitySAInterface * entityInterface  = 0;
	DWORD dwFunction = FUNC_AddExplosion;
	CExplosion * explosion = CExplosionManagerSA::FindFreeExplosion();
	BYTE bSuccess = 0;
	if ( creator )
	{
		CEntitySA * vcEnt = dynamic_cast < CEntitySA* > ( creator );
		if ( vcEnt )
			entityInterface = vcEnt->GetInterface();
	}

	/*
	static bool		AddExplosion(CEntity *pExplodingEntity, CEntity *pEntExplosionOwner, 
	eExplosionType ExplosionType, CVector vecExplosionPosition, unsigned int ActivationDelay=0, 
	unsigned char bMakeSound = true, float fCamShake=-1.0f, unsigned char noDamage=false);
	*/

	FLOAT fX = vecPosition->fX;
	FLOAT fY = vecPosition->fY;
	FLOAT fZ = vecPosition->fZ;

	_asm
	{
		push	bNoDamage
		push	fCamShake
		push	bMakeSound
		push	0
		push	fZ
		push	fY
		push	fX
		push	explosiontype
		push	0
		push	0

		// OUR CALL
		push	returnhere // simulate a call, by pusing our return address
		// NOW the code is actually inside CExplosion__AddExplosion, but may be patched by Multiplayer
		sub		esp, 0x1C
		push	ebx
		push	ebp
		push	esi
		mov		ebx, dwFunction // Now jump in 6 bytes later (6 bytes might be used for our patch-jump in Multiplayer)
		add		ebx, 6
		jmp		ebx
		
returnhere:
		add		esp, 0x28
		mov		bSuccess, al
	}
	if(bSuccess) return explosion;
	return NULL;
}

/**
 * \todo Need to simulate this manually (loop and IsNear...)
 */
VOID CExplosionManagerSA::RemoveAllExplosionsInArea ( CVector * vecPosition, FLOAT fRadius )
{
	DEBUG_TRACE("VOID CExplosionManagerSA::RemoveAllExplosionsInArea ( CVector * vecPosition, FLOAT fRadius )");
	
/*	DWORD dwFunction = FUNC_RemoveAllExplosionsInArea;
	CVector * vecPos = (CVector *)vecPosition;
	FLOAT fX = vecPos->fX;
	FLOAT fY = vecPos->fY;
	FLOAT fZ = vecPos->fZ;
	
	_asm
	{
		push	fRadius
		push	fZ
		push	fY
		push	fX
		call	dwFunction
		add		esp, 0x10
	}*/
}

VOID CExplosionManagerSA::RemoveAllExplosions (  )
{
	DEBUG_TRACE("VOID CExplosionManagerSA::RemoveAllExplosions (  )");
	for(int i = 0; i < MAX_EXPLOSIONS; i++)
		if(Explosions[i]->IsActive()) Explosions[i]->Remove();
}

CExplosion * CExplosionManagerSA::GetExplosion ( DWORD ID )
{
	DEBUG_TRACE("CExplosion * CExplosionManagerSA::GetExplosion ( DWORD ID )");
	return Explosions[ID];
}

CExplosion * CExplosionManagerSA::FindFreeExplosion (  )
{
	DEBUG_TRACE("CExplosion * CExplosionManagerSA::FindFreeExplosion (  )");
	for(int i = 0; i < MAX_EXPLOSIONS; i++)
		if(!Explosions[i]->IsActive()) return Explosions[i];
	return NULL;
}