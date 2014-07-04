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

#include "StdInc.h"

using SharedUtil::CalcMTASAPath;
using std::list;

enum
{
    MARKER_SQUARE_INDEX         = 0,
    MARKER_UP_TRIANGLE_INDEX    = 1,
    MARKER_DOWN_TRIANGLE_INDEX  = 2,
    MARKER_FIRST_SPRITE_INDEX   = 3,
    MARKER_LAST_SPRITE_INDEX    = MARKER_FIRST_SPRITE_INDEX + RADAR_MARKER_LIMIT - 1,
};

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
    m_bHideHelpText = false;

    // Set the movement bools
    m_bIsMovingNorth = false;
    m_bIsMovingSouth = false;
    m_bIsMovingEast = false;
    m_bIsMovingWest = false;
    m_bTextVisible = false;

    // Set the update time to the current time
    m_ulUpdateTime = GetTickCount32 ();

    // Get the window sizes and set the map variables to default zoom/movement
    m_uiHeight = g_pCore->GetGraphics ()->GetViewportHeight ();
    m_uiWidth = g_pCore->GetGraphics ()->GetViewportWidth ();
    m_fZoom = 1;
    m_iHorizontalMovement = 0;
    m_iVerticalMovement = 0;
    SetupMapVariables ();

    // Create the radar and local player blip images
    m_pRadarImage = g_pCore->GetGraphics()->GetRenderItemManager ()->CreateTexture ( CalcMTASAPath("MTA\\cgui\\images\\radar.jpg"), NULL, false, 1024, 1024, RFORMAT_DXT1 );
    m_pLocalPlayerBlip = g_pCore->GetGraphics()->GetRenderItemManager ()->CreateTexture ( CalcMTASAPath("MTA\\cgui\\images\\radarset\\02.png") );

    // Create the marker textures
    CreateMarkerTextures ();

    // Create the text displays for the help text
    struct {
        SColor color;
        float fPosY;
        float fScale;
        SString strMessage;
    } messageList [] = {
        { SColorRGBA ( 255, 255, 255, 200 ), 0.92f, 1.5f, "Current Mode: Kill all humans" },
        { SColorRGBA ( 255, 255, 255, 255 ), 0.95f, 1.0f, SString ( "Press %s to change mode.", *GetBoundKeyName ( "radar_attach" ) ) },
        { SColorRGBA ( 255, 255, 255, 255 ), 0.05f, 1.0f, SString ( "Press %s/%s to zoom in/out.", *GetBoundKeyName ( "radar_zoom_in" ), *GetBoundKeyName ( "radar_zoom_out" ) ) },
        { SColorRGBA ( 255, 255, 255, 255 ), 0.08f, 1.0f, SString ( "Press %s, %s, %s, %s to navigate the map.", *GetBoundKeyName ( "radar_move_north" ), *GetBoundKeyName ( "radar_move_east" ), *GetBoundKeyName ( "radar_move_south" ), *GetBoundKeyName ( "radar_move_west" ) ) },
        { SColorRGBA ( 255, 255, 255, 255 ), 0.11f, 1.0f, SString ( "Press %s/%s to change opacity.", *GetBoundKeyName ( "radar_opacity_down" ), *GetBoundKeyName ( "radar_opacity_up" ) ) },
        { SColorRGBA ( 255, 255, 255, 255 ), 0.14f, 1.0f, SString ( "Press %s to hide the map.", *GetBoundKeyName ( "radar" ) ) },
        { SColorRGBA ( 255, 255, 255, 255 ), 0.17f, 1.0f, SString ( "Press %s to hide this help text.", *GetBoundKeyName ( "radar_help" ) ) },
    };

    for ( uint i = 0 ; i < NUMELMS( messageList ) ; i++ )
    {
        CClientTextDisplay* pTextDisplay = new CClientTextDisplay ( m_pManager->GetDisplayManager () );
        pTextDisplay->SetCaption ( messageList[i].strMessage );
        pTextDisplay->SetColor( messageList[i].color );
        pTextDisplay->SetPosition ( CVector ( 0.50f, messageList[i].fPosY, 0 ) );
        pTextDisplay->SetFormat ( DT_CENTER | DT_VCENTER );
        pTextDisplay->SetScale ( messageList[i].fScale );
        pTextDisplay->SetVisible ( false );

        m_HelpTextList.push_back ( pTextDisplay );
    }

    // Default to attached to player
    SetAttachedToLocalPlayer ( true );
}


CRadarMap::~CRadarMap ( void )
{
    // Delete our images
    SAFE_RELEASE( m_pRadarImage );
    SAFE_RELEASE( m_pLocalPlayerBlip );

    for ( uint i = 0 ; i < m_MarkerTextureList.size () ; i++ )
        SAFE_RELEASE( m_MarkerTextureList[i] );

    m_MarkerTextureList.clear ();

    // Don't need to delete the help texts as those are destroyed by the display manager
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
        if ( GetTickCount32 () >= m_ulUpdateTime + 50 )
        {
            // Set the update time
            m_ulUpdateTime = GetTickCount32 ();

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
    }
}


//
// Precreate all the textures for the radar map markers
//
void CRadarMap::CreateMarkerTextures ( void )
{
    assert ( m_MarkerTextureList.empty () );
    SString strRadarSetDirectory = CalcMTASAPath ( "MTA\\cgui\\images\\radarset\\" );

    // Load the 3 shapes
    const char* shapeFileNames[] = { "square.png", "up.png", "down.png" };
    for ( uint i = 0 ; i < NUMELMS( shapeFileNames ) ; i++ )
    {
        CTextureItem* pTextureItem = g_pCore->GetGraphics()->GetRenderItemManager ()->CreateTexture ( PathJoin ( strRadarSetDirectory, shapeFileNames[i] ) );
        m_MarkerTextureList.push_back ( pTextureItem );
    }

    assert ( m_MarkerTextureList.size () == MARKER_FIRST_SPRITE_INDEX );

    // Load the icons
    for ( uint i = 0 ; i < RADAR_MARKER_LIMIT ; i++ )
    {
        CTextureItem* pTextureItem = g_pCore->GetGraphics()->GetRenderItemManager ()->CreateTexture ( PathJoin ( strRadarSetDirectory, SString ( "%02u.png", i + 1 ) ) );
        m_MarkerTextureList.push_back ( pTextureItem );
    }

    assert ( m_MarkerTextureList.size () == MARKER_LAST_SPRITE_INDEX + 1 );
}


//
// Get a texture for a marker, including scale and color
//
CTextureItem* CRadarMap::GetMarkerTexture ( CClientRadarMarker* pMarker, float fLocalZ, float* pfScale, SColor* pColor )
{
    float fScale = pMarker->GetScale ();
    ulong ulSprite = pMarker->GetSprite ();
    SColor color = pMarker->GetColor ();

    // Make list index
    uint uiListIndex = 0;

    if ( ulSprite )
    {
        // ulSprite >= 1 and <= 63
        // Remap to texture list index
        uiListIndex = ulSprite - 1 + MARKER_FIRST_SPRITE_INDEX;
        color = SColorARGB ( 255, 255, 255, 255 );
        fScale = 1;
    }
    else
    {
        // ulSprite == 0 so draw a square or triangle depending on relative z position
        CVector vecMarker;
        pMarker->GetPosition ( vecMarker );

        if ( fLocalZ > vecMarker.fZ + 4.0f )
            uiListIndex = MARKER_DOWN_TRIANGLE_INDEX;   // We're higher than this marker, so draw the arrow pointing down
        else
        if ( fLocalZ < vecMarker.fZ - 4.0f )
            uiListIndex = MARKER_UP_TRIANGLE_INDEX;     // We're lower than this entity, so draw the arrow pointing up
        else
            uiListIndex = MARKER_SQUARE_INDEX;          // We're at the same level so draw a square

        fScale /= 4;
    }

    *pfScale = fScale;
    *pColor = color;

    if ( uiListIndex >= m_MarkerTextureList.size () )
        return NULL;

    return m_MarkerTextureList [ uiListIndex ];
}


void CRadarMap::DoRender ( void )
{
    bool bIsRadarShowing = IsRadarShowing ();

    // Render if showing
    if ( bIsRadarShowing )
    {
        // Get the alpha value from the settings
        int iRadarAlpha;
        g_pCore->GetCVars()->Get ( "mapalpha", iRadarAlpha );

        g_pCore->GetGraphics()->DrawTexture ( m_pRadarImage, static_cast < float > ( m_iMapMinX ),
                                                             static_cast < float > ( m_iMapMinY ),
                                                             m_fMapSize / m_pRadarImage->m_uiSizeX,
                                                             m_fMapSize / m_pRadarImage->m_uiSizeY,
                                                             0.0f, 0.0f, 0.0f,
                                                             SColorARGB ( iRadarAlpha, 255, 255, 255 ) );

        // Grab the info for the local player blip
        CVector2D vecLocalPos;
        CVector vecLocal;
        CVector vecLocalRot;
        if ( m_pManager->GetCamera()->IsInFixedMode() )
        {
            m_pManager->GetCamera()->GetPosition ( vecLocal );
            m_pManager->GetCamera()->GetRotation ( vecLocalRot );
        }
        else
        {
            CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager ()->GetLocalPlayer ();
            if ( !pLocalPlayer )
                return;
            pLocalPlayer->GetPosition ( vecLocal );
            pLocalPlayer->GetRotationDegrees ( vecLocalRot );
        }

        CalculateEntityOnScreenPosition ( vecLocal, vecLocalPos );

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
                CalculateEntityOnScreenPosition ( pArea, vecPos );

                // Get the area size and work out the ratio
                CVector2D vecSize;
                float fX = (*areaIter)->GetSize ().fX;
                float fY = (*areaIter)->GetSize ().fY;
                float fRatio = 6000.0f / m_fMapSize;

                // Calculate the size of the area
                vecSize.fX = static_cast < float > ( fX / fRatio );
                vecSize.fY = static_cast < float > ( fY / fRatio );
                g_pCore->GetGraphics ()->DrawRectangle ( vecPos.fX, vecPos.fY, vecSize.fX, -vecSize.fY, pArea->GetColor () );
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
                float fScale = 1;
                SColor color;
                CTextureItem* pTexture = GetMarkerTexture ( *markerIter, vecLocal.fZ, &fScale, &color );

                if ( pTexture )
                {
                    CVector2D vecPos;
                    CalculateEntityOnScreenPosition ( *markerIter, vecPos );
                    g_pCore->GetGraphics()->DrawTexture ( pTexture, vecPos.fX, vecPos.fY, fScale, fScale, 0.0f, 0.5f, 0.5f, color );
                }
            }
        }

        g_pCore->GetGraphics()->DrawTexture ( m_pLocalPlayerBlip, vecLocalPos.fX, vecLocalPos.fY, 1.0, 1.0, vecLocalRot.fZ, 0.5f, 0.5f );
    }

    // Update visibility of help text
    bool bRequiredTextVisible = bIsRadarShowing && !m_bHideHelpText;
    if ( bRequiredTextVisible != m_bTextVisible )
    {
        m_bTextVisible = bRequiredTextVisible;
        for ( uint i = 0 ; i < m_HelpTextList.size () ; i++ )
            m_HelpTextList[i]->SetVisible ( m_bTextVisible );

        SetupMapVariables ();
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
    }
    else
    {
        g_pGame->GetHud ()->Disable ( false );
        g_pMultiplayer->HideRadar ( false );
        g_pCore->SetChatVisible ( m_bChatVisible );
        g_pCore->SetDebugVisible ( m_bDebugVisible );
    }
}


bool CRadarMap::CalculateEntityOnScreenPosition ( CClientEntity* pEntity, CVector2D& vecLocalPos )
{
    // If the entity exists
    if ( pEntity )
    {
        // Get the Entities ingame position
        CVector vecPosition;
        pEntity->GetPosition ( vecPosition );

        // Adjust to the map variables and create the map ratio
        float fX = vecPosition.fX + 3000.0f;
        float fY = vecPosition.fY + 3000.0f;
        float fRatio = 6000.0f / m_fMapSize;

        // Calculate the screen position for the marker
        vecLocalPos.fX = static_cast < float > ( m_iMapMinX ) + ( fX / fRatio );
        vecLocalPos.fY = static_cast < float > ( m_iMapMaxY ) - ( fY / fRatio );

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


bool CRadarMap::CalculateEntityOnScreenPosition ( CVector vecPosition, CVector2D& vecLocalPos )
{
    // Adjust to the map variables and create the map ratio
    float fX = vecPosition.fX + 3000.0f;
    float fY = vecPosition.fY + 3000.0f;
    float fRatio = 6000.0f / m_fMapSize;

    // Calculate the screen position for the marker
    vecLocalPos.fX = static_cast < float > ( m_iMapMinX ) + ( fX / fRatio );
    vecLocalPos.fY = static_cast < float > ( m_iMapMaxY ) - ( fY / fRatio );

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
    m_fMapSize = static_cast < float > ( m_uiHeight * m_fZoom );
    int iMiddleX = static_cast < int > ( m_uiWidth / 2 );
    int iMiddleY = static_cast < int > ( m_uiHeight / 2 );

    // If we are attached to the local player and zoomed in at all
    if ( m_bIsAttachedToLocal && m_fZoom > 1 )
    {
        // Get the local player position
        CVector vec;
        CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager ()->GetLocalPlayer ();
        if ( pLocalPlayer )
            pLocalPlayer->GetPosition ( vec );

        // Calculate the maps min and max vector positions putting the local player in the middle of the map
        m_iMapMinX = static_cast < int > ( iMiddleX - ( iMiddleY * m_fZoom ) - ( ( vec.fX * m_fMapSize ) / 6000.0f ) );
        m_iMapMaxX = static_cast < int > ( m_iMapMinX + m_fMapSize );
        m_iMapMinY = static_cast < int > ( iMiddleY - ( iMiddleY * m_fZoom ) + ( ( vec.fY * m_fMapSize ) / 6000.0f ) );
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
    // If we are in free roam mode or not zoomed in
    else
    {
        // Set the maps min and max vector positions relative to the movement selected
        m_iMapMinX = static_cast < int > ( iMiddleX - ( iMiddleY * m_fZoom ) - ( ( m_iHorizontalMovement * m_fMapSize ) / 6000.0f ) );
        m_iMapMaxX = static_cast < int > ( m_iMapMinX + m_fMapSize );
        m_iMapMinY = static_cast < int > ( iMiddleY - ( iMiddleY * m_fZoom ) + ( ( m_iVerticalMovement * m_fMapSize ) / 6000.0f )  );
        m_iMapMaxY = static_cast < int > ( m_iMapMinY + m_fMapSize );

        // If we are zoomed in
        if ( m_fZoom > 1 )
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

    // Show mode only when zoomed in
    if ( !m_HelpTextList.empty () )
    {
        m_HelpTextList[0]->SetVisible ( m_fZoom > 1 && m_bTextVisible );
        m_HelpTextList[1]->SetVisible ( m_fZoom > 1 && m_bTextVisible );
    }
}


void CRadarMap::ZoomIn ( void )
{
    if ( m_fZoom <= 4 )
    {
        m_fZoom = m_fZoom * 2;
        SetupMapVariables ();
    }
}


void CRadarMap::ZoomOut ( void )
{
    if ( m_fZoom >= 1 )
    {
        m_fZoom = m_fZoom / 2;

        if ( m_fZoom > 1 )
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
        if ( m_fZoom > 1 )
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
        if ( m_fZoom > 1 )
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
        if ( m_fZoom > 1 )
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
        if ( m_fZoom > 1 )
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
        m_HelpTextList[0]->SetCaption ( "Current Mode: Attached to local player" );
    }
    else
    {
        m_HelpTextList[0]->SetCaption ( "Current Mode: Free Move" );
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
        vecMin.fX = static_cast < float > ( m_iMapMinX );
        vecMin.fY = static_cast < float > ( m_iMapMinY );

        vecMax.fX = static_cast < float > ( m_iMapMaxX );
        vecMax.fY = static_cast < float > ( m_iMapMaxY );

        return true;
    }
    else
    {
        return false;
    }
}

void CRadarMap::ToggleHelpText ( void )
{
    m_bHideHelpText = !m_bHideHelpText;
}

SString CRadarMap::GetBoundKeyName ( const SString& strCommand )
{
    CCommandBind* pCommandBind = g_pCore->GetKeyBinds () -> GetBindFromCommand ( strCommand, 0, 0, 0, false, 0 );
    if ( !pCommandBind )
        return strCommand;
    return pCommandBind->boundKey->szKey;
}
