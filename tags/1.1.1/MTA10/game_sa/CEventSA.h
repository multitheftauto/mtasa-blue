/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEventSA.h
*  PURPOSE:     Header file for base event class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_EVENT
#define __CGAMESA_EVENT

#include <game/CEvent.h>

class CEventSAInterface
{
public:
    eEventEntityType    m_eEventType;           // What type of event is it?
    eEventEntity        m_eEventEntity;         // What type of entity does the event relate to?
    DWORD               m_nEventEntityRef;      // Unique reference to the event entity. Can be used to
                                                // get a pointer to the event entity.
    CEntity             * m_pCommitedBy;        // for crimes, keeps track of who commited the crime
    CVector             m_vecEventPosition;     // Position of an event. Can register an event using just
    DWORD               m_nEventExpiryTime;     // a type and position rather than a type and an entity
    DWORD               m_nEventRemoveFrame;    // Time when this event will be removed from the event list
};

class CEventSA : public CEvent
{
public:
                                CEventSA            ( CEventSAInterface * pInterface )  { m_pInterface = pInterface; }

    CEventSAInterface *         GetInterface        ( void )    { return m_pInterface; }

    eEventEntityType            GetType             ( void );
    CEntity *                   GetEntity           ( void );
    eEventEntity                GetEntityType       ( void );
    CEntity *                   GetCommittedBy      ( void );
    CVector *                   GetPosition         ( void );

protected:
    CEventSAInterface *         m_pInterface;
};

#endif
