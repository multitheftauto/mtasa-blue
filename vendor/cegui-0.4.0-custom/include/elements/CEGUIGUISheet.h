/************************************************************************
    filename:   CEGUIGUISheet.h
    created:    5/6/2004
    author:     Paul D Turner

    purpose:    Interface to a default window
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
#ifndef _CEGUIGUISheet_h_
#define _CEGUIGUISheet_h_

#include "CEGUIWindow.h"
#include "CEGUIWindowFactory.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
    Window class intended to be used as a simple, generic Window.

    This class does no rendering and so appears totally transparent.  This window defaults
    to position 0.0f, 0.0f with a size of 1.0f x 1.0f.

    /note
    The C++ name of this class has been retained for backward compatibility reasons.  The intended usage of
    this window type has now been extended beyond that of a gui-sheet or root window.
*/
class CEGUIEXPORT GUISheet : public Window
{
public:
    /*************************************************************************
        Constants
    *************************************************************************/
    // type name for this widget
    static const String WidgetTypeName;             //!< The unique typename of this widget


    /*************************************************************************
        Construction and Destruction
    *************************************************************************/
    /*!
    \brief
        Constructor for GUISheet windows.
    */
    GUISheet(const String& type, const String& name) : Window(type, name) {}


    /*!
    \brief
        Destructor for GUISheet windows.
    */
    virtual ~GUISheet(void) {}


protected:
    /*!
    \brief
        Perform the actual rendering for this Window.

    \param z
        float value specifying the base Z co-ordinate that should be used when rendering

    \return
        Nothing
    */
    virtual void    drawSelf(float z) {}


    /*!
    \brief
        overridden initialise member to set-up our default state.
    */
    void initialise(void)
    {
        Window::initialise();

        setMaximumSize(Size(1.0f, 1.0f));
        setSize(Size(1.0f, 1.0f));
    }


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
		if (class_name==(const utf8*)"GUISheet")	return true;
		return Window::testClassName_impl(class_name);
	}
};


/*!
\brief
    Factory class for producing default windows
*/
class GUISheetFactory : public WindowFactory
{
public:
    /*************************************************************************
        Construction and Destruction
    *************************************************************************/
    GUISheetFactory(void) : WindowFactory(GUISheet::WidgetTypeName) { }
    ~GUISheetFactory(void){}


    /*!
    \brief
        Create a new Window object of whatever type this WindowFactory produces.

    \param name
        A unique name that is to be assigned to the newly created Window object

    \return
        Pointer to the new Window object.
    */
    Window* createWindow(const String& name)
    {
        return new GUISheet(d_type, name);
    }


    /*!
    \brief
        Destroys the given Window object.

    \param window
        Pointer to the Window object to be destroyed.

    \return
        Nothing.
    */
    virtual void    destroyWindow(Window* window)    { if (window->getType() == d_type) delete window; }
};

/*!
\brief
    typedef for DefaultWindow, which is the new name for GUISheet.
*/
typedef GUISheet DefaultWindow;


} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIGUISheet_h_
