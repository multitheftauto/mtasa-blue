/************************************************************************
    filename:   FalStaticText.h
    created:    Tue Jul 5 2005
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
#ifndef _FalStaticText_h_
#define _FalStaticText_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIStaticText.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        StaticText class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled            - basic rendering for enabled state.
            - Disabled           - basic rendering for disabled state.
            - EnabledFrame       - frame rendering for enabled state
            - DisabledFrame      - frame rendering for disabled state.
            - EnabledBackground  - backdrop rendering for enabled state
            - DisabledBackground - backdrop rendering for disabled state

        Named Areas (missing areas will default to 'TextRenderArea'):
            TextRenderArea
            TextRenderAreaHScroll
            TextRenderAreaVScroll
            TextRenderAreaHVScroll
    */
    class FALAGARDBASE_API FalagardStaticText : public StaticText
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardStaticText(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardStaticText();

    protected:
        // overridden from StaticText base class.
        void populateRenderCache();
        Scrollbar* createVertScrollbar(const String& name) const;
        Scrollbar* createHorzScrollbar(const String& name) const;
        Rect getTextRenderArea(void) const;
    };

    /*!
    \brief
        WindowFactory for FalagardStaticText type Window objects.
    */
    class FALAGARDBASE_API FalagardStaticTextFactory : public WindowFactory
    {
    public:
        FalagardStaticTextFactory(void) : WindowFactory(FalagardStaticText::WidgetTypeName) { }
        ~FalagardStaticTextFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalStaticText_h_
