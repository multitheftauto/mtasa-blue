/************************************************************************
	filename: 	CEGUIEditboxProperties.cpp
	created:	9/7/2004
	author:		Paul D Turner
	
	purpose:	Implements properties for the Editbox class
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
#include "elements/CEGUIEditboxProperties.h"
#include "elements/CEGUIEditbox.h"
#include "CEGUIPropertyHelper.h"
#include "CEGUIExceptions.h"
#include <cstdlib>
#include <cstdio>


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of EditboxProperties namespace section
namespace EditboxProperties
{

String ReadOnly::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Editbox*>(receiver)->isReadOnly());
}


void ReadOnly::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setReadOnly(PropertyHelper::stringToBool(value));
}


String MaskText::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Editbox*>(receiver)->isTextMasked());
}


void MaskText::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setTextMasked(PropertyHelper::stringToBool(value));
}


String MaskCodepoint::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<const Editbox*>(receiver)->getMaskCodePoint());
}


void MaskCodepoint::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setMaskCodePoint(PropertyHelper::stringToUint(value));
}


String ValidationString::get(const PropertyReceiver* receiver) const
{
	return static_cast<const Editbox*>(receiver)->getValidationString();
}


void ValidationString::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setValidationString(value);
}


String CaratIndex::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<uint>(static_cast<const Editbox*>(receiver)->getCaratIndex()));
}


void CaratIndex::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setCaratIndex(PropertyHelper::stringToUint(value));
}


String SelectionStart::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<uint>(static_cast<const Editbox*>(receiver)->getSelectionStartIndex()));
}


void SelectionStart::set(PropertyReceiver* receiver, const String& value)
{
	Editbox* eb = static_cast<Editbox*>(receiver);
	uint selStart = PropertyHelper::stringToUint(value);
	eb->setSelection(selStart, selStart + eb->getSelectionLength());
}


String SelectionLength::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<uint>(static_cast<const Editbox*>(receiver)->getSelectionLength()));
}


void SelectionLength::set(PropertyReceiver* receiver, const String& value)
{
	Editbox* eb = static_cast<Editbox*>(receiver);
	uint selLen = PropertyHelper::stringToUint(value);
	eb->setSelection(eb->getSelectionStartIndex(), eb->getSelectionStartIndex() + selLen);
}


String MaxTextLength::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<uint>(static_cast<const Editbox*>(receiver)->getMaxTextLength()));
}


void MaxTextLength::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setMaxTextLength(PropertyHelper::stringToUint(value));
}


String NormalTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const Editbox*>(receiver)->getNormalTextColour());
}


void NormalTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setNormalTextColour(PropertyHelper::stringToColour(value));
}


String SelectedTextColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const Editbox*>(receiver)->getSelectedTextColour());
}


void SelectedTextColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setSelectedTextColour(PropertyHelper::stringToColour(value));
}


String ActiveSelectionColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const Editbox*>(receiver)->getNormalSelectBrushColour());
}


void ActiveSelectionColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setNormalSelectBrushColour(PropertyHelper::stringToColour(value));
}


String InactiveSelectionColour::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::colourToString(static_cast<const Editbox*>(receiver)->getInactiveSelectBrushColour());
}


void InactiveSelectionColour::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Editbox*>(receiver)->setInactiveSelectBrushColour(PropertyHelper::stringToColour(value));
}

} // End of  EditboxProperties namespace section

} // End of  CEGUI namespace section
