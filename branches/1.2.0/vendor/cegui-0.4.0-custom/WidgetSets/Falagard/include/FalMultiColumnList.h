/************************************************************************
    filename:   FalMultiColumnList.h
    created:    Wed Jul 6 2005
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
#ifndef _FalMultiColumnList_h_
#define _FalMultiColumnList_h_

#include "FalModule.h"
#include "elements/CEGUIMultiColumnList.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIWindowFactory.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        MultiColumnList class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled
            - Disabled

        Named Areas:
            - ItemRenderingArea         - area where items will be drawn when no scrollbars are visible.
            - ItemRenderingAreaHScroll  - area where items will be drawn when the horizontal scrollbar is visible.
            - ItemRenderingAreaVScroll  - area where items will be drawn when the vertical scrollbar is visible.
            - ItemRenderingAreaHVScroll - area where items will be drawn when both scrollbars are visible.

        Child Widgets:
            Scrollbar based widget with name suffix "__auto_vscrollbar__"
            Scrollbar based widget with name suffix "__auto_hscrollbar__"
            ListHeader based widget with name suffix "__auto_listheader__"
    */
    class FALAGARDBASE_API FalagardMultiColumnList : public MultiColumnList
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardMultiColumnList(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardMultiColumnList();

    protected:
        // overridden from MultiColumnList base class.
        Rect getListRenderArea(void) const;
        ListHeader* createListHeader(const String& name) const;
        Scrollbar* createVertScrollbar(const String& name) const;
        Scrollbar* createHorzScrollbar(const String& name) const;
        void cacheListboxBaseImagery();
    };

    /*!
    \brief
        WindowFactory for FalagardMultiColumnList type Window objects.
    */
    class FALAGARDBASE_API FalagardMultiColumnListFactory : public WindowFactory
    {
    public:
        FalagardMultiColumnListFactory(void) : WindowFactory(FalagardMultiColumnList::WidgetTypeName) { }
        ~FalagardMultiColumnListFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalMultiColumnList_h_
