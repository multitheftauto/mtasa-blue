/************************************************************************
    filename:   CEGUIFalWidgetComponent.h
    created:    Mon Jun 13 2005
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
#ifndef _CEGUIFalWidgetComponent_h_
#define _CEGUIFalWidgetComponent_h_

#include "falagard/CEGUIFalDimensions.h"
#include "falagard/CEGUIFalPropertyInitialiser.h"
#include "CEGUIWindow.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Class that encapsulates information regarding a sub-widget required for a widget.

    \todo
        This is not finished in the slightest!  There will be many changes here...
    */
    class CEGUIEXPORT WidgetComponent
    {
    public:
        WidgetComponent() {}
        WidgetComponent(const String& type, const String& look, const String& suffix);

        /*!
        \brief
            Create an instance of this widget component adding it as a child to the given Window.
        */
        void create(Window& parent) const;

        const ComponentArea& getComponentArea() const;
        void setComponentArea(const ComponentArea& area);

        const String& getBaseWidgetType() const;
        void setBaseWidgetType(const String& type);

        const String& getWidgetLookName() const;
        void setWidgetLookName(const String& look);

        const String& getWidgetNameSuffix() const;
        void setWidgetNameSuffix(const String& suffix);

        VerticalAlignment getVerticalWidgetAlignemnt() const;
        void setVerticalWidgetAlignment(VerticalAlignment alignment);

        HorizontalAlignment getHorizontalWidgetAlignemnt() const;
        void setHorizontalWidgetAlignemnt(HorizontalAlignment alignment);

        void addPropertyInitialiser(const PropertyInitialiser& initialiser);
        void clearPropertyInitialisers();

        void layout(const Window& owner) const;

        /*!
        \brief
            Writes an xml representation of this WidgetComponent to \a out_stream.

        \param out_stream
            Stream where xml data should be output.

        \return
            Nothing.
        */
        void writeXMLToStream(OutStream& out_stream) const;

    private:
        typedef std::vector<PropertyInitialiser> PropertiesList;

        ComponentArea   d_area;              //!< Destination area for the widget (relative to it's parent).
        String   d_baseType;                 //!< Type of widget to be created.
        String   d_imageryName;              //!< Name of a WidgetLookFeel to be used for the widget.
        String   d_nameSuffix;               //!< Suffix to apply to the parent Window name to create this widgets unique name.
        VerticalAlignment    d_vertAlign;    //!< Vertical alignment to be used for this widget.
        HorizontalAlignment  d_horzAlign;    //!< Horizontal alignment to be used for this widget.
        PropertiesList  d_properties;        //!< Collection of PropertyInitialisers to be applied the the widget upon creation.
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _CEGUIFalWidgetComponent_h_
