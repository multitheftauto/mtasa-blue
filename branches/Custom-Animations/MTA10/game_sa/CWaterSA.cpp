/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWaterSA.cpp
*  PURPOSE:     Control the lakes and seas
*  DEVELOPERS:  arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CWaterManagerSA* g_pWaterManager;

// -----------------------------------------------------
// Vertices

WORD CWaterVertexSA::GetID ()
{
    if ( !m_pInterface )
        return ~0;
    return (WORD)(m_pInterface - g_pWaterManager->m_VertexPool);
}

void CWaterVertexSA::GetPosition ( CVector& vec )
{
    vec.fX = (float)m_pInterface->m_sX;
    vec.fY = (float)m_pInterface->m_sY;
    vec.fZ = m_pInterface->m_fZ;
}

bool CWaterVertexSA::SetPosition ( CVector& vec, void* pChangeSource )
{
    if ( pChangeSource )
        g_pWaterManager->AddChange ( pChangeSource, this, new CWaterChangeVertexMove ( this ) );
    
    m_pInterface->m_sX = ((short)vec.fX) & ~1;
    m_pInterface->m_sY = ((short)vec.fY) & ~1;
    m_pInterface->m_fZ = vec.fZ;
    return true;
}

// -----------------------------------------------------
// Polygons

void CWaterQuadSA::SetInterface ( CWaterPolySAInterface* pInterface )
{
    m_pInterface = pInterface;
    m_wID = (WORD)(pInterface - g_pWaterManager->m_QuadPool);
}

void CWaterTriangleSA::SetInterface ( CWaterPolySAInterface* pInterface )
{
    m_pInterface = pInterface;
    m_wID = (WORD)(pInterface - g_pWaterManager->m_TrianglePool);
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
