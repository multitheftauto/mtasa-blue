/************************************************************************
    filename:   CEGUIFalImagerySection.cpp
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
#include "falagard/CEGUIFalImagerySection.h"
#include "CEGUIPropertyHelper.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    ImagerySection::ImagerySection() :
        d_masterColours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        d_colourProperyIsRect(false)
    {}

    ImagerySection::ImagerySection(const String& name) :
        d_name(name),
        d_masterColours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        d_colourProperyIsRect(false)
    {}

    void ImagerySection::render(Window& srcWindow, float base_z, const CEGUI::ColourRect* modColours, const Rect* clipper, bool clipToDisplay) const
    {
        // decide what to do as far as colours go
        ColourRect finalCols;
        initMasterColourRect(srcWindow, finalCols);

        if (modColours)
            finalCols *= *modColours;

        ColourRect* finalColsPtr = (finalCols.isMonochromatic() && finalCols.d_top_left.getARGB() == 0xFFFFFFFF) ? 0 : &finalCols;

        // render all frame components in this section
        for(FrameList::const_iterator frame = d_frames.begin(); frame != d_frames.end(); ++frame)
        {
            (*frame).render(srcWindow, base_z, finalColsPtr, clipper, clipToDisplay);
        }
        // render all image components in this section
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            (*image).render(srcWindow, base_z, finalColsPtr, clipper, clipToDisplay);
        }
        // render all text components in this section
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            (*text).render(srcWindow, base_z, finalColsPtr, clipper, clipToDisplay);
        }
    }

    void ImagerySection::render(Window& srcWindow, const Rect& baseRect, float base_z, const CEGUI::ColourRect* modColours, const Rect* clipper, bool clipToDisplay) const
    {
        // decide what to do as far as colours go
        ColourRect finalCols;
        initMasterColourRect(srcWindow, finalCols);

        if (modColours)
            finalCols *= *modColours;

        ColourRect* finalColsPtr = (finalCols.isMonochromatic() && finalCols.d_top_left.getARGB() == 0xFFFFFFFF) ? 0 : &finalCols;

        // render all frame components in this section
        for(FrameList::const_iterator frame = d_frames.begin(); frame != d_frames.end(); ++frame)
        {
            (*frame).render(srcWindow, baseRect, base_z, finalColsPtr, clipper, clipToDisplay);
        }
        // render all image components in this section
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            (*image).render(srcWindow, baseRect, base_z, finalColsPtr, clipper, clipToDisplay);
        }
        // render all text components in this section
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            (*text).render(srcWindow, baseRect, base_z, finalColsPtr, clipper, clipToDisplay);
        }
    }

    void ImagerySection::addImageryComponent(const ImageryComponent& img)
    {
        d_images.push_back(img);
    }

    void ImagerySection::clearImageryComponents()
    {
        d_images.clear();
    }

    void ImagerySection::addTextComponent(const TextComponent& text)
    {
        d_texts.push_back(text);
    }

    void ImagerySection::clearTextComponents()
    {
        d_texts.clear();
    }

    void ImagerySection::clearFrameComponents()
    {
        d_frames.clear();
    }

    void ImagerySection::addFrameComponent(const FrameComponent& frame)
    {
        d_frames.push_back(frame);
    }

    const ColourRect& ImagerySection::getMasterColours() const
    {
        return d_masterColours;
    }

    void ImagerySection::setMasterColours(const ColourRect& cols)
    {
        d_masterColours = cols;
    }

    const String& ImagerySection::getName() const
    {
        return d_name;
    }

    void ImagerySection::setMasterColoursPropertySource(const String& property)
    {
        d_colourPropertyName = property;
    }

    void ImagerySection::setMasterColoursPropertyIsColourRect(bool setting)
    {
        d_colourProperyIsRect = setting;
    }

    void ImagerySection::initMasterColourRect(const Window& wnd, ColourRect& cr) const
    {
        // if colours come via a colour property
        if (!d_colourPropertyName.empty())
        {
            // if property accesses a ColourRect
            if (d_colourProperyIsRect)
            {
                cr = PropertyHelper::stringToColourRect(wnd.getProperty(d_colourPropertyName));
            }
            // property accesses a colour
            else
            {
                colour val(PropertyHelper::stringToColour(wnd.getProperty(d_colourPropertyName)));
                cr.d_top_left     = val;
                cr.d_top_right    = val;
                cr.d_bottom_left  = val;
                cr.d_bottom_right = val;
            }
        }
        // use explicit ColourRect.
        else
        {
            cr = d_masterColours;
        }
    }

    Rect ImagerySection::getBoundingRect(const Window& wnd) const
    {
        Rect compRect;
        Rect bounds(0, 0, 0, 0);

        // measure all frame components
        for(FrameList::const_iterator frame = d_frames.begin(); frame != d_frames.end(); ++frame)
        {
            compRect = (*frame).getComponentArea().getPixelRect(wnd);

            bounds.d_left   = ceguimin(bounds.d_left, compRect.d_left);
            bounds.d_top    = ceguimin(bounds.d_top, compRect.d_top);
            bounds.d_right  = ceguimax(bounds.d_right, compRect.d_right);
            bounds.d_bottom = ceguimax(bounds.d_bottom, compRect.d_bottom);
        }
        // measure all imagery components
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            compRect = (*image).getComponentArea().getPixelRect(wnd);

            bounds.d_left   = ceguimin(bounds.d_left, compRect.d_left);
            bounds.d_top    = ceguimin(bounds.d_top, compRect.d_top);
            bounds.d_right  = ceguimax(bounds.d_right, compRect.d_right);
            bounds.d_bottom = ceguimax(bounds.d_bottom, compRect.d_bottom);
        }
        // measure all text components
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            compRect = (*text).getComponentArea().getPixelRect(wnd);

            bounds.d_left   = ceguimin(bounds.d_left, compRect.d_left);
            bounds.d_top    = ceguimin(bounds.d_top, compRect.d_top);
            bounds.d_right  = ceguimax(bounds.d_right, compRect.d_right);
            bounds.d_bottom = ceguimax(bounds.d_bottom, compRect.d_bottom);
        }

        return bounds;
    }

    Rect ImagerySection::getBoundingRect(const Window& wnd, const Rect& rect) const
    {
        Rect compRect;
        Rect bounds(0, 0, 0, 0);

        // measure all frame components
        for(FrameList::const_iterator frame = d_frames.begin(); frame != d_frames.end(); ++frame)
        {
            compRect = (*frame).getComponentArea().getPixelRect(wnd, rect);

            bounds.d_left   = ceguimin(bounds.d_left, compRect.d_left);
            bounds.d_top    = ceguimin(bounds.d_top, compRect.d_top);
            bounds.d_right  = ceguimax(bounds.d_right, compRect.d_right);
            bounds.d_bottom = ceguimax(bounds.d_bottom, compRect.d_bottom);
        }
        // measure all imagery components
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            compRect = (*image).getComponentArea().getPixelRect(wnd, rect);

            bounds.d_left   = ceguimin(bounds.d_left, compRect.d_left);
            bounds.d_top    = ceguimin(bounds.d_top, compRect.d_top);
            bounds.d_right  = ceguimax(bounds.d_right, compRect.d_right);
            bounds.d_bottom = ceguimax(bounds.d_bottom, compRect.d_bottom);
        }
        // measure all text components
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            compRect = (*text).getComponentArea().getPixelRect(wnd, rect);

            bounds.d_left   = ceguimin(bounds.d_left, compRect.d_left);
            bounds.d_top    = ceguimin(bounds.d_top, compRect.d_top);
            bounds.d_right  = ceguimax(bounds.d_right, compRect.d_right);
            bounds.d_bottom = ceguimax(bounds.d_bottom, compRect.d_bottom);
        }

        return bounds;
    }

    void ImagerySection::writeXMLToStream(OutStream& out_stream) const
    {
        // output opening tag
        out_stream << "<ImagerySection name=\"" << d_name << "\">" << std::endl;

        // output modulative colours for this section
        if (!d_colourPropertyName.empty())
        {
            if (d_colourProperyIsRect)
                out_stream << "<ColourRectProperty ";
            else
                out_stream << "<ColourProperty ";

            out_stream << "name=\"" << d_colourPropertyName << "\" />" << std::endl;
        }
        else if (!d_masterColours.isMonochromatic() || d_masterColours.d_top_left != colour(1,1,1,1))
        {
            out_stream << "<Colours ";
            out_stream << "topLeft=\"" << PropertyHelper::colourToString(d_masterColours.d_top_left) << "\" ";
            out_stream << "topRight=\"" << PropertyHelper::colourToString(d_masterColours.d_top_right) << "\" ";
            out_stream << "bottomLeft=\"" << PropertyHelper::colourToString(d_masterColours.d_bottom_left) << "\" ";
            out_stream << "bottomRight=\"" << PropertyHelper::colourToString(d_masterColours.d_bottom_right) << "\" />" << std::endl;
        }

        // output all frame components.
        for(FrameList::const_iterator frame = d_frames.begin(); frame != d_frames.end(); ++frame)
        {
            (*frame).writeXMLToStream(out_stream);
        }

        // output all imagery components
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            (*image).writeXMLToStream(out_stream);
        }

        // output all text components
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            (*text).writeXMLToStream(out_stream);
        }

        // output closing tag
        out_stream << "</ImagerySection>" << std::endl;
    }

} // End of  CEGUI namespace section
