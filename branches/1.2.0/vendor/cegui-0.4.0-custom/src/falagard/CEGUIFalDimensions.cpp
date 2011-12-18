/************************************************************************
    filename:   CEGUIFalDimensions.cpp
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
#include "StdInc.h"
#include "falagard/CEGUIFalDimensions.h"
#include "falagard/CEGUIFalXMLEnumHelper.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIImage.h"
#include "CEGUIWindowManager.h"
#include "CEGUIWindow.h"
#include "CEGUIExceptions.h"
#include "CEGUIFontManager.h"
#include "CEGUIFont.h"
#include "CEGUIPropertyHelper.h"
#include <cassert>

// Start of CEGUI namespace section
namespace CEGUI
{
    BaseDim::BaseDim() :
        d_operator(DOP_NOOP),
        d_operand(0)
    {
    }

    BaseDim::~BaseDim()
    {
        delete d_operand;
    }

    float BaseDim::getValue(const Window& wnd) const
    {
        // get sub-class to return value for this dimension.
        float val = getValue_impl(wnd);

        // if we have an attached operand, perform math on value as needed
        if (d_operand)
        {
            switch (d_operator)
            {
            case DOP_ADD:
                val += d_operand->getValue(wnd);
                break;
            case DOP_SUBTRACT:
                val -= d_operand->getValue(wnd);
                break;
            case DOP_MULTIPLY:
                val *= d_operand->getValue(wnd);
                break;
            case DOP_DIVIDE:
                val /= d_operand->getValue(wnd);
                break;
            default:
                // No-op.
                break;
            }
        }

        return val;
    }

    float BaseDim::getValue(const Window& wnd, const Rect& container) const
    {
        // get sub-class to return value for this dimension.
        float val = getValue_impl(wnd, container);

        // if we have an attached operand, perform math on value as needed
        if (d_operand)
        {
            switch (d_operator)
            {
            case DOP_ADD:
                val += d_operand->getValue(wnd, container);
                break;
            case DOP_SUBTRACT:
                val -= d_operand->getValue(wnd, container);
                break;
            case DOP_MULTIPLY:
                val *= d_operand->getValue(wnd, container);
                break;
            case DOP_DIVIDE:
                val /= d_operand->getValue(wnd, container);
                break;
            default:
                // No-op.
                break;
            }
        }

        return val;
    }

    BaseDim* BaseDim::clone() const
    {
        // get sub-class to return a cloned object
        BaseDim* o = clone_impl();

        // fill in operator for cloned object
        o->d_operator = d_operator;

        // now clone any attached operand dimension
        if (d_operand)
            o->d_operand = d_operand->clone();

        return o;
    }

    DimensionOperator BaseDim::getDimensionOperator() const
    {
        return d_operator;
    }

    void BaseDim::setDimensionOperator(DimensionOperator op)
    {
        d_operator = op;
    }

    const BaseDim* BaseDim::getOperand() const
    {
        return d_operand;
    }

    void BaseDim::setOperand(const BaseDim& operand)
    {
        // release old operand, if any.
        if(d_operand) delete d_operand;

        d_operand = operand.clone();
    }

    void BaseDim::writeXMLToStream(OutStream& out_stream) const
    {
        // open tag
        out_stream << "<";
        // get sub-class to output the data for this single dimension
        writeXMLElementName_impl(out_stream);
        out_stream << " ";
        writeXMLElementAttributes_impl(out_stream);

        if (d_operand)
        {
            // terminate the opening element tag
            out_stream << ">" << std::endl;
            // write out the DimOperator
            out_stream << "<DimOperator op=\"" << FalagardXMLHelper::dimensionOperatorToString(d_operator) << "\">" << std::endl;
            // write out the other operand
            d_operand->writeXMLToStream(out_stream);
            // write closing tag for DimOperator element
            out_stream << "</DimOperator>" << std::endl;
            // write closing tag for this dimension element
            out_stream << "</";
            writeXMLElementName_impl(out_stream);
            out_stream << ">" << std::endl;
        }
        // no operand, so just close this tag.
        else
        {
            out_stream << " />" << std::endl;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////

    AbsoluteDim::AbsoluteDim(float val) :
        d_val(val)
    {}

    void AbsoluteDim::setValue(float val)
    {
        d_val = val;
    }

    float AbsoluteDim::getValue_impl(const Window& wnd) const
    {
        return d_val;
    }

    float AbsoluteDim::getValue_impl(const Window& wnd, const Rect& container) const
    {
        return d_val;
    }

    BaseDim* AbsoluteDim::clone_impl() const
    {
        AbsoluteDim* ndim = new AbsoluteDim(d_val);
        return ndim;
    }

    void AbsoluteDim::writeXMLElementName_impl(OutStream& out_stream) const
    {
        out_stream << "AbsoluteDim";
    }

    void AbsoluteDim::writeXMLElementAttributes_impl(OutStream& out_stream) const
    {
        out_stream << "value=\"" << d_val << "\"";
    }


    ////////////////////////////////////////////////////////////////////////////////

    ImageDim::ImageDim(const String& imageset, const String& image, DimensionType dim) :
        d_imageset(imageset),
        d_image(image),
        d_what(dim)
    {}

    void ImageDim::setSourceImage(const String& imageset, const String& image)
    {
        d_imageset = imageset;
        d_image = image;
    }

    void ImageDim::setSourceDimension(DimensionType dim)
    {
        d_what = dim;
    }

    float ImageDim::getValue_impl(const Window& wnd) const
    {
        const Image* img = &ImagesetManager::getSingleton().getImageset(d_imageset)->getImage(d_image);

        switch (d_what)
        {
            case DT_WIDTH:
                return img->getWidth();
                break;

            case DT_HEIGHT:
                return img->getHeight();
                break;

            case DT_X_OFFSET:
                return img->getOffsetX();
                break;

            case DT_Y_OFFSET:
                return img->getOffsetY();
                break;

            // these other options will not be particularly useful for most people since they return the edges of the
            // image on the source texture.
            case DT_LEFT_EDGE:
            case DT_X_POSITION:
                return img->getSourceTextureArea().d_left;
                break;

            case DT_TOP_EDGE:
            case DT_Y_POSITION:
                return img->getSourceTextureArea().d_top;
                break;

            case DT_RIGHT_EDGE:
                return img->getSourceTextureArea().d_right;
                break;

            case DT_BOTTOM_EDGE:
                return img->getSourceTextureArea().d_bottom;
                break;

            default:
                throw InvalidRequestException("ImageDim::getValue - unknown or unsupported DimensionType encountered.");
                break;
        }
    }

    float ImageDim::getValue_impl(const Window& wnd, const Rect& container) const
    {
        // This dimension type does not alter when whithin a container Rect.
        return getValue(wnd);
    }


    BaseDim* ImageDim::clone_impl() const
    {
        ImageDim* ndim = new ImageDim(d_imageset, d_image, d_what);
        return ndim;
    }

    void ImageDim::writeXMLElementName_impl(OutStream& out_stream) const
    {
        out_stream << "ImageDim";
    }

    void ImageDim::writeXMLElementAttributes_impl(OutStream& out_stream) const
    {
        out_stream << "imageset=\"" << d_imageset << "\" image=\"" << d_image << "\" dimension=\"" << FalagardXMLHelper::dimensionTypeToString(d_what) << "\"";
    }

    ////////////////////////////////////////////////////////////////////////////////

    WidgetDim::WidgetDim(const String& name, DimensionType dim) :
        d_widgetName(name),
        d_what(dim)
    {}

    void WidgetDim::setWidgetName(const String& name)
    {
        d_widgetName = name;
    }

    void WidgetDim::setSourceDimension(DimensionType dim)
    {
        d_what = dim;
    }

    float WidgetDim::getValue_impl(const Window& wnd) const
    {
        const Window* widget;

        // if target widget name is empty, then use the input window.
        if (d_widgetName.empty())
        {
            widget = &wnd;
        }
        // name not empty, so find window with required name
        else
        {
            widget = WindowManager::getSingleton().getWindow(wnd.getName() + d_widgetName);
        }

        switch (d_what)
        {
            case DT_WIDTH:
                return widget->getAbsoluteWidth();
                break;

            case DT_HEIGHT:
                return widget->getAbsoluteHeight();
                break;

            case DT_X_OFFSET:
                Logger::getSingleton().logEvent("WigetDim::getValue - Nonsensical DimensionType of DT_X_OFFSET specified!  returning 0.0f");
                return 0.0f;
                break;

            case DT_Y_OFFSET:
                Logger::getSingleton().logEvent("WigetDim::getValue - Nonsensical DimensionType of DT_Y_OFFSET specified!  returning 0.0f");
                return 0.0f;
                break;

            case DT_LEFT_EDGE:
            case DT_X_POSITION:
                return widget->getAbsolutePosition().d_x;
                break;

            case DT_TOP_EDGE:
            case DT_Y_POSITION:
                return widget->getAbsolutePosition().d_y;
                break;

            case DT_RIGHT_EDGE:
                return widget->getAbsoluteRect().d_right;
                break;

            case DT_BOTTOM_EDGE:
                return widget->getAbsoluteRect().d_bottom;
                break;

            default:
                throw InvalidRequestException("WidgetDim::getValue - unknown or unsupported DimensionType encountered.");
                break;
        }
    }

    float WidgetDim::getValue_impl(const Window& wnd, const Rect& container) const
    {
        // This dimension type does not alter when whithin a container Rect.
        return getValue(wnd);
    }

    BaseDim* WidgetDim::clone_impl() const
    {
        WidgetDim* ndim = new WidgetDim(d_widgetName, d_what);
        return ndim;
    }

    void WidgetDim::writeXMLElementName_impl(OutStream& out_stream) const
    {
        out_stream << "WidgetDim";
    }

    void WidgetDim::writeXMLElementAttributes_impl(OutStream& out_stream) const
    {
        if (!d_widgetName.empty())
            out_stream << "widget=\"" << d_widgetName << "\" ";

        out_stream << "dimension=\"" << FalagardXMLHelper::dimensionTypeToString(d_what) << "\"";
    }

    ////////////////////////////////////////////////////////////////////////////////

    FontDim::FontDim(const String& name, const String& font, const String& text, FontMetricType metric, float padding) :
        d_font(font),
        d_text(text),
        d_childSuffix(name),
        d_metric(metric),
        d_padding(padding)
    {
    }

    float FontDim::getValue_impl(const Window& wnd) const
    {
        // get window to use.
        const Window& sourceWindow = d_childSuffix.empty() ? wnd : *WindowManager::getSingleton().getWindow(wnd.getName() + d_childSuffix);
        // get font to use
        const Font* fontObj = d_font.empty() ? sourceWindow.getFont() : FontManager::getSingleton().getFont(d_font);

        if (fontObj)
        {
            switch (d_metric)
            {
                case FMT_LINE_SPACING:
                    return fontObj->getLineSpacing() + d_padding;
                    break;
                case FMT_BASELINE:
                    return fontObj->getBaseline() + d_padding;
                    break;
                case FMT_HORZ_EXTENT:
                    return fontObj->getTextExtent(d_text.empty() ? sourceWindow.getText(true) : d_text) + d_padding;
                    break;
                default:
                    throw InvalidRequestException("FontDim::getValue - unknown or unsupported FontMetricType encountered.");
                    break;
            }
        }
        // no font, return padding value only.
        else
        {
            return d_padding;
        }
    }

    float FontDim::getValue_impl(const Window& wnd, const Rect& container) const
    {
        return getValue(wnd);
    }

    BaseDim* FontDim::clone_impl() const
    {
        FontDim* ndim = new FontDim(d_childSuffix, d_font, d_text, d_metric, d_padding);
        return ndim;
    }

    void FontDim::writeXMLElementName_impl(OutStream& out_stream) const
    {
        out_stream << "FontDim";
    }

    void FontDim::writeXMLElementAttributes_impl(OutStream& out_stream) const
    {
        if (!d_childSuffix.empty())
            out_stream << "widget=\"" << d_childSuffix << "\" ";

        if (!d_font.empty())
            out_stream << "font=\"" << d_font << "\" ";

        if (!d_text.empty())
            out_stream << "string=\"" << d_text << "\" ";

        if (d_padding != 0)
            out_stream << "padding=\"" << d_padding << "\" ";

        out_stream << "type=\"" << FalagardXMLHelper::fontMetricTypeToString(d_metric) << "\"";
    }

    ////////////////////////////////////////////////////////////////////////////////

    PropertyDim::PropertyDim(const String& name, const String& property) :
        d_property(property),
        d_childSuffix(name)
    {
    }

    float PropertyDim::getValue_impl(const Window& wnd) const
    {
        // get window to use.
        const Window& sourceWindow = d_childSuffix.empty() ? wnd : *WindowManager::getSingleton().getWindow(wnd.getName() + d_childSuffix);
        // return property value.
        return PropertyHelper::stringToFloat(sourceWindow.getProperty(d_property));
    }

    float PropertyDim::getValue_impl(const Window& wnd, const Rect& container) const
    {
        return getValue(wnd);
    }

    BaseDim* PropertyDim::clone_impl() const
    {
        PropertyDim* ndim = new PropertyDim(d_childSuffix, d_property);
        return ndim;
    }

    void PropertyDim::writeXMLElementName_impl(OutStream& out_stream) const
    {
        if (!d_childSuffix.empty())
            out_stream << "widget=\"" << d_childSuffix << "\" ";

        out_stream << "PropertyDim";
    }

    void PropertyDim::writeXMLElementAttributes_impl(OutStream& out_stream) const
    {
        out_stream << "name=\"" << d_property << "\"";
    }

    ////////////////////////////////////////////////////////////////////////////////

    Dimension::Dimension()
    {
        d_value = 0;
        d_type = DT_INVALID;
    }

    Dimension::~Dimension()
    {
        if (d_value)
            delete d_value;
    }

    Dimension::Dimension(const BaseDim& dim, DimensionType type)
    {
        d_value = dim.clone();
        d_type = type;
    }

    Dimension::Dimension(const Dimension& other)
    {
        d_value = other.d_value ? other.d_value->clone() : 0;
        d_type = other.d_type;
    }

    Dimension& Dimension::operator=(const Dimension& other)
    {
        // release old value, if any.
        if (d_value)  delete d_value;

        d_value = other.d_value ? other.d_value->clone() : 0;
        d_type = other.d_type;

		return *this;
    }

    const BaseDim& Dimension::getBaseDimension() const
    {
        assert(d_value);
        return *d_value;
    }

    void Dimension::setBaseDimension(const BaseDim& dim)
    {
        // release old value, if any.
        if (d_value)  delete d_value;

        d_value = dim.clone();
    }

    DimensionType Dimension::getDimensionType() const
    {
        return d_type;
    }

    void Dimension::setDimensionType(DimensionType type)
    {
        d_type = type;
    }

    void Dimension::writeXMLToStream(OutStream& out_stream) const
    {
        out_stream << "<Dim type=\"" << FalagardXMLHelper::dimensionTypeToString(d_type) << "\">" << std::endl;

        if (d_value)
            d_value->writeXMLToStream(out_stream);

        out_stream << "</Dim>" << std::endl;
    }

    ////////////////////////////////////////////////////////////////////////////////

    UnifiedDim::UnifiedDim(const UDim& value, DimensionType dim) :
        d_value(value),
        d_what(dim)
    {
    }

    float UnifiedDim::getValue_impl(const Window& wnd) const
    {
        switch (d_what)
        {
            case DT_LEFT_EDGE:
            case DT_RIGHT_EDGE:
            case DT_X_POSITION:
            case DT_X_OFFSET:
            case DT_WIDTH:
                return d_value.asAbsolute(wnd.getAbsoluteWidth());
                break;

            case DT_TOP_EDGE:
            case DT_BOTTOM_EDGE:
            case DT_Y_POSITION:
            case DT_Y_OFFSET:
            case DT_HEIGHT:
                return d_value.asAbsolute(wnd.getAbsoluteHeight());
                break;

            default:
                throw InvalidRequestException("UnifiedDim::getValue - unknown or unsupported DimensionType encountered.");
                break;
        }
    }

    float UnifiedDim::getValue_impl(const Window& wnd, const Rect& container) const
    {
        switch (d_what)
        {
            case DT_LEFT_EDGE:
            case DT_RIGHT_EDGE:
            case DT_X_POSITION:
            case DT_X_OFFSET:
            case DT_WIDTH:
                return d_value.asAbsolute(container.getWidth());
                break;

            case DT_TOP_EDGE:
            case DT_BOTTOM_EDGE:
            case DT_Y_POSITION:
            case DT_Y_OFFSET:
            case DT_HEIGHT:
                return d_value.asAbsolute(container.getHeight());
                break;

            default:
                throw InvalidRequestException("UnifiedDim::getValue - unknown or unsupported DimensionType encountered.");
                break;
        }
    }

    BaseDim* UnifiedDim::clone_impl() const
    {
        UnifiedDim* ndim = new UnifiedDim(d_value, d_what);
        return ndim;
    }

    void UnifiedDim::writeXMLElementName_impl(OutStream& out_stream) const
    {
        out_stream << "UnifiedDim";
    }

    void UnifiedDim::writeXMLElementAttributes_impl(OutStream& out_stream) const
    {
        if (d_value.d_scale != 0)
            out_stream << "scale=\"" << d_value.d_scale << "\" ";

        if (d_value.d_offset != 0)
            out_stream << "offset=\"" << d_value.d_offset << "\" ";

        out_stream << "type=\"" << FalagardXMLHelper::dimensionTypeToString(d_what) << "\"";
    }

    ////////////////////////////////////////////////////////////////////////////////

    Rect ComponentArea::getPixelRect(const Window& wnd) const
    {
        Rect pixelRect;

        // use a property?
        if (isAreaFetchedFromProperty())
        {
            pixelRect = PropertyHelper::stringToURect(wnd.getProperty(d_areaProperty)).asAbsolute(wnd.getAbsoluteSize());
        }
        // not via property - calculate using Dimensions
        else
        {
            // sanity check, we mus be able to form a Rect from what we represent.
            assert(d_left.getDimensionType() == DT_LEFT_EDGE || d_left.getDimensionType() == DT_X_POSITION);
            assert(d_top.getDimensionType() == DT_TOP_EDGE || d_top.getDimensionType() == DT_Y_POSITION);
            assert(d_right_or_width.getDimensionType() == DT_RIGHT_EDGE || d_right_or_width.getDimensionType() == DT_WIDTH);
            assert(d_bottom_or_height.getDimensionType() == DT_BOTTOM_EDGE || d_bottom_or_height.getDimensionType() == DT_HEIGHT);

            pixelRect.d_left = d_left.getBaseDimension().getValue(wnd);
            pixelRect.d_top = d_top.getBaseDimension().getValue(wnd);

            if (d_right_or_width.getDimensionType() == DT_WIDTH)
                pixelRect.setWidth(d_right_or_width.getBaseDimension().getValue(wnd));
            else
                pixelRect.d_right = d_right_or_width.getBaseDimension().getValue(wnd);

            if (d_bottom_or_height.getDimensionType() == DT_HEIGHT)
                pixelRect.setHeight(d_bottom_or_height.getBaseDimension().getValue(wnd));
            else
                pixelRect.d_bottom = d_bottom_or_height.getBaseDimension().getValue(wnd);
        }

        return pixelRect;
    }

    Rect ComponentArea::getPixelRect(const Window& wnd, const Rect& container) const
    {
        Rect pixelRect;

        // use a property?
        if (isAreaFetchedFromProperty())
        {
            pixelRect = PropertyHelper::stringToURect(wnd.getProperty(d_areaProperty)).asAbsolute(wnd.getAbsoluteSize());
        }
        // not via property - calculate using Dimensions
        else
        {
            // sanity check, we mus be able to form a Rect from what we represent.
            assert(d_left.getDimensionType() == DT_LEFT_EDGE || d_left.getDimensionType() == DT_X_POSITION);
            assert(d_top.getDimensionType() == DT_TOP_EDGE || d_top.getDimensionType() == DT_Y_POSITION);
            assert(d_right_or_width.getDimensionType() == DT_RIGHT_EDGE || d_right_or_width.getDimensionType() == DT_WIDTH);
            assert(d_bottom_or_height.getDimensionType() == DT_BOTTOM_EDGE || d_bottom_or_height.getDimensionType() == DT_HEIGHT);

            pixelRect.d_left = d_left.getBaseDimension().getValue(wnd, container) + container.d_left;
            pixelRect.d_top = d_top.getBaseDimension().getValue(wnd, container) + container.d_top;

            if (d_right_or_width.getDimensionType() == DT_WIDTH)
                pixelRect.setWidth(d_right_or_width.getBaseDimension().getValue(wnd, container));
            else
                pixelRect.d_right = d_right_or_width.getBaseDimension().getValue(wnd, container) + container.d_left;

            if (d_bottom_or_height.getDimensionType() == DT_HEIGHT)
                pixelRect.setHeight(d_bottom_or_height.getBaseDimension().getValue(wnd, container));
            else
                pixelRect.d_bottom = d_bottom_or_height.getBaseDimension().getValue(wnd, container) + container.d_top;
        }

        return pixelRect;
    }

    void ComponentArea::writeXMLToStream(OutStream& out_stream) const
    {
        out_stream << "<Area>" << std::endl;

        // see if we should write an AreaProperty element
        if (isAreaFetchedFromProperty())
        {
            out_stream << "<AreaProperty name=\"" << d_areaProperty << "\" />" << std::endl;
        }
        // not a property, write out individual dimensions explicitly.
        else
        {
            d_left.writeXMLToStream(out_stream);
            d_top.writeXMLToStream(out_stream);
            d_right_or_width.writeXMLToStream(out_stream);
            d_bottom_or_height.writeXMLToStream(out_stream);
        }
        out_stream << "</Area>" << std::endl;
    }

    bool ComponentArea::isAreaFetchedFromProperty() const
    {
        return !d_areaProperty.empty();
    }

    const String& ComponentArea::getAreaPropertySource() const
    {
        return d_areaProperty;
    }

    void ComponentArea::setAreaPropertySource(const String& property)
    {
        d_areaProperty = property;
    }

} // End of  CEGUI namespace section
