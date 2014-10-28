/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamSector.h
*  PURPOSE:     Stream sector class header
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CClientStreamSector_H
#define __CClientStreamSector_H

#include <list>
#include "CClientCommon.h"

class CClientStreamer;
class CClientStreamElement;
class CClientStreamSectorRow;

class CClientStreamSector
{
    friend class CClientStreamer;
    friend class CClientStreamSectorRow;
public:
                                                    CClientStreamSector     ( CClientStreamSectorRow * pRow, CVector2D & vecBottomLeft, CVector2D & vecTopRight );
                                                    ~CClientStreamSector    ( void );

    bool                                            DoesContain             ( CVector & vecPosition );
    bool                                            DoesContain             ( float fX );
    void                                            GetSurroundingSectors   ( CClientStreamSector ** pArray );
    bool                                            IsMySurroundingSector   ( CClientStreamSector * pSector );
    void                                            CompareSurroundings     ( CClientStreamSector * pSector, std::list < CClientStreamSector * > * pCommon, std::list < CClientStreamSector * > * pUncommon, bool bIncludeCenter );

    void                                            Add                     ( CClientStreamElement * pElement )     { m_Elements.push_back ( pElement ); }
    void                                            Remove                  ( CClientStreamElement * pElement )     { m_Elements.remove ( pElement ); }
    std::list < CClientStreamElement * > ::iterator Begin                   ( void )    { return m_Elements.begin (); }
    std::list < CClientStreamElement * > ::iterator End                     ( void )    { return m_Elements.end (); }

    void                                            AddElements             ( std::list < CClientStreamElement * > * pList );
    void                                            RemoveElements          ( std::list < CClientStreamElement * > * pList );
    unsigned int                                    CountElements           ( void )    { return m_Elements.size (); }

    CClientStreamSectorRow *                        GetRow                  ( void )    { return m_pRow; }
    void                                            GetCorners              ( CVector2D & vecBottomLeft, CVector2D & vecTopRight )      { vecBottomLeft = m_vecBottomLeft; vecTopRight = m_vecTopRight; }

private:
    bool                                            IsActivated             ( void )            { return m_bActivated; }
    void                                            SetActivated            ( bool bActivated ) { m_bActivated = bActivated; }
    
    bool                                            IsExtra                 ( void )            { return m_bExtra; }
    void                                            SetExtra                ( bool bExtra )     { m_bExtra = bExtra; }

    CVector2D                                       m_vecBottomLeft;
    CVector2D                                       m_vecTopRight;
    std::list < CClientStreamElement * >            m_Elements;
    CClientStreamSector                             * m_pLeft, * m_pRight, * m_pTop, * m_pBottom;
    CClientStreamSectorRow                          * m_pRow;
    bool                                            m_bActivated;
    bool                                            m_bExtra;

    class CClientRadarArea *                        m_pArea;
};

#endif