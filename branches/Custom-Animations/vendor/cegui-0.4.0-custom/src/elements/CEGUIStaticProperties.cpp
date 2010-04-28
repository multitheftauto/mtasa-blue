/************************************************************************
	filename: 	CEGUIStaticProperties.cpp
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Implements property classes for the Static widget.
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
#include "elements/CEGUIStaticProperties.h"
#include "elements/CEGUIStatic.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of StaticProperties namespace section
namespace StaticProperties
{
String	FrameEnabled::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Static*>(receiver)->isFrameEnabled());
}


void	FrameEnabled::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setFrameEnabled(PropertyHelper::stringToBool(value));
}


String	BackgroundEnabled::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Static*>(receiver)->isBackgroundEnabled());
}


void	BackgroundEnabled::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setBackgroundEnabled(PropertyHelper::stringToBool(value));
}


String	FrameColours::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourRectToString(static_cast<const Static*>(receiver)->getFrameColours());
}


void	FrameColours::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setFrameColours(PropertyHelper::stringToColourRect(value));
}


String	BackgroundColours::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourRectToString(static_cast<const Static*>(receiver)->getBackgroundColours());
}


void	BackgroundColours::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setBackgroundColours(PropertyHelper::stringToColourRect(value));
}


String	BackgroundImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getBackgroundImage());
}


void	BackgroundImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setBackgroundImage(PropertyHelper::stringToImage(value));
}


String	TopLeftFrameImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getImageForFrameLocation(TopLeftCorner));
}


void	TopLeftFrameImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setImageForFrameLocation(TopLeftCorner, PropertyHelper::stringToImage(value));
}


String	TopRightFrameImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getImageForFrameLocation(TopRightCorner));
}


void	TopRightFrameImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setImageForFrameLocation(TopRightCorner, PropertyHelper::stringToImage(value));
}


String	BottomLeftFrameImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getImageForFrameLocation(BottomLeftCorner));
}


void	BottomLeftFrameImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setImageForFrameLocation(BottomLeftCorner, PropertyHelper::stringToImage(value));
}


String	BottomRightFrameImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getImageForFrameLocation(BottomRightCorner));
}


void	BottomRightFrameImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setImageForFrameLocation(BottomRightCorner, PropertyHelper::stringToImage(value));
}


String	LeftFrameImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getImageForFrameLocation(LeftEdge));
}


void	LeftFrameImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setImageForFrameLocation(LeftEdge, PropertyHelper::stringToImage(value));
}


String	RightFrameImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getImageForFrameLocation(RightEdge));
}


void	RightFrameImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setImageForFrameLocation(RightEdge, PropertyHelper::stringToImage(value));
}


String	TopFrameImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getImageForFrameLocation(TopEdge));
}


void	TopFrameImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setImageForFrameLocation(TopEdge, PropertyHelper::stringToImage(value));
}


String	BottomFrameImage::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::imageToString(static_cast<const Static*>(receiver)->getImageForFrameLocation(BottomEdge));
}


void	BottomFrameImage::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Static*>(receiver)->setImageForFrameLocation(BottomEdge, PropertyHelper::stringToImage(value));
}

} // End of  StaticProperties namespace section

} // End of  CEGUI namespace section
