/************************************************************************
    filename:   FalThumb.h
    created:    Sun Jul 3 2005
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
#ifndef _FalThumb_h_
#define _FalThumb_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIThumb.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Thumb class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States (missing states will default to 'Normal'):
            - Normal    - Rendering for when the thumb is neither pushed or has the mouse hovering over it.
            - Hover     - Rendering for then the thumb has the mouse hovering over it.
            - Pushed    - Rendering for when the thumb is pushed.
            - Disabled  - Rendering for when the thumb is disabled.
    */
    class FALAGARDBASE_API FalagardThumb : public Thumb
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardThumb(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardThumb();

    protected:
        // overridden from ButtonBase.
        void drawSelf(float z);

        // implementation of abstract methods in Thumb base class
        void drawNormal(float z);
        void drawHover(float z);
        void drawPushed(float z);
        void drawDisabled(float z);

        void doThumbRender(const String& state);

    };

    /*!
    \brief
        WindowFactory for FalagardThumb type Window objects.
    */
    class FALAGARDBASE_API FalagardThumbFactory : public WindowFactory
    {
    public:
        FalagardThumbFactory(void) : WindowFactory(FalagardThumb::WidgetTypeName) { }
        ~FalagardThumbFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalThumb_h_
