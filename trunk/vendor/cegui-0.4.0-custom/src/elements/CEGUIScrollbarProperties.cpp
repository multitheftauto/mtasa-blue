/************************************************************************
	filename: 	CEGUIScrollbarProperties.cpp
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Implements properties for the scroll bar.
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
#include "elements/CEGUIScrollbarProperties.h"
#include "elements/CEGUIScrollbar.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ScrollbarProperties namespace section
namespace ScrollbarProperties
{
String	DocumentSize::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const Scrollbar*>(receiver)->getDocumentSize());
}


void	DocumentSize::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Scrollbar*>(receiver)->setDocumentSize(PropertyHelper::stringToFloat(value));
}


String	PageSize::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const Scrollbar*>(receiver)->getPageSize());
}


void	PageSize::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Scrollbar*>(receiver)->setPageSize(PropertyHelper::stringToFloat(value));
}


String	StepSize::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const Scrollbar*>(receiver)->getStepSize());
}


void	StepSize::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Scrollbar*>(receiver)->setStepSize(PropertyHelper::stringToFloat(value));
}


String	OverlapSize::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const Scrollbar*>(receiver)->getOverlapSize());
}


void	OverlapSize::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Scrollbar*>(receiver)->setOverlapSize(PropertyHelper::stringToFloat(value));
}


String	ScrollPosition::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const Scrollbar*>(receiver)->getScrollPosition());
}


void	ScrollPosition::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Scrollbar*>(receiver)->setScrollPosition(PropertyHelper::stringToFloat(value));
}

} // End of  ScrollbarProperties namespace section

} // End of  CEGUI namespace section
