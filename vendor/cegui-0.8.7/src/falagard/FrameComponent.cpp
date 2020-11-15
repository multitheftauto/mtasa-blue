/***********************************************************************
    created:    Mon Jul 18 2005
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
#include "CEGUI/falagard/FrameComponent.h"
#include "CEGUI/falagard/XMLEnumHelper.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/Image.h"
#include "CEGUI/CoordConverter.h"
#include <iostream>
#include <cstdlib>

namespace CEGUI
{

//! Default values
const HorizontalFormatting FrameComponent::HorizontalFormattingDefault(HF_STRETCHED);
const VerticalFormatting FrameComponent::VerticalFormattingDefault(VF_STRETCHED);

//----------------------------------------------------------------------------//
FrameComponent::FrameComponent() :
    d_leftEdgeFormatting(VerticalFormattingDefault),
    d_rightEdgeFormatting(VerticalFormattingDefault),
    d_topEdgeFormatting(HorizontalFormattingDefault),
    d_bottomEdgeFormatting(HorizontalFormattingDefault),
    d_backgroundVertFormatting(VerticalFormattingDefault),
    d_backgroundHorzFormatting(HorizontalFormattingDefault)
{
}

//----------------------------------------------------------------------------//
void FrameComponent::setLeftEdgeFormatting(VerticalFormatting fmt)
{
    d_leftEdgeFormatting.set(fmt);
}

//----------------------------------------------------------------------------//
void FrameComponent::setRightEdgeFormatting(VerticalFormatting fmt)
{
    d_rightEdgeFormatting.set(fmt);
}

//----------------------------------------------------------------------------//
void FrameComponent::setTopEdgeFormatting(HorizontalFormatting fmt)
{
    d_topEdgeFormatting.set(fmt);
}

//----------------------------------------------------------------------------//
void FrameComponent::setBottomEdgeFormatting(HorizontalFormatting fmt)
{
    d_bottomEdgeFormatting.set(fmt);
}

//----------------------------------------------------------------------------//
void FrameComponent::setBackgroundVerticalFormatting(VerticalFormatting fmt)
{
    d_backgroundVertFormatting.set(fmt);
}

//----------------------------------------------------------------------------//
void FrameComponent::setBackgroundHorizontalFormatting(HorizontalFormatting fmt)
{
    d_backgroundHorzFormatting.set(fmt);
}

//----------------------------------------------------------------------------//
void FrameComponent::setLeftEdgeFormattingPropertySource(
                                                    const String& property_name)
{
    d_leftEdgeFormatting.setPropertySource(property_name);
}

//----------------------------------------------------------------------------//
void FrameComponent::setRightEdgeFormattingPropertySource(
                                                    const String& property_name)
{
    d_rightEdgeFormatting.setPropertySource(property_name);
}

//----------------------------------------------------------------------------//
void FrameComponent::setTopEdgeFormattingPropertySource(
                                                    const String& property_name)
{
    d_topEdgeFormatting.setPropertySource(property_name);
}

//----------------------------------------------------------------------------//
void FrameComponent::setBottomEdgeFormattingPropertySource(
                                                    const String& property_name)
{
    d_bottomEdgeFormatting.setPropertySource(property_name);
}

//----------------------------------------------------------------------------//
void FrameComponent::setBackgroundVerticalFormattingPropertySource(
                                                    const String& property_name)
{
    d_backgroundVertFormatting.setPropertySource(property_name);
}

//----------------------------------------------------------------------------//
void FrameComponent::setBackgroundHorizontalFormattingPropertySource(
                                                    const String& property_name)
{
    d_backgroundHorzFormatting.setPropertySource(property_name);
}

//----------------------------------------------------------------------------//
VerticalFormatting FrameComponent::getLeftEdgeFormatting(const Window& wnd) const
{
    return d_leftEdgeFormatting.get(wnd);
}

//----------------------------------------------------------------------------//
VerticalFormatting FrameComponent::getRightEdgeFormatting(const Window& wnd) const
{
    return d_rightEdgeFormatting.get(wnd);
}

//----------------------------------------------------------------------------//
HorizontalFormatting FrameComponent::getTopEdgeFormatting(const Window& wnd) const
{
    return d_topEdgeFormatting.get(wnd);
}

//----------------------------------------------------------------------------//
HorizontalFormatting FrameComponent::getBottomEdgeFormatting(const Window& wnd) const
{
    return d_bottomEdgeFormatting.get(wnd);
}

//----------------------------------------------------------------------------//
VerticalFormatting FrameComponent::getBackgroundVerticalFormatting(
                                                    const Window& wnd) const
{
    return d_backgroundVertFormatting.get(wnd);
}

//----------------------------------------------------------------------------//
HorizontalFormatting FrameComponent::getBackgroundHorizontalFormatting(
                                                    const Window& wnd) const
{
    return d_backgroundHorzFormatting.get(wnd);
}

//----------------------------------------------------------------------------//
const Image* FrameComponent::getImage(FrameImageComponent imageComponent,
                                      const Window& wnd) const
{
    assert(imageComponent < FIC_FRAME_IMAGE_COUNT);

    if (!d_frameImages[imageComponent].d_specified)
        return 0;

    if (d_frameImages[imageComponent].d_propertyName.empty())
        return d_frameImages[imageComponent].d_image;

    return wnd.getProperty<Image*>(d_frameImages[imageComponent].d_propertyName);
}

//----------------------------------------------------------------------------//
const Image* FrameComponent::getImage(FrameImageComponent imageComponent) const
{
    assert(imageComponent < FIC_FRAME_IMAGE_COUNT);

    if (!d_frameImages[imageComponent].d_specified)
        return 0;

    if (d_frameImages[imageComponent].d_propertyName.empty())
        return d_frameImages[imageComponent].d_image;

    return 0;
}

//----------------------------------------------------------------------------//
void FrameComponent::setImage(FrameImageComponent part, const Image* image)
{
    assert(part < FIC_FRAME_IMAGE_COUNT);

    d_frameImages[part].d_image = image;
    d_frameImages[part].d_specified = image != 0;
    d_frameImages[part].d_propertyName.clear();
}

//----------------------------------------------------------------------------//
void FrameComponent::setImage(FrameImageComponent part, const String& name)
{
    const Image* image;
    CEGUI_TRY
    {
        image = &ImageManager::getSingleton().get(name);
    }
    CEGUI_CATCH (UnknownObjectException&)
    {
        image = 0;
    }

    setImage(part, image);
}

//----------------------------------------------------------------------------//
void FrameComponent::setImagePropertySource(FrameImageComponent part,
                                            const String& name)
{
    assert(part < FIC_FRAME_IMAGE_COUNT);

    d_frameImages[part].d_image = 0;
    d_frameImages[part].d_specified = !name.empty();
    d_frameImages[part].d_propertyName = name;
}

//----------------------------------------------------------------------------//
bool FrameComponent::isImageSpecified(FrameImageComponent part) const
{
    assert(part < FIC_FRAME_IMAGE_COUNT);

    return d_frameImages[part].d_specified;
}

//----------------------------------------------------------------------------//
bool FrameComponent::isImageFetchedFromProperty(FrameImageComponent part) const
{
    assert(part < FIC_FRAME_IMAGE_COUNT);

    return d_frameImages[part].d_specified &&
           !d_frameImages[part].d_propertyName.empty();
}

//----------------------------------------------------------------------------//
const String& FrameComponent::getImagePropertySource(
                                    FrameImageComponent part) const
{
    assert(part < FIC_FRAME_IMAGE_COUNT);

    return d_frameImages[part].d_propertyName;
}

//----------------------------------------------------------------------------//
void FrameComponent::render_impl(Window& srcWindow, Rectf& destRect,
                                 const CEGUI::ColourRect* modColours,
                                 const Rectf* clipper, bool clipToDisplay) const
{
    Rectf backgroundRect(destRect);
    Rectf finalRect;
    Sizef imageSize;
    Vector2f imageOffsets;
    ColourRect imageColours;
    float leftfactor, rightfactor, topfactor, bottomfactor;
    bool calcColoursPerImage;

    // vars we use to track what to do with the side pieces.
    float topOffset = 0, bottomOffset = 0, leftOffset = 0, rightOffset = 0;
    float topWidth, bottomWidth, leftHeight, rightHeight;
    topWidth = bottomWidth = destRect.getWidth();
    leftHeight = rightHeight = destRect.getHeight();

    // calculate final overall colours to be used
    ColourRect finalColours;
    initColoursRect(srcWindow, modColours, finalColours);

    if (finalColours.isMonochromatic())
    {
        calcColoursPerImage = false;
        imageColours = finalColours;
    }
    else
    {
        calcColoursPerImage = true;
    }
    
    // top-left image
    if (const Image* const componentImage = getImage(FIC_TOP_LEFT_CORNER, srcWindow))
    {
        // calculate final destination area
        imageSize = componentImage->getRenderedSize();
        imageOffsets = componentImage->getRenderedOffset();
        finalRect.d_min = destRect.d_min;
        finalRect.setSize(imageSize);
        finalRect = destRect.getIntersection(finalRect);

        // update adjustments required to edges do to presence of this element.
        topOffset  += imageSize.d_width + imageOffsets.d_x;
        leftOffset += imageSize.d_height + imageOffsets.d_y;
        topWidth   -= topOffset;
        leftHeight -= leftOffset;

        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (finalRect.left() + imageOffsets.d_x) / destRect.getWidth();
            rightfactor  = leftfactor + finalRect.getWidth() / destRect.getWidth();
            topfactor    = (finalRect.top() + imageOffsets.d_y) / destRect.getHeight();
            bottomfactor = topfactor + finalRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
        }

        // draw this element.
        componentImage->render(srcWindow.getGeometryBuffer(), finalRect, clipper, imageColours);
    }

    // top-right image
    if (const Image* const componentImage = getImage(FIC_TOP_RIGHT_CORNER, srcWindow))
    {
        // calculate final destination area
        imageSize = componentImage->getRenderedSize();
        imageOffsets = componentImage->getRenderedOffset();
        finalRect.left(destRect.right() - imageSize.d_width);
        finalRect.top(destRect.top());
        finalRect.setSize(imageSize);
        finalRect = destRect.getIntersection(finalRect);

        // update adjustments required to edges do to presence of this element.
        rightOffset += imageSize.d_height + imageOffsets.d_y;
        topWidth    -= imageSize.d_width - imageOffsets.d_x;
        rightHeight -= rightOffset;

        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (finalRect.left() + imageOffsets.d_x) / destRect.getWidth();
            rightfactor  = leftfactor + finalRect.getWidth() / destRect.getWidth();
            topfactor    = (finalRect.top() + imageOffsets.d_y) / destRect.getHeight();
            bottomfactor = topfactor + finalRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle(leftfactor, rightfactor, topfactor, bottomfactor);
        }

        // draw this element.
        componentImage->render(srcWindow.getGeometryBuffer(), finalRect, clipper, imageColours);
    }

    // bottom-left image
    if (const Image* const componentImage = getImage(FIC_BOTTOM_LEFT_CORNER, srcWindow))
    {
        // calculate final destination area
        imageSize = componentImage->getRenderedSize();
        imageOffsets = componentImage->getRenderedOffset();
        finalRect.left(destRect.left());
        finalRect.top(destRect.bottom() - imageSize.d_height);
        finalRect.setSize(imageSize);
        finalRect = destRect.getIntersection(finalRect);

        // update adjustments required to edges do to presence of this element.
        bottomOffset += imageSize.d_width + imageOffsets.d_x;
        bottomWidth  -= bottomOffset;
        leftHeight   -= imageSize.d_height - imageOffsets.d_y;

        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (finalRect.left() + imageOffsets.d_x) / destRect.getWidth();
            rightfactor  = leftfactor + finalRect.getWidth() / destRect.getWidth();
            topfactor    = (finalRect.top() + imageOffsets.d_y) / destRect.getHeight();
            bottomfactor = topfactor + finalRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle(leftfactor, rightfactor, topfactor, bottomfactor);
        }

        // draw this element.
        componentImage->render(srcWindow.getGeometryBuffer(), finalRect, clipper, imageColours);
    }

    // bottom-right image
    if (const Image* const componentImage = getImage(FIC_BOTTOM_RIGHT_CORNER, srcWindow))
    {
        // calculate final destination area
        imageSize = componentImage->getRenderedSize();
        imageOffsets = componentImage->getRenderedOffset();
        finalRect.left(destRect.right() - imageSize.d_width);
        finalRect.top(destRect.bottom() - imageSize.d_height);
        finalRect.setSize(imageSize);
        finalRect = destRect.getIntersection(finalRect);

        // update adjustments required to edges do to presence of this element.
        bottomWidth -= imageSize.d_width - imageOffsets.d_x;
        rightHeight -= imageSize.d_height - imageOffsets.d_y;

        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (finalRect.left() + componentImage->getRenderedOffset().d_x) / destRect.getWidth();
            rightfactor  = leftfactor + finalRect.getWidth() / destRect.getWidth();
            topfactor    = (finalRect.top() + componentImage->getRenderedOffset().d_y) / destRect.getHeight();
            bottomfactor = topfactor + finalRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
        }

        // draw this element.
        componentImage->render(srcWindow.getGeometryBuffer(), finalRect, clipper, imageColours);
    }

    // top image
    if (const Image* const componentImage = getImage(FIC_TOP_EDGE, srcWindow))
    {
        // calculate final destination area
        imageSize = componentImage->getRenderedSize();
        finalRect.left(destRect.left() + topOffset);
        finalRect.right(finalRect.left() + topWidth);
        finalRect.top(destRect.top());
        finalRect.bottom(finalRect.top() + imageSize.d_height);
        finalRect = destRect.getIntersection(finalRect);

        // adjust background area to miss this edge
        backgroundRect.d_min.d_y += imageSize.d_height + componentImage->getRenderedOffset().d_y;

        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (finalRect.left() + componentImage->getRenderedOffset().d_x) / destRect.getWidth();
            rightfactor  = leftfactor + finalRect.getWidth() / destRect.getWidth();
            topfactor    = (finalRect.top() + componentImage->getRenderedOffset().d_y) / destRect.getHeight();
            bottomfactor = topfactor + finalRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
        }

        // draw this element.
        renderImage(srcWindow.getGeometryBuffer(), componentImage,
                    VF_TOP_ALIGNED, d_topEdgeFormatting.get(srcWindow),
                    finalRect, imageColours, clipper, clipToDisplay);
    }

    // bottom image
    if (const Image* const componentImage = getImage(FIC_BOTTOM_EDGE, srcWindow))
    {
        // calculate final destination area
        imageSize = componentImage->getRenderedSize();
        finalRect.left(destRect.left() + bottomOffset);
        finalRect.right(finalRect.left() + bottomWidth);
        finalRect.bottom(destRect.bottom());
        finalRect.top(finalRect.bottom() - imageSize.d_height);
        finalRect = destRect.getIntersection (finalRect);

        // adjust background area to miss this edge
        backgroundRect.d_max.d_y -= imageSize.d_height - componentImage->getRenderedOffset().d_y;

        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (finalRect.left() + componentImage->getRenderedOffset().d_x) / destRect.getWidth();
            rightfactor  = leftfactor + finalRect.getWidth() / destRect.getWidth();
            topfactor    = (finalRect.top() + componentImage->getRenderedOffset().d_y) / destRect.getHeight();
            bottomfactor = topfactor + finalRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle(leftfactor, rightfactor, topfactor, bottomfactor);
        }

        // draw this element.
        renderImage(srcWindow.getGeometryBuffer(), componentImage,
                    VF_BOTTOM_ALIGNED, d_bottomEdgeFormatting.get(srcWindow),
                    finalRect, imageColours, clipper, clipToDisplay);
    }

    // left image
    if (const Image* const componentImage = getImage(FIC_LEFT_EDGE, srcWindow))
    {
        // calculate final destination area
        imageSize = componentImage->getRenderedSize();
        finalRect.left(destRect.left());
        finalRect.right(finalRect.left() + imageSize.d_width);
        finalRect.top(destRect.top() + leftOffset);
        finalRect.bottom(finalRect.top() + leftHeight);
        finalRect = destRect.getIntersection(finalRect);

        // adjust background area to miss this edge
        backgroundRect.d_min.d_x += imageSize.d_width + componentImage->getRenderedOffset().d_x;

        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (finalRect.left() + componentImage->getRenderedOffset().d_x) / destRect.getWidth();
            rightfactor  = leftfactor + finalRect.getWidth() / destRect.getWidth();
            topfactor    = (finalRect.top() + componentImage->getRenderedOffset().d_y) / destRect.getHeight();
            bottomfactor = topfactor + finalRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
        }

        // draw this element.
        renderImage(srcWindow.getGeometryBuffer(), componentImage,
                    d_leftEdgeFormatting.get(srcWindow), HF_LEFT_ALIGNED,
                    finalRect, imageColours, clipper, clipToDisplay);
    }

    // right image
    if (const Image* const componentImage = getImage(FIC_RIGHT_EDGE, srcWindow))
    {
        // calculate final destination area
        imageSize = componentImage->getRenderedSize();
        finalRect.top(destRect.top() + rightOffset);
        finalRect.bottom(finalRect.top() + rightHeight);
        finalRect.right(destRect.right());
        finalRect.left(finalRect.right() - imageSize.d_width);
        finalRect = destRect.getIntersection (finalRect);

        // adjust background area to miss this edge
        backgroundRect.d_max.d_x -= imageSize.d_width - componentImage->getRenderedOffset().d_x;

        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (finalRect.left() + componentImage->getRenderedOffset().d_x) / destRect.getWidth();
            rightfactor  = leftfactor + finalRect.getWidth() / destRect.getWidth();
            topfactor    = (finalRect.top() + componentImage->getRenderedOffset().d_y) / destRect.getHeight();
            bottomfactor = topfactor + finalRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
        }

        // draw this element.
        renderImage(srcWindow.getGeometryBuffer(), componentImage,
                    d_rightEdgeFormatting.get(srcWindow), HF_RIGHT_ALIGNED,
                    finalRect, imageColours, clipper, clipToDisplay);
    }

    if (const Image* const componentImage = getImage(FIC_BACKGROUND, srcWindow))
    {
        // calculate colours that are to be used to this component image
        if (calcColoursPerImage)
        {
            leftfactor   = (backgroundRect.left() + componentImage->getRenderedOffset().d_x) / destRect.getWidth();
            rightfactor  = leftfactor + backgroundRect.getWidth() / destRect.getWidth();
            topfactor    = (backgroundRect.top() + componentImage->getRenderedOffset().d_y) / destRect.getHeight();
            bottomfactor = topfactor + backgroundRect.getHeight() / destRect.getHeight();

            imageColours = finalColours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
        }

        const HorizontalFormatting horzFormatting =
            d_backgroundHorzFormatting.get(srcWindow);

        const VerticalFormatting vertFormatting =
            d_backgroundVertFormatting.get(srcWindow);

        renderImage(srcWindow.getGeometryBuffer(), componentImage,
                    vertFormatting, horzFormatting,
                    backgroundRect, imageColours, clipper, clipToDisplay);
    }
}

//----------------------------------------------------------------------------//
void FrameComponent::renderImage(GeometryBuffer& buffer, const Image* image,
                                 VerticalFormatting vertFmt,
                                 HorizontalFormatting horzFmt,
                                 Rectf& destRect, const ColourRect& colours,
                                 const Rectf* clipper, bool /*clipToDisplay*/) const
{
    uint horzTiles, vertTiles;
    float xpos, ypos;

    Sizef imgSz(image->getRenderedSize());

    // calculate initial x co-ordinate and horizontal tile count according to formatting options
    switch (horzFmt)
    {
        case HF_STRETCHED:
            imgSz.d_width = destRect.getWidth();
            xpos = destRect.left();
            horzTiles = 1;
            break;

        case HF_TILED:
            xpos = destRect.left();
            horzTiles = std::abs(static_cast<int>(
                (destRect.getWidth() + (imgSz.d_width - 1)) / imgSz.d_width));
            break;

        case HF_LEFT_ALIGNED:
            xpos = destRect.left();
            horzTiles = 1;
            break;

        case HF_CENTRE_ALIGNED:
            xpos = destRect.left() + CoordConverter::alignToPixels((destRect.getWidth() - imgSz.d_width) * 0.5f);
            horzTiles = 1;
            break;

        case HF_RIGHT_ALIGNED:
            xpos = destRect.right() - imgSz.d_width;
            horzTiles = 1;
            break;

        default:
            CEGUI_THROW(InvalidRequestException(
                "An unknown HorizontalFormatting value was specified."));
    }

    // calculate initial y co-ordinate and vertical tile count according to formatting options
    switch (vertFmt)
    {
        case VF_STRETCHED:
            imgSz.d_height = destRect.getHeight();
            ypos = destRect.top();
            vertTiles = 1;
            break;

        case VF_TILED:
            ypos = destRect.top();
            vertTiles = std::abs(static_cast<int>(
                (destRect.getHeight() + (imgSz.d_height - 1)) / imgSz.d_height));
            break;

        case VF_TOP_ALIGNED:
            ypos = destRect.top();
            vertTiles = 1;
            break;

        case VF_CENTRE_ALIGNED:
            ypos = destRect.top() + CoordConverter::alignToPixels((destRect.getHeight() - imgSz.d_height) * 0.5f);
            vertTiles = 1;
            break;

        case VF_BOTTOM_ALIGNED:
            ypos = destRect.bottom() - imgSz.d_height;
            vertTiles = 1;
            break;

        default:
            CEGUI_THROW(InvalidRequestException(
                "An unknown VerticalFormatting value was specified."));
    }

    // perform final rendering (actually is now a caching of the images which will be drawn)
    Rectf finalRect;
    Rectf finalClipper;
    const Rectf* clippingRect;
    finalRect.d_min.d_y = ypos;
    finalRect.d_max.d_y = ypos + imgSz.d_height;

    for (uint row = 0; row < vertTiles; ++row)
    {
        finalRect.d_min.d_x = xpos;
        finalRect.d_max.d_x = xpos + imgSz.d_width;

        for (uint col = 0; col < horzTiles; ++col)
        {
            // use custom clipping for right and bottom edges when tiling the imagery
            if (((vertFmt == VF_TILED) && row == vertTiles - 1) ||
                ((horzFmt == HF_TILED) && col == horzTiles - 1))
            {
                finalClipper = clipper ? clipper->getIntersection(destRect) : destRect;
                clippingRect = &finalClipper;
            }
            // not tiliing, or not on far edges, just used passed in clipper (if any).
            else
                clippingRect = clipper;

            image->render(buffer, finalRect, clippingRect, colours);

            finalRect.d_min.d_x += imgSz.d_width;
            finalRect.d_max.d_x += imgSz.d_width;
        }

        finalRect.d_min.d_y += imgSz.d_height;
        finalRect.d_max.d_y += imgSz.d_height;
    }
}

//----------------------------------------------------------------------------//
void FrameComponent::writeXMLToStream(XMLSerializer& xml_stream) const
{
    // opening tag
    xml_stream.openTag(Falagard_xmlHandler::FrameComponentElement);
    // write out area
    d_area.writeXMLToStream(xml_stream);

    // write images
    for (int i = 0; i < FIC_FRAME_IMAGE_COUNT; ++i)
    {
        if (d_frameImages[i].d_specified)
        {
            if (d_frameImages[i].d_propertyName.empty())
                xml_stream.openTag(Falagard_xmlHandler::ImageElement)
                    .attribute(Falagard_xmlHandler::ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::toString(static_cast<FrameImageComponent>(i)))
                    .attribute(Falagard_xmlHandler::NameAttribute, d_frameImages[i].d_image->getName())
                    .closeTag();
            else
                xml_stream.openTag(Falagard_xmlHandler::ImagePropertyElement)
                    .attribute(Falagard_xmlHandler::ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::toString(static_cast<FrameImageComponent>(i)))
                    .attribute(Falagard_xmlHandler::NameAttribute, d_frameImages[i].d_propertyName)
                    .closeTag();
        }
    }

    // get base class to write colours
    writeColoursXML(xml_stream);

    if(d_leftEdgeFormatting.getValue() != VerticalFormattingDefault)
    {
        d_leftEdgeFormatting.writeXMLTagToStream(xml_stream);
        xml_stream.attribute(Falagard_xmlHandler::ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::LeftEdge);
        d_leftEdgeFormatting.writeXMLAttributesToStream(xml_stream);
        xml_stream.closeTag();
    }

    if(d_rightEdgeFormatting.getValue() != VerticalFormattingDefault)
    {
        d_rightEdgeFormatting.writeXMLTagToStream(xml_stream);
        xml_stream.attribute(Falagard_xmlHandler::ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::RightEdge);
        d_rightEdgeFormatting.writeXMLAttributesToStream(xml_stream);
        xml_stream.closeTag();
    }

    if(d_backgroundHorzFormatting.getValue() != HorizontalFormattingDefault)
    {
        d_backgroundHorzFormatting.writeXMLTagToStream(xml_stream);
        xml_stream.attribute(Falagard_xmlHandler::ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::Background);
        d_backgroundHorzFormatting.writeXMLAttributesToStream(xml_stream);
        xml_stream.closeTag();
    }

    if(d_topEdgeFormatting.getValue() != HorizontalFormattingDefault)
    {
        d_topEdgeFormatting.writeXMLTagToStream(xml_stream);
        xml_stream.attribute(Falagard_xmlHandler::ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::TopEdge);
        d_topEdgeFormatting.writeXMLAttributesToStream(xml_stream);
        xml_stream.closeTag();
    }

    if(d_bottomEdgeFormatting.getValue() != HorizontalFormattingDefault)
    {
        d_bottomEdgeFormatting.writeXMLTagToStream(xml_stream);
        xml_stream.attribute(Falagard_xmlHandler::ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::BottomEdge);
        d_bottomEdgeFormatting.writeXMLAttributesToStream(xml_stream);
        xml_stream.closeTag();
    }

    if(d_backgroundVertFormatting.getValue() != VerticalFormattingDefault)
    {
        d_backgroundVertFormatting.writeXMLTagToStream(xml_stream);
        xml_stream.attribute(Falagard_xmlHandler::ComponentAttribute, FalagardXMLHelper<FrameImageComponent>::Background);
        d_backgroundVertFormatting.writeXMLAttributesToStream(xml_stream);
        xml_stream.closeTag();
    }

    // closing tag
    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//
bool FrameComponent::operator==(const FrameComponent& rhs) const
{
    if (d_backgroundVertFormatting != rhs.d_backgroundVertFormatting ||
        d_backgroundHorzFormatting != rhs.d_backgroundHorzFormatting)
            return false;

    for (int i = 0; i < FIC_FRAME_IMAGE_COUNT; ++i)
        if (d_frameImages[i] != rhs.d_frameImages[i])
            return false;

    return true;
}

//----------------------------------------------------------------------------//

}

