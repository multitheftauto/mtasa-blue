/************************************************************************
    filename:   FalMenuItem.h
    created:    Fri Jul 8 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#ifndef _FalMenuItem_h_
#define _FalMenuItem_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIMenuItem.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        MenuItem class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - EnabledNormal
            - EnabledHover
            - EnabledPushed
            - EnabledPopupOpen
            - DisabledNormal
            - DisabledHover
            - DisabledPushed
            - DisabledPopupOpen
            - PopupClosedIcon   - Additional state drawn when item has a pop-up attached (in closed state)
            - PopupOpenIcon     - Additional state drawn when item has a pop-up attached (in open state)
    */
    class FALAGARDBASE_API FalagardMenuItem : public MenuItem
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardMenuItem(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardMenuItem();

    protected:
        // overridden from MenuItem base class.
        void populateRenderCache();
    };

    /*!
    \brief
        WindowFactory for FalagardMenuItem type Window objects.
    */
    class FALAGARDBASE_API FalagardMenuItemFactory : public WindowFactory
    {
    public:
        FalagardMenuItemFactory(void) : WindowFactory(FalagardMenuItem::WidgetTypeName) { }
        ~FalagardMenuItemFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalMenuItem_h_
