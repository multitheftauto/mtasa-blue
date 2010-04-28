/************************************************************************
    filename:   FalListHeader.h
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
#ifndef _FalListHeader_h_
#define _FalListHeader_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIListHeader.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "FalListHeaderProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        ListHeader class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        Property Initialisers:
            SegmentWidgetType   - type of widget to create for segments.

        Imagery States:
            - Enabled           - basic rendering for enabled state.
            - Disabled          - basic rendering for disabled state.
    */
    class FALAGARDBASE_API FalagardListHeader : public ListHeader
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardListHeader(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardListHeader();

        const String& getSegmentWidgetType() const;
        void setSegmentWidgetType(const String& type);

    protected:
        // overridden from ListHeader base class.
        void populateRenderCache();
        ListHeaderSegment* createNewSegment(const String& name) const;
        void destroyListSegment(ListHeaderSegment* segment) const;

        // properties
        static FalagardListHeaderProperties::SegmentWidgetType   d_segmentWidgetTypeProperty;

        // data fields
        String  d_segmentWidgetType;
    };

    /*!
    \brief
        WindowFactory for FalagardListHeader type Window objects.
    */
    class FALAGARDBASE_API FalagardListHeaderFactory : public WindowFactory
    {
    public:
        FalagardListHeaderFactory(void) : WindowFactory(FalagardListHeader::WidgetTypeName) { }
        ~FalagardListHeaderFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _FalListHeader_h_
