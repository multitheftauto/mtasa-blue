/************************************************************************
    filename:   FalScrollablePane.h
    created:    Thu Jul 7 2005
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
#ifndef _FalScrollablePane_h_
#define _FalScrollablePane_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIScrollablePane.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        ScrollablePane class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled
            - Disabled

        Named Areas:
            - ViewableArea         - area where content will be drawn when no scrollbars are visible.
            - ViewableAreaHScroll  - area where content will be drawn when the horizontal scrollbar is visible.
            - ViewableAreaVScroll  - area where content will be drawn when the vertical scrollbar is visible.
            - ViewableAreaHVScroll - area where content will be drawn when both scrollbars are visible.

        Child Widgets:
            Scrollbar based widget with name suffix "__auto_vscrollbar__"
            Scrollbar based widget with name suffix "__auto_hscrollbar__"

    */
    class FALAGARDBASE_API FalagardScrollablePane : public ScrollablePane
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardScrollablePane(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardScrollablePane();

    protected:
        // overridden from ScrollablePane base class.
        void populateRenderCache();
        Scrollbar* createVerticalScrollbar(const String& name) const;
        Scrollbar* createHorizontalScrollbar(const String& name) const;
        Rect getViewableArea(void) const;
    };

    /*!
    \brief
        WindowFactory for FalagardScrollablePane type Window objects.
    */
    class FALAGARDBASE_API FalagardScrollablePaneFactory : public WindowFactory
    {
    public:
        FalagardScrollablePaneFactory(void) : WindowFactory(FalagardScrollablePane::WidgetTypeName) { }
        ~FalagardScrollablePaneFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section

#endif
