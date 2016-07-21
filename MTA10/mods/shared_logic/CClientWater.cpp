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

CClientWater::CClientWater ( CClientManager* pManager, ElementID ID, CVector& vecBL, CVector& vecBR, CVector& vecTL, CVector& vecTR, bool bShallow ) : ClassInit ( this ), CClientEntity ( ID )
{
    m_pManager = pManager;
    m_pWaterManager = pManager->GetWaterManager ();
    SetTypeName ( "water" );

    m_bTriangle = false;
    m_Verticies.push_back(vecBL);
    m_Verticies.push_back(vecBR);
    m_Verticies.push_back(vecTL);
    m_Verticies.push_back(vecTR);
    m_bShallow = bShallow;

    Create();
    m_pWaterManager->AddToList ( this );
}

CClientWater::CClientWater ( CClientManager* pManager, ElementID ID, CVector& vecL, CVector& vecR, CVector& vecTB, bool bShallow ) : ClassInit ( this ), CClientEntity ( ID )
{
    m_pManager = pManager;
    m_pWaterManager = pManager->GetWaterManager ();
    SetTypeName ( "water" );

    m_bTriangle = true;
    m_Verticies.push_back(vecL);
    m_Verticies.push_back(vecR);
    m_Verticies.push_back(vecTB);
    m_bShallow = bShallow;

    Create();
    m_pWaterManager->AddToList ( this );
}

CClientWater::~CClientWater ()
{
    Unlink ();
    Destroy();
}

bool CClientWater::Create ( void )
{
    if (m_pPoly)
        return false;

    if (m_bTriangle)
        m_pPoly = g_pGame->GetWaterManager()->CreateTriangle(m_Verticies[0], m_Verticies[1], m_Verticies[2], m_bShallow);
    else
        m_pPoly = g_pGame->GetWaterManager()->CreateQuad(m_Verticies[0], m_Verticies[1], m_Verticies[2], m_Verticies[3], m_bShallow);
    
    return true;
}

bool CClientWater::Destroy ( void )
{
    if (!m_pPoly)
        return false;
    
    g_pGame->GetWaterManager()->DeletePoly(m_pPoly);
    m_pPoly = NULL;
    return true;
}

int CClientWater::GetNumVertices () const
{
    return m_bTriangle ? 3 : 4;
}

void CClientWater::GetPosition ( CVector& vecPosition ) const
{
    // Calculate the average of the vertex positions
    vecPosition.fX = 0.0f;
    vecPosition.fY = 0.0f;
    vecPosition.fZ = 0.0f;

    for ( CVector vecVertexPos : m_Verticies )
        vecPosition += vecVertexPos;

    vecPosition /= static_cast < float > ( GetNumVertices () );
}

bool CClientWater::GetVertexPosition ( int iVertexIndex, CVector& vecPosition )
{
    if (iVertexIndex < 0 || iVertexIndex >= GetNumVertices())
        return false;

    vecPosition = m_Verticies.at(iVertexIndex);
    return true;
}

void CClientWater::SetPosition ( const CVector& vecPosition )
{
    if ( !m_pPoly )
        return;

    CVector vecCurrentPosition;
    GetPosition ( vecCurrentPosition );
    CVector vecDelta = vecPosition - vecCurrentPosition;

    int i = 0;
    for (CVector& vecVertexPos : m_Verticies)
    {
        vecVertexPos += vecDelta;
        if (m_pPoly)
            m_pPoly->GetVertex(i)->SetPosition(vecVertexPos);
        i++;

    }

    g_pGame->GetWaterManager ()->RebuildIndex ();
}

bool CClientWater::SetVertexPosition ( int iVertexIndex, CVector& vecPosition, void* pChangeSource )
{
    if (iVertexIndex < 0 || iVertexIndex >= GetNumVertices())
        return false;

    m_Verticies.at(iVertexIndex) = vecPosition;

    if (m_pPoly)
    {
        CWaterVertex* pVertex = m_pPoly->GetVertex(iVertexIndex);
        if (pVertex)
            return pVertex->SetPosition(vecPosition, pChangeSource);
    }
    return true;   
}

void CClientWater::Unlink ()
{
    m_pWaterManager->RemoveFromList ( this );
}

void CClientWater::SetDimension(unsigned short usDimension)
{
    m_usDimension = usDimension;
    RelateDimension(m_pManager->GetWaterManager()->GetDimension());
}

void CClientWater::RelateDimension(unsigned short usWorldDimension)
{
    if (usWorldDimension == m_usDimension)
        Create();
    else
        Destroy();
}
