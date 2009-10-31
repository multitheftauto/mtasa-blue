/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamer.h
*  PURPOSE:     Streamer class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Ed Lyons <eai@opencoding.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#ifndef __CClientStreamer_H
#define __CClientStreamer_H

#include "CClientCommon.h"
#include <list>
#include <vector>
class CClientStreamSector;
class CClientStreamSectorRow;
class CClientStreamElement;
typedef bool ( StreamerLimitReachedFunction ) ( void );


class CClientStreamer
{
    friend CClientStreamElement;
public:
                                            CClientStreamer             ( StreamerLimitReachedFunction* pLimitReachedFunc, float fMaxDistance );
                                            ~CClientStreamer            ( void );

    void                                    SetupSectors                ( void );
    
    static void                             UpdateSectors               ( CVector & vecPosition );
    static void                             OnEnterSector               ( CClientStreamSector * pSector );

    void                                    DoPulse                     ( CVector & vecPosition );
    void                                    SetDimension                ( unsigned short usDimension );

    static bool                             CompareExpDistance          ( CClientStreamElement* p1, CClientStreamElement* p2 );

    unsigned int                            CountActiveElements         ( void )    { return ( unsigned int ) m_ActiveElements.size (); }
    bool                                    IsActiveElement             ( CClientStreamElement * pElement );
    std::list < CClientStreamElement * > ::iterator  ActiveElementsBegin( void )    { return m_ActiveElements.begin (); }
    std::list < CClientStreamElement * > ::iterator  ActiveElementsEnd  ( void )    { return m_ActiveElements.end (); }

private:
    static void                             CreateSectors               ( std::vector < CClientStreamSectorRow * > * pList, CVector2D & vecSize, CVector2D & vecBottomLeft, CVector2D & vecTopRight );
    static void                             ConnectSector               ( CClientStreamSector * pSector );
    static void                             ConnectRow                  ( CClientStreamSectorRow * pRow );

    static CClientStreamSectorRow *         FindOrCreateRow             ( CVector & vecPosition, CClientStreamSectorRow * pSurrounding = NULL );
    static CClientStreamSectorRow *         FindRow                     ( float fY );
    
    void                                    OnUpdateStreamPosition      ( CClientStreamElement * pElement );
    
    void                                    AddElement                  ( CClientStreamElement * pElement );
    void                                    RemoveElement               ( CClientStreamElement * pElement );

    void                                    SetExpDistances             ( std::list < CClientStreamElement * > * pList );
    void                                    AddToSortedList             ( std::list < CClientStreamElement * > * pList, CClientStreamElement * pElement );
    
    void                                    Restream                    ( void );
    bool                                    ReachedLimit                ( void )    { return m_pLimitReachedFunc (); }

    void                                    OnSectorChange              ( std::vector < CClientStreamSector * > & activateSectors, std::vector < CClientStreamSector * > & deactivateSectors );

    void                                    OnElementEnterSector        ( CClientStreamElement * pElement, CClientStreamSector * pSector );
    void                                    OnElementForceStreamIn      ( CClientStreamElement * pElement );
    void                                    OnElementForceStreamOut     ( CClientStreamElement * pElement );
    void                                    OnElementDimension          ( CClientStreamElement * pElement );

    float                                   m_fMaxDistanceExp;
    float                                   m_fMaxDistanceThreshold;
    StreamerLimitReachedFunction*           m_pLimitReachedFunc;
    std::list < CClientStreamElement * >    m_ActiveElements;
    std::list < CClientStreamElement * >    m_ToStreamOut;
    CVector                                 m_vecPosition;
    unsigned short                          m_usDimension;

    static std::vector < CClientStreamer * > m_Streamers;
    static std::vector < CClientStreamSectorRow * >  m_WorldRows;
    static std::vector < CClientStreamSectorRow * >  m_ExtraRows;
    static CClientStreamSectorRow *         m_pRow;
    static CClientStreamSector *            m_pSector;
    static CVector                          m_vecSectorPosition;
};

#endif
