/************************************************************************
filename: 	CEGUIImageset_xmlHandler.cpp
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
#include "CEGUIImageset_xmlHandler.h"

#include "CEGUIExceptions.h"
#include "CEGUISystem.h"
#include "CEGUILogger.h"
#include "CEGUIXMLAttributes.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
Definition of constant data for Imageset (and sub-classes)
*************************************************************************/
// Declared in Imageset::xmlHandler
const String Imageset_xmlHandler::ImagesetElement( (utf8*)"Imageset" );
const String Imageset_xmlHandler::ImageElement( (utf8*)"Image" );
const char	Imageset_xmlHandler::ImagesetImageFileAttribute[]		= "Imagefile";
const char	Imageset_xmlHandler::ImagesetResourceGroupAttribute[]   = "ResourceGroup";
const char	Imageset_xmlHandler::ImagesetNameAttribute[]			= "Name";
const char	Imageset_xmlHandler::ImagesetNativeHorzResAttribute[]	= "NativeHorzRes";
const char	Imageset_xmlHandler::ImagesetNativeVertResAttribute[]	= "NativeVertRes";
const char	Imageset_xmlHandler::ImagesetAutoScaledAttribute[]		= "AutoScaled";
const char	Imageset_xmlHandler::ImageNameAttribute[]				= "Name";
const char	Imageset_xmlHandler::ImageXPosAttribute[]				= "XPos";
const char	Imageset_xmlHandler::ImageYPosAttribute[]				= "YPos";
const char	Imageset_xmlHandler::ImageWidthAttribute[]				= "Width";
const char	Imageset_xmlHandler::ImageHeightAttribute[]				= "Height";
const char	Imageset_xmlHandler::ImageXOffsetAttribute[]			= "XOffset";
const char	Imageset_xmlHandler::ImageYOffsetAttribute[]			= "YOffset";

/*************************************************************************
SAX2 Handler methods
*************************************************************************/
void Imageset_xmlHandler::elementStart(const String& element, const XMLAttributes& attributes)
{
	// handle an Image element (extract all element attributes and use data to define an Image for the Imageset)
	if (element == ImageElement)
	{
		String	name(attributes.getValueAsString(ImageNameAttribute));

		Rect	rect;
        rect.d_left	= (float)attributes.getValueAsInteger(ImageXPosAttribute);
        rect.d_top	= (float)attributes.getValueAsInteger(ImageYPosAttribute);
        rect.setWidth((float)attributes.getValueAsInteger(ImageWidthAttribute));
        rect.setHeight((float)attributes.getValueAsInteger(ImageHeightAttribute));

		Point	offset;
        offset.d_x	= (float)attributes.getValueAsInteger(ImageXOffsetAttribute, 0);
        offset.d_y	= (float)attributes.getValueAsInteger(ImageYOffsetAttribute, 0);

		d_imageset->defineImage(name, rect, offset);
	}
	// handle root Imageset element
	else if (element == ImagesetElement)
	{
        d_imageset->d_name = attributes.getValueAsString(ImagesetNameAttribute);

		Logger::getSingleton().logEvent("Started creation of Imageset '" + d_imageset->d_name + "' via XML file.", Informative);

		//
		// load auto-scaling configuration
		//
		float hres, vres;

		// get native horizontal resolution
        hres = (float)attributes.getValueAsInteger(ImagesetNativeHorzResAttribute, 640);

		// get native vertical resolution
        vres = (float)attributes.getValueAsInteger(ImagesetNativeVertResAttribute, 480);

		d_imageset->setNativeResolution(Size(hres, vres));

		// enable / disable auto-scaling for this Imageset according to the setting
        d_imageset->setAutoScalingEnabled(attributes.getValueAsBool(ImagesetAutoScaledAttribute, false));

		//
		// Create a Texture object via the specified filename, and set it as the texture for the Imageset
		//
        String filename(attributes.getValueAsString(ImagesetImageFileAttribute));
        String resourceGroup(attributes.getValueAsString(ImagesetResourceGroupAttribute));

		try
		{
			d_imageset->d_texture = System::getSingleton().getRenderer()->createTexture(filename, resourceGroup);
		}
		catch(...)
		{
			throw RendererException((utf8*)"Imageset::xmlHandler::startElement - An unexpected error occurred while creating a Texture object from file '" + filename + "'");
		}

        d_imageset->d_textureFilename = filename;
	}
	// anything else is an error which *should* have already been caught by XML validation
	else
	{
		throw FileIOException("Imageset::xmlHandler::startElement - Unexpected data was found while parsing the Imageset file: '" + element + "' is unknown.");
	}

}

void Imageset_xmlHandler::elementEnd(const String& element)
{
	if (element == ImagesetElement)
	{
		Logger::getSingleton().logEvent("Finished creation of Imageset '" + d_imageset->d_name + "' via XML file.", Informative);
	}
}

} // End of  CEGUI namespace section
