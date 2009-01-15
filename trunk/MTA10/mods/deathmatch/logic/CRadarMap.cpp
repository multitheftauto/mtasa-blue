/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRadarMap.cpp
*  PURPOSE:     Full screen radar map renderer
*  DEVELOPERS:  Oliver Brown <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CRadarMap::CRadarMap ( CClientManager* pManager )
{
    // Setup our managers
    m_pManager = pManager;
    m_pRadarMarkerManager = pManager->GetRadarMarkerManager ();
    m_pRadarAreaManager = m_pManager->GetRadarAreaManager ();

    // Set the radar bools
    m_bIsRadarEnabled = false;
    m_bForcedState = false;
    m_bIsAttachedToLocal = false;

    // Set the movement bools
    m_bIsMovingNorth = false;
    m_bIsMovingSouth = false;
    m_bIsMovingEast = false;
    m_bIsMovingWest = false;

    // Set the update time to the current time
    m_ulUpdateTime = GetTickCount ();

    // Get the window sizes and set the map variables to default zoom/movement
    m_uiHeight = g_pCore->GetGraphics ()->GetViewportHeight ();
    m_uiWidth = g_pCore->GetGraphics ()->GetViewportWidth ();
    m_ucZoom = 1;
    m_iHorizontalMovement = 0;
    m_iVerticalMovement = 0;
    SetupMapVariables ();

    // Set the blip sizes to default
    m_fLocalPlayerBlipSize = ( m_fMapSize / m_ucZoom ) / 35.0f;
    m_fStandardImageBlipSize = ( m_fMapSize / m_ucZoom ) / 50.0f;

    // Create the radar and local player blip images
    m_pRadarImage = g_pCore->GetGraphics()->LoadTexture ( "MTA\\cgui\\images\\radar.jpg", RADAR_TEXTURE_WIDTH, RADAR_TEXTURE_HEIGHT );
    m_pLocalPlayerBlip = g_pCore->GetGraphics()->LoadTexture ( "MTA\\cgui\\images\\radarset\\03.png"  );

    // Create the text display for the mode text
    m_pModeText = new CClientTextDisplay ( m_pManager->GetDisplayManager (), 0xFFFFFFFF, false );
    m_pModeText->SetColor ( 255, 0, 0, 255 );
    m_pModeText->SetPosition ( CVector ( 0.50f, 0.92f, 0 ) );
    m_pModeText->SetFormat ( DT_CENTER | DT_VCENTER );
    m_pModeText->SetScale ( 2.0f );
    m_pModeText->SetCaption ( "Current Mode: Free Move" );
    m_pModeText->SetVisible ( false );
}


CRadarMap::~CRadarMap ( void )
{
    // Delete our images
    if ( m_pRadarImage )
    {
        m_pRadarImage->Release();
    }

    if ( m_pLocalPlayerBlip )
    {
        m_pLocalPlayerBlip->Release();
    }
}


void CRadarMap::DoPulse ( void )
{
    // If our radar image exists
    if ( IsRadarShowing () )
    {
        // If we are following the local player blip
        if ( m_bIsAttachedToLocal )
        {
            // Get the latest vars for the map
            SetupMapVariables ();
        }

        // If the update time is more than 50ms behind
        if ( GetTickCount () >= m_ulUpdateTime + 50 )
        {
            // Set the update time
            m_ulUpdateTime = GetTickCount ();

            // If we are set to moving then do a zoom/move level jump
            if ( m_bIsMovingNorth )
            {
                MoveNorth ();
            }
            else if ( m_bIsMovingSouth )
            {
                MoveSouth ();
            }
            else if ( m_bIsMovingEast )
            {
                MoveEast ();
            }
            else if ( m_bIsMovingWest )
            {
                MoveWest ();
            }
        }

        g_pCore->GetGraphics()->DrawTexture ( m_pRadarImage, static_cast < float > ( m_iMapMinX ),
                                                             static_cast < float > ( m_iMapMinY ),
                                                             m_fMapSize / RADAR_TEXTURE_WIDTH,
                                                             m_fMapSize / RADAR_TEXTURE_HEIGHT,
                                                             RADAR_TEXTURE_ALPHA );

        // Grab the info for the local player blip
        CVector2D vecLocalPos;
        CVector vecLocal;
        if ( m_pManager->GetCamera()->IsInFixedMode() )
            m_pManager->GetCamera()->GetPosition ( vecLocal );
        else
            m_pManager->GetPlayerManager ()->GetLocalPlayer ()->GetPosition ( vecLocal );

        CalculateEntityOnScreenPosition ( vecLocal, vecLocalPos, m_fLocalPlayerBlipSize, CVector2D ( 0.5f, 0.0f ) );
        g_pCore->GetGraphics()->DrawTexture ( m_pLocalPlayerBlip, vecLocalPos.fX, vecLocalPos.fY );


        // Now loop our radar areas
        unsigned short usDimension = m_pRadarAreaManager->GetDimension ();
        CClientRadarArea * pArea = NULL;
        list < CClientRadarArea* > ::const_iterator areaIter = m_pRadarAreaManager->IterBegin ();
        for ( ; areaIter != m_pRadarAreaManager->IterEnd (); areaIter++ )
        {
            pArea = *areaIter;

            if ( pArea->GetDimension() == usDimension )
            {
                // Grab the area image and calculate the position to put it on the screen
                CVector2D vecPos;
                CalculateEntityOnScreenPosition ( pArea, vecPos, 1.0f, CVector2D ( 0.5f, 0.5f ) );

                // Get the area size and work out the ratio
                CVector2D vecSize;
                float fX = (*areaIter)->GetSize ().fX;
                float fY = (*areaIter)->GetSize ().fY;
                float fRatio = 6000.0f / m_fMapSize;

                // Calculate the size of the area
                vecSize.fX = static_cast < float > ( fX / fRatio );
                vecSize.fY = static_cast < float > ( fY / fRatio );
                unsigned long ulColor = pArea->GetColor ();

                // Convert the color from ABGR to ARGB
                ulColor = ( ulColor & 0xFF000000 ) |            // a
                        ( ( ulColor & 0x000000FF ) << 16 ) |  // r
                        ( ulColor & 0x0000FF00 ) |            // g
                        ( ( ulColor & 0x00FF0000 ) >> 16 );   // b

                g_pCore->GetGraphics ()->DrawRectangle ( vecPos.fX, vecPos.fY, vecSize.fX, -vecSize.fY, ulColor );
            }
        }

        // Now loop our radar markers
        usDimension = m_pRadarMarkerManager->GetDimension();
        list < CClientRadarMarker* > ::const_iterator markerIter = m_pRadarMarkerManager->IterBegin ();
        for ( ; markerIter != m_pRadarMarkerManager->IterEnd (); markerIter++ )
        {
            if ( (*markerIter)->IsVisible () && (*markerIter)->GetDimension() == usDimension )
            {
                // Grab the marker image and calculate the position to put it on the screen
                CVector2D vecPos;
                IDirect3DTexture9* pImage = (*markerIter)->GetMapMarkerImage ();

                // Scale the marker to the right size
                float fScale = (*markerIter)->GetScale ();

                switch ( (*markerIter)->GetMapMarkerState () )
                {
                    case CClientRadarMarker::MAP_MARKER_SQUARE:
                    {
                        fScale = fScale / 5.0f;
                        break;
                    }
                    case CClientRadarMarker::MAP_MARKER_TRIANGLE_UP:
                    {
                        fScale = fScale / 4.0f;
                        break;
                    }
                    case CClientRadarMarker::MAP_MARKER_TRIANGLE_DOWN:
                    {
                        fScale = fScale / 4.0f;
                        break;
                    }
                    default:
                    {
                        fScale = 1.0f;
                        break;
                    }
                }

                CalculateEntityOnScreenPosition ( *markerIter, vecPos, fScale, CVector2D ( 0.5f, 0.5f ) );

                // If it is a picture icon not a blip
                if ( (*markerIter)->GetSprite () != 0 )
                {
                    // If the picture exists
                    if ( pImage )
                    {
                        // Set the size, position etc and show it here
                        g_pCore->GetGraphics()->DrawTexture ( pImage, vecPos.fX, vecPos.fY, fScale, fScale );
                    }
                }
                else
                {
                    // If the radar blip hasn't been created
                    if ( !pImage )
                    {
                        // Create it and set it to a square blip
                        (*markerIter)->SetMapMarkerState ( CClientRadarMarker::MAP_MARKER_SQUARE );
                    }

                    CVector vecMarker;
                    (*markerIter)->GetPosition ( vecMarker );

                    CClientRadarMarker::EMapMarkerState eMapMarkerState = (*markerIter)->GetMapMarkerState ();

                    // We're higher than this marker, so draw the arrow pointing down
                    if ( vecLocal.fZ > vecMarker.fZ + 4.0f )
                    {
                        // If this is not the right blip
                        if ( eMapMarkerState != CClientRadarMarker::MAP_MARKER_TRIANGLE_DOWN )
                        {
                            // Set it to the right blip
                            (*markerIter)->SetMapMarkerState ( CClientRadarMarker::MAP_MARKER_TRIANGLE_DOWN );
                        }
                    }

                    // We're lower than this entity, so draw the arrow pointing up
                    else if ( vecLocal.fZ < vecMarker.fZ - 4.0f )
                    {
                        // If this is not the right blip
                        if ( eMapMarkerState != CClientRadarMarker::MAP_MARKER_TRIANGLE_UP )
                        {
                            // Set it to the right blip
                            (*markerIter)->SetMapMarkerState ( CClientRadarMarker::MAP_MARKER_TRIANGLE_UP );
                        }
                    }

                    // We're at the same level so draw a square
                    else
                    {
                        // If this is not the right blip
                        if ( eMapMarkerState != CClientRadarMarker::MAP_MARKER_SQUARE )
                        {
                            // Set it to the right blip
                            (*markerIter)->SetMapMarkerState ( CClientRadarMarker::MAP_MARKER_SQUARE );
                        }
                    }

                    // Grab the image pointer
                    pImage = (*markerIter)->GetMapMarkerImage ();

                    // If the image exists
                    if ( pImage )
                    {
                        // Set the size, position etc and show it here
                        g_pCore->GetGraphics()->DrawTexture ( pImage, vecPos.fX, vecPos.fY, fScale, fScale );
                    }
                }
            }
        }
        if ( !m_pModeText->IsVisible () )
            m_pModeText->SetVisible ( true );
    }
    else
    {
        if ( m_pModeText->IsVisible () )
            m_pModeText->SetVisible ( false );
    }
}


void CRadarMap::SetRadarEnabled ( bool bIsRadarEnabled )
{
    bool bAlreadyEnabled = ( m_bIsRadarEnabled || m_bForcedState );
    bool bWillShow = ( bIsRadarEnabled || m_bForcedState );
    if ( bAlreadyEnabled != bWillShow )
    {
        InternalSetRadarEnabled ( bWillShow );
    }
    m_bIsRadarEnabled = bIsRadarEnabled;
}


void CRadarMap::SetForcedState ( bool bState )
{
    bool bAlreadyShowing = ( m_bIsRadarEnabled || m_bForcedState );
    bool bWillShow = ( m_bIsRadarEnabled || bState );
    if ( bAlreadyShowing != bWillShow )
    {
        InternalSetRadarEnabled ( bWillShow );
    }
    m_bForcedState = bState;
}


void CRadarMap::InternalSetRadarEnabled ( bool bEnabled )
{
    if ( bEnabled )
    {   
        m_bChatVisible = g_pCore->IsChatVisible ();
        m_bDebugVisible = g_pCore->IsDebugVisible ();

        g_pGame->GetHud ()->Disable ( true );
        g_pMultiplayer->HideRadar ( true );
        g_pCore->SetChatVisible ( false );
        g_pCore->SetDebugVisible ( false );

		// Disable graphical effects
		g_pMultiplayer->DisableHeatHazeEffect ( true );
    }
    else
    {
        g_pGame->GetHud ()->Disable ( false );
        g_pMultiplayer->HideRadar ( false );
        g_pCore->SetChatVisible ( m_bChatVisible );
        g_pCore->SetDebugVisible ( m_bDebugVisible );

		// Enable graphical effects
		g_pMultiplayer->DisableHeatHazeEffect ( false );
	}
}


bool CRadarMap::CalculateEntityOnScreenPosition ( CClientEntity* pEntity, CVector2D& vecLocalPos, float fBlipSize, CVector2D vecBlipPosition )
{
    // If the entity exists
    if ( pEntity )
    {
        // If the X position is greater than 1.0
        if ( vecBlipPosition.fX > 1.0f )
        {
            // Set the X position to 1.0
            vecBlipPosition.fX = 1.0f;
        }
        // If the X position is less than 0.0
        else if ( vecBlipPosition.fX < 0.0f )
        {
            // Set the X position to 0.0
            vecBlipPosition.fX = 0.0f;
        }

        // If the Y position is greater than 1.0
        if ( vecBlipPosition.fY > 1.0f )
        {
            // Set the Y position to 1.0
            vecBlipPosition.fY = 1.0f;
        }
        // If the Y position is less than 0.0
        else if ( vecBlipPosition.fY < 0.0f )
        {
            // Set the Y position to 0.0
            vecBlipPosition.fY = 0.0f;
        }

        // Get the Entities ingame position
        CVector vecPosition;
        pEntity->GetPosition ( vecPosition );

        // Adjust to the map variables and create the map ratio
        float fX = vecPosition.fX + 3000.0f;
        float fY = vecPosition.fY + 3000.0f;
        float fRatio = 6000.0f / m_fMapSize;

        // Calculate the screen position for the marker
        vecLocalPos.fX = static_cast < float > ( m_iMapMinX ) + ( fX / fRatio ) - ( fBlipSize / ( 1 / vecBlipPosition.fX ) );
        vecLocalPos.fY = static_cast < float > ( m_iMapMaxY ) - ( fY / fRatio ) - ( fBlipSize / ( 1 / vecBlipPosition.fY ) );

        // If the position is on the screen
        if ( vecLocalPos.fX >= 0.0f &&
             vecLocalPos.fX <= static_cast < float > ( m_uiWidth ) &&
             vecLocalPos.fY >= 0.0f &&
             vecLocalPos.fY <= static_cast < float > ( m_uiHeight ) )
        {
            // Then return true as it is on the screen
            return true;
        }
    }

    // Return false as it is not on the screen
    return false;
}


bool CRadarMap::CalculateEntityOnScreenPosition ( CVector vecPosition, CVector2D& vecLocalPos, float fBlipSize, CVector2D vecBlipPosition )
{
    // If the X position is greater than 1.0
    if ( vecBlipPosition.fX > 1.0f )
    {
        // Set the X position to 1.0
        vecBlipPosition.fX = 1.0f;
    }
    // If the X position is less than 0.0
    else if ( vecBlipPosition.fX < 0.0f )
    {
        // Set the X position to 0.0
        vecBlipPosition.fX = 0.0f;
    }

    // If the Y position is greater than 1.0
    if ( vecBlipPosition.fY > 1.0f )
    {
        // Set the Y position to 1.0
        vecBlipPosition.fY = 1.0f;
    }
    // If the Y position is less than 0.0
    else if ( vecBlipPosition.fY < 0.0f )
    {
        // Set the Y position to 0.0
        vecBlipPosition.fY = 0.0f;
    }

    // Adjust to the map variables and create the map ratio
    float fX = vecPosition.fX + 3000.0f;
    float fY = vecPosition.fY + 3000.0f;
    float fRatio = 6000.0f / m_fMapSize;

    // Calculate the screen position for the marker
    vecLocalPos.fX = static_cast < float > ( m_iMapMinX ) + ( fX / fRatio ) - ( fBlipSize / ( 1 / vecBlipPosition.fX ) );
    vecLocalPos.fY = static_cast < float > ( m_iMapMaxY ) - ( fY / fRatio ) - ( fBlipSize / ( 1 / vecBlipPosition.fY ) );

    // If the position is on the screen
    if ( vecLocalPos.fX >= 0.0f &&
            vecLocalPos.fX <= static_cast < float > ( m_uiWidth ) &&
            vecLocalPos.fY >= 0.0f &&
            vecLocalPos.fY <= static_cast < float > ( m_uiHeight ) )
    {
        // Then return true as it is on the screen
        return true;
    }

    // Return false as it is not on the screen
    return false;
}


void CRadarMap::SetupMapVariables ( void )
{
    // Calculate the map size and the middle of the screen coords
    m_fMapSize = static_cast < float > ( m_uiHeight * m_ucZoom );
    int iMiddleX = static_cast < int > ( m_uiWidth / 2 );
    int iMiddleY = static_cast < int > ( m_uiHeight / 2 );

    // If we are attached to the local player
    if ( m_bIsAttachedToLocal )
    {
        // If we are zoomed in at all
        if ( m_ucZoom > 1 )
        {
            // Get the local player position
            CVector vec;
            CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager ()->GetLocalPlayer ();
            pLocalPlayer->GetPosition ( vec );

            // Calculate the maps min and max vector positions putting the local player in the middle of the map
            m_iMapMinX = static_cast < int > ( iMiddleX - ( iMiddleY * m_ucZoom ) - ( ( vec.fX * m_fMapSize ) / 6000.0f ) );
            m_iMapMaxX = static_cast < int > ( m_iMapMinX + m_fMapSize );
            m_iMapMinY = static_cast < int > ( iMiddleY - ( iMiddleY * m_ucZoom ) + ( ( vec.fY * m_fMapSize ) / 6000.0f ) );
            m_iMapMaxY = static_cast < int > ( m_iMapMinY + m_fMapSize );

            // If we are moving the map too far then stop centering the local player blip
            if ( m_iMapMinX > 0 )
            {
                m_iMapMinX = 0;
                m_iMapMaxX = static_cast < int > ( m_iMapMinX + m_fMapSize );
            }
            else if ( m_iMapMaxX <= static_cast < int > ( m_uiWidth ) )
            {
                m_iMapMaxX = m_uiWidth;
                m_iMapMinX = static_cast < int > ( m_iMapMaxX - m_fMapSize );
            }

            if ( m_iMapMinY > 0 )
            {
                m_iMapMinY = 0;
                m_iMapMaxY = static_cast < int > ( m_iMapMinY + m_fMapSize );
            }
            else if ( m_iMapMaxY <= static_cast < int > ( m_uiHeight ) )
            {
                m_iMapMaxY = m_uiHeight;
                m_iMapMinY = static_cast < int > ( m_iMapMaxY - m_fMapSize );
            }
        }
        // If we are not zoomed in
        else
        {
            // Set the map to the middle of the screen
            m_iMapMinX = static_cast < int > ( iMiddleX - iMiddleY );
            m_iMapMaxX = static_cast < int > ( iMiddleX + iMiddleY );
            m_iMapMinY = static_cast < int > ( iMiddleY - iMiddleY );
            m_iMapMaxY = static_cast < int > ( iMiddleY + iMiddleY );
        }

    }
    // If we are in free roam mode
    else
    {
        // Set the maps min and max vector positions relative to the movement selected
        m_iMapMinX = static_cast < int > ( iMiddleX - ( iMiddleY * m_ucZoom ) - ( ( m_iHorizontalMovement * m_fMapSize ) / 6000.0f ) );
        m_iMapMaxX = static_cast < int > ( m_iMapMinX + m_fMapSize );
        m_iMapMinY = static_cast < int > ( iMiddleY - ( iMiddleY * m_ucZoom ) + ( ( m_iVerticalMovement * m_fMapSize ) / 6000.0f )  );
        m_iMapMaxY = static_cast < int > ( m_iMapMinY + m_fMapSize );

        // If we are zoomed in
        if ( m_ucZoom > 1 )
        {
            if ( m_iMapMinX >= 0 )
            {
                m_iMapMinX = 0;
                m_iMapMaxX = static_cast < int > ( m_iMapMinX + m_fMapSize );
            }
            else if ( m_iMapMaxX <= static_cast < int > ( m_uiWidth ) )
            {
                m_iMapMaxX = m_uiWidth;
                m_iMapMinX = static_cast < int > ( m_iMapMaxX - m_fMapSize );
            }

            if ( m_iMapMinY >= 0 )
            {
                m_iMapMinY = 0;
                m_iMapMaxY = static_cast < int > ( m_iMapMinY + m_fMapSize );
            }
            else if ( m_iMapMaxY <= static_cast < int > ( m_uiHeight ) )
            {
                m_iMapMaxY = m_uiHeight;
                m_iMapMinY = static_cast < int > ( m_iMapMaxY - m_fMapSize );
            }
        }
        // If we are not zoomed in
        else
        {
            // Set the movement margins to 0
            m_iHorizontalMovement = 0;
            m_iVerticalMovement = 0;
        }
    }
}


void CRadarMap::ZoomIn ( void )
{
    if ( m_ucZoom <= 4 )
    {
        m_ucZoom = m_ucZoom * 2;
        SetupMapVariables ();
    }
}


void CRadarMap::ZoomOut ( void )
{
    if ( m_ucZoom >= 2 )
    {
        m_ucZoom = m_ucZoom / 2;

        if ( m_ucZoom > 1 )
        {
            m_iVerticalMovement = static_cast < int > ( m_iVerticalMovement / 1.7f );
            m_iHorizontalMovement = static_cast < int > ( m_iHorizontalMovement / 1.7f );
        }
        else
        {
            m_iVerticalMovement = 0;
            m_iHorizontalMovement = 0;
            // Stop the movement
            m_bIsMovingNorth = false;
            m_bIsMovingSouth = false;
            m_bIsMovingEast = false;
            m_bIsMovingWest = false;
        }

        SetupMapVariables ();
    }
}


void CRadarMap::MoveNorth ( void )
{
    if ( !m_bIsAttachedToLocal )
    {
        if ( m_ucZoom > 1 )
        {
            if ( m_iMapMinY >= 0 )
            {
                m_iMapMinY = 0;
                m_iMapMaxY = static_cast < int > ( m_iMapMinY + m_fMapSize );
            }
            else
            {
                m_iVerticalMovement = m_iVerticalMovement + 20;
                SetupMapVariables ();
            }
        }
    }
}


void CRadarMap::MoveSouth ( void )
{
    if ( !m_bIsAttachedToLocal )
    {
        if ( m_ucZoom > 1 )
        {
            if ( m_iMapMaxY <= static_cast < int > ( m_uiHeight ) )
            {
                m_iMapMaxY = m_uiHeight;
                m_iMapMinY = static_cast < int > ( m_iMapMaxY - m_fMapSize );
            }
            else
            {
                m_iVerticalMovement = m_iVerticalMovement - 20;
                SetupMapVariables ();
            }
        }
    }
}


void CRadarMap::MoveEast ( void )
{
    if ( !m_bIsAttachedToLocal )
    {
        if ( m_ucZoom > 1 )
        {
            if ( m_iMapMaxX <= static_cast < int > ( m_uiWidth ) )
            {
                m_iMapMaxX = m_uiWidth;
                m_iMapMinX = static_cast < int > ( m_iMapMaxX - m_fMapSize );
            }
            else
            {
                m_iHorizontalMovement = m_iHorizontalMovement + 20;
                SetupMapVariables ();
            }
        }
    }
}


void CRadarMap::MoveWest ( void )
{
    if ( !m_bIsAttachedToLocal )
    {
        if ( m_ucZoom > 1 )
        {
            if ( m_iMapMinX >= 0 )
            {
                m_iMapMinX = 0;
                m_iMapMaxX = static_cast < int > ( m_iMapMinX + m_fMapSize );
            }
            else
            {
                m_iHorizontalMovement = m_iHorizontalMovement - 20;
                SetupMapVariables ();
            }
        }
    }
}


void CRadarMap::SetAttachedToLocalPlayer ( bool bIsAttachedToLocal )
{
    m_bIsAttachedToLocal = bIsAttachedToLocal;
    SetupMapVariables ();

    if ( m_bIsAttachedToLocal )
    {
        m_pModeText->SetCaption ( "Current Mode: Attached to local player" );
    }
    else
    {
        m_pModeText->SetCaption ( "Current Mode: Free Move" );
    }
}


bool CRadarMap::IsRadarShowing ( void )
{
    return ( ( m_bIsRadarEnabled || m_bForcedState ) &&
             m_pRadarImage &&
             m_pLocalPlayerBlip &&
             ( !g_pCore->GetConsole ()->IsVisible () && !g_pCore->IsMenuVisible () ) );
}

bool CRadarMap::GetBoundingBox ( CVector &vecMin, CVector &vecMax )
{
    // If our radar image exists (Values are not calculated unless map is showing)
    if ( IsRadarShowing () )
	{
		vecMin.fX = m_iMapMinX;
		vecMin.fY = m_iMapMinY;

		vecMax.fX = m_iMapMaxX;
		vecMax.fY = m_iMapMaxY;

		return true;
	}
	else
	{
		return false;
	}
}