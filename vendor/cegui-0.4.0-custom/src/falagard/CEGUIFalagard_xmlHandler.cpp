/************************************************************************
    filename:   CEGUIFalagard_xmlHandler.cpp
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
#include "StdInc.h"
#include "falagard/CEGUIFalagard_xmlHandler.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "falagard/CEGUIFalWidgetComponent.h"
#include "falagard/CEGUIFalTextComponent.h"
#include "falagard/CEGUIFalFrameComponent.h"
#include "falagard/CEGUIFalNamedArea.h"
#include "falagard/CEGUIFalPropertyDefinition.h"
#include "falagard/CEGUIFalXMLEnumHelper.h"
#include "CEGUIXMLAttributes.h"
#include "CEGUILogger.h"
#include <sstream>

// Start of CEGUI namespace section
namespace CEGUI
{
    ////////////////////////////////////////////////////////////////////////////////
    // element names
    const String Falagard_xmlHandler::FalagardElement("Falagard");
    const String Falagard_xmlHandler::WidgetLookElement("WidgetLook");
    const String Falagard_xmlHandler::ChildElement("Child");
    const String Falagard_xmlHandler::ImagerySectionElement("ImagerySection");
    const String Falagard_xmlHandler::StateImageryElement("StateImagery");
    const String Falagard_xmlHandler::LayerElement("Layer");
    const String Falagard_xmlHandler::SectionElement("Section");
    const String Falagard_xmlHandler::ImageryComponentElement("ImageryComponent");
    const String Falagard_xmlHandler::TextComponentElement("TextComponent");
    const String Falagard_xmlHandler::FrameComponentElement("FrameComponent");
    const String Falagard_xmlHandler::AreaElement("Area");
    const String Falagard_xmlHandler::ImageElement("Image");
    const String Falagard_xmlHandler::ColoursElement("Colours");
    const String Falagard_xmlHandler::VertFormatElement("VertFormat");
    const String Falagard_xmlHandler::HorzFormatElement("HorzFormat");
    const String Falagard_xmlHandler::VertAlignmentElement("VertAlignment");
    const String Falagard_xmlHandler::HorzAlignmentElement("HorzAlignment");
    const String Falagard_xmlHandler::PropertyElement("Property");
	const String Falagard_xmlHandler::DimElement("Dim");
	const String Falagard_xmlHandler::UnifiedDimElement("UnifiedDim");
    const String Falagard_xmlHandler::AbsoluteDimElement("AbsoluteDim");
    const String Falagard_xmlHandler::ImageDimElement("ImageDim");
    const String Falagard_xmlHandler::WidgetDimElement("WidgetDim");
    const String Falagard_xmlHandler::FontDimElement("FontDim");
    const String Falagard_xmlHandler::PropertyDimElement("PropertyDim");
    const String Falagard_xmlHandler::TextElement("Text");
    const String Falagard_xmlHandler::ColourPropertyElement("ColourProperty");
    const String Falagard_xmlHandler::ColourRectPropertyElement("ColourRectProperty");
    const String Falagard_xmlHandler::NamedAreaElement("NamedArea");
    const String Falagard_xmlHandler::PropertyDefinitionElement("PropertyDefinition");
    const String Falagard_xmlHandler::DimOperatorElement("DimOperator");
    const String Falagard_xmlHandler::VertFormatPropertyElement("VertFormatProperty");
    const String Falagard_xmlHandler::HorzFormatPropertyElement("HorzFormatProperty");
    const String Falagard_xmlHandler::AreaPropertyElement("AreaProperty");
    const String Falagard_xmlHandler::ImagePropertyElement("ImageProperty");
    // attribute names
    const String Falagard_xmlHandler::TopLeftAttribute("topLeft");
    const String Falagard_xmlHandler::TopRightAttribute("topRight");
    const String Falagard_xmlHandler::BottomLeftAttribute("bottomLeft");
    const String Falagard_xmlHandler::BottomRightAttribute("bottomRight");
    const String Falagard_xmlHandler::ImagesetAttribute("imageset");
    const String Falagard_xmlHandler::ImageAttribute("image");
    const String Falagard_xmlHandler::TypeAttribute("type");
    const String Falagard_xmlHandler::NameAttribute("name");
    const String Falagard_xmlHandler::PriorityAttribute("priority");
    const String Falagard_xmlHandler::SectionNameAttribute("section");
    const String Falagard_xmlHandler::NameSuffixAttribute("nameSuffix");
    const String Falagard_xmlHandler::LookAttribute("look");
    const String Falagard_xmlHandler::ScaleAttribute("scale");
    const String Falagard_xmlHandler::OffsetAttribute("offset");
    const String Falagard_xmlHandler::ValueAttribute("value");
    const String Falagard_xmlHandler::DimensionAttribute("dimension");
    const String Falagard_xmlHandler::WidgetAttribute("widget");
    const String Falagard_xmlHandler::StringAttribute("string");
    const String Falagard_xmlHandler::FontAttribute("font");
    const String Falagard_xmlHandler::InitialValueAttribute("initialValue");
    const String Falagard_xmlHandler::ClippedAttribute("clipped");
    const String Falagard_xmlHandler::OperatorAttribute("op");
    const String Falagard_xmlHandler::PaddingAttribute("padding");
    const String Falagard_xmlHandler::LayoutOnWriteAttribute("layoutOnWrite");
    const String Falagard_xmlHandler::RedrawOnWriteAttribute("redrawOnWrite");

    ////////////////////////////////////////////////////////////////////////////////


    Falagard_xmlHandler::Falagard_xmlHandler(WidgetLookManager* mgr) :
        d_manager(mgr),
        d_widgetlook(0),
        d_childcomponent(0),
        d_imagerysection(0),
        d_stateimagery(0),
        d_layer(0),
        d_section(0),
        d_imagerycomponent(0),
        d_area(0),
        d_textcomponent(0),
        d_namedArea(0),
        d_framecomponent(0)
    {
    }

    Falagard_xmlHandler::~Falagard_xmlHandler()
    {}

    void Falagard_xmlHandler::elementStart(const String& element, const XMLAttributes& attributes)
    {
        // root Falagard element
        if (element == FalagardElement)
        {
            Logger::getSingleton().logEvent("===== Falagard 'root' element: look and feel parsing begins =====");
        }
        // starting a new widget-look
        else if (element == WidgetLookElement)
        {
            assert(d_widgetlook == 0);
            d_widgetlook = new WidgetLookFeel(attributes.getValueAsString(NameAttribute));

            Logger::getSingleton().logEvent("---> Start of definition for widget look '" + d_widgetlook->getName() + "'.", Informative);
        }
        // starting a component widget "Child" element
        else if (element == ChildElement)
        {
            assert(d_childcomponent == 0);
            d_childcomponent = new WidgetComponent(attributes.getValueAsString(TypeAttribute), attributes.getValueAsString(LookAttribute), attributes.getValueAsString(NameSuffixAttribute));

            CEGUI_LOGINSANE("-----> Start of definition for child widget. Type: " + d_childcomponent->getBaseWidgetType() + " Suffix: " + d_childcomponent->getWidgetNameSuffix() + " Look: " + d_childcomponent->getWidgetLookName());
        }
        else if (element == ImagerySectionElement)
        {
            assert(d_imagerysection == 0);
            d_imagerysection = new ImagerySection(attributes.getValueAsString(NameAttribute));

            CEGUI_LOGINSANE("-----> Start of definition for imagery section '" + d_imagerysection->getName() + "'.");
        }
        else if (element == StateImageryElement)
        {
            assert(d_stateimagery == 0);
            d_stateimagery = new StateImagery(attributes.getValueAsString(NameAttribute));
            d_stateimagery->setClippedToDisplay(!attributes.getValueAsBool(ClippedAttribute, true));

            CEGUI_LOGINSANE("-----> Start of definition for imagery for state '" + d_stateimagery->getName() + "'.");
        }
        else if (element == LayerElement)
        {
            assert(d_layer == 0);
            d_layer = new LayerSpecification(attributes.getValueAsInteger(PriorityAttribute, 0));

            CEGUI_LOGINSANE("-------> Start of definition of new imagery layer, priority: " + attributes.getValueAsString(PriorityAttribute, "0"));
        }
        else if (element == SectionElement)
        {
            assert(d_section == 0);
            assert(d_widgetlook != 0);
            String owner(attributes.getValueAsString(LookAttribute));
            d_section = new SectionSpecification(owner.empty() ? d_widgetlook->getName() : owner, attributes.getValueAsString(SectionNameAttribute));

            CEGUI_LOGINSANE("---------> Layer references imagery section '" + d_section->getSectionName() + "'.");
        }
        else if (element == ImageryComponentElement)
        {
            assert(d_imagerycomponent == 0);
            d_imagerycomponent = new ImageryComponent();

            CEGUI_LOGINSANE("-------> Image component definition...");
        }
        else if (element == TextComponentElement)
        {
            assert(d_textcomponent == 0);
            d_textcomponent = new TextComponent();

            CEGUI_LOGINSANE("-------> Text component definition...");
        }
        else if (element == FrameComponentElement)
        {
            assert(d_framecomponent == 0);
            d_framecomponent = new FrameComponent();

            CEGUI_LOGINSANE("-------> Frame component definition...");
        }
        else if (element == AreaElement)
        {
            assert(d_area == 0);
            d_area = new ComponentArea();
        }
        else if (element == ColoursElement)
        {
            ColourRect cols(
                hexStringToARGB(attributes.getValueAsString(TopLeftAttribute)),
                hexStringToARGB(attributes.getValueAsString(TopRightAttribute)),
                hexStringToARGB(attributes.getValueAsString(BottomLeftAttribute)),
                hexStringToARGB(attributes.getValueAsString(BottomRightAttribute))
            );

            // need to decide what to apply colours to
            if (d_framecomponent)
            {
                d_framecomponent->setColours(cols);
            }
            else if (d_imagerycomponent)
            {
                d_imagerycomponent->setColours(cols);
            }
            else if (d_textcomponent)
            {
                d_textcomponent->setColours(cols);
            }
            else if (d_imagerysection)
            {
                d_imagerysection->setMasterColours(cols);
            }
            else if (d_section)
            {
                d_section->setOverrideColours(cols);
				d_section->setUsingOverrideColours(true);
            }
        }
        else if ((element == ColourPropertyElement) || (element == ColourRectPropertyElement))
        {
            // need to decide what to apply colours to
            if (d_framecomponent)
            {
                d_framecomponent->setColoursPropertySource(attributes.getValueAsString(NameAttribute));
                d_framecomponent->setColoursPropertyIsColourRect(element == ColourRectPropertyElement);
            }
            else if (d_imagerycomponent)
            {
                d_imagerycomponent->setColoursPropertySource(attributes.getValueAsString(NameAttribute));
                d_imagerycomponent->setColoursPropertyIsColourRect(element == ColourRectPropertyElement);
            }
            else if (d_textcomponent)
            {
                d_textcomponent->setColoursPropertySource(attributes.getValueAsString(NameAttribute));
                d_textcomponent->setColoursPropertyIsColourRect(element == ColourRectPropertyElement);
            }
            else if (d_imagerysection)
            {
                d_imagerysection->setMasterColoursPropertySource(attributes.getValueAsString(NameAttribute));
                d_imagerysection->setMasterColoursPropertyIsColourRect(element == ColourRectPropertyElement);
            }
            else if (d_section)
            {
                d_section->setOverrideColoursPropertySource(attributes.getValueAsString(NameAttribute));
                d_section->setOverrideColoursPropertyIsColourRect(element == ColourRectPropertyElement);
                d_section->setUsingOverrideColours(true);
            }
        }
        else if (element == PropertyElement)
        {
            assert(d_widgetlook != 0);
            PropertyInitialiser prop(attributes.getValueAsString(NameAttribute), attributes.getValueAsString(ValueAttribute));

            if (d_childcomponent)
            {
                d_childcomponent->addPropertyInitialiser(prop);
                CEGUI_LOGINSANE("-------> Added property initialiser for property: " + prop.getTargetPropertyName() + " with value: " + prop.getInitialiserValue());
            }
            else
            {
                d_widgetlook->addPropertyInitialiser(prop);
                CEGUI_LOGINSANE("---> Added property initialiser for property: " + prop.getTargetPropertyName() + " with value: " + prop.getInitialiserValue());
            }

        }
        else if (element == VertAlignmentElement)
        {
            assert(d_childcomponent != 0);
            d_childcomponent->setVerticalWidgetAlignment(FalagardXMLHelper::stringToVertAlignment(attributes.getValueAsString(TypeAttribute)));
        }
        else if (element == HorzAlignmentElement)
        {
            assert(d_childcomponent != 0);
            d_childcomponent->setHorizontalWidgetAlignemnt(FalagardXMLHelper::stringToHorzAlignment(attributes.getValueAsString(TypeAttribute)));
        }
        else if (element == VertFormatElement)
        {
            if (d_framecomponent)
            {
                d_framecomponent->setBackgroundVerticalFormatting(FalagardXMLHelper::stringToVertFormat(attributes.getValueAsString(TypeAttribute)));
            }
            else if (d_imagerycomponent)
            {
                d_imagerycomponent->setVerticalFormatting(FalagardXMLHelper::stringToVertFormat(attributes.getValueAsString(TypeAttribute)));
            }
            else if (d_textcomponent)
            {
                d_textcomponent->setVerticalFormatting(FalagardXMLHelper::stringToVertTextFormat(attributes.getValueAsString(TypeAttribute)));
            }
        }
        else if (element == HorzFormatElement)
        {
            if (d_framecomponent)
            {
                d_framecomponent->setBackgroundHorizontalFormatting(FalagardXMLHelper::stringToHorzFormat(attributes.getValueAsString(TypeAttribute)));
            }
            else if (d_imagerycomponent)
            {
                d_imagerycomponent->setHorizontalFormatting(FalagardXMLHelper::stringToHorzFormat(attributes.getValueAsString(TypeAttribute)));
            }
            else if (d_textcomponent)
            {
                d_textcomponent->setHorizontalFormatting(FalagardXMLHelper::stringToHorzTextFormat(attributes.getValueAsString(TypeAttribute)));
            }
        }
        else if (element == ImageElement)
        {
            if (d_imagerycomponent)
            {
                d_imagerycomponent->setImage(attributes.getValueAsString(ImagesetAttribute), attributes.getValueAsString(ImageAttribute));
                CEGUI_LOGINSANE("---------> Using image: " + attributes.getValueAsString(ImageAttribute) + " from imageset: " + attributes.getValueAsString(ImagesetAttribute));
            }
            else if (d_framecomponent)
            {
                d_framecomponent->setImage(
                    FalagardXMLHelper::stringToFrameImageComponent(attributes.getValueAsString(TypeAttribute)),
                    attributes.getValueAsString(ImagesetAttribute),
                    attributes.getValueAsString(ImageAttribute));

                CEGUI_LOGINSANE("---------> Using image: " +
                    attributes.getValueAsString(ImageAttribute) + " from imageset: " +
                    attributes.getValueAsString(ImagesetAttribute) + " for: " +
                    attributes.getValueAsString(TypeAttribute));
            }
        }
		else if (element == DimElement)
		{
			d_dimension.setDimensionType(FalagardXMLHelper::stringToDimensionType(attributes.getValueAsString(TypeAttribute)));
		}
        else if (element == UnifiedDimElement)
        {
            UnifiedDim base(UDim(attributes.getValueAsFloat(ScaleAttribute, 0.0f), attributes.getValueAsFloat(OffsetAttribute, 0.0f)), FalagardXMLHelper::stringToDimensionType(attributes.getValueAsString(TypeAttribute)));
            doBaseDimStart(&base);
        }
        else if (element == AbsoluteDimElement)
        {
            AbsoluteDim base(attributes.getValueAsFloat(ValueAttribute, 0.0f));
            doBaseDimStart(&base);
        }
        else if (element == ImageDimElement)
        {
            ImageDim base(attributes.getValueAsString(ImagesetAttribute), attributes.getValueAsString(ImageAttribute), FalagardXMLHelper::stringToDimensionType(attributes.getValueAsString(DimensionAttribute)));
            doBaseDimStart(&base);
        }
        else if (element == WidgetDimElement)
        {
            WidgetDim base(attributes.getValueAsString(WidgetAttribute), FalagardXMLHelper::stringToDimensionType(attributes.getValueAsString(DimensionAttribute)));
            doBaseDimStart(&base);
        }
        else if (element == FontDimElement)
        {
            FontDim base(
                attributes.getValueAsString(WidgetAttribute),
                attributes.getValueAsString(FontAttribute),
                attributes.getValueAsString(StringAttribute),
                FalagardXMLHelper::stringToFontMetricType(attributes.getValueAsString(TypeAttribute)),
                attributes.getValueAsFloat(PaddingAttribute, 0.0f));

            doBaseDimStart(&base);
        }
        else if (element == PropertyDimElement)
        {
            PropertyDim base(attributes.getValueAsString(WidgetAttribute), attributes.getValueAsString(NameAttribute));
            doBaseDimStart(&base);
        }
        else if (element == TextElement)
        {
            assert (d_textcomponent != 0);
            d_textcomponent->setText(attributes.getValueAsString(StringAttribute));
            d_textcomponent->setFont(attributes.getValueAsString(FontAttribute));
        }
        else if (element == NamedAreaElement)
        {
            assert(d_namedArea == 0);
            d_namedArea = new NamedArea(attributes.getValueAsString(NameAttribute));

            CEGUI_LOGINSANE("-----> Creating named area: " + d_namedArea->getName());
        }
        else if (element == DimOperatorElement)
        {
            if (!d_dimStack.empty())
            {
                d_dimStack.back()->setDimensionOperator(FalagardXMLHelper::stringToDimensionOperator(attributes.getValueAsString(OperatorAttribute)));
            }
        }
        else if (element == PropertyDefinitionElement)
        {
            assert(d_widgetlook);

            PropertyDefinition prop(
                attributes.getValueAsString(NameAttribute),
                attributes.getValueAsString(InitialValueAttribute),
                attributes.getValueAsBool(RedrawOnWriteAttribute, false),
                attributes.getValueAsBool(LayoutOnWriteAttribute, false)
            );

            CEGUI_LOGINSANE("-----> Adding PropertyDefiniton. Name: " + prop.getName() + " Default Value: " + attributes.getValueAsString(InitialValueAttribute));

            d_widgetlook->addPropertyDefinition(prop);
        }
        else if (element == VertFormatPropertyElement)
        {
            if (d_framecomponent)
                d_framecomponent->setVertFormattingPropertySource(attributes.getValueAsString(NameAttribute));
            else if (d_imagerycomponent)
                d_imagerycomponent->setVertFormattingPropertySource(attributes.getValueAsString(NameAttribute));
            else if (d_textcomponent)
                d_textcomponent->setVertFormattingPropertySource(attributes.getValueAsString(NameAttribute));
        }
        else if (element == HorzFormatPropertyElement)
        {
            if (d_framecomponent)
                d_framecomponent->setHorzFormattingPropertySource(attributes.getValueAsString(NameAttribute));
            else if (d_imagerycomponent)
                d_imagerycomponent->setHorzFormattingPropertySource(attributes.getValueAsString(NameAttribute));
            else if (d_textcomponent)
                d_textcomponent->setHorzFormattingPropertySource(attributes.getValueAsString(NameAttribute));
        }
        else if (element == AreaPropertyElement)
        {
            assert (d_area != 0);

            d_area->setAreaPropertySource(attributes.getValueAsString(NameAttribute));
        }
        else if (element == ImagePropertyElement)
        {
            assert(d_imagerycomponent != 0);

            d_imagerycomponent->setImagePropertySource(attributes.getValueAsString(NameAttribute));
        }
        else
        {
            throw FileIOException("Falagard::xmlHandler::elementStart - The unknown XML element '" + element + "' was encountered while processing the look and feel file.");
        }
    }

    void Falagard_xmlHandler::elementEnd(const String& element)
    {
        // end of main element
        if (element == FalagardElement)
        {
            Logger::getSingleton().logEvent("===== Look and feel parsing completed =====");
        }
        // ending a widget-look
        else if (element == WidgetLookElement)
        {
            if (d_widgetlook)
            {
                Logger::getSingleton().logEvent("---< End of definition for widget look '" + d_widgetlook->getName() + "'.", Informative);
                d_manager->addWidgetLook(*d_widgetlook);
                delete d_widgetlook;
                d_widgetlook = 0;
            }
        }
        // ending a component widget
        else if (element == ChildElement)
        {
            assert(d_widgetlook != 0);

            if (d_childcomponent)
            {
                CEGUI_LOGINSANE("-----< End of definition for child widget. Type: " + d_childcomponent->getBaseWidgetType() + ".");
                d_widgetlook->addWidgetComponent(*d_childcomponent);
                delete d_childcomponent;
                d_childcomponent = 0;
            }
        }
        // ending an ImagerySection
        else if (element == ImagerySectionElement)
        {
            assert(d_widgetlook != 0);

            if (d_imagerysection)
            {
                CEGUI_LOGINSANE("-----< End of definition for imagery section '" + d_imagerysection->getName() + "'.");
                d_widgetlook->addImagerySection(*d_imagerysection);
                delete d_imagerysection;
                d_imagerysection = 0;
            }
        }
        // ending a StateImagery section
        else if (element == StateImageryElement)
        {
            assert(d_widgetlook != 0);

            if (d_stateimagery)
            {
                CEGUI_LOGINSANE("-----< End of definition for imagery for state '" + d_stateimagery->getName() + "'.");
                d_widgetlook->addStateSpecification(*d_stateimagery);
                delete d_stateimagery;
                d_stateimagery = 0;
            }
        }
        // ending a Layer section
        else if (element == LayerElement)
        {
            assert(d_stateimagery != 0);

            if (d_layer)
            {
                CEGUI_LOGINSANE("-------< End of definition of imagery layer.");
                d_stateimagery->addLayer(*d_layer);
                delete d_layer;
                d_layer = 0;
            }
        }
        // ending a Section specification
        else if (element == SectionElement)
        {
            assert(d_layer != 0);

            if (d_section)
            {
                d_layer->addSectionSpecification(*d_section);
                delete d_section;
                d_section = 0;
            }
        }
        // ending an Imagery component specification
        else if (element == ImageryComponentElement)
        {
            assert(d_imagerysection != 0);

            if (d_imagerycomponent)
            {
                d_imagerysection->addImageryComponent(*d_imagerycomponent);
                delete d_imagerycomponent;
                d_imagerycomponent = 0;
            }
        }
        // ending a Text component specification
        else if (element == TextComponentElement)
        {
            assert(d_imagerysection != 0);

            if (d_textcomponent)
            {
                d_imagerysection->addTextComponent(*d_textcomponent);
                delete d_textcomponent;
                d_textcomponent = 0;
            }
        }
        // ending a Frame component specification
        else if (element == FrameComponentElement)
        {
            assert(d_imagerysection != 0);

            if (d_framecomponent)
            {
                d_imagerysection->addFrameComponent(*d_framecomponent);
                delete d_framecomponent;
                d_framecomponent = 0;
            }
        }
        // component area end
        else if (element == AreaElement)
        {
            assert((d_childcomponent != 0) || (d_imagerycomponent != 0) || (d_textcomponent != 0) || d_namedArea != 0 || d_framecomponent != 0);
            assert(d_area != 0);

            if (d_childcomponent)
            {
                d_childcomponent->setComponentArea(*d_area);
            }
            else if (d_framecomponent)
            {
                d_framecomponent->setComponentArea(*d_area);
            }
            else if (d_imagerycomponent)
            {
                d_imagerycomponent->setComponentArea(*d_area);
            }
            else if (d_textcomponent)
            {
                d_textcomponent->setComponentArea(*d_area);
            }
            else if (d_namedArea)
            {
                d_namedArea->setArea(*d_area);
            }

            delete d_area;
            d_area = 0;
        }
        // NamedArea end
        else if (element == NamedAreaElement)
        {
            assert(d_widgetlook != 0);

            if (d_namedArea)
            {
                d_widgetlook->addNamedArea(*d_namedArea);
                delete d_namedArea;
                d_namedArea = 0;
            }
        }
        // End of a *Dim element.
        else if ((element == UnifiedDimElement) || (element == AbsoluteDimElement) || (element == ImageDimElement) ||
                 (element == WidgetDimElement) || (element == FontDimElement) || (element == PropertyDimElement))
        {
            doBaseDimEnd();
        }
    }

    argb_t Falagard_xmlHandler::hexStringToARGB(const String& str)
    {
        argb_t val;
        std::istringstream s(str.c_str());
        s >> std::hex >> val;

        return val;
    }

    void Falagard_xmlHandler::assignAreaDimension(Dimension& dim)
    {
        if (d_area)
        {
            switch (dim.getDimensionType())
            {
            case DT_LEFT_EDGE:
            case DT_X_POSITION:
                d_area->d_left = dim;
                break;
            case DT_TOP_EDGE:
            case DT_Y_POSITION:
                d_area->d_top = dim;
                break;
            case DT_RIGHT_EDGE:
            case DT_WIDTH:
                d_area->d_right_or_width = dim;
                break;
            case DT_BOTTOM_EDGE:
            case DT_HEIGHT:
                d_area->d_bottom_or_height = dim;
                break;
            default:
                throw InvalidRequestException("Falagard::xmlHandler::assignAreaDimension - Invalid DimensionType specified for area component.");
            }
        }
    }

    void Falagard_xmlHandler::doBaseDimStart(const BaseDim* dim)
    {
        BaseDim* cloned = dim->clone();
        d_dimStack.push_back(cloned);
    }

    void Falagard_xmlHandler::doBaseDimEnd()
    {
        if (!d_dimStack.empty())
        {
            BaseDim* currDim = d_dimStack.back();
            d_dimStack.pop_back();

            if (!d_dimStack.empty())
            {
                d_dimStack.back()->setOperand(*currDim);
            }
            else
            {
                d_dimension.setBaseDimension(*currDim);
                assignAreaDimension(d_dimension);
            }

            // release the dim we popped.
            delete currDim;
        }
    }

} // End of  CEGUI namespace section
