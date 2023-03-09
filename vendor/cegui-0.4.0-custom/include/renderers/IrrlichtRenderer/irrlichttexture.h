/************************************************************************
	filename: 	irrlichttexture.h
	created:	20/7/2004
	author:		Thomas Suter
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
#ifndef IRRLICHTTEXTURE_H_INCLUDED
#define IRRLICHTTEXTURE_H_INCLUDED

#include "IrrlichtRendererDef.h"

#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"

#include <irrlicht.h>

namespace CEGUI
{
	class IRRLICHT_GUIRENDERER_API IrrlichtTexture : public Texture
	{
	public:

		// constructor
		IrrlichtTexture(Renderer* r, irr::IrrlichtDevice* device);
		
		// destructor
		virtual ~IrrlichtTexture();
		
		// returns the irrlich texture
		irr::video::ITexture* getTexture();

		// set the irrlicht texture
		void setTexture(irr::video::ITexture* texture);


		/*!
		\brief
		Returns the current pixel width of the texture

		\return
		ushort value that is the current width of the texture in pixels
		*/
		virtual	ushort	getWidth(void) const;


		/*!
		\brief
		Returns the current pixel height of the texture

		\return
		ushort value that is the current height of the texture in pixels
		*/
		virtual	ushort	getHeight(void) const;


		/*!
		\brief
		Loads the specified image file into the texture.  The texture is resized as required to hold the image.

		\param filename
		The filename of the image file that is to be loaded into the texture

	    \param resourceGroup
        Resource group identifier passed to the resource provider.

		\return
		Nothing.
		*/
		virtual void	loadFromFile(const String& filename, const String& resourceGroup);


		/*!
		\brief
		Loads (copies) an image in memory into the texture.  The texture is resized as required to hold the image.

		\param buffPtr
		Pointer to the buffer containing the image data

		\param buffWidth
		Width of the buffer (in 0xAARRGGBB pixels)

		\param buffHeight
		Height of the buffer (in 0xAARRGGBB pixels)

		\return
		Nothing.
		*/
		virtual void	loadFromMemory(const void* buffPtr, uint buffWidth, uint buffHeight);

	private:

		// counter for unique texture names
		static int iTextureNumber;

		// generate a unique name
		static irr::core::stringc getUniqueName(void);
		
		// remove the texture from irrlicht textures
		void freeTexture();

		// the current texture for rendering
		irr::video::ITexture* tex;

		// the irrlicht video driver 
		irr::video::IVideoDriver* driver;

		// the irrlicht device
		irr::IrrlichtDevice* device;

	};
}
#endif
