/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarAreaManager.h
*  PURPOSE:     Radar area entity manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Chris McArthur <>
*
*****************************************************************************/

#ifndef __CCLIENTRADARAREAMANAGER_H
#define __CCLIENTRADARAREAMANAGER_H

#include "CClientRadarArea.h"
#include <list>

using namespace std;

class CClientRadarAreaManager
{
    friend class CClientManager;
    friend class CClientRadarArea;
    friend class CClientGame;

public:
                                                            CClientRadarAreaManager             ( CClientManager* pManager );
                                                            ~CClientRadarAreaManager            ( void );

    CClientRadarArea*                                       Create                              ( ElementID ID );

    void                                                    Delete                              ( CClientRadarArea* pRadarArea );
    void                                                    DeleteAll                           ( void );

    inline list < CClientRadarArea* > ::const_iterator      IterBegin                           ( void )                                { return m_List.begin (); };
    inline list < CClientRadarArea* > ::const_iterator      IterEnd                             ( void )                                { return m_List.end (); };

    static CClientRadarArea*                                Get                                 ( ElementID ID );

    unsigned short                                          GetDimension                        ( void )                                { return m_usDimension; };
    void                                                    SetDimension                        ( unsigned short usDimension );

private:
    void                                                    DoPulse                             ( void );
	void													DoPulse								( bool bRender );

    inline void                                             AddToList                           ( CClientRadarArea* pRadarArea )        { m_List.push_back ( pRadarArea ); };
    void                                                    RemoveFromList                      ( CClientRadarArea* pRadarArea );

private:
    CClientManager*                                         m_pManager;

    list < CClientRadarArea* >                              m_List;
    bool                                                    m_bDontRemoveFromList;
    unsigned short                                          m_usDimension;

};

#endif
