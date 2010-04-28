/************************************************************************
    filename:   FalTabButton.h
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
#ifndef _FalTabButton_h_
#define _FalTabButton_h_

#include "FalModule.h"
#include "elements/CEGUITabButton.h"
#include "CEGUIWindowFactory.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        TabButton class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States (missing states will default to 'Normal'):
            - Normal    - Rendering for when the tab button is neither selected nor has the mouse hovering over it.
            - Hover     - Rendering for then the tab button has the mouse hovering over it.
            - Selected  - Rendering for when the tab button is the button for the selected tab.
            - Disabled  - Rendering for when the tab button is disabled.
    */
    class FALAGARDBASE_API FalagardTabButton : public TabButton
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardTabButton(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardTabButton();

        // overridden from TabButton base class.
        void drawSelf(float z);

        // implementation of abstract methods in TabButton base class.
        void drawNormal(float z);
        void drawHover(float z);
        void drawPushed(float z);
        void drawDisabled(float z);

    protected:
        void doTabButtonRender(const String& state);
    };

    /*!
    \brief
        WindowFactory for FalagardTabButton type Window objects.
    */
    class FALAGARDBASE_API FalagardTabButtonFactory : public WindowFactory
    {
    public:
        FalagardTabButtonFactory(void) : WindowFactory(FalagardTabButton::WidgetTypeName) { }
        ~FalagardTabButtonFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalTabButton_h_
