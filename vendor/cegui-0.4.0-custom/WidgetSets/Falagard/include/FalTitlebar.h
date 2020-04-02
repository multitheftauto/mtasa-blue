/************************************************************************
    filename:   FalTitlebar.h
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
#ifndef _FalTitlebar_h_
#define _FalTitlebar_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUITitlebar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Titlebar class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Active
            - Inactive
            - Disabled
    */
    class FALAGARDBASE_API FalagardTitlebar : public Titlebar
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardTitlebar(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardTitlebar();

        // overridden from Window base class.
        Rect getPixelRect(void) const;

    protected:
        // overridden from Titlebar base class.
        void populateRenderCache();
    };

    /*!
    \brief
        WindowFactory for FalagardTitlebar type Window objects.
    */
    class FALAGARDBASE_API FalagardTitlebarFactory : public WindowFactory
    {
    public:
        FalagardTitlebarFactory(void) : WindowFactory(FalagardTitlebar::WidgetTypeName) { }
        ~FalagardTitlebarFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalTitlebar_h_
