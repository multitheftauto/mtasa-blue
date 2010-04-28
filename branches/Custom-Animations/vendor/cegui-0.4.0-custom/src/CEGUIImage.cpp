/************************************************************************
	filename: 	CEGUIImage.cpp
	created:	13/3/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Image class members
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
#include "CEGUIImage.h"
#include "CEGUIExceptions.h"
#include "CEGUITexture.h"
#include "CEGUIImageset.h"
#include "CEGUIRenderer.h"
#include "CEGUIPropertyHelper.h"
#include <cmath>
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Constructor
*************************************************************************/
Image::Image(const Imageset* owner, const String& name, const Rect& area, const Point& render_offset, float horzScaling, float vertScaling) :
	d_owner(owner),
	d_area(area),
	d_offset(render_offset),
	d_name(name)
{
	if (d_owner == NULL)
	{
		throw NullObjectException((utf8*)"Image::Image - Imageset pointer passed to Image constructor must not be null.");
	}

	// setup initial image scaling
	setHorzScaling(horzScaling);
	setVertScaling(vertScaling);

	// TODO: if we ever store texture co-ordinates, they should be calculated here.
}

/*************************************************************************
	Copy constructor
*************************************************************************/
Image::Image(const Image& image) :
	d_owner(image.d_owner),
	d_area(image.d_area),
	d_offset(image.d_offset),
	d_scaledWidth(image.d_scaledWidth),
	d_scaledHeight(image.d_scaledHeight),
	d_scaledOffset(image.d_scaledOffset),
	d_name(image.d_name)
{
}

/*************************************************************************
	destructor
*************************************************************************/
Image::~Image(void)
{
}


/*************************************************************************
	set the horizontal scaling factor to be applied to this Image
*************************************************************************/
void Image::setHorzScaling(float factor)
{
	d_scaledWidth		= PixelAligned(d_area.getWidth() * factor);
	d_scaledOffset.d_x	= PixelAligned(d_offset.d_x * factor);
}


/*************************************************************************
	set the vertical scaling factor to be applied to this Image
*************************************************************************/
void Image::setVertScaling(float factor)
{
	d_scaledHeight		= PixelAligned(d_area.getHeight() * factor);
	d_scaledOffset.d_y	= PixelAligned(d_offset.d_y * factor);
}


/*************************************************************************
	Clip and then queue the image to be rendered.
*************************************************************************/
void Image::draw(const Rect& dest_rect, float z, const Rect& clip_rect, const ColourRect& colours, QuadSplitMode quad_split_mode) const
{
	Rect dest(dest_rect);

	// apply rendering offset to the destination Rect
	dest.offset(d_scaledOffset);

	// draw
	d_owner->draw(d_area, dest, z, clip_rect, colours, quad_split_mode);
}


/*************************************************************************
	String object containing the name of this Image	
*************************************************************************/
const String& Image::getName(void) const
{
	return d_name;
}


/*************************************************************************
	Return the name of the Imageset that contains this Image	
*************************************************************************/
const String& Image::getImagesetName(void) const
{
	return d_owner->getName();
}

/*************************************************************************
	Return Rect describing the source texture area used by this Image.
*************************************************************************/
const Rect& Image::getSourceTextureArea(void) const
{
    return d_area;
}


/*************************************************************************
    Output XML <Image ... > element for this image
*************************************************************************/
void Image::writeXMLToStream(OutStream& out_stream) const
{
    out_stream << "<Image Name=\"" << d_name.c_str() << "\" ";
    out_stream << "XPos=\"" << PropertyHelper::uintToString(static_cast<uint>(d_area.d_left)) << "\" ";
    out_stream << "YPos=\"" << PropertyHelper::uintToString(static_cast<uint>(d_area.d_top)) << "\" ";
    out_stream << "Width=\"" << PropertyHelper::uintToString(static_cast<uint>(d_area.getWidth())) << "\" ";
    out_stream << "Height=\"" << PropertyHelper::uintToString(static_cast<uint>(d_area.getHeight())) << "\" ";

    if (d_offset.d_x != 0.0f)
        out_stream << "XOffset=\"" << PropertyHelper::intToString(static_cast<int>(d_offset.d_x)) << "\" ";

    if (d_offset.d_y != 0.0f)
        out_stream << "YOffset=\"" << PropertyHelper::intToString(static_cast<int>(d_offset.d_x)) << "\" ";

    out_stream << "/>" << std::endl;
}


} // End of  CEGUI namespace section
