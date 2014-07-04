/************************************************************************
	filename: 	CEGUIWindowFactory.h
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Defines abstract base class for WindowFactory objects
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
#ifndef _CEGUIWindowFactory_h_
#define _CEGUIWindowFactory_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIWindow.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Abstract class that defines the required interface for all WindowFactory objects

	A WindowFactory is used to create and destroy windows of a specific type.  For every
	type of Window object wihin the system (widgets, dialogs, movable windows etc) there
	must be an associated WindowFactory registered with the WindowManager so that the system
	knows how to create and destroy those types of Window base object.
*/
class CEGUIEXPORT WindowFactory
{
public:
	/*!
	\brief
		Create a new Window object of whatever type this WindowFactory produces.

	\param name
		A unique name that is to be assigned to the newly created Window object

	\return
		Pointer to the new Window object.
	*/
	virtual	Window*	createWindow(const String& name) = 0;

	/*!
	\brief
		Destroys the given Window object.

	\param window
		Pointer to the Window object to be destroyed.

	\return
		Nothing.
	*/
	virtual void	destroyWindow(Window* window) = 0;

	/*!
	\brief
		Get the string that describes the type of Window object this WindowFactory produces.

	\return
		String object that contains the unique Window object type produced by this WindowFactory
	*/
	const String& getTypeName(void) const		{return d_type;}

protected:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	WindowFactory(const String& type) : d_type(type) {}

public:		// luabind compatibility
	virtual ~WindowFactory(void) {}

protected:
	/*************************************************************************
		Implementation Data
	*************************************************************************/
	String		d_type;		//!< String holding the type of object created by this factory
};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIWindowFactory_h_
