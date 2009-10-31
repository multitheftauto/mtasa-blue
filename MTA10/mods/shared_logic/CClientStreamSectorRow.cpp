/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamSectorRow.cpp
*  PURPOSE:     Stream sector row class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::vector;

CClientStreamSectorRow::CClientStreamSectorRow ( float fBottom, float fTop )
{
    m_fBottom = fBottom;
    m_fTop = fTop;
    m_bExtra = false;
    m_pTop = NULL; m_pBottom = NULL;
}


CClientStreamSectorRow::~CClientStreamSectorRow ( void )
{
    // Clear our sectors
    vector < CClientStreamSector * > ::iterator iter = m_Sectors.begin ();
    for ( ; iter != m_Sectors.end () ; iter++ )
    {
        delete *iter;
    }
    m_Sectors.clear ();
}


void CClientStreamSectorRow::Add ( CClientStreamSector * pSector )
{
    m_Sectors.push_back ( pSector );
    pSector->m_pRow = this;
}


void CClientStreamSectorRow::Remove ( CClientStreamSector * pSector )
{
    vector < CClientStreamSector * > ::iterator iter = m_Sectors.begin ();
    for ( ; iter != m_Sectors.end () ; iter++ )
    {
        if ( *iter == pSector )
        {
            m_Sectors.erase ( iter );
            break;
        }
    }
}


bool CClientStreamSectorRow::DoesContain ( CVector & vecPosition )
{
    return ( vecPosition.fY >= m_fBottom && vecPosition.fY < m_fTop );
}


bool CClientStreamSectorRow::DoesContain ( float fY )
{
    return ( fY >= m_fBottom && fY < m_fTop );
}


CClientStreamSector * CClientStreamSectorRow::FindOrCreateSector ( CVector & vecPosition, CClientStreamSector * pSurrounding )
{
    // Do we have a sector to check around first?
    if ( pSurrounding )
    {
        // Only check left and right, because we know its in the same row
        if ( pSurrounding->m_pLeft && pSurrounding->m_pLeft->DoesContain ( vecPosition ) )
            return pSurrounding->m_pLeft;
        if ( pSurrounding->m_pRight && pSurrounding->m_pRight->DoesContain ( vecPosition ) )
            return pSurrounding->m_pRight;
    }
    
    // Search through our row of sectors
    CClientStreamSector * pSector = NULL;
    vector < CClientStreamSector * > ::iterator iter = m_Sectors.begin ();
    for ( ; iter != m_Sectors.end () ; iter++ )
    {
        pSector = *iter;
        if ( pSector->DoesContain ( vecPosition.fX ) )
        {
            return pSector;
        }
    }
    
    // We need a new row, align it with the others
    float fLeft = float ( ( int ) ( vecPosition.fX / SECTOR_SIZE ) ) * SECTOR_SIZE;
    if ( vecPosition.fX < 0.0f ) fLeft -= SECTOR_SIZE;
    CVector2D vecBottomLeft ( fLeft, m_fBottom );
    CVector2D vecTopRight ( vecBottomLeft.fX + SECTOR_SIZE, vecBottomLeft.fY + ROW_SIZE );
    pSector = new CClientStreamSector ( this, vecBottomLeft, vecTopRight );
    ConnectSector ( pSector );
    pSector->SetExtra ( true );
    m_Sectors.push_back ( pSector );
    
    return pSector;
}


CClientStreamSector * CClientStreamSectorRow::FindSector ( float fX )
{    
    // Search through our row of sectors
    CClientStreamSector * pSector = NULL;
    vector < CClientStreamSector * > ::iterator iter = m_Sectors.begin ();
    for ( ; iter != m_Sectors.end () ; iter++ )
    {
        pSector = *iter;
        if ( pSector->DoesContain ( fX ) )
        {
            return pSector;
        }
    }

    return NULL;
}


void CClientStreamSectorRow::ConnectSector ( CClientStreamSector * pSector )
{
    // Connect up our sector..
    CVector2D vecBottomLeft, vecTopRight;
    pSector->GetCorners ( vecBottomLeft, vecTopRight );

    // Grab our left and right sectors from the same row
    pSector->m_pLeft = FindSector ( vecBottomLeft.fX - ( SECTOR_SIZE / 2.0f ) );
    pSector->m_pRight = FindSector ( vecTopRight.fX + ( SECTOR_SIZE / 2.0f ) );

    // Grab our top and bottom sectors from the row below and above our own
    if ( m_pTop ) pSector->m_pTop = m_pTop->FindSector ( vecBottomLeft.fX );
    if ( m_pBottom ) pSector->m_pBottom = m_pBottom->FindSector ( vecBottomLeft.fX );

    // Connect the other sectors to us
    if ( pSector->m_pLeft ) pSector->m_pLeft->m_pRight = pSector;
    if ( pSector->m_pRight ) pSector->m_pRight->m_pLeft = pSector;
    if ( pSector->m_pTop ) pSector->m_pTop->m_pBottom = pSector;
    if ( pSector->m_pBottom ) pSector->m_pBottom->m_pTop = pSector;
}