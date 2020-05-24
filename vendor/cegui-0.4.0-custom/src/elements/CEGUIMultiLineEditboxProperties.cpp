/************************************************************************
	filename: 	CEGUIMultiLineEditboxProperties.cpp
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Implements properties for the MultiLineEditbox class
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
#include "elements/CEGUIMultiLineEditboxProperties.h"
#include "elements/CEGUIMultiLineEditbox.h"
#include "CEGUIPropertyHelper.h"
#include "CEGUIExceptions.h"
#include "CEGUIFont.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of MultiLineEditboxProperties namespace section
namespace MultiLineEditboxProperties
{
String ReadOnly::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const MultiLineEditbox*>(receiver)->isReadOnly());
}


void ReadOnly::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiLineEditbox*>(receiver)->setReadOnly(PropertyHelper::stringToBool(value));
}


String WordWrap::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const MultiLineEditbox*>(receiver)->isWordWrapped());
}


void WordWrap::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiLineEditbox*>(receiver)->setWordWrapping(PropertyHelper::stringToBool(value));
}


String CaratIndex::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<uint>(static_cast<const MultiLineEditbox*>(receiver)->getCaratIndex()));
}


void CaratIndex::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiLineEditbox*>(receiver)->setCaratIndex(PropertyHelper::stringToUint(value));
}


String SelectionStart::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<uint>(static_cast<const MultiLineEditbox*>(receiver)->getSelectionStartIndex()));
}


void SelectionStart::set(PropertyReceiver* receiver, const String& value)
{
	MultiLineEditbox* eb = static_cast<MultiLineEditbox*>(receiver);
	uint selStart = PropertyHelper::stringToUint(value);
	eb->setSelection(selStart, selStart + eb->getSelectionLength());
}


String SelectionLength::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<uint>(static_cast<const MultiLineEditbox*>(receiver)->getSelectionLength()));
}


void SelectionLength::set(PropertyReceiver* receiver, const String& value)
{
	MultiLineEditbox* eb = static_cast<MultiLineEditbox*>(receiver);
	uint selLen = PropertyHelper::stringToUint(value);
	eb->setSelection(eb->getSelectionStartIndex(), eb->getSelectionStartIndex() + selLen);
}


String MaxTextLength::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<uint>(static_cast<const MultiLineEditbox*>(receiver)->getMaxTextLength()));
}


void MaxTextLength::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiLineEditbox*>(receiver)->setMaxTextLength(PropertyHelper::stringToUint(value));
}


String NormalTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MultiLineEditbox*>(receiver)->getNormalTextColour());
}


void NormalTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiLineEditbox*>(receiver)->setNormalTextColour(PropertyHelper::stringToColour(value));
}


String SelectedTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MultiLineEditbox*>(receiver)->getSelectedTextColour());
}


void SelectedTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiLineEditbox*>(receiver)->setSelectedTextColour(PropertyHelper::stringToColour(value));
}


String ActiveSelectionColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MultiLineEditbox*>(receiver)->getNormalSelectBrushColour());
}


void ActiveSelectionColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiLineEditbox*>(receiver)->setNormalSelectBrushColour(PropertyHelper::stringToColour(value));
}


String InactiveSelectionColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const MultiLineEditbox*>(receiver)->getInactiveSelectBrushColour());
}


void InactiveSelectionColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<MultiLineEditbox*>(receiver)->setInactiveSelectBrushColour(PropertyHelper::stringToColour(value));
}

} // End of  MultiLineEditboxProperties namespace section

} // End of  CEGUI namespace section
