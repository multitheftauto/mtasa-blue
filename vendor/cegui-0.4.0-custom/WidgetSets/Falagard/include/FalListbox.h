/************************************************************************
    filename:   FalListbox.h
    created:    Mon Jul 4 2005
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
#ifndef _FalListbox_h_
#define _FalListbox_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIListbox.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Listbox class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled
            - Disabled

        Named Areas:
            - ItemRenderingArea
            - ItemRenderingAreaHScroll
            - ItemRenderingAreaVScroll
            - ItemRenderingAreaHVScroll

        Child Widgets:
            Scrollbar based widget with name suffix "__auto_vscrollbar__"
            Scrollbar based widget with name suffix "__auto_hscrollbar__"
    */
    class FALAGARDBASE_API FalagardListbox : public Listbox
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardListbox(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardListbox();

    protected:
        // overridden from Listbox base class.
        Rect getListRenderArea(void) const;
        Scrollbar* createVertScrollbar(const String& name) const;
        Scrollbar* createHorzScrollbar(const String& name) const;
        void cacheListboxBaseImagery();
    };

    /*!
    \brief
        WindowFactory for FalagardListbox type Window objects.
    */
    class FALAGARDBASE_API FalagardListboxFactory : public WindowFactory
    {
    public:
        FalagardListboxFactory(void) : WindowFactory(FalagardListbox::WidgetTypeName) { }
        ~FalagardListboxFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalListbox_h_
