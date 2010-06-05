/*****************************************************************************
*
*  PROJECT:		DirectX Video-to-texture renderer
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		texturerenderer/CTextureRenderer.cpp
*  PURPOSE:		Direct3D video renderer base filter implementation
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*
*  Based on the PSDK Texture3D9 code
*  Copyright (c) Microsoft Corporation.  All rights reserved.
*
*****************************************************************************/

#include "CTextureRenderer.h"

#define Lock()		EnterCriticalSection ( m_pLock )
#define Unlock()	LeaveCriticalSection ( m_pLock )

extern IBaseFilter * CreateTextureRenderer ( HRESULT *phr, IDirect3DDevice9 *pDev, IVideoRenderer *pMaster )
{
	return new CTextureRenderer ( phr, pDev, pMaster );
}

//-----------------------------------------------------------------------------
// CTextureRenderer constructor
//-----------------------------------------------------------------------------
CTextureRenderer::CTextureRenderer( HRESULT *phr, IDirect3DDevice9 * pDevice, IVideoRenderer *pMaster )
                                  : CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer),
                                    NAME("Texture Renderer"), NULL, phr),
                                    m_bUseDynamicTextures(TRUE),
									m_pTexture(NULL)
{
	m_pDevice = pDevice;
	m_pMaster = pMaster;

	// Get the mutex, for performance reasons
	m_pLock = (LPCRITICAL_SECTION)(m_pMaster->GetMutex ());
	m_Format = GUID_NULL;

    // Store and AddRef the texture for our use.
    ASSERT(phr);
    if (phr)
        *phr = S_OK;
}


//-----------------------------------------------------------------------------
// CTextureRenderer destructor
//-----------------------------------------------------------------------------
CTextureRenderer::~CTextureRenderer()
{
    if( m_pTexture ) m_pTexture->Release();
	m_pTexture = m_pTexture;
}


//-----------------------------------------------------------------------------
// CheckMediaType: This method forces the graph to give us an R8G8B8 video
// type, making our copy to texture memory trivial.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::CheckMediaType(const CMediaType *pmt)
{
    HRESULT   hr = E_FAIL;

    CheckPointer(pmt,E_POINTER);

	const GUID guidType = *pmt->Type ();
	const GUID guidSubtype = *pmt->Subtype ();
	const GUID guidFormatType = *pmt->FormatType ();

	// Reject the connection if this is not a video type
	m_Format = *pmt->FormatType();
    if( m_Format == FORMAT_VideoInfo ) {
		VIDEOINFO *pvi;

		// Only accept RGB24 video
		pvi = (VIDEOINFO *)pmt->Format();

		if(IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video)  &&
		IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24))
		{
			hr = S_OK;
		}    
	} else if ( m_Format == FORMAT_VideoInfo2 ) {
		VIDEOINFOHEADER2 *pvi;

		// Only accept RGB24 video
		pvi = (VIDEOINFOHEADER2 *)pmt->Format();

		if(IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video)  &&
		IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24))
		{
			hr = S_OK;
		}    
	} else {
		return E_INVALIDARG;
	}

    return hr;
}

//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::SetMediaType(const CMediaType *pmt)
{
    HRESULT hr;

	Lock ();

    UINT uintWidth = 2;
    UINT uintHeight = 2;

	D3DCAPS9 caps;

	// Retrive the size of this media type
    if( m_Format == FORMAT_VideoInfo ) {
		VIDEOINFO *pviBmp;                      // Bitmap info header
		pviBmp = (VIDEOINFO *)pmt->Format();

		m_lVidWidth  = pviBmp->bmiHeader.biWidth;
		m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
		m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24
	} else if ( m_Format == FORMAT_VideoInfo2 ) {
		VIDEOINFOHEADER2 *pviBmp;                      // Bitmap info header
		pviBmp = (VIDEOINFOHEADER2 *)pmt->Format();

		m_lVidWidth  = pviBmp->bmiHeader.biWidth;
		m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
		m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24
	}

    // here let's check if we can use dynamic textures
    ZeroMemory( &caps, sizeof(D3DCAPS9));
    hr = m_pDevice->GetDeviceCaps( &caps );
    if( caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES )
    {
        m_bUseDynamicTextures = TRUE;
    }

    if( caps.TextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        while( (LONG)uintWidth < m_lVidWidth )
        {
            uintWidth = uintWidth << 1;
        }
        while( (LONG)uintHeight < m_lVidHeight )
        {
            uintHeight = uintHeight << 1;
        }
        //UpgradeGeometry( m_lVidWidth, uintWidth, m_lVidHeight, uintHeight);
    }
    else
    {
        uintWidth = m_lVidWidth;
        uintHeight = m_lVidHeight;
    }

    // Create the texture that maps to this media type
    hr = m_pDevice->CreateTexture( uintWidth, uintHeight, 1, 0, D3DFMT_X8R8G8B8,D3DPOOL_MANAGED, &m_pTexture, NULL );
	m_pMaster->SetVideoTexture( m_pTexture );
	m_pMaster->SetVideoFormat( m_lVidWidth, m_lVidHeight );

    if( FAILED(hr))
    {
		Unlock ();
        return hr;
    }

    // CreateTexture can silently change the parameters on us
    D3DSURFACE_DESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));

    if ( FAILED( hr = m_pTexture->GetLevelDesc( 0, &ddsd ) ) ) {
		Unlock ();
        return hr;
    }

    IDirect3DSurface9 * pSurf;

    if (SUCCEEDED(hr = m_pTexture->GetSurfaceLevel(0, &pSurf)))
        pSurf->GetDesc(&ddsd);
	
	pSurf->Release();

    // Save format info
    m_TextureFormat = ddsd.Format;

    if (m_TextureFormat != D3DFMT_X8R8G8B8 &&
        m_TextureFormat != D3DFMT_A1R5G5B5) {
		Unlock ();
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

	Unlock ();
    return S_OK;
}


//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT CTextureRenderer::DoRenderSample( IMediaSample * pSample )
{
    BYTE  *pBmpBuffer, *pTxtBuffer; // Bitmap buffer, texture buffer
    LONG  lTxtPitch;                // Pitch of bitmap, texture

    BYTE  * pbS = NULL;
    DWORD * pdwS = NULL;
    DWORD * pdwD = NULL;
    UINT row, col, dwordWidth;

	Lock ();

    CheckPointer(pSample,E_POINTER);
    CheckPointer(m_pTexture,E_UNEXPECTED);

    // Get the video bitmap buffer
    pSample->GetPointer( &pBmpBuffer );

    // Lock the Texture
    D3DLOCKED_RECT d3dlr;
    if( m_bUseDynamicTextures )
    {
		if( FAILED(m_pTexture->LockRect(0, &d3dlr, 0, D3DLOCK_DISCARD))) {
			Unlock ();
            return E_FAIL;
		}
    }
    else
    {
		if (FAILED(m_pTexture->LockRect(0, &d3dlr, 0, 0))) {
			Unlock ();
            return E_FAIL;
		}
    }
    // Get the texture buffer & pitch
    lTxtPitch = d3dlr.Pitch;
	pTxtBuffer = static_cast<byte *>(d3dlr.pBits);

    // Copy the bits

    if (m_TextureFormat == D3DFMT_X8R8G8B8)
    {
		// Flip the picture
	    //pTxtBuffer += lTxtPitch * m_lVidHeight - 4;

        // Instead of copying data bytewise, we use DWORD alignment here.
        // We also unroll loop by copying 4 pixels at once.
        //
        // original BYTE array is [b0][g0][r0][b1][g1][r1][b2][g2][r2][b3][g3][r3]
        //
        // aligned DWORD array is     [b1 r0 g0 b0][g2 b2 r1 g1][r3 g3 b3 r2]
        //
        // We want to transform it to [ff r0 g0 b0][ff r1 g1 b1][ff r2 g2 b2][ff r3 b3 g3]
        // below, bitwise operations do exactly this.

        dwordWidth = m_lVidWidth / 4; // aligned width of the row, in DWORDS
                                      // (pixel by 3 bytes over sizeof(DWORD))

        for( row = 0; row< (UINT)m_lVidHeight; row++)
        {
            pdwS = ( DWORD*)pBmpBuffer;
            pdwD = ( DWORD*)pTxtBuffer;

            for( col = 0; col < dwordWidth; col ++ )
            {
                pdwD[0] =  pdwS[0] | 0xFF000000;
                pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
                pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
                pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
                pdwD +=4;
                pdwS +=3;
            }

            // we might have remaining (misaligned) bytes here
            pbS = (BYTE*) pdwS;
            for( col = 0; col < (UINT)m_lVidWidth % 4; col++)
            {
                *pdwD = 0xFF000000     |
                        (pbS[2] << 16) |
                        (pbS[1] <<  8) |
                        (pbS[0]);
                pdwD++;
                pbS += 3;
            }

            pBmpBuffer  += m_lVidPitch;
            pTxtBuffer += lTxtPitch;
        }// for rows
    }

    if (m_TextureFormat == D3DFMT_A1R5G5B5)
    {
        for(int y = 0; y < m_lVidHeight; y++ )
        {
            BYTE *pBmpBufferOld = pBmpBuffer;
            BYTE *pTxtBufferOld = pTxtBuffer;

            for (int x = 0; x < m_lVidWidth; x++)
            {
                *(WORD *)pTxtBuffer = (WORD)
                    (0x8000 +
                    ((pBmpBuffer[2] & 0xF8) << 7) +
                    ((pBmpBuffer[1] & 0xF8) << 2) +
                    (pBmpBuffer[0] >> 3));

                pTxtBuffer += 2;
                pBmpBuffer += 3;
            }

            pBmpBuffer = pBmpBufferOld + m_lVidPitch;
            pTxtBuffer = pTxtBufferOld + lTxtPitch;
        }
    }

    // Unlock the Texture
	if (FAILED(m_pTexture->UnlockRect(0))) {
		Unlock ();
        return E_FAIL;
	}

	Unlock ();
    return S_OK;
}

void CTextureRenderer::OnLostDevice ()
{
}

void CTextureRenderer::OnResetDevice ()
{
}
