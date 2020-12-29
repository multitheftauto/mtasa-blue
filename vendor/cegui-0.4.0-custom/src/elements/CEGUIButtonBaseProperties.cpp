/************************************************************************
	filename: 	CEGUIButtonBaseProperties.cpp
	created:	9/7/2004
	author:		Paul D Turner
	
	purpose:	Implements properties for button base class
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
#include "elements/CEGUIButtonBaseProperties.h"
#include "elements/CEGUIButtonBase.h"
#include "CEGUIPropertyHelper.h"
#include "CEGUIExceptions.h"
#include <cstdlib>
#include <cstdio>


// Start of CEGUI namespace section
namespace CEGUI
{
// Start of ButtonBaseProperties namespace section
namespace ButtonBaseProperties
{

String NormalTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const ButtonBase*>(receiver)->getNormalTextColour());
}


void NormalTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ButtonBase*>(receiver)->setNormalTextColour(PropertyHelper::stringToColour(value));
}


String	HoverTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const ButtonBase*>(receiver)->getHoverTextColour());
}


void	HoverTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ButtonBase*>(receiver)->setHoverTextColour(PropertyHelper::stringToColour(value));
}


String	PushedTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const ButtonBase*>(receiver)->getPushedTextColour());
}


void	PushedTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ButtonBase*>(receiver)->setPushedTextColour(PropertyHelper::stringToColour(value));
}


String DisabledTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const ButtonBase*>(receiver)->getDisabledTextColour());
}


void DisabledTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<ButtonBase*>(receiver)->setDisabledTextColour(PropertyHelper::stringToColour(value));
}


} // End of  ButtonBaseProperties namespace section

} // End of  CEGUI namespace section
