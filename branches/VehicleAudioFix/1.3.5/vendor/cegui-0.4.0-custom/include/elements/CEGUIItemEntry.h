/************************************************************************
	filename: 	CEGUIItemEntry.h
	created:	31/3/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Interface to base class for ItemEntry widget
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
#ifndef _CEGUIItemEntry_h_
#define _CEGUIItemEntry_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class for item type widgets.
*/
class CEGUIEXPORT ItemEntry : public Window
{
public:
	/*************************************************************************
		Abstract Implementation Functions (must be provided by derived class)
	*************************************************************************/
	/*!
	\brief
		Return the "optimal" size for the item
	
	\return
		Size describing the size in pixel that this ItemEntry's content requires
		for non-clipped rendering
	*/
	virtual Size getItemPixelSize(void) = 0;

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for ItemEntry objects
	*/
	ItemEntry(const String& type, const String& name);


	/*!
	\brief
		Destructor for ItemEntry objects
	*/
	virtual ~ItemEntry(void);


protected:
	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add itementry specific events
	*/
	void	addItemEntryEvents(void);


	/*!
    \brief
        Perform the actual rendering for this Window.

    \return
        Nothing
    */
    virtual void    populateRenderCahce() {};


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
		if (class_name==(const utf8*)"ItemEntry")	return true;
		return Window::testClassName_impl(class_name);
	}
   
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif


#endif	// end of guard _CEGUIItemEntry_h_
