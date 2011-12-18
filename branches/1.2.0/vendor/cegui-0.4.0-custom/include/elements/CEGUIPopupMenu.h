/************************************************************************
	filename: 	CEGUIPopupMenu.h
	created:	27/3/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Interface to base class for PopupMenu widget
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
#ifndef _CEGUIPopupMenu_h_
#define _CEGUIPopupMenu_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIPopupMenuProperties.h"
#include "elements/CEGUIMenuBase.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class for popup menus
*/
class CEGUIEXPORT PopupMenu : public MenuBase
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Accessor type functions
	*************************************************************************/
	/*!
	\brief
		Get the fade in time for this popup menu.

	\return
		The time in seconds that it takes for the popup to fade in.
		0 if fading is disabled.
	*/
	float	getFadeInTime(void) const			{return d_fadeInTime;}


	/*!
	\brief
		Get the fade out time for this popup menu.

	\return
		The time in seconds that it takes for the popup to fade out.
		0 if fading is disabled.
	*/
	float	getFadeOutTime(void) const			{return d_fadeOutTime;}


	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
		Set the fade in time for this popup menu.

	\param fadetime
		The time in seconds that it takes for the popup to fade in.
		If this parameter is zero, fading is disabled.
	*/
	void	setFadeInTime(float fadetime)		{d_fadeInTime=fadetime;}


	/*!
	\brief
		Set the fade out time for this popup menu.

	\param fadetime
		The time in seconds that it takes for the popup to fade out.
		If this parameter is zero, fading is disabled.
	*/
	void	setFadeOutTime(float fadetime)		{d_fadeOutTime=fadetime;}

	/*!
	\brief
		Tells the popup menu to open.
	*/
	void	openPopupMenu(void);


	/*!
	\brief
		Tells the popup menu to close.
	*/
	void	closePopupMenu(void);


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for PopupMenu objects
	*/
	PopupMenu(const String& type, const String& name);


	/*!
	\brief
		Destructor for PopupMenu objects
	*/
	virtual ~PopupMenu(void);


protected:
	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
	Perform actual update processing for this Window.

	\param elapsed
	float value indicating the number of seconds elapsed since the last update call.

	\return
	Nothing.
	*/
	virtual void	updateSelf(float elapsed);


	/*!
	\brief
		Setup size and position for the item widgets attached to this Listbox

	\return
		Nothing.
	*/
	virtual void	layoutItemWidgets(void);


	/*!
	\brief
		Resizes the popup menu to exactly fit the content that is attached to it.

	\return
		Nothing.
	*/
	virtual Size getContentSize(void);


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
		if (class_name==(const utf8*)"PopupMenu")	return true;
		return MenuBase::testClassName_impl(class_name);
	}


	/*************************************************************************
		Overridden event handlers
	*************************************************************************/
	virtual void onAlphaChanged(WindowEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	float d_origAlpha;			//!< The original alpha of this window.

	float d_fadeElapsed;		//!< The time in seconds this popup menu has been fading.
	float d_fadeOutTime;		//!< The time in seconds it takes for this popup menu to fade out.
	float d_fadeInTime;			//!< The time in seconds it takes for this popup menu to fade in.
	bool d_fading;				//!< true if this popup menu is fading in/out. false if not
	bool d_fadingOut;			//!< true if this popup menu is fading out. false if fading in.

private:
	/*************************************************************************
	Static Properties for this class
	*************************************************************************/
	static PopupMenuProperties::FadeInTime	d_fadeInTimeProperty;
	static PopupMenuProperties::FadeOutTime	d_fadeOutTimeProperty;


	/*************************************************************************
	Private methods
	*************************************************************************/
	void	addPopupMenuProperties( bool bCommon = true );
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addPopupMenuProperties(false); }
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif


#endif	// end of guard _CEGUIPopupMenu_h_
