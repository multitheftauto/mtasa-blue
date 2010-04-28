/************************************************************************
	filename: 	CEGUICheckbox.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for Checkbox Widget
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
#ifndef _CEGUICheckbox_h_
#define _CEGUICheckbox_h_

#include "CEGUIBase.h"
#include "elements/CEGUIButtonBase.h"
#include "elements/CEGUICheckboxProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class providing logic for Check-box widgets
*/
class CEGUIEXPORT Checkbox : public ButtonBase
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Event name constants
	*************************************************************************/
	// generated internally by Window
	static const String EventCheckStateChanged;			//!< The check-state of the widget has changed.


	/*************************************************************************
		Accessor Functions
	*************************************************************************/
	/*!
	\brief
		return true if the check-box is selected (has the checkmark)

	\return
		true if the widget is selected and has the check-mark, false if the widget
		is not selected and does not have the check-mark.
	*/
	bool	isSelected(void) const						{return d_selected;}


	/*************************************************************************
		Manipulator Functions
	*************************************************************************/
	/*!
	\brief
		set whether the check-box is selected or not

	\param select
		true to select the widget and give it the check-mark.  false to de-select the widget and
		remove the check-mark.

	\return
		Nothing.
	*/
	void	setSelected(bool select);


	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Checkbox class.
	*/
	Checkbox(const String& type, const String& name);


	/*!
	\brief
		Destructor for Checkbox class.
	*/
	virtual ~Checkbox(void);


protected:
	/*************************************************************************
		New event handlers
	*************************************************************************/
	/*!
	\brief
		event triggered internally when state of check-box changes
	*/
	virtual void	onSelectStateChange(WindowEventArgs& e);


	/*************************************************************************
		Overridden event handlers
	*************************************************************************/
	virtual void	onMouseButtonUp(MouseEventArgs& e);


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add check-box specific events
	*/
	void	addCheckboxEvents(bool bCommon=true);
	void	addUncommonEvents( void )							{ __super::addUncommonEvents(); addCheckboxEvents(false); }


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
		if (class_name==(const utf8*)"Checkbox")	return true;
		return ButtonBase::testClassName_impl(class_name);
	}


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	bool		d_selected;					//!< true if check-box is selected (has checkmark)

private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static CheckboxProperties::Selected	d_selectedProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addCheckboxProperties( bool bCommon = true );
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addCheckboxProperties(false); }
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUICheckbox_h_
