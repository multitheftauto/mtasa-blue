/************************************************************************
	filename: 	CEGUIRadioButton.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for RadioButton widget
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
#ifndef _CEGUIRadioButton_h_
#define _CEGUIRadioButton_h_

#include "CEGUIBase.h"
#include "elements/CEGUIButtonBase.h"
#include "elements/CEGUIRadioButtonProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Base class to provide the logic for Radio Button widgets.

*/
class CEGUIEXPORT RadioButton : public ButtonBase
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Event name constants
	*************************************************************************/
	// generated internally by Window
	static const String EventSelectStateChanged;			//!< The selected state of the widget has changed.


	/*************************************************************************
		Accessor Functions
	*************************************************************************/ 
	/*!
	\brief
		return true if the radio button is selected (has the checkmark)

	\return
		true if this widget is selected, false if the widget is not selected.
	*/
	bool	isSelected(void) const				{return d_selected;}

	
	/*!
	\brief
		return the groupID assigned to this radio button

	\return
		ulong value that identifies the Radio Button group this widget belongs to.
	*/
	ulong	getGroupID(void) const				{return d_groupID;}


	/*!
	\brief
		Return a pointer to the RadioButton object within the same group as this RadioButton, that
		is currently selected.

	\return
		Pointer to the RadioButton object that is the RadioButton within the same group as this RadioButton,
		and is attached to the same parent window as this RadioButton, that is currently selected.
		Returns NULL if no button within the group is selected, or if 'this' is not attached to a parent window.
	*/
	RadioButton*	getSelectedButtonInGroup(void) const;


	/*************************************************************************
		Manipulator Functions
	*************************************************************************/
	/*!
	\brief
		set whether the radio button is selected or not

	\param select
		true to put the radio button in the selected state, false to put the radio button in the
		deselected state.  If changing to the selected state, any previously selected radio button
		within the same group is automatically deselected.

	\return
		Nothing.
	*/
	void	setSelected(bool select);

	
	/*!
	\brief
		set the groupID for this radio button

	\param group
		ulong value specifying the radio button group that this widget belongs to.

	\return	
		Nothing.
	*/
	void	setGroupID(ulong group);


	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	RadioButton(const String& type, const String& name);
	virtual ~RadioButton(void);


protected:
	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add radio button specific events
	*/
	void	addRadioButtonEvents(bool bCommon=true);
	void	addUncommonEvents( void )							{ __super::addUncommonEvents(); addRadioButtonEvents(false); }


	/*!
	\brief
		Deselect any selected radio buttons attached to the same parent within the same group
		(but not do not deselect 'this').
	*/
	void	deselectOtherButtonsInGroup(void) const;


	/*!
	\brief
		Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

	\param class_name
		The class name that is to be checked.

	\return
		true if this window was inherited from \a class_name. false if not.
	*/
	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"RadioButton")	return true;
		return ButtonBase::testClassName_impl(class_name);
	}


	/*************************************************************************
		New Radio Button Events
	*************************************************************************/
	/*!
	\brief
		event triggered internally when the select state of the button changes.
	*/
	virtual void	onSelectStateChanged(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event handlers
	*************************************************************************/
	virtual void	onMouseButtonUp(MouseEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	bool		d_selected;				// true when radio button is selected (has checkmark)
	ulong		d_groupID;				// radio button group ID


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static RadioButtonProperties::Selected	d_selectedProperty;
	static RadioButtonProperties::GroupID	d_groupIDProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addRadioButtonProperties( bool bCommon = true );
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addRadioButtonProperties(false); }
};


} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIRadioButton_h_
