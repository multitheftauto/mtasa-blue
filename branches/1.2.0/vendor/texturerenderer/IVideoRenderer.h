/*****************************************************************************
*
*  PROJECT:		DirectX Video-to-texture renderer
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		texturerenderer/IVideoRenderer.h
*  PURPOSE:		Interface for Direct3D video renderer class
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

class IVideoRenderer;

#ifndef __IVIDEORENDERER_H
#define __IVIDEORENDERER_H

#include <windows.h>
#include "d3d9.h"

class IVideoRenderer
{
public:
	virtual void							SetVideoTexture			( IDirect3DTexture9 * pTexture ) = 0;

	virtual LPCRITICAL_SECTION				GetMutex				( void ) = 0;

	virtual void							OnLostDevice			( void ) = 0;
	virtual void							OnResetDevice			( IDirect3DDevice9* pDevice ) = 0;
};

#endif
