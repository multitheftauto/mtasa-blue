/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CEventSA.cpp
*  PURPOSE:		Base event
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

eEventEntityType CEventSA::GetType (  )
{
	DEBUG_TRACE("eEventEntityType CEventSA::GetType (  )");
	return internalInterface->m_eEventType;
}

CEntity	* CEventSA::GetEntity (  )
{
	DEBUG_TRACE("CEntity * CEventSA::GetEntity (  )");
	if(this->GetEntityType() == EVENT_NULL)
		return NULL;

	CPoolsSA * pPools = (CPoolsSA *)pGame->GetPools();
	CEntity * entity;		

	switch(internalInterface->m_eEventEntity)
	{
	case ENTITY_EVENT_PED:
		entity = (CEntity *)pPools->GetPedFromRef((DWORD)internalInterface->m_nEventEntityRef);
		break;
	case ENTITY_EVENT_VEHICLE:
		entity = (CEntity *)pPools->GetVehicleFromRef((DWORD)internalInterface->m_nEventEntityRef);
		break;
	case ENTITY_EVENT_OBJECT:
		entity = (CEntity *)pPools->GetObjectFromRef((DWORD)internalInterface->m_nEventEntityRef);
		break;
	default:
		return NULL;
	}
	return entity;
}

eEventEntity CEventSA::GetEntityType (  )
{
	DEBUG_TRACE("eEventEntity CEventSA::GetEntityType (  )");
	return internalInterface->m_eEventEntity;
}

CEntity * CEventSA::GetCommittedBy (  )
{
	DEBUG_TRACE("CEntity * CEventSA::GetCommittedBy (  )");
	return internalInterface->m_pCommitedBy;
}

CVector * CEventSA::GetPosition (  )
{
	DEBUG_TRACE("CVector * CEventSA::GetPosition (  )");
	return &internalInterface->m_vecEventPosition;
}
