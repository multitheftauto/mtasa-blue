/************************************************************************
	filename: 	CEGUIComboboxProperties.h
	created:	11/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to Combobox property classes
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
#ifndef _CEGUIComboboxProperties_h_
#define _CEGUIComboboxProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ComboboxProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the Combobox class
*/
namespace ComboboxProperties
{
/*!
\brief
	Property to access the read-only setting of the edit box.

	\par Usage:
		- Name: ReadOnly
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate the edit box is read-only.
		- "False" to indicate the edit box is not read-only (text may be edited by user).
*/
class ReadOnly : public Property
{
public:
	ReadOnly() : Property(
		"ReadOnly",
		"Property to get/set the read-only setting for the Editbox.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the string used for regular expression validation of the edit box text.

	\par Usage:
		- Name: ValidationString
		- Format: "[text]"

	\par Where:
		- [Text] is the string used for validating text entry.
*/
class ValidationString : public Property
{
public:
	ValidationString() : Property(
		"ValidationString",
		"Property to get/set the validation string Editbox.  Value is a text string.",
		".*")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the current carat index.

	\par Usage:
		- Name: CaratIndex
		- Format: "[uint]"

	\par Where:
		- [uint] is the zero based index of the carat position within the text.
*/
class CaratIndex : public Property
{
public:
	CaratIndex() : Property(
		"CaratIndex",
		"Property to get/set the current carat index.  Value is \"[uint]\".",
		"0")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the current selection start index.

	\par Usage:
		- Name: EditSelectionStart
		- Format: "[uint]"

	\par Where:
		- [uint] is the zero based index of the selection start position within the text.
*/
class EditSelectionStart : public Property
{
public:
	EditSelectionStart() : Property(
		"EditSelectionStart",
		"Property to get/set the zero based index of the selection start position within the text.  Value is \"[uint]\".",
		"0")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the current selection length.

	\par Usage:
		- Name: EditSelectionLength
		- Format: "[uint]"

	\par Where:
		- [uint] is the length of the selection (as a count of the number of code points selected).
*/
class EditSelectionLength : public Property
{
public:
	EditSelectionLength() : Property(
		"EditSelectionLength",
		"Property to get/set the length of the selection (as a count of the number of code points selected).  Value is \"[uint]\".",
		"0")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the maximum text length for the edit box.

	\par Usage:
		- Name: MaxEditTextLength
		- Format: "[uint]"

	\par Where:
		- [uint] is the maximum allowed text length (as a count of code points).
*/
class MaxEditTextLength : public Property
{
public:
	MaxEditTextLength() : Property(
		"MaxEditTextLength",
		"Property to get/set the the maximum allowed text length (as a count of code points).  Value is \"[uint]\".",
		"1073741824")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the normal, unselected, text colour used for rendering text.

	\par Usage:
		- Name: NormalEditTextColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class NormalEditTextColour : public Property
{
public:
	NormalEditTextColour() : Property(
		"NormalEditTextColour",
		"Property to get/set the normal, unselected, text colour used for rendering text.  Value is \"aarrggbb\" (hex).",
		"00FFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the colour used for rendering text within the selection area.

	\par Usage:
		- Name: SelectedEditTextColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class SelectedEditTextColour : public Property
{
public:
	SelectedEditTextColour() : Property(
		"SelectedEditTextColour",
		"Property to get/set the colour used for rendering text within the selection area.  Value is \"aarrggbb\" (hex).",
		"00000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the colour used for rendering the selection highlight when the edit box is active.

	\par Usage:
		- Name: ActiveEditSelectionColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class ActiveEditSelectionColour : public Property
{
public:
	ActiveEditSelectionColour() : Property(
		"ActiveEditSelectionColour",
		"Property to get/set the colour used for rendering the selection highlight when the edit box is active.  Value is \"aarrggbb\" (hex).",
		"006060FF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the colour used for rendering the selection highlight when the edit box is inactive.

	\par Usage:
		- Name: InactiveEditSelectionColour
		- Format: "aarrggbb".

	\par Where:
		- aarrggbb is the ARGB colour value to be used.
*/
class InactiveEditSelectionColour : public Property
{
public:
	InactiveEditSelectionColour() : Property(
		"InactiveEditSelectionColour",
		"Property to get/set the colour used for rendering the selection highlight when the edit box is inactive.  Value is \"aarrggbb\" (hex).",
		"00808080")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the sort setting of the list box.

	\par Usage:
		- Name: SortList
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate the list items should be sorted.
		- "False" to indicate the list items should not be sorted.
*/
class SortList : public Property
{
public:
	SortList() : Property(
		"SortList",
		"Property to get/set the sort setting of the list box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the 'always show' setting for the vertical scroll bar of the list box.

	\par Usage:
		- Name: ForceVertScrollbar
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate that the vertical scroll bar will always be shown.
		- "False" to indicate that the vertical scroll bar will only be shown when it is needed.
*/
class ForceVertScrollbar : public Property
{
public:
	ForceVertScrollbar() : Property(
		"ForceVertScrollbar",
		"Property to get/set the 'always show' setting for the vertical scroll bar of the list box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the 'always show' setting for the horizontal scroll bar of the list box.

	\par Usage:
		- Name: ForceHorzScrollbar
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate that the horizontal scroll bar will always be shown.
		- "False" to indicate that the horizontal scroll bar will only be shown when it is needed.
*/
class ForceHorzScrollbar : public Property
{
public:
	ForceHorzScrollbar() : Property(
		"ForceHorzScrollbar",
		"Property to get/set the 'always show' setting for the horizontal scroll bar of the list box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the 'single click mode' setting for the combo box.

	\par Usage:
		- Name: SingleClickMode
		- Format: "[text]"

	\par Where [Text] is:
		- "True" to indicate that the box will operate in single click mode
		- "False" to indicate that the box will not operate in single click mode
*/
class SingleClickMode : public Property
{
public:
	SingleClickMode() : Property(
		"SingleClickMode",
		"Property to get/set the 'single click mode' setting for the combo box.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


} // End of  ComboboxProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIComboboxProperties_h_
