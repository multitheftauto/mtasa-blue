/************************************************************************
	filename: 	CEGUIMenuItemProperties.cpp
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
#include "elements/CEGUIMenuItemProperties.h"
#include "elements/CEGUIMenuItem.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of MenuItemProperties namespace section
namespace MenuItemProperties
{


String HoverColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MenuItem*>(receiver)->getHoverColour());
}

void HoverColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuItem*>(receiver)->setHoverColour(PropertyHelper::stringToColour(value));
}




String PushedColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MenuItem*>(receiver)->getPushedColour());
}

void PushedColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuItem*>(receiver)->setPushedColour(PropertyHelper::stringToColour(value));
}




String OpenedColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MenuItem*>(receiver)->getOpenedColour());
}

void OpenedColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuItem*>(receiver)->setOpenedColour(PropertyHelper::stringToColour(value));
}




String NormalTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MenuItem*>(receiver)->getNormalTextColour());
}

void NormalTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuItem*>(receiver)->setNormalTextColour(PropertyHelper::stringToColour(value));
}




String DisabledTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MenuItem*>(receiver)->getDisabledTextColour());
}

void DisabledTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuItem*>(receiver)->setDisabledTextColour(PropertyHelper::stringToColour(value));
}


} // End of  MenuItemProperties namespace section
} // End of  CEGUI namespace section
