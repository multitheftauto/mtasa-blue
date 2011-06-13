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
#include "CTileBatcher.h"

using namespace std;

template<> CGraphics * CSingleton< CGraphics >::m_pSingleton = NULL;

const unsigned char g_szPixel [] = { 0x42, 0x4D, 0x3A, 0, 0, 0, 0, 0, 0, 0, 0x36, 0, 0, 0, 0x28, 0, 0,
                                    0, 0x1, 0, 0, 0, 0x1, 0, 0, 0, 0x1, 0, 0x18, 0, 0, 0, 0, 0,
                                    0x4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0xFF, 0xFF, 0xFF, 0 };

CGraphics::CGraphics ( CLocalGUI* pGUI )
{
    m_pGUI = pGUI;
    memset ( m_pDXFonts, 0, sizeof ( m_pDXFonts ) );
    memset ( m_pBigDXFonts, 0, sizeof ( m_pBigDXFonts ) );
    m_pDevice = NULL;
    m_pLineInterface = NULL;
    m_pDXSprite = NULL;

    m_pRenderTarget = NULL;
    m_pOriginalTarget = NULL;

    m_bIsDrawing = false;
    m_iDebugQueueRefs = 0;

    m_pRenderItemManager = new CRenderItemManager ();
    m_pTileBatcher = new CTileBatcher ();
}


CGraphics::~CGraphics ( void )
{
    if ( m_pLineInterface )
        m_pLineInterface->Release ();

    DestroyStandardDXFonts ();

    SAFE_DELETE ( m_pRenderItemManager );
    SAFE_DELETE ( m_pTileBatcher );
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
            
            // Convert to UTF16
            std::wstring strText = MbUTF8ToUTF16(szText);

            pDXFont->DrawTextW ( m_pDXSprite, strText.c_str(), -1, &rect, ulFormat, ulColor );
        m_pDXSprite->End ();
    }        
}


void CGraphics::DrawText ( int iX, int iY, unsigned long dwColor, float fScale, const char * szText, ... )
{
    char szBuffer [ 1024 ];
    va_list ap;
    va_start ( ap, szText );
    VSNPRINTF ( szBuffer, 1024, szText, ap );
    va_end ( ap );

    DrawText ( iX, iY, iX, iY, dwColor, szBuffer, fScale, fScale, DT_NOCLIP );
}


void CGraphics::DrawText2DA ( int uiX, int uiY, unsigned long ulColor, float fScale, const char* szDisplayText, ... )
{
    char szBuffer [ 1024 ];
    va_list ap;
    va_start ( ap, szDisplayText );
    VSNPRINTF ( szBuffer, 1024, szDisplayText, ap );
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
    VSNPRINTF ( szBuffer, 1024, szDisplayText, ap );
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
    D3DXVECTOR3 vec(fX, fY, fZ);
    m_pGUI->GetRenderingLibrary ()->Render3DSprite ( NULL, 1.0f, &vec, &ViewMatrix, ulColor );

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

        std::wstring strText = MbUTF8ToUTF16(szText);

        GetTextExtentPoint32W ( dc, strText.c_str(), strText.length(), &size );

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
                                 SMaterialItem* pMaterial,
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
    Item.Texture.pMaterial = pMaterial;
    Item.Texture.fRotation = fRotation;
    Item.Texture.fRotCenOffX = fRotCenOffX;
    Item.Texture.fRotCenOffY = fRotCenOffY;
    Item.Texture.ulColor = ulColor;

    // Keep material valid while in the queue
    AddQueueRef ( pMaterial );

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

        // Convert to wstring        
        Item.strText = MbUTF8ToUTF16(szText);

        // Add it to the queue
        AddQueueItem ( Item, bPostGUI );
    }
}

bool CGraphics::LoadStandardDXFonts ( void )
{
    // Add our custom font resources
    if ( m_FontResourceNames.empty () )
    {
        m_FontResourceNames.push_back ( "pricedown.ttf" );
        m_FontResourceNames.push_back ( "sabankgothic.ttf" );
        m_FontResourceNames.push_back ( "saheader.ttf" );
        m_FontResourceNames.push_back ( "sagothic.ttf" );
        m_FontResourceNames.push_back ( "unifont-5.1.20080907.ttf" );
    }

    for ( uint i = 0 ; i < m_FontResourceNames.size () ; i++ )
    {
        if ( !AddFontResourceEx ( CalcMTASAPath ( "MTA\\cgui\\" + m_FontResourceNames[i] ), FR_PRIVATE, 0 ) )
        {
            BrowseToSolution ( "mta-datafiles-missing", true, true, true, "Error loading MTA font " + m_FontResourceNames[i] );
        }
    }

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
        { "beckett",              30, FW_NORMAL },
        { "unifont",              14, FW_NORMAL }
    };

    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        m_pDXFonts[i] = m_pBigDXFonts[i] = NULL;

        // Normal size
        if( SUCCEEDED ( D3DXCreateFont ( m_pDevice, fontInfos[i].uiHeight, 0, fontInfos[i].uiWeight, 1,
            FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontInfos[i].szName,
            &m_pDXFonts[i] ) ) )
        {
            // Big size (4x)
            if( SUCCEEDED ( D3DXCreateFont ( m_pDevice, fontInfos[i].uiHeight*4, 0, fontInfos[i].uiWeight, 1,
                FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontInfos[i].szName,
                &m_pBigDXFonts[i] ) ) )
            {
                continue;
            }
        }

        SString strMessage ( "Could not create Direct3D font '%s'", fontInfos[i].szName );
        CLogger::GetSingleton ().ErrorPrintf( "%s", *strMessage );
        BrowseToSolution ( "create-fonts", true, true, true, strMessage );
    }

    if( !SUCCEEDED ( D3DXCreateSprite ( m_pDevice, &m_pDXSprite ) ) )
    {
        BrowseToSolution ( "fonts-create-sprite-fail", true, true, true, "Error calling CGraphics::LoadFonts() D3DXCreateSprite" );
    }

    return true;
}

bool CGraphics::LoadAdditionalDXFont ( std::string strFontPath, std::string strFontName, unsigned int uiHeight, bool bBold, ID3DXFont** pDXSmallFont, ID3DXFont** pDXBigFont )
{
    int iLoaded = AddFontResourceEx ( strFontPath.c_str (), FR_PRIVATE, 0 );

    int iWeight = bBold ? FW_BOLD : FW_NORMAL;
    ID3DXFont *pNewDXSmallFont = NULL;
    ID3DXFont *pNewDXBigFont = NULL;

    bool bSuccess = true;
    // Normal size
    if( !SUCCEEDED ( D3DXCreateFont ( m_pDevice, uiHeight, 0, iWeight, 1,
        FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, strFontName.c_str(),
        &pNewDXSmallFont ) ) )
    {
        CLogger::GetSingleton ().ErrorPrintf( "Could not create Direct3D font '%s'", strFontName.c_str() );
        bSuccess = false;
    }

    // Big size (4x)
    if( !SUCCEEDED ( D3DXCreateFont ( m_pDevice, uiHeight*4, 0, iWeight, 1,
        FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, strFontName.c_str(),
        &pNewDXBigFont ) ) )
    {
        CLogger::GetSingleton ().ErrorPrintf( "Could not create Direct3D big font '%s'", strFontName.c_str() );
        bSuccess = false;
    }
    *pDXSmallFont = pNewDXSmallFont;
    *pDXBigFont = pNewDXBigFont;

    return bSuccess && SUCCEEDED ( D3DXCreateSprite ( m_pDevice, &m_pDXSprite ) ) && ( iLoaded == 1 );
}

bool CGraphics::DestroyAdditionalDXFont ( std::string strFontPath, ID3DXFont *pDXSmallFont, ID3DXFont *pDXBigFont )
{
    bool bResult = RemoveFontResourceEx ( strFontPath.c_str (), FR_PRIVATE, 0 ) != 0;
    SAFE_RELEASE( pDXSmallFont );
    SAFE_RELEASE( pDXBigFont );
    return bResult;
}

bool CGraphics::DestroyStandardDXFonts ( void )
{
    // Remove our custom font resources (needs to be identical to LoadFonts)
    for ( uint i = 0 ; i < m_FontResourceNames.size () ; i++ )
    {
        RemoveFontResourceEx ( CalcMTASAPath ( "MTA\\cgui\\" + m_FontResourceNames[i] ), FR_PRIVATE, 0 );
    }

    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        SAFE_RELEASE( m_pDXFonts[i] );
        SAFE_RELEASE( m_pBigDXFonts[i] );
    }

    // Release 
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
    if ( !texture )
        return;
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

    LoadStandardDXFonts ();

    // Get the original render target
    assert ( !m_pOriginalTarget );
    assert ( m_pDevice->GetRenderTarget ( 0, &m_pOriginalTarget ) == D3D_OK );

    // Create drawing devices
    D3DXCreateLine ( pDevice, &m_pLineInterface );
    D3DXCreateTextureFromFileInMemory ( pDevice, g_szPixel, sizeof ( g_szPixel ), &m_pDXPixelTexture );

    m_pTileBatcher->OnDeviceCreate ( pDevice, GetViewportWidth (), GetViewportHeight () );
    m_pRenderItemManager->OnDeviceCreate ( pDevice, GetViewportWidth (), GetViewportHeight () );
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

    // Both queues should be empty now, and there should be no outstanding refs
    assert ( m_PreGUIQueue.empty () && m_iDebugQueueRefs == 0 );
}


void CGraphics::HandleDrawQueueModeChange ( uint curMode, uint newMode )
{
    // Changing to...
    if ( newMode == 'spri' )
    {
        // ...sprite mode
        m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND );
        m_pDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        m_pDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    }

    // Changing from...
    if ( curMode == 'spri' )
    {
        // ...sprite mode
        m_pDXSprite->End ();
    }
    else
    if ( curMode == 'tile' )
    {
        // ...tile mode
        m_pTileBatcher->Flush ();
    }
}


void CGraphics::DrawQueue ( std::vector < sDrawQueueItem >& Queue )
{
    // Items to draw?
    if ( Queue.size () > 0 )
    {
        BeginSingleDrawing ();

        // Loop through it
        int curMode = 0;
        std::vector < sDrawQueueItem >::iterator iter = Queue.begin ();
        for ( ; iter != Queue.end (); iter++ )
        {
            const sDrawQueueItem& item = *iter;

            // Determine new mode
            uint newMode;
            if ( item.eType == QUEUE_TEXTURE )                                 newMode = 'tile';
            else if ( item.eType == QUEUE_LINE || item.eType == QUEUE_LINE3D )  newMode = 'misc';
            else                                                                newMode = 'spri';

            // Switching mode ?
            if ( curMode != newMode )
            {
                HandleDrawQueueModeChange ( curMode, newMode );
                curMode = newMode;
            }

            // Draw the item
            DrawQueueItem ( item );
        }

        // Mode clean up
        HandleDrawQueueModeChange ( curMode, 0 );

        EndSingleDrawing ();

        // Clear the list
        Queue.clear ();
    }
}


void CGraphics::AddQueueItem ( const sDrawQueueItem& Item, bool bPostGUI )
{
    // Add it to the correct queue
    if ( bPostGUI && !CCore::GetSingleton ().IsMenuVisible() ) //Don't draw over the main menu.  Ever.
        m_PostGUIQueue.push_back ( Item );
    else
        m_PreGUIQueue.push_back ( Item );

    // Prevent queuing when minimized
    if ( g_pCore->IsWindowMinimized () )
    {
        ClearDrawQueue ( m_PreGUIQueue );
        ClearDrawQueue ( m_PostGUIQueue );
        assert ( m_iDebugQueueRefs == 0 );
        return;
    }
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
            Item.Text.pDXFont->DrawTextW ( m_pDXSprite, Item.strText.c_str (), -1, &rect, Item.Text.ulFormat, Item.Text.ulColor );
            break;
        }
        case QUEUE_TEXTURE:
        {
            const sDrawQueueTexture& t = Item.Texture;
            float fU1 = t.fU;
            float fV1 = t.fV;
            float fU2 = ( t.fU + t.fSizeU );
            float fV2 = ( t.fV + t.fSizeV );
            if ( !t.bRelativeUV )
            {
                // If UV's are absolute pixels, then scale the range to 0.0f - 1.0f.
                float fUScale = 1.0f / (float)t.pMaterial->uiSizeX;
                float fVScale = 1.0f / (float)t.pMaterial->uiSizeY;
                fU1 *= fUScale;
                fV1 *= fVScale;
                fU2 *= fUScale;
                fV2 *= fVScale;
            }
            m_pTileBatcher->AddTile ( t.fX, t.fY, t.fX+t.fWidth, t.fY+t.fHeight, fU1, fV1, fU2, fV2, t.pMaterial, t.fRotation, t.fRotCenOffX, t.fRotCenOffY, t.ulColor );
            RemoveQueueRef ( Item.Texture.pMaterial );
            break;
        }
        // Circle type?
        case QUEUE_CIRCLE:
        {
            break;
        }
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


//
// Clear queue, releasing render items where necessary
//
void CGraphics::ClearDrawQueue ( std::vector < sDrawQueueItem >& Queue )
{
    for ( std::vector < sDrawQueueItem >::const_iterator iter = Queue.begin () ; iter != Queue.end (); iter++ )
    {
        const sDrawQueueItem& item = *iter;
        if ( item.eType == QUEUE_TEXTURE )
            RemoveQueueRef ( item.Texture.pMaterial );
    }
    Queue.clear ();
}


//
// Use ref counting to prevent render items from being deleted while in the queue
//
void CGraphics::AddQueueRef ( SRenderItem* pRenderItem )
{
    pRenderItem->AddRef ();
    m_iDebugQueueRefs++;    // For debugging
}

void CGraphics::RemoveQueueRef ( SRenderItem* pRenderItem )
{
    pRenderItem->Release ();
    m_iDebugQueueRefs--;    // For debugging
}

// Entering or leaving a section where the rendertarget can be changed from script
void CGraphics::EnableSetRenderTarget ( bool bEnable )
{
    if ( !bEnable )
        m_pRenderItemManager->RestoreDefaultRenderTarget ();
}

// Notification that the render target will be changing
void CGraphics::OnChangingRenderTarget ( uint uiNewViewportSizeX, uint uiNewViewportSizeY )
{
    // Flush dx draws
    DrawPreGUIQueue ();
    // Inform tile batcher
    m_pTileBatcher->OnChangingRenderTarget ( uiNewViewportSizeX, uiNewViewportSizeY );
}
