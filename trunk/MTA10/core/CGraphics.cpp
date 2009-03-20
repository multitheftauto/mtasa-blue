/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CGraphics.cpp
*  PURPOSE:     General graphics subsystem
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

template<> CGraphics * CSingleton< CGraphics >::m_pSingleton = NULL;

const unsigned char g_szPixel [] = { 0x42, 0x4D, 0x3A, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0, 0x28, 0, 0,
                                    0, 0x1, 0, 0, 0, 0x1, 0, 0, 0, 0x1, 0, 0x18, 0, 0, 0, 0, 0,
                                    0x4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0xFF, 0xFF, 0xFF, 0 };

CGraphics::CGraphics ( CLocalGUI* pGUI )
{
    m_pGUI = pGUI;
    m_pFont = NULL;
    m_pDXDefaultFont = NULL;
    m_pDXBigDefaultFont = NULL;
    m_pDXDefaultBoldFont = NULL;
    m_pDXClearFont = NULL;
    m_pDXArialFont = NULL;
    m_pDXSansFont = NULL;
    m_pDXPricedownFont = NULL;
	m_pDXBankGothicFont = NULL;
	m_pDXDiplomaFont = NULL;
	m_pDXBeckettFont = NULL;
	m_pDevice = NULL;
    m_pLineInterface = NULL;

	m_pRenderTarget = NULL;
	m_pOriginalTarget = NULL;

    m_bIsDrawing = false;
    m_bCEGUI = false;
}


CGraphics::~CGraphics ( void )
{
    if ( m_pLineInterface )
        m_pLineInterface->Release ();

	DestroyFonts ();
    ExpireCachedTextures ( true );
}


void CGraphics::BeginDrawing ( void )
{
    if ( !m_bIsDrawing )
    {
        m_pGUI->GetRenderingLibrary ()->UTIL_CaptureDeviceState ( );
        m_pGUI->GetRenderingLibrary ()->BeginDrawing ();
		
		if ( m_pRenderTarget ) {
			// Set the render target we want
			HRESULT x = m_pDevice->SetRenderTarget ( 0, m_pRenderTarget );
			assert ( x == D3D_OK );

			// Clear the buffer
			m_pDevice->Clear ( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1, 0 );
		} else
			m_pDevice->SetRenderTarget ( 0, m_pOriginalTarget );

        m_bIsDrawing = true;
    }
}

void CGraphics::EndDrawing ( void )
{
    if ( m_bIsDrawing )
    {
		if ( m_pRenderTarget ) {
			// Set the render target back to the original one
			m_pDevice->SetRenderTarget ( 0, m_pOriginalTarget );
		}
        m_pGUI->GetRenderingLibrary ()->EndDrawing ();
        m_pGUI->GetRenderingLibrary ()->UTIL_RestoreDeviceState ( );
        m_bIsDrawing = false;
    }
}

int CGraphics::GetTextExtent ( const char* szText, float fScale )
{
    // If we got a font, use it to calculate the extent of the given text
    if ( m_pFont )
    {
        return static_cast < int > ( m_pFont->GetTextExtent ( szText, fScale / 2 ) );
    }
    return 0;
}


int CGraphics::GetTextHeight ( float fScale )
{
    if ( m_pFont )
    {
        return static_cast < int > ( m_pFont->GetFontHeight ( fScale / 2 ) );
    }
    return 0;
}


void CGraphics::DrawText ( int uiLeft, int uiTop, int uiRight, int uiBottom, unsigned long ulColor, const char* szText, float fScaleX, float fScaleY, unsigned long ulFormat, LPD3DXFONT pDXFont )
{   
	// Do not accept NULL text strings or invalid sprites
	if ( !szText || !m_pDXSprite )
		return;
    
	// If no font was specified, use the default font
	if ( !pDXFont ) pDXFont = GetFont ();

    // We're using a big font to keep it looking nice, so get the actual scale
    if ( pDXFont == m_pDXDefaultFont && ( fScaleX > 1.1f || fScaleY > 1.1f ) )
    {
        pDXFont = m_pDXBigDefaultFont;
        fScaleX /= 4.0f;
        fScaleY /= 4.0f;
    }

	// Check for a valid font
    if ( pDXFont )
    {
        // Prevent the rect from getting scaled along with the size
        uiLeft = unsigned int ( ( float ) uiLeft * ( 1.0f / fScaleX ) );
        uiTop = unsigned int ( ( float ) uiTop * ( 1.0f / fScaleY ) );
        uiRight = unsigned int ( ( float ) uiRight * ( 1.0f / fScaleX ) );
        uiBottom = unsigned int ( ( float ) uiBottom * ( 1.0f / fScaleY ) );

        RECT rect;        
        SetRect ( &rect, uiLeft, uiTop, uiRight, uiBottom );  

        D3DXMATRIX matrix;
        D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
        D3DXVECTOR2 scaling ( fScaleX, fScaleY );

		m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
			D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, NULL );
			m_pDXSprite->SetTransform ( &matrix );        
			pDXFont->DrawText ( m_pDXSprite, szText, -1, &rect, ulFormat, ulColor );
        m_pDXSprite->End ();
    }        
}


void CGraphics::DrawText ( int iX, int iY, unsigned long dwColor, float fScale, const char * szText, ... )
{
    char szBuffer [ 1024 ];
	va_list ap;
	va_start ( ap, szText );
	_VSNPRINTF ( szBuffer, 1024, szText, ap );
	va_end ( ap );

    DrawText ( iX, iY, iX, iY, dwColor, szBuffer, fScale, fScale, DT_NOCLIP );
}


void CGraphics::DrawTextCEGUI ( int uiLeft, int uiTop, int uiRight, int uiBottom, unsigned long ulColor, const char* szText, float fScale, unsigned long ulFormat )
{
    // Create a font?
    if ( !m_pFont )
    {
        // Load it
		char szFont[128];
		GetWindowsDirectory ( szFont, 128 );
		strcat ( &szFont[0], "\\fonts\\arial.ttf" );
		m_pFont = CCore::GetSingleton ().GetGUI ()->CreateFnt ( "arial", szFont, 18, 0 );

		if ( m_pFont != NULL ) {
			// Proceed with the font
			m_pFont->SetAntiAliasingEnabled ( true );
			m_pFont->SetNativeResolution ( 800, 600 );
			m_pFont->SetAutoScalingEnabled ( true );
			return;
		} else {
			// Prevent invalid file from crashing us
			static bool bHasWarned = false;
			if ( !bHasWarned )
			{
				m_pGUI->GetConsole ()->Echo ( "WARNING: arial.ttf could not be located. No text is drawn." );
				bHasWarned = true;
			}
			return;
		}
    }

    // Start drawing
    BeginSingleDrawing ( );

	#pragma message(__LOC__"(IJs) Using the font directly to render text is a big performance issue (hence our FPS drop on nametags, displays, etc.), since there is no caching at all.")

    // Draw it
	m_pFont->DrawTextString ( szText, CRect2D ( ( float ) uiLeft, ( float ) uiTop, ( float ) uiRight, ( float ) uiBottom ), 1.0f, CRect2D ( 0, 0, 2000, 2000 ), ulFormat, ulColor, fScale / 2, fScale / 2 );

	// End drawing
    EndSingleDrawing ( );
}


void CGraphics::DrawText2DA ( int uiX, int uiY, unsigned long ulColor, float fScale, const char* szDisplayText, ... )
{
    char szBuffer [ 1024 ];
	va_list ap;
	va_start ( ap, szDisplayText );
	_VSNPRINTF ( szBuffer, 1024, szDisplayText, ap );
	va_end ( ap );

    // Start drawing
    BeginSingleDrawing ();

    // Draw the text
    m_pGUI->GetRenderingLibrary ()->DrawText2D ( static_cast < float > ( uiX ), static_cast < float > ( uiY ), ulColor, szBuffer, fScale );

    // End drawing
    EndSingleDrawing ();
}


void CGraphics::DrawText3DA	( float fX, float fY, float fZ, unsigned long ulColor, float fScale, const char* szDisplayText, ... )
{
    char szBuffer [ 1024 ];
	va_list ap;
	va_start ( ap, szDisplayText );
	_VSNPRINTF ( szBuffer, 1024, szDisplayText, ap );
	va_end ( ap );

    // Initialize ViewMatrix
    D3DMATRIX ViewMatrix;
    memset ( &ViewMatrix, 0, sizeof ( D3DMATRIX ) );

    // Get the view matrix for billboarding calculation by DrawText3D.
    CDirect3DData::GetSingleton ( ).GetTransform ( D3DTS_VIEW, &ViewMatrix );
    //assert ( 0 ); // StoreTransform uses a pretty significant amount of processing

    // Start drawing
    BeginSingleDrawing ( );

    // Draw this text.
    m_pGUI->GetRenderingLibrary ( )->DrawText3D ( fX,
                                                  fY,
                                                  fZ,
                                                  &ViewMatrix,
                                                  ulColor, 
                                                  szBuffer, 
                                                  fScale );
    // End drawing
    EndSingleDrawing ( );
}


void CGraphics::Draw3DBox ( float fX, float fY, float fZ, float fL, float fW, float fH, DWORD dwColor, bool bWireframe )
{
	BeginSingleDrawing ( );

	m_pGUI->GetRenderingLibrary ()->Render3DBox ( fX, fY, fZ, fL, fW, fH, dwColor, bWireframe );

	EndSingleDrawing ( );
}


void CGraphics::DrawLine ( float fX1, float fY1, float fX2, float fY2, unsigned long ulColor )
{
   // Start drawing
    BeginSingleDrawing ( );

    // Draw the line
    m_pGUI->GetRenderingLibrary ()->DrawLine ( fX1, fY1, 0, fX2, fY2, 0, ulColor );

    // End drawing
    EndSingleDrawing ( );
}


void CGraphics::DrawLine3D ( const CVector& vecBegin, const CVector& vecEnd, unsigned long ulColor, float fWidth )
{
	BeginSingleDrawing ( );

	m_pGUI->GetRenderingLibrary ()->DrawLine3D ( vecBegin.fX, vecBegin.fY, vecBegin.fZ, vecEnd.fX, vecEnd.fY, vecEnd.fZ, 0, fWidth / 75.0f, NULL, ulColor );
    m_pGUI->GetRenderingLibrary ()->DrawLine3D ( vecBegin.fX, vecBegin.fY, vecBegin.fZ, vecEnd.fX, vecEnd.fY, vecEnd.fZ, fWidth / 75.0f, 0, NULL, ulColor );

	// Why draw it twice? - Commented out for a test
    //m_pGUI->GetRenderingLibrary ()->DrawLine3D ( vecEnd.fX, vecEnd.fY, vecEnd.fZ, vecBegin.fX, vecBegin.fY, vecBegin.fZ, 0, fWidth / 75.0f, NULL, ulColor );
    //m_pGUI->GetRenderingLibrary ()->DrawLine3D ( vecEnd.fX, vecEnd.fY, vecEnd.fZ, vecBegin.fX, vecBegin.fY, vecBegin.fZ, fWidth / 75.0f, 0, NULL, ulColor );

	EndSingleDrawing ( );
}


void CGraphics::DrawRectangle ( float fX, float fY, float fWidth, float fHeight, unsigned long ulColor )
{      
    m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
    D3DXMATRIX matrix;
    D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
    D3DXVECTOR2 scaling ( fWidth, fHeight );
    D3DXVECTOR2 position ( fX, fY );
    D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, &position );
    m_pDXSprite->SetTransform ( &matrix );
    m_pDXSprite->Draw ( m_pDXPixelTexture, NULL, NULL, NULL, ulColor );
    m_pDXSprite->End ();
}


void CGraphics::CalcWorldCoors ( CVector * vecScreen, CVector * vecWorld )
{
	// Get the static view matrix as D3DXMATRIX
	D3DXMATRIXA16 m ( (float*)(0xB6FA2C) );

	// Invert the view matrix
	D3DXMATRIXA16 minv;
	memset ( &minv, 0, sizeof ( D3DXMATRIXA16 ) );
	m._44 = 1.0f;
	D3DXMatrixInverse ( &minv, NULL, &m );

	DWORD *dwLenX = (DWORD*)(0xC17044);
	DWORD *dwLenY = (DWORD*)(0xC17048);

	// Reverse screen coordinates
	float fRecip = 1.0f / vecScreen->fZ;
	vecScreen->fX /= fRecip * (*dwLenX);
	vecScreen->fY /= fRecip * (*dwLenY);

	// Do an (inverse) transformation
	vecWorld->fX = vecScreen->fZ * minv._31 + vecScreen->fY * minv._21 + vecScreen->fX * minv._11 + minv._41;
	vecWorld->fY = vecScreen->fZ * minv._32 + vecScreen->fY * minv._22 + vecScreen->fX * minv._12 + minv._42;
	vecWorld->fZ = vecScreen->fZ * minv._33 + vecScreen->fY * minv._23 + vecScreen->fX * minv._13 + minv._43;
}


void CGraphics::CalcScreenCoors ( CVector * vecWorld, CVector * vecScreen )
{
	/** C++-ifyed function 0x71DA00, formerly called by CHudSA::CalcScreenCoors **/

	// Get the static view matrix as D3DXMATRIX
	D3DXMATRIX m ( (float*)(0xB6FA2C) );

	// Get the static virtual screen (x,y)-sizes
	DWORD *dwLenX = (DWORD*)(0xC17044);
	DWORD *dwLenY = (DWORD*)(0xC17048);

	// Do a transformation
	vecScreen->fX = vecWorld->fZ * m._31 + vecWorld->fY * m._21 + vecWorld->fX * m._11 + m._41;
	vecScreen->fY = vecWorld->fZ * m._32 + vecWorld->fY * m._22 + vecWorld->fX * m._12 + m._42;
	vecScreen->fZ = vecWorld->fZ * m._33 + vecWorld->fY * m._23 + vecWorld->fX * m._13 + m._43;

	// Get the correct screen coordinates
	float fRecip = 1.0f / vecScreen->fZ;
	vecScreen->fX *= fRecip * (*dwLenX);
	vecScreen->fY *= fRecip * (*dwLenY);
}


void CGraphics::Render3DSprite ( float fX, float fY, float fZ, float fScale, unsigned long ulColor )
{
    // Initialize ViewMatrix
    D3DXMATRIX ViewMatrix;
    memset ( &ViewMatrix, 0, sizeof ( D3DXMATRIX ) );

    // Get the view matrix for billboarding calculation by DrawText3D.
    CDirect3DData::GetSingleton ( ).GetTransform ( D3DTS_VIEW, &ViewMatrix );
    //assert ( 0 ); // StoreTransform uses a pretty significant amount of processing

    // Begin drawing
    BeginSingleDrawing ();

    // Render it
    m_pGUI->GetRenderingLibrary ()->Render3DSprite ( NULL, 1.0f, &D3DXVECTOR3 ( fX, fY, fZ ), &ViewMatrix, ulColor );

    // End drawing
    EndSingleDrawing ();
}


unsigned int CGraphics::GetViewportWidth ( void )
{
    return CDirect3DData::GetSingleton ().GetViewportWidth ();
}


unsigned int CGraphics::GetViewportHeight ( void )
{
    return CDirect3DData::GetSingleton ().GetViewportHeight ();
}


float CGraphics::GetDXFontHeight ( float fScale, LPD3DXFONT pDXFont )
{
    if ( !pDXFont ) pDXFont = GetFont ();

    // We're using a big font to keep it looking nice, so get the actual scale
    if ( pDXFont == m_pDXDefaultFont && fScale > 1.1f )
    {
        pDXFont = m_pDXBigDefaultFont;
        fScale /= 4.0f;
    }

	if ( pDXFont )
    {
        D3DXFONT_DESC desc;
        pDXFont->GetDesc ( &desc );
        return ( ( float ) desc.Height * fScale );
    }
    return 0.0f;
}


float CGraphics::GetDXCharacterWidth ( char c, float fScale, LPD3DXFONT pDXFont )
{
    if ( !pDXFont ) pDXFont = GetFont ();

    // We're using a big font to keep it looking nice, so get the actual scale
    if ( pDXFont == m_pDXDefaultFont && fScale > 1.1f )
    {
        pDXFont = m_pDXBigDefaultFont;
        fScale /= 4.0f;
    }

	if ( pDXFont )
    {
        HDC dc = pDXFont->GetDC ();
        SIZE size;
        GetTextExtentPoint32 ( dc, &c, 1, &size );

        return ( ( float ) size.cx * fScale );
    }
    return 0.0f;
}


float CGraphics::GetDXTextExtent ( const char * szText, float fScale, LPD3DXFONT pDXFont )
{
    if ( !pDXFont ) pDXFont = GetFont ();

    // We're using a big font to keep it looking nice, so get the actual scale
    if ( pDXFont == m_pDXDefaultFont && fScale > 1.1f )
    {
        pDXFont = m_pDXBigDefaultFont;
        fScale /= 4.0f;
    }


	if ( pDXFont )
    {
        HDC dc = pDXFont->GetDC ();
        SIZE size;
        GetTextExtentPoint32 ( dc, szText, strlen ( szText ), &size );

        return ( ( float ) size.cx * fScale );
    }
    return 0.0f;
}

ID3DXFont * CGraphics::GetFont ( eFontType fontType )
{
    switch ( fontType )
    {
        case FONT_DEFAULT_BOLD:	return m_pDXDefaultBoldFont; break;
        case FONT_CLEAR:		return m_pDXClearFont; break;
        case FONT_ARIAL:		return m_pDXArialFont; break;
        case FONT_SANS:			return m_pDXSansFont; break;
        case FONT_PRICEDOWN:	return m_pDXPricedownFont; break;
		case FONT_BANKGOTHIC:	return m_pDXBankGothicFont; break;
		case FONT_DIPLOMA:		return m_pDXDiplomaFont; break;
		case FONT_BECKETT:		return m_pDXBeckettFont; break;
        default:				break;
    }
    return m_pDXDefaultFont;
}


eFontType CGraphics::GetFontType ( char * szFontName )
{
    assert ( szFontName );
    if ( !stricmp ( szFontName, "default" ) )		return FONT_DEFAULT;
    if ( !stricmp ( szFontName, "default-bold" ) )	return FONT_DEFAULT_BOLD;
    if ( !stricmp ( szFontName, "clear" ) )			return FONT_CLEAR;
    if ( !stricmp ( szFontName, "arial" ) )			return FONT_ARIAL;
    if ( !stricmp ( szFontName, "sans" ) )			return FONT_SANS;
    if ( !stricmp ( szFontName, "pricedown" ) )		return FONT_PRICEDOWN;
	if ( !stricmp ( szFontName, "bankgothic" ) )	return FONT_BANKGOTHIC;
	if ( !stricmp ( szFontName, "diploma" ) )		return FONT_DIPLOMA;
	if ( !stricmp ( szFontName, "beckett" ) )		return FONT_BECKETT;
    return FONT_DEFAULT;
}


void CGraphics::BeginSingleDrawing ( void )
{
    // This function is used to determine if we need to capture the device state and begin drawing or not
    // and must be called before ANY drawing inside this class. This function will capture the device states
    // only if they aren't already captured through a BeginDrawing () call made by the client if it's going
    // to do multiple operations (thus saving the time it takes to do this).

    // We might consider doing a BeginDrawing () before any call to a client function and EndDrawing ()
    // after, in other words we don't have to worry about the states not getting restored back to GTA.
    // - ChrML

    // Start drawing?
    if ( !m_bIsDrawing )
    {
        m_pGUI->GetRenderingLibrary ( )->UTIL_CaptureDeviceState ( );
        m_pGUI->GetRenderingLibrary ()->BeginDrawing ();
    }
}


void CGraphics::EndSingleDrawing ( void )
{
    // See comment in BeginSingleDrawing ()

    // End drawing?
    if ( !m_bIsDrawing )
    {
        m_pGUI->GetRenderingLibrary ()->EndDrawing ();
        m_pGUI->GetRenderingLibrary ( )->UTIL_RestoreDeviceState ( );
    }
}


void CGraphics::SetCursorPosition ( int iX, int iY, DWORD Flags )
{
	m_pDevice->SetCursorPosition ( iX, iY, Flags );
}


void CGraphics::DrawLineQueued ( float fX1, float fY1,
                                 float fX2, float fY2,
                                 float fWidth,
                                 unsigned long ulColor,
                                 bool bPostGUI )
{
    // Set up a queue item
    sDrawQueueItem Item;
    Item.eType = QUEUE_LINE;
    Item.Line.fX1 = fX1;
    Item.Line.fY1 = fY1;
    Item.Line.fX2 = fX2;
    Item.Line.fY2 = fY2;
    Item.Line.fWidth = fWidth;
    Item.Line.ulColor = ulColor;

    // Add it to the queue
    AddQueueItem ( Item, bPostGUI );
}


void CGraphics::DrawLine3DQueued ( const CVector& vecBegin,
                                   const CVector& vecEnd,
                                   float fWidth,
                                   unsigned long ulColor,
                                   bool bPostGUI )
{
    // Set up a queue item
    sDrawQueueItem Item;
    Item.eType = QUEUE_LINE3D;
    Item.Line3D.fX1 = vecBegin.fX;
    Item.Line3D.fY1 = vecBegin.fY;
	Item.Line3D.fZ1 = vecBegin.fZ;
    Item.Line3D.fX2 = vecEnd.fX;
    Item.Line3D.fY2 = vecEnd.fY;
	Item.Line3D.fZ2 = vecEnd.fZ;
    Item.Line3D.fWidth = fWidth;
    Item.Line3D.ulColor = ulColor;

    // Add it to the queue
    AddQueueItem ( Item, bPostGUI );
}


void CGraphics::DrawRectQueued ( float fX, float fY,
                                 float fWidth, float fHeight,
                                 unsigned long ulColor,
                                 bool bPostGUI )
{
    // Set up a queue item
    sDrawQueueItem Item;
    Item.eType = QUEUE_RECT;
    Item.Rect.fX = fX;
    Item.Rect.fY = fY;
    Item.Rect.fWidth = fWidth;
    Item.Rect.fHeight = fHeight;
    Item.Rect.ulColor = ulColor;

    // Add it to the queue
    AddQueueItem ( Item, bPostGUI );
}


bool CGraphics::DrawTextureQueued ( float fX, float fY,
                                 float fWidth, float fHeight,
                                 const string& strFilename,
                                 float fRotation,
                                 float fRotCenOffX,
                                 float fRotCenOffY,
                                 unsigned long ulColor,
                                 bool bPostGUI )
{
    // Set up a queue item
    sDrawQueueItem Item;
    Item.eType = QUEUE_TEXTURE;
    Item.Texture.fX = fX;
    Item.Texture.fY = fY;
    Item.Texture.fWidth = fWidth;
    Item.Texture.fHeight = fHeight;
    Item.Texture.texture = CacheTexture( strFilename );
    Item.Texture.fRotation = fRotation;
    Item.Texture.fRotCenOffX = fRotCenOffX;
    Item.Texture.fRotCenOffY = fRotCenOffY;
    Item.Texture.ulColor = ulColor;

    if ( !Item.Texture.texture )
        return false;

    // Add it to the queue
    AddQueueItem ( Item, bPostGUI );
    return true;
}


void CGraphics::DrawTextQueued ( int iLeft, int iTop,
								 int iRight, int iBottom,
								 unsigned long dwColor,
								 const char* szText,
								 float fScaleX,
								 float fScaleY,
								 unsigned long ulFormat,
								 ID3DXFont * pDXFont,
								 bool bPostGUI )
{
	if ( !szText || !m_pDXSprite )
		return;

	if ( !pDXFont ) pDXFont = GetFont ();

	// We're using a big font to keep it looking nice, so get the actual scale
	if ( pDXFont == m_pDXDefaultFont && ( fScaleX > 1.1f || fScaleY > 1.1f ) )
	{
		pDXFont = m_pDXBigDefaultFont;
		fScaleX /= 4.0f;
		fScaleY /= 4.0f;
	}


	if ( pDXFont )
    {
		iLeft = unsigned int ( ( float ) iLeft * ( 1.0f / fScaleX ) );
		iTop = unsigned int ( ( float ) iTop * ( 1.0f / fScaleY ) );
		iRight = unsigned int ( ( float ) iRight * ( 1.0f / fScaleX ) );
		iBottom = unsigned int ( ( float ) iBottom * ( 1.0f / fScaleY ) );

		sDrawQueueItem Item;
		Item.eType = QUEUE_TEXT;

		Item.Text.iLeft = iLeft;
		Item.Text.iTop = iTop;
		Item.Text.iRight = iRight;
		Item.Text.iBottom = iBottom;
		Item.Text.ulColor = dwColor;
		Item.Text.fScaleX = fScaleX;
		Item.Text.fScaleY = fScaleY;
		Item.Text.ulFormat = ulFormat;
		Item.Text.pDXFont = pDXFont;

		Item.strText = szText;

		// Add it to the queue
		AddQueueItem ( Item, bPostGUI );
	}
}

bool CGraphics::LoadFonts ( void )
{
	std::string strFontPath;

	CFilePathTranslator FilePathTranslator;
	FilePathTranslator.GetMTASARootDirectory ( strFontPath );
	strFontPath += "\\MTA\\cgui\\";

	// Add our custom font resources
	int iLoaded = 0;
	iLoaded += AddFontResourceEx ( std::string ( strFontPath + "pricedown.ttf" ).c_str (), FR_PRIVATE, 0 );
	iLoaded += AddFontResourceEx ( std::string ( strFontPath + "sabankgothic.ttf" ).c_str (), FR_PRIVATE, 0 );
	iLoaded += AddFontResourceEx ( std::string ( strFontPath + "saheader.ttf" ).c_str (), FR_PRIVATE, 0 );
	iLoaded += AddFontResourceEx ( std::string ( strFontPath + "sagothic.ttf" ).c_str (), FR_PRIVATE, 0 );

	// Create DirectX font and sprite objects
    return
		(	SUCCEEDED (D3DXCreateFont ( m_pDevice, 15, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "tahoma", &m_pDXDefaultFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 60, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "tahoma", &m_pDXBigDefaultFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "tahomabd", &m_pDXDefaultBoldFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 15, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "verdana", &m_pDXClearFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 15, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "arial", &m_pDXArialFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "microsoft sans serif", &m_pDXSansFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 30, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "pricedown", &m_pDXPricedownFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 30, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "bankgothic md bt", &m_pDXBankGothicFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 30, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "diploma", &m_pDXDiplomaFont ))
		&&	SUCCEEDED (D3DXCreateFont ( m_pDevice, 30, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "beckett", &m_pDXBeckettFont ))
		&&	SUCCEEDED (D3DXCreateSprite ( m_pDevice, &m_pDXSprite ))
		&&	SUCCEEDED (iLoaded == 4) );
}

bool CGraphics::DestroyFonts ( void )
{
	std::string strFontPath;

	CFilePathTranslator FilePathTranslator;
	FilePathTranslator.GetMTASARootDirectory ( strFontPath );
	strFontPath += "\\MTA\\cgui\\";

	// Remove our custom font resources (needs to be identical to LoadFonts)
	RemoveFontResourceEx ( std::string ( strFontPath + "pricedown.ttf" ).c_str (), FR_PRIVATE, 0 );
	RemoveFontResourceEx ( std::string ( strFontPath + "sabankgothic.ttf" ).c_str (), FR_PRIVATE, 0 );
	RemoveFontResourceEx ( std::string ( strFontPath + "saheader.ttf" ).c_str (), FR_PRIVATE, 0 );
	RemoveFontResourceEx ( std::string ( strFontPath + "sagothic.ttf" ).c_str (), FR_PRIVATE, 0 );

	return true;
}

IDirect3DTexture9* CGraphics::CreateTexture ( DWORD* dwBitMap, unsigned int uiWidth, unsigned int uiHeight )
{
    IDirect3DTexture9* texture = NULL;
    D3DXCreateTexture ( m_pDevice, uiWidth, uiHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture );

    D3DLOCKED_RECT lRect;
    texture->LockRect ( 0, &lRect, 0, 0 );

    BYTE* pTexture;
    pTexture = static_cast < BYTE* > ( lRect.pBits );

    DWORD color, pitch;
    pitch = lRect.Pitch;
    for ( UINT c = 0; c < uiHeight; c++ )
    {
        for ( UINT r = 0; r < uiWidth; r++ )
        {
            color = dwBitMap[uiWidth*c+r];
            pTexture[r*4+c*pitch+0] = static_cast < BYTE > ( color );
            pTexture[r*4+c*pitch+1] = static_cast < BYTE > ( color >> 8 );
            pTexture[r*4+c*pitch+2] = static_cast < BYTE > ( color >> 16 );
            pTexture[r*4+c*pitch+3] = static_cast < BYTE > ( color >> 24 );
        }
    }

    lRect.pBits = pTexture;

    texture->UnlockRect ( 0 );
    return texture;
}

IDirect3DTexture9* CGraphics::LoadTexture ( const char* szFile )
{
    IDirect3DTexture9* texture = NULL;
	D3DXCreateTextureFromFile ( m_pDevice, szFile, &texture );
    return texture;
}

IDirect3DTexture9* CGraphics::LoadTexture ( const char* szFile, unsigned int uiWidth, unsigned int uiHeight )
{
    IDirect3DTexture9* texture = NULL;
    D3DXCreateTextureFromFileEx ( m_pDevice, szFile, uiWidth, uiHeight, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture );
    return texture;
}

extern CCore* g_pCore;
void CGraphics::DrawTexture ( IDirect3DTexture9* texture, float fX, float fY, float fScaleX, float fScaleY, float fRotation, float fCenterX, float fCenterY, unsigned char ucAlpha )
{
    m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
    D3DSURFACE_DESC textureDesc;
    texture->GetLevelDesc( 0, &textureDesc );
    D3DXMATRIX matrix;
    D3DXVECTOR2 scaling ( fScaleX, fScaleY );
    D3DXVECTOR2 rotationCenter  ( ( float ) textureDesc.Width * fCenterX, ( float ) textureDesc.Height * fCenterY );
    D3DXVECTOR2 position ( fX - ( float ) textureDesc.Width * fScaleX * fCenterX, fY - ( float ) textureDesc.Height * fScaleY * fCenterY );
    D3DXMatrixTransformation2D ( &matrix, NULL, NULL, &scaling, &rotationCenter, fRotation * 6.2832f / 360.f, &position );
    m_pDXSprite->SetTransform ( &matrix );
    m_pDXSprite->Draw ( texture, NULL, NULL, NULL, D3DCOLOR_ARGB ( ucAlpha, 255, 255, 255 ) );
    m_pDXSprite->End ();
}

void CGraphics::OnDeviceCreate ( IDirect3DDevice9 * pDevice )
{
	m_pDevice = pDevice;

	if ( !LoadFonts () )
		// Some of the above objects could not be created
		CCore::GetSingleton ().GetConsole ()->Printf ( "WARNING: Some fonts could not be loaded! Your game will not be able to display any text." );

	// Get the original render target
	m_pDevice->GetRenderTarget ( 0, &m_pOriginalTarget );

    // Create drawing devices
    D3DXCreateLine ( pDevice, &m_pLineInterface );
    D3DXCreateTextureFromFileInMemory ( pDevice, g_szPixel, sizeof ( g_szPixel ), &m_pDXPixelTexture );
}


void CGraphics::OnDeviceInvalidate ( IDirect3DDevice9 * pDevice )
{
	SAFE_RELEASE ( m_pOriginalTarget );

	if ( m_pDXDefaultFont )
        m_pDXDefaultFont->OnLostDevice ();

    if ( m_pDXBigDefaultFont )
        m_pDXBigDefaultFont->OnLostDevice ();

    if ( m_pDXDefaultBoldFont )
        m_pDXDefaultBoldFont->OnLostDevice ();

    if ( m_pDXClearFont )
        m_pDXClearFont->OnLostDevice ();

    if ( m_pDXArialFont )
        m_pDXArialFont->OnLostDevice ();

    if ( m_pDXSansFont )
        m_pDXSansFont->OnLostDevice ();

    if ( m_pDXPricedownFont )
        m_pDXPricedownFont->OnLostDevice ();

    if ( m_pDXBankGothicFont )
        m_pDXBankGothicFont->OnLostDevice ();

	if ( m_pDXDiplomaFont )
        m_pDXDiplomaFont->OnLostDevice ();

	if ( m_pDXBeckettFont )
		m_pDXBeckettFont->OnLostDevice ();

	if ( m_pDXSprite )
        m_pDXSprite->OnLostDevice ();

    if ( m_pLineInterface )
        m_pLineInterface->OnLostDevice ();
}


void CGraphics::OnDeviceRestore ( IDirect3DDevice9 * pDevice )
{
	// Get the original render target
	m_pDevice->GetRenderTarget ( 0, &m_pOriginalTarget );

    if ( m_pDXDefaultFont )
        m_pDXDefaultFont->OnResetDevice ();

    if ( m_pDXBigDefaultFont )
        m_pDXBigDefaultFont->OnResetDevice ();

    if ( m_pDXDefaultBoldFont )
        m_pDXDefaultBoldFont->OnResetDevice ();

    if ( m_pDXClearFont )
        m_pDXClearFont->OnResetDevice ();

    if ( m_pDXArialFont )
        m_pDXArialFont->OnResetDevice ();

    if ( m_pDXSansFont )
        m_pDXSansFont->OnResetDevice ();

    if ( m_pDXPricedownFont )
        m_pDXPricedownFont->OnResetDevice ();

    if ( m_pDXSprite )
        m_pDXSprite->OnResetDevice ();

    if ( m_pLineInterface )
        m_pLineInterface->OnResetDevice ();
}


void CGraphics::DrawPreGUIQueue ( void )
{
    // Items to draw?
    if ( m_PreGUIQueue.size () > 0 )
    {
		BeginSingleDrawing ();

		// Loop through it
        std::list < sDrawQueueItem > ::iterator iter = m_PreGUIQueue.begin ();
        for ( ; iter != m_PreGUIQueue.end (); iter++ )
        {
            // Draw the item
            DrawQueueItem ( *iter );
        }

		EndSingleDrawing ();

		// Clear the list
        m_PreGUIQueue.clear ();
    }
}


void CGraphics::DrawPostGUIQueue ( void )
{
    // Items to draw?
    if ( m_PostGUIQueue.size () > 0 )
    {
		BeginSingleDrawing ();

        // Loop through it
        std::list < sDrawQueueItem > ::iterator iter = m_PostGUIQueue.begin ();
        for ( ; iter != m_PostGUIQueue.end (); iter++ )
        {
            // Draw the item
            DrawQueueItem ( *iter );
        }

		EndSingleDrawing ();

        // Clear the list
        m_PostGUIQueue.clear ();
    }

    // Clean out unused textures here
    ExpireCachedTextures ();
}


void CGraphics::AddQueueItem ( const sDrawQueueItem& Item, bool bPostGUI )
{
    // Add it to the correct queue
    if ( bPostGUI )
        m_PostGUIQueue.push_back ( Item );
    else
        m_PreGUIQueue.push_back ( Item );
}


void CGraphics::DrawQueueItem ( const sDrawQueueItem& Item )
{
    // TODO: Optimize using the Begin / End calls for our DX interfaces. Maybe sort the list?

    // Line type?
    switch ( Item.eType )
    {
        // Line type?
        case QUEUE_LINE:
        {
            // Got a line interface?
            if ( m_pLineInterface )
            {
                // Populate a list over vectors to draw
                D3DXVECTOR2 List [2];
                List [0].x = Item.Line.fX1;
                List [0].y = Item.Line.fY1;
                List [1].x = Item.Line.fX2;
                List [1].y = Item.Line.fY2;

                // Draw it
                m_pLineInterface->SetWidth ( Item.Line.fWidth );
                m_pLineInterface->Draw ( List, 2, Item.Line.ulColor );
            }

            break;
        };
		// 3D Line type?
		case QUEUE_LINE3D:
		{
			m_pGUI->GetRenderingLibrary ()->DrawLine3D ( Item.Line3D.fX1, 
														 Item.Line3D.fY1,
														 Item.Line3D.fZ1,
														 Item.Line3D.fX2,
														 Item.Line3D.fY2,
														 Item.Line3D.fZ2,
														 0,
														 Item.Line3D.fWidth / 75.0f,
														 NULL,
														 Item.Line3D.ulColor );
			break;
		};
		// Rectangle type?
		case QUEUE_RECT:
		{
            m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
			D3DXMATRIX matrix;
			D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
			D3DXVECTOR2 scaling ( Item.Rect.fWidth, Item.Rect.fHeight );
			D3DXVECTOR2 position ( Item.Rect.fX, Item.Rect.fY );
			D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, &position );
			m_pDXSprite->SetTransform ( &matrix );
			m_pDXSprite->Draw ( m_pDXPixelTexture, NULL, NULL, NULL, Item.Rect.ulColor );
            m_pDXSprite->End ();
			break;
		};
		case QUEUE_TEXT:
		{
            m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
			RECT rect;        
			SetRect ( &rect, Item.Text.iLeft, Item.Text.iTop, Item.Text.iRight, Item.Text.iBottom );  
			D3DXMATRIX matrix;
			D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
			D3DXVECTOR2 scaling ( Item.Text.fScaleX, Item.Text.fScaleY );
			D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, NULL );
			m_pDXSprite->SetTransform ( &matrix );        
			Item.Text.pDXFont->DrawText ( m_pDXSprite, Item.strText.c_str (), -1, &rect, Item.Text.ulFormat, Item.Text.ulColor );
            m_pDXSprite->End ();
			break;
		}
        case QUEUE_TEXTURE:
        {
            m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
            D3DSURFACE_DESC texureDesc;
            Item.Texture.texture->GetLevelDesc( 0, &texureDesc );
            float texureWidth   = texureDesc.Width;
            float texureHeight  = texureDesc.Height;
            float fRotationRad  = Item.Texture.fRotation * (6.2832f/360.f);
            D3DXMATRIX matrix;
            D3DXVECTOR2 scaling         ( Item.Texture.fWidth / texureWidth, Item.Texture.fHeight / texureHeight );
            D3DXVECTOR2 rotationCenter  ( Item.Texture.fWidth * 0.5f + Item.Texture.fRotCenOffX, Item.Texture.fHeight * 0.5f + Item.Texture.fRotCenOffY );
            D3DXVECTOR2 position        ( Item.Texture.fX, Item.Texture.fY );
			D3DXMatrixTransformation2D  ( &matrix, NULL, 0.0f, &scaling, &rotationCenter, fRotationRad, &position );
            m_pDXSprite->SetTransform ( &matrix );
            m_pDXSprite->Draw ( Item.Texture.texture, NULL, NULL, NULL, Item.Texture.ulColor );
            m_pDXSprite->End ();
            break;
        }
        // Circle type?
        case QUEUE_CIRCLE:
        {
            break;
        };
    }
}


// Cache a texture for current and future use.
IDirect3DTexture9* CGraphics::CacheTexture ( const string& strFilename )
{
    // Find exisiting
    map < string, SCachedTextureInfo >::iterator iter = m_CachedTextureInfoMap.find ( strFilename );

    if ( iter == m_CachedTextureInfoMap.end () )
    {
        // Add if not found
        m_CachedTextureInfoMap[strFilename] = SCachedTextureInfo();
        iter = m_CachedTextureInfoMap.find ( strFilename );

        SCachedTextureInfo& info = iter->second;
        info.texture = LoadTexture( strFilename.c_str () );
    }

    SCachedTextureInfo& info = iter->second;
    info.ulTimeLastUsed = GetTickCount();

    return info.texture;
}


// Remove any cached texures that have not been used for a little while.
void CGraphics::ExpireCachedTextures ( bool bExpireAll )
{
    // Expire unused cached textures
    // 1 cached texture     = 15 seconds till expire
    // 50 cached textures   = 8 seconds till expire
    // 100 cached textures  = 1 second till expire
    long ulNumTextures              = m_CachedTextureInfoMap.size ();
    unsigned long ulMaxAgeSeconds   = max( 1, 15 - (ulNumTextures * 15 / 100) );

    map < string, SCachedTextureInfo > ::iterator iter = m_CachedTextureInfoMap.begin ();
    for ( ; iter != m_CachedTextureInfoMap.end (); iter++ )
    {
        SCachedTextureInfo& info    = iter->second;
        unsigned long ulAge         = GetTickCount() - info.ulTimeLastUsed;
        if ( ulAge > ulMaxAgeSeconds * 1000 || bExpireAll )
        {
            SAFE_RELEASE ( info.texture );
            m_CachedTextureInfoMap.erase ( iter );
            iter = m_CachedTextureInfoMap.begin ();
        }
    }
}

