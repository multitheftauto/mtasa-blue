/************************************************************************
	filename: 	CEGUIMenubar.h
	created:	27/3/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Interface to base class for Menubar widget
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
#ifndef _CEGUIMenubar_h_
#define _CEGUIMenubar_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
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
	Base class for menu bars.
*/
class CEGUIEXPORT Menubar : public MenuBase
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Menubar objects
	*/
	Menubar(const String& type, const String& name);


	/*!
	\brief
		Destructor for Menubar objects
	*/
	virtual ~Menubar(void);


protected:
	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Setup size and position for the item widgets attached to this Menubar

	\return
		Nothing.
	*/
	virtual void	layoutItemWidgets();


	/*!
	\brief
		Resizes the menubar to exactly fit the content that is attached to it.

	\return
		Nothing.
	*/
	virtual Size getContentSize();


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
		if (class_name==(const utf8*)"Menubar")	return true;
		return MenuBase::testClassName_impl(class_name);
	}

};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIMenubar_h_
