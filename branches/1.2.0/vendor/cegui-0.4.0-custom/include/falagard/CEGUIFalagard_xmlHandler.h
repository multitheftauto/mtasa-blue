/************************************************************************
    filename:   CEGUIFalagard_xmlHandler.h
    created:    Fri Jun 17 2005
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
#ifndef _CEGUIFalagard_xmlHandler_h_
#define _CEGUIFalagard_xmlHandler_h_

#include "CEGUIXMLHandler.h"
#include "CEGUIcolour.h"
#include "falagard/CEGUIFalDimensions.h"
#include "CEGUIWindow.h"
#include <vector>

// Start of CEGUI namespace section
namespace CEGUI
{
    // forward refs
    class WidgetLookManager;
    class WidgetLookFeel;
    class WidgetComponent;
    class ImagerySection;
    class StateImagery;
    class LayerSpecification;
    class SectionSpecification;
    class ImageryComponent;
    class ComponentArea;
    class Dimension;
    class TextComponent;
    class NamedArea;
    class FrameComponent;

    /*!
    \brief
        Handler class used to parse look & feel XML files used by the Falagard system.
    */
    class Falagard_xmlHandler : public XMLHandler
    {
    public:
        /*!
        \brief
            Constructor for Falagard_xmlHandler objects
        */
        Falagard_xmlHandler(WidgetLookManager* mgr);

        /*!
        \brief
            Destructor for Falagard_xmlHandler objects
        */
        ~Falagard_xmlHandler();

        /*************************************************************************
            XMLHandler base class overrides
        *************************************************************************/
        void elementStart(const String& element, const XMLAttributes& attributes);
        void elementEnd(const String& element);

    private:
        /*************************************************************************
            Implementation Constants
        *************************************************************************/
        // element names
        static const String FalagardElement;            //!< Tag name for root Falagard elements.
        static const String WidgetLookElement;          //!< Tag name for WidgetLook elements.
        static const String ChildElement;               //!< Tag name for Child elements.
        static const String ImagerySectionElement;      //!< Tag name for ImagerySection elements.
        static const String StateImageryElement;        //!< Tag name for StateImagery elements.
        static const String LayerElement;               //!< Tag name for Layer elements.
        static const String SectionElement;             //!< Tag name for Section elements.
        static const String ImageryComponentElement;    //!< Tag name for ImageryComponent elements.
        static const String TextComponentElement;       //!< Tag name for TextComponent elements.
        static const String FrameComponentElement;      //!< Tag name for FrameComponent elements.
        static const String AreaElement;                //!< Tag name for Area elements.
        static const String ImageElement;               //!< Tag name for Image elements.
        static const String ColoursElement;             //!< Tag name for Colours elements.
        static const String VertFormatElement;          //!< Tag name for VertFormat elements.
        static const String HorzFormatElement;          //!< Tag name for HorzFormat elements.
        static const String VertAlignmentElement;       //!< Tag name for VertAlignment elements.
        static const String HorzAlignmentElement;       //!< Tag name for HorzAlignment elements.
        static const String PropertyElement;            //!< Tag name for Property elements.
		static const String DimElement;                 //!< Tag name for dimension container elements.
		static const String UnifiedDimElement;          //!< Tag name for unified dimension elements.
        static const String AbsoluteDimElement;         //!< Tag name for absolute dimension elements.
        static const String ImageDimElement;            //!< Tag name for image dimension elements.
        static const String WidgetDimElement;           //!< Tag name for widget dimension elements.
        static const String FontDimElement;             //!< Tag name for font dimension elements.
        static const String PropertyDimElement;         //!< Tag name for property dimension elements.
        static const String TextElement;                //!< Tag name for text component text elements
        static const String ColourPropertyElement;      //!< Tag name for property colour elements (fetches cols from a colour property)
        static const String ColourRectPropertyElement;  //!< Tag name for property colour elements (fetches cols from a ColourRect property)
        static const String NamedAreaElement;           //!< Tag name for named area elements.
        static const String PropertyDefinitionElement;  //!< Tag name for property definition elements.
        static const String DimOperatorElement;         //!< Tag name for dimension operator elements.
        static const String VertFormatPropertyElement;  //!< Tag name for element that specifies a vertical formatting property.
        static const String HorzFormatPropertyElement;  //!< Tag name for element that specifies a horizontal formatting property..
        static const String AreaPropertyElement;        //!< Tag name for element that specifies a URect property..
        static const String ImagePropertyElement;       //!< Tag name for element that specifies an Image property..
        // attribute names
        static const String TopLeftAttribute;           //!< Attribute name that stores colour for top-left corner.
        static const String TopRightAttribute;          //!< Attribute name that stores colour for top-right corner.
        static const String BottomLeftAttribute;        //!< Attribute name that stores colour for bottom-left corner.
        static const String BottomRightAttribute;       //!< Attribute name that stores colour for bottom-right corner.
        static const String ImagesetAttribute;          //!< Attribute name that stores name of an imageset.
        static const String ImageAttribute;             //!< Attribute name that stores name of an image.
        static const String TypeAttribute;              //!< Attribute name that stores a type string.
        static const String NameAttribute;              //!< Attribute name that stores name string
        static const String PriorityAttribute;          //!< Attribute name that stores an integer priority.
        static const String SectionNameAttribute;       //!< Attribute name that stores an imagery section name.
        static const String NameSuffixAttribute;        //!< Attribute name that stores a widget name suffix.
        static const String LookAttribute;              //!< Attribute name that stores the name of a widget look.
        static const String ScaleAttribute;             //!< Attribute name that stores a UDim scale value.
        static const String OffsetAttribute;            //!< Attribute name that stores a UDim offset value.
        static const String ValueAttribute;             //!< Attribute name that stores a property value string.
        static const String DimensionAttribute;         //!< Attribute name that stores a dimension type.
        static const String WidgetAttribute;            //!< Attribute name that stores the name of a widget (suffix).
        static const String StringAttribute;            //!< Attribute name that stores a string of text.
        static const String FontAttribute;              //!< Attribute name that stores the name of a font.
        static const String InitialValueAttribute;      //!< Attribute name that stores the initial default value for a property definition.
        static const String ClippedAttribute;           //!< Attribute name that stores whether some component will be clipped.
        static const String OperatorAttribute;          //!< Attribute name that stores the name of an operator.
        static const String PaddingAttribute;           //!< Attribute name that stores some padding value..
        static const String LayoutOnWriteAttribute;     //!< Attribute name that stores whether to layout on write of a property.
        static const String RedrawOnWriteAttribute;     //!< Attribute name that stores whether to redraw on write of a property.

        /*************************************************************************
            helper methods
        **************************************************************************/
        static argb_t hexStringToARGB(const String& str);

        /*************************************************************************
            implementation methods
        **************************************************************************/
        void assignAreaDimension(Dimension& dim);

        void doBaseDimStart(const BaseDim* dim);
        void doBaseDimEnd();

        /*************************************************************************
            Implementation Data
        *************************************************************************/
        WidgetLookManager* d_manager;

        // these hold pointers to various objects under construction.
        WidgetLookFeel*     d_widgetlook;
        WidgetComponent*    d_childcomponent;
        ImagerySection*     d_imagerysection;
        StateImagery*       d_stateimagery;
        LayerSpecification* d_layer;
        SectionSpecification* d_section;
        ImageryComponent*   d_imagerycomponent;
        ComponentArea*   d_area;
        Dimension       d_dimension;
        TextComponent*  d_textcomponent;
        NamedArea*      d_namedArea;
        FrameComponent*  d_framecomponent;

        std::vector<BaseDim*>    d_dimStack;
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalagard_xmlHandler_h_
