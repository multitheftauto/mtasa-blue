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
int ms_iNumNonDefaultAndNonZeroVertices = 0;

// -----------------------------------------------------
// Vertices

CWaterVertexSA::CWaterVertexSA ( void )
{
    m_pInterface = NULL;
    m_bIsWorldWaterVertex = false;
}

CWaterVertexSA::CWaterVertexSA ( CWaterVertexSAInterface* pInterface )
{
    m_pInterface = pInterface;
    m_bIsWorldWaterVertex = false;
}

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

bool CWaterVertexSA::SetPosition ( const CVector& vec, void* pChangeSource )
{
    if ( pChangeSource )
        g_pWaterManager->AddChange ( pChangeSource, this, new CWaterChangeVertexMove ( this ) );

    OnChangeLevel ( m_pInterface->m_fZ, vec.fZ );

    m_pInterface->m_sX = ((short)vec.fX) & ~1;
    m_pInterface->m_sY = ((short)vec.fY) & ~1;
    m_pInterface->m_fZ = vec.fZ;
    return true;
}

// See if level will require alt render order
void CWaterVertexSA::OnChangeLevel ( float fOldZ, float fNewZ )
{
    if ( m_bIsWorldWaterVertex )
    {
        // If changing from default or zero, increment global counter
        // If changing to default or zero, decrement global counter
        bool bWasDefaultOrZero = ( fOldZ == m_fDefaultZ ) || ( fOldZ == 0 );
        bool bIsDefaultOrZero  = ( fNewZ == m_fDefaultZ ) || ( fNewZ == 0 );
        if ( bWasDefaultOrZero != bIsDefaultOrZero )
        {
            if ( bWasDefaultOrZero )
                ms_iNumNonDefaultAndNonZeroVertices++;
            else
                ms_iNumNonDefaultAndNonZeroVertices--;

            if ( ms_iNumNonDefaultAndNonZeroVertices == 0 || ms_iNumNonDefaultAndNonZeroVertices == 1 )
                g_pWaterManager->UpdateRenderOrderRequirement ();
        }
    }
}

// Save initial state
void CWaterVertexSA::Init ( bool bIsWorldWaterVertex )
{
    m_bIsWorldWaterVertex = bIsWorldWaterVertex;
    if ( m_bIsWorldWaterVertex )
        m_fDefaultZ = m_pInterface->m_fZ;
}

// Restore initial state
void CWaterVertexSA::Reset ( void )
{
    if ( m_bIsWorldWaterVertex )
    {
        CVector vec;
        GetPosition ( vec );
        vec.fZ = m_fDefaultZ;
        SetPosition ( vec );
    }
}

// Check if swimming pool etc.
bool CWaterVertexSA::IsWorldNonSeaLevel ( void )
{
    return m_bIsWorldWaterVertex && m_fDefaultZ != 0.f;
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
