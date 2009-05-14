/*****************************************************************************
*
*  PROJECT:		DirectX Video-to-texture renderer
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		texturerenderer/CTextureRenderer.h
*  PURPOSE:		Header file for Direct3D video renderer base filter class
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*
*  Based on the PSDK Texture3D9 code
*  Copyright (c) Microsoft Corporation.  All rights reserved.
*
*****************************************************************************/

class CTextureRenderer;

#ifndef __CTEXTURERENDERER_H
#define __CTEXTURERENDERER_H

#include "ITextureRenderer.h"

#include <windows.h>
#include <dshow.h>
#include <uuids.h>
#include <dvdmedia.h>

#include "d3d9.h"
#include "d3dx9.h"

#include <streams.h>

//-----------------------------------------------------------------------------
// Define GUID for Texture Renderer
// {71771540-2017-11cf-AE26-0020AFD79767}
//-----------------------------------------------------------------------------
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

//-----------------------------------------------------------------------------
// CTextureRenderer Class Declarations
//-----------------------------------------------------------------------------
class CTextureRenderer : public CBaseVideoRenderer
{
public:
							CTextureRenderer	( HRESULT *phr, IDirect3DDevice9 *pDev, IVideoRenderer *pMaster );
							~CTextureRenderer	( void );

public:
    HRESULT					CheckMediaType		( const CMediaType *pmt );
    HRESULT					SetMediaType		( const CMediaType *pmt );
    HRESULT					DoRenderSample		( IMediaSample *pMediaSample );

	void					OnLostDevice		( void );
	void					OnResetDevice		( void );
   
    bool					m_bUseDynamicTextures;
    long					m_lVidWidth;
    long					m_lVidHeight;
    long					m_lVidPitch;

private:
	IDirect3DDevice9 *		m_pDevice;
	IDirect3DTexture9 *		m_pTexture;
	IVideoRenderer *		m_pMaster;
	D3DFORMAT				m_TextureFormat;
	LPCRITICAL_SECTION		m_pLock;

	GUID					m_Format;
};

#endif
