/************************************************************************
    filename:   FalFrameWindow.h
    created:    Sat Jul 2 2005
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
#ifndef _FalFrameWindow_h_
#define _FalFrameWindow_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIFrameWindow.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        FrameWindow class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - ActiveWithTitleWithFrame
            - InactiveWithTitleWithFrame
            - DisabledWithTitleWithFrame
            - ActiveWithTitleNoFrame
            - InactiveWithTitleNoFrame
            - DisabledWithTitleNoFrame
            - ActiveNoTitleWithFrame
            - InactiveNoTitleWithFrame
            - DisabledNoTitleWithFrame
            - ActiveNoTitleNoFrame
            - InactiveNoTitleNoFrame
            - DisabledNoTitleNoFrame

        Named Areas:
            - ClientWithTitleWithFrame
            - ClientWithTitleNoFrame
            - ClientNoTitleWithFrame
            - ClientNoTitleNoFrame
    */
    class FALAGARDBASE_API FalagardFrameWindow : public FrameWindow
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardFrameWindow(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardFrameWindow();

        // overridden from Window base class.
        Rect getUnclippedInnerRect(void) const;

    protected:
        // overridden from FrameWindow base class.
        void populateRenderCache();
        Titlebar* createTitlebar(const String& name) const;
        PushButton* createCloseButton(const String& name) const;
    };

    /*!
    \brief
        WindowFactory for FalagardFrameWindow type Window objects.
    */
    class FALAGARDBASE_API FalagardFrameWindowFactory : public WindowFactory
    {
    public:
        FalagardFrameWindowFactory(void) : WindowFactory(FalagardFrameWindow::WidgetTypeName) { }
        ~FalagardFrameWindowFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalFrameWindow_h_
