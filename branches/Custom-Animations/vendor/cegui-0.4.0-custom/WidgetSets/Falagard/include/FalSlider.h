/************************************************************************
    filename:   FalSlider.h
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
#ifndef _FalSlider_h_
#define _FalSlider_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUISlider.h"
#include "FalSliderProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Slider class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled
            - Disabled

        Named Areas:
            - ThumbTrackArea

        Child Widgets:
            Thumb based widget with name suffix "__auto_thumb__"
    */
    class FALAGARDBASE_API FalagardSlider : public Slider
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardSlider(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardSlider();

        bool isVertical() const;
        void setVertical(bool setting);

        bool isReversedDirection() const;
        void setReversedDirection(bool setting);

    protected:
        // data members
        bool    d_vertical;     //!< True if slider operates in vertical direction.
        bool    d_reversed;     //!< true if slider operates in reversed direction to 'normal'.

        // overridden from Slider base class.
        void populateRenderCache();
        Thumb* createThumb(const String& name) const;
        void performChildWindowLayout();
        void updateThumb(void);
        float getValueFromThumb(void) const;
        float getAdjustDirectionFromPoint(const Point& pt) const;

        // property objects
        static FalagardSliderProperties::VerticalSlider d_verticalProperty;
        static FalagardSliderProperties::ReversedDirection d_reversedProperty;
    };

    /*!
    \brief
        WindowFactory for FalagardSlider type Window objects.
    */
    class FALAGARDBASE_API FalagardSliderFactory : public WindowFactory
    {
    public:
        FalagardSliderFactory(void) : WindowFactory(FalagardSlider::WidgetTypeName) { }
        ~FalagardSliderFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _FalSlider_h_
