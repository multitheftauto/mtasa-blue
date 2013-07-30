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
#include "CMaterialLine3DBatcher.h"
#include "CAspectRatioConverter.h"
extern CCore* g_pCore;
extern bool g_bInGTAScene;
extern bool g_bInMTAScene;

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
    m_ActiveBlendMode = EBlendMode::BLEND;
    m_CurDrawMode = EDrawMode::NONE;
    m_CurBlendMode = EBlendMode::BLEND;

    m_pRenderItemManager = new CRenderItemManager ();
    m_pTileBatcher = new CTileBatcher ();
    m_pLine3DBatcherPreGUI = new CLine3DBatcher ( true );
    m_pLine3DBatcherPostGUI = new CLine3DBatcher ( false );
    m_pMaterialLine3DBatcher = new CMaterialLine3DBatcher ();

    m_pScreenGrabber = NewScreenGrabber ();
    m_pPixelsManager = NewPixelsManager ();
    m_LastLostDeviceTimer.SetMaxIncrement( 250 );
    m_pAspectRatioConverter = new CAspectRatioConverter();
}


CGraphics::~CGraphics ( void )
{
    if ( m_pLineInterface )
        m_pLineInterface->Release ();

    DestroyStandardDXFonts ();

    SAFE_RELEASE ( m_ProgressSpinnerTexture );
    SAFE_DELETE ( m_pRenderItemManager );
    SAFE_DELETE ( m_pTileBatcher );
    SAFE_DELETE ( m_pLine3DBatcherPreGUI );
    SAFE_DELETE ( m_pLine3DBatcherPostGUI );
    SAFE_DELETE ( m_pMaterialLine3DBatcher );
    SAFE_DELETE ( m_pScreenGrabber );
    SAFE_DELETE ( m_pPixelsManager );
    SAFE_DELETE ( m_pAspectRatioConverter );
}


void CGraphics::DrawText ( int uiLeft, int uiTop, int uiRight, int uiBottom, unsigned long ulColor, const char* szText, float fScaleX, float fScaleY, unsigned long ulFormat, LPD3DXFONT pDXFont )
{   
    if ( g_pCore->IsWindowMinimized () )
        return;

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
            CheckModes ( EDrawMode::DX_SPRITE, m_ActiveBlendMode );
            m_pDXSprite->SetTransform ( &matrix );  
            
            // Convert to UTF16
            std::wstring strText = MbUTF8ToUTF16(szText);

            pDXFont->DrawTextW ( m_pDXSprite, strText.c_str(), -1, &rect, ulFormat, ulColor );
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
    if ( g_pCore->IsWindowMinimized () )
        return;
      
    BeginDrawBatch ();
    D3DXMATRIX matrix;
    D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
    D3DXVECTOR2 scaling ( fWidth, fHeight );
    D3DXVECTOR2 position ( fX, fY );
    D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, &position );
    CheckModes ( EDrawMode::DX_SPRITE, m_ActiveBlendMode );
    m_pDXSprite->SetTransform ( &matrix );
    m_pDXSprite->Draw ( m_pDXPixelTexture, NULL, NULL, NULL, ulColor );
    EndDrawBatch ();
}


//
// Aspect ratio compensation malarky
// 
void CGraphics::SetAspectRatioAdjustmentEnabled( bool bEnabled, float fSourceRatio )
{
    m_pAspectRatioConverter->SetSourceRatioValue( bEnabled ? fSourceRatio : 0 );
}

bool CGraphics::IsAspectRatioAdjustmentEnabled( void )
{
    return m_pAspectRatioConverter->IsEnabled();
}

float CGraphics::GetAspectRatioAdjustmentSourceRatio( void )
{
    return m_pAspectRatioConverter->GetSourceRatioValue();
}

void CGraphics::SetAspectRatioAdjustmentSuspended( bool bSuspended )
{
    m_pAspectRatioConverter->SetSuspended( bSuspended );
}


float CGraphics::ConvertPositionForAspectRatio( float fY )
{
    return m_pAspectRatioConverter->ConvertPositionForAspectRatio( fY );
}

void CGraphics::ConvertSideForAspectRatio( float* pfY, float* pfHeight )
{
    m_pAspectRatioConverter->ConvertSideForAspectRatio( pfY, pfHeight );
}


//
// Set/get current blend mode
// 
void CGraphics::SetBlendMode ( EBlendModeType blendMode )
{
    m_ActiveBlendMode = blendMode;
}

EBlendModeType CGraphics::GetBlendMode ( void )
{
    return m_ActiveBlendMode;
}

//
// Modify diffuse color if blend mode requires it
// 
SColor CGraphics::ModifyColorForBlendMode ( SColor color, EBlendModeType blendMode )
{
    if ( blendMode == EBlendMode::MODULATE_ADD )
    {
        // Apply modulation to diffuse color also
        color.R = color.R * color.A / 256;
        color.G = color.G * color.A / 256;
        color.B = color.B * color.A / 256;
    }
    return color;
}


//
// SetBlendModeRenderStates
// 
void CGraphics::SetBlendModeRenderStates ( EBlendModeType blendMode )
{
    switch ( blendMode )
    {
        case EBlendMode::BLEND:
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
        break;

        case EBlendMode::MODULATE_ADD:
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
        break;

        case EBlendMode::ADD:
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
        break;

        case EBlendMode::OVERWRITE:
        {
            m_pDevice->SetRenderState ( D3DRS_ZENABLE,          D3DZB_FALSE );
            m_pDevice->SetRenderState ( D3DRS_CULLMODE,         D3DCULL_NONE );
            m_pDevice->SetRenderState ( D3DRS_SHADEMODE,        D3DSHADE_GOURAUD );
            m_pDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
            m_pDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,  FALSE );
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
        break;

        default:
            break;
    }
}


//
// BeginDrawBatch
//
// Batching is automatic, and it is vital to to flush the last set of operations.
// Surrounding drawing calls with BeginDrawBatch() and EndDrawBatch() will ensure everything goes to plan.
//
void CGraphics::BeginDrawBatch ( EBlendModeType xxblendMode )
{
    m_iDrawBatchRefCount++;
}

//
// EndDrawBatch
//
void CGraphics::EndDrawBatch ( void )
{
    if ( --m_iDrawBatchRefCount == 0 )
        CheckModes ( EDrawMode::NONE, EBlendMode::BLEND );
    m_iDrawBatchRefCount = Max ( 0, m_iDrawBatchRefCount );
}


//
// Flush buffers if a change in drawing mode if occurring.
//
void CGraphics::CheckModes ( EDrawModeType newDrawMode, EBlendModeType newBlendMode )
{
    bool bDrawModeChanging = ( m_CurDrawMode != newDrawMode );
    bool bBlendModeChanging = ( m_CurBlendMode != newBlendMode );
    // Draw mode changing?
    if ( bDrawModeChanging || bBlendModeChanging )
    {
        // Flush old
        if ( m_CurDrawMode == EDrawMode::DX_SPRITE )
        {
            m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
            m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
            m_pDXSprite->End ();
        }
        else
        if ( m_CurDrawMode == EDrawMode::DX_LINE )
        {
            m_pLineInterface->End ();
        }
        else
        if ( m_CurDrawMode == EDrawMode::TILE_BATCHER )
        {
            m_pTileBatcher->Flush ();
        }


        // Start new
        if ( newDrawMode == EDrawMode::DX_SPRITE )
        {
            m_pDXSprite->Begin ( D3DXSPRITE_DONOTSAVESTATE | D3DXSPRITE_DONOTMODIFY_RENDERSTATE );
        }
        else
        if ( newDrawMode == EDrawMode::DX_LINE )
        {
            m_pLineInterface->Begin ();
        }
    }

    // Blend mode changing?
    if ( bDrawModeChanging || bBlendModeChanging )
    {
        SetBlendModeRenderStates ( newBlendMode );
    }

    m_CurDrawMode = newDrawMode;
    m_CurBlendMode = newBlendMode;
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


float CGraphics::GetDXTextExtent ( const char * szText, float fScale, LPD3DXFONT pDXFont, bool bColorCoded )
{
    if ( !pDXFont )
        pDXFont = GetFont ();

    pDXFont = MaybeGetBigFont ( pDXFont, fScale, fScale );

    if ( pDXFont )
    {
        HDC dc = pDXFont->GetDC ();
        SIZE size;

        WString strText = MbUTF8ToUTF16(szText);

        if ( bColorCoded )
            RemoveColorCodesInPlaceW( strText );

        GetTextExtentPoint32W ( dc, strText.c_str(), strText.length(), &size );

        return ( ( float ) size.cx * fScale );
    }
    return 0.0f;
}


float CGraphics::GetDXTextExtentW ( const wchar_t* wszText, float fScale, LPD3DXFONT pDXFont )
{
    if ( !pDXFont )
        pDXFont = GetFont ();

    pDXFont = MaybeGetBigFont ( pDXFont, fScale, fScale );

    if ( pDXFont )
    {
        HDC dc = pDXFont->GetDC ();
        SIZE size;

        GetTextExtentPoint32W ( dc, wszText, wcslen ( wszText ), &size );

        return ( ( float ) size.cx * fScale );
    }
    return 0.0f;
}


ID3DXFont * CGraphics::GetFont ( eFontType fontType, float* pfOutScaleUsed, float fRequestedScale, const char* szCustomScaleUser )
{
    if ( pfOutScaleUsed )
        *pfOutScaleUsed = fRequestedScale;

    if ( szCustomScaleUser )
    {
        // Try for a custom scale
        SCustomScaleFontInfo& info = MapGet( m_CustomScaleFontMap, szCustomScaleUser );

        // Already have font?
        if ( info.pFont )
        {
            // Can keep old font?
            if ( fontType == info.fontType && fRequestedScale == info.fScale )
                return info.pFont;

            // Otherwise delete old font
            SAFE_RELEASE( info.pFont );
            info.fScale = 0;
            info.fontType = FONT_DEFAULT;
        }

        // Need to create new font?     
        if ( fRequestedScale != 1 )
        {
            if ( CreateStandardDXFontWithCustomScale( fontType, fRequestedScale, &info.pFont ) )
            {
                info.fScale = fRequestedScale;
                info.fontType = fontType;
                return info.pFont;
            }
        }
    }

    // Didn't / couldn't do custom scale
    if ( pfOutScaleUsed )
        *pfOutScaleUsed = 1;

    if ( fontType < 0 || fontType >= NUM_FONTS )
        return m_pDXFonts [ FONT_DEFAULT ];

    return m_pDXFonts [ fontType ];
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
    fY1 = m_pAspectRatioConverter->ConvertPositionForAspectRatio( fY1 );
    fY2 = m_pAspectRatioConverter->ConvertPositionForAspectRatio( fY2 );

    // Set up a queue item
    sDrawQueueItem Item;
    Item.eType = QUEUE_LINE;
    Item.blendMode = m_ActiveBlendMode;
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

void CGraphics::DrawMaterialLine3DQueued ( const CVector& vecBegin,
                                           const CVector& vecEnd,
                                           float fWidth,
                                           unsigned long ulColor,
                                           CMaterialItem* pMaterial,
                                           float fU, float fV,
                                           float fSizeU, float fSizeV, 
                                           bool bRelativeUV,
                                           bool bUseFaceToward,
                                           const CVector& vecFaceToward )
{
    if ( g_pCore->IsWindowMinimized () )
        return;

    if ( CShaderItem* pShaderItem = DynamicCast < CShaderItem >( pMaterial ) )
    {
        // If material is a shader, use its current instance
        pMaterial = pShaderItem->m_pShaderInstance;
    }

    // Add it to the queue
    m_pMaterialLine3DBatcher->AddLine3D ( vecBegin, vecEnd, fWidth, ulColor, pMaterial, fU, fV, fSizeU, fSizeV, bRelativeUV, bUseFaceToward, vecFaceToward ); 
}



void CGraphics::DrawRectQueued ( float fX, float fY,
                                 float fWidth, float fHeight,
                                 unsigned long ulColor,
                                 bool bPostGUI )
{
    m_pAspectRatioConverter->ConvertSideForAspectRatio( &fY, &fHeight );

    // Set up a queue item
    sDrawQueueItem Item;
    Item.eType = QUEUE_RECT;
    Item.blendMode = m_ActiveBlendMode;
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
    m_pAspectRatioConverter->ConvertSideForAspectRatio( &fY, &fHeight );

    // Set up a queue item
    sDrawQueueItem Item;
    Item.blendMode = m_ActiveBlendMode;
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

        // Use tilebatcher if non-default texture addessing is needed
        if ( pMaterial->m_TextureAddress == TADDRESS_WRAP )
            Item.eType = QUEUE_TEXTURE;
        else
            Item.eType = QUEUE_SHADER;
    }

    // Keep material valid while in the queue
    AddQueueRef ( Item.Texture.pMaterial );

    // Add it to the queue
    AddQueueItem ( Item, bPostGUI );
}


void CGraphics::DrawTextQueued ( float fLeft, float fTop,
                                 float fRight, float fBottom,
                                 unsigned long dwColor,
                                 const char* szText,
                                 float fScaleX,
                                 float fScaleY,
                                 unsigned long ulFormat,
                                 ID3DXFont * pDXFont,
                                 bool bPostGUI,
                                 bool bColorCoded,
                                 bool bSubPixelPositioning )
{
    if ( !szText || !m_pDXSprite )
        return;

    if ( !pDXFont ) pDXFont = GetFont ();

    // We're using a big font to keep it looking nice, so get the actual scale
    pDXFont = MaybeGetBigFont ( pDXFont, fScaleX, fScaleY );

    if ( !pDXFont )
        return;

    fTop = m_pAspectRatioConverter->ConvertPositionForAspectRatio( fTop );
    fBottom = m_pAspectRatioConverter->ConvertPositionForAspectRatio( fBottom );

    if ( !bColorCoded )
    {
        //
        // Simple case without color coding
        //

        if ( fScaleX != 1.0f || fScaleY != 1.0f )
        {
            const float fRcpScaleX = 1 / fScaleX;
            const float fRcpScaleY = 1 / fScaleY;
            fLeft *= fRcpScaleX;
            fTop *= fRcpScaleY;
            fRight *= fRcpScaleX;
            fBottom *= fRcpScaleY;
        }

        if ( !bSubPixelPositioning )
        {
            fLeft = floor ( fLeft );
            fTop = floor ( fTop );
            fRight = floor ( fRight );
            fBottom = floor ( fBottom );
        }

        sDrawQueueItem Item;
        Item.eType = QUEUE_TEXT;
        Item.blendMode = m_ActiveBlendMode;

        Item.Text.fLeft = fLeft;
        Item.Text.fTop = fTop;
        Item.Text.fRight = fRight;
        Item.Text.fBottom = fBottom;
        Item.Text.ulColor = dwColor;
        Item.Text.fScaleX = fScaleX;
        Item.Text.fScaleY = fScaleY;
        Item.Text.ulFormat = ulFormat;
        Item.Text.pDXFont = pDXFont;

        // Convert to wstring        
        Item.wstrText = MbUTF8ToUTF16 ( szText );

        // Keep font valid while in the queue incase it's a custom font
        AddQueueRef ( Item.Text.pDXFont );

        // Add it to the queue
        AddQueueItem ( Item, bPostGUI );
        return;
    }
    else
    {
        //
        // Complex case with color coding
        //

        // Do everything as a wide string from this point
        std::wstring wstrText = MbUTF8ToUTF16 ( szText );   

        // Break into lines
        CSplitStringW splitLines ( wstrText, L"\n" );
        int iNumLines = splitLines.size ();

        float fLineHeight = GetDXFontHeight ( fScaleY, pDXFont );
        float fTotalHeight = iNumLines * fLineHeight;

        // Y position of text
        float fY;
        if ( ulFormat & DT_BOTTOM )
            fY = fBottom - fTotalHeight;
        else
        if ( ulFormat & DT_VCENTER )
            fY = ( fBottom + fTop - fTotalHeight ) * 0.5f;
        else
            fY = fTop;  // DT_TOP

        // Process each line
        SColor currentColor = dwColor;
        for ( uint i = 0 ; i < splitLines.size () ; i++ )
        {
            DrawColorCodedTextLine ( fLeft, fRight, fY, currentColor, splitLines[i], fScaleX, fScaleY, ulFormat, pDXFont, bPostGUI, bSubPixelPositioning );
            fY += fLineHeight;
        }
    }
}


void CGraphics::DrawColorCodedTextLine ( float fLeft, float fRight, float fY, SColor& currentColor, const wchar_t* wszText,
                                         float fScaleX, float fScaleY, unsigned long ulFormat, ID3DXFont* pDXFont, bool bPostGUI, bool bSubPixelPositioning )
{
    struct STextSection
    {
        std::wstring wstrText;
        float fWidth;
        SColor color;
    };

    std::list < STextSection > sectionList;

    // Break line into color sections
    float fTotalWidth = 0;
    const wchar_t* wszSectionPos = wszText;
    do
    {
        unsigned int uiSeekPos = 0;
        const wchar_t* wszSectionStart = wszSectionPos;
        SColor nextColor = currentColor;
        while ( *wszSectionPos != '\0' )      // find end of this section
        {
            if ( IsColorCodeW ( wszSectionPos ) )
            {
                unsigned long ulColor = 0;
                swscanf ( wszSectionPos + 1, L"%06x", &ulColor );
                nextColor = ulColor;
                wszSectionPos += 7;
                break;
            }
            wszSectionPos++;
            uiSeekPos++;
        }

        if ( uiSeekPos > 0 )
        {
            // Add section
            if ( !sectionList.empty () && sectionList.back ().color == currentColor )
            {
                // Append to last section if color has not changed
                std::wstring strExtraText = std::wstring ( wszSectionStart, uiSeekPos );
                float fExtraWidth = GetDXTextExtentW ( strExtraText.c_str (), fScaleX, pDXFont );

                STextSection& section = sectionList.back ();
                section.wstrText += strExtraText;
                section.fWidth += fExtraWidth;
                dassert ( section.color == currentColor );

                fTotalWidth += fExtraWidth;
            }
            else
            {
                // Add a new section
                sectionList.push_back ( STextSection () );
                STextSection& section = sectionList.back ();
                section.wstrText = std::wstring ( wszSectionStart, uiSeekPos );
                section.fWidth = GetDXTextExtentW ( section.wstrText.c_str (), fScaleX, pDXFont );
                section.color = currentColor;

                fTotalWidth += section.fWidth;
            }
        }

        nextColor.A = currentColor.A;
        currentColor = nextColor;
    }
    while ( *wszSectionPos != '\0' );

    // X position of text
    float fX;
    if ( ulFormat & DT_RIGHT )
        fX = fRight - fTotalWidth;
    else
    if ( ulFormat & DT_CENTER )
        fX = ( fRight + fLeft - fTotalWidth ) * 0.5f;
    else
        fX = fLeft;  // DT_LEFT

    // Draw all the color sections
    for ( std::list < STextSection >::const_iterator iter = sectionList.begin () ; iter != sectionList.end () ; ++iter )
    {
        const STextSection& section = *iter;

        float fLeft = fX;
        float fTop = fY;

        if ( fScaleX != 1.0f || fScaleY != 1.0f )
        {
            fLeft /= fScaleX;
            fTop /= fScaleY;
        }

        if ( !bSubPixelPositioning )
        {
            fLeft = floor ( fLeft );
            fTop = floor ( fTop );
        }

        sDrawQueueItem Item;
        Item.eType = QUEUE_TEXT;
        Item.blendMode = m_ActiveBlendMode;

        Item.Text.fLeft = fLeft;
        Item.Text.fTop = fTop;
        Item.Text.fRight = fLeft;
        Item.Text.fBottom = fTop;
        Item.Text.ulColor = section.color;
        Item.Text.fScaleX = fScaleX;
        Item.Text.fScaleY = fScaleY;
        Item.Text.ulFormat = DT_NOCLIP;
        Item.Text.pDXFont = pDXFont;

        Item.wstrText = section.wstrText;

        // Keep font valid while in the queue incase it's a custom font
        AddQueueRef ( Item.Text.pDXFont );

        // Add it to the queue
        AddQueueItem ( Item, bPostGUI );

        fX += section.fWidth;
    }
}


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
            BrowseToSolution ( "mta-datafiles-missing", EXIT_GAME_FIRST | ASK_GO_ONLINE, "Error loading MTA font " + m_FontResourceNames[i] );
        }
    }

    // Create DirectX font and sprite objects
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
        WriteErrorEvent( strMessage );
        BrowseToSolution ( "create-fonts", EXIT_GAME_FIRST | ASK_GO_ONLINE, strMessage );
    }

    if( !SUCCEEDED ( D3DXCreateSprite ( m_pDevice, &m_pDXSprite ) ) )
    {
        BrowseToSolution ( "fonts-create-sprite-fail", EXIT_GAME_FIRST | ASK_GO_ONLINE, "Error calling CGraphics::LoadFonts() D3DXCreateSprite" );
    }

    return true;
}


bool CGraphics::CreateStandardDXFontWithCustomScale ( eFontType fontType, float fScale, ID3DXFont** ppD3DXFont )
{
    if ( fontType < 0 || fontType >= NUMELMS( fontInfos ) )
        return false;

    const sFontInfo& info = fontInfos[ fontType ];

    if( FAILED ( D3DXCreateFont ( m_pDevice, info.uiHeight * fScale, 0, info.uiWeight, 1,
        FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, info.szName,
        ppD3DXFont ) ) )
    {
        return false;
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
        WriteErrorEvent( SString( "Could not create Direct3D font '%s'", strFontName.c_str() ) );
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

    // Release 
    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        SAFE_RELEASE( m_pDXFonts[i] );
        SAFE_RELEASE( m_pBigDXFonts[i] );
    }

    // Release custom scale versions of standard fonts as well
    for ( std::map < SString, SCustomScaleFontInfo >::iterator iter = m_CustomScaleFontMap.begin() ; iter != m_CustomScaleFontMap.end() ; ++iter )
        SAFE_RELEASE( iter->second.pFont );

    return true;
}

void CGraphics::DrawTexture ( CTextureItem* pTexture, float fX, float fY, float fScaleX, float fScaleY, float fRotation, float fCenterX, float fCenterY, DWORD dwColor, float fU, float fV, float fSizeU, float fSizeV, bool bRelativeUV )
{
    if ( g_pCore->IsWindowMinimized () )
        return;

    if ( !pTexture )
        return;

    const float fSurfaceWidth  = pTexture->m_uiSurfaceSizeX;
    const float fSurfaceHeight = pTexture->m_uiSurfaceSizeY;
    const float fFileWidth     = pTexture->m_uiSizeX;
    const float fFileHeight    = pTexture->m_uiSizeY;

    BeginDrawBatch ();
    RECT cutImagePos;
    const float fMultU = ( bRelativeUV ? fSurfaceWidth  : fSurfaceWidth  / fFileWidth );
    const float fMultV = ( bRelativeUV ? fSurfaceHeight : fSurfaceHeight / fFileHeight );
    cutImagePos.left    = ( fU )          * fMultU;
    cutImagePos.right   = ( fU + fSizeU ) * fMultU;
    cutImagePos.top     = ( fV )          * fMultV;
    cutImagePos.bottom  = ( fV + fSizeV ) * fMultV;
    const float fCutWidth  = cutImagePos.right - cutImagePos.left;
    const float fCutHeight = cutImagePos.bottom - cutImagePos.top;

    D3DXMATRIX matrix;
    D3DXVECTOR2 scaling ( fScaleX * fFileWidth / fCutWidth, fScaleY * fFileHeight / fCutHeight );
    D3DXVECTOR2 rotationCenter  ( fFileWidth * fScaleX * fCenterX, fFileHeight * fScaleX * fCenterY );
    D3DXVECTOR2 position ( fX - fFileWidth * fScaleX * fCenterX, fY - fFileHeight * fScaleY * fCenterY );
    D3DXMatrixTransformation2D ( &matrix, NULL, NULL, &scaling, &rotationCenter, fRotation * 6.2832f / 360.f, &position );
    CheckModes ( EDrawMode::DX_SPRITE, m_ActiveBlendMode );
    m_pDXSprite->SetTransform ( &matrix );
    m_pDXSprite->Draw ( (IDirect3DTexture9*)pTexture->m_pD3DTexture, &cutImagePos, NULL, NULL, /*ModifyColorForBlendMode (*/ dwColor/*, blendMode )*/ );
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
    m_pMaterialLine3DBatcher->OnDeviceCreate ( pDevice, GetViewportWidth (), GetViewportHeight () );
    m_pRenderItemManager->OnDeviceCreate ( pDevice, GetViewportWidth (), GetViewportHeight () );
    m_pScreenGrabber->OnDeviceCreate ( pDevice );
    m_pPixelsManager->OnDeviceCreate ( pDevice );
    m_ProgressSpinnerTexture = GetRenderItemManager ()->CreateTexture ( CalcMTASAPath( "MTA\\cgui\\images\\busy_spinner.png" ), NULL, false, -1, -1, RFORMAT_DXT3, TADDRESS_CLAMP );
    m_pAspectRatioConverter->Init( GetViewportHeight () );
}


void CGraphics::OnDeviceInvalidate ( IDirect3DDevice9 * pDevice )
{
    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        if( m_pDXFonts[i] ) m_pDXFonts[i]->OnLostDevice ();
        if( m_pBigDXFonts[i] ) m_pBigDXFonts[i]->OnLostDevice ();
    }

    for ( std::map < SString, SCustomScaleFontInfo >::iterator iter = m_CustomScaleFontMap.begin() ; iter != m_CustomScaleFontMap.end() ; ++iter )
        if ( iter->second.pFont )
            iter->second.pFont->OnLostDevice();

    if ( m_pDXSprite )
        m_pDXSprite->OnLostDevice ();

    if ( m_pLineInterface )
        m_pLineInterface->OnLostDevice ();

    m_pRenderItemManager->OnLostDevice ();
    m_pScreenGrabber->OnLostDevice ();
    SAFE_RELEASE( m_pSavedFrontBufferData );
    SAFE_RELEASE( m_pTempBackBufferData );
}


void CGraphics::OnDeviceRestore ( IDirect3DDevice9 * pDevice )
{
    for ( int i = 0; i < NUM_FONTS; i++ )
    {
        if( m_pDXFonts[i] ) m_pDXFonts[i]->OnResetDevice ();
        if( m_pBigDXFonts[i] ) m_pBigDXFonts[i]->OnResetDevice ();
    }

    for ( std::map < SString, SCustomScaleFontInfo >::iterator iter = m_CustomScaleFontMap.begin() ; iter != m_CustomScaleFontMap.end() ; ++iter )
        if ( iter->second.pFont )
            iter->second.pFont->OnResetDevice();

    if ( m_pDXSprite )
        m_pDXSprite->OnResetDevice ();

    if ( m_pLineInterface )
        m_pLineInterface->OnResetDevice ();

    m_pRenderItemManager->OnResetDevice ();
    m_pScreenGrabber->OnResetDevice ();
}


void CGraphics::OnZBufferModified ( void )
{
    m_pTileBatcher->OnZBufferModified ();
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

void CGraphics::DrawMaterialLine3DQueue ( void )
{
    m_pMaterialLine3DBatcher->Flush ();
}

bool CGraphics::HasMaterialLine3DQueueItems ( void )
{
    return m_pMaterialLine3DBatcher->HasItems ();
}


void CGraphics::DrawQueue ( std::vector < sDrawQueueItem >& Queue )
{
    BeginDrawBatch ();
    // Items to draw?
    if ( Queue.size () > 0 )
    {
        // Loop through it
        std::vector < sDrawQueueItem >::iterator iter = Queue.begin ();
        for ( ; iter != Queue.end (); iter++ )
        {
            // Draw the item
            DrawQueueItem ( *iter );
        }

        // Clear the list
        Queue.clear ();
    }
    EndDrawBatch ();
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
                if ( m_pLineInterface->GetWidth () != Item.Line.fWidth )
                {
                    if ( m_CurDrawMode != EDrawMode::DX_LINE )
                        m_pLineInterface->SetWidth ( Item.Line.fWidth );
                    else
                    {
                        m_pLineInterface->End ();
                        m_pLineInterface->SetWidth ( Item.Line.fWidth );
                        m_pLineInterface->Begin ();
                    }
                }
                CheckModes ( EDrawMode::DX_LINE, Item.blendMode );
                m_pLineInterface->Draw ( List, 2, ModifyColorForBlendMode ( Item.Line.ulColor, Item.blendMode ) );
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
            CheckModes ( EDrawMode::DX_SPRITE, Item.blendMode );
            m_pDXSprite->SetTransform ( &matrix );
            m_pDXSprite->Draw ( m_pDXPixelTexture, NULL, NULL, NULL, /*ModifyColorForBlendMode (*/ Item.Rect.ulColor/*, Item.blendMode )*/ );
            break;
        }
        case QUEUE_TEXT:
        {
            RECT rect;        
            SetRect ( &rect, Item.Text.fLeft, Item.Text.fTop, Item.Text.fRight, Item.Text.fBottom );  
            const float fPosFracX = Item.Text.fLeft - rect.left;
            const float fPosFracY = Item.Text.fTop - rect.top;
            D3DXMATRIX matrix;
            D3DXVECTOR2 scalingCentre ( 0.5f, 0.5f );
            D3DXVECTOR2 scaling ( Item.Text.fScaleX, Item.Text.fScaleY );
            D3DXVECTOR2 translation ( fPosFracX * Item.Text.fScaleX, fPosFracY * Item.Text.fScaleY );   // Sub-pixel positioning
            D3DXMatrixTransformation2D ( &matrix, NULL, 0.0f, &scaling, NULL, 0.0f, &translation );
            CheckModes ( EDrawMode::DX_SPRITE, Item.blendMode );
            m_pDXSprite->SetTransform ( &matrix );        
            Item.Text.pDXFont->DrawTextW ( m_pDXSprite, Item.wstrText.c_str (), -1, &rect, Item.Text.ulFormat, /*ModifyColorForBlendMode (*/ Item.Text.ulColor/*, Item.blendMode )*/ );
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
                CheckModes ( EDrawMode::DX_SPRITE, Item.blendMode );
                m_pDXSprite->SetTransform ( &matrix );
                m_pDXSprite->Draw ( (IDirect3DTexture9*)pTexture->m_pD3DTexture, &cutImagePos, NULL, NULL, /*ModifyColorForBlendMode (*/ Item.Texture.ulColor/*, Item.blendMode )*/ );
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
            CheckModes ( EDrawMode::TILE_BATCHER );
            m_pTileBatcher->AddTile ( t.fX, t.fY, t.fX+t.fWidth, t.fY+t.fHeight, fU1, fV1, fU2, fV2, t.pMaterial, t.fRotation, t.fRotCenOffX, t.fRotCenOffY, t.ulColor );
            RemoveQueueRef ( Item.Texture.pMaterial );
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

// Notification that the render target will be changing
void CGraphics::OnChangingRenderTarget ( uint uiNewViewportSizeX, uint uiNewViewportSizeY )
{
    // Flush dx draws
    DrawPreGUIQueue ();
    // Inform tile batcher
    m_pTileBatcher->OnChangingRenderTarget ( uiNewViewportSizeX, uiNewViewportSizeY );
}


////////////////////////////////////////////////////////////////
//
// CGraphics::EnteringMTARenderZone
//
// Called when entering known areas of MTA rendering activity
//
////////////////////////////////////////////////////////////////
void CGraphics::EnteringMTARenderZone( void )
{
    SaveGTARenderStates();
    m_MTARenderZone = MTA_RZONE_MAIN;
    m_iOutsideZoneCount = 0;
}


////////////////////////////////////////////////////////////////
//
// CGraphics::LeavingMTARenderZone
//
// Called when leaving known areas of MTA rendering activity
//
////////////////////////////////////////////////////////////////
void CGraphics::LeavingMTARenderZone( void )
{
    RestoreGTARenderStates();
    m_MTARenderZone = MTA_RZONE_NONE;
    m_iOutsideZoneCount = 0;
}


////////////////////////////////////////////////////////////////
//
// CGraphics::MaybeEnteringMTARenderZone
//
// Should be called before MTA rendering activity, if there is a chance it will be outside the known areas
//
////////////////////////////////////////////////////////////////
void CGraphics::MaybeEnteringMTARenderZone( void )
{
    if ( m_MTARenderZone == MTA_RZONE_OUTSIDE )
    {
        // Handle stacking if already outside
        m_iOutsideZoneCount++;
    }
    else
    if ( m_MTARenderZone == MTA_RZONE_NONE )
    {
        assert( !m_pSavedStateBlock );
        assert( m_iOutsideZoneCount == 0 );

        // Save states if moving from unknown area
        SaveGTARenderStates();
        m_iOutsideZoneCount = 1;
        m_MTARenderZone = MTA_RZONE_OUTSIDE;
    }
}


////////////////////////////////////////////////////////////////
//
// CGraphics::MaybeLeavingMTARenderZone
//
// Should be called sometime after MaybeEnteringMTARenderZone
//
////////////////////////////////////////////////////////////////
void CGraphics::MaybeLeavingMTARenderZone( void )
{
    if ( m_MTARenderZone == MTA_RZONE_OUTSIDE )
    {
        // Handle unstacking multiple calls
        m_iOutsideZoneCount--;
        if ( m_iOutsideZoneCount == 0 )
        {
            // Do restore on final call
            RestoreGTARenderStates();
            m_MTARenderZone = MTA_RZONE_NONE;
        }
    }
}


////////////////////////////////////////////////////////////////
//
// CGraphics::SaveGTARenderStates
//
// Handle moving into MTA controlled rendering
//
////////////////////////////////////////////////////////////////
void CGraphics::SaveGTARenderStates( void )
{
    SAFE_RELEASE( m_pSavedStateBlock );
    // Create a state block.
    m_pDevice->CreateStateBlock ( D3DSBT_ALL, &m_pSavedStateBlock );

    // Make sure linear sampling is enabled
    m_pDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    m_pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

    // Make sure stencil is off to avoid problems with flame effects
    m_pDevice->SetRenderState ( D3DRS_STENCILENABLE, FALSE );
}


////////////////////////////////////////////////////////////////
//
// CGraphics::RestoreGTARenderStates
//
// Handle moving out of MTA controlled rendering
//
////////////////////////////////////////////////////////////////
void CGraphics::RestoreGTARenderStates( void )
{
    // Restore these transforms to fix various weird stuff
    m_pDevice->SetTransform ( D3DTS_PROJECTION, &g_pDeviceState->TransformState.PROJECTION );
    m_pDevice->SetTransform ( D3DTS_WORLD, &g_pDeviceState->TransformState.WORLD );
    m_pDevice->SetTransform ( D3DTS_VIEW, &g_pDeviceState->TransformState.VIEW );

    // Restore the render states
    if ( m_pSavedStateBlock )
    {
        m_pSavedStateBlock->Apply ( );
        SAFE_RELEASE( m_pSavedStateBlock );
    }
}


////////////////////////////////////////////////////////////////
//
// CGraphics::DidRenderScene
//
// Notify that scene rendering is working
//
////////////////////////////////////////////////////////////////
void CGraphics::DidRenderScene( void )
{
    if ( m_bProgressVisible )
    {
        // Min display time before turning off spinner
        if ( m_FirstDrawnProgressTimer.Get() > DUMMY_PROGRESS_MIN_DISPLAY_TIME )
            m_bProgressVisible = false;
        else
            DrawProgressMessage( false );
    }
    SAFE_RELEASE( m_pSavedFrontBufferData );
    SAFE_RELEASE( m_pTempBackBufferData );
    m_LastRenderedSceneTimer.Reset();
    float fTargetAspectRatioValue = 0;
    if ( CVARS_GET_VALUE < bool > ( "hud_match_aspect_ratio" ) )
        fTargetAspectRatioValue = CCore::GetSingleton ().GetGame ()->GetSettings ()->GetAspectRatioValue();
    m_pAspectRatioConverter->Pulse( fTargetAspectRatioValue );
}


////////////////////////////////////////////////////////////////
//
// CGraphics::SetProgressMessage
//
// Set (and maybe show) status message used when scene rendering is stalled
//
////////////////////////////////////////////////////////////////
void CGraphics::SetProgressMessage( const SString& strMessage )
{
    if ( !m_bProgressVisible )
    {
        if ( m_LastRenderedSceneTimer.Get() < DUMMY_PROGRESS_INITIAL_DELAY )
            return;

        // Enable progress drawing
        m_bProgressVisible = true;
        m_FirstDrawnProgressTimer.Reset();
    }

    if ( g_pCore->IsWindowMinimized() )
        return;

    if ( !m_pRenderItemManager->IsUsingDefaultRenderTarget() )
        return;

    if ( m_LastDrawnProgressTimer.Get() < 100 )
        return;

    m_strProgressMessage = strMessage;
    DrawProgressMessage();

    m_LastDrawnProgressTimer.Reset();
}


////////////////////////////////////////////////////////////////
//
// CGraphics::DrawProgressMessage
//
// Do clever things to draw and show message without changing the backbuffer contents (if required)
//
////////////////////////////////////////////////////////////////
void CGraphics::DrawProgressMessage( bool bPreserveBackbuffer )
{
    // Check d3d device is usable for at least 1 second and 4 calls
    if ( IsIconic( g_pCore->GetHookedWindow() ) || m_pDevice->TestCooperativeLevel() != D3D_OK )
    {
        m_LastLostDeviceTimer.Reset();
        return;
    }

    if ( m_LastLostDeviceTimer.Get() < 1000 )
        return;

    // Must be in a scene
    if ( !g_bInGTAScene && !g_bInMTAScene )
        return;
    const bool bWasInScene = true;
    bool bInScene = true;

    // Check if disabled
    static bool bDisabled = !GetApplicationSetting( "diagnostics", "user-confirmed-bsod-time" ).empty();
    if ( bDisabled )
        return;

    //
    // Save stuff
    //
    if ( m_iDrawBatchRefCount )
        CheckModes( EDrawMode::NONE, EBlendMode::BLEND );
    EBlendModeType savedBlendMode = m_ActiveBlendMode;

    HRESULT hr;
    IDirect3DStateBlock9* pSavedStateBlock = NULL;
    m_pDevice->CreateStateBlock( D3DSBT_ALL, &pSavedStateBlock );

    //
    // Do stuff
    //
    IDirect3DSurface9* pD3DBackBufferSurface = NULL;
    IDirect3DSurface9* pTempFrontBufferData = NULL;
    do
    {
        if ( bPreserveBackbuffer )
        {
            if ( bInScene )
            {
                m_pDevice->EndScene();
                bInScene = false;
            }

            // Get backbuffer surface
            hr = m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pD3DBackBufferSurface );
            if ( FAILED( hr ) )
                break;
            D3DSURFACE_DESC BackBufferDesc;
            hr = pD3DBackBufferSurface->GetDesc( &BackBufferDesc );
            if ( FAILED( hr ) )
                break;

            // Maybe save frontbuffer pixels
            if ( !m_pSavedFrontBufferData )
            {
                D3DDISPLAYMODE displayMode;
                hr = m_pDevice->GetDisplayMode( 0, &displayMode );
                if ( FAILED( hr ) )
                    break;
                hr = m_pDevice->CreateOffscreenPlainSurface( displayMode.Width, displayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTempFrontBufferData, NULL );
                if ( FAILED( hr ) )
                    break;
                hr = m_pDevice->GetFrontBufferData( 0, pTempFrontBufferData );
                if ( FAILED( hr ) )
                    break;

                HWND hwnd = g_pCore->GetHookedWindow();
                RECT clientRect;
                GetClientRect( hwnd, &clientRect );
                MapWindowPoints(hwnd, NULL, (LPPOINT)(&clientRect), (sizeof(clientRect)/sizeof(POINT)) );
                hr = m_pDevice->CreateOffscreenPlainSurface( BackBufferDesc.Width, BackBufferDesc.Height, BackBufferDesc.Format, D3DPOOL_SYSTEMMEM, &m_pSavedFrontBufferData, NULL );
                if ( FAILED( hr ) )
                    break;
                hr = D3DXLoadSurfaceFromSurface( m_pSavedFrontBufferData, NULL, NULL, pTempFrontBufferData, NULL, &clientRect, D3DX_FILTER_NONE, 0 );
                if ( FAILED( hr ) )
                    break;
                SAFE_RELEASE( pTempFrontBufferData );
            }

            // Save backbuffer pixels
            if ( !m_pTempBackBufferData)
                m_pTempBackBufferData = CGraphics::GetSingleton().GetRenderItemManager()->CreateRenderTarget( BackBufferDesc.Width, BackBufferDesc.Height, true, true );
            if ( !m_pTempBackBufferData )
                break;
            hr = m_pDevice->StretchRect( pD3DBackBufferSurface, NULL, m_pTempBackBufferData->m_pD3DRenderTargetSurface, NULL, D3DTEXF_POINT );
            if ( FAILED( hr ) )
                break;

            // Copy saved frontbuffer pixels onto backbuffer surface
            hr = D3DXLoadSurfaceFromSurface( pD3DBackBufferSurface, NULL, NULL, m_pSavedFrontBufferData, NULL, NULL, D3DX_FILTER_NONE, 0 );
            if ( FAILED( hr ) )
                break;

            m_pDevice->BeginScene();
            bInScene = true;
        }

        // Draw progress graphics on backbuffer surface
        {
            m_ActiveBlendMode = EBlendMode::MODULATE_ADD;

            const uint uiViewportHeight = GetViewportHeight();
            const uint uiViewportWidth = GetViewportWidth();

            if ( !m_strProgressMessage.empty() )
            {
                const uint uiMessageWidth = GetDXTextExtent( m_strProgressMessage );
                const uint uiMessagePosX = uiViewportWidth / 2 - uiMessageWidth / 2;
                const DWORD dwMessageColor = 0xA0FFFFFF;
                DrawText( uiMessagePosX, uiViewportHeight - 57, dwMessageColor, 1, "%s", *m_strProgressMessage );
            }

            if ( m_ProgressSpinnerTexture )
            {
                const uint uiNumFrames = 12;
                const uint uiFrameWidth = m_ProgressSpinnerTexture->m_uiSizeX / uiNumFrames;
                const uint uiFrameHeight = m_ProgressSpinnerTexture->m_uiSizeY;
                const uint uiSpinnerPosX = uiViewportWidth / 2 - uiFrameWidth / 2;
                const float fScaleX = 1.f / uiNumFrames;
                const DWORD dwSpinnerColor = 0x90FFFFFF;

                if ( m_ProgressAnimTimer.Get() > DUMMY_PROGRESS_ANIMATION_INTERVAL )
                {
                    m_ProgressAnimTimer.Reset();
                    m_uiProgressAnimFrame = ( m_uiProgressAnimFrame + 1 ) % uiNumFrames;
                }
                DrawTexture( m_ProgressSpinnerTexture, uiSpinnerPosX, uiViewportHeight - 40, fScaleX, 1, 0, 0, 0, dwSpinnerColor, m_uiProgressAnimFrame * uiFrameWidth, 0, uiFrameWidth, uiFrameHeight, false );
            }

            CheckModes( EDrawMode::NONE, EBlendMode::BLEND );
        }

        if ( bPreserveBackbuffer )
        {
            m_pDevice->EndScene();
            bInScene = false;

            // Flip backbuffer onto front buffer
            SAFE_RELEASE( pD3DBackBufferSurface );
            hr = m_pDevice->Present( NULL, NULL, NULL, NULL );
            if ( FAILED( hr ) )
                break;

            // Restore backbuffer surface
            hr = m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pD3DBackBufferSurface );
            if ( FAILED( hr ) )
                break;
            hr = m_pDevice->StretchRect( m_pTempBackBufferData->m_pD3DRenderTargetSurface, NULL, pD3DBackBufferSurface, NULL, D3DTEXF_POINT );

            m_pDevice->BeginScene();
            bInScene = true;
        }
    }
    while( false );

    // Tidy
    SAFE_RELEASE( pTempFrontBufferData );
    SAFE_RELEASE( pD3DBackBufferSurface );

    // Ensure scene status is restored
    if ( bInScene != bWasInScene )
    {
        if ( bWasInScene )
            m_pDevice->BeginScene();
        else
            m_pDevice->EndScene();
        bInScene = bWasInScene;
    }

    //
    // Restore stuff
    //
    if ( pSavedStateBlock )
    {
        pSavedStateBlock->Apply();
        SAFE_RELEASE( pSavedStateBlock );
    }

    m_ActiveBlendMode = savedBlendMode;
}
