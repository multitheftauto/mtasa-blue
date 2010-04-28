/************************************************************************
	filename: 	CEGUIMenuBaseProperties.cpp
	created:	5/4/2005
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
#include "elements/CEGUIMenuBaseProperties.h"
#include "elements/CEGUIMenuBase.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of MenuBaseProperties namespace section
namespace MenuBaseProperties
{

String ItemSpacing::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const MenuBase*>(receiver)->getItemSpacing());
}

void ItemSpacing::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuBase*>(receiver)->setItemSpacing(PropertyHelper::stringToFloat(value));
}


String HorzPadding::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const MenuBase*>(receiver)->getHorzPadding());
}

void HorzPadding::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuBase*>(receiver)->setHorzPadding(PropertyHelper::stringToFloat(value));
}


String VertPadding::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const MenuBase*>(receiver)->getVertPadding());
}

void VertPadding::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuBase*>(receiver)->setVertPadding(PropertyHelper::stringToFloat(value));
}


String BorderWidth::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const MenuBase*>(receiver)->getBorderWidth());
}

void BorderWidth::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuBase*>(receiver)->setBorderWidth(PropertyHelper::stringToFloat(value));
}


String BackgroundColours::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourRectToString(static_cast<const MenuBase*>(receiver)->getBackgroundColours());
}

void BackgroundColours::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MenuBase*>(receiver)->setBackgroundColours(PropertyHelper::stringToColourRect(value));
}

String AllowMultiplePopups::get(const PropertyReceiver* receiver) const
{
    return PropertyHelper::boolToString(static_cast<const MenuBase*>(receiver)->isMultiplePopupsAllowed());
}

void AllowMultiplePopups::set(PropertyReceiver* receiver, const String& value)
{
    static_cast<MenuBase*>(receiver)->setAllowMultiplePopups(PropertyHelper::stringToBool(value));
}

} // End of  MenuBaseProperties namespace section
} // End of  CEGUI namespace section
