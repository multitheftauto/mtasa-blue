/***********************************************************************
    created:    Mon Jun 13 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/falagard/SectionSpecification.h"
#include "CEGUI/falagard/ImagerySection.h"
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/WindowManager.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    // This is deprecated and declared as an extern in PropertyLinkDefinition.h
    const String S_parentIdentifier("__parent__");

    SectionSpecification::SectionSpecification() :
        d_usingColourOverride(false)
    {}

    SectionSpecification::SectionSpecification(const String& owner,
                                               const String& sectionName,
                                               const String& controlPropertySource,
                                               const String& controlPropertyValue,
                                               const String& controlPropertyWidget) :
        d_owner(owner),
        d_sectionName(sectionName),
        d_usingColourOverride(false),
        d_renderControlProperty(controlPropertySource),
        d_renderControlValue(controlPropertyValue),
        d_renderControlWidget(controlPropertyWidget)
    {}

    SectionSpecification::SectionSpecification(const String& owner,
                                               const String& sectionName,
                                               const String& controlPropertySource,
                                               const String& controlPropertyValue,
                                               const String& controlPropertyWidget,
                                               const ColourRect& cols) :
        d_owner(owner),
        d_sectionName(sectionName),
        d_coloursOverride(cols),
        d_usingColourOverride(true),
        d_renderControlProperty(controlPropertySource),
        d_renderControlValue(controlPropertyValue),
        d_renderControlWidget(controlPropertyWidget)
    {}

    void SectionSpecification::render(Window& srcWindow,
                                      const ColourRect* modcols,
                                      const Rectf* clipper,
                                      bool clipToDisplay) const
    {
        // see if we need to bother rendering
        if (!shouldBeDrawn(srcWindow))
            return;

        CEGUI_TRY
        {
            // get the imagery section object with the name we're set up to use
            const ImagerySection* sect =
                &WidgetLookManager::getSingleton().getWidgetLook(d_owner).getImagerySection(d_sectionName);

            // decide what colours are to be used
            ColourRect finalColours;
            initColourRectForOverride(srcWindow, finalColours);
            finalColours.modulateAlpha(srcWindow.getEffectiveAlpha());

            if (modcols)
                finalColours *= *modcols;

            // render the imagery section
            sect->render(srcWindow, &finalColours, clipper, clipToDisplay);
        }
        // do nothing here, errors are non-faltal and are logged for debugging purposes.
        CEGUI_CATCH (Exception&)
        {}
    }

    void SectionSpecification::render(Window& srcWindow, const Rectf& baseRect,
                                      const ColourRect* modcols,
                                      const Rectf* clipper,
                                      bool clipToDisplay) const
    {
        // see if we need to bother rendering
        if (!shouldBeDrawn(srcWindow))
            return;

        CEGUI_TRY
        {
            // get the imagery section object with the name we're set up to use
            const ImagerySection* sect =
                &WidgetLookManager::getSingleton().getWidgetLook(d_owner).getImagerySection(d_sectionName);

            // decide what colours are to be used
            ColourRect finalColours;
            initColourRectForOverride(srcWindow, finalColours);
            finalColours.modulateAlpha(srcWindow.getEffectiveAlpha());

            if (modcols)
                finalColours *= *modcols;

            // render the imagery section
            sect->render(srcWindow, baseRect, &finalColours, clipper, clipToDisplay);
        }
        // do nothing here, errors are non-faltal and are logged for debugging purposes.
        CEGUI_CATCH (Exception&)
        {}
    }

    const String& SectionSpecification::getOwnerWidgetLookFeel() const
    {
        return d_owner;
    }

    const String& SectionSpecification::getSectionName() const
    {
        return d_sectionName;
    }


    void SectionSpecification::setOwnerWidgetLookFeel(const String& owner)
    {
        d_owner = owner;
    }

    void SectionSpecification::setSectionName(const String& name)
    {
        d_sectionName = name;
    }

    const ColourRect& SectionSpecification::getOverrideColours() const
    {
        return d_coloursOverride;
    }

    void SectionSpecification::setOverrideColours(const ColourRect& cols)
    {
        d_coloursOverride = cols;
    }

    bool SectionSpecification::isUsingOverrideColours() const
    {
        return d_usingColourOverride;
    }

    void SectionSpecification::setUsingOverrideColours(bool setting)
    {
        d_usingColourOverride = setting;
    }

    const String& SectionSpecification::getOverrideColoursPropertySource() const
    {
        return d_colourPropertyName;
    }

    void SectionSpecification::setOverrideColoursPropertySource(const String& property)
    {
        d_colourPropertyName = property;
    }

    void SectionSpecification::initColourRectForOverride(const Window& wnd, ColourRect& cr) const
    {
        // if no override set
        if (!d_usingColourOverride)
        {
            Colour val(1,1,1,1);
            cr.d_top_left     = val;
            cr.d_top_right    = val;
            cr.d_bottom_left  = val;
            cr.d_bottom_right = val;
        }
        // if override comes via a colour property
        else if (!d_colourPropertyName.empty())
        {
            // if property accesses a ColourRect or a colour
            cr = wnd.getProperty<ColourRect>(d_colourPropertyName);
        }
        // override is an explicitly defined ColourRect.
        else
        {
            cr = d_coloursOverride;
        }
    }

    const String& SectionSpecification::getRenderControlPropertySource() const
    {
        return d_renderControlProperty;
    }

    void SectionSpecification::setRenderControlPropertySource(const String& property)
    {
        d_renderControlProperty = property;
    }

    void SectionSpecification::writeXMLToStream(XMLSerializer& xml_stream) const
    {
        xml_stream.openTag(Falagard_xmlHandler::SectionElement);

        if (!d_owner.empty())
            xml_stream.attribute(Falagard_xmlHandler::LookAttribute, d_owner);

        xml_stream.attribute(Falagard_xmlHandler::SectionNameAttribute, d_sectionName);

        // render controlling property name if needed
        if (!d_renderControlProperty.empty())
            xml_stream.attribute(Falagard_xmlHandler::ControlPropertyAttribute, d_renderControlProperty);
        if (!d_renderControlValue.empty())
            xml_stream.attribute(Falagard_xmlHandler::ControlValueAttribute, d_renderControlValue);
        if (!d_renderControlWidget.empty())
            xml_stream.attribute(Falagard_xmlHandler::ControlWidgetAttribute, d_renderControlWidget);

        if (d_usingColourOverride)
        {
            // output modulative colours for this section
            if (!d_colourPropertyName.empty())
            {
                xml_stream.openTag(Falagard_xmlHandler::ColourRectPropertyElement);
                xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_colourPropertyName)
                    .closeTag();
            }
            else if (!d_coloursOverride.isMonochromatic() || d_coloursOverride.d_top_left != Colour(1,1,1,1))
            {
                xml_stream.openTag(Falagard_xmlHandler::ColoursElement)
                    .attribute(Falagard_xmlHandler::TopLeftAttribute, PropertyHelper<Colour>::toString(d_coloursOverride.d_top_left))
                    .attribute(Falagard_xmlHandler::TopRightAttribute, PropertyHelper<Colour>::toString(d_coloursOverride.d_top_right))
                    .attribute(Falagard_xmlHandler::BottomLeftAttribute, PropertyHelper<Colour>::toString(d_coloursOverride.d_bottom_left))
                    .attribute(Falagard_xmlHandler::BottomRightAttribute, PropertyHelper<Colour>::toString(d_coloursOverride.d_bottom_right))
                    .closeTag();
            }

        }
        // close section element 
        xml_stream.closeTag();
    }

//----------------------------------------------------------------------------//
bool SectionSpecification::shouldBeDrawn(const Window& wnd) const
{
    // test the simple case first.
    if (d_renderControlProperty.empty())
        return true;

    const Window* property_source;

    // work out which window the property should be accessed for.
    if (d_renderControlWidget.empty())
        property_source = &wnd;
    else if (d_renderControlWidget == Falagard_xmlHandler::ParentIdentifier)
        property_source = wnd.getParent();
    else
        property_source = wnd.getChild(d_renderControlWidget);

    // if no source window, we can't access the property, so never draw
    if (!property_source)
        return false;

    // return whether to draw based on property value.
    if (d_renderControlValue.empty())
        return property_source->
            getProperty<bool>(d_renderControlProperty);
    else
        return
            property_source->getProperty(d_renderControlProperty) == d_renderControlValue;
}

//----------------------------------------------------------------------------//
const String& SectionSpecification::getRenderControlValue() const
{
    return d_renderControlValue;
}

//----------------------------------------------------------------------------//
void SectionSpecification::setRenderControlValue(const String& value)
{
    d_renderControlValue = value;
}

//----------------------------------------------------------------------------//
const String& SectionSpecification::getRenderControlWidget() const
{
    return d_renderControlWidget;
}

//----------------------------------------------------------------------------//
void SectionSpecification::setRenderControlWidget(const String& widget)
{
    d_renderControlWidget = widget;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
