/*****************************************************************************
*
*  PROJECT:		DirectX Video-to-texture renderer
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		texturerenderer/ITextureRenderer.h
*  PURPOSE:		Interface for Direct3D video renderer base filter class
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

class ITextureRenderer;

#ifndef __ITEXTURERENDERER_H
#define __ITEXTURERENDERER_H

#include "IVideoRenderer.h"

#include <windows.h>
#include <dshow.h>
#include "d3d9.h"

IBaseFilter * CreateTextureRenderer ( HRESULT *phr, IDirect3DDevice9 *pDev, IVideoRenderer *pMaster );

#endif
