/************************************************************************
    filename:   FalMenubar.h
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
#ifndef _FalMenubar_h_
#define _FalMenubar_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIMenubar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Menubar class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled
            - Disabled

        Named Areas:
            ItemRenderArea
    */
    class FALAGARDBASE_API FalagardMenubar : public Menubar
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardMenubar(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardMenubar();

    protected:
        // overridden from Menubar base class.
        void populateRenderCache();
        void sizeToContent_impl(void);
        Rect getItemRenderArea(void) const;
    };

    /*!
    \brief
        WindowFactory for FalagardMenubar type Window objects.
    */
    class FALAGARDBASE_API FalagardMenubarFactory : public WindowFactory
    {
    public:
        FalagardMenubarFactory(void) : WindowFactory(FalagardMenubar::WidgetTypeName) { }
        ~FalagardMenubarFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section

#endif  // end of guard _FalMenubar_h_
