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

using namespace std;

template<> CGraphics * CSingleton< CGraphics >::m_pSingleton = NULL;

const unsigned char g_szPixel [] = { 0x42, 0x4D, 0x3A, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0, 0x28, 0, 0,
                                    0, 0x1, 0, 0, 0, 0x1, 0, 0, 0, 0x1, 0, 0x18, 0, 0, 0, 0, 0,
                                    0x4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0xFF, 0xFF, 0xFF, 0 };

CGraphics::CGraphics ( CLocalGUI* pGUI )
{
    m_pGUI = pGUI;
    m_pFont = NULL;
    memset ( m_pDXFonts, 0, sizeof ( m_pDXFonts ) );
    memset ( m_pBigDXFonts, 0, sizeof ( m_pBigDXFonts ) );
    m_pDevice = NULL;
    m_pLineInterface = NULL;
    m_pDXSprite = NULL;

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
    if ( !pDXFont )
    pDXFont = GetFont ();

    // We're using a big font to keep it looking nice, so get the actual scale
    if ( fScaleX > 1.1f || fScaleY > 1.1f )
    {
        pDXFont = GetBigFont ( pDXFont );
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


void CGraphics::DrawText3DA ( float fX, float fY, float fZ, unsigned long ulColor, float fScale, const char* szDisplayText, ... )
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
    m_pGUI->GetRenderingLibrary ()->DrawLine ( D3DXVECTOR3 ( fX1, fY1, 0 ), D3DXVECTOR3 ( fX2, fY2, 0 ), ulColor );

    // End drawing
    EndSingleDrawing ( );
}


void CGraphics::DrawLine3D ( const CVector& vecBegin, const CVector& vecEnd, unsigned long ulColor, float fWidth )
{
    BeginSingleDrawing ();

    m_pGUI->GetRenderingLibrary ()->DrawLine3D (
        D3DXVECTOR3 ( vecBegin.fX, vecBegin.fY, vecBegin.fZ ),
        D3DXVECTOR3 ( vecEnd.fX, vecEnd.fY, vecEnd.fZ ),
        fWidth / 75.0f,
        NULL,
        ulColor
    );

    EndSingleDrawing ();
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
    if ( !pDXFont )
        pDXFont = GetFont ();

    if ( fScale > 1.1f )
    {
        pDXFont = GetBigFont ( pDXFont );
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
    if ( !pDXFont )
        pDXFont = GetFont ();

    if ( fScale > 1.1f )
    {
        pDXFont = GetBigFont ( pDXFont );
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
    if ( !pDXFont )
        pDXFont = GetFont ();

    if ( fScale > 1.1f )
    {
        pDXFont = GetBigFont ( pDXFont );
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
    if ( fontType < 0 || fontType >= NUM_FONTS )
        return m_pDXFonts [ FONT_DEFAULT ];

    return m_pDXFonts [ fontType ];
}


eFontType CGraphics::GetFontType ( const char* szFontName )
{
    assert ( szFontName );
    if ( !stricmp ( szFontName, "default" ) )       return FONT_DEFAULT;
    if ( !stricmp ( szFontName, "default-bold" ) )  return FONT_DEFAULT_BOLD;
    if ( !stricmp ( szFontName, "clear" ) )         return FONT_CLEAR;
    if ( !stricmp ( szFontName, "arial" ) )         return FONT_ARIAL;
    if ( !stricmp ( szFontName, "sans" ) )          return FONT_SANS;
    if ( !stricmp ( szFontName, "pricedown" ) )     return FONT_PRICEDOWN;
    if ( !stricmp ( szFontName, "bankgothic" ) )    return FONT_BANKGOTHIC;
    if ( !stricmp ( szFontName, "diploma" ) )       return FONT_DIPLOMA;
    if ( !stricmp ( szFontName, "beckett" ) )       return FONT_BECKETT;
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
                                 float fU, float fV,
                                 float fSizeU, float fSizeV,
                                 bool bRelativeUV,
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
    Item.Texture.fU = fU;
    Item.Texture.fV = fV;
    Item.Texture.fSizeU = fSizeU;
    Item.Texture.fSizeV = fSizeV;
    Item.Texture.bRelativeUV = bRelativeUV;
    Item.Texture.info = CacheTexture( strFilename );
    Item.Texture.fRotation = fRotation;
    Item.Texture.fRotCenOffX = fRotCenOffX;
    Item.Texture.fRotCenOffY = fRotCenOffY;
    Item.Texture.ulColor = ulColor;

    if ( !Item.Texture.info.d3dTexture )
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
    if ( fScaleX > 1.1f || fScaleY > 1.1f )
    {
        pDXFont = GetBigFont ( pDXFont );
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
    static const sFontInfo fontInfos[] = {
        { "tahoma",               15, FW_NORMAL },
        { "tahomabd",             15, FW_BOLD   },
        { "verdana",              15, FW_NORMAL },
        { "arial",                15, FW_NORMAL },
        { "microsoft sans serif", 15, FW_BOLD   },
        { "pricedown",            30, FW_NORMAL },
        { "bankgothic md bt",     30, FW_NORMAL },
        { "diploma",              30, FW_NORMAL },
        { "beckett",              30, FW_NORMAL }
    };

    bool bSuccess = true;
    for ( int i = 0; bSuccess && i < NUM_FONTS; i++ )
    {
        m_pDXFonts[i] = m_pBigDXFonts[i] = NULL;

        // Normal size
        if( !SUCCEEDED ( D3DXCreateFont ( m_pDevice, fontInfos[i].uiHeight, 0, fontInfos[i].uiWeight, 1,
            FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontInfos[i].szName,
            &m_pDXFonts[i] ) ) )
        {
            CLogger::GetSingleton ().ErrorPrintf( "Could not create Direct3D font '%s'", fontInfos[i].szName );
            bSuccess = false;
        }

        // Big size (4x)
        if( !SUCCEEDED ( D3DXCreateFont ( m_pDevice, fontInfos[i].uiHeight*4, 0, fontInfos[i].uiWeight, 1,
            FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontInfos[i].szName,
            &m_pBigDXFonts[i] ) ) )
        {
            CLogger::GetSingleton ().ErrorPrintf( "Could not create Direct3D big font '%s'", fontInfos[i].szName );
            bSuccess = false;
        }
    }

    return bSuccess && SUCCEEDED ( D3DXCreateSprite ( m_pDevice, &m_pDXSprite ) ) && ( iLoaded == 4 );
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
    assert ( !m_pOriginalTarget );
    assert ( m_pDevice->GetRenderTarget ( 0, &m_pOriginalTarget ) == D3D_OK );

    // Create drawing devices
    D3DXCreateLine ( pDevice, &m_pLineInterface );
    D3DXCreateTextureFromFileInMemory ( pDevice, g_szPixel, sizeof ( g_szPixel ), &m_pDXPixelTexture );
}


void CGraphics::OnDeviceInvalidate ( IDirect3DDevice9 * pDevice )
{
    assert ( m_pOriginalTarget );
    SAFE_RELEASE ( m_pOriginalTarget );

    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        if( m_pDXFonts[i] ) m_pDXFonts[i]->OnLostDevice ();
        if( m_pBigDXFonts[i] ) m_pBigDXFonts[i]->OnLostDevice ();
    }

    if ( m_pDXSprite )
        m_pDXSprite->OnLostDevice ();

    if ( m_pLineInterface )
        m_pLineInterface->OnLostDevice ();
}


void CGraphics::OnDeviceRestore ( IDirect3DDevice9 * pDevice )
{
    // Get the original render target
    assert ( !m_pOriginalTarget );
    assert ( m_pDevice->GetRenderTarget ( 0, &m_pOriginalTarget ) == D3D_OK );

    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        if( m_pDXFonts[i] ) m_pDXFonts[i]->OnResetDevice ();
        if( m_pBigDXFonts[i] ) m_pBigDXFonts[i]->OnResetDevice ();
    }

    if ( m_pDXSprite )
        m_pDXSprite->OnResetDevice ();

    if ( m_pLineInterface )
        m_pLineInterface->OnResetDevice ();
}


void CGraphics::DrawPreGUIQueue ( void )
{
    DrawQueue ( m_PreGUIQueue );
}


void CGraphics::DrawPostGUIQueue ( void )
{
    DrawQueue ( m_PostGUIQueue );

    // Clean out unused textures here
    ExpireCachedTextures ();
}

bool CGraphics::IsDrawQueueItemSprite ( const sDrawQueueItem& Item )
{
    return Item.eType == QUEUE_TEXT || Item.eType == QUEUE_RECT ||
           Item.eType == QUEUE_CIRCLE || Item.eType == QUEUE_TEXTURE;
}

void CGraphics::DrawQueue ( std::vector < sDrawQueueItem >& Queue )
{
    // Items to draw?
    if ( Queue.size () > 0 )
    {
        BeginSingleDrawing ();

        // Loop through it
        bool bSpriteMode = false;
        std::vector < sDrawQueueItem >::iterator iter = Queue.begin ();
        for ( ; iter != Queue.end (); iter++ )
        {
            if ( IsDrawQueueItemSprite ( *iter ) != bSpriteMode )
            {
                bSpriteMode = IsDrawQueueItemSprite ( *iter );
                if ( bSpriteMode )
                {
                    m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND );
                    m_pDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
                    m_pDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
                }
                else
                    m_pDXSprite->End ();
            }
            // Draw the item
            DrawQueueItem ( *iter );
        }
        if ( bSpriteMode )
            m_pDXSprite->End ();

        EndSingleDrawing ();

        // Clear the list
        Queue.clear ();
    }
}


void CGraphics::AddQueueItem ( const sDrawQueueItem& Item, bool bPostGUI )
{
    // Prevent queuing when minimized
    if ( g_pCore->IsWindowMinimized () )
    {
        m_PostGUIQueue.clear ();
        m_PreGUIQueue.clear ();
        return;
    }

    // Add it to the correct queue
    if ( bPostGUI && !CCore::GetSingleton ().IsMenuVisible() ) //Don't draw over the main menu.  Ever.
        m_PostGUIQueue.push_back ( Item );
    else
        m_PreGUIQueue.push_back ( Item );
}


void CGraphics::DrawQueueItem ( const sDrawQueueItem& Item )
{
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
        }
        // 3D Line type?
        case QUEUE_LINE3D:
        {
            m_pGUI->GetRenderingLibrary ()->DrawLine3D (
                D3DXVECTOR3 ( Item.Line3D.fX1, Item.Line3D.fY1, Item.Line3D.fZ1 ),
                D3DXVECTOR3 ( Item.Line3D.fX2, Item.Line3D.fY2, Item.Line3D.fZ2 ),
                Item.Line3D.fWidth / 75.0f,
                NULL,
                Item.Line3D.ulColor
            );
            break;
        }
        // Rectangle type?
        case QUEUE_RECT:
        {
            D3DXMATRIX matrix;
            D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
            D3DXVECTOR2 scaling ( Item.Rect.fWidth, Item.Rect.fHeight );
            D3DXVECTOR2 position ( Item.Rect.fX, Item.Rect.fY );
            D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, &position );
            m_pDXSprite->SetTransform ( &matrix );
            m_pDXSprite->Draw ( m_pDXPixelTexture, NULL, NULL, NULL, Item.Rect.ulColor );
            break;
        }
        case QUEUE_TEXT:
        {
            RECT rect;        
            SetRect ( &rect, Item.Text.iLeft, Item.Text.iTop, Item.Text.iRight, Item.Text.iBottom );  
            D3DXMATRIX matrix;
            D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
            D3DXVECTOR2 scaling ( Item.Text.fScaleX, Item.Text.fScaleY );
            D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, NULL );
            m_pDXSprite->SetTransform ( &matrix );        
            Item.Text.pDXFont->DrawText ( m_pDXSprite, Item.strText.c_str (), -1, &rect, Item.Text.ulFormat, Item.Text.ulColor );
            break;
        }
        case QUEUE_TEXTURE:
        {
            RECT cutImagePos;
            const float fSurfaceWidth  = Item.Texture.info.uiSurfaceWidth;
            const float fSurfaceHeight = Item.Texture.info.uiSurfaceHeight;
            const float fFileWidth     = Item.Texture.info.uiFileWidth;
            const float fFileHeight    = Item.Texture.info.uiFileHeight;
            cutImagePos.left    = ( Item.Texture.fU )                       * ( Item.Texture.bRelativeUV ? fSurfaceWidth  : fSurfaceWidth  / fFileWidth );
            cutImagePos.top     = ( Item.Texture.fV )                       * ( Item.Texture.bRelativeUV ? fSurfaceHeight : fSurfaceHeight / fFileHeight );
            cutImagePos.right   = ( Item.Texture.fU + Item.Texture.fSizeU ) * ( Item.Texture.bRelativeUV ? fSurfaceWidth  : fSurfaceWidth  / fFileWidth );
            cutImagePos.bottom  = ( Item.Texture.fV + Item.Texture.fSizeV ) * ( Item.Texture.bRelativeUV ? fSurfaceHeight : fSurfaceHeight / fFileHeight );
            const float fCutWidth  = cutImagePos.right - cutImagePos.left;
            const float fCutHeight = cutImagePos.bottom - cutImagePos.top;
            const D3DXVECTOR2 scaling         ( Item.Texture.fWidth / fCutWidth, Item.Texture.fHeight / fCutHeight );
            const D3DXVECTOR2 rotationCenter  ( Item.Texture.fWidth * 0.5f + Item.Texture.fRotCenOffX, Item.Texture.fHeight * 0.5f + Item.Texture.fRotCenOffY );
            const D3DXVECTOR2 position        ( Item.Texture.fX, Item.Texture.fY );
            const float fRotationRad  = Item.Texture.fRotation * (6.2832f/360.f);
            D3DXMATRIX matrix;
            D3DXMatrixTransformation2D  ( &matrix, NULL, 0.0f, &scaling, &rotationCenter, fRotationRad, &position );
            m_pDXSprite->SetTransform ( &matrix );
            m_pDXSprite->Draw ( Item.Texture.info.d3dTexture, &cutImagePos, NULL, NULL, Item.Texture.ulColor );
            break;
        }
        // Circle type?
        case QUEUE_CIRCLE:
        {
            break;
        }
    }
}


// Cache a texture for current and future use.
SCachedTextureInfo& CGraphics::CacheTexture ( const string& strFilename )
{
    // Find exisiting
    map < string, SCachedTextureInfo >::iterator iter = m_CachedTextureInfoMap.find ( strFilename );

    if ( iter == m_CachedTextureInfoMap.end () )
    {
        // Add if not found
        m_CachedTextureInfoMap[strFilename] = SCachedTextureInfo();
        iter = m_CachedTextureInfoMap.find ( strFilename );

        SCachedTextureInfo& info = iter->second;
        info.d3dTexture = LoadTexture( strFilename.c_str () );
        info.uiFileWidth = 1;
        info.uiFileHeight = 1;
        info.uiSurfaceWidth = 1;
        info.uiSurfaceHeight = 1;

        if ( info.d3dTexture )
        {
            D3DXIMAGE_INFO imageInfo;
            if ( SUCCEEDED ( D3DXGetImageInfoFromFile( strFilename.c_str (), &imageInfo ) ) )
            {
                info.uiFileWidth = imageInfo.Width;
                info.uiFileHeight = imageInfo.Height;
            }
            D3DSURFACE_DESC surfaceDesc;
            if ( SUCCEEDED ( info.d3dTexture->GetLevelDesc( 0, &surfaceDesc ) ) )
            {
                info.uiSurfaceWidth = surfaceDesc.Width;
                info.uiSurfaceHeight = surfaceDesc.Height;
            }
        }
    }

    SCachedTextureInfo& info = iter->second;
    info.ulTimeLastUsed = GetTickCount();

    return info;
}


// Remove any cached texures that have not been used for a little while.
void CGraphics::ExpireCachedTextures ( bool bExpireAll )
{
    // Expire unused cached textures
    // 1 cached texture     = 15 seconds till expire
    // 50 cached textures   = 8 seconds till expire
    // 100 cached textures  = 1 second till expire
    long ulNumTextures              = m_CachedTextureInfoMap.size ();
    unsigned long ulMaxAgeSeconds   = Max < long > ( 1, 15 - (ulNumTextures * 15 / 100) );

    map < string, SCachedTextureInfo > ::iterator iter = m_CachedTextureInfoMap.begin ();
    while ( iter != m_CachedTextureInfoMap.end () )
    {
        SCachedTextureInfo& info    = iter->second;
        unsigned long ulAge         = GetTickCount() - info.ulTimeLastUsed;
        if ( ulAge > ulMaxAgeSeconds * 1000 || bExpireAll )
        {
            SAFE_RELEASE ( info.d3dTexture );
            iter = m_CachedTextureInfoMap.erase ( iter );
        }
        else
            ++iter;
    }
}

ID3DXFont* CGraphics::GetBigFont ( ID3DXFont* pDXFont )
{
    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        if ( m_pDXFonts [ i ] == pDXFont )
            return m_pBigDXFonts [ i ];
    }
    return pDXFont;
}
