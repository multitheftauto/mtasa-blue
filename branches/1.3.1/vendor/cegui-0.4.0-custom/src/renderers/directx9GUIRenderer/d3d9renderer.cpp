/************************************************************************
	filename: 	d3d9renderer.cpp
	created:	17/7/2004
	author:		Paul D Turner with D3D 9 Updates by Magnus Österlind
	
	purpose:	Main source file for Renderer class using DirectX 9.0
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
#include "renderers/directx9GUIRenderer/d3d9renderer.h"
#include "renderers/directx9GUIRenderer/d3d9texture.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"

#include <d3dx9.h>
#include <dxerr.h>
#include <algorithm>
#undef min

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants definitions
*************************************************************************/
const int			DirectX9Renderer::VERTEX_PER_QUAD			= 6;
const int			DirectX9Renderer::VERTEX_PER_TRIANGLE		= 3;
const int			DirectX9Renderer::VERTEXBUFFER_CAPACITY	= 4096;
const ulong			DirectX9Renderer::VERTEX_FVF				= (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1);


/*************************************************************************
	Constructor
*************************************************************************/
DirectX9Renderer::DirectX9Renderer(LPDIRECT3DDEVICE9 device, uint max_quads)
{
	d_device = device;
	Size size(getViewportSize());

	constructor_impl(device, size);
}


/*************************************************************************
	method to do work of constructor
*************************************************************************/
void DirectX9Renderer::constructor_impl(LPDIRECT3DDEVICE9 device, const Size& display_size)
{
	d_device        = device;
	d_queueing      = true;
	d_currTexture   = NULL;
	d_buffer        = NULL;
	d_bufferPos     = 0;

	// initialise renderer display area
	d_display_area.d_left	= 0;
	d_display_area.d_top	= 0;
	d_display_area.setSize(display_size);

	// Create a vertex buffer
	if (FAILED(d_device->CreateVertexBuffer(
		(VERTEXBUFFER_CAPACITY * sizeof(QuadVertex)), 
		D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 
		VERTEX_FVF, 
		D3DPOOL_DEFAULT, 
		&d_buffer,
		NULL)))
	{
		// Ideally, this would have been a RendererException, but we can't use that until the System object is created
		// and that requires a Renderer passed to the constructor, so we throw this instead.
		throw std::runtime_error("Creation of VertexBuffer for Renderer object failed");
	}

	// get the maximum available texture size.
	D3DCAPS9	devCaps;
	if (FAILED(device->GetDeviceCaps(&devCaps)))
	{
		// release vertex buffer
		d_buffer->Release();
		throw std::runtime_error("Unable to retrieve device capabilities from Direct3DDevice9.");
	}

	// set max texture size the the smaller of max width and max height.
	d_maxTextureSize = ceguimin(devCaps.MaxTextureWidth, devCaps.MaxTextureHeight);

	d_device->AddRef();

    // set ID string
    d_identifierString = "CEGUI::DirectX81Renderer - Official Direct3D 9 based renderer module for CEGUI";
}


/*************************************************************************
	Destructor
*************************************************************************/
DirectX9Renderer::~DirectX9Renderer(void)
{
	if (d_buffer != NULL)
	{
		d_buffer->Release();
	}

	destroyAllTextures();

	if (d_device != NULL)
	{
		d_device->Release();
	}
}


/*************************************************************************
	add's a quad to the list to be rendered
*************************************************************************/
void DirectX9Renderer::addQuad(const Rect& dest_rect, float z, const Texture* tex, const Rect& texture_rect, const ColourRect& colours, QuadSplitMode quad_split_mode, const Image* image)
{
	// if not queueing, render directly (as in, right now!)
	if (!d_queueing)
	{
		renderQuadDirect(dest_rect, z, tex, texture_rect, colours, quad_split_mode);
	}
	else
	{
		QuadInfo quad;

		quad.position		= dest_rect;
		quad.z				= z;
		quad.texture		= ((DirectX9Texture*)tex);
        quad.image          = image;
		quad.texPosition	= texture_rect;
		quad.topLeftCol		= colours.d_top_left.getARGB();
		quad.topRightCol	= colours.d_top_right.getARGB();
		quad.bottomLeftCol	= colours.d_bottom_left.getARGB();
		quad.bottomRightCol	= colours.d_bottom_right.getARGB();

		// offset destination to get correct texel to pixel mapping from Direct3D
		quad.position.offset(Point(-0.5f, -0.5f));

        // set quad split mode
        quad.splitMode = quad_split_mode;

		d_quadlist.insert(quad);
	}

}


//
//	Check quadlist has valid image items
//
void DirectX9Renderer::NotifyImageInvalid ( Image* const image )
{
    // Quickest thing to do here is to clear the quad list
    d_quadlist.clear ();
#if 0
	for (QuadList::iterator i = d_quadlist.begin(); i != d_quadlist.end(); )
	{
		const QuadInfo& quad = (*i);
        if ( quad.image == image )
        {
            d_quadlist.erase ( i++ );
        }
        else
            ++i;
    }
#endif
}


/*************************************************************************
	perform final rendering for all queued renderable quads.
*************************************************************************/
void DirectX9Renderer::doRender(void)
{
	d_currTexture = NULL;

    if ( !d_buffer )
        return;

	initPerFrameStates();

	bool locked = false;
	QuadVertex*	buffmem;

	// iterate over each quad in the list
	for (QuadList::iterator i = d_quadlist.begin(); i != d_quadlist.end(); ++i)
	{
		const QuadInfo& quad = (*i);

		LPDIRECT3DTEXTURE9 d3dTexture = quad.texture->getD3DTexture();
		// flush & set texture if needed
		if (d_currTexture != d3dTexture)
		{
			if (locked)
			{
				d_buffer->Unlock();
				locked = false;
			}

			// render any remaining quads for current texture
			renderVBuffer();

			// set new texture
			d_device->SetTexture(0, d3dTexture);
			d_currTexture = d3dTexture;
		}

		if (!locked)
		{
			if (FAILED(d_buffer->Lock(0, 0, (void**)&buffmem, D3DLOCK_DISCARD)))
			{
				return;
			}

			locked = true;
		}

        // Hack: Inform the Font class that this glyph has been used recently, if it's a glyph being drawn
        if ( quad.image )
        {
            unsigned long ulCodepoint = quad.image->getCodepoint();
            // Is it a glyph?
            if ( ulCodepoint != 0 && ulCodepoint > 127 && ulCodepoint < 65534 )
            {
                CEGUI::Font* pFont = quad.image->getFont();
                if ( pFont )
                    pFont->refreshCachedGlyph(ulCodepoint);
            }
        }

		// setup Vertex 1...
		buffmem->x = quad.position.d_left;
		buffmem->y = quad.position.d_top;
		buffmem->z = quad.z;
		buffmem->rhw = 1.0f;
		buffmem->diffuse = quad.topLeftCol;
		buffmem->tu1 = quad.texPosition.d_left;
		buffmem->tv1 = quad.texPosition.d_top;
		++buffmem;

		// setup Vertex 2...

        // top-left to bottom-right diagonal
        if (quad.splitMode == TopLeftToBottomRight)
        {
            buffmem->x = quad.position.d_right;
            buffmem->y = quad.position.d_bottom;
            buffmem->z = quad.z;
            buffmem->rhw = 1.0f;
            buffmem->diffuse = quad.bottomRightCol;
            buffmem->tu1 = quad.texPosition.d_right;
            buffmem->tv1 = quad.texPosition.d_bottom;
        }
        // bottom-left to top-right diagonal
        else
        {
            buffmem->x = quad.position.d_right;
            buffmem->y = quad.position.d_top;
            buffmem->z = quad.z;
            buffmem->rhw = 1.0f;
            buffmem->diffuse = quad.topRightCol;
            buffmem->tu1 = quad.texPosition.d_right;
            buffmem->tv1 = quad.texPosition.d_top;
        }
        ++buffmem;

		// setup Vertex 3...
		buffmem->x = quad.position.d_left;
		buffmem->y = quad.position.d_bottom;
		buffmem->z = quad.z;
		buffmem->rhw = 1.0f;
		buffmem->diffuse = quad.bottomLeftCol;
		buffmem->tu1 = quad.texPosition.d_left;
		buffmem->tv1 = quad.texPosition.d_bottom;
		++buffmem;

		// setup Vertex 4...
		buffmem->x = quad.position.d_right;
		buffmem->y = quad.position.d_top;
		buffmem->z = quad.z;
		buffmem->rhw = 1.0f;
		buffmem->diffuse = quad.topRightCol;
		buffmem->tu1 = quad.texPosition.d_right;
		buffmem->tv1 = quad.texPosition.d_top;
		++buffmem;

		// setup Vertex 5...
		buffmem->x = quad.position.d_right;
		buffmem->y = quad.position.d_bottom;
		buffmem->z = quad.z;
		buffmem->rhw = 1.0f;
		buffmem->diffuse = quad.bottomRightCol;
		buffmem->tu1 = quad.texPosition.d_right;
		buffmem->tv1 = quad.texPosition.d_bottom;
		++buffmem;

		// setup Vertex 6...

        // top-left to bottom-right diagonal
        if (quad.splitMode == TopLeftToBottomRight)
        {
            buffmem->x = quad.position.d_left;
            buffmem->y = quad.position.d_top;
            buffmem->z = quad.z;
            buffmem->rhw = 1.0f;
            buffmem->diffuse = quad.topLeftCol;
            buffmem->tu1 = quad.texPosition.d_left;
            buffmem->tv1 = quad.texPosition.d_top;
        }
        // bottom-left to top-right diagonal
        else
        {
            buffmem->x = quad.position.d_left;
            buffmem->y = quad.position.d_bottom;
            buffmem->z = quad.z;
            buffmem->rhw = 1.0f;
            buffmem->diffuse = quad.bottomLeftCol;
            buffmem->tu1 = quad.texPosition.d_left;
            buffmem->tv1 = quad.texPosition.d_bottom;
        }
        ++buffmem;

		// update buffer level
		d_bufferPos += VERTEX_PER_QUAD;

		// if there is not enough room in the buffer for another sprite, render what we have
		if (d_bufferPos >= (VERTEXBUFFER_CAPACITY - VERTEX_PER_QUAD))
		{
			if (locked)
			{
				d_buffer->Unlock();
				locked = false;
			}

			renderVBuffer();
		}

	}

	if (locked)
	{
		d_buffer->Unlock();
		locked = false;
	}

	renderVBuffer();
}


/*************************************************************************
	clear the queue
*************************************************************************/
void DirectX9Renderer::clearRenderList(void)
{
	d_quadlist.clear();
}


/*************************************************************************
	create an empty texture
*************************************************************************/
Texture* DirectX9Renderer::createTexture(void)
{
	DirectX9Texture* tex = new DirectX9Texture(this);
	d_texturelist.push_back(tex);
	return tex;
}


/*************************************************************************
	Create a new Texture object and load a file into it.
*************************************************************************/
Texture* DirectX9Renderer::createTexture(const String& filename, const String& resourceGroup)
{
	DirectX9Texture* tex = (DirectX9Texture*)createTexture();
	tex->loadFromFile(filename, resourceGroup);

	return tex;
}


/*************************************************************************
	Create a new texture with the given dimensions
*************************************************************************/
Texture* DirectX9Renderer::createTexture(float size)
{
	DirectX9Texture* tex = (DirectX9Texture*)createTexture();
	tex->setD3DTextureSize((uint)size);

	return tex;
}

/*************************************************************************
	Destroy a texture
*************************************************************************/
void DirectX9Renderer::destroyTexture(Texture* texture)
{
	if (texture != NULL)
	{
		DirectX9Texture* tex = (DirectX9Texture*)texture;
		d_texturelist.remove(tex);
		delete tex;
	}

}


/*************************************************************************
	destroy all textures still active
*************************************************************************/
void DirectX9Renderer::destroyAllTextures(void)
{
	while (!d_texturelist.empty())
	{
		destroyTexture(*(d_texturelist.begin()));
	}
}


/*************************************************************************
	setup states etc
*************************************************************************/
void DirectX9Renderer::initPerFrameStates(void)
{
	// setup vertex stream
	d_device->SetStreamSource(0, d_buffer, 0, sizeof(QuadVertex));
	d_device->SetFVF(VERTEX_FVF);
	d_device->SetVertexShader( NULL );
	d_device->SetPixelShader( NULL );

	// set device states
	d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


	// setup texture addressing settings
	d_device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	d_device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// setup colour calculations
	d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	// setup alpha calculations
	d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	// setup filtering
	d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// setup scene alpha blending
	d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}


/*************************************************************************
	renders whatever is in the vertex buffer
*************************************************************************/
void DirectX9Renderer::renderVBuffer(void)
{
	// if bufferPos is 0 there is no data in the buffer and nothing to render
	if (d_bufferPos == 0)
	{
		return;
	}

	// render the sprites
	d_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, (d_bufferPos / VERTEX_PER_TRIANGLE));

	// reset buffer position to 0...
	d_bufferPos = 0;
}


/*************************************************************************
	sort quads list according to texture
*************************************************************************/
void DirectX9Renderer::sortQuads(void)
{
}


/*************************************************************************
	render a quad directly to the display
*************************************************************************/
void DirectX9Renderer::renderQuadDirect(const Rect& dest_rect, float z, const Texture* tex, const Rect& texture_rect, const ColourRect& colours, QuadSplitMode quad_split_mode)
{
    if ( !d_buffer )
        return;

	// ensure offset destination to ensure proper texel to pixel mapping from D3D.
	Rect final_rect(dest_rect);
	final_rect.offset(Point(-0.5f, -0.5f));

	QuadVertex*	buffmem;

	initPerFrameStates();
	d_device->SetTexture(0, ((DirectX9Texture*)tex)->getD3DTexture());

	if (SUCCEEDED(d_buffer->Lock(0, VERTEX_PER_QUAD * sizeof(QuadVertex), (void**)&buffmem, D3DLOCK_DISCARD)))
	{
		// setup Vertex 1...
		buffmem->x = final_rect.d_left;
		buffmem->y = final_rect.d_top;
		buffmem->z = z;
		buffmem->rhw = 1.0f;
		buffmem->diffuse = colours.d_top_left.getARGB();
		buffmem->tu1 = texture_rect.d_left;
		buffmem->tv1 = texture_rect.d_top;
		++buffmem;

        // setup Vertex 2...

        // top-left to bottom-right diagonal
        if (quad_split_mode == TopLeftToBottomRight)
        {
            buffmem->x = final_rect.d_right;
            buffmem->y = final_rect.d_bottom;
            buffmem->z = z;
            buffmem->rhw = 1.0f;
            buffmem->diffuse = colours.d_bottom_right.getARGB();
            buffmem->tu1 = texture_rect.d_right;
            buffmem->tv1 = texture_rect.d_bottom;
        }
        // bottom-left to top-right diagonal
        else
        {
            buffmem->x = final_rect.d_right;
            buffmem->y = final_rect.d_top;
            buffmem->z = z;
            buffmem->rhw = 1.0f;
            buffmem->diffuse = colours.d_top_right.getARGB();
            buffmem->tu1 = texture_rect.d_right;
            buffmem->tv1 = texture_rect.d_top;
        }
        ++buffmem;

		// setup Vertex 3...
		buffmem->x = final_rect.d_left;
		buffmem->y = final_rect.d_bottom;
		buffmem->z = z;
		buffmem->rhw = 1.0f;
		buffmem->diffuse = colours.d_bottom_left.getARGB();
		buffmem->tu1 = texture_rect.d_left;
		buffmem->tv1 = texture_rect.d_bottom;
		++buffmem;

		// setup Vertex 4...
		buffmem->x = final_rect.d_right;
		buffmem->y = final_rect.d_top;
		buffmem->z = z;
		buffmem->rhw = 1.0f;
		buffmem->diffuse = colours.d_top_right.getARGB();
		buffmem->tu1 = texture_rect.d_right;
		buffmem->tv1 = texture_rect.d_top;
		++buffmem;

		// setup Vertex 5...
		buffmem->x = final_rect.d_right;
		buffmem->y = final_rect.d_bottom;
		buffmem->z = z;
		buffmem->rhw = 1.0f;
		buffmem->diffuse = colours.d_bottom_right.getARGB();
		buffmem->tu1 = texture_rect.d_right;
		buffmem->tv1 = texture_rect.d_bottom;
		++buffmem;

		// setup Vertex 6...

        // top-left to bottom-right diagonal
        if (quad_split_mode == TopLeftToBottomRight)
        {
            buffmem->x = final_rect.d_left;
            buffmem->y = final_rect.d_top;
            buffmem->z = z;
            buffmem->rhw = 1.0f;
            buffmem->diffuse = colours.d_top_left.getARGB();
            buffmem->tu1 = texture_rect.d_left;
            buffmem->tv1 = texture_rect.d_top;
        }
        // bottom-left to top-right diagonal
        else
        {
            buffmem->x = final_rect.d_left;
            buffmem->y = final_rect.d_bottom;
            buffmem->z = z;
            buffmem->rhw = 1.0f;
            buffmem->diffuse = colours.d_bottom_left.getARGB();
            buffmem->tu1 = texture_rect.d_left;
            buffmem->tv1 = texture_rect.d_bottom;
        }

		d_buffer->Unlock();
		d_bufferPos = VERTEX_PER_QUAD;

		renderVBuffer();
	}

}


/*************************************************************************
	Direct3D support method that must be called prior to a Reset call
	on the Direct3DDevice.
*************************************************************************/
void DirectX9Renderer::preD3DReset(void)
{
	// release the buffer prior to the reset call (will be re-created later)
    if ( d_buffer )
    {
        if (FAILED(d_buffer->Release()))
        {
            throw RendererException("DirectX9Renderer::preD3DReset - Failed to release the VertexBuffer used by the DirectX9Renderer object.");
        }

        d_buffer = 0;
    }

	// perform pre-reset operations on all textures
	std::list<DirectX9Texture*>::iterator ctex = d_texturelist.begin();
	std::list<DirectX9Texture*>::iterator endtex = d_texturelist.end();

	for (; ctex != endtex; ++ctex)
	{
		(*ctex)->preD3DReset();
	}

}


/*************************************************************************
	Direct3D support method that must be called after a Reset call on the
	Direct3DDevice.
*************************************************************************/
void DirectX9Renderer::postD3DReset(void)
{
	// Recreate a vertex buffer
	if (FAILED(d_device->CreateVertexBuffer((VERTEXBUFFER_CAPACITY * sizeof(QuadVertex)), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, VERTEX_FVF, D3DPOOL_DEFAULT, &d_buffer, NULL)))
	{
		throw RendererException("DirectX9Renderer::preD3DReset - Failed to create the VertexBuffer for use by the DirectX9Renderer object.");
	}

	// perform post-reset operations on all textures
	std::list<DirectX9Texture*>::iterator ctex = d_texturelist.begin();
	std::list<DirectX9Texture*>::iterator endtex = d_texturelist.end();

	for (; ctex != endtex; ++ctex)
	{
		(*ctex)->postD3DReset();
	}

	// update display size
	setDisplaySize(getViewportSize());

	// Now we've come back, we MUST ensure a full redraw is done since the
	// textures in the stored quads will have been invalidated.
	System::getSingleton().signalRedraw();
}

/*************************************************************************
	return size of device view port (if possible)	
*************************************************************************/
Size DirectX9Renderer::getViewportSize(void)
{
	// initialise renderer size
	D3DVIEWPORT9	vp;

	if (FAILED(d_device->GetViewport(&vp)))
	{
		throw std::runtime_error("Unable to access required view port information from Direct3DDevice9.");
	}
	else
	{
		return Size((float)vp.Width, (float)vp.Height);
	}

}


/*************************************************************************
	Set the size of the display in pixels.	
*************************************************************************/
void DirectX9Renderer::setDisplaySize(const Size& sz)
{
	if (d_display_area.getSize() != sz)
	{
		d_display_area.setSize(sz);

		EventArgs args;
		fireEvent(EventDisplaySizeChanged, args, EventNamespace);
	}

}


} // End of  CEGUI namespace section

