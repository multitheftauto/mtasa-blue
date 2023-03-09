/************************************************************************
	filename: 	CEGUITabButton.h
	created:	8/8/2004
	author:		Steve Streeting
	
	purpose:	Interface to base class for TabButton widget
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
#ifndef _CEGUITabButton_h_
#define _CEGUITabButton_h_

#include "CEGUIBase.h"
#include "elements/CEGUIButtonBase.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
    Base class for TabButtons.  A TabButton based class is used internally as
    the button that appears at the top of a TabControl widget to select the
    active tab pane.
*/
class CEGUIEXPORT TabButton : public ButtonBase
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Event name constants
	*************************************************************************/
	// generated internally by Window
	static const String EventClicked;					//!< The button was clicked.

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for base TabButton class
	*/
	TabButton(const String& type, const String& name);


	/*!
	\brief
		Destructor for TabButton class
	*/
	virtual ~TabButton(void);

    /*!
    \brief
        Set whether this tab button is selected or not
    */
    virtual void setSelected(bool selected) { d_selected = selected; requestRedraw(); }

    /*!
    \brief
        Set whether this tab button is on the right of the selected button, 
        used to disable edges of buttons when deselected (to give an overlapping
        look)
    */
    virtual void setRightOfSelected(bool isRight) { d_rightOfSelected = isRight; requestRedraw(); }

    /*!
    \brief
        Return whether this tab button is selected or not
    */
    bool isSelected(void) const { return d_selected; }


    /*!
    \brief
        Set the target window which is the content pane which this button is
        covering.
    */
    void setTargetWindow(Window* wnd);
    /*!
    \brief
        Get the target window which is the content pane which this button is
        covering.
    */
    Window* getTargetWindow(void) { return d_targetWindow; }

    /*!
    \brief
        Set the index at which this tab is positioned.
    */
    void setTabIndex(uint idx) { d_tabIndex = idx; }

    /*!
    \brief
        Get the index at which this tab is positioned.
    */
    uint getTabIndex(void) { return d_tabIndex; }
protected:
    /*************************************************************************
    Implementation Data
    *************************************************************************/
    bool    d_selected;             //!< Is this button selected?
    bool    d_rightOfSelected;      //!< Is this button to the right of the selected tab?
    Window* d_targetWindow;         //!< The target window which this button is representing
    uint    d_tabIndex;             //!< The index at which this tab is positioned
	/*************************************************************************
		New Event Handlers
	*************************************************************************/
	/*!
	\brief
		handler invoked internally when the button is clicked.
	*/
	virtual void	onClicked(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event Handlers
	*************************************************************************/
	virtual void	onMouseButtonUp(MouseEventArgs& e);


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add button specific events
	*/
	void	addTabButtonEvents(bool bCommon=true);
	void	addUncommonEvents( void )							{ __super::addUncommonEvents(); addTabButtonEvents(false); }
	/*!
	\brief
		Perform the rendering for this widget.

	\param z
		float value specifying the base Z co-ordinate that should be used when rendering

	\return
		Nothing
	*/
	void	drawSelf(float z);


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
		if (class_name==(const utf8*)"TabButton")	return true;
		return ButtonBase::testClassName_impl(class_name);
	}
};


} // End of  CEGUI namespace section


#endif	// end of guard _CEGUITabButton_h_
