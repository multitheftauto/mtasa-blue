/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRadarArea.cpp
*  PURPOSE:     Radar area entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CRadarArea::CRadarArea ( CRadarAreaManager* pRadarAreaManager, CElement* pParent, CXMLNode* pNode ) : CPerPlayerEntity ( pParent, pNode )
{
    // Init
    m_iType = CElement::RADAR_AREA;
    SetTypeName ( "radararea" );
    m_pRadarAreaManager = pRadarAreaManager;
    m_Color = SColorRGBA ( 255, 255, 255, 255 );
    m_bIsFlashing = false;

    // Pop an id for us and add us to the manger's list
    pRadarAreaManager->AddToList ( this );
}


CRadarArea::~CRadarArea ( void )
{
    // Unlink us from manager
    Unlink ();
}


void CRadarArea::Unlink ( void )
{
    // Remove us from the manager's list
    m_pRadarAreaManager->RemoveFromList ( this );
}


bool CRadarArea::ReadSpecialData ( void )
{
    // Grab the "posX" data
    if ( !GetCustomDataFloat ( "posX", m_vecPosition.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posX' attribute in <radararea> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posY" data
    if ( !GetCustomDataFloat ( "posY", m_vecPosition.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posY' attribute in <radararea> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "sizeX" data
    if ( !GetCustomDataFloat ( "sizeX", m_vecSize.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'sizeX' attribute in <radararea> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "sizeY" data
    if ( !GetCustomDataFloat ( "sizeY", m_vecSize.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'sizeY' attribute in <radararea> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "color" data
    char szColor [64];
    if ( GetCustomDataString ( "color", szColor, 64, true ) )
    {
        // Convert it to RGBA
        if ( !XMLColorToInt ( szColor, m_Color.R, m_Color.G, m_Color.B, m_Color.A ) )
        {
            CLogger::ErrorPrintf ( "Bad 'color' value specified in <radararea> (line %u)\n", m_uiLine );
            return false;
        }
    }
    else
    {
        m_Color = SColorRGBA ( 255, 0, 0, 255 );
    }

	int iTemp;
	if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    return true;
}


void CRadarArea::SetPosition ( const CVector& vecPosition )
{
    // Remember the last position
    m_vecLastPosition = m_vecPosition;

    // Different from our current position?
    if ( vecPosition != m_vecPosition )
    {
        // Set the new position
        m_vecPosition = vecPosition;

        // We need to make sure the time context is replaced 
        // before that so old packets don't arrive after this.
        GenerateSyncTimeContext ();

        // Tell all the players that know about us
        CBitStream BitStream;
        BitStream.pBitStream->Write ( m_ID );
        BitStream.pBitStream->Write ( vecPosition.fX );
        BitStream.pBitStream->Write ( vecPosition.fY );
        BitStream.pBitStream->Write ( vecPosition.fZ );
        BitStream.pBitStream->Write ( GetSyncTimeContext () );
        BroadcastOnlyVisible ( CLuaPacket ( SET_ELEMENT_POSITION, *BitStream.pBitStream ) );
    }
}


void CRadarArea::SetSize ( const CVector2D& vecSize )
{
    // Different from our current size?
    if ( vecSize != m_vecSize )
    {
        // Update the size
        m_vecSize = vecSize;

        // Tell all the players that know about us
        CBitStream BitStream;
        BitStream.pBitStream->Write ( m_ID );
        BitStream.pBitStream->Write ( vecSize.fX );
        BitStream.pBitStream->Write ( vecSize.fY );
        BroadcastOnlyVisible ( CLuaPacket ( SET_RADAR_AREA_SIZE, *BitStream.pBitStream ) );
    }
}


void CRadarArea::SetColor ( const SColor color )
{
    // Different from our current color?
    if ( color != m_Color )
    {
        // Update the color
        m_Color = color;

        // Tell all the players that know about us
        CBitStream BitStream;
        BitStream.pBitStream->Write ( m_ID );
        BitStream.pBitStream->Write ( color.R );
        BitStream.pBitStream->Write ( color.G );
        BitStream.pBitStream->Write ( color.B );
        BitStream.pBitStream->Write ( color.A );
        BroadcastOnlyVisible ( CLuaPacket ( SET_RADAR_AREA_COLOR, *BitStream.pBitStream ) );
    }
}


void CRadarArea::SetFlashing ( bool bFlashing )
{
    // Different from our current flashing status?
    if ( bFlashing != m_bIsFlashing )
    {
        // Set the new status
        m_bIsFlashing = bFlashing;

        // Tell all the players that know about us
        CBitStream BitStream;
        BitStream.pBitStream->Write ( m_ID );

        if ( bFlashing )
        {
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( 1 ) );
        }
        else
        {
            BitStream.pBitStream->Write ( static_cast < unsigned char > ( 0 ) );
        }

        BroadcastOnlyVisible ( CLuaPacket ( SET_RADAR_AREA_FLASHING, *BitStream.pBitStream ) );
    }
}
