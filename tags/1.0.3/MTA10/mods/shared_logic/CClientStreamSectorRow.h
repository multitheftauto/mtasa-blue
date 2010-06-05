/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamSectorRow.h
*  PURPOSE:     Stream sector row class header
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CClientStreamSectorRow_H
#define __CClientStreamSectorRow_H

#define SECTOR_SIZE         300.0f		// was 300.0f
#define ROW_SIZE            300.0f		// was 300.0f
#define WORLD_SIZE          6000.0f		// was 3000.0f

#include "CClientCommon.h"
#include <list>
class CClientStreamSector;
class CClientStreamer;

class CClientStreamSectorRow
{
    friend CClientStreamer;
public:
                                                    CClientStreamSectorRow      ( float fBottom, float fTop );
                                                    ~CClientStreamSectorRow     ( void );

    std::list < CClientStreamSector * > ::iterator  Begin                       ( void )                            { return m_Sectors.begin (); }
    std::list < CClientStreamSector * > ::iterator  End                         ( void )                            { return m_Sectors.end (); }
    CClientStreamSector *                           Front                       ( void )                            { return m_Sectors.front (); }
    void                                            Add                         ( CClientStreamSector * pSector );
    void                                            Remove                      ( CClientStreamSector * pSector );
    unsigned int                                    CountSectors                ( void )                            { return m_Sectors.size (); }

    bool                                            DoesContain                 ( CVector & vecPosition );
    bool                                            DoesContain                 ( float fY );

    CClientStreamSector *                           FindOrCreateSector          ( CVector & vecPosition, CClientStreamSector * pSurrounding = NULL );
    CClientStreamSector *                           FindSector                  ( float fX );

    void                                            ConnectSector               ( CClientStreamSector * pSector );

    void                                            GetPosition                 ( float & fTop, float & fBottom )   { fTop = m_fTop; fBottom = m_fBottom; }
private:
    bool                                            IsExtra                     ( void )            { return m_bExtra; }
    void                                            SetExtra                    ( bool bExtra )     { m_bExtra = bExtra; }

    float                                           m_fBottom, m_fTop;
    std::list < CClientStreamSector * >             m_Sectors;
    bool                                            m_bExtra;
    CClientStreamSectorRow                          * m_pTop, * m_pBottom;
};

#endif