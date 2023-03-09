/************************************************************************
	filename: 	CEGUITextItemProperties.cpp
	created:	8/4/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
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
#include "elements/CEGUITextItemProperties.h"
#include "elements/CEGUITextItem.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of TextItemProperties namespace section
namespace TextItemProperties
{

String TextXOffset::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const TextItem*>(receiver)->getTextXOffset());
}

void TextXOffset::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<TextItem*>(receiver)->setTextXOffset(PropertyHelper::stringToFloat(value));
}


String TextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const TextItem*>(receiver)->getTextColour());
}


void TextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<TextItem*>(receiver)->setTextColour(PropertyHelper::stringToColour(value));
}


String TextFormatting::get(const PropertyReceiver* receiver) const
{
	switch(static_cast<const TextItem*>(receiver)->getTextFormatting())
	{
	case RightAligned:
		return String((utf8*)"RightAligned");
		break;

	case Centred:
		return String((utf8*)"HorzCentred");
		break;

	case Justified:
		return String((utf8*)"HorzJustified");
		break;

	case WordWrapLeftAligned:
		return String((utf8*)"WordWrapLeftAligned");
		break;

	case WordWrapRightAligned:
		return String((utf8*)"WordWrapRightAligned");
		break;

	case WordWrapCentred:
		return String((utf8*)"WordWrapCentred");
		break;

	case WordWrapJustified:
		return String((utf8*)"WordWrapJustified");
		break;

	default:
		return String((utf8*)"LeftAligned");
		break;
	}
}


void TextFormatting::set(PropertyReceiver* receiver, const String& value)
{
	CEGUI::TextFormatting fmt;

	if (value == (utf8*)"RightAligned")
	{
		fmt = RightAligned;
	}
	else if (value == (utf8*)"HorzCentred")
	{
		fmt = Centred;
	}
	else if (value == (utf8*)"HorzJustified")
	{
		fmt = Justified;
	}
	else if (value == (utf8*)"WordWrapLeftAligned")
	{
		fmt = WordWrapLeftAligned;
	}
	else if (value == (utf8*)"WordWrapRightAligned")
	{
		fmt = WordWrapRightAligned;
	}
	else if (value == (utf8*)"WordWrapCentred")
	{
		fmt = WordWrapCentred;
	}
	else if (value == (utf8*)"WordWrapJustified")
	{
		fmt = WordWrapJustified;
	}
	else
	{
		fmt = LeftAligned;
	}

	static_cast<TextItem*>(receiver)->setTextFormatting(fmt);
}

} // End of  TextItemProperties namespace section
} // End of  CEGUI namespace section
