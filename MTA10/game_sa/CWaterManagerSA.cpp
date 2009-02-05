/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CWaterManagerSA.cpp
*  PURPOSE:		Control the lakes and seas
*  DEVELOPERS:	arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define POLYENTRY_TYPE(entry) ( (entry)->m_wValue >> 14 )
#define POLYENTRY_ID(entry) ( (entry)->m_wValue & 0x3FFF )

CWaterManagerSA* g_pWaterManager = NULL;

// -----------------------------------------------------
// Water zone iterator (iterates over polys in a zone)

CWaterZoneSA::iterator::iterator ()
{
    m_pCurrent = NULL;
    m_pFirst = NULL;
    m_bSinglePoly = false;
}

CWaterZoneSA::iterator::iterator ( CWaterZoneSA* pZone )
{
    if ( POLYENTRY_TYPE ( pZone->GetInterface () ) == WATER_POLY_LIST )
    {
        m_pCurrent = &((CWaterPolyEntrySAInterface *)ARRAY_WaterZonePolys) [ POLYENTRY_ID ( pZone->GetInterface () ) ];
        m_bSinglePoly = false;
    }
    else
    {
        m_pCurrent = pZone->GetInterface ();
        m_bSinglePoly = true;
    }
    m_pFirst = m_pCurrent;
}

CWaterZoneSA::iterator::iterator ( CWaterZoneSA::iterator& other )
{
    operator= ( other );
}

CWaterZoneSA::iterator& CWaterZoneSA::iterator::operator= ( CWaterZoneSA::iterator& other )
{
    m_pCurrent = other.m_pCurrent;
    m_pFirst = other.m_pFirst;
    m_bSinglePoly = other.m_bSinglePoly;
    return *this;
}

void CWaterZoneSA::iterator::operator++ ()
{
    m_pCurrent++;
}

void CWaterZoneSA::iterator::operator-- ()
{
    m_pCurrent--;
}

bool CWaterZoneSA::iterator::operator== ( CWaterZoneSA::iterator& other )
{
    return m_pCurrent == other.m_pCurrent;
}

bool CWaterZoneSA::iterator::operator!= ( CWaterZoneSA::iterator& other )
{
    return m_pCurrent != other.m_pCurrent;
}

CWaterPolySA* CWaterZoneSA::iterator::operator* ()
{
    if ( (m_bSinglePoly && m_pCurrent != m_pFirst) || m_pCurrent->m_wValue == NULL )
        return NULL;

    if ( POLYENTRY_TYPE ( m_pCurrent ) == WATER_POLY_QUAD )
    {
        return &g_pWaterManager->m_Quads [ POLYENTRY_ID ( m_pCurrent ) ];
    }
    else if ( POLYENTRY_TYPE ( m_pCurrent ) == WATER_POLY_TRIANGLE )
    {
        return &g_pWaterManager->m_Triangles [ POLYENTRY_ID ( m_pCurrent ) ];
    }
    return NULL;
}

CWaterZoneSA::iterator CWaterZoneSA::begin ()
{
    return iterator ( this );
}

// -----------------------------------------------------
// Vertices

void CWaterVertexSA::GetPosition ( CVector& vec )
{
    vec.fX = (float)m_pInterface->m_sX;
    vec.fY = (float)m_pInterface->m_sY;
    vec.fZ = m_pInterface->m_fZ;
}

void CWaterVertexSA::SetPosition ( CVector& vec, void* pChangeSource )
{
    if ( pChangeSource )
        g_pWaterManager->AddChange ( pChangeSource, this, new CWaterChangeVertexMove ( this ) );
    m_pInterface->m_sX = (short)vec.fX;
    m_pInterface->m_sY = (short)vec.fY;
    m_pInterface->m_fZ = vec.fZ;
}

// -----------------------------------------------------
// Polygons

void CWaterQuadSA::SetInterface ( CWaterPolySAInterface* pInterface )
{
    m_pInterface = pInterface;
    m_wID = (WORD) ( ((DWORD)pInterface - ARRAY_WaterQuads) / sizeof ( CWaterQuadSAInterface ) );
}

void CWaterTriangleSA::SetInterface ( CWaterPolySAInterface* pInterface )
{
    m_pInterface = pInterface;
    m_wID = (WORD) ( ((DWORD)pInterface - ARRAY_WaterTriangles) / sizeof ( CWaterTriangleSAInterface ) );
}

CWaterVertex* CWaterPolySA::GetVertex ( int index )
{
    if ( index < 0 || index >= GetNumVertices () )
        return NULL;

    return &g_pWaterManager->m_Vertices [
        GetType () == WATER_POLY_QUAD ? ((CWaterQuadSA *)this)->GetInterface ()->m_wVertexIDs[index]
                                  : ((CWaterTriangleSA *)this)->GetInterface ()->m_wVertexIDs[index]
    ];
}

bool CWaterPolySA::ContainsPoint ( float fX, float fY )
{
    bool bInside = false;
    int numVertices = GetNumVertices ();
    WORD* pwVertexIDs =
        GetType () == WATER_POLY_QUAD ? ((CWaterQuadSA *)this)->GetInterface ()->m_wVertexIDs
                                  : ((CWaterTriangleSA *)this)->GetInterface ()->m_wVertexIDs;
    
    CWaterVertexSA* pFrom;
    CWaterVertexSA* pTo;
    CVector vecFrom;
    CVector vecTo;
    int next[4];
    if ( GetType () == WATER_POLY_QUAD )
    {
        next[0] = 1;
        next[1] = 3;
        next[2] = 0;
        next[3] = 2;
    }
    else
    {
        next[0] = 1;
        next[1] = 2;
        next[2] = 0;
    }
    for ( int i = 0; i < numVertices; i++ )
    {
        pFrom = &g_pWaterManager->m_Vertices [ pwVertexIDs[i] ];
        pTo   = &g_pWaterManager->m_Vertices [ pwVertexIDs[next[i]] ];
        pFrom->GetPosition ( vecFrom );
        pTo->GetPosition ( vecTo );
        
        if ( (vecFrom.fY > fY) != (vecTo.fY > fY) &&
             fX < vecFrom.fX + (vecTo.fX - vecFrom.fX) * (fY - vecFrom.fX) / (vecTo.fY - vecFrom.fY) )
           bInside = !bInside;
    }
    return bInside;
}

// -----------------------------------------------------
// Change trackkeepers

CWaterChangeVertexMove::CWaterChangeVertexMove ( CWaterVertex* pVertex )
{
    pVertex->GetPosition ( m_vecOriginalPosition );
}

void CWaterChangeVertexMove::Undo ( void* pChangedObject )
{
    ((CWaterVertexSA *)pChangedObject)->SetPosition ( m_vecOriginalPosition );
}

// -----------------------------------------------------
// Manager

CWaterManagerSA::CWaterManagerSA ()
    : m_Vertices  ( NUM_DefWaterVertices ),
      m_Quads     ( NUM_DefWaterQuads ),
      m_Triangles ( NUM_DefWaterTriangles ),
      m_Zones     ( NUM_WaterZones )
{
    g_pWaterManager = this;

    for ( DWORD i = 0; i < NUM_WaterZones; i++ )
        m_Zones [ i ].SetInterface ( &((CWaterPolyEntrySAInterface *)ARRAY_WaterZones) [ i ] );

    for ( i = 0; i < NUM_DefWaterVertices; i++ )
        m_Vertices [ i ].SetInterface ( &((CWaterVertexSAInterface *)ARRAY_WaterVertices) [ i ] );

    for ( i = 0; i < NUM_DefWaterQuads; i++ )
        m_Quads [ i ].SetInterface ( &((CWaterQuadSAInterface *)ARRAY_WaterQuads) [ i ] );

    for ( i = 0; i < NUM_DefWaterTriangles; i++ )
        m_Triangles [ i ].SetInterface ( &((CWaterTriangleSAInterface *)ARRAY_WaterTriangles) [ i ] );
}

CWaterManagerSA::~CWaterManagerSA ()
{
    UndoChanges ();
    g_pWaterManager = NULL;
}

CWaterPoly* CWaterManagerSA::GetPolyAtPoint ( CVector& vecPosition )
{
    if ( vecPosition.fX < -3000.0f || vecPosition.fX > 3000.0f || vecPosition.fY < -3000.0f || vecPosition.fY > 3000.0f )
        return NULL;

    CWaterZoneSA* pZone = GetZoneContaining ( vecPosition.fX, vecPosition.fY );
    if ( !pZone )
        return NULL;

    CWaterZoneSA::iterator it;
    for ( it = pZone->begin (); *it; ++it )
    {
        if ( (*it)->ContainsPoint ( vecPosition.fX, vecPosition.fY ) )
        {
            return *it;
        }
    }
    return NULL;
}

CWaterZoneSA* CWaterManagerSA::GetZoneContaining ( float fX, float fY )
{
    if ( fX < -3000.0f || fX > 3000.0f || fY < -3000.0f || fY > 3000.0f )
        return NULL;

    if ( fX == 3000.0f )
        fX = 2999.0f;
    if ( fY == 3000.0f )
        fY = 2999.0f;

    int zoneID = 12*((int)(fY + 3000.0f) / 500) + (int)(fX + 3000.0f) / 500;
    return &m_Zones [ zoneID ];
}

bool CWaterManagerSA::GetWaterLevel ( CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown )
{
    return ( (bool (__cdecl *)(float, float, float, float*, DWORD, CVector*)) FUNC_GetWaterLevel )
        ( vecPosition.fX, vecPosition.fY, vecPosition.fZ, pfLevel, bCheckWaves, pvecUnknown );
}

bool CWaterManagerSA::SetWaterLevel ( CVector& vecPosition, float fLevel, void* pChangeSource )
{
    CWaterPoly* pPoly = GetPolyAtPoint ( vecPosition );
    if ( !pPoly )
        return false;

    CVector vecVertexPos;
    for ( int i = 0; i < pPoly->GetNumVertices (); i++ )
    {
        pPoly->GetVertex ( i )->GetPosition ( vecVertexPos );
        vecVertexPos.fZ = fLevel;
        pPoly->GetVertex ( i )->SetPosition ( vecVertexPos, pChangeSource );
    }
    return true;
}

bool CWaterManagerSA::TestLineAgainstWater ( CVector& vecStart, CVector& vecEnd, CVector* vecCollision )
{
    return ( (bool (__cdecl *)(float, float, float, float, float, float, CVector*)) FUNC_TestLineAgainstWater )
        ( vecEnd.fX, vecEnd.fY, vecEnd.fZ, vecStart.fX, vecStart.fY, vecStart.fZ, vecCollision );
}

void CWaterManagerSA::AddChange ( void *pChangeSource, void* pChangedObject, CWaterChange* pChange )
{
    if ( !pChangeSource )
        return;

    std::map < void*, std::map < void*, CWaterChange* > >::iterator sourceIt = m_Changes.find ( pChangeSource );
    if ( sourceIt == m_Changes.end () )
        sourceIt = m_Changes.insert ( make_pair ( pChangeSource, std::map < void*, CWaterChange* > () ) ).first;
    
    std::map < void*, CWaterChange* >::iterator changeIt = sourceIt->second.find ( pChangedObject );
    if ( changeIt == sourceIt->second.end () )
    {
        sourceIt->second.insert ( make_pair ( pChangedObject, pChange ) );
    }
    else
    {
        delete changeIt->second;
        changeIt->second = pChange;
    }
}

void CWaterManagerSA::UndoChanges ( void* pChangeSource )
{
    if ( pChangeSource == NULL )
    {
        std::map < void*, std::map < void*, CWaterChange* > >::iterator sourceIt = m_Changes.begin ();
        for ( ; sourceIt != m_Changes.end (); sourceIt++ )
        {
            if ( sourceIt->first )
                UndoChanges ( sourceIt->first );
        }
    }
    else
    {
        std::map < void*, std::map < void*, CWaterChange* > >::iterator sourceIt = m_Changes.find ( pChangeSource );
        if ( sourceIt == m_Changes.end () )
            return;
        std::map < void*, CWaterChange* >::iterator changeIt = sourceIt->second.begin ();
        for ( ; changeIt != sourceIt->second.end (); changeIt++ )
        {
            changeIt->second->Undo ( changeIt->first );
            delete changeIt->second;
        }
        m_Changes.erase ( sourceIt );
    }
}

void CWaterManagerSA::Reset ()
{
    ( (void (__cdecl *)()) FUNC_ReadWaterConfiguration )();
}
