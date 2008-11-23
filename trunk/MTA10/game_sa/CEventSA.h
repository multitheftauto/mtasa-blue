/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CEventSA.cpp
*  PURPOSE:		Header file for base event class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_EVENT
#define __CGAMESA_EVENT

#include <game/CEvent.h>

#include "Common.h"
#include "CEntitySA.h"
#include "CPoolsSA.h"

class CEventSAInterface
{
public:
	eEventEntityType	m_eEventType;			// What type of event is it?
	eEventEntity 		m_eEventEntity;			// What type of entity does the event relate to?
	DWORD				m_nEventEntityRef;		// Unique reference to the event entity. Can be used to
											// get a pointer to the event entity.
	CEntity				* m_pCommitedBy;		// for crimes, keeps track of who commited the crime
	CVector			m_vecEventPosition;		// Position of an event. Can register an event using just
	DWORD				m_nEventExpiryTime;		// a type and position rather than a type and an entity
	DWORD				m_nEventRemoveFrame;	// Time when this event will be removed from the event list
};

class CEventSA : public CEvent
{
//private:
	CEventSAInterface			* internalInterface;
public:
	eEventEntityType	GetType (  );
	CEntity				* GetEntity (  );
	eEventEntity		GetEntityType (  );
	CEntity				* GetCommittedBy (  );
	CVector			* GetPosition (  );
};

#endif