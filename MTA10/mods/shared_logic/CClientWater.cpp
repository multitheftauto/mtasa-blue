/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWater.cpp
*  PURPOSE:     Water entity class
*  DEVELOPERS:  arc_
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CClientWater::CClientWater ( ElementID ID, CVector& vecBL, CVector& vecBR, CVector& vecTL, CVector& vecTR, bool bShallow ) : CClientEntity ( ID )
{
    m_pPoly = g_pGame->GetWaterManager ()->CreateQuad ( vecBL, vecBR, vecTL, vecTR, bShallow );
    SetTypeName ( "water" );
}

CClientWater::CClientWater ( ElementID ID, CVector& vecL, CVector& vecR, CVector& vecTB, bool bShallow ) : CClientEntity ( ID )
{
    m_pPoly = g_pGame->GetWaterManager ()->CreateTriangle ( vecL, vecR, vecTB, bShallow );
    SetTypeName ( "water" );
}

CClientWater::~CClientWater ()
{
    if ( m_pPoly )
        g_pGame->GetWaterManager ()->DeletePoly ( m_pPoly );
}

void CClientWater::GetPosition ( CVector& vecPosition ) const
{
    // Calculate the average of the vertex positions
    vecPosition.fX = 0.0f;
    vecPosition.fY = 0.0f;
    vecPosition.fZ = 0.0f;
    if ( !m_pPoly )
        return;

    CVector vecVertexPos;
    for ( int i = 0; i < m_pPoly->GetNumVertices (); i++ )
    {
        m_pPoly->GetVertex ( i )->GetPosition ( vecVertexPos );
        vecPosition += vecVertexPos;
    }
    vecPosition /= m_pPoly->GetNumVertices ();
}

bool CClientWater::GetVertexPosition ( int iVertexIndex, CVector& vecPosition )
{
    if ( !m_pPoly )
        return false;

    CWaterVertex* pVertex = m_pPoly->GetVertex ( iVertexIndex );
    if ( !pVertex )
        return false;

    pVertex->GetPosition ( vecPosition );
    return true;
}

void CClientWater::SetPosition ( const CVector& vecPosition )
{
    if ( !m_pPoly )
        return;

    CVector vecCurrentPosition;
    GetPosition ( vecCurrentPosition );
    CVector vecDelta = vecPosition - vecCurrentPosition;

    CVector vecVertexPos;
    for ( int i = 0; i < m_pPoly->GetNumVertices (); i++ )
    {
        m_pPoly->GetVertex ( i )->GetPosition ( vecVertexPos );
        vecVertexPos += vecDelta;
        m_pPoly->GetVertex ( i )->SetPosition ( vecVertexPos );
    }
    g_pGame->GetWaterManager ()->RebuildIndex ();
}

bool CClientWater::SetVertexPosition ( int iVertexIndex, CVector& vecPosition )
{
    if ( !m_pPoly )
        return false;

    CWaterVertex* pVertex = m_pPoly->GetVertex ( iVertexIndex );
    if ( !pVertex )
        return false;

    pVertex->SetPosition ( vecPosition );
    return true;
}

void CClientWater::Unlink ()
{

}
