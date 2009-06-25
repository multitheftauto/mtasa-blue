/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CEventList.h
*  PURPOSE:		Event list interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_EVENTLIST
#define __CGAME_EVENTLIST

#include "Common.h"
#include "CEvent.h"
#include <CVector.h>

class CEventDamage;
class CEventDamageSAInterface;

class CEventList
{
public:
    virtual CEvent			* FindClosestEvent ( eEventType eventtype, CVector * vecPosition ) = 0;
	virtual CEvent			* GetEvent ( DWORD ID ) = 0;
	virtual CEvent			* GetEvent ( eEventType eventtype ) = 0;

    virtual CEventDamage    * GetEventDamage ( CEventDamageSAInterface * pInterface ) = 0;
    virtual CEventDamage    * CreateEventDamage ( CEntity * pEntity, unsigned int i_1, eWeaponType weaponType, ePedPieceTypes hitZone, unsigned char uc_2, bool b_3, bool b_4 ) = 0;
};

#endif