/************************************************************************
	filename: 	d3d9texture.cpp
	created:	17/7/2004
	author:		Paul D Turner with D3D 9 Updates by Magnus Österlind
	
	purpose:	Implements texture class for D3D9.0 system
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "renderers/directx9GUIRenderer/d3d9texture.h"
#include "renderers/directx9GUIRenderer/d3d9renderer.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"

#include <d3dx9.h>
#include <dxerr.h>
#undef max

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Constructor
*************************************************************************/
DirectX9Texture::DirectX9Texture(Renderer* owner) :
	Texture(owner)
{
	d_d3dtexture = NULL;

	// do this mainly to indicate the lack of a filename.
	d_isMemoryTexture = true;
	d_isRenderTarget = false;
}

/*************************************************************************
	Destructor
*************************************************************************/
DirectX9Texture::~DirectX9Texture(void)
{
	freeD3DTexture();
}

/*************************************************************************
	Load texture from file.  Texture is made to be same size as image in
	file.
*************************************************************************/
void DirectX9Texture::loadFromFile(const String& filename, const String& resourceGroup)
{
	freeD3DTexture();
	
	// load the file via the resource provider
	RawDataContainer texFile;
	System::getSingleton().getResourceProvider()->loadRawDataContainer(filename, texFile, resourceGroup);

	D3DXIMAGE_INFO texInfo;
	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(((DirectX9Renderer*)getRenderer())->getDevice(), texFile.getDataPtr(),
            static_cast<UINT>(texFile.getSize()), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
            D3DX_DEFAULT, D3DX_DEFAULT, 0, &texInfo, NULL, &d_d3dtexture);
	
	System::getSingleton().getResourceProvider()->unloadRawDataContainer(texFile);
	
	if (SUCCEEDED(hr))
	{
		d_width		= (ushort)texInfo.Width;
		d_height	= (ushort)texInfo.Height;

		d_filename = filename;
        d_resourceGroup = resourceGroup;
		d_isMemoryTexture = true;
		d_isRenderTarget = false;
	}
	else
	{
		throw RendererException((utf8*)"Failed to create Texture object from file '" + filename + "'.  Additional Info: " + (const utf8*)DXGetErrorString(hr));
	}

}


/*************************************************************************
	Load texture from raw memory.	
*************************************************************************/
void DirectX9Texture::loadFromMemory(const void* buffPtr, uint buffWidth, uint buffHeight)
{
	using namespace std;

	// release old texture
	freeD3DTexture();

	// calculate square size big enough for whole memory buffer
	uint tex_size = ceguimax(buffWidth, buffHeight);

	// create a texture
	// TODO: Check resulting pixel format and react appropriately.
	HRESULT hr = D3DXCreateTexture(((DirectX9Renderer*)getRenderer())->getDevice(), tex_size, tex_size, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &d_d3dtexture);

	if (FAILED(hr))
	{
		throw RendererException((utf8*)"Failed to load texture from memory: D3D Texture creation failed.");
	}
	else
	{
		D3DSURFACE_DESC	texdesc;
		d_d3dtexture->GetLevelDesc(0, &texdesc);

		// store new size;
		d_width		= (ushort)texdesc.Width;
		d_height	= (ushort)texdesc.Height;

		// lock the D3D texture
		D3DLOCKED_RECT	rect;
		hr = d_d3dtexture->LockRect(0, &rect, NULL, 0);

		if (FAILED(hr))
		{
			d_d3dtexture->Release();
			d_d3dtexture = NULL;

			throw RendererException((utf8*)"Failed to load texture from memory: IDirect3DTexture9::LockRect failed.");
		}
		else
		{
			// copy data from buffer into texture
			ulong* dst = (ulong*)rect.pBits;
			ulong* src = (ulong*)buffPtr;

			for (uint i = 0; i < buffHeight; ++i)
			{
				for (uint j = 0; j < buffWidth; ++j)
				{
					dst[j] = src[j];
				}

				dst += rect.Pitch / sizeof(ulong);
				src += buffWidth;
			}

			d_d3dtexture->UnlockRect(0);
		}

	}

}


/*************************************************************************
	safely release D3D texture associated with this Texture
*************************************************************************/
void DirectX9Texture::freeD3DTexture(void)
{
	if (d_d3dtexture != NULL)
	{
		d_d3dtexture->Release();
		d_d3dtexture = NULL;
	}

	d_filename.clear();
	d_isMemoryTexture = true;
	d_isRenderTarget = false;
}


void DirectX9Texture::createRenderTarget(uint width, uint height)
{
	freeD3DTexture();

	d_isMemoryTexture = true;
	d_isRenderTarget = true;

	HRESULT hr = D3DXCreateTexture(((DirectX9Renderer*)getRenderer())->getDevice(), width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d_d3dtexture);

	if (FAILED(hr))
	{
		throw RendererException((utf8*)"Failed to create texture of specified size: D3D Texture creation failed.");
	}
	else
	{
		D3DSURFACE_DESC	texdesc;
		d_d3dtexture->GetLevelDesc(0, &texdesc);

		// store new size;
		d_width		= (ushort)texdesc.Width;
		d_height	= (ushort)texdesc.Height;
	}
}


/*************************************************************************
	allocate a D3D texture >= 'size'.  Previous D3D texture is lost
*************************************************************************/
void DirectX9Texture::setD3DTextureSize(uint size)
{
	freeD3DTexture();

	HRESULT hr = D3DXCreateTexture(((DirectX9Renderer*)getRenderer())->getDevice(), size, size, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &d_d3dtexture);

	if (FAILED(hr))
	{
		throw RendererException((utf8*)"Failed to create texture of specified size: D3D Texture creation failed.");
	}
	else
	{
		D3DSURFACE_DESC	texdesc;
		d_d3dtexture->GetLevelDesc(0, &texdesc);

		// store new size;
		d_width		= (ushort)texdesc.Width;
		d_height	= (ushort)texdesc.Height;
	}

}


/*************************************************************************
	Direct3D support method that must be called prior to a Reset call
	on the Direct3DDevice.
*************************************************************************/
void DirectX9Texture::preD3DReset(void)
{
	// textures not based on files are in the managed pool, so we do
	// not worry about those.
	if (!d_isMemoryTexture || d_isRenderTarget)
	{
		// release the d3d texture
		if (d_d3dtexture != NULL)
		{
			if (FAILED(d_d3dtexture->Release()))
			{
				throw RendererException("DirectX9Texture::preD3DReset - failed to release the Direct3DTexture9 object for this texture.");
			}

			d_d3dtexture = NULL;
		}

	}

}


/*************************************************************************
	Direct3D support method that must be called after a Reset call on the
	Direct3DDevice.
*************************************************************************/
void DirectX9Texture::postD3DReset(void)
{
	// textures not based on files are in the managed pool, so we do
	// not worry about those.
	if (!d_isMemoryTexture)
	{
		// The reason this copy is made is that d_filename will be reset once we
		// call loadFromFile and loading would always fail due to invalid filenames.
		String name(d_filename);

		// re-load the texture
		loadFromFile(name, d_resourceGroup);
	} else if (d_isRenderTarget) {
		createRenderTarget(d_width,d_height);
	}
}

} // End of  CEGUI namespace section
