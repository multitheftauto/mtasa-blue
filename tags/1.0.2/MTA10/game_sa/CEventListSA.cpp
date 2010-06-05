/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CEventListSA.cpp
*  PURPOSE:		Event list
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CEvent * CEventListSA::FindClosestEvent ( eEventType eventtype, CVector * vecPosition )
{
	DEBUG_TRACE("CEvent * CEventListSA::FindClosestEvent ( eEventType eventtype, CVector * vecPosition )");
	CEventSA * event;

	CVector * vecPos = (CVector *)vecPosition;
		
	DWORD dwFunction = FUNC_FindClosestEvent;
	FLOAT fX = vecPos->fX;
	FLOAT fY = vecPos->fY;
	FLOAT fZ = vecPos->fZ;
	DWORD dwReturn;
	_asm
	{
		push	eax
		lea		eax, dwReturn
		push	eax
		push	fZ
		push	fY
		push	fX
		push	eventtype
		call	dwFunction
		add		esp, 0x14
		pop		eax
	}
	
	event = (CEventSA *)(ARRAY_CEvent + dwReturn * sizeof(CEventSA));
	return event;
}

CEvent * CEventListSA::GetEvent ( DWORD ID )
{	
	DEBUG_TRACE("CEvent * CEventListSA::GetEvent ( DWORD ID )");
	CEventSA * event;
	event = (CEventSA *)(ARRAY_CEvent + ID * sizeof(CEventSA));
	return event;
}

CEvent * CEventListSA::GetEvent ( eEventType eventtype )
{
	DEBUG_TRACE("CEvent * CEventListSA::GetEvent ( eEventType eventtype )");
	CEventSA * event;
	for(int i = 0;i<MAX_EVENTS;i++)
	{
		event = (CEventSA *)(ARRAY_CEvent + i * sizeof(CEventSA));
		if(event->GetType() == eventtype)
			return event;
	}
	return NULL;
}


CEventDamage * CEventListSA::GetEventDamage ( CEventDamageSAInterface * pInterface )
{
    return new CEventDamageSA ( pInterface );
}


CEventDamage * CEventListSA::CreateEventDamage ( CEntity * pEntity, unsigned int i_1, eWeaponType weaponType, ePedPieceTypes hitZone, unsigned char uc_2, bool b_3, bool b_4 )
{        
    return new CEventDamageSA ( pEntity, i_1, weaponType, hitZone, uc_2, b_3, b_4 );
}


CEventGunShot * CEventListSA::CreateEventGunShot ( CEntity * pEntity, CVector & vecOrigin, CVector & vecTarget, bool b_1 )
{
    return new CEventGunShotSA ( pEntity, vecOrigin, vecTarget, b_1 );
}