/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMarker.cpp
*  PURPOSE:     Marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CMarker::CMarker ( CMarkerManager* pMarkerManager, CColManager* pColManager, CElement* pParent, CXMLNode* pNode ) : CPerPlayerEntity ( pParent, pNode )
{
    // Init
    m_pMarkerManager = pMarkerManager;
    m_pColManager = pColManager;
    m_iType = CElement::MARKER;
    SetTypeName ( "marker" );
    m_ucType = TYPE_CHECKPOINT;
    m_fSize = 4.0f;
    m_Color = SColorRGBA ( 255, 255, 255, 255 );
    m_bHasTarget = false;
    m_ucIcon = ICON_NONE;

    // Create our collision object
    m_pCollision = new CColCircle ( pColManager, NULL, m_vecPosition, m_fSize, NULL, true );
    m_pCollision->SetCallback ( this );
    m_pCollision->SetAutoCallEvent ( false );

    // Add us to the marker manager
    pMarkerManager->AddToList ( this );
    UpdateSpatialData ();
}


CMarker::~CMarker ( void )
{
    // Delete our collision object
    if ( m_pCollision )
        delete m_pCollision;

    // Unlink from manager
    Unlink ();
}


void CMarker::Unlink ( void )
{
    // Remove us from the marker manager
    m_pMarkerManager->RemoveFromList ( this );
}


bool CMarker::ReadSpecialData ( void )
{
    // Grab the "posX" data
    if ( !GetCustomDataFloat ( "posX", m_vecPosition.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posX' attribute in <marker> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posY" data
    if ( !GetCustomDataFloat ( "posY", m_vecPosition.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posY' attribute in <marker> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posZ" data
    if ( !GetCustomDataFloat ( "posZ", m_vecPosition.fZ, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posZ' attribute in <marker> (line %u)\n", m_uiLine );
        return false;
    }

    // Set the position in the col object
    if ( m_pCollision )
        m_pCollision->SetPosition ( m_vecPosition );

    // Grab the "type" data
    char szBuffer [128];
    if ( GetCustomDataString ( "type", szBuffer, 128, true ) )
    {
        // Convert it to a type
        m_ucType = static_cast < unsigned char > ( CMarkerManager::StringToType ( szBuffer ) );
        if ( m_ucType == CMarker::TYPE_INVALID )
        {
            CLogger::LogPrintf ( "WARNING: Unknown 'type' value specified in <marker>; defaulting to \"default\" (line %u)\n", m_uiLine );
            m_ucType = CMarker::TYPE_CHECKPOINT;
        }
    }
    else
    {
        m_ucType = CMarker::TYPE_CHECKPOINT;
    }

    // Grab the "color" data
    if ( GetCustomDataString ( "color", szBuffer, 128, true ) )
    {
        // Convert the HTML-style color to RGB
        if ( !XMLColorToInt ( szBuffer, m_Color.R, m_Color.G, m_Color.B, m_Color.A ) )
        {
            CLogger::ErrorPrintf ( "Bad 'color' specified in <marker> (line %u)\n", m_uiLine );
            return false;
        }
    }
    else
    {
        SetColor ( SColorRGBA( 255, 0, 0, 255 ) );
    }

    float fSize;
    if ( GetCustomDataFloat ( "size", fSize, true ) )
        m_fSize = fSize;

    int iTemp;
    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    if ( GetCustomDataInt ( "interior", iTemp, true ) )
        m_ucInterior = static_cast < unsigned char > ( iTemp );

    // Success
    return true;
}


void CMarker::SetPosition ( const CVector& vecPosition )
{
    // Remember our last position
    m_vecLastPosition = m_vecPosition;

    // Different from our current position?
    if ( m_vecPosition != vecPosition )
    {
        // Set the new position
        m_vecPosition = vecPosition;
        if ( m_pCollision )
            m_pCollision->SetPosition ( vecPosition );
        UpdateSpatialData ();

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


void CMarker::SetTarget ( const CVector* pTargetVector )
{
    // Got a target vector?
    if ( pTargetVector )
    {
        // Different from our current target
        if ( !m_bHasTarget || m_vecTarget != *pTargetVector )
        {
            // Is this a marker type that has a destination?
            if ( m_ucType == CMarker::TYPE_CHECKPOINT || m_ucType == CMarker::TYPE_RING )
            {
                // Set the target position
                m_bHasTarget = true;
                m_vecTarget = *pTargetVector;

                // Tell everyone that knows about this marker
                CBitStream BitStream;
                BitStream.pBitStream->Write ( m_ID );
                BitStream.pBitStream->Write ( static_cast < unsigned char > ( 1 ) );
                BitStream.pBitStream->Write ( m_vecTarget.fX );
                BitStream.pBitStream->Write ( m_vecTarget.fY );
                BitStream.pBitStream->Write ( m_vecTarget.fZ );
                BroadcastOnlyVisible ( CLuaPacket ( SET_MARKER_TARGET, *BitStream.pBitStream ) );
            }
            else
            {
                // Reset the target position
                m_bHasTarget = false;
            }
        }
    }
    else
    {
        // Not already without target?
        if ( m_bHasTarget )
        {
            // Reset the target position
            m_bHasTarget = false;

            // Is this a marker type that has a destination?
            if ( m_ucType == CMarker::TYPE_CHECKPOINT || m_ucType == CMarker::TYPE_RING )
            {
                // Tell everyone that knows about this marker
                CBitStream BitStream;
                BitStream.pBitStream->Write ( m_ID );
                BitStream.pBitStream->Write ( static_cast < unsigned char > ( 0 ) );
                BroadcastOnlyVisible ( CLuaPacket ( SET_MARKER_TARGET, *BitStream.pBitStream ) );
            }
        }
    }
}


void CMarker::SetMarkerType ( unsigned char ucType )
{
    // Different from our current type?
    if ( ucType != m_ucType )
    {
        // Set the new type
        unsigned char ucOldType = m_ucType;
        m_ucType = ucType;
        UpdateCollisionObject ( ucOldType );

        // Tell all players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( m_ID );
        BitStream.pBitStream->Write ( ucType );
        BroadcastOnlyVisible ( CLuaPacket ( SET_MARKER_TYPE, *BitStream.pBitStream ) );

        // Is the new type not a checkpoint or a ring? Remove the target
        if ( ucType != CMarker::TYPE_CHECKPOINT && ucType != CMarker::TYPE_RING )
        {
            m_bHasTarget = false;
        }
    }
}


void CMarker::SetSize ( float fSize )
{
    // Different from our current size?
    if ( fSize != m_fSize )
    {
        // Set the new size and update the col object
        m_fSize = fSize;
        UpdateCollisionObject ( m_ucType );

        // Tell all players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( m_ID );
        BitStream.pBitStream->Write ( fSize );
        BroadcastOnlyVisible ( CLuaPacket ( SET_MARKER_SIZE, *BitStream.pBitStream ) );
    }
}


void CMarker::SetColor ( const SColor color )
{
    // Different from our current color?
    if ( color != m_Color )
    {
        // Set the new color
        m_Color = color;

        // Tell all the players
        CBitStream BitStream;
        BitStream.pBitStream->Write ( m_ID );
        BitStream.pBitStream->Write ( color.B  );
        BitStream.pBitStream->Write ( color.G );
        BitStream.pBitStream->Write ( color.R );
        BitStream.pBitStream->Write ( color.A );
        BroadcastOnlyVisible ( CLuaPacket ( SET_MARKER_COLOR, *BitStream.pBitStream ) );
    }
}


void CMarker::SetIcon ( unsigned char ucIcon )
{
    if ( m_ucIcon != ucIcon )
    {
        m_ucIcon = ucIcon;

        // Tell everyone that knows about this marker
        CBitStream BitStream;
        BitStream.pBitStream->Write ( m_ID );
        BitStream.pBitStream->Write ( m_ucIcon );
        BroadcastOnlyVisible ( CLuaPacket ( SET_MARKER_ICON, *BitStream.pBitStream ) );
    }
}


void CMarker::Callback_OnCollision ( CColShape& Shape, CElement& Element )
{
    // Matching interior?
    if ( GetInterior () == Element.GetInterior () )
    {
        // Call the marker hit event
        CLuaArguments Arguments;
        Arguments.PushElement ( &Element ); // Hit element
        Arguments.PushBoolean ( GetDimension () == Element.GetDimension () );        // Matching dimension?
        CallEvent ( "onMarkerHit", Arguments );

        if ( IS_PLAYER ( &Element ) )
        {
            CLuaArguments Arguments2;
            Arguments2.PushElement ( this );        // marker
            Arguments2.PushBoolean ( GetDimension () == Element.GetDimension () );        // Matching dimension?
            Element.CallEvent ( "onPlayerMarkerHit", Arguments2 );
        }
    }
}


void CMarker::Callback_OnLeave ( CColShape& Shape, CElement& Element )
{
    // Matching interior?
    if ( GetInterior () == Element.GetInterior () )
    {
        // Call the marker hit event
        CLuaArguments Arguments;
        Arguments.PushElement ( &Element ); // Hit element
        Arguments.PushBoolean ( GetDimension () == Element.GetDimension () );        // Matching dimension?
        CallEvent ( "onMarkerLeave", Arguments );

        if ( IS_PLAYER ( &Element ) )
        {
            CLuaArguments Arguments2;
            Arguments2.PushElement ( this );        // marker
            Arguments2.PushBoolean ( GetDimension () == Element.GetDimension () );        // Matching dimension?
            Element.CallEvent ( "onPlayerMarkerLeave", Arguments2 );
        }
    }
}


void CMarker::Callback_OnCollisionDestroy ( CColShape* pCollision )
{
    if ( pCollision == m_pCollision )
        m_pCollision = NULL;
}


void CMarker::UpdateCollisionObject ( unsigned char ucOldType )
{
    // Different type than before?
    if ( m_ucType != ucOldType )
    {
        // Are we becomming a checkpoint? Make the col object a circle. If we're becomming something and we were a checkpoint, make the col object a sphere.

        // lil_Toady: Simply deleting the colshape may cause a dangling pointer in CColManager if we're in DoHitDetection loop
        if ( m_ucType == CMarker::TYPE_CHECKPOINT )
        {
            if ( m_pCollision )
                g_pGame->GetElementDeleter()->Delete ( m_pCollision );
            m_pCollision = new CColCircle ( m_pColManager, NULL, m_vecPosition, m_fSize, NULL );
        }
        else if ( ucOldType == CMarker::TYPE_CHECKPOINT )
        {
            if ( m_pCollision )
                g_pGame->GetElementDeleter()->Delete ( m_pCollision );
            m_pCollision = new CColSphere ( m_pColManager, NULL, m_vecPosition, m_fSize, NULL );
        }

        m_pCollision->SetCallback ( this );
        m_pCollision->SetAutoCallEvent ( false );
    }

    // Set the radius after the size
    if ( m_ucType == CMarker::TYPE_CHECKPOINT )
    {
        static_cast < CColCircle* > ( m_pCollision )->SetRadius ( m_fSize );
    }
    else
    {
        static_cast < CColSphere* > ( m_pCollision )->SetRadius ( m_fSize );
    }
}


CSphere CMarker::GetWorldBoundingSphere ( void )
{
    return CSphere ( GetPosition (), GetSize () );
}