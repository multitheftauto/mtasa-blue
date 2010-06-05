/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/d3d/d3dfont.cpp
*  PURPOSE:		Texture-based font class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*  Copyright (c) 1999-2001 Microsoft Corporation. All rights reserved.
*
*****************************************************************************/

#define STRICT
#include <stdio.h>
#include <tchar.h>
#include "include/D3DX8.h"
#include "include/D3DFont.h"
#include "include/D3DUtil.h"
#include "include/DXUtil.h"




//-----------------------------------------------------------------------------
// Custom vertex types for rendering text
//-----------------------------------------------------------------------------
#define MAX_NUM_VERTICES 50*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };
struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

inline FONT3DVERTEX InitFont3DVertex( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
                                      FLOAT tu, FLOAT tv )
{
    FONT3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv;
    return v;
}




//-----------------------------------------------------------------------------
// Name: CD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFont::CD3DFont( TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags )
{
    _tcscpy( m_strFontName, strFontName );
    m_dwFontHeight         = dwHeight;
    m_dwFontFlags          = dwFlags;

    m_pd3dDevice           = NULL;
    m_pTexture             = NULL;
    m_pVB                  = NULL;

    m_dwSavedStateBlock    = 0L;
    m_dwDrawTextStateBlock = 0L;
    m_dwDrawBoxStateBlock  = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice )
{
    HRESULT hr;

    // Keep a local copy of the device
    m_pd3dDevice = pd3dDevice;

    // Establish the font and texture size
    m_fTextScale  = 1.0f; // Draw fonts into texture without scaling

    // Large fonts need larger textures
    if( m_dwFontHeight > 40 )
        m_dwTexWidth = m_dwTexHeight = 1024;
    else if( m_dwFontHeight > 20 )
        m_dwTexWidth = m_dwTexHeight = 512;
    else
        m_dwTexWidth  = m_dwTexHeight = 256;

    // If requested texture is too big, use a smaller texture and smaller font,
    // and scale up when rendering.
    D3DCAPS8 d3dCaps;
    m_pd3dDevice->GetDeviceCaps( &d3dCaps );

    if( m_dwTexWidth > d3dCaps.MaxTextureWidth )
    {
        m_fTextScale = (FLOAT)d3dCaps.MaxTextureWidth / (FLOAT)m_dwTexWidth;
        m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
    }

    // Create a new texture for the font
    hr = m_pd3dDevice->CreateTexture( m_dwTexWidth, m_dwTexHeight, 1,
                                      0, D3DFMT_A4R4G4B4,
                                      D3DPOOL_MANAGED, &m_pTexture );
    if( FAILED(hr) )
        return hr;

    // Prepare to create a bitmap
    DWORD*      pBitmapBits;
    BITMAPINFO bmi;
    ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  (int)m_dwTexWidth;
    bmi.bmiHeader.biHeight      = -(int)m_dwTexHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    // Create a DC and a bitmap for the font
    HDC     hDC       = CreateCompatibleDC( NULL );
    HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
                                          (VOID**)&pBitmapBits, NULL, 0 );
    SetMapMode( hDC, MM_TEXT );

    // Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
    // antialiased font, but this is not guaranteed.
    INT nHeight    = -MulDiv( m_dwFontHeight, 
        (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fTextScale), 72 );
    DWORD dwBold   = (m_dwFontFlags&D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
    DWORD dwItalic = (m_dwFontFlags&D3DFONT_ITALIC) ? TRUE    : FALSE;
    HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH, m_strFontName );
    if( NULL==hFont )
        return E_FAIL;

    SelectObject( hDC, hbmBitmap );
    SelectObject( hDC, hFont );

    // Set text properties
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor(   hDC, 0x00000000 );
    SetTextAlign( hDC, TA_TOP );

    // Loop through all printable character and output them to the bitmap..
    // Meanwhile, keep track of the corresponding tex coords for each character.
    DWORD x = 0;
    DWORD y = 0;
    TCHAR str[2] = _T("x");
    SIZE size;

    for( TCHAR c=32; c<127; c++ )
    {
        str[0] = c;
        GetTextExtentPoint32( hDC, str, 1, &size );
        if( (DWORD)(x+size.cx+1) > m_dwTexWidth )
        {
            x  = 0;
            y += size.cy+1;
        }

        ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, 1, NULL );

        m_fTexCoords[c-32][0] = ((FLOAT)(x+0))/m_dwTexWidth;
        m_fTexCoords[c-32][1] = ((FLOAT)(y+0))/m_dwTexHeight;
        m_fTexCoords[c-32][2] = ((FLOAT)(x+0+size.cx))/m_dwTexWidth;
        m_fTexCoords[c-32][3] = ((FLOAT)(y+0+size.cy))/m_dwTexHeight;

        x += size.cx+1;
    }

    // Lock the surface and write the alpha values for the set pixels
    D3DLOCKED_RECT d3dlr;
    m_pTexture->LockRect( 0, &d3dlr, 0, 0 );
    BYTE* pDstRow = (BYTE*)d3dlr.pBits;
    WORD* pDst16;
    BYTE bAlpha; // 4-bit measure of pixel intensity

    for( y=0; y < m_dwTexHeight; y++ )
    {
        pDst16 = (WORD*)pDstRow;
        for( x=0; x < m_dwTexWidth; x++ )
        {
            bAlpha = (BYTE)((pBitmapBits[m_dwTexWidth*y + x] & 0xff) >> 4);
            if (bAlpha > 0)
            {
                *pDst16++ = (bAlpha << 12) | 0x0fff;
            }
            else
            {
                *pDst16++ = 0x0000;
            }
        }
        pDstRow += d3dlr.Pitch;
    }

    // Done updating texture, so clean up used objects
    m_pTexture->UnlockRect(0);
    DeleteObject( hbmBitmap );
    DeleteDC( hDC );
    DeleteObject( hFont );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DFont::RestoreDeviceObjects()
{
    HRESULT hr;

    // Create vertex buffer for the letters
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( MAX_NUM_VERTICES*sizeof(FONT2DVERTEX),
                                                       D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,                                                      D3DPOOL_DEFAULT, &m_pVB ) ) )
    {
        return hr;
    }

    m_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_dwSavedStateBlock);
    m_pd3dDevice->CaptureStateBlock(m_dwSavedStateBlock);

    // Create the state blocks for rendering text
    {
            m_pd3dDevice->BeginStateBlock();
            m_pd3dDevice->SetTexture( 0, m_pTexture );

            if ( D3DFONT_ZENABLE & m_dwFontFlags )
                m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
            else
                m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
            m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
            m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
            m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
            m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
 
            m_pd3dDevice->EndStateBlock( &m_dwDrawTextStateBlock );
            m_pd3dDevice->BeginStateBlock();

			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
            m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
            m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
            m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
            m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
            m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
            m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

            m_pd3dDevice->EndStateBlock( &m_dwDrawBoxStateBlock );
    }
    m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );

    // Delete the state blocks
    if( m_pd3dDevice )
    {
        if( m_dwSavedStateBlock )
            m_pd3dDevice->DeleteStateBlock( m_dwSavedStateBlock );
        if( m_dwDrawTextStateBlock )
            m_pd3dDevice->DeleteStateBlock( m_dwDrawTextStateBlock );
        if( m_dwDrawBoxStateBlock )
            m_pd3dDevice->DeleteStateBlock( m_dwDrawBoxStateBlock );
    }

    m_dwSavedStateBlock    = 0L;
    m_dwDrawTextStateBlock = 0L;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DeleteDeviceObjects()
{
    SAFE_RELEASE( m_pTexture );
    m_pd3dDevice = NULL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextExtent( const TCHAR* strText, SIZE* pSize, FLOAT fMaxWidth )
{
    if( NULL==strText || NULL==pSize )
        return E_FAIL;

    FLOAT fRowWidth  = 0.0f;
    FLOAT fRowHeight = (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight / m_fTextScale;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = fRowHeight;

	int tabs = 1;
    while( *strText )
    {
        TCHAR c = *strText++;

		if( c == _T('\01') ) {
			strText+=4;
			continue;
		}
		
		if( c == _T('\02') ) continue;

        if( c == _T('\n') )
        {
            fRowWidth = 0.0f;
            fHeight  += fRowHeight;
			tabs=1;
        }
		if( c == _T('\t') )
		{
			fRowWidth = tabs*50.0f - fRowWidth;
			tabs++;
			continue;
		}
        if( c < _T(' ') )
            continue;
		
	/*	if (fMaxWidth != 0.0f)
		{
			if (fRowWidth > fMaxWidth) {
				fRowWidth = 0.0f;
				fHeight  += fRowHeight;
				tabs=1;
			}
		}*/

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT tx2 = m_fTexCoords[c-32][2];

        fRowWidth += (tx2-tx1) *  m_dwTexWidth / m_fTextScale;


        if( fRowWidth > fWidth )
            fWidth = fRowWidth;
    }

    pSize->cx = (int)fWidth;
	pSize->cy = (int)fHeight;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetTextMax()
// Desc: Get the pointer to the substring in strText that fits in fMaxWidth
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextMax( const TCHAR* strText, const TCHAR** strResult, FLOAT fMaxWidth )
{
    if( NULL==strText || NULL==strResult || fMaxWidth < 1.0f )
        return E_FAIL;

    FLOAT fRowWidth  = 0.0f;
    FLOAT fRowHeight = (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight / m_fTextScale;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = fRowHeight;
	*strResult = strText + _tcslen(strText)-1;

	int tabs = 1;
    while( *strResult != strText )
    {
        TCHAR c = **strResult;
		*strResult -= 1;

		if( c == _T('\01') ) {
			*strResult-=4;
			continue;
		}
		
		if( c == _T('\02') ) continue;

        if( c == _T('\n') )
        {
            fRowWidth = 0.0f;
            fHeight  += fRowHeight;
			tabs=1;
        }
		if( c == _T('\t') )
		{
			fRowWidth = tabs*50.0f - fRowWidth;
			tabs++;
			continue;
		}
        if( c < _T(' ') )
            continue;
		
        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT tx2 = m_fTexCoords[c-32][2];


        if ((fRowWidth += (tx2-tx1) *  m_dwTexWidth / m_fTextScale) >= fMaxWidth-20.0f) break;


        if( fRowWidth > fWidth )
            fWidth = fRowWidth;
    }

    return S_OK;
}

HRESULT CD3DFont::FormatText( const TCHAR* theText, TCHAR* outText, float fMaxWidth )
{
	FLOAT fCurrentWidth = 0.0f;
	DWORD dwOutputIndex = 0;

	// Make sure we have valid parameters.
	if ( theText == NULL || outText == NULL )
	{
		return E_FAIL;
	}

	while ( *theText )
	{
		TCHAR c = *theText++;

		// If the amount of characters we've emitted into 
		// outText is > dwBuffLen, return an error.
		//if ( dwOutputIndex >= dwBuffLen )
		//{
		//	return E_OUTOFMEMORY;
		//}

		// Check for newline case.
		// If we find a newline, reset current width and continue.
		if ( c == _T('\n') )
		{
			fCurrentWidth = 0.0f;
			outText[dwOutputIndex++] = c;
			continue;
		}

		FLOAT tx1 = m_fTexCoords[c-32][0];
		FLOAT tx2 = m_fTexCoords[c-32][2];
		
		
		// Add the character to the width if it isn't a space.
		if ( c != _T(' ') )
		{
			fCurrentWidth += (tx2-tx1) *  m_dwTexWidth / m_fTextScale;
		}

		// If we've gone over our limit, emit a newline,
		// increment the output index, reset, and continue.
		if ( fCurrentWidth >= fMaxWidth )
		{
			outText[dwOutputIndex++] = '\n';
			fCurrentWidth = 0.0f;
			continue;
		}

		// Copy the character into the output buffer.
		//if ( dwOutputIndex < dwBuffLen )
		//{
			outText[dwOutputIndex++] = c;
		//}

		// If we reach a space, we have to look ahead to see
		// if the word will fit in our string.  If not, we either
		// have to put the word on the next line, or chop the word
		// at some point.   MTA Blue v1.0 BETA BUILD
		if ( c == _T(' ') )
		{
			FLOAT fWordWidth = 0.0f;
			const TCHAR *theWord = theText;

			while ( *theWord )
			{
				TCHAR c = *theWord++;
				
				FLOAT tx1 = m_fTexCoords[c-32][0];
				FLOAT tx2 = m_fTexCoords[c-32][2];
				
				fWordWidth += (tx2-tx1) *  m_dwTexWidth / m_fTextScale;
				
				// Check if word length is over limit.  If it is,
				// we emit a newline, reset, and break out of the
				// word loop.
				if ( (fWordWidth+fCurrentWidth) >= fMaxWidth )
				{
					outText[dwOutputIndex++] = '\n';
					fCurrentWidth = 0.0f;
					break;
				}

				// We've found the end of the word, and it
				// fits in, so lets copy it into our output
				// buffer.
				if ( c == _T(' ') || c == _T('\n') )
				{
					DWORD dwWordLen = theWord-theText-1;

					outText[dwOutputIndex] = NULL;
					_tcsncat( outText, theText, dwWordLen );
					dwOutputIndex += dwWordLen;
					theText += dwWordLen;
					
					if ( c == _T(' ') )
					{
						fCurrentWidth += fWordWidth;
					}
					else
					{
						fCurrentWidth = 0.0f;
					}
					
					break;
				}
			}
		}
	}
	
	// Put a NULL terminator on our output string.
	outText[dwOutputIndex] = NULL;
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawTextScaled()
// Desc: Draws scaled 2D text.  Note that x and y are in viewport coordinates
//       (ranging from -1 to +1).  fXScale and fYScale are the size fraction 
//       relative to the entire viewport.  For example, a fXScale of 0.25 is
//       1/8th of the screen width.  This allows you to output text at a fixed
//       fraction of the viewport, even if the screen or window size changes.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawTextScaled( FLOAT x, FLOAT y, FLOAT z,
                                  FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                                  TCHAR* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

    // Set up renderstate
    m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
    m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );
    m_pd3dDevice->SetVertexShader( D3DFVF_FONT2DVERTEX );
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    }

    D3DVIEWPORT8 vp;
    m_pd3dDevice->GetViewport( &vp );
    FLOAT sx  = (x+1.0f)*vp.Width/2;
    FLOAT sy  = (y-1.0f)*vp.Height/2;
    FLOAT sz  = z;
    FLOAT rhw = 1.0f;
    FLOAT fStartX = sx;

    FLOAT fLineHeight = ( m_fTexCoords[0][3] - m_fTexCoords[0][1] ) * m_dwTexHeight;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );

    while( *strText )
    {
        TCHAR c = *strText++;

        if( c == _T('\n') )
        {
            sx  = fStartX;
            sy += fYScale*vp.Height;
        }
        if( c < _T(' ') )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1)*m_dwTexWidth;
        FLOAT h = (ty2-ty1)*m_dwTexHeight;

        w *= (fXScale*vp.Height)/fLineHeight;
        h *= (fYScale*vp.Height)/fLineHeight;

        if( c != _T(' ') )
        {
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,sz,rhw), dwColor, tx1, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,sz,rhw), dwColor, tx1, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,sz,rhw), dwColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,sz,rhw), dwColor, tx2, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,sz,rhw), dwColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,sz,rhw), dwColor, tx1, ty1 );
            dwNumTriangles += 2;

            if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
            {
                // Unlock, render, and relock the vertex buffer
                m_pVB->Unlock();
                m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
                m_pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );
                dwNumTriangles = 0L;
            }
        }

        sx += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // Restore the modified renderstates
    m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );

    return S_OK;
}


HRESULT CD3DFont::DrawTextBox(float xTop, float yTop, float pad, DWORD dwColorBox, DWORD dwColorTxt, const TCHAR* strText, DWORD dwFlags)
{
    // Setup renderstate
    char *vb_verts = 0;
    SIZE sz;
    
    GetTextExtent(strText,&sz);
    my_vertex square_verts[] ={
        { xTop-pad,  yTop+sz.cy+pad, 0.5f, 1.0f, dwColorBox }, // x, y, z, rhw, color
        { xTop-pad,  yTop-pad,  0.5f, 1.0f, dwColorBox },
        { xTop+sz.cx+pad,  yTop+sz.cy+pad, 0.5f, 1.0f, dwColorBox },
        { xTop+sz.cx+pad,  yTop-pad,  0.5f, 1.0f, dwColorBox }
    };
    m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
    m_pd3dDevice->ApplyStateBlock( m_dwDrawBoxStateBlock );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(my_vertex) );

    m_pVB->Lock( 0, 0, (BYTE**)&vb_verts, 0  );
    memcpy(vb_verts,square_verts,sizeof(square_verts));
    m_pVB->Unlock();
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );
    DrawText(xTop,yTop,dwColorTxt,strText,dwFlags);
    return 0;
}

HRESULT CD3DFont::DrawBox(float xTop, float yTop, float fWidth, float fHeight, float pad, DWORD dwColorBox)
{
    // Setup renderstate
    char *vb_verts = 0;
    
    xTop -= fWidth/2;
    my_vertex square_verts[] ={
        { xTop-pad,  yTop+fHeight+pad, 0.5f, 1.0f, dwColorBox }, // x, y, z, rhw, color
        { xTop-pad,  yTop,  0.5f, 1.0f, dwColorBox },
        { xTop+fWidth+pad,  yTop+fHeight+pad, 0.5f, 1.0f, dwColorBox },
        { xTop+fWidth+pad,  yTop,  0.5f, 1.0f, dwColorBox }
    };
    m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
    m_pd3dDevice->ApplyStateBlock( m_dwDrawBoxStateBlock );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(my_vertex) );

    m_pVB->Lock( 0, 0, (BYTE**)&vb_verts, 0  );
    memcpy(vb_verts,square_verts,sizeof(square_verts));
    m_pVB->Unlock();
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );
    return 0;
}

BOOL CD3DFont::BeginDrawing ()
{
	if ( m_pVB == NULL || m_pd3dDevice == NULL)
	{
		return FALSE;
	}

	///////////////////////////////////////////////////
	// Setup the rendering.
	m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
	m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );
	m_pd3dDevice->SetVertexShader( D3DFVF_FONT2DVERTEX );
	m_pd3dDevice->SetPixelShader( NULL );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FONT2DVERTEX) );

 /*   if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    }
	return TRUE;*/
	return TRUE;
}

BOOL CD3DFont::EndDrawing ()
{
//	m_pDevice->SetTexture( 0, NULL );

	////////////////////////////////////////////////////
	// Restore old states and return.
	m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );
	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws 2D text
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawText( FLOAT sx, FLOAT sy, DWORD dwColor,
                            const TCHAR* strText, DWORD dwFlags )
{
    DWORD dwSColor = dwColor;
    if( m_pd3dDevice == NULL )
        return E_FAIL;
    
    // Setup renderstate
   // m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
   // m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );
 //   m_pd3dDevice->SetVertexShader( D3DFVF_FONT2DVERTEX );
 //   m_pd3dDevice->SetPixelShader( NULL );
  //  m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    }

    FLOAT fStartX = sx;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices = NULL;
    DWORD         dwNumTriangles = 0;
    m_pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );
	int tabs = 1;

    while( *strText )
    {
        TCHAR c = *strText++;
        if( c == _T('\01'))
        {
            dwSColor = ((DWORD)strText[0]<<24)|((DWORD)strText[1]<<16)|((DWORD)strText[2]<<8)|((DWORD)strText[3]);
            strText+=4;
            continue;
        }
		if( c == _T('\t') )
		{
			sx = fStartX + tabs*50.0f;
			tabs++;
			continue;
		}

        if( c == _T('\n') )
        {
			sx = fStartX;
			sy += (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight;
			tabs = 1;
        }

        if( c < _T(' ') )
			continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1) *  m_dwTexWidth / m_fTextScale;
        FLOAT h = (ty2-ty1) * m_dwTexHeight / m_fTextScale;

        if( c != _T(' ') )
        {
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwSColor, tx1, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwSColor, tx1, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwSColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwSColor, tx2, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwSColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwSColor, tx1, ty1 );
            dwNumTriangles += 2;

            if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
            {
                // Unlock, render, and relock the vertex buffer
                m_pVB->Unlock();
                m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
                pVertices = NULL;
                m_pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );
                dwNumTriangles = 0L;
            }
        }

        sx += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // Restore the modified renderstates
   // m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render3DText()
// Desc: Renders 3D text
//-----------------------------------------------------------------------------
HRESULT CD3DFont::Render3DText( TCHAR* strText, DWORD dwFlags, float r, float g, float b, float a )
{
    D3DMATERIAL8 mtrl;
    if( m_pd3dDevice == NULL )
        return E_FAIL;

    // Setup renderstate
    m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
    m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );
    m_pd3dDevice->SetVertexShader( D3DFVF_FONT3DVERTEX );
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FONT3DVERTEX) );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

    D3DUtil_InitMaterial( mtrl, r, g, b, a );
    m_pd3dDevice->SetMaterial(&mtrl);

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    }

    // Position for each text element
    FLOAT x = 0.0f;
    FLOAT y = 0.0f;

    // Center the text block at the origin
    if( dwFlags & D3DFONT_CENTERED )
    {
        SIZE sz;
        GetTextExtent( strText, &sz );
        x = -(((FLOAT)sz.cx)/10.0f)/2.0f;
        y = -(((FLOAT)sz.cy)/10.0f)/2.0f;
    }

    // Turn off culling for two-sided text
    if( dwFlags & D3DFONT_TWOSIDED )
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    FLOAT fStartX = x;
    TCHAR c;

    // Fill vertex buffer
    FONT3DVERTEX* pVertices;
    DWORD         dwVertex       = 0L;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );

    while( c = *strText++ )
    {
        if( c == '\n' )
        {
            x = fStartX;
            y -= (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight/10.0f;
        }

        if( c == _T('\01'))
        {
            // Unlock, render, and relock the vertex buffer
            m_pVB->Unlock();
            m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
            m_pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );
            dwNumTriangles = 0L;

			float g1 = (float)((int)(0xFF&*strText++) / 255.f);
			float b1 = (float)((int)(0xFF&*strText++) / 255.f);
			float r1 = (float)((int)(0xFF&*strText++) / 255.f);
			float a1 = (float)((int)(0xFF&*strText++) / 255.f);
			
			D3DUtil_InitMaterial( mtrl, r1,b1,g1,a1 );/*((float)(*strText++))/255.0f, ((float)(*strText++))/255.0f, 
									//	((float)(*strText++))/255.0f, ((float)(*strText++))/255.0f );*/
			m_pd3dDevice->SetMaterial(&mtrl);
			continue;
        }

        if( c < 32 )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1) * m_dwTexWidth  / ( 10.0f * m_fTextScale );
        FLOAT h = (ty2-ty1) * m_dwTexHeight / ( 10.0f * m_fTextScale );

        if( c != _T(' ') )
        {
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+0,0), D3DXVECTOR3(0,0,-1), tx1, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), tx1, ty1 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), tx2, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+h,0), D3DXVECTOR3(0,0,-1), tx2, ty1 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), tx2, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), tx1, ty1 );
            dwNumTriangles += 2;

            if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
            {
                // Unlock, render, and relock the vertex buffer
                m_pVB->Unlock();
                m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
                m_pVB->Lock( 0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD );
                dwNumTriangles = 0L;
            }
        }

        x += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // Restore the modified renderstates
    m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );

    return S_OK;
}




