/************************************************************************
	filename: 	CEGUIStaticTextProperties.cpp
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Implements properties for the StaticText class
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
#include "elements/CEGUIStaticTextProperties.h"
#include "elements/CEGUIStaticText.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of StaticTextProperties namespace section
namespace StaticTextProperties
{
String	TextColours::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourRectToString(static_cast<const StaticText*>(receiver)->getTextColours());
}


void	TextColours::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<StaticText*>(receiver)->setTextColours(PropertyHelper::stringToColourRect(value));
}


String	HorzFormatting::get(const PropertyReceiver* receiver) const
{
	switch(static_cast<const StaticText*>(receiver)->getHorizontalFormatting())
	{
	case StaticText::RightAligned:
		return String((utf8*)"RightAligned");
		break;

	case StaticText::HorzCentred:
		return String((utf8*)"HorzCentred");
		break;

	case StaticText::HorzJustified:
		return String((utf8*)"HorzJustified");
		break;

	case StaticText::WordWrapLeftAligned:
		return String((utf8*)"WordWrapLeftAligned");
		break;

	case StaticText::WordWrapRightAligned:
		return String((utf8*)"WordWrapRightAligned");
		break;

	case StaticText::WordWrapCentred:
		return String((utf8*)"WordWrapCentred");
		break;

	case StaticText::WordWrapJustified:
		return String((utf8*)"WordWrapJustified");
		break;

	default:
		return String((utf8*)"LeftAligned");
		break;
	}
}


void	HorzFormatting::set(PropertyReceiver* receiver, const String& value)
{
	StaticText::HorzFormatting fmt;

	if (value == (utf8*)"RightAligned")
	{
		fmt = StaticText::RightAligned;
	}
	else if (value == (utf8*)"HorzCentred")
	{
		fmt = StaticText::HorzCentred;
	}
	else if (value == (utf8*)"HorzJustified")
	{
		fmt = StaticText::HorzJustified;
	}
	else if (value == (utf8*)"WordWrapLeftAligned")
	{
		fmt = StaticText::WordWrapLeftAligned;
	}
	else if (value == (utf8*)"WordWrapRightAligned")
	{
		fmt = StaticText::WordWrapRightAligned;
	}
	else if (value == (utf8*)"WordWrapCentred")
	{
		fmt = StaticText::WordWrapCentred;
	}
	else if (value == (utf8*)"WordWrapJustified")
	{
		fmt = StaticText::WordWrapJustified;
	}
	else
	{
		fmt = StaticText::LeftAligned;
	}

	static_cast<StaticText*>(receiver)->setHorizontalFormatting(fmt);
}


String	VertFormatting::get(const PropertyReceiver* receiver) const
{
	switch(static_cast<const StaticText*>(receiver)->getVerticalFormatting())
	{
	case StaticText::BottomAligned:
		return String((utf8*)"BottomAligned");
		break;

	case StaticText::VertCentred:
		return String((utf8*)"VertCentred");
		break;

	default:
		return String((utf8*)"TopAligned");
		break;
	}
}


void	VertFormatting::set(PropertyReceiver* receiver, const String& value)
{
	StaticText::VertFormatting fmt;

	if (value == (utf8*)"BottomAligned")
	{
		fmt = StaticText::BottomAligned;
	}
	else if (value == (utf8*)"VertCentred")
	{
		fmt = StaticText::VertCentred;
	}
	else
	{
		fmt = StaticText::TopAligned;
	}

	static_cast<StaticText*>(receiver)->setVerticalFormatting(fmt);
}


String	VertScrollbar::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const StaticText*>(receiver)->isVerticalScrollbarEnabled());
}


void	VertScrollbar::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<StaticText*>(receiver)->setVerticalScrollbarEnabled(PropertyHelper::stringToBool(value));
}

String	HorzScrollbar::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const StaticText*>(receiver)->isHorizontalScrollbarEnabled());
}


void	HorzScrollbar::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<StaticText*>(receiver)->setHorizontalScrollbarEnabled(PropertyHelper::stringToBool(value));
}

} // End of  StaticTextProperties namespace section

} // End of  CEGUI namespace section
