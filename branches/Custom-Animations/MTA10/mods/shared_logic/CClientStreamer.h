/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamer.h
*  PURPOSE:     Streamer class header
*  DEVELOPERS:  
*
*****************************************************************************/

#ifndef __CClientStreamer_H
#define __CClientStreamer_H

#include "CClientCommon.h"
#include <list>

typedef bool ( StreamerLimitReachedFunction ) ( void );

class CClientStreamer
{
    friend CClientStreamElement;
public:
                                            CClientStreamer             ( StreamerLimitReachedFunction* pLimitReachedFunc, float fMaxDistance, eClientEntityType entityType, unsigned int uiMaxCount );
                                            ~CClientStreamer            ( void );
    
    void                                    DoPulse                     ( const CVector& vecPosition );
    void                                    SetDimension                ( unsigned short usDimension );

    void                                    OnUpdateStreamPosition      ( CClientStreamElement * pElement );
    
    void                                    AddElement                  ( CClientStreamElement * pElement );
    void                                    RemoveElement               ( CClientStreamElement * pElement );
    void                                    OnElementForceStreamIn      ( CClientStreamElement * pElement );
    void                                    OnElementUnforceStreamIn    ( CClientStreamElement * pElement );
    void                                    OnElementDimension          ( CClientStreamElement * pElement );
    void                                    DoStreamOut                 ( CClientStreamElement * pElement );
    void                                    DoStreamIn                  ( CClientStreamElement * pElement );
    bool                                    IsStreamedIn                ( CClientStreamElement * pElement );
    bool                                    ReachedLimit                ( void );

    void                                    NotifyElementStreamedIn     ( CClientStreamElement * pElement );
    void                                    NotifyElementStreamedOut    ( CClientStreamElement * pElement );

private:
    eClientEntityType                       m_EntityType;
    float                                   m_fMaxDistance;
    unsigned short                          m_usDimension;
    unsigned int                            m_uiMaxCount;
    StreamerLimitReachedFunction*           m_pLimitReachedFunc;

    std::map < CClientStreamElement*, int > m_StreamedInMap;
    std::map < CClientStreamElement*, int > m_ForcedInMap;

    std::vector < CClientStreamElement* >   elementList;

    static void*                            pAddingElement;
};


#endif
