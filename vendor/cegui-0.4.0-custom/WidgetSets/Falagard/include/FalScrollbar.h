/************************************************************************
    filename:   FalScrollbar.h
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
#ifndef _FalScrollbar_h_
#define _FalScrollbar_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIScrollbar.h"
#include "FalScrollbarProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Scrollbar class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled
            - Disabled

        Named Areas:
            - ThumbTrackArea

        Child Widgets:
            Thumb based widget with name suffix "__auto_thumb__"
            PushButton based widget with name suffix "__auto_incbtn__"
            PushButton based widget with name suffix "__auto_decbtn__"
    */
    class FALAGARDBASE_API FalagardScrollbar : public Scrollbar
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardScrollbar(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardScrollbar();

        bool isVertical() const;
        void setVertical(bool setting);

    protected:
        // data members
        bool    d_vertical;     //!< True if slider operates in vertical direction.

        // overridden from Scrollbar base class.
        void populateRenderCache();
        Thumb* createThumb(const String& name) const;
        PushButton* createIncreaseButton(const String& name) const;
        PushButton* createDecreaseButton(const String& name) const;
        void performChildWindowLayout();
        void updateThumb(void);
        float getValueFromThumb(void) const;
        float getAdjustDirectionFromPoint(const Point& pt) const;

        // property objects
        static FalagardScrollbarProperties::VerticalScrollbar d_verticalProperty;
    };

    /*!
    \brief
        WindowFactory for FalagardScrollbar type Window objects.
    */
    class FALAGARDBASE_API FalagardScrollbarFactory : public WindowFactory
    {
    public:
        FalagardScrollbarFactory(void) : WindowFactory(FalagardScrollbar::WidgetTypeName) { }
        ~FalagardScrollbarFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _FalScrollbar_h_
