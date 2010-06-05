/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarMarker.cpp
*  PURPOSE:     Radar marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using SharedUtil::CalcMTASAPath;

CClientRadarMarker::CClientRadarMarker ( CClientManager* pManager, ElementID ID, short sOrdering, float fVisibleDistance ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pRadarMarkerManager = pManager->GetRadarMarkerManager ();

    SetTypeName ( "blip" );

    m_bIsVisible = true;
    m_pMarker = NULL;
    m_usScale = 2;
    m_ulSprite = 0;
    m_ucRed = 0;
    m_ucGreen = 0;
    m_ucBlue = 255;
    m_ucAlpha = 255;
    m_pMapMarkerImage = NULL;
    m_eMapMarkerState = MAP_MARKER_OTHER;
    m_sOrdering = sOrdering;
    m_fVisibleDistance = fVisibleDistance;

    // Add us to the radar marker manager list
    m_pRadarMarkerManager->AddToList ( this );

    // Try to create it
    Create ();
}


CClientRadarMarker::~CClientRadarMarker ( void )
{
    // Try to destroy it
    Destroy ();

    // Remove us from the marker manager
    Unlink ();
}


void CClientRadarMarker::Unlink ( void )
{
    m_pRadarMarkerManager->RemoveFromList ( this );
}


void CClientRadarMarker::DoPulse ( void )
{    
    // Are we attached to something?
    if ( m_pAttachedToEntity )
    {
        CVector vecPosition;
        m_pAttachedToEntity->GetPosition ( vecPosition );        
        vecPosition += m_vecAttachedPosition;
        SetPosition ( vecPosition );

        // TODO: move to ::SetElementDimension
        if ( m_usDimension != m_pAttachedToEntity->GetDimension () )
        {
            SetDimension ( m_pAttachedToEntity->GetDimension () );
        }
    }

    // Are we in a visible distance?
    if ( IsInVisibleDistance () )
    {
        // Are we not created?
        if ( !m_pMarker )
        {
            // Are we visible and in the right dimension?
            if ( m_bIsVisible && m_usDimension == m_pManager->GetRadarMarkerManager ()->GetDimension () )
            {
                // Recreate our marker
                CreateMarker ();
            }
        }
    }
    else
    {
        // Are we created?
        if ( m_pMarker )
        {
            // Destroy our marker
            DestroyMarker ();
        }
    }
}


void CClientRadarMarker::SetPosition ( const CVector& vecPosition )
{
    if ( m_pMarker )
    {
        m_pMarker->SetPosition ( const_cast < CVector* > ( &vecPosition ) );
    }

    m_vecPosition = vecPosition;
}


void CClientRadarMarker::GetPosition ( CVector& vecPosition ) const
{
    if ( m_pMarker )
    {
        vecPosition = *m_pMarker->GetPosition ();
    }
    else
    {
        vecPosition = m_vecPosition;
    }
}


void CClientRadarMarker::SetScale ( unsigned short usScale )
{
    m_usScale = usScale;

    if ( m_pMarker )
    {
        m_pMarker->SetScale ( usScale );
    }
}


void CClientRadarMarker::SetColor ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha )
{
    // Store the color
    m_ucRed = ucRed;
    m_ucGreen = ucGreen;
    m_ucBlue = ucBlue;
    m_ucAlpha = ucAlpha;

    DWORD dwColor;

    // Supposed to be black? Make it almost black as entirely black
    // might make us hit some reserved colors. Which'd be unexpected.
    if ( ucRed == 0 &&
         ucGreen == 0 &&
         ucBlue == 0 )
    {
        dwColor = COLOR_ARGB ( 1, 1, 1, ucAlpha );
    }
    else
    {
        dwColor = COLOR_ARGB ( ucRed, ucGreen, ucBlue, ucAlpha );
    }

    // Set the color
    if ( m_pMarker )
    {
        m_pMarker->SetColor ( dwColor );
    }

    SetMapMarkerState ( m_eMapMarkerState );
}


void CClientRadarMarker::SetSprite ( unsigned long ulSprite )
{
    m_ulSprite = ulSprite;

    if ( m_pMarker )
    {
        m_pMarker->SetSprite ( static_cast < eMarkerSprite > ( ulSprite ) );
	}
    if ( ulSprite == 0 )
    {
		if ( m_pMapMarkerImage )
		{
			m_pMapMarkerImage->Release();
			m_pMapMarkerImage = NULL;
		}

		SetMapMarkerState ( MAP_MARKER_SQUARE );
	}
	else if ( ulSprite <= RADAR_MARKER_LIMIT )
	{
		m_eMapMarkerState = MAP_MARKER_OTHER;

		if ( m_pMapMarkerImage )
		{
			m_pMapMarkerImage->Release();
			m_pMapMarkerImage = NULL;
		}

	    SString strSprite ( "MTA\\cgui\\images\\radarset\\%02u.png", ulSprite );
        m_pMapMarkerImage = g_pCore->GetGraphics()->LoadTexture ( CalcMTASAPath ( strSprite ) );
	}
}


void CClientRadarMarker::SetVisible ( bool bVisible )
{
    // Are we in the right dimension?
    if ( m_usDimension == m_pManager->GetRadarMarkerManager ()->GetDimension () )
    {
        // Visible now but weren't last time?
        if ( bVisible && !m_bIsVisible )
        {
            Create ();
        }

        // Invisible now but weren't last time? Destroy it
        else if ( !bVisible && m_bIsVisible )
        {
            Destroy ();
        }
    }

    // Update visibility
    m_bIsVisible = bVisible;
}


bool CClientRadarMarker::Create ( void )
{
    m_pManager->GetRadarMarkerManager ()->m_bOrderOnPulse = true;

    return true;
}


void CClientRadarMarker::InternalCreate ( void )
{
    // Not already got a marker?
    if ( !m_pMarker )
    {
        CreateMarker ();

        // Create the marker on the full size map
        SetSprite ( m_ulSprite );
    }
}


void CClientRadarMarker::Destroy ( void )
{
    DestroyMarker ();

	if ( m_pMapMarkerImage )
	{
		m_pMapMarkerImage->Release();
		m_pMapMarkerImage = NULL;
	}
}


void CClientRadarMarker::CreateMarker ( void )
{
    // Not already got a marker?
    if ( !m_pMarker )
    {
        if ( IsInVisibleDistance () )
        {
            // Create the marker
            m_pMarker = g_pGame->GetRadar ()->CreateMarker ( &m_vecPosition );
            if ( m_pMarker )
            {
                DWORD dwColor;

                // Supposed to be black? Make it almost black as entirely black
                // might make us hit some reserved colors. Which'd be unexpected.
                if ( m_ucRed == 0 &&
                    m_ucGreen == 0 &&
                    m_ucBlue == 0 )
                {
                    dwColor = COLOR_ARGB ( 1, 1, 1, m_ucAlpha );
                }
                else
                {
                    dwColor = COLOR_ARGB ( m_ucRed, m_ucGreen, m_ucBlue, m_ucAlpha );
                }

                // Set the properties
                m_pMarker->SetPosition ( &m_vecPosition );
                m_pMarker->SetScale ( m_usScale );
                m_pMarker->SetColor ( dwColor );
                m_pMarker->SetSprite ( static_cast < eMarkerSprite > ( m_ulSprite ) );
            }
        }
    }
}


void CClientRadarMarker::DestroyMarker ( void )
{
    if ( m_pMarker )
    {
        m_pMarker->Remove ();
        m_pMarker = NULL;
    }
}


void CClientRadarMarker::SetMapMarkerState ( EMapMarkerState eMapMarkerState )
{
    m_eMapMarkerState = eMapMarkerState;

    if ( !m_ulSprite )
    {
        DWORD dwBitMap[MAP_MARKER_WIDTH*MAP_MARKER_HEIGHT];

		if ( m_pMapMarkerImage )
		{
			m_pMapMarkerImage->Release();
			m_pMapMarkerImage = NULL;
		}

        switch ( eMapMarkerState )
        {
            case MAP_MARKER_SQUARE:
            {
                GetSquareTexture ( dwBitMap );
                m_pMapMarkerImage = g_pCore->GetGraphics()->CreateTexture ( dwBitMap, MAP_MARKER_WIDTH, MAP_MARKER_HEIGHT );
                break;
            }
            case MAP_MARKER_TRIANGLE_UP:
            {
                GetUpTriangleTexture ( dwBitMap );
                m_pMapMarkerImage = g_pCore->GetGraphics()->CreateTexture ( dwBitMap, MAP_MARKER_WIDTH, MAP_MARKER_HEIGHT );
                break;
            }
            case MAP_MARKER_TRIANGLE_DOWN:
            {
                GetDownTriangleTexture ( dwBitMap );
                m_pMapMarkerImage = g_pCore->GetGraphics()->CreateTexture ( dwBitMap, MAP_MARKER_WIDTH, MAP_MARKER_HEIGHT );
                break;
            }
            default: break;
        }
    }
}


void CClientRadarMarker::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    RelateDimension ( m_pManager->GetRadarMarkerManager ()->GetDimension () );
}


void CClientRadarMarker::RelateDimension ( unsigned short usDimension )
{
    // Are we visible?
    if ( m_bIsVisible )
    {
        if ( usDimension == m_usDimension )
        {
            Create ();
        }
        else
        {
            Destroy ();
        }
    }
}


void CClientRadarMarker::SetOrdering ( short sOrdering )
{
    if ( sOrdering != m_sOrdering )
    {
        m_sOrdering = sOrdering;
        m_pManager->GetRadarMarkerManager ()->m_bOrderOnPulse = true;
    }
}


bool CClientRadarMarker::IsInVisibleDistance ( void )
{
    float fDistance = DistanceBetweenPoints3D ( m_vecPosition, m_pRadarMarkerManager->m_vecCameraPosition );
    return ( fDistance <= m_fVisibleDistance );
}


void CClientRadarMarker::GetSquareTexture ( DWORD dwBitMap[] )
{
    DWORD dwA = COLOR_ARGB ( 0, 0, 0, 0 );
    DWORD dwB = COLOR_ARGB ( m_ucAlpha, 0, 0, 0 );
    DWORD dwC = COLOR_ARGB ( m_ucAlpha, m_ucRed, m_ucGreen, m_ucBlue );

    DWORD dwBitMapTemp[MAP_MARKER_WIDTH*MAP_MARKER_HEIGHT] = 
    {
/*0*/   dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*1*/   dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*2*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*3*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*4*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*5*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*6*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*7*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*8*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*9*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*10*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*11*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*12*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*13*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*14*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*15*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*16*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*17*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*18*/  dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*19*/  dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
    };

    memcpy ( dwBitMap, dwBitMapTemp, sizeof ( dwBitMapTemp ) );
}


void CClientRadarMarker::GetUpTriangleTexture ( DWORD dwBitMap[] )
{
    DWORD dwA = COLOR_ARGB ( 0, 0, 0, 0 );
    DWORD dwB = COLOR_ARGB ( m_ucAlpha, 0, 0, 0 );
    DWORD dwC = COLOR_ARGB ( m_ucAlpha, m_ucRed, m_ucGreen, m_ucBlue );

    DWORD dwBitMapTemp[MAP_MARKER_WIDTH*MAP_MARKER_HEIGHT] = 
    {
/*0*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*1*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*2*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*3*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*4*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*5*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*6*/   dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA,
/*7*/   dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA,
/*8*/   dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA,
/*9*/   dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA,
/*10*/  dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA,
/*11*/  dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA,
/*12*/  dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA,
/*13*/  dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA,
/*14*/  dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA,
/*15*/  dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA,
/*16*/  dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA,
/*17*/  dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA,
/*18*/  dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*19*/  dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB
    };

    memcpy ( dwBitMap, dwBitMapTemp, sizeof ( dwBitMapTemp ) );
}


void CClientRadarMarker::GetDownTriangleTexture ( DWORD dwBitMap[] )
{
    DWORD dwA = COLOR_ARGB ( 0, 0, 0, 0 );
    DWORD dwB = COLOR_ARGB ( m_ucAlpha, 0, 0, 0 );
    DWORD dwC = COLOR_ARGB ( m_ucAlpha, m_ucRed, m_ucGreen, m_ucBlue );

    DWORD dwBitMapTemp[MAP_MARKER_WIDTH*MAP_MARKER_HEIGHT] = 
    {
/*0*/   dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*1*/   dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*2*/   dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA,
/*3*/   dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA,
/*4*/   dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA,
/*5*/   dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA,
/*6*/   dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA,
/*7*/   dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA,
/*8*/   dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA,
/*9*/   dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA,
/*10*/  dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA,
/*11*/  dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA,
/*12*/  dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA,
/*13*/  dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA,
/*14*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*15*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*16*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*17*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*18*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*19*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
    };

    memcpy ( dwBitMap, dwBitMapTemp, sizeof ( dwBitMapTemp ) );
}
