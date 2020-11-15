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
#include "CEGUI/falagard/ImageryComponent.h"
#include "CEGUI/falagard/XMLEnumHelper.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/Image.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/CoordConverter.h"
#include <iostream>
#include <cstdlib>

// void	draw(const Rect& dest_rect, float z, const Rect& clip_rect,const ColourRect& colours);

// Start of CEGUI namespace section
namespace CEGUI
{
    ImageryComponent::ImageryComponent() :
        d_image(0),
        d_vertFormatting(VF_TOP_ALIGNED),
        d_horzFormatting(HF_LEFT_ALIGNED)
    {}

    const Image* ImageryComponent::getImage() const
    {
        return d_image;
    }

    void ImageryComponent::setImage(const Image* image)
    {
        d_image = image;
    }

    void ImageryComponent::setImage(const String& name)
    {
        CEGUI_TRY
        {
            d_image = &ImageManager::getSingleton().get(name);
        }
        CEGUI_CATCH (UnknownObjectException&)
        {
            d_image = 0;
        }
    }

    VerticalFormatting ImageryComponent::getVerticalFormatting(const Window& wnd) const
    {
        return d_vertFormatting.get(wnd);
    }

    VerticalFormatting ImageryComponent::getVerticalFormattingFromComponent() const
    {
        return d_vertFormatting.getValue();
    }

    void ImageryComponent::setVerticalFormatting(VerticalFormatting fmt)
    {
        d_vertFormatting.set(fmt);
    }

    HorizontalFormatting ImageryComponent::getHorizontalFormatting(const Window& wnd) const
    {
        return d_horzFormatting.get(wnd);
    }

    HorizontalFormatting ImageryComponent::getHorizontalFormattingFromComponent() const
    {
        return d_horzFormatting.getValue();
    }

    void ImageryComponent::setHorizontalFormatting(HorizontalFormatting fmt)
    {
        d_horzFormatting.set(fmt);
    }

    const String& ImageryComponent::getHorizontalFormattingPropertySource() const
    {
        return d_horzFormatting.getPropertySource();
    }

    void ImageryComponent::setHorizontalFormattingPropertySource(
                                                const String& property_name)
    {
        d_horzFormatting.setPropertySource(property_name);
    }

    const String& ImageryComponent::getVerticalFormattingPropertySource() const
    {
        return d_vertFormatting.getPropertySource();
    }

    void ImageryComponent::setVerticalFormattingPropertySource(
                                                const String& property_name)
    {
        d_vertFormatting.setPropertySource(property_name);
    }

    void ImageryComponent::render_impl(Window& srcWindow, Rectf& destRect, const CEGUI::ColourRect* modColours, const Rectf* clipper, bool /*clipToDisplay*/) const
    {
        // get final image to use.
        const Image* img = isImageFetchedFromProperty() ?
            srcWindow.getProperty<Image*>(d_imagePropertyName) :
            d_image;

        // do not draw anything if image is not set.
        if (!img)
            return;

        const HorizontalFormatting horzFormatting = d_horzFormatting.get(srcWindow);
        const VerticalFormatting vertFormatting = d_vertFormatting.get(srcWindow);

        uint horzTiles, vertTiles;
        float xpos, ypos;

        Sizef imgSz(img->getRenderedSize());

        // calculate final colours to be used
        ColourRect finalColours;
        initColoursRect(srcWindow, modColours, finalColours);

        // calculate initial x co-ordinate and horizontal tile count according to formatting options
        switch (horzFormatting)
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
        switch (vertFormatting)
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
        finalRect.top(ypos);
        finalRect.bottom(ypos + imgSz.d_height);

        for (uint row = 0; row < vertTiles; ++row)
        {
            finalRect.left(xpos);
            finalRect.right(xpos + imgSz.d_width);

            for (uint col = 0; col < horzTiles; ++col)
            {
                // use custom clipping for right and bottom edges when tiling the imagery
                if (((vertFormatting == VF_TILED) && row == vertTiles - 1) ||
                    ((horzFormatting == HF_TILED) && col == horzTiles - 1))
                {
                    finalClipper = clipper ? clipper->getIntersection(destRect) : destRect;
                    clippingRect = &finalClipper;
                }
                // not tiliing, or not on far edges, just used passed in clipper (if any).
                else
                {
                    clippingRect = clipper;
                }

                // add geometry for image to the target window.
                img->render(srcWindow.getGeometryBuffer(), finalRect, clippingRect, finalColours);

                finalRect.d_min.d_x += imgSz.d_width;
                finalRect.d_max.d_x += imgSz.d_width;
            }

            finalRect.d_min.d_y += imgSz.d_height;
            finalRect.d_max.d_y += imgSz.d_height;
        }
    }

    void ImageryComponent::writeXMLToStream(XMLSerializer& xml_stream) const
    {
        // opening tag
        xml_stream.openTag(Falagard_xmlHandler::ImageryComponentElement);
        // write out area
        d_area.writeXMLToStream(xml_stream);

        // write image
        if (isImageFetchedFromProperty())
            xml_stream.openTag(Falagard_xmlHandler::ImagePropertyElement)
            .attribute(Falagard_xmlHandler::NameAttribute, d_imagePropertyName)
                .closeTag();
        else
            xml_stream.openTag(Falagard_xmlHandler::ImageElement)
                .attribute(Falagard_xmlHandler::NameAttribute, d_image->getName())
                .closeTag();

        // get base class to write colours
        writeColoursXML(xml_stream);

        d_vertFormatting.writeXMLToStream(xml_stream);
        d_horzFormatting.writeXMLToStream(xml_stream);

        // closing tag
        xml_stream.closeTag();
    }

    bool ImageryComponent::isImageFetchedFromProperty() const
    {
        return !d_imagePropertyName.empty();
    }

    const String& ImageryComponent::getImagePropertySource() const
    {
        return d_imagePropertyName;
    }

    void ImageryComponent::setImagePropertySource(const String& property)
    {
        d_imagePropertyName = property;
    }

} // End of  CEGUI namespace section
