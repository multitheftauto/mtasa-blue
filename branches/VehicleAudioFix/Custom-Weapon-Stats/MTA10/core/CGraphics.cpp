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
#include "CLine3DBatcher.h"
extern CCore* g_pCore;

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

    m_iDebugQueueRefs = 0;
    m_iDrawBatchRefCount = 0;
    m_BlendMode = EBlendMode::BLEND;
    m_BatchBlendMode = EBlendMode::BLEND;

    m_pRenderItemManager = new CRenderItemManager ();
    m_pTileBatcher = new CTileBatcher ();
    m_pLine3DBatcherPreGUI = new CLine3DBatcher ( true );
    m_pLine3DBatcherPostGUI = new CLine3DBatcher ( false );
    m_bSetRenderTargetEnabled = false;
}


CGraphics::~CGraphics ( void )
{
    if ( m_pLineInterface )
        m_pLineInterface->Release ();

    DestroyStandardDXFonts ();

    SAFE_DELETE ( m_pRenderItemManager );
    SAFE_DELETE ( m_pTileBatcher );
    SAFE_DELETE ( m_pLine3DBatcherPreGUI );
    SAFE_DELETE ( m_pLine3DBatcherPostGUI );
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
    pDXFont = MaybeGetBigFont ( pDXFont, fScaleX, fScaleY );

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

        BeginDrawBatch ();
            D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, NULL );
            m_pDXSprite->SetTransform ( &matrix );  
            
            // Convert to UTF16
            std::wstring strText = MbUTF8ToUTF16(szText);

            pDXFont->DrawTextW ( m_pDXSprite, strText.c_str(), -1, &rect, ulFormat, ModifyColorForBlendMode ( ulColor ) );
        EndDrawBatch ();
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


void CGraphics::DrawLine3D ( const CVector& vecBegin, const CVector& vecEnd, unsigned long ulColor, float fWidth )
{
    DrawLine3DQueued ( vecBegin, vecEnd, fWidth, ulColor, true );
}


void CGraphics::DrawRectangle ( float fX, float fY, float fWidth, float fHeight, unsigned long ulColor )
{      
    BeginDrawBatch ();
    D3DXMATRIX matrix;
    D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
    D3DXVECTOR2 scaling ( fWidth, fHeight );
    D3DXVECTOR2 position ( fX, fY );
    D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, &position );
    m_pDXSprite->SetTransform ( &matrix );
    m_pDXSprite->Draw ( m_pDXPixelTexture, NULL, NULL, NULL, ModifyColorForBlendMode ( ulColor ) );
    EndDrawBatch ();
}


//
// Set current blend mode
// 
void CGraphics::SetBlendMode ( EBlendModeType blendMode )
{
    m_BlendMode = blendMode;
}


//
// Modify diffuse color if blend mode requires it
// 
SColor CGraphics::ModifyColorForBlendMode ( SColor color )
{
    if ( m_BlendMode == EBlendMode::MODULATE_ADD )
    {
        // Apply modulation to diffuse color also
        color.R = color.R * color.A / 256;
        color.G = color.G * color.A / 256;
        color.B = color.B * color.A / 256;
    }
    return color;
}


//
// BeginDrawBatch
//
// Slightly speed up successive uses of DXSprite.
// Matching EndDrawBatch() must be called otherwise end of world.
//
void CGraphics::BeginDrawBatch ( void )
{
    if ( m_BlendMode != m_BatchBlendMode )
    {
        if ( m_iDrawBatchRefCount > 0 )
        {
            // Flush if changing blend mode
            m_iDrawBatchRefCount = 1;
            EndDrawBatch ();
        }
        m_BatchBlendMode = m_BlendMode;
    }

    if ( m_iDrawBatchRefCount++ == 0 )
    {
        if ( m_BlendMode == EBlendMode::BLEND )
        {
            // Draw NonPM texture
            m_pDevice->SetRenderState ( D3DRS_ZENABLE,          D3DZB_FALSE );
            m_pDevice->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_NONE );
            m_pDevice->SetRenderState ( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD );
            m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
            m_pDevice->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
            m_pDevice->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
            m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,  TRUE );
            m_pDevice->SetRenderState ( D3DRS_ALPHAREF,         0x01 );
            m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
            m_pDevice->SetRenderState ( D3DRS_LIGHTING,         FALSE);

            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,        D3DTOP_MODULATE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,      D3DTA_DIFFUSE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,      D3DTA_TEXTURE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,        D3DTOP_MODULATE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,      D3DTA_DIFFUSE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,      D3DTA_TEXTURE );

            m_pDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,        D3DTOP_DISABLE );
            m_pDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,        D3DTOP_DISABLE );
        }
        else
        if ( m_BlendMode == EBlendMode::MODULATE_ADD )
        {
            // Draw NonPM texture as PM texture
            m_pDevice->SetRenderState ( D3DRS_ZENABLE,          D3DZB_FALSE );
            m_pDevice->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_NONE );
            m_pDevice->SetRenderState ( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD );
            m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
            m_pDevice->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_ONE );
            m_pDevice->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
            m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,  TRUE );
            m_pDevice->SetRenderState ( D3DRS_ALPHAREF,         0x01 );
            m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
            m_pDevice->SetRenderState ( D3DRS_LIGHTING,         FALSE);
            m_pDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,    0 );

            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

            m_pDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_BLENDCURRENTALPHA );
            m_pDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
            m_pDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );
            m_pDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
            m_pDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
            m_pDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

            m_pDevice->SetTextureStageState ( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
            m_pDevice->SetTextureStageState ( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

        }
        else
        if ( m_BlendMode == EBlendMode::ADD )
        {
            // Draw PM texture
            m_pDevice->SetRenderState ( D3DRS_ZENABLE,          D3DZB_FALSE );
            m_pDevice->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_NONE );
            m_pDevice->SetRenderState ( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD );
            m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
            m_pDevice->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_ONE );
            m_pDevice->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
            m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,  TRUE );
            m_pDevice->SetRenderState ( D3DRS_ALPHAREF,         0x01 );
            m_pDevice->SetRenderState ( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
            m_pDevice->SetRenderState ( D3DRS_LIGHTING,         FALSE );

            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
            m_pDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

            m_pDevice->SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
            m_pDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

        }
        else
            assert ( 0 );

        m_pDXSprite->Begin ( D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE
                | D3DXSPRITE_DONOTSAVESTATE
                | D3DXSPRITE_DONOTMODIFY_RENDERSTATE
            );
    }
}

//
// EndDrawBatch
//
void CGraphics::EndDrawBatch ( void )
{
    if ( --m_iDrawBatchRefCount == 0 )
        m_pDXSprite->End ();
    m_iDrawBatchRefCount = Max ( 0, m_iDrawBatchRefCount );
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

    pDXFont = MaybeGetBigFont ( pDXFont, fScale, fScale );

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

    pDXFont = MaybeGetBigFont ( pDXFont, fScale, fScale );

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

    pDXFont = MaybeGetBigFont ( pDXFont, fScale, fScale );

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
    if ( g_pCore->IsWindowMinimized () )
        return;

    // Add it to the queue
    if ( bPostGUI )
        m_pLine3DBatcherPostGUI->AddLine3D ( vecBegin, vecEnd, fWidth, ulColor ); 
    else
        m_pLine3DBatcherPreGUI->AddLine3D ( vecBegin, vecEnd, fWidth, ulColor ); 
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


void CGraphics::DrawTextureQueued ( float fX, float fY,
                                 float fWidth, float fHeight,
                                 float fU, float fV,
                                 float fSizeU, float fSizeV,
                                 bool bRelativeUV,
                                 CMaterialItem* pMaterial,
                                 float fRotation,
                                 float fRotCenOffX,
                                 float fRotCenOffY,
                                 unsigned long ulColor,
                                 bool bPostGUI )
{

    // Set up a queue item
    sDrawQueueItem Item;
    Item.Texture.fX = fX;
    Item.Texture.fY = fY;
    Item.Texture.fWidth = fWidth;
    Item.Texture.fHeight = fHeight;
    Item.Texture.fU = fU;
    Item.Texture.fV = fV;
    Item.Texture.fSizeU = fSizeU;
    Item.Texture.fSizeV = fSizeV;
    Item.Texture.bRelativeUV = bRelativeUV;
    Item.Texture.fRotation = fRotation;
    Item.Texture.fRotCenOffX = fRotCenOffX;
    Item.Texture.fRotCenOffY = fRotCenOffY;
    Item.Texture.ulColor = ulColor;

    if ( CShaderItem* pShaderItem = DynamicCast < CShaderItem >( pMaterial ) )
    {
        // If material is a shader, use its current instance
        Item.Texture.pMaterial = pShaderItem->m_pShaderInstance;
        Item.eType = QUEUE_SHADER;
    }
    else
    {
        // Otherwise material must be a texture
        Item.Texture.pMaterial = pMaterial;
        Item.eType = QUEUE_TEXTURE;
    }

    // Keep material valid while in the queue
    AddQueueRef ( Item.Texture.pMaterial );

    // Add it to the queue
    AddQueueItem ( Item, bPostGUI );
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
    pDXFont = MaybeGetBigFont ( pDXFont, fScaleX, fScaleY );

    if ( pDXFont )
    {
        if ( fScaleX != 1.0f || fScaleY != 1.0f )
        {
	        iLeft = unsigned int ( ( float ) iLeft * ( 1.0f / fScaleX ) );
	        iTop = unsigned int ( ( float ) iTop * ( 1.0f / fScaleY ) );
	        iRight = unsigned int ( ( float ) iRight * ( 1.0f / fScaleX ) );
	        iBottom = unsigned int ( ( float ) iBottom * ( 1.0f / fScaleY ) );
        }

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

        // Keep font valid while in the queue incase it's a custom font
        AddQueueRef ( Item.Text.pDXFont );

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

bool CGraphics::LoadAdditionalDXFont ( std::string strFontPath, std::string strFontName, unsigned int uiHeight, bool bBold, ID3DXFont** ppD3DXFont )
{
    int iLoaded = AddFontResourceEx ( strFontPath.c_str (), FR_PRIVATE, 0 );

    int iWeight = bBold ? FW_BOLD : FW_NORMAL;
    *ppD3DXFont = NULL;

    bool bSuccess = true;
    // Normal size
    if( !SUCCEEDED ( D3DXCreateFont ( m_pDevice, uiHeight, 0, iWeight, 1,
        FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, strFontName.c_str(),
        ppD3DXFont ) ) )
    {
        CLogger::GetSingleton ().ErrorPrintf( "Could not create Direct3D font '%s'", strFontName.c_str() );
        bSuccess = false;
    }

    return bSuccess && ( iLoaded == 1 );
}

bool CGraphics::DestroyAdditionalDXFont ( std::string strFontPath, ID3DXFont *pD3DXFont )
{
    bool bResult = RemoveFontResourceEx ( strFontPath.c_str (), FR_PRIVATE, 0 ) != 0;
    SAFE_RELEASE( pD3DXFont );
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

void CGraphics::DrawTexture ( CTextureItem* pTexture, float fX, float fY, float fScaleX, float fScaleY, float fRotation, float fCenterX, float fCenterY, DWORD dwColor )
{
    if ( !pTexture )
        return;

    const float fSurfaceWidth  = pTexture->m_uiSurfaceSizeX;
    const float fSurfaceHeight = pTexture->m_uiSurfaceSizeY;
    const float fFileWidth     = pTexture->m_uiSizeX;
    const float fFileHeight    = pTexture->m_uiSizeY;

    BeginDrawBatch ();
    D3DXMATRIX matrix;
    D3DXVECTOR2 scaling ( fScaleX * fFileWidth / fSurfaceWidth, fScaleY * fFileHeight / fSurfaceHeight );
    D3DXVECTOR2 rotationCenter  ( fFileWidth * fScaleX * fCenterX, fFileHeight * fScaleX * fCenterY );
    D3DXVECTOR2 position ( fX - fFileWidth * fScaleX * fCenterX, fY - fFileHeight * fScaleY * fCenterY );
    D3DXMatrixTransformation2D ( &matrix, NULL, NULL, &scaling, &rotationCenter, fRotation * 6.2832f / 360.f, &position );
    m_pDXSprite->SetTransform ( &matrix );
    m_pDXSprite->Draw ( (IDirect3DTexture9*)pTexture->m_pD3DTexture, NULL, NULL, NULL, ModifyColorForBlendMode ( dwColor ) );
    EndDrawBatch ();
}

void CGraphics::OnDeviceCreate ( IDirect3DDevice9 * pDevice )
{
    m_pDevice = pDevice;

    LoadStandardDXFonts ();

    // Create drawing devices
    D3DXCreateLine ( pDevice, &m_pLineInterface );
    D3DXCreateTextureFromFileInMemory ( pDevice, g_szPixel, sizeof ( g_szPixel ), &m_pDXPixelTexture );

    m_pTileBatcher->OnDeviceCreate ( pDevice, GetViewportWidth (), GetViewportHeight () );
    m_pLine3DBatcherPreGUI->OnDeviceCreate ( pDevice, GetViewportWidth (), GetViewportHeight () );
    m_pLine3DBatcherPostGUI->OnDeviceCreate ( pDevice, GetViewportWidth (), GetViewportHeight () );
    m_pRenderItemManager->OnDeviceCreate ( pDevice, GetViewportWidth (), GetViewportHeight () );
}


void CGraphics::OnDeviceInvalidate ( IDirect3DDevice9 * pDevice )
{
    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        if( m_pDXFonts[i] ) m_pDXFonts[i]->OnLostDevice ();
        if( m_pBigDXFonts[i] ) m_pBigDXFonts[i]->OnLostDevice ();
    }

    if ( m_pDXSprite )
        m_pDXSprite->OnLostDevice ();

    if ( m_pLineInterface )
        m_pLineInterface->OnLostDevice ();

    m_pRenderItemManager->OnLostDevice ();
}


void CGraphics::OnDeviceRestore ( IDirect3DDevice9 * pDevice )
{
    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        if( m_pDXFonts[i] ) m_pDXFonts[i]->OnResetDevice ();
        if( m_pBigDXFonts[i] ) m_pBigDXFonts[i]->OnResetDevice ();
    }

    if ( m_pDXSprite )
        m_pDXSprite->OnResetDevice ();

    if ( m_pLineInterface )
        m_pLineInterface->OnResetDevice ();

    m_pRenderItemManager->OnResetDevice ();
}


void CGraphics::DrawPreGUIQueue ( void )
{
    m_pLine3DBatcherPreGUI->Flush ();
    DrawQueue ( m_PreGUIQueue );
}


void CGraphics::DrawPostGUIQueue ( void )
{
    DrawQueue ( m_PostGUIQueue );
    m_pLine3DBatcherPostGUI->Flush ();

    // Both queues should be empty now, and there should be no outstanding refs
    assert ( m_PreGUIQueue.empty () && m_iDebugQueueRefs == 0 );
}


void CGraphics::HandleDrawQueueModeChange ( uint curMode, uint newMode )
{
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

    // Changing to...
    if ( newMode == 'spri' )
    {
        // ...sprite mode
        m_pDXSprite->Begin ( D3DXSPRITE_ALPHABLEND );
        m_pDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        m_pDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    }
}


void CGraphics::DrawQueue ( std::vector < sDrawQueueItem >& Queue )
{
    // Items to draw?
    if ( Queue.size () > 0 )
    {
        // Loop through it
        int curMode = 0;
        std::vector < sDrawQueueItem >::iterator iter = Queue.begin ();
        for ( ; iter != Queue.end (); iter++ )
        {
            const sDrawQueueItem& item = *iter;

            // Determine new mode
            uint newMode;
            if ( item.eType == QUEUE_SHADER )                                   newMode = 'tile';
            else if ( item.eType == QUEUE_LINE )                                newMode = 'misc';
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
            RemoveQueueRef ( Item.Text.pDXFont );
            break;
        }
        case QUEUE_TEXTURE:
        {
            if ( CTextureItem* pTexture = DynamicCast < CTextureItem > ( Item.Texture.pMaterial ) )
            {
                const sDrawQueueTexture& t = Item.Texture;
                RECT cutImagePos;
                const float fSurfaceWidth  = pTexture->m_uiSurfaceSizeX;
                const float fSurfaceHeight = pTexture->m_uiSurfaceSizeY;
                const float fFileWidth     = Item.Texture.pMaterial->m_uiSizeX;
                const float fFileHeight    = Item.Texture.pMaterial->m_uiSizeY;
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
                m_pDXSprite->Draw ( (IDirect3DTexture9*)pTexture->m_pD3DTexture, &cutImagePos, NULL, NULL, Item.Texture.ulColor );
            }
            RemoveQueueRef ( Item.Texture.pMaterial );
            break;
        }
        case QUEUE_SHADER:
        {
            const sDrawQueueTexture& t = Item.Texture;
            float fU1 = t.fU;
            float fV1 = t.fV;
            float fU2 = ( t.fU + t.fSizeU );
            float fV2 = ( t.fV + t.fSizeV );
            if ( !t.bRelativeUV )
            {
                // If UV's are absolute pixels, then scale the range to 0.0f - 1.0f.
                float fUScale = 1.0f / (float)t.pMaterial->m_uiSizeX;
                float fVScale = 1.0f / (float)t.pMaterial->m_uiSizeY;
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


//
// Maybe change to a higher resolution font
//
ID3DXFont* CGraphics::MaybeGetBigFont ( ID3DXFont* pDXFont, float& fScaleX, float& fScaleY )
{
    if ( fScaleX > 1.1f || fScaleY > 1.1f )
    {
        for ( int i = 0; i < NUM_FONTS; i++ )
        {
            if ( m_pDXFonts [ i ] == pDXFont )
            {
                // Adjust scale to compensate for higher res font
                fScaleX *= 0.25f;
                if ( &fScaleX != &fScaleY )     // Check fScaleY is not the same variable
                    fScaleY *= 0.25f;
                return m_pBigDXFonts [ i ];
            }
        }
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
        if ( item.eType == QUEUE_TEXTURE || item.eType == QUEUE_SHADER )
            RemoveQueueRef ( item.Texture.pMaterial );
        else
        if ( item.eType == QUEUE_TEXT )
            RemoveQueueRef ( item.Text.pDXFont );
    }
    Queue.clear ();
}


//
// Use ref counting to prevent render items from being deleted while in the queue
//
void CGraphics::AddQueueRef ( CRenderItem* pRenderItem )
{
    pRenderItem->AddRef ();
    m_iDebugQueueRefs++;    // For debugging
}

void CGraphics::RemoveQueueRef ( CRenderItem* pRenderItem )
{
    pRenderItem->Release ();
    m_iDebugQueueRefs--;    // For debugging
}

void CGraphics::AddQueueRef ( IUnknown* pUnknown )
{
    pUnknown->AddRef ();
    m_iDebugQueueRefs++;    // For debugging
}

void CGraphics::RemoveQueueRef ( IUnknown* pUnknown )
{
    pUnknown->Release ();
    m_iDebugQueueRefs--;    // For debugging
}


// Entering or leaving a section where the rendertarget can be changed from script
void CGraphics::EnableSetRenderTarget ( bool bEnable )
{
    // Must be changing
    assert ( m_bSetRenderTargetEnabled != bEnable );

    if ( !bEnable )
        m_pRenderItemManager->RestoreDefaultRenderTarget ();
    else
        m_pRenderItemManager->SaveDefaultRenderTarget ();

    m_bSetRenderTargetEnabled = bEnable;
}

// Notification that the render target will be changing
void CGraphics::OnChangingRenderTarget ( uint uiNewViewportSizeX, uint uiNewViewportSizeY )
{
    // Flush dx draws
    DrawPreGUIQueue ();
    // Inform tile batcher
    m_pTileBatcher->OnChangingRenderTarget ( uiNewViewportSizeX, uiNewViewportSizeY );
}
