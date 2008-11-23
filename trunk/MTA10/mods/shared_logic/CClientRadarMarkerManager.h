/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarMarkerManager.h
*  PURPOSE:     Radar marker entity manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*
*****************************************************************************/

class CClientRadarMarkerManager;

#ifndef __CCLIENTRADARMARKERMANAGER_H
#define __CCLIENTRADARMARKERMANAGER_H

#include "CClientRadarMarker.h"
#include <list>

using namespace std;

class CClientRadarMarkerManager
{
    friend class CClientRadarMarker;

public:
                                                            CClientRadarMarkerManager       ( class CClientManager* pManager );
                                                            ~CClientRadarMarkerManager      ( void );

    void                                                    DoPulse                         ( void );

    void                                                    DeleteAll                       ( void );

    static CClientRadarMarker*                              Get                             ( ElementID ID );
    
    inline unsigned short                                   GetDimension                    ( void )                                    { return m_usDimension; }
    void                                                    SetDimension                    ( unsigned short usDimension );

    inline list < CClientRadarMarker* > ::const_iterator    IterBegin                       ( void )                                    { return m_Markers.begin (); };
    inline list < CClientRadarMarker* > ::const_iterator    IterEnd                         ( void )                                    { return m_Markers.end (); };

    bool                                                    Exists                          ( CClientRadarMarker* pMarker );

private:
    inline void                                             AddToList                       ( CClientRadarMarker* pMarker )             { m_Markers.push_back ( pMarker ); };
    void                                                    RemoveFromList                  ( CClientRadarMarker* pMarker );
    
    void                                                    OrderMarkers                    ( void );
    static bool                                             CompareOrderingIndex            ( CClientRadarMarker * p1, CClientRadarMarker * p2 );

    class CClientManager*                                   m_pManager;
    bool                                                    m_bCanRemoveFromList;
    list < CClientRadarMarker* >                            m_Markers;

    unsigned short                                          m_usDimension;
    bool                                                    m_bOrderOnPulse;
    CVector                                                 m_vecCameraPosition;
};

#endif
