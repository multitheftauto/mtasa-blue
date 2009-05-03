/************************************************************************
	filename: 	CEGUIComboDropList.h
	created:	13/6/2004
	author:		Paul D Turner
	
	purpose:	Interface for the Combobox Drop-List widget base class
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
#ifndef _CEGUIComboDropList_h_
#define _CEGUIComboDropList_h_

#include "elements/CEGUIListbox.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Base class for the combo box drop down list.  This is a specialisation of the Listbox class.
*/
class CEGUIEXPORT ComboDropList : public Listbox
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Constants
	*************************************************************************/
	// Event names
	static const String EventListSelectionAccepted;		//!< Event fired when the user confirms the selection by clicking the mouse.


	/*!
	\brief
		Initialise the Window based object ready for use.

	\note
		This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	\return
		Nothing
	*/
	virtual void	initialise(void);


	/*!
	\brief
		Set whether the drop-list is 'armed' for selection.

	\note
		This setting is not exclusively under client control; the ComboDropList will auto-arm in
		response to certain left mouse button events.  This is also dependant upon the autoArm
		setting of the ComboDropList.

	\param setting
		- true to arm the box; items will be highlighted and the next left button up event
		will cause dismissal and possible item selection.

		- false to disarm the box; items will not be highlighted or selected until the box is armed.

	\return
		Nothing.
	*/
	void	setArmed(bool setting)		{ d_armed = setting; }


	/*!
	\brief
		Return the 'armed' state of the ComboDropList.

	\return
		- true if the box is armed; items will be highlighted and the next left button up event
		will cause dismissal and possible item selection.

		- false if the box is not armed; items will not be highlighted or selected until the box is armed.
	*/
	bool	isArmed(void) const		{ return d_armed; }


	/*!
	\brief
		Set the mode of operation for the ComboDropList.

	\param setting
		- true if the ComboDropList auto-arms when the mouse enters the box.
		- false if the user must click to arm the box.

	\return
		Nothing.
	*/
	void	setAutoArmEnabled(bool setting)		{ d_autoArm = setting; }


	/*!
	\brief
		returns the mode of operation for the drop-list

	\return
		- true if the ComboDropList auto-arms when the mouse enters the box.
		- false if the user must click to arm the box.
	*/
	bool	isAutoArmEnabled(void) const		{ return d_autoArm; }


	/*************************************************************************
		Constructor & Destructor
	*************************************************************************/
	/*!
	\brief
		Constructor for ComboDropList base class
	*/
	ComboDropList(const String& type, const String& name);


	/*!
	\brief
		Destructor for ComboDropList base class
	*/
	virtual ~ComboDropList(void);


protected:
	/*!
	\brief
		Add drop-list specific events
	*/
	void	addComboDropListEvents(void);


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
		if (class_name==(const utf8*)"ComboDropList")	return true;
		return Listbox::testClassName_impl(class_name);
	}
	
	/*************************************************************************
		New event handlers
	*************************************************************************/
	/*!
	\brief
		Handler for when list selection is confirmed.
	*/
	void	onListSelectionAccepted(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event handling
	*************************************************************************/
	virtual void	onMouseMove(MouseEventArgs& e);
	virtual void	onMouseButtonDown(MouseEventArgs& e);
	virtual void	onMouseButtonUp(MouseEventArgs& e);
	virtual void	onCaptureLost(WindowEventArgs& e);
	virtual void	onActivated(ActivationEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	bool	d_autoArm;		//!< true if the box auto-arms when the mouse enters it.
	bool	d_armed;		//!< true when item selection has been armed.
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIComboDropList_h_
