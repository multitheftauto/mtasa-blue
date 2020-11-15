/***********************************************************************
    created:    Fri Jun 17 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2015 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/ColourRect.h"

#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/falagard/WidgetComponent.h"
#include "CEGUI/falagard/TextComponent.h"
#include "CEGUI/falagard/FrameComponent.h"
#include "CEGUI/falagard/NamedArea.h"
#include "CEGUI/falagard/PropertyDefinition.h"
#include "CEGUI/falagard/PropertyLinkDefinition.h"
#include "CEGUI/falagard/XMLEnumHelper.h"
#include "CEGUI/XMLAttributes.h"
#include "CEGUI/Logger.h"
#include "CEGUI/Animation_xmlHandler.h"
#include "CEGUI/AnimationManager.h"

#include "CEGUI/widgets/Thumb.h"
#include "CEGUI/widgets/TabControl.h"
#include "CEGUI/widgets/Spinner.h"
#include "CEGUI/widgets/ItemListBase.h"
#include "CEGUI/widgets/ListHeaderSegment.h"
#include "CEGUI/widgets/MultiColumnList.h"

#include <sstream>

// Start of CEGUI namespace section
namespace CEGUI
{
    // note: The assets' versions aren't usually the same as CEGUI version, they are versioned from version 1 onwards!
    const String Falagard_xmlHandler::NativeVersion("7");

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
    const String Falagard_xmlHandler::ImagePropertyDimElement("ImagePropertyDim");
    const String Falagard_xmlHandler::WidgetDimElement("WidgetDim");
    const String Falagard_xmlHandler::FontDimElement("FontDim");
    const String Falagard_xmlHandler::PropertyDimElement("PropertyDim");
    const String Falagard_xmlHandler::TextElement("Text");
    const String Falagard_xmlHandler::ColourPropertyElement("ColourProperty");
    const String Falagard_xmlHandler::ColourRectPropertyElement("ColourRectProperty");
    const String Falagard_xmlHandler::NamedAreaElement("NamedArea");
    const String Falagard_xmlHandler::PropertyDefinitionElement("PropertyDefinition");
    const String Falagard_xmlHandler::PropertyLinkDefinitionElement("PropertyLinkDefinition");
    const String Falagard_xmlHandler::PropertyLinkTargetElement("PropertyLinkTarget");
    const String Falagard_xmlHandler::OperatorDimElement("OperatorDim");
    const String Falagard_xmlHandler::VertFormatPropertyElement("VertFormatProperty");
    const String Falagard_xmlHandler::HorzFormatPropertyElement("HorzFormatProperty");
    const String Falagard_xmlHandler::AreaPropertyElement("AreaProperty");
    const String Falagard_xmlHandler::ImagePropertyElement("ImageProperty");
    const String Falagard_xmlHandler::TextPropertyElement("TextProperty");
    const String Falagard_xmlHandler::FontPropertyElement("FontProperty");
    const String Falagard_xmlHandler::ColourElement("Colour");
    const String Falagard_xmlHandler::EventLinkDefinitionElement("EventLinkDefinition");
    const String Falagard_xmlHandler::EventLinkTargetElement("EventLinkTarget");
    const String Falagard_xmlHandler::NamedAreaSourceElement("NamedAreaSource");
    const String Falagard_xmlHandler::EventActionElement("EventAction");
    // attribute names
    const String Falagard_xmlHandler::VersionAttribute("version");
    const String Falagard_xmlHandler::TopLeftAttribute("topLeft");
    const String Falagard_xmlHandler::TopRightAttribute("topRight");
    const String Falagard_xmlHandler::BottomLeftAttribute("bottomLeft");
    const String Falagard_xmlHandler::BottomRightAttribute("bottomRight");
    const String Falagard_xmlHandler::TypeAttribute("type");
    const String Falagard_xmlHandler::NameAttribute("name");
    const String Falagard_xmlHandler::PriorityAttribute("priority");
    const String Falagard_xmlHandler::SectionNameAttribute("section");
    const String Falagard_xmlHandler::NameSuffixAttribute("nameSuffix");
    const String Falagard_xmlHandler::RendererAttribute("renderer");
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
    const String Falagard_xmlHandler::TargetPropertyAttribute("targetProperty");
    const String Falagard_xmlHandler::ControlPropertyAttribute("controlProperty");
    const String Falagard_xmlHandler::ColourAttribute("colour");
    const String Falagard_xmlHandler::PropertyAttribute("property");
    const String Falagard_xmlHandler::ControlValueAttribute("controlValue");
    const String Falagard_xmlHandler::ControlWidgetAttribute("controlWidget");
    const String Falagard_xmlHandler::HelpStringAttribute("help");
    const String Falagard_xmlHandler::EventAttribute("event");
    const String Falagard_xmlHandler::InheritsAttribute("inherits");
    const String Falagard_xmlHandler::AutoWindowAttribute("autoWindow");
    const String Falagard_xmlHandler::FireEventAttribute("fireEvent");
    const String Falagard_xmlHandler::ActionAttribute("action");
    const String Falagard_xmlHandler::ComponentAttribute("component");

    // Default values
    const String Falagard_xmlHandler::PropertyDefinitionHelpDefaultValue("Falagard custom property definition - "
                                                                         "gets/sets a named user string.");
    const String Falagard_xmlHandler::PropertyLinkDefinitionHelpDefaultValue("Falagard property link definition - links a "
                                                                             "property on this window to properties "
                                                                             "defined on one or more child windows, or "
                                                                             "the parent window.");


    //! Helper function for throwing exception strings. Adds introductory text andi s attempting to specify where the issue occured.
    CEGUI::String getStartTextForException(const WidgetLookFeel* widgetLook)
    {
        CEGUI::String widgetLookInfo;
        if (widgetLook != 0)
        {
            widgetLookInfo  = "Occurrence inside " + Falagard_xmlHandler::WidgetLookElement + " : \"" + widgetLook->getName() + "\".\n"; 
        }
        else
        {
            widgetLookInfo  = "Occurrence outside of any " + Falagard_xmlHandler::WidgetLookElement + " elements.\n"; 
        }
       
        return "Exception occured while parsing a Look N' Feel file. " + widgetLookInfo + "Exception: ";
        
    }

    //! Throws an exception message for a node added as a child of a node of same type
    void throwExceptionChildOfSameNode(const WidgetLookFeel* widgetLook, const String& nodeType, const String& elementNameOrValue)
    {
        CEGUI_THROW(InvalidRequestException(
            getStartTextForException(widgetLook) +
            nodeType +  " nodes may not be children of other " + nodeType +
            " nodes. This was not the case for a " + nodeType + " with name or value \"" +
            elementNameOrValue + "\"."
            ));
    }

    //! Throws an exception message for a node added as a child of a node of same type
    void throwExceptionChildOfSameNode(const WidgetLookFeel* widgetLook, const String& nodeType)
    {
        CEGUI_THROW(InvalidRequestException(
            getStartTextForException(widgetLook) +
            nodeType +  " nodes may not be children of other " + nodeType +
            " nodes. This was not the case for a " + nodeType + " node."
            ));
    }

    //! Throws an exception message for a node which is not part of the required parent node
    void throwExceptionNotChildOfNode(const WidgetLookFeel* widgetLook, const String& childNodeType, const String& childElementName, const String& parentNodeType)
    {
        CEGUI_THROW(InvalidRequestException(
            getStartTextForException(widgetLook) +
            childNodeType +  " nodes must be part of a " + parentNodeType +
            " node. This was not the case for a " + childNodeType + " with name or value \"" +
            childElementName + "\"."
            ));
    }

    //! Throws an exception message for a node which is not part of the required parent node
    void throwExceptionNotChildOfNode(const WidgetLookFeel* widgetLook, const String& childNodeType, const String& parentNodeType)
    {
        CEGUI_THROW(InvalidRequestException(
            getStartTextForException(widgetLook) +
            childNodeType +  " nodes must be part of a " + parentNodeType +
            " node. This was not the case for a " + childNodeType + " node."
            ));
    }


    // Specific attribute values
    const String Falagard_xmlHandler::GenericDataType("Generic");
    const String Falagard_xmlHandler::ParentIdentifier("__parent__");

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
        d_framecomponent(0),
        d_propertyLink(0),
        d_eventLink(0)
    {
        // register element start handlers
        registerElementStartHandler(FalagardElement, &Falagard_xmlHandler::elementFalagardStart);
        registerElementStartHandler(WidgetLookElement, &Falagard_xmlHandler::elementWidgetLookStart);
        registerElementStartHandler(ChildElement, &Falagard_xmlHandler::elementChildStart);
        registerElementStartHandler(ImagerySectionElement, &Falagard_xmlHandler::elementImagerySectionStart);
        registerElementStartHandler(StateImageryElement, &Falagard_xmlHandler::elementStateImageryStart);
        registerElementStartHandler(LayerElement, &Falagard_xmlHandler::elementLayerStart);
        registerElementStartHandler(SectionElement, &Falagard_xmlHandler::elementSectionStart);
        registerElementStartHandler(ImageryComponentElement, &Falagard_xmlHandler::elementImageryComponentStart);
        registerElementStartHandler(TextComponentElement, &Falagard_xmlHandler::elementTextComponentStart);
        registerElementStartHandler(FrameComponentElement, &Falagard_xmlHandler::elementFrameComponentStart);
        registerElementStartHandler(AreaElement, &Falagard_xmlHandler::elementAreaStart);
        registerElementStartHandler(ImageElement, &Falagard_xmlHandler::elementImageStart);
        registerElementStartHandler(ColoursElement, &Falagard_xmlHandler::elementColoursStart);
        registerElementStartHandler(VertFormatElement, &Falagard_xmlHandler::elementVertFormatStart);
        registerElementStartHandler(HorzFormatElement, &Falagard_xmlHandler::elementHorzFormatStart);
        registerElementStartHandler(VertAlignmentElement, &Falagard_xmlHandler::elementVertAlignmentStart);
        registerElementStartHandler(HorzAlignmentElement, &Falagard_xmlHandler::elementHorzAlignmentStart);
        registerElementStartHandler(PropertyElement, &Falagard_xmlHandler::elementPropertyStart);
        registerElementStartHandler(DimElement, &Falagard_xmlHandler::elementDimStart);
        registerElementStartHandler(UnifiedDimElement, &Falagard_xmlHandler::elementUnifiedDimStart);
        registerElementStartHandler(AbsoluteDimElement, &Falagard_xmlHandler::elementAbsoluteDimStart);
        registerElementStartHandler(ImageDimElement, &Falagard_xmlHandler::elementImageDimStart);
        registerElementStartHandler(ImagePropertyDimElement, &Falagard_xmlHandler::elementImagePropertyDimStart);
        registerElementStartHandler(WidgetDimElement, &Falagard_xmlHandler::elementWidgetDimStart);
        registerElementStartHandler(FontDimElement, &Falagard_xmlHandler::elementFontDimStart);
        registerElementStartHandler(PropertyDimElement, &Falagard_xmlHandler::elementPropertyDimStart);
        registerElementStartHandler(TextElement, &Falagard_xmlHandler::elementTextStart);
        registerElementStartHandler(ColourPropertyElement, &Falagard_xmlHandler::elementColourRectPropertyStart);
        registerElementStartHandler(ColourRectPropertyElement, &Falagard_xmlHandler::elementColourRectPropertyStart);
        registerElementStartHandler(NamedAreaElement, &Falagard_xmlHandler::elementNamedAreaStart);
        registerElementStartHandler(PropertyDefinitionElement, &Falagard_xmlHandler::elementPropertyDefinitionStart);
        registerElementStartHandler(PropertyLinkDefinitionElement, &Falagard_xmlHandler::elementPropertyLinkDefinitionStart);
        registerElementStartHandler(OperatorDimElement, &Falagard_xmlHandler::elementOperatorDimStart);
        registerElementStartHandler(VertFormatPropertyElement, &Falagard_xmlHandler::elementVertFormatPropertyStart);
        registerElementStartHandler(HorzFormatPropertyElement, &Falagard_xmlHandler::elementHorzFormatPropertyStart);
        registerElementStartHandler(AreaPropertyElement, &Falagard_xmlHandler::elementAreaPropertyStart);
        registerElementStartHandler(ImagePropertyElement, &Falagard_xmlHandler::elementImagePropertyStart);
        registerElementStartHandler(TextPropertyElement, &Falagard_xmlHandler::elementTextPropertyStart);
        registerElementStartHandler(FontPropertyElement, &Falagard_xmlHandler::elementFontPropertyStart);
        registerElementStartHandler(ColourElement, &Falagard_xmlHandler::elementColourStart);
        registerElementStartHandler(PropertyLinkTargetElement, &Falagard_xmlHandler::elementPropertyLinkTargetStart);
        registerElementStartHandler(AnimationDefinitionHandler::ElementName, &Falagard_xmlHandler::elementAnimationDefinitionStart);
        registerElementStartHandler(EventLinkDefinitionElement, &Falagard_xmlHandler::elementEventLinkDefinitionStart);
        registerElementStartHandler(EventLinkTargetElement, &Falagard_xmlHandler::elementEventLinkTargetStart);
        registerElementStartHandler(NamedAreaSourceElement, &Falagard_xmlHandler::elementNamedAreaSourceStart);
        registerElementStartHandler(EventActionElement, &Falagard_xmlHandler::elementEventActionStart);

        // register element end handlers
        registerElementEndHandler(FalagardElement, &Falagard_xmlHandler::elementFalagardEnd);
        registerElementEndHandler(WidgetLookElement, &Falagard_xmlHandler::elementWidgetLookEnd);
        registerElementEndHandler(ChildElement, &Falagard_xmlHandler::elementChildEnd);
        registerElementEndHandler(ImagerySectionElement, &Falagard_xmlHandler::elementImagerySectionEnd);
        registerElementEndHandler(StateImageryElement, &Falagard_xmlHandler::elementStateImageryEnd);
        registerElementEndHandler(LayerElement, &Falagard_xmlHandler::elementLayerEnd);
        registerElementEndHandler(SectionElement, &Falagard_xmlHandler::elementSectionEnd);
        registerElementEndHandler(ImageryComponentElement, &Falagard_xmlHandler::elementImageryComponentEnd);
        registerElementEndHandler(TextComponentElement, &Falagard_xmlHandler::elementTextComponentEnd);
        registerElementEndHandler(FrameComponentElement, &Falagard_xmlHandler::elementFrameComponentEnd);
        registerElementEndHandler(AreaElement, &Falagard_xmlHandler::elementAreaEnd);
        registerElementEndHandler(UnifiedDimElement, &Falagard_xmlHandler::elementAnyDimEnd);
        registerElementEndHandler(AbsoluteDimElement, &Falagard_xmlHandler::elementAnyDimEnd);
        registerElementEndHandler(ImageDimElement, &Falagard_xmlHandler::elementAnyDimEnd);
        registerElementEndHandler(ImagePropertyDimElement, &Falagard_xmlHandler::elementAnyDimEnd);
        registerElementEndHandler(WidgetDimElement, &Falagard_xmlHandler::elementAnyDimEnd);
        registerElementEndHandler(FontDimElement, &Falagard_xmlHandler::elementAnyDimEnd);
        registerElementEndHandler(PropertyDimElement, &Falagard_xmlHandler::elementAnyDimEnd);
        registerElementEndHandler(OperatorDimElement, &Falagard_xmlHandler::elementAnyDimEnd);
        registerElementEndHandler(NamedAreaElement, &Falagard_xmlHandler::elementNamedAreaEnd);
        registerElementEndHandler(PropertyLinkDefinitionElement, &Falagard_xmlHandler::elementPropertyLinkDefinitionEnd);
        registerElementEndHandler(EventLinkDefinitionElement, &Falagard_xmlHandler::elementEventLinkDefinitionEnd);
    }

    Falagard_xmlHandler::~Falagard_xmlHandler()
    {}

    /*************************************************************************
        Handle an opening XML element tag.
    *************************************************************************/
    void Falagard_xmlHandler::elementStartLocal(const String& element, const XMLAttributes& attributes)
    {
        // find registered handler for this element.
        ElementStartHandlerMap::const_iterator iter = d_startHandlersMap.find(element);

        // if a handler existed
        if (iter != d_startHandlersMap.end())
        {
            // call the handler for this element
            (this->*(iter->second))(attributes);
        }
        // no handler existed
        else
        {
            Logger::getSingleton().logEvent("Falagard::xmlHandler::elementStart - The unknown XML element '" + element + "' was encountered while processing the look and feel file.", Errors);
        }
    }

    /*************************************************************************
        Handle a closing XML element tag
    *************************************************************************/
    void Falagard_xmlHandler::elementEndLocal(const String& element)
    {
        // find registered handler for this element.
        ElementEndHandlerMap::const_iterator iter = d_endHandlersMap.find(element);

        // if a handler existed
        if (iter != d_endHandlersMap.end())
            // call the handler for this element
            (this->*(iter->second))();
    }

    /*************************************************************************
        Convert a hex string "AARRGGBB" to type argb_t
    *************************************************************************/
    argb_t Falagard_xmlHandler::hexStringToARGB(const String& str)
    {
        argb_t val;
        std::istringstream s(str.c_str());
        s >> std::hex >> val;

        return val;
    }

    /*************************************************************************
        Assign a dimension to a ComponentArea depending upon the dimension's
        type.
    *************************************************************************/
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
                CEGUI_THROW(InvalidRequestException(
                    "Invalid DimensionType specified for area component."));
            }
        }
    }

    /*************************************************************************
        Assign a ColourRect to the current element supporting such a thing
    *************************************************************************/
    void Falagard_xmlHandler::assignColours(const ColourRect& cols)
    {
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

    /*************************************************************************
        Method that performs common handling for all *Dim elements.
    *************************************************************************/
    void Falagard_xmlHandler::doBaseDimStart(const BaseDim* dim)
    {
        BaseDim* cloned = dim->clone();
        d_dimStack.push_back(cloned);
    }

    /*************************************************************************
        Method that handles the opening Falagard XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementFalagardStart(const XMLAttributes& attributes)
    {
        Logger::getSingleton().logEvent("===== Falagard 'root' element: look and feel parsing begins =====");

        const String version = attributes.getValueAsString(VersionAttribute, "unknown");

        if (version != NativeVersion)
        {
            CEGUI_THROW(InvalidRequestException(
                "You are attempting to load a looknfeel of version '" + version +
                "' but this CEGUI version is only meant to load looknfeels of "
                "version '" + NativeVersion + "'. Consider using the migrate.py "
                "script bundled with CEGUI Unified Editor to migrate your data."));
        }
    }

    /*************************************************************************
        Method that handles the opening WidgetLook XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementWidgetLookStart(const XMLAttributes& attributes)
    {
        if (d_widgetlook != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, WidgetLookElement, attributes.getValueAsString(NameAttribute));
        }

        d_widgetlook = CEGUI_NEW_AO WidgetLookFeel(attributes.getValueAsString(NameAttribute),
                                                   attributes.getValueAsString(InheritsAttribute));

        Logger::getSingleton().logEvent("---> Start of definition for widget look '" + d_widgetlook->getName() + "'.", Informative);
    }

    /*************************************************************************
        Method that handles the opening Child XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementChildStart(const XMLAttributes& attributes)
    {
        if (d_childcomponent != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, ChildElement, attributes.getValueAsString(NameAttribute));
        }

        d_childcomponent = CEGUI_NEW_AO WidgetComponent(
            attributes.getValueAsString(TypeAttribute),
            attributes.getValueAsString(LookAttribute),
            attributes.getValueAsString(NameSuffixAttribute),
            attributes.getValueAsString(RendererAttribute),
            attributes.getValueAsBool(AutoWindowAttribute, true));

        CEGUI_LOGINSANE("-----> Start of definition for child widget."
            " Type: " + d_childcomponent->getBaseWidgetType() +
            " Name: " + d_childcomponent->getWidgetName() +
            " Look: " + d_childcomponent->getWidgetLookName() +
            " Auto: " + (d_childcomponent->isAutoWindow() ? "Yes" : "No"));
    }

    /*************************************************************************
        Method that handles the opening ImagerySection XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementImagerySectionStart(const XMLAttributes& attributes)
    {
        if (d_imagerysection != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, ImagerySectionElement, attributes.getValueAsString(NameAttribute));
        }

        d_imagerysection = CEGUI_NEW_AO ImagerySection(attributes.getValueAsString(NameAttribute));

        CEGUI_LOGINSANE("-----> Start of definition for imagery section '" + d_imagerysection->getName() + "'.");
    }

    /*************************************************************************
        Method that handles the opening StateImagery XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementStateImageryStart(const XMLAttributes& attributes)
    {
        if (d_stateimagery != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, StateImageryElement, attributes.getValueAsString(NameAttribute));
        }

        d_stateimagery = CEGUI_NEW_AO StateImagery(attributes.getValueAsString(NameAttribute));
        d_stateimagery->setClippedToDisplay(!attributes.getValueAsBool(ClippedAttribute, true));

        CEGUI_LOGINSANE("-----> Start of definition for imagery for state '" + d_stateimagery->getName() + "'.");
    }

    /*************************************************************************
        Method that handles the opening Layer XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementLayerStart(const XMLAttributes& attributes)
    {
        if (d_layer != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, LayerElement, CEGUI::String("Priority: ") + attributes.getValueAsString(PriorityAttribute, "0"));
        }
        if (d_stateimagery == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, LayerElement, CEGUI::String("Priority: ") + attributes.getValueAsString(PriorityAttribute, "0"), StateImageryElement);
        }

        d_layer = CEGUI_NEW_AO LayerSpecification(attributes.getValueAsInteger(PriorityAttribute, 0));

        CEGUI_LOGINSANE("-------> Start of definition of new imagery layer, priority: " + attributes.getValueAsString(PriorityAttribute, "0"));
    }

    /*************************************************************************
        Method that handles the opening Section XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementSectionStart(const XMLAttributes& attributes)
    {
        if (d_section != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, SectionElement, attributes.getValueAsString(SectionNameAttribute));
        }

        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, SectionElement, attributes.getValueAsString(SectionNameAttribute), WidgetLookElement);
        }

        if (d_layer == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, SectionElement, d_section->getSectionName(), LayerElement);
        }

        String owner(attributes.getValueAsString(LookAttribute));
        d_section =
            CEGUI_NEW_AO SectionSpecification(owner.empty() ? d_widgetlook->getName() : owner,
                                     attributes.getValueAsString(SectionNameAttribute),
                                     attributes.getValueAsString(ControlPropertyAttribute),
                                     attributes.getValueAsString(ControlValueAttribute),
                                     attributes.getValueAsString(ControlWidgetAttribute));

        CEGUI_LOGINSANE("---------> Layer references imagery section '" + d_section->getSectionName() + "'.");
    }

    /*************************************************************************
        Method that handles the opening ImageryComponent XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementImageryComponentStart(const XMLAttributes&)
    {
        if (d_imagerycomponent != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, ImageryComponentElement);
        }

        if (d_imagerysection == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, ImageryComponentElement, ImagerySectionElement);
        }

        d_imagerycomponent = CEGUI_NEW_AO ImageryComponent();

        CEGUI_LOGINSANE("-------> Image component definition...");
    }

    /*************************************************************************
        Method that handles the opening TextComponent XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementTextComponentStart(const XMLAttributes&)
    {
        if (d_textcomponent != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, TextComponentElement);
        }

        if (d_imagerysection == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, TextComponentElement, ImagerySectionElement);
        }

        d_textcomponent = CEGUI_NEW_AO TextComponent();

        CEGUI_LOGINSANE("-------> Text component definition...");
    }

    /*************************************************************************
        Method that handles the opening FrameComponent XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementFrameComponentStart(const XMLAttributes&)
    {
        if (d_framecomponent != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, FrameComponentElement);
        }

        if (d_imagerysection == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, FrameComponentElement, ImagerySectionElement);
        }

        d_framecomponent = CEGUI_NEW_AO FrameComponent();

        CEGUI_LOGINSANE("-------> Frame component definition...");
    }

    /*************************************************************************
        Method that handles the opening Area XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementAreaStart(const XMLAttributes&)
    {
        if (d_area != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, AreaElement);
        }

        if ((d_childcomponent == 0) && (d_imagerycomponent == 0) && (d_textcomponent == 0) && d_namedArea == 0 && d_framecomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, AreaElement, ChildElement + " or " + ImageryComponentElement + " or " +
                TextComponentElement + " or " + NamedAreaElement + " or " + FrameComponentElement);
        }

        d_area = CEGUI_NEW_AO ComponentArea();
    }

    /*************************************************************************
        Method that handles the opening Image XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementImageStart(const XMLAttributes& attributes)
    {
        if (d_imagerycomponent)
        {
            d_imagerycomponent->setImage(attributes.getValueAsString(NameAttribute));
            CEGUI_LOGINSANE("---------> Using image: " + attributes.getValueAsString(NameAttribute));
        }
        else if (d_framecomponent)
        {
            d_framecomponent->setImage(
                FalagardXMLHelper<FrameImageComponent>::fromString(
                    attributes.getValueAsString(ComponentAttribute)),
                attributes.getValueAsString(NameAttribute));

            CEGUI_LOGINSANE("---------> Using image: " +
                attributes.getValueAsString(NameAttribute) + " for: " +
                attributes.getValueAsString(ComponentAttribute));
        }
    }

    /*************************************************************************
        Method that handles the opening Colours XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementColoursStart(const XMLAttributes& attributes)
    {
        ColourRect cols(
            hexStringToARGB(attributes.getValueAsString(TopLeftAttribute)),
            hexStringToARGB(attributes.getValueAsString(TopRightAttribute)),
            hexStringToARGB(attributes.getValueAsString(BottomLeftAttribute)),
            hexStringToARGB(attributes.getValueAsString(BottomRightAttribute)));

        assignColours(cols);
    }

    /*************************************************************************
        Method that handles the opening VertFormat XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementVertFormatStart(const XMLAttributes& attributes)
    {
        if (d_framecomponent)
        {
            const FrameImageComponent what =
                FalagardXMLHelper<FrameImageComponent>::fromString(
                    attributes.getValueAsString(ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::Background));
            const VerticalFormatting fmt =
                FalagardXMLHelper<VerticalFormatting>::fromString(
                    attributes.getValueAsString(TypeAttribute));

            switch(what)
            {
                case FIC_LEFT_EDGE:
                    d_framecomponent->setLeftEdgeFormatting(fmt);
                    break;
                case FIC_RIGHT_EDGE:
                    d_framecomponent->setRightEdgeFormatting(fmt);
                    break;
                case FIC_BACKGROUND:
                    d_framecomponent->setBackgroundVerticalFormatting(fmt);
                    break;
                default:
                    CEGUI_THROW(InvalidRequestException(
                        VertFormatElement + " within " +
                        FrameComponentElement + " may only be used for "
                        "LeftEdge, RightEdge or Background components. "
                        "Received: " +
                        attributes.getValueAsString(ComponentAttribute)));
            }
        }
        else if (d_imagerycomponent)
        {
            d_imagerycomponent->setVerticalFormatting(
                FalagardXMLHelper<VerticalFormatting>::fromString(
                    attributes.getValueAsString(TypeAttribute)));
        }
        else if (d_textcomponent)
        {
            d_textcomponent->setVerticalFormatting(
                FalagardXMLHelper<VerticalTextFormatting>::fromString(
                    attributes.getValueAsString(TypeAttribute)));
        }
    }

    /*************************************************************************
        Method that handles the opening HorzFormat XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementHorzFormatStart(const XMLAttributes& attributes)
    {
        if (d_framecomponent)
        {
            const FrameImageComponent what =
                FalagardXMLHelper<FrameImageComponent>::fromString(
                    attributes.getValueAsString(ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::Background));
            const HorizontalFormatting fmt =
                FalagardXMLHelper<HorizontalFormatting>::fromString(
                    attributes.getValueAsString(TypeAttribute));

            switch(what)
            {
                case FIC_TOP_EDGE:
                    d_framecomponent->setTopEdgeFormatting(fmt);
                    break;
                case FIC_BOTTOM_EDGE:
                    d_framecomponent->setBottomEdgeFormatting(fmt);
                    break;
                case FIC_BACKGROUND:
                    d_framecomponent->setBackgroundHorizontalFormatting(fmt);
                    break;
                default:
                    CEGUI_THROW(InvalidRequestException(
                        HorzFormatElement + " within " +
                        FrameComponentElement + " may only be used for "
                        "TopEdge, BottomEdge or Background components. "
                        "Received: " +
                        attributes.getValueAsString(ComponentAttribute)));
            }
        }
        else if (d_imagerycomponent)
        {
            d_imagerycomponent->setHorizontalFormatting(
                FalagardXMLHelper<HorizontalFormatting>::fromString(
                    attributes.getValueAsString(TypeAttribute)));
        }
        else if (d_textcomponent)
        {
            d_textcomponent->setHorizontalFormatting(
                FalagardXMLHelper<HorizontalTextFormatting>::fromString(
                    attributes.getValueAsString(TypeAttribute)));
        }
    }

    /*************************************************************************
        Method that handles the opening VertAlignment XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementVertAlignmentStart(const XMLAttributes& attributes)
    {
        if (d_childcomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, VertAlignmentElement, ChildElement);
        }

        d_childcomponent->setVerticalWidgetAlignment(
            FalagardXMLHelper<VerticalAlignment>::fromString(
                attributes.getValueAsString(TypeAttribute)));
    }

    /*************************************************************************
        Method that handles the opening HorzAlignment XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementHorzAlignmentStart(const XMLAttributes& attributes)
    {
        if (d_childcomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, HorzAlignmentElement, ChildElement);
        }

        d_childcomponent->setHorizontalWidgetAlignment(
            FalagardXMLHelper<HorizontalAlignment>::fromString(
                attributes.getValueAsString(TypeAttribute)));
    }

    /*************************************************************************
        Method that handles the opening Property XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementPropertyStart(const XMLAttributes& attributes)
    {
        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, PropertyElement, attributes.getValueAsString(NameAttribute), WidgetLookElement);
        }

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

    /*************************************************************************
        Method that handles the opening Dim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementDimStart(const XMLAttributes& attributes)
    {
        d_dimension.setDimensionType(
            FalagardXMLHelper<DimensionType>::fromString(
                attributes.getValueAsString(TypeAttribute)));
    }

    /*************************************************************************
        Method that handles the opening UnifiedDim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementUnifiedDimStart(const XMLAttributes& attributes)
    {
        UnifiedDim base(
            UDim(attributes.getValueAsFloat(ScaleAttribute, 0.0f),
                 attributes.getValueAsFloat(OffsetAttribute, 0.0f)),
            FalagardXMLHelper<DimensionType>::fromString(
                attributes.getValueAsString(TypeAttribute)));

        doBaseDimStart(&base);
    }

    /*************************************************************************
        Method that handles the opening AbsoluteDim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementAbsoluteDimStart(const XMLAttributes& attributes)
    {
        AbsoluteDim base(attributes.getValueAsFloat(ValueAttribute, 0.0f));
        doBaseDimStart(&base);
    }

    /*************************************************************************
        Method that handles the opening ImageDim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementImageDimStart(const XMLAttributes& attributes)
    {
        ImageDim base(attributes.getValueAsString(NameAttribute),
                      FalagardXMLHelper<DimensionType>::fromString(
                          attributes.getValueAsString(DimensionAttribute)));

        doBaseDimStart(&base);
    }

    /*************************************************************************
        Method that handles the opening ImageDim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementImagePropertyDimStart(const XMLAttributes& attributes)
    {
        ImagePropertyDim base(
            attributes.getValueAsString(NameAttribute),
            FalagardXMLHelper<DimensionType>::fromString(
                attributes.getValueAsString(DimensionAttribute)));

        doBaseDimStart(&base);
    }

    /*************************************************************************
        Method that handles the opening WidgetDim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementWidgetDimStart(const XMLAttributes& attributes)
    {
        WidgetDim base(attributes.getValueAsString(WidgetAttribute),
                       FalagardXMLHelper<DimensionType>::fromString(
                           attributes.getValueAsString(DimensionAttribute)));

        doBaseDimStart(&base);
    }

    /*************************************************************************
        Method that handles the opening FontDim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementFontDimStart(const XMLAttributes& attributes)
    {
        FontDim base(
            attributes.getValueAsString(WidgetAttribute),
            attributes.getValueAsString(FontAttribute),
            attributes.getValueAsString(StringAttribute),
            FalagardXMLHelper<FontMetricType>::fromString(
                attributes.getValueAsString(TypeAttribute)),
            attributes.getValueAsFloat(PaddingAttribute, 0.0f));

        doBaseDimStart(&base);
    }

    /*************************************************************************
        Method that handles the opening PropertyDim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementPropertyDimStart(const XMLAttributes& attributes)
    {
        String str_type = attributes.getValueAsString(TypeAttribute);
        DimensionType type = DT_INVALID;
        if (!str_type.empty())
            type = FalagardXMLHelper<DimensionType>::fromString(str_type);

        PropertyDim base(attributes.getValueAsString(WidgetAttribute),
                         attributes.getValueAsString(NameAttribute),
                         type);

        doBaseDimStart(&base);
    }

    /*************************************************************************
        Method that handles the opening Text XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementTextStart(const XMLAttributes& attributes)
    {
        if (d_textcomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, TextElement, attributes.getValueAsString(NameAttribute), TextComponentElement);
        }

        d_textcomponent->setText(attributes.getValueAsString(StringAttribute));
        d_textcomponent->setFont(attributes.getValueAsString(FontAttribute));
    }

    /*************************************************************************
        Method that handles the opening ColourRectProperty XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementColourRectPropertyStart(const XMLAttributes& attributes)
    {
        // need to decide what to apply colours to
        if (d_framecomponent)
        {
            d_framecomponent->setColoursPropertySource(attributes.getValueAsString(NameAttribute));
        }
        else if (d_imagerycomponent)
        {
            d_imagerycomponent->setColoursPropertySource(attributes.getValueAsString(NameAttribute));
        }
        else if (d_textcomponent)
        {
            d_textcomponent->setColoursPropertySource(attributes.getValueAsString(NameAttribute));
        }
        else if (d_imagerysection)
        {
            d_imagerysection->setMasterColoursPropertySource(attributes.getValueAsString(NameAttribute));
        }
        else if (d_section)
        {
            d_section->setOverrideColoursPropertySource(attributes.getValueAsString(NameAttribute));
            d_section->setUsingOverrideColours(true);
        }
    }

    /*************************************************************************
        Method that handles the opening NamedArea XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementNamedAreaStart(const XMLAttributes& attributes)
    {
        if (d_namedArea != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, NamedAreaElement, attributes.getValueAsString(NameAttribute));
        }

        d_namedArea = CEGUI_NEW_AO NamedArea(attributes.getValueAsString(NameAttribute));

        CEGUI_LOGINSANE("-----> Creating named area: " + d_namedArea->getName());
    }

    /*************************************************************************
        Method that handles the opening PropertyDefinition XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementPropertyDefinitionStart(const XMLAttributes& attributes)
    {
        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, PropertyDefinitionElement, attributes.getValueAsString(NameAttribute), WidgetLookElement);
        }


        PropertyDefinitionBase* prop;

        const String name(attributes.getValueAsString(NameAttribute));
        const String init(attributes.getValueAsString(InitialValueAttribute));
        const String help(attributes.getValueAsString(HelpStringAttribute,
                                                      PropertyDefinitionHelpDefaultValue));
        const String type(attributes.getValueAsString(TypeAttribute, GenericDataType));
        bool redraw(attributes.getValueAsBool(RedrawOnWriteAttribute, false));
        bool layout(attributes.getValueAsBool(LayoutOnWriteAttribute, false));
        const String eventName(attributes.getValueAsString(FireEventAttribute));
        typedef std::pair<float, float> Range;

        if(type == PropertyHelper<Colour>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Colour>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName() );
        else if(type == PropertyHelper<ColourRect>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<ColourRect>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<UBox>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<UBox>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<URect>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<URect>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<USize>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<USize>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<UDim>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<UDim>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<UVector2>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<UVector2>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Sizef>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Sizef>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Vector2f>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Vector2f>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Vector3f>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Vector3f>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Rectf>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Rectf>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Font*>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Font*>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Image*>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Image*>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Quaternion>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Quaternion>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<AspectMode>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<AspectMode>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<HorizontalAlignment>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<HorizontalAlignment>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<VerticalAlignment>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<VerticalAlignment>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<HorizontalTextFormatting>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<HorizontalTextFormatting>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<VerticalTextFormatting>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<VerticalTextFormatting>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<WindowUpdateMode>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<WindowUpdateMode>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<bool>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<bool>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<uint>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<uint>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<unsigned long>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<unsigned long>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<int>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<int>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<float>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<float>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<double>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<double>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<TabControl::TabPanePosition>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<TabControl::TabPanePosition>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Spinner::TextInputMode>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Spinner::TextInputMode>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<ItemListBase::SortMode>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<ItemListBase::SortMode>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<ListHeaderSegment::SortDirection>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<ListHeaderSegment::SortDirection>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<MultiColumnList::SelectionMode>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<MultiColumnList::SelectionMode>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<VerticalFormatting>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<VerticalFormatting>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<HorizontalFormatting>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<HorizontalFormatting>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if(type == PropertyHelper<Range>::getDataTypeName())
            prop = CEGUI_NEW_AO PropertyDefinition<Range>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else
        {
            if (type != GenericDataType && type != "String")
            {
                // type was specified but wasn't recognised
                Logger::getSingleton().logEvent("Type '" + type + "' wasn't recognized in property definition (name: '" + name + "').", Warnings);
            }

            prop = CEGUI_NEW_AO PropertyDefinition<String>(name, init, help, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        }

        CEGUI_LOGINSANE("-----> Adding PropertyDefiniton. Name: " + name + " Default Value: " + init);

        d_widgetlook->addPropertyDefinition(prop);
    }

    /*************************************************************************
        Method that handles the opening PropertyLinkDefinition XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementPropertyLinkDefinitionStart(const XMLAttributes& attributes)
    {
        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, PropertyLinkDefinitionElement, attributes.getValueAsString(NameAttribute), WidgetLookElement);
        }
        
        if (d_propertyLink != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, PropertyLinkDefinitionElement);
        }


        const String widget(attributes.getValueAsString(WidgetAttribute));
        const String target(attributes.getValueAsString(TargetPropertyAttribute));
        const String name(attributes.getValueAsString(NameAttribute));
        const String init(attributes.getValueAsString(InitialValueAttribute));
        const String type(attributes.getValueAsString(TypeAttribute, GenericDataType));
        bool redraw(attributes.getValueAsBool(RedrawOnWriteAttribute, false));
        bool layout(attributes.getValueAsBool(LayoutOnWriteAttribute, false));
        const String eventName(attributes.getValueAsString(FireEventAttribute));
        typedef std::pair<float, float> Range;

        if (type == PropertyHelper<Colour>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Colour>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<ColourRect>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<ColourRect>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<UBox>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<UBox>(name, widget,
                    target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<URect>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<URect>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<USize>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<USize>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<UDim>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<UDim>(name, widget,
                    target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<UVector2>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<UVector2>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<Sizef>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Sizef>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<Vector2f>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Vector2f>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<Vector3f>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Vector3f>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<Rectf>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Rectf>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<Font*>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Font*>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<Image*>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Image*>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<Quaternion>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Quaternion>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<AspectMode>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<AspectMode>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<HorizontalAlignment>::getDataTypeName())
            d_propertyLink =CEGUI_NEW_AO PropertyLinkDefinition<HorizontalAlignment>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<VerticalAlignment>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<VerticalAlignment>(
                    name, widget, target, init, d_widgetlook->getName(), redraw,
                    layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<HorizontalTextFormatting>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<
                    HorizontalTextFormatting>(name, widget, target, init,
                    d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<VerticalTextFormatting>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<
                    VerticalTextFormatting>(name, widget, target, init,
                    d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<WindowUpdateMode>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<WindowUpdateMode>(
                    name, widget, target, init, d_widgetlook->getName(), redraw,
                    layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<bool>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<bool>(name, widget,
                    target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<uint>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<uint>(name, widget,
                    target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<unsigned long>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<unsigned long>(
                    name, widget, target, init, d_widgetlook->getName(), redraw,
                    layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<int>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<int>(name, widget,
                    target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<float>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<float>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<double>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<double>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<TabControl::TabPanePosition>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<
                    TabControl::TabPanePosition>(name, widget, target, init,
                    d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<Spinner::TextInputMode>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<
                    Spinner::TextInputMode>(name, widget, target, init,
                    d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<ItemListBase::SortMode>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<
                    ItemListBase::SortMode>(name, widget, target, init,
                    d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<ListHeaderSegment::SortDirection>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<
                    ListHeaderSegment::SortDirection>(name, widget, target, init,
                    d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<MultiColumnList::SelectionMode>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<
                    MultiColumnList::SelectionMode>(name, widget, target, init,
                    d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else if (type == PropertyHelper<VerticalFormatting>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<VerticalFormatting>(
                    name, widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName()
            );
        else if (type == PropertyHelper<HorizontalFormatting>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<HorizontalFormatting>(
                    name, widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName()
            );
        else if (type == PropertyHelper<Range>::getDataTypeName())
            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<Range>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        else
        {
            if (type != GenericDataType && type != PropertyHelper<String>::getDataTypeName())
            {
                // type was specified but wasn't recognised
                Logger::getSingleton().logEvent("Type '" + type + "' wasn't recognized in property link definition (name: '" + name + "').", Warnings);
            }

            d_propertyLink = CEGUI_NEW_AO PropertyLinkDefinition<String>(name,
                    widget, target, init, d_widgetlook->getName(), redraw, layout, eventName, d_widgetlook->getName());
        }
        CEGUI_LOGINSANE("-----> Adding PropertyLinkDefiniton. Name: " +
                        name);

        if (!widget.empty() || !target.empty())
        {
            CEGUI_LOGINSANE("-------> Adding link target to property: " + target +
                        " on widget: " + widget);
        }
    }

    /*************************************************************************
        Method that handles the opening OperatorDim XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementOperatorDimStart(const XMLAttributes& attributes)
    {
        OperatorDim base(FalagardXMLHelper<DimensionOperator>::fromString(
            attributes.getValueAsString(OperatorAttribute)));

        doBaseDimStart(&base);
    }

    /*************************************************************************
        Method that handles the opening VertFormatProperty XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementVertFormatPropertyStart(const XMLAttributes& attributes)
    {
        if (d_framecomponent)
        {
            const FrameImageComponent what =
                FalagardXMLHelper<FrameImageComponent>::fromString(
                    attributes.getValueAsString(ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::Background));
            const VerticalFormatting fmt =
                FalagardXMLHelper<VerticalFormatting>::fromString(
                    attributes.getValueAsString(TypeAttribute));

            switch(what)
            {
                case FIC_LEFT_EDGE:
                    d_framecomponent->setLeftEdgeFormatting(fmt);
                    break;
                case FIC_RIGHT_EDGE:
                    d_framecomponent->setRightEdgeFormatting(fmt);
                    break;
                case FIC_BACKGROUND:
                    d_framecomponent->setBackgroundVerticalFormatting(fmt);
                    break;
                default:
                    CEGUI_THROW(InvalidRequestException(
                        VertFormatPropertyElement + " within " +
                        FrameComponentElement + " may only be used for "
                        "LeftEdge, RightEdge or Background components. "
                        "Received: " +
                        attributes.getValueAsString(ComponentAttribute)));
            }
        }
        else if (d_imagerycomponent)
            d_imagerycomponent->setVerticalFormattingPropertySource(attributes.getValueAsString(NameAttribute));
        else if (d_textcomponent)
            d_textcomponent->setVerticalFormattingPropertySource(attributes.getValueAsString(NameAttribute));
    }

    /*************************************************************************
        Method that handles the opening HorzFormatProperty XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementHorzFormatPropertyStart(const XMLAttributes& attributes)
    {
        if (d_framecomponent)
        {
            const FrameImageComponent what =
                FalagardXMLHelper<FrameImageComponent>::fromString(
                    attributes.getValueAsString(ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::Background));
            const HorizontalFormatting fmt =
                FalagardXMLHelper<HorizontalFormatting>::fromString(
                    attributes.getValueAsString(TypeAttribute));

            switch(what)
            {
                case FIC_TOP_EDGE:
                    d_framecomponent->setTopEdgeFormatting(fmt);
                    break;
                case FIC_BOTTOM_EDGE:
                    d_framecomponent->setBottomEdgeFormatting(fmt);
                    break;
                case FIC_BACKGROUND:
                    d_framecomponent->setBackgroundHorizontalFormatting(fmt);
                    break;
                default:
                    CEGUI_THROW(InvalidRequestException(
                        HorzFormatPropertyElement + " within " +
                        FrameComponentElement + " may only be used for "
                        "TopEdge, BottomEdge or Background components. "
                        "Received: " +
                        attributes.getValueAsString(ComponentAttribute)));
            }
        }
        else if (d_imagerycomponent)
            d_imagerycomponent->setHorizontalFormattingPropertySource(attributes.getValueAsString(NameAttribute));
        else if (d_textcomponent)
            d_textcomponent->setHorizontalFormattingPropertySource(attributes.getValueAsString(NameAttribute));
    }

    /*************************************************************************
        Method that handles the opening AreaProperty XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementAreaPropertyStart(const XMLAttributes& attributes)
    {
        if (d_area != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, AreaPropertyElement);
        }

        d_area->setAreaPropertySource(attributes.getValueAsString(NameAttribute));
    }

    /*************************************************************************
        Method that handles the opening ImageProperty XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementImagePropertyStart(const XMLAttributes& attributes)
    {
        if (d_imagerycomponent == 0 && d_framecomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, ImagePropertyElement, attributes.getValueAsString(NameAttribute), 
                ImageryComponentElement + " or " + FrameComponentElement);
        }

        if (d_imagerycomponent)
        {
            d_imagerycomponent->setImagePropertySource(
                attributes.getValueAsString(NameAttribute));

            CEGUI_LOGINSANE("---------> Using image via property: " +
                attributes.getValueAsString(NameAttribute));
        }
        else if (d_framecomponent)
        {
            d_framecomponent->setImagePropertySource(
                FalagardXMLHelper<FrameImageComponent>::fromString(
                    attributes.getValueAsString(ComponentAttribute)),
                attributes.getValueAsString(NameAttribute));

            CEGUI_LOGINSANE("---------> Using image via property: " +
                attributes.getValueAsString(NameAttribute) + " for: " +
                attributes.getValueAsString(ComponentAttribute));
        }
    }

    /*************************************************************************
        Method that handles the opening TextProperty XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementTextPropertyStart(const XMLAttributes& attributes)
    {
        if (d_textcomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, FontPropertyElement, attributes.getValueAsString(NameAttribute), TextComponentElement);
        }

        d_textcomponent->setTextPropertySource(attributes.getValueAsString(NameAttribute));
    }

    /*************************************************************************
        Method that handles the opening FontProperty XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementFontPropertyStart(const XMLAttributes& attributes)
    {
        if (d_textcomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, FontPropertyElement, attributes.getValueAsString(NameAttribute), TextComponentElement);
        }

        d_textcomponent->setFontPropertySource(attributes.getValueAsString(NameAttribute));
    }

    /*************************************************************************
        Method that handles the opening Colours XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementColourStart(const XMLAttributes& attributes)
    {
        ColourRect cols(hexStringToARGB(attributes.getValueAsString(ColourAttribute)));
        assignColours(cols);
    }

    /*************************************************************************
        Method that handles the closing Falagard XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementFalagardEnd()
    {
        Logger::getSingleton().logEvent("===== Look and feel parsing completed =====");
    }

    /*************************************************************************
        Method that handles the closing WidgetLook XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementWidgetLookEnd()
    {
        assert(d_widgetlook != 0);

        Logger::getSingleton().logEvent("---< End of definition for widget look '" + d_widgetlook->getName() + "'.", Informative);
        d_manager->addWidgetLook(*d_widgetlook);
        CEGUI_DELETE_AO d_widgetlook;
        d_widgetlook = 0;
    }

    /*************************************************************************
        Method that handles the closing Child XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementChildEnd()
    {
        assert(d_childcomponent != 0);

        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, ChildElement, d_childcomponent->getWidgetName(), WidgetLookElement);
        }

        CEGUI_LOGINSANE("-----< End of definition for child widget. Type: " + d_childcomponent->getBaseWidgetType() + ".");
        d_widgetlook->addWidgetComponent(*d_childcomponent);
        CEGUI_DELETE_AO d_childcomponent;
        d_childcomponent = 0;
    }

    /*************************************************************************
        Method that handles the closing ImagerySection XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementImagerySectionEnd()
    {
        assert(d_imagerysection != 0);

        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, ImagerySectionElement, d_imagerysection->getName(), WidgetLookElement);
        }

        CEGUI_LOGINSANE("-----< End of definition for imagery section '" + d_imagerysection->getName() + "'.");
        d_widgetlook->addImagerySection(*d_imagerysection);
        CEGUI_DELETE_AO d_imagerysection;
        d_imagerysection = 0;
    }

    /*************************************************************************
        Method that handles the closing StateImagery XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementStateImageryEnd()
    {
        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, StateImageryElement, d_stateimagery->getName(), WidgetLookElement);
        }

        if (d_stateimagery)
        {
            CEGUI_LOGINSANE("-----< End of definition for imagery for state '" + d_stateimagery->getName() + "'.");
            d_widgetlook->addStateSpecification(*d_stateimagery);
            CEGUI_DELETE_AO d_stateimagery;
            d_stateimagery = 0;
        }
    }

    /*************************************************************************
        Method that handles the closing Layer XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementLayerEnd()
    {
        assert(d_layer != 0);

        if (d_stateimagery == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, LayerElement, StateImageryElement);
        }

        CEGUI_LOGINSANE("-------< End of definition of imagery layer.");
        d_stateimagery->addLayer(*d_layer);
        CEGUI_DELETE_AO d_layer;
        d_layer = 0;
    }

    /*************************************************************************
        Method that handles the closing Section XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementSectionEnd()
    {
        assert(d_section != 0);

        if (d_layer == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, SectionElement, d_section->getSectionName(), LayerElement);
        }

        d_layer->addSectionSpecification(*d_section);
        CEGUI_DELETE_AO d_section;
        d_section = 0;
    }

    /*************************************************************************
        Method that handles the closing ImageryComponent XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementImageryComponentEnd()
    {
        assert(d_imagerycomponent != 0);

        if (d_imagerysection == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, ImageryComponentElement, ImagerySectionElement);
        }

        d_imagerysection->addImageryComponent(*d_imagerycomponent);
        CEGUI_DELETE_AO d_imagerycomponent;
        d_imagerycomponent = 0;
    }

    /*************************************************************************
        Method that handles the closing TextComponent XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementTextComponentEnd()
    {
        assert(d_textcomponent != 0);

        if (d_imagerysection == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, TextComponentElement, ImagerySectionElement);
        }

        d_imagerysection->addTextComponent(*d_textcomponent);
        CEGUI_DELETE_AO d_textcomponent;
        d_textcomponent = 0;
    }

    /*************************************************************************
        Method that handles the closing FrameComponent XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementFrameComponentEnd()
    {
        assert(d_framecomponent != 0);

        if (d_imagerysection == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, FrameComponentElement, ImagerySectionElement);
        }

        d_imagerysection->addFrameComponent(*d_framecomponent);
        CEGUI_DELETE_AO d_framecomponent;
        d_framecomponent = 0;
    }

    /*************************************************************************
        Method that handles the closing Area XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementAreaEnd()
    {
        assert(d_area != 0);

        if ((d_childcomponent == 0) && (d_imagerycomponent == 0) && (d_textcomponent == 0) && d_namedArea == 0 && d_framecomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, AreaElement, ChildElement + " or " + ImageryComponentElement + " or " +
                TextComponentElement + " or " + NamedAreaElement + " or " + FrameComponentElement);
        }   

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

        CEGUI_DELETE_AO d_area;
        d_area = 0;
    }

    /*************************************************************************
        Method that handles the closing NamedArea XML element.
    *************************************************************************/
    void Falagard_xmlHandler::elementNamedAreaEnd()
    {
        assert(d_namedArea != 0);

        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, NamedAreaElement, d_namedArea->getName(), WidgetLookElement);
        }

        d_widgetlook->addNamedArea(*d_namedArea);
        CEGUI_DELETE_AO d_namedArea;
        d_namedArea = 0;
    }

    /*************************************************************************
        Method that handles the closing of all *Dim XML elements.
    *************************************************************************/
    void Falagard_xmlHandler::elementAnyDimEnd()
    {
        if (!d_dimStack.empty())
        {
            BaseDim* currDim = d_dimStack.back();
            d_dimStack.pop_back();

            if (!d_dimStack.empty())
            {
                if (OperatorDim* op = dynamic_cast<OperatorDim*>(d_dimStack.back()))
                   op->setNextOperand(currDim);
            }
            else
            {
                d_dimension.setBaseDimension(*currDim);
                assignAreaDimension(d_dimension);
            }

            // release the dim we popped.
            CEGUI_DELETE_AO currDim;
        }
    }

    void Falagard_xmlHandler::elementPropertyLinkDefinitionEnd()
    {
        assert(d_propertyLink);

        d_widgetlook->addPropertyLinkDefinition(d_propertyLink);

        CEGUI_LOGINSANE("<----- End of PropertyLinkDefiniton. Name: " +
                        d_propertyLink->getPropertyName());
        d_propertyLink = 0;
    }

    void Falagard_xmlHandler::elementPropertyLinkTargetStart(const XMLAttributes& attributes)
    {
        if (d_propertyLink == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, PropertyLinkTargetElement, attributes.getValueAsString(PropertyAttribute), PropertyLinkDefinitionElement);
        }

        const String w(attributes.getValueAsString(WidgetAttribute));
        const String p(attributes.getValueAsString(PropertyAttribute));

        if (!w.empty() || !p.empty())
        {
            const String type(dynamic_cast<Property*>(d_propertyLink)->getDataType());

            typedef std::pair<float,float> Range;

            if(type == PropertyHelper<Colour>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Colour>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<ColourRect>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<ColourRect>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<UBox>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<UBox>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<URect>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<URect>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<USize>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<USize>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<UDim>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<UDim>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<UVector2>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<UVector2>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<Sizef>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Sizef>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<Vector2f>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Vector2f>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<Vector3f>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Vector3f>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<Rectf>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Rectf>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<Font*>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Font*>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<Image*>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Image*>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<Quaternion>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Quaternion>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<AspectMode>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<AspectMode>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<HorizontalAlignment>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<HorizontalAlignment>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<VerticalAlignment>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<VerticalAlignment>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<HorizontalTextFormatting>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<HorizontalTextFormatting>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<VerticalTextFormatting>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<VerticalTextFormatting>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<WindowUpdateMode>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<WindowUpdateMode>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<bool>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<bool>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<uint>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<uint>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<unsigned long>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<unsigned long>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<uint>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<uint>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<int>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<int>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<float>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<float>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<double>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<double>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<TabControl::TabPanePosition>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<TabControl::TabPanePosition>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<Spinner::TextInputMode>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Spinner::TextInputMode>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<ItemListBase::SortMode>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<ItemListBase::SortMode>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<ListHeaderSegment::SortDirection>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<ListHeaderSegment::SortDirection>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<MultiColumnList::SelectionMode>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<MultiColumnList::SelectionMode>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<VerticalFormatting>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<VerticalFormatting>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<HorizontalFormatting>::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<HorizontalFormatting>* >(d_propertyLink)->addLinkTarget(w, p);
            else if(type == PropertyHelper<std::pair<float,float> >::getDataTypeName())
                dynamic_cast<PropertyLinkDefinition<Range>* >(d_propertyLink)->addLinkTarget(w, p);
            else
                dynamic_cast<PropertyLinkDefinition<String>* >(d_propertyLink)->addLinkTarget(w, p);

            CEGUI_LOGINSANE("-------> Adding link target to property: " + p +
                        " on widget: " + w);
        }
    }

    void Falagard_xmlHandler::elementAnimationDefinitionStart(
                                            const XMLAttributes& attributes)
    {
        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, AnimationDefinitionHandler::ElementName, attributes.getValueAsString(NameAttribute), WidgetLookElement);
        }

        String anim_name_prefix(d_widgetlook->getName());
        anim_name_prefix.append("/");

        const String anim_name(anim_name_prefix +
                        attributes.getValueAsString(NameAttribute));


        if (AnimationManager::getSingleton().isAnimationPresent(anim_name))
        {
            Logger::getSingleton().logEvent(
                "[XMLHandler] WARNING: Using existing Animation :" + anim_name);
        }
        else
        {
            d_chainedHandler = CEGUI_NEW_AO AnimationDefinitionHandler(
                attributes, anim_name_prefix);
        }

        // This is a little bit of abuse here, ideally we would get the name
        // somewhere else.
        d_widgetlook->addAnimationName(
            anim_name_prefix +
            attributes.getValueAsString("name"));
    }


    void Falagard_xmlHandler::elementEventLinkDefinitionStart(
                                                const XMLAttributes& attributes)
    {
        if (d_widgetlook == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, EventLinkDefinitionElement, attributes.getValueAsString(NameAttribute), WidgetLookElement);
        }

        if (d_eventLink != 0)
        {
            throwExceptionChildOfSameNode(d_widgetlook, EventLinkDefinitionElement);
        }

        const String widget(attributes.getValueAsString(WidgetAttribute));
        const String event(attributes.getValueAsString(EventAttribute));

        d_eventLink = CEGUI_NEW_AO EventLinkDefinition(
            attributes.getValueAsString(NameAttribute));

        CEGUI_LOGINSANE("-----> Adding EventLinkDefiniton. Name: " +
                        d_eventLink->getName());

        processEventLinkTarget(widget, event);
    }

    void Falagard_xmlHandler::processEventLinkTarget(const String& widget, const String& event)
    {
        if (d_eventLink == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, EventLinkTargetElement, EventLinkDefinitionElement);
        }

        if (!widget.empty() || !event.empty())
        {
            d_eventLink->addLinkTarget(widget, event);
            CEGUI_LOGINSANE("-------> Adding link target to event: " + event +
                        " on widget: " + widget);
        }
    }

    void Falagard_xmlHandler::elementEventLinkTargetStart(
                                                const XMLAttributes& attributes)
    {
        const String widget(attributes.getValueAsString(WidgetAttribute));
        const String event(attributes.getValueAsString(EventAttribute));

        processEventLinkTarget(widget, event);
    }

    void Falagard_xmlHandler::elementEventLinkDefinitionEnd()
    {
        assert(d_eventLink);

        d_widgetlook->addEventLinkDefinition(*d_eventLink);

        CEGUI_LOGINSANE("<----- End of EventLinkDefiniton. Name: " +
                        d_eventLink->getName());

        CEGUI_DELETE_AO d_eventLink;
        d_eventLink = 0;
    }

    void Falagard_xmlHandler::elementNamedAreaSourceStart(const XMLAttributes& attributes)
    {
        if (d_area == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, NamedAreaElement, AreaElement);
        }

        const String look(attributes.getValueAsString(LookAttribute));

        d_area->setNamedAreaSouce(look.empty() ? d_widgetlook->getName() : look,
                                  attributes.getValueAsString(NameAttribute));
    }

    void Falagard_xmlHandler::elementEventActionStart(const XMLAttributes& attributes)
    {
        if (d_childcomponent == 0)
        {
            throwExceptionNotChildOfNode(d_widgetlook, EventActionElement, ChildElement);
        }

        const EventAction action(
            attributes.getValueAsString(EventAttribute),
            FalagardXMLHelper<ChildEventAction>::fromString(
                attributes.getValueAsString(ActionAttribute)));

        d_childcomponent->addEventAction(action);
    }

    /*************************************************************************
        register a handler for the opening tag of an XML element
    *************************************************************************/
    void Falagard_xmlHandler::registerElementStartHandler(const String& element, ElementStartHandler handler)
    {
        d_startHandlersMap[element] = handler;
    }

    /*************************************************************************
        register a handler for the closing tag of an XML element
    *************************************************************************/
    void Falagard_xmlHandler::registerElementEndHandler(const String& element, ElementEndHandler handler)
    {
        d_endHandlersMap[element] = handler;
    }

} // End of  CEGUI namespace section
