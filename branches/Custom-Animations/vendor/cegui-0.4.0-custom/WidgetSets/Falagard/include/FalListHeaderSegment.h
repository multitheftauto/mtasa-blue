/************************************************************************
    filename:   FalListHeaderSegment.h
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
#ifndef _FalListHeaderSegment_h_
#define _FalListHeaderSegment_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIListHeaderSegment.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "FalListHeaderSegmentProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        ListHeaderSegment class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Disabled
            - Normal
            - Hover
            - SplitterHover
            - DragGhost
            - AscendingSortIcon
            - DescendingSortDown
            - GhostAscendingSortIcon
            - GhostDescendingSortDown
    */
    class FALAGARDBASE_API FalagardListHeaderSegment : public ListHeaderSegment
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardListHeaderSegment(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardListHeaderSegment();

        const Image* getSizingCursorImage() const;
        void setSizingCursorImage(const Image* image);
        void setSizingCursorImage(const String& imageset, const String& image);

        const Image* getMovingCursorImage() const;
        void setMovingCursorImage(const Image* image);
        void setMovingCursorImage(const String& imageset, const String& image);

    protected:
        // overridden from ListHeaderSegment base class.
        void populateRenderCache();

        // properties
        static FalagardListHeaderSegmentProperties::SizingCursorImage   d_sizingCursorProperty;
        static FalagardListHeaderSegmentProperties::MovingCursorImage   d_movingCursorProperty;
    };

    /*!
    \brief
        WindowFactory for FalagardListHeaderSegment type Window objects.
    */
    class FALAGARDBASE_API FalagardListHeaderSegmentFactory : public WindowFactory
    {
    public:
        FalagardListHeaderSegmentFactory(void) : WindowFactory(FalagardListHeaderSegment::WidgetTypeName) { }
        ~FalagardListHeaderSegmentFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _FalListHeaderSegment_h_
