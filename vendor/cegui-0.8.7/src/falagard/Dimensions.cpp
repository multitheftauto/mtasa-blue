/***********************************************************************
    created:    Mon Jun 13 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/falagard/Dimensions.h"
#include "CEGUI/falagard/XMLEnumHelper.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/Image.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Window.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/Font.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/CoordConverter.h"
#include <cassert>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
BaseDim::BaseDim()
{
}

//----------------------------------------------------------------------------//
BaseDim::~BaseDim()
{
}

//----------------------------------------------------------------------------//
void BaseDim::writeXMLToStream(XMLSerializer& xml_stream) const
{
    // get sub-class to output the data for this single dimension
    writeXMLElementName_impl(xml_stream);
    writeXMLElementAttributes_impl(xml_stream);
    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//
bool BaseDim::handleFontRenderSizeChange(Window& /*window*/,
                                         const Font* /*font*/) const
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
OperatorDim::OperatorDim() :
    d_left(0),
    d_right(0),
    d_op(DOP_NOOP)
{
}

//----------------------------------------------------------------------------//
OperatorDim::OperatorDim(DimensionOperator op) :
    d_left(0),
    d_right(0),
    d_op(op)
{
}

//----------------------------------------------------------------------------//
OperatorDim::OperatorDim(DimensionOperator op, BaseDim* left, BaseDim* right) :
    d_left(left ? left->clone() : 0),
    d_right(right ? right->clone() : 0),
    d_op(op)
{
}

//----------------------------------------------------------------------------//
OperatorDim::~OperatorDim()
{
    CEGUI_DELETE_AO d_right;
    CEGUI_DELETE_AO d_left;
}

//----------------------------------------------------------------------------//
void OperatorDim::setLeftOperand(const BaseDim* operand)
{
    CEGUI_DELETE_AO d_left;
    
    d_left = operand ? operand->clone() : 0;
}

//----------------------------------------------------------------------------//
BaseDim* OperatorDim::getLeftOperand() const
{
    return d_left;
}

//----------------------------------------------------------------------------//
void OperatorDim::setRightOperand(const BaseDim* operand)
{
    CEGUI_DELETE_AO d_right;
    
    d_right = operand ? operand->clone() : 0;
}

//----------------------------------------------------------------------------//
BaseDim* OperatorDim::getRightOperand() const
{
    return d_right;
}

//----------------------------------------------------------------------------//
void OperatorDim::setOperator(DimensionOperator op)
{
    d_op = op;
}

//----------------------------------------------------------------------------//
DimensionOperator OperatorDim::getOperator() const
{
    return d_op;
}

//----------------------------------------------------------------------------//
void OperatorDim::setNextOperand(const BaseDim* operand)
{
    if (!d_left)
        d_left = operand ? operand->clone() : 0;
    else if (!d_right)
        d_right = operand ? operand->clone() : 0;
    else
        CEGUI_THROW(InvalidRequestException(
            "Both operands are already set."));
}

//----------------------------------------------------------------------------//
float OperatorDim::getValue(const Window& wnd) const
{
    const float lval = d_left ? d_left->getValue(wnd) : 0.0f;
    const float rval = d_right ? d_right->getValue(wnd) : 0.0f;

    return getValueImpl(lval, rval);
}

//----------------------------------------------------------------------------//
float OperatorDim::getValue(const Window& wnd, const Rectf& container) const
{
    const float lval = d_left ? d_left->getValue(wnd, container) : 0.0f;
    const float rval = d_right ? d_right->getValue(wnd, container) : 0.0f;

    return getValueImpl(lval, rval);
}

//----------------------------------------------------------------------------//
float OperatorDim::getValueImpl(const float lval, const float rval) const
{
    switch(d_op)
    {
    case DOP_NOOP:
        return 0.0f;

    case DOP_ADD:
        return lval + rval;

    case DOP_SUBTRACT:
        return lval - rval;

    case DOP_MULTIPLY:
        return lval * rval;

    // divide by zero returns zero.  Not 100% correct but is better than the
    // alternatives in the majority of cases where LookNFeels are concerned.
    case DOP_DIVIDE:
        return rval == 0.0f ? rval : lval / rval;

    default:
        CEGUI_THROW(InvalidRequestException(
            "Unknown DimensionOperator value."));
    }
}

//----------------------------------------------------------------------------//
BaseDim* OperatorDim::clone() const
{
    return CEGUI_NEW_AO OperatorDim(d_op, d_left, d_right);
}

//----------------------------------------------------------------------------//
void OperatorDim::writeXMLToStream(XMLSerializer& xml_stream) const
{
    writeXMLElementName_impl(xml_stream);
    writeXMLElementAttributes_impl(xml_stream);

    if (d_left)
        d_left->writeXMLToStream(xml_stream);

    if (d_right)
        d_right->writeXMLToStream(xml_stream);

    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//
void OperatorDim::writeXMLElementName_impl(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::OperatorDimElement);
}

//----------------------------------------------------------------------------//
void OperatorDim::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
    xml_stream.attribute(
        Falagard_xmlHandler::OperatorAttribute, FalagardXMLHelper<DimensionOperator>::toString(d_op));
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
AbsoluteDim::AbsoluteDim(float val) :
    d_val(val)
{}

//----------------------------------------------------------------------------//
float AbsoluteDim::getBaseValue() const
{
    return d_val;
}

//----------------------------------------------------------------------------//
void AbsoluteDim::setBaseValue(float val)
{
    d_val = val;
}

//----------------------------------------------------------------------------//
float AbsoluteDim::getValue(const Window&) const
{
    return d_val;
}

//----------------------------------------------------------------------------//
float AbsoluteDim::getValue(const Window&, const Rectf&) const
{
    return d_val;
}

//----------------------------------------------------------------------------//
BaseDim* AbsoluteDim::clone() const
{
    return CEGUI_NEW_AO AbsoluteDim(*this);
}

//----------------------------------------------------------------------------//
void AbsoluteDim::writeXMLElementName_impl(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::AbsoluteDimElement);
}

//----------------------------------------------------------------------------//
void AbsoluteDim::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
    xml_stream.attribute(Falagard_xmlHandler::ValueAttribute, PropertyHelper<float>::toString(d_val));
}


////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
ImageDimBase::ImageDimBase(DimensionType dim) :
    d_what(dim)
{}

//----------------------------------------------------------------------------//
DimensionType ImageDimBase::getSourceDimension() const
{
    return d_what;
}

//----------------------------------------------------------------------------//
void ImageDimBase::setSourceDimension(DimensionType dim)
{
    d_what = dim;
}

//----------------------------------------------------------------------------//
float ImageDimBase::getValue(const Window& wnd) const
{
    const Image* const img = getSourceImage(wnd);

    if (!img)
        return 0.0f;

    switch (d_what)
    {
        case DT_WIDTH:
            return img->getRenderedSize().d_width;
            break;

        case DT_HEIGHT:
            return img->getRenderedSize().d_height;
            break;

        case DT_X_OFFSET:
            return img->getRenderedOffset().d_x;
            break;

        case DT_Y_OFFSET:
            return img->getRenderedOffset().d_y;
            break;

        default:
            CEGUI_THROW(InvalidRequestException(
                "unknown or unsupported DimensionType encountered."));
            break;
    }
}

//----------------------------------------------------------------------------//
float ImageDimBase::getValue(const Window& wnd, const Rectf&) const
{
    // This dimension type does not alter when whithin a container Rect.
    return getValue(wnd);
}

//----------------------------------------------------------------------------//
void ImageDimBase::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
    xml_stream.attribute(Falagard_xmlHandler::DimensionAttribute,
                         FalagardXMLHelper<DimensionType>::toString(d_what));
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
ImageDim::ImageDim(const String& image_name, DimensionType dim) :
    ImageDimBase(dim),
    d_imageName(image_name)
{
}

//----------------------------------------------------------------------------//
const String& ImageDim::getSourceImage() const
{
    return d_imageName;
}

//----------------------------------------------------------------------------//
void ImageDim::setSourceImage(const String& image_name)
{
    d_imageName = image_name;
}

//----------------------------------------------------------------------------//
const Image* ImageDim::getSourceImage(const Window& /*wnd*/) const
{
    return &ImageManager::getSingleton().get(d_imageName);
}

//----------------------------------------------------------------------------//
BaseDim* ImageDim::clone() const
{
    return CEGUI_NEW_AO ImageDim(*this);
}

//----------------------------------------------------------------------------//
void ImageDim::writeXMLElementName_impl(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::ImageDimElement);
}

//----------------------------------------------------------------------------//
void ImageDim::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
    ImageDimBase::writeXMLElementAttributes_impl(xml_stream);
    xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_imageName);
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
ImagePropertyDim::ImagePropertyDim(const String& property_name,
                                   DimensionType dim) :
    ImageDimBase(dim),
    d_propertyName(property_name)
{
}

//----------------------------------------------------------------------------//
const String& ImagePropertyDim::getSourceProperty() const
{
    return d_propertyName;
}

//----------------------------------------------------------------------------//
void ImagePropertyDim::setSourceProperty(const String& property_name)
{
    d_propertyName = property_name;
}

//----------------------------------------------------------------------------//
const Image* ImagePropertyDim::getSourceImage(const Window& wnd) const
{
    const String& image_name(wnd.getProperty(d_propertyName));
    return image_name.empty() ? 0 : &ImageManager::getSingleton().get(image_name);
}

//----------------------------------------------------------------------------//
BaseDim* ImagePropertyDim::clone() const
{
    return CEGUI_NEW_AO ImagePropertyDim(*this);
}

//----------------------------------------------------------------------------//
void ImagePropertyDim::writeXMLElementName_impl(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::ImagePropertyDimElement);
}

//----------------------------------------------------------------------------//
void ImagePropertyDim::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
    ImageDimBase::writeXMLElementAttributes_impl(xml_stream);
    xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_propertyName);
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
WidgetDim::WidgetDim(const String& name, DimensionType dim) :
    d_widgetName(name),
    d_what(dim)
{}

//----------------------------------------------------------------------------//
const String& WidgetDim::getWidgetName() const
{
    return d_widgetName;
}

//----------------------------------------------------------------------------//
void WidgetDim::setWidgetName(const String& name)
{
    d_widgetName = name;
}

//----------------------------------------------------------------------------//
DimensionType WidgetDim::getSourceDimension() const
{
    return d_what;
}

//----------------------------------------------------------------------------//
void WidgetDim::setSourceDimension(DimensionType dim)
{
    d_what = dim;
}

//----------------------------------------------------------------------------//
float WidgetDim::getValue(const Window& wnd) const
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
        widget = wnd.getChild(d_widgetName);
    }

    // get size of parent; required to extract pixel values
    Sizef parentSize(widget->getParentPixelSize());

    switch (d_what)
    {
        case DT_WIDTH:
            return widget->getPixelSize().d_width;
            break;

        case DT_HEIGHT:
            return widget->getPixelSize().d_height;
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
            return CoordConverter::asAbsolute(widget->getPosition().d_x, parentSize.d_width);
            break;

        case DT_TOP_EDGE:
        case DT_Y_POSITION:
            return CoordConverter::asAbsolute(widget->getPosition().d_y, parentSize.d_height);
            break;

        case DT_RIGHT_EDGE:
            return CoordConverter::asAbsolute(widget->getArea().d_max.d_x, parentSize.d_width);
            break;

        case DT_BOTTOM_EDGE:
            return CoordConverter::asAbsolute(widget->getArea().d_max.d_y, parentSize.d_height);
            break;

        default:
            CEGUI_THROW(InvalidRequestException(
                "unknown or unsupported DimensionType encountered."));
            break;
    }
}

//----------------------------------------------------------------------------//
float WidgetDim::getValue(const Window& wnd, const Rectf&) const
{
    // This dimension type does not alter when whithin a container Rect.
    return getValue(wnd);
}

//----------------------------------------------------------------------------//
BaseDim* WidgetDim::clone() const
{
    return CEGUI_NEW_AO WidgetDim(*this);
}

//----------------------------------------------------------------------------//
void WidgetDim::writeXMLElementName_impl(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::WidgetDimElement);
}

//----------------------------------------------------------------------------//
void WidgetDim::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
    if (!d_widgetName.empty())
        xml_stream.attribute(Falagard_xmlHandler::WidgetAttribute, d_widgetName);

    xml_stream.attribute(Falagard_xmlHandler::DimensionAttribute, FalagardXMLHelper<DimensionType>::toString(d_what));
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
FontDim::FontDim(const String& name, const String& font, const String& text,
                 FontMetricType metric, float padding) :
    d_font(font),
    d_text(text),
    d_childName(name),
    d_metric(metric),
    d_padding(padding)
{
}

//----------------------------------------------------------------------------//
const String& FontDim::getName() const
{
    return d_childName;
}

//----------------------------------------------------------------------------//
void FontDim::setName(const String& name)
{
    d_childName = name;
}

//----------------------------------------------------------------------------//
const String& FontDim::getFont() const
{
    return d_font;
}

//----------------------------------------------------------------------------//
void FontDim::setFont(const String& font)
{
    d_font = font;
}

//----------------------------------------------------------------------------//
const String& FontDim::getText() const
{
    return d_text;
}

//----------------------------------------------------------------------------//
void FontDim::setText(const String& text)
{
    d_text = text;
}

//----------------------------------------------------------------------------//
FontMetricType FontDim::getMetric() const
{
    return d_metric;
}

//----------------------------------------------------------------------------//
void FontDim::setMetric(FontMetricType metric)
{
    d_metric = metric;
}

//----------------------------------------------------------------------------//
float FontDim::getPadding() const
{
    return d_padding;
}

//----------------------------------------------------------------------------//
void FontDim::setPadding(float padding)
{
    d_padding = padding;
}

//----------------------------------------------------------------------------//
float FontDim::getValue(const Window& wnd) const
{
    // get window to use.
    const Window& sourceWindow = d_childName.empty() ? wnd : *wnd.getChild(d_childName);
    // get font to use
    const Font* fontObj = getFontObject(sourceWindow);

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
                return fontObj->getTextExtent(d_text.empty() ? sourceWindow.getText() : d_text) + d_padding;
                break;
            default:
                CEGUI_THROW(InvalidRequestException(
                    "unknown or unsupported FontMetricType encountered."));
                break;
        }
    }
    // no font, return padding value only.
    else
    {
        return d_padding;
    }
}

//----------------------------------------------------------------------------//
const Font* FontDim::getFontObject(const Window& window) const
{
    return d_font.empty() ? window.getFont() :
                            &FontManager::getSingleton().get(d_font);
}

//----------------------------------------------------------------------------//
float FontDim::getValue(const Window& wnd, const Rectf&) const
{
    return getValue(wnd);
}

//----------------------------------------------------------------------------//
BaseDim* FontDim::clone() const
{
    return CEGUI_NEW_AO FontDim(*this);
}

//----------------------------------------------------------------------------//
void FontDim::writeXMLElementName_impl(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::FontDimElement);
}

//----------------------------------------------------------------------------//
void FontDim::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
    xml_stream.attribute(Falagard_xmlHandler::TypeAttribute, FalagardXMLHelper<FontMetricType>::toString(d_metric));

    if (!d_childName.empty())
        xml_stream.attribute(Falagard_xmlHandler::WidgetAttribute, d_childName);

    if (!d_font.empty())
        xml_stream.attribute(Falagard_xmlHandler::FontAttribute, d_font);

    if (!d_text.empty())
        xml_stream.attribute(Falagard_xmlHandler::StringAttribute, d_text);

    if (d_padding != 0)
        xml_stream.attribute(Falagard_xmlHandler::PaddingAttribute, PropertyHelper<float>::toString(d_padding));
}

//----------------------------------------------------------------------------//
bool FontDim::handleFontRenderSizeChange(Window& window,
                                         const Font* font) const
{
    return font == getFontObject(window);
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
PropertyDim::PropertyDim(const String& name, const String& property,
    DimensionType type) :
    d_property(property),
    d_childName(name),
    d_type (type)
{
}

//----------------------------------------------------------------------------//
const String& PropertyDim::getWidgetName() const
{
    return d_childName;
}

//----------------------------------------------------------------------------//
void PropertyDim::setWidgetName(const String& name)
{
    d_childName = name;
}

//----------------------------------------------------------------------------//
const String& PropertyDim::getPropertyName() const
{
    return d_property;
}

//----------------------------------------------------------------------------//
void PropertyDim::setPropertyName(const String& property)
{
    d_property = property;
}

//----------------------------------------------------------------------------//
DimensionType PropertyDim::getSourceDimension() const
{
    return d_type;
}

//----------------------------------------------------------------------------//
void PropertyDim::setSourceDimension(DimensionType dim)
{
    d_type = dim;
}

//----------------------------------------------------------------------------//
float PropertyDim::getValue(const Window& wnd) const
{
    // get window to use.
    const Window& sourceWindow = d_childName.empty() ? wnd : *wnd.getChild(d_childName);

    if (d_type == DT_INVALID)
    {
        // check property data type and convert to float if necessary
        Property* pi = sourceWindow.getPropertyInstance(d_property);
        if (pi->getDataType() == PropertyHelper<bool>::getDataTypeName())
            return sourceWindow.getProperty<bool>(d_property) ? 1.0f : 0.0f;

        // return float property value.
        return sourceWindow.getProperty<float>(d_property);
    }

    const UDim d = sourceWindow.getProperty<UDim>(d_property);
    const Sizef s = sourceWindow.getPixelSize();

    switch (d_type)
    {
        case DT_WIDTH:
            return CoordConverter::asAbsolute(d, s.d_width);

        case DT_HEIGHT:
            return CoordConverter::asAbsolute(d, s.d_height);

        default:
            CEGUI_THROW(InvalidRequestException(
                "unknown or unsupported DimensionType encountered."));
    }
}

//----------------------------------------------------------------------------//
float PropertyDim::getValue(const Window& wnd, const Rectf&) const
{
    return getValue(wnd);
}

//----------------------------------------------------------------------------//
BaseDim* PropertyDim::clone() const
{
    return CEGUI_NEW_AO PropertyDim(*this);
}

//----------------------------------------------------------------------------//
void PropertyDim::writeXMLElementName_impl(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::PropertyDimElement);
}

//----------------------------------------------------------------------------//
void PropertyDim::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
    if (!d_childName.empty())
        xml_stream.attribute(Falagard_xmlHandler::WidgetAttribute, d_childName);
    xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_property);
    if (d_type != DT_INVALID)
        xml_stream.attribute(Falagard_xmlHandler::TypeAttribute, FalagardXMLHelper<DimensionType>::toString(d_type));
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
Dimension::Dimension()
{
    d_value = 0;
    d_type = DT_INVALID;
}

//----------------------------------------------------------------------------//
Dimension::~Dimension()
{
    if (d_value)
        CEGUI_DELETE_AO d_value;
}

//----------------------------------------------------------------------------//
Dimension::Dimension(const BaseDim& dim, DimensionType type)
{
    d_value = dim.clone();
    d_type = type;
}

//----------------------------------------------------------------------------//
Dimension::Dimension(const Dimension& other)
{
    d_value = other.d_value ? other.d_value->clone() : 0;
    d_type = other.d_type;
}

//----------------------------------------------------------------------------//
Dimension& Dimension::operator=(const Dimension& other)
{
    // release old value, if any.
    if (d_value)
        CEGUI_DELETE_AO d_value;

    d_value = other.d_value ? other.d_value->clone() : 0;
    d_type = other.d_type;

    return *this;
}

//----------------------------------------------------------------------------//
const BaseDim& Dimension::getBaseDimension() const
{
    assert(d_value);
    return *d_value;
}

//----------------------------------------------------------------------------//
void Dimension::setBaseDimension(const BaseDim& dim)
{
    // release old value, if any.
    if (d_value)
        CEGUI_DELETE_AO d_value;

    d_value = dim.clone();
}

//----------------------------------------------------------------------------//
DimensionType Dimension::getDimensionType() const
{
    return d_type;
}

//----------------------------------------------------------------------------//
void Dimension::setDimensionType(DimensionType type)
{
    d_type = type;
}

//----------------------------------------------------------------------------//
void Dimension::writeXMLToStream(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::DimElement)
        .attribute(Falagard_xmlHandler::TypeAttribute, FalagardXMLHelper<DimensionType>::toString(d_type));

    if (d_value)
        d_value->writeXMLToStream(xml_stream);
    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//
bool Dimension::handleFontRenderSizeChange(Window& window,
                                           const Font* font) const
{
    return d_value ? d_value->handleFontRenderSizeChange(window, font) :
                     false;
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
UnifiedDim::UnifiedDim(const UDim& value, DimensionType dim) :
    d_value(value),
    d_what(dim)
{
}

//----------------------------------------------------------------------------//
const UDim& UnifiedDim::getBaseValue() const
{
    return d_value;
}

//----------------------------------------------------------------------------//
void UnifiedDim::setBaseValue(const UDim& dim)
{
    d_value = dim;
}

//----------------------------------------------------------------------------//
DimensionType UnifiedDim::getSourceDimension() const
{
    return d_what;
}

//----------------------------------------------------------------------------//
void UnifiedDim::setSourceDimension(DimensionType dim)
{
    d_what = dim;
}

//----------------------------------------------------------------------------//
float UnifiedDim::getValue(const Window& wnd) const
{
    switch (d_what)
    {
        case DT_LEFT_EDGE:
        case DT_RIGHT_EDGE:
        case DT_X_POSITION:
        case DT_X_OFFSET:
        case DT_WIDTH:
            return CoordConverter::asAbsolute(d_value, wnd.getPixelSize().d_width);
            break;

        case DT_TOP_EDGE:
        case DT_BOTTOM_EDGE:
        case DT_Y_POSITION:
        case DT_Y_OFFSET:
        case DT_HEIGHT:
            return CoordConverter::asAbsolute(d_value, wnd.getPixelSize().d_height);
            break;

        default:
            CEGUI_THROW(InvalidRequestException(
                "unknown or unsupported DimensionType encountered."));
            break;
    }
}

//----------------------------------------------------------------------------//
float UnifiedDim::getValue(const Window&, const Rectf& container) const
{
    switch (d_what)
    {
        case DT_LEFT_EDGE:
        case DT_RIGHT_EDGE:
        case DT_X_POSITION:
        case DT_X_OFFSET:
        case DT_WIDTH:
            return CoordConverter::asAbsolute(d_value, container.getWidth());
            break;

        case DT_TOP_EDGE:
        case DT_BOTTOM_EDGE:
        case DT_Y_POSITION:
        case DT_Y_OFFSET:
        case DT_HEIGHT:
            return CoordConverter::asAbsolute(d_value, container.getHeight());
            break;

        default:
            CEGUI_THROW(InvalidRequestException(
                "unknown or unsupported DimensionType encountered."));
            break;
    }
}

//----------------------------------------------------------------------------//
BaseDim* UnifiedDim::clone() const
{
    UnifiedDim* ndim = CEGUI_NEW_AO UnifiedDim(d_value, d_what);
    return ndim;
}

//----------------------------------------------------------------------------//
void UnifiedDim::writeXMLElementName_impl(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::UnifiedDimElement);
}

//----------------------------------------------------------------------------//
void UnifiedDim::writeXMLElementAttributes_impl(XMLSerializer& xml_stream) const
{
        xml_stream.attribute(Falagard_xmlHandler::TypeAttribute, FalagardXMLHelper<DimensionType>::toString(d_what));

    if (d_value.d_scale != 0)
        xml_stream.attribute(Falagard_xmlHandler::ScaleAttribute, PropertyHelper<float>::toString(d_value.d_scale));

    if (d_value.d_offset != 0)
        xml_stream.attribute(Falagard_xmlHandler::OffsetAttribute, PropertyHelper<float>::toString(d_value.d_offset));
}

////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
ComponentArea::ComponentArea() :
    d_left(AbsoluteDim(0.0f), DT_LEFT_EDGE),
    d_top(AbsoluteDim(0.0f), DT_TOP_EDGE),
    d_right_or_width(UnifiedDim(UDim(1.0f, 0.0f), DT_WIDTH), DT_RIGHT_EDGE),
    d_bottom_or_height(UnifiedDim(UDim(1.0f, 0.0f), DT_HEIGHT), DT_BOTTOM_EDGE)
{}

//----------------------------------------------------------------------------//
Rectf ComponentArea::getPixelRect(const Window& wnd) const
{
    Rectf pixelRect;

    // use a property?
    if (isAreaFetchedFromProperty())
    {
        pixelRect = CoordConverter::asAbsolute(
            wnd.getProperty<URect>(d_namedSource), wnd.getPixelSize());
    }
    else if (isAreaFetchedFromNamedArea())
    {
        return WidgetLookManager::getSingleton()
            .getWidgetLook(d_namedAreaSourceLook)
            .getNamedArea(d_namedSource)
            .getArea()
            .getPixelRect(wnd);
    }
    // not via property or named area- calculate using Dimensions
    else
    {
        // sanity check, we mus be able to form a Rect from what we represent.
        assert(d_left.getDimensionType() == DT_LEFT_EDGE || d_left.getDimensionType() == DT_X_POSITION);
        assert(d_top.getDimensionType() == DT_TOP_EDGE || d_top.getDimensionType() == DT_Y_POSITION);
        assert(d_right_or_width.getDimensionType() == DT_RIGHT_EDGE || d_right_or_width.getDimensionType() == DT_WIDTH);
        assert(d_bottom_or_height.getDimensionType() == DT_BOTTOM_EDGE || d_bottom_or_height.getDimensionType() == DT_HEIGHT);

        pixelRect.left(d_left.getBaseDimension().getValue(wnd));
        pixelRect.top(d_top.getBaseDimension().getValue(wnd));

        if (d_right_or_width.getDimensionType() == DT_WIDTH)
            pixelRect.setWidth(d_right_or_width.getBaseDimension().getValue(wnd));
        else
            pixelRect.right(d_right_or_width.getBaseDimension().getValue(wnd));

        if (d_bottom_or_height.getDimensionType() == DT_HEIGHT)
            pixelRect.setHeight(d_bottom_or_height.getBaseDimension().getValue(wnd));
        else
            pixelRect.bottom(d_bottom_or_height.getBaseDimension().getValue(wnd));
    }

    return pixelRect;
}

//----------------------------------------------------------------------------//
Rectf ComponentArea::getPixelRect(const Window& wnd, const Rectf& container) const
{
    Rectf pixelRect;

    // use a property?
    if (isAreaFetchedFromProperty())
    {
        pixelRect = CoordConverter::asAbsolute(
            wnd.getProperty<URect>(d_namedSource), wnd.getPixelSize());
    }
    else if (isAreaFetchedFromNamedArea())
    {
        return WidgetLookManager::getSingleton()
            .getWidgetLook(d_namedAreaSourceLook)
            .getNamedArea(d_namedSource)
            .getArea()
            .getPixelRect(wnd, container);
    }
    // not via property or named area- calculate using Dimensions
    else
    {
        // sanity check, we mus be able to form a Rect from what we represent.
        assert(d_left.getDimensionType() == DT_LEFT_EDGE || d_left.getDimensionType() == DT_X_POSITION);
        assert(d_top.getDimensionType() == DT_TOP_EDGE || d_top.getDimensionType() == DT_Y_POSITION);
        assert(d_right_or_width.getDimensionType() == DT_RIGHT_EDGE || d_right_or_width.getDimensionType() == DT_WIDTH);
        assert(d_bottom_or_height.getDimensionType() == DT_BOTTOM_EDGE || d_bottom_or_height.getDimensionType() == DT_HEIGHT);

        pixelRect.left(d_left.getBaseDimension().getValue(wnd, container) + container.left());
        pixelRect.top(d_top.getBaseDimension().getValue(wnd, container) + container.top());

        if (d_right_or_width.getDimensionType() == DT_WIDTH)
            pixelRect.setWidth(d_right_or_width.getBaseDimension().getValue(wnd, container));
        else
            pixelRect.right(d_right_or_width.getBaseDimension().getValue(wnd, container) + container.left());

        if (d_bottom_or_height.getDimensionType() == DT_HEIGHT)
            pixelRect.setHeight(d_bottom_or_height.getBaseDimension().getValue(wnd, container));
        else
            pixelRect.bottom(d_bottom_or_height.getBaseDimension().getValue(wnd, container) + container.top());
    }

    return pixelRect;
}

//----------------------------------------------------------------------------//
void ComponentArea::writeXMLToStream(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::AreaElement);

    // see if we should write an AreaProperty element
    if (isAreaFetchedFromProperty())
    {
        xml_stream.openTag(Falagard_xmlHandler::AreaPropertyElement)
            .attribute(Falagard_xmlHandler::NameAttribute, d_namedSource)
            .closeTag();
    }
    else if (isAreaFetchedFromNamedArea())
    {
        xml_stream.openTag(Falagard_xmlHandler::NamedAreaSourceElement)
            .attribute(Falagard_xmlHandler::LookAttribute, d_namedAreaSourceLook)
            .attribute(Falagard_xmlHandler::NameAttribute, d_namedSource)
            .closeTag();
    }
    // not a property, write out individual dimensions explicitly.
    else
    {
        d_left.writeXMLToStream(xml_stream);
        d_top.writeXMLToStream(xml_stream);
        d_right_or_width.writeXMLToStream(xml_stream);
        d_bottom_or_height.writeXMLToStream(xml_stream);
    }
    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//
bool ComponentArea::isAreaFetchedFromProperty() const
{
    return !d_namedSource.empty() && d_namedAreaSourceLook.empty();
}

//----------------------------------------------------------------------------//
const String& ComponentArea::getAreaPropertySource() const
{
    return d_namedSource;
}

//----------------------------------------------------------------------------//
void ComponentArea::setAreaPropertySource(const String& property)
{
    d_namedSource = property;
    d_namedAreaSourceLook.clear();
}

//----------------------------------------------------------------------------//
const String& ComponentArea::getNamedAreaSourceLook() const
{
    return d_namedAreaSourceLook;
}

//----------------------------------------------------------------------------//
void ComponentArea::setNamedAreaSouce(const String& widget_look,
                                      const String& area_name)
{
    d_namedSource = area_name;
    d_namedAreaSourceLook = widget_look;
}

//----------------------------------------------------------------------------//
bool ComponentArea::isAreaFetchedFromNamedArea() const
{
    return !d_namedAreaSourceLook.empty() && !d_namedSource.empty();
}

//----------------------------------------------------------------------------//
bool ComponentArea::handleFontRenderSizeChange(Window& window,
                                               const Font* font) const
{
    if (isAreaFetchedFromProperty())
        return false;

    if (isAreaFetchedFromNamedArea())
    {
        return WidgetLookManager::getSingleton()
            .getWidgetLook(d_namedAreaSourceLook)
            .getNamedArea(d_namedSource)
            .handleFontRenderSizeChange(window, font);
    }

    bool result = d_left.handleFontRenderSizeChange(window, font);
    result |= d_top.handleFontRenderSizeChange(window, font);
    result |= d_right_or_width.handleFontRenderSizeChange(window, font);
    result |= d_bottom_or_height.handleFontRenderSizeChange(window, font);

    return result;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

