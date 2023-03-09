/************************************************************************
    filename:   CEGUIFalImageryComponent.cpp
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
#include "falagard/CEGUIFalImageryComponent.h"
#include "falagard/CEGUIFalXMLEnumHelper.h"
#include "CEGUIImage.h"
#include "CEGUIExceptions.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIPropertyHelper.h"
#include <iostream>

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

    void ImageryComponent::setImage(const String& imageset, const String& image)
    {
        try
        {
            d_image = &ImagesetManager::getSingleton().getImageset(imageset)->getImage(image);
        }
        catch (UnknownObjectException)
        {
            d_image = 0;
        }
    }

    VerticalFormatting ImageryComponent::getVerticalFormatting() const
    {
        return d_vertFormatting;
    }

    void ImageryComponent::setVerticalFormatting(VerticalFormatting fmt)
    {
        d_vertFormatting = fmt;
    }

    HorizontalFormatting ImageryComponent::getHorizontalFormatting() const
    {
        return d_horzFormatting;
    }

    void ImageryComponent::setHorizontalFormatting(HorizontalFormatting fmt)
    {
        d_horzFormatting = fmt;
    }

    void ImageryComponent::render_impl(Window& srcWindow, Rect& destRect, float base_z, const CEGUI::ColourRect* modColours, const Rect* clipper, bool clipToDisplay) const
    {
        // get final image to use.
        const Image* img = isImageFetchedFromProperty() ?
            PropertyHelper::stringToImage(srcWindow.getProperty(d_imagePropertyName)) :
            d_image;

        // do not draw anything if image is not set.
        if (!img)
            return;

        HorizontalFormatting horzFormatting = d_horzFormatPropertyName.empty() ? d_horzFormatting :
            FalagardXMLHelper::stringToHorzFormat(srcWindow.getProperty(d_horzFormatPropertyName));

        VerticalFormatting vertFormatting = d_vertFormatPropertyName.empty() ? d_vertFormatting :
            FalagardXMLHelper::stringToVertFormat(srcWindow.getProperty(d_vertFormatPropertyName));

        uint horzTiles, vertTiles;
        float xpos, ypos;

        Size imgSz(img->getSize());

        // calculate final colours to be used
        ColourRect finalColours;
        initColoursRect(srcWindow, modColours, finalColours);

        // calculate initial x co-ordinate and horizontal tile count according to formatting options
        switch (horzFormatting)
        {
            case HF_STRETCHED:
                imgSz.d_width = destRect.getWidth();
                xpos = destRect.d_left;
                horzTiles = 1;
                break;

            case HF_TILED:
                xpos = destRect.d_left;
                horzTiles = (uint)((destRect.getWidth() + (imgSz.d_width - 1)) / imgSz.d_width);
                break;

            case HF_LEFT_ALIGNED:
                xpos = destRect.d_left;
                horzTiles = 1;
                break;

            case HF_CENTRE_ALIGNED:
                xpos = destRect.d_left + PixelAligned((destRect.getWidth() - imgSz.d_width) * 0.5f);
                horzTiles = 1;
                break;

            case HF_RIGHT_ALIGNED:
                xpos = destRect.d_right - imgSz.d_width;
                horzTiles = 1;
                break;

            default:
                throw InvalidRequestException("ImageryComponent::render - An unknown HorizontalFormatting value was specified.");
        }

        // calculate initial y co-ordinate and vertical tile count according to formatting options
        switch (vertFormatting)
        {
            case VF_STRETCHED:
                imgSz.d_height = destRect.getHeight();
                ypos = destRect.d_top;
                vertTiles = 1;
                break;

            case VF_TILED:
                ypos = destRect.d_top;
                vertTiles = (uint)((destRect.getHeight() + (imgSz.d_height - 1)) / imgSz.d_height);
                break;

            case VF_TOP_ALIGNED:
                ypos = destRect.d_top;
                vertTiles = 1;
                break;

            case VF_CENTRE_ALIGNED:
                ypos = destRect.d_top + PixelAligned((destRect.getHeight() - imgSz.d_height) * 0.5f);
                vertTiles = 1;
                break;

            case VF_BOTTOM_ALIGNED:
                ypos = destRect.d_bottom - imgSz.d_height;
                vertTiles = 1;
                break;

            default:
                throw InvalidRequestException("ImageryComponent::render - An unknown VerticalFormatting value was specified.");
        }

        // perform final rendering (actually is now a caching of the images which will be drawn)
        Rect finalRect;
        Rect finalClipper;
        const Rect* clippingRect;
        finalRect.d_top = ypos;
        finalRect.d_bottom = ypos + imgSz.d_height;

        for (uint row = 0; row < vertTiles; ++row)
        {
            finalRect.d_left = xpos;
            finalRect.d_right = xpos + imgSz.d_width;

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

                // add image to the rendering cache for the target window.
                srcWindow.getRenderCache().cacheImage(*img, finalRect, base_z, finalColours, clippingRect, clipToDisplay);

                finalRect.d_left += imgSz.d_width;
                finalRect.d_right += imgSz.d_width;
            }

            finalRect.d_top += imgSz.d_height;
            finalRect.d_bottom += imgSz.d_height;
        }
    }

    void ImageryComponent::writeXMLToStream(OutStream& out_stream) const
    {
        // opening tag
        out_stream << "<ImageryComponent>" << std::endl;
        // write out area
        d_area.writeXMLToStream(out_stream);

        // write image
        if (isImageFetchedFromProperty())
            out_stream << "<ImageProperty name=\"" << d_imagePropertyName << "\" />" << std::endl;
        else
            out_stream << "<Image imageset=\"" << d_image->getImagesetName() << "\" image=\"" << d_image->getName() << "\" />" << std::endl;

        // get base class to write colours
        writeColoursXML(out_stream);

        // write vert format, allowing base class to do this for us if a propety is in use
        if (!writeVertFormatXML(out_stream))
        {
            // was not a property, so write out explicit formatting in use
            out_stream << "<VertFormat type=\"" << FalagardXMLHelper::vertFormatToString(d_vertFormatting) << "\" />" << std::endl;
        }

        // write horz format, allowing base class to do this for us if a propety is in use
        if (!writeHorzFormatXML(out_stream))
        {
            // was not a property, so write out explicit formatting in use
            out_stream << "<HorzFormat type=\"" << FalagardXMLHelper::horzFormatToString(d_horzFormatting) << "\" />" << std::endl;
        }

        // closing tag
        out_stream << "</ImageryComponent>" << std::endl;
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
