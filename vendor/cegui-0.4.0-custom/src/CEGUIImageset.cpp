/************************************************************************
	filename: 	CEGUIImageset.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements the Imageset class
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
#include "StdInc.h"
#include "CEGUIImageset.h"
#include "CEGUIExceptions.h"
#include "CEGUITexture.h"
#include "CEGUIRenderer.h"
#include "CEGUISystem.h"
#include "CEGUIImageset_xmlHandler.h"
#include "CEGUILogger.h"
#include "CEGUIDataContainer.h"
#include "CEGUIXMLParser.h"
#include <iostream>
#include <cmath>

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Definition of constant data for Imageset (and sub-classes)
*************************************************************************/
// Declared in Imageset
const char	Imageset::ImagesetSchemaName[]			= "Imageset.xsd";


/*************************************************************************
	constructor
*************************************************************************/
Imageset::Imageset(const String& name, Texture* texture, bool bDestroyTextureManagedExternally) :
    d_name(name),
    d_texture(texture),
    d_bDestroyTextureManagedExternally(bDestroyTextureManagedExternally)
{
	if (d_texture == NULL)
	{
		throw NullObjectException((utf8*)"Imageset::Imageset - Texture object supplied for Imageset creation must not be NULL");
	}

	// defaults for scaling options
	d_autoScale = false;
	setNativeResolution(Size(DefaultNativeHorzRes, DefaultNativeVertRes));
}


/*************************************************************************
	construct and initialise Imageset from the specified file.
*************************************************************************/
Imageset::Imageset(const String& filename, const String& resourceGroup, bool bDestroyTextureManagedExternally) :
    d_bDestroyTextureManagedExternally(bDestroyTextureManagedExternally)
{
	// defaults for scaling options
	d_autoScale = false;
	setNativeResolution(Size(DefaultNativeHorzRes, DefaultNativeVertRes));

	d_texture = NULL;
	load(filename, resourceGroup);
}


Imageset::Imageset(const String& name, const String& filename, const String& resourceGroup, bool bDestroyTextureManagedExternally) :
    d_name(name),
    d_bDestroyTextureManagedExternally(bDestroyTextureManagedExternally)
{
    // try to load the image file using the renderer
    d_texture =
        System::getSingleton().getRenderer()->createTexture(filename, resourceGroup);

    // initialse the auto-scaling for this Imageset
    d_autoScale = true;
    setNativeResolution(
        Size(d_texture->getWidth(), d_texture->getHeight()));

    // define the default image for this Imageset
    defineImage(
        "full_image",
        Rect(0, 0, d_texture->getWidth(), d_texture->getHeight()),
        Point(0, 0)
    );
}


/*************************************************************************
	destructor
*************************************************************************/
Imageset::~Imageset(void)
{
	unload();
}


/*************************************************************************
	Set texture for use by this imageset object
*************************************************************************/
void Imageset::setTexture(Texture* texture)
{
	if (d_texture == NULL)
	{
		throw NullObjectException((utf8*)"Imageset::setTexture - Texture object supplied for Imageset creation must not be NULL");
	}

	d_texture = texture;
}


/*************************************************************************
	load Imageset data from the specified file
*************************************************************************/
void Imageset::load(const String& filename, const String& resourceGroup)
{
	// unload old data and texture.
	unload();

	if (filename.empty() || (filename == (utf8*)""))
	{
		throw InvalidRequestException((utf8*)"Imageset::load - Filename supplied for Imageset loading must be valid");
	}

    // create handler object
    Imageset_xmlHandler handler(this);

	// do parse (which uses handler to create actual data)
	try
	{
        System::getSingleton().getXMLParser()->parseXMLFile(handler, filename, ImagesetSchemaName, resourceGroup);
	}
	catch(...)
	{
		unload();

        Logger::getSingleton().logEvent("Imageset::load - loading of Imageset from file '" + filename +"' failed.", Errors);
        throw;
	}

}

 
/*************************************************************************
	return the Image object for the named image
*************************************************************************/
const Image& Imageset::getImage(const String& name) const
{
	ImageRegistry::const_iterator	pos = d_images.find(name);

	if (pos == d_images.end())
	{
		throw	UnknownObjectException("Imageset::getImage - The Image named '" + name + "' could not be found in Imageset '" + d_name + "'.");
	}

	return pos->second;
}


/*************************************************************************
	defines a new Image.
*************************************************************************/
void Imageset::defineImage(const String& name, const Rect& image_rect, const Point& render_offset, unsigned long ulCodepoint, Font* pFont )
{
	if (isImageDefined(name))
	{
		throw AlreadyExistsException("Imageset::defineImage - An image with the name '" + name + "' already exists in Imageset '" + d_name + "'.");
	}

	// get scaling factors
	float hscale = d_autoScale ? d_horzScaling : 1.0f;
	float vscale = d_autoScale ? d_vertScaling : 1.0f;

	// add the Image definition
	d_images[name] = Image(this, name, image_rect, render_offset, hscale, vscale, ulCodepoint, pFont );

	CEGUI_LOGINSANE("Image '" + name + "' has been defined for Imageset '" + d_name + "'.")
}


/*************************************************************************
	Queues an area of the associated Texture the be drawn on the screen.
	Low-level routine not normally used!
*************************************************************************/
void Imageset::draw(const Rect& source_rect, const Rect& dest_rect, float z, const Rect& clip_rect,const ColourRect& colours, QuadSplitMode quad_split_mode, const Image* image ) const
{
	// get the rect area that we will actually draw to (i.e. perform clipping)
	Rect final_rect(dest_rect.getIntersection(clip_rect));

	// check if rect was totally clipped
	if (final_rect.getWidth() != 0)
	{
		float x_scale = 1.0f / (float)d_texture->getWidth();
		float y_scale = 1.0f / (float)d_texture->getHeight();

		float tex_per_pix_x = source_rect.getWidth() / dest_rect.getWidth();
		float tex_per_pix_y = source_rect.getHeight() / dest_rect.getHeight();

		// calculate final, clipped, texture co-ordinates
		Rect  tex_rect((source_rect.d_left + ((final_rect.d_left - dest_rect.d_left) * tex_per_pix_x)) * x_scale,
			(source_rect.d_top + ((final_rect.d_top - dest_rect.d_top) * tex_per_pix_y)) * y_scale,
			(source_rect.d_right + ((final_rect.d_right - dest_rect.d_right) * tex_per_pix_x)) * x_scale,
			(source_rect.d_bottom + ((final_rect.d_bottom - dest_rect.d_bottom) * tex_per_pix_y)) * y_scale);

		final_rect.d_left	= PixelAligned(final_rect.d_left);
		final_rect.d_right	= PixelAligned(final_rect.d_right);
		final_rect.d_top	= PixelAligned(final_rect.d_top);
		final_rect.d_bottom	= PixelAligned(final_rect.d_bottom);

		// queue a quad to be rendered
		d_texture->getRenderer()->addQuad(final_rect, z, d_texture, tex_rect, colours, quad_split_mode,image);
	}

}

/*************************************************************************
	Unload all data, leaving Imageset in a clean (but unusable) state
*************************************************************************/
void Imageset::unload(void)
{
	undefineAllImages();

	// cleanup texture
    // MTA destroys textures manually
    if ( !d_bDestroyTextureManagedExternally )
    	System::getSingleton().getRenderer()->destroyTexture(d_texture);
	d_texture = NULL;
}


/*************************************************************************
	Sets the scaling factor for all Images that are a part of this Imageset.
*************************************************************************/
void Imageset::updateImageScalingFactors(void)
{
	float hscale, vscale;

	if (d_autoScale)
	{
		hscale = d_horzScaling;
		vscale = d_vertScaling;
	}
	else
	{
		hscale = vscale = 1.0f;
	}

	ImageRegistry::iterator pos = d_images.begin(), end = d_images.end();
	for(; pos != end; ++pos)
	{
		pos->second.setHorzScaling(hscale);
		pos->second.setVertScaling(vscale);
	}

}


/*************************************************************************
	Enable or disable auto-scaling for this Imageset.
*************************************************************************/
void Imageset::setAutoScalingEnabled(bool setting)
{
	if (setting != d_autoScale)
	{
		d_autoScale = setting;
		updateImageScalingFactors();
	}

}


/*************************************************************************
	Set the native resolution for this Imageset
*************************************************************************/
void Imageset::setNativeResolution(const Size& size)
{
	d_nativeHorzRes = size.d_width;
	d_nativeVertRes = size.d_height;

	// re-calculate scaling factors & notify images as required
	notifyScreenResolution(System::getSingleton().getRenderer()->getSize());
}


/*************************************************************************
	Notify the Imageset of the current (usually new) display resolution.
*************************************************************************/
void Imageset::notifyScreenResolution(const Size& size)
{
	d_horzScaling = size.d_width / d_nativeHorzRes;
	d_vertScaling = size.d_height / d_nativeVertRes;

	if (d_autoScale)
	{
		updateImageScalingFactors();
	}

}

void Imageset::writeXMLToStream(OutStream& out_stream) const
{
    // output opening tag
    out_stream << "<Imageset Name=\"" << d_name << "\" ";
    out_stream << "Filename=\"" << d_textureFilename << "\" ";

    if (d_nativeHorzRes != DefaultNativeHorzRes)
        out_stream << "NativeHorzRes=\"" << static_cast<uint>(d_nativeHorzRes) << "\" ";

    if (d_nativeVertRes != DefaultNativeVertRes)
        out_stream << "NativeVertRes=\"" << static_cast<uint>(d_nativeVertRes) << "\" ";

    if (d_autoScale)
        out_stream << "AutoScaled=\"True\" ";

    out_stream << ">" << std::endl;

    // output images
    ImageIterator image = getIterator();

    while (!image.isAtEnd())
    {
        image.getCurrentValue().writeXMLToStream(out_stream);
        ++image;
    }

    // output closing tag
    out_stream << "</Imageset>" << std::endl;
}


/*************************************************************************
	Return an iterator object that can be used to iterate over the Image
	objects in the Imageset.
*************************************************************************/
Imageset::ImageIterator Imageset::getIterator(void) const
{
	return ImageIterator(d_images.begin(), d_images.end());
}


void Imageset::undefineImage(const String& name)
{
	d_images.erase(name);

	CEGUI_LOGINSANE("Image '" + name + "' has been removed from Imageset '" + d_name + "'.")
}


void Imageset::undefineAllImages(void)
{
	d_images.clear();

	CEGUI_LOGINSANE("All images have been removed from Imageset '" + d_name + "'.")
}

} // End of  CEGUI namespace section
