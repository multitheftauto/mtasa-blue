/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWater.cpp
*  PURPOSE:     Object entity class
*  DEVELOPERS:  arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CWater::CWater ( CWaterManager* pWaterManager, CElement* pParent, CXMLNode* pNode, EWaterType waterType ) : CElement ( pParent, pNode )
{
    m_pWaterManager = pWaterManager;

    m_iType = CElement::WATER;
    SetTypeName ( "water" );
    m_WaterType = waterType;

    m_Vertices [ 0 ] = CVector ( -10.0f, -10.0f, 0.0f );
    m_Vertices [ 1 ] = CVector (  10.0f, -10.0f, 0.0f );
    m_Vertices [ 2 ] = CVector ( -10.0f,  10.0f, 0.0f );
    if ( m_WaterType == QUAD )
        m_Vertices [ 3 ] = CVector ( 10.0f, 10.0f, 0.0f );

    if ( m_pWaterManager )
        m_pWaterManager->AddToList ( this );
}

CWater::~CWater ()
{
    Unlink ();
}

void CWater::Unlink ()
{
    if ( m_pWaterManager )
        m_pWaterManager->RemoveFromList ( this );
}

const CVector& CWater::GetPosition ()
{
    m_vecPosition = CVector ( 0.0f, 0.0f, 0.0f );
    for ( int i = 0; i < GetNumVertices (); i++ )
    {
        m_vecPosition += m_Vertices [ i ];
    }
    m_vecPosition /= GetNumVertices ();
    return m_vecPosition;
}

void CWater::SetPosition ( const CVector& vecPosition )
{
    CVector vecDelta = vecPosition - CWater::GetPosition ();
    for ( int i = 0; i < GetNumVertices (); i++ )
    {
        m_Vertices [ i ] += vecDelta;
    }
    RoundVertices ();
}

bool CWater::ReadSpecialData ()
{
    // Grab the "posX" data
    if ( !GetCustomDataFloat ( "posX", m_vecPosition.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posX' attribute in <water> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posY" data
    if ( !GetCustomDataFloat ( "posY", m_vecPosition.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posY' attribute in <water> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posZ" data
    if ( !GetCustomDataFloat ( "posZ", m_vecPosition.fZ, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posZ' attribute in <water> (line %u)\n", m_uiLine );
        return false;
    }

    float fWidth = 20.0f;
    float fHeight = 20.0f;
    GetCustomDataFloat ( "sizeX", fWidth, false );
    GetCustomDataFloat ( "sizeY", fHeight, false );

    if ( fWidth <= 0.0f )
    {
        CLogger::ErrorPrintf ( "Invalid 'sizeX' attribute in <water> (line %u)\n", m_uiLine );
        return false;
    }

    if ( fHeight <= 0.0f )
    {
        CLogger::ErrorPrintf ( "Invalid 'sizeY' attribute in <water> (line %u)\n", m_uiLine );
        return false;
    }

    m_WaterType = QUAD;
    m_Vertices [ 0 ] = m_vecPosition + CVector ( -fWidth / 2.0f, -fHeight / 2.0f, 0.0f );
    m_Vertices [ 1 ] = m_vecPosition + CVector (  fWidth / 2.0f, -fHeight / 2.0f, 0.0f );
    m_Vertices [ 2 ] = m_vecPosition + CVector ( -fWidth / 2.0f,  fHeight / 2.0f, 0.0f );
    m_Vertices [ 3 ] = m_vecPosition + CVector (  fWidth / 2.0f,  fHeight / 2.0f, 0.0f );
    RoundVertices ();
    if ( !Valid () )
    {
        CLogger::ErrorPrintf ( "Invalid <water> element (line %u)\n", m_uiLine );
        return false;
    }
    return true;
}

bool CWater::GetVertex ( int index, CVector& vecPosition )
{
    if ( index < 0 || index >= GetNumVertices () )
        return false;

    vecPosition = m_Vertices [ index ];
    return true;
}

bool CWater::SetVertex ( int index, CVector& vecPosition )
{
    if ( index < 0 || index >= GetNumVertices () )
        return false;

    m_Vertices [ index ] = vecPosition;
    RoundVertex ( index );
    return true;
}

bool CWater::Valid ()
{
    for ( int i = 0; i < GetNumVertices (); i++ )
    {
        if ( !IsVertexWithinWorld ( i ) )
            return false;
    }

    if ( m_WaterType == QUAD )
    {
        return m_Vertices[0].fX < m_Vertices[1].fX && m_Vertices[2].fX < m_Vertices[3].fX &&
               m_Vertices[0].fY < m_Vertices[2].fY && m_Vertices[1].fY < m_Vertices[3].fY;
    }
    else
    {
        return m_Vertices[0].fX < m_Vertices[1].fX &&
              (m_Vertices[0].fY < m_Vertices[2].fY) == (m_Vertices[1].fY < m_Vertices[2].fY);
    }
}

void CWater::RoundVertices ()
{
    for ( int i = 0; i < GetNumVertices (); i++ )
    {
        RoundVertex ( i );
    }
}

void CWater::RoundVertex ( int index )
{
    m_Vertices [ index ].fX = (float)( (int)m_Vertices [ index ].fX & ~1 );
    m_Vertices [ index ].fY = (float)( (int)m_Vertices [ index ].fY & ~1 );
}

bool CWater::IsVertexWithinWorld ( int index )
{
    return m_Vertices [ index ].fX >= -3000.0f && m_Vertices [ index ].fX <= 3000.0f &&
           m_Vertices [ index ].fY >= -3000.0f && m_Vertices [ index ].fY <= 3000.0f;
}
