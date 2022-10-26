/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventSA.cpp
 *  PURPOSE:     Base event
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEventSA.h"
#include "CGameSA.h"
#include "CPoolsSA.h"

extern CGameSA* pGame;

eEventEntityType CEventSA::GetType()
{
    return m_pInterface->m_eEventType;
}

CEntity* CEventSA::GetEntity()
{
    if (this->GetEntityType() == EVENT_NULL)
        return NULL;

    CPoolsSA* pPools = (CPoolsSA*)pGame->GetPools();
    CEntity*  entity;

    switch (m_pInterface->m_eEventEntity)
    {
        case ENTITY_EVENT_PED:
            entity = (CEntity*)pPools->GetPedFromRef((DWORD)m_pInterface->m_nEventEntityRef);
            break;
        case ENTITY_EVENT_VEHICLE:
            entity = (CEntity*)pPools->GetVehicleFromRef((DWORD)m_pInterface->m_nEventEntityRef);
            break;
        case ENTITY_EVENT_OBJECT:
            entity = (CEntity*)pPools->GetObjectFromRef((DWORD)m_pInterface->m_nEventEntityRef);
            break;
        default:
            return NULL;
    }
    return entity;
}

eEventEntity CEventSA::GetEntityType()
{
    return m_pInterface->m_eEventEntity;
}

CEntity* CEventSA::GetCommittedBy()
{
    return m_pInterface->m_pCommitedBy;
}

CVector* CEventSA::GetPosition()
{
    return &m_pInterface->m_vecEventPosition;
}
