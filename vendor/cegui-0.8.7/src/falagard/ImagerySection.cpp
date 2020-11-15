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
#include "CEGUI/falagard/ImagerySection.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/Logger.h"
#include <iostream>
#include <limits>

// Start of CEGUI namespace section
namespace CEGUI
{
    ImagerySection::ImagerySection() :
        d_masterColours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
    {}

    ImagerySection::ImagerySection(const String& name) :
        d_name(name),
        d_masterColours(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
    {}

    void ImagerySection::render(Window& srcWindow, const CEGUI::ColourRect* modColours, const Rectf* clipper, bool clipToDisplay) const
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
            (*frame).render(srcWindow, finalColsPtr, clipper, clipToDisplay);
        }
        // render all image components in this section
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            (*image).render(srcWindow, finalColsPtr, clipper, clipToDisplay);
        }
        // render all text components in this section
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            (*text).render(srcWindow, finalColsPtr, clipper, clipToDisplay);
        }
    }

    void ImagerySection::render(Window& srcWindow, const Rectf& baseRect, const CEGUI::ColourRect* modColours, const Rectf* clipper, bool clipToDisplay) const
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
            (*frame).render(srcWindow, baseRect, finalColsPtr, clipper, clipToDisplay);
        }
        // render all image components in this section
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            (*image).render(srcWindow, baseRect, finalColsPtr, clipper, clipToDisplay);
        }
        // render all text components in this section
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            (*text).render(srcWindow, baseRect, finalColsPtr, clipper, clipToDisplay);
        }
    }

    void ImagerySection::addImageryComponent(const ImageryComponent& imageryComponent)
    {
        d_images.push_back(imageryComponent);
    }

    void ImagerySection::removeImageryComponent(const ImageryComponent& imageryComponent)
    {
        ImageryList::iterator imageryComponentIter = d_images.begin();
        ImageryList::iterator imageryComponentIterEnd = d_images.end();
        while(imageryComponentIter != imageryComponentIterEnd)
        {
            if(&(*imageryComponentIter) == &imageryComponent)
            {
                d_images.erase(imageryComponentIter);
                return;
            }
            
            ++imageryComponentIter;
        }

        Logger::getSingleton().logEvent("ImagerySection::removeImageryComponent: The ImageryComponent that has been supplied  "
                                        "could not be found in this ImagerySection. No element has been removed");
    }

    void ImagerySection::clearImageryComponents()
    {
        d_images.clear();
    }

    void ImagerySection::addTextComponent(const TextComponent& textComponent)
    {
        d_texts.push_back(textComponent);
    }

    void ImagerySection::removeTextComponent(const TextComponent& textComponent)
    {
        TextList::iterator textComponentIter = d_texts.begin();
        TextList::iterator textComponentIterEnd = d_texts.end();
        while(textComponentIter != textComponentIterEnd)
        {
            if(&(*textComponentIter) == &textComponent)
            {
                d_texts.erase(textComponentIter);
                return;
            }

            ++textComponentIter;
        }

        Logger::getSingleton().logEvent("ImagerySection::removeTextComponent: The TextComponent that has been supplied  "
            "could not be found in this ImagerySection. No element has been removed");
    }

    void ImagerySection::clearTextComponents()
    {
        d_texts.clear();
    }

    void ImagerySection::clearFrameComponents()
    {
        d_frames.clear();
    }

    void ImagerySection::addFrameComponent(const FrameComponent& frameComponent)
    {
        d_frames.push_back(frameComponent);
    }

    void ImagerySection::removeFrameComponent(const FrameComponent& frameComponent)
    {
        FrameList::iterator frameComponentIter = d_frames.begin();
        FrameList::iterator frameComponentIterEnd = d_frames.end();
        while(frameComponentIter != frameComponentIterEnd)
        {
            if(&(*frameComponentIter) == &frameComponent)
            {
                d_frames.erase(frameComponentIter);
                return;
            }

            ++frameComponentIter;
        }

        Logger::getSingleton().logEvent("ImagerySection::removeFrameComponent: The FrameComponent that has been supplied  "
            "could not be found in this ImagerySection. No element has been removed");
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

    void ImagerySection::setName(const String& name )
    {
        d_name = name;
    }

    const String& ImagerySection::getMasterColoursPropertySource() const
    {
        return d_colourPropertyName;
    }

    void ImagerySection::setMasterColoursPropertySource(const String& property)
    {
        d_colourPropertyName = property;
    }

    void ImagerySection::initMasterColourRect(const Window& wnd, ColourRect& cr) const
    {
        // if colours come via a colour property
        if (!d_colourPropertyName.empty())
        {
            // if property accesses a ColourRect or a colour
            cr = wnd.getProperty<ColourRect>(d_colourPropertyName);
        }
        // use explicit ColourRect.
        else
        {
            cr = d_masterColours;
        }
    }

    Rectf ImagerySection::getBoundingRect(const Window& wnd) const
    {
        Rectf compRect;
        Rectf bounds(std::numeric_limits<float>::max(), 
                     std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::min(),
                     std::numeric_limits<float>::min());

        // measure all frame components
        for(FrameList::const_iterator frame = d_frames.begin(); frame != d_frames.end(); ++frame)
        {
            compRect = (*frame).getComponentArea().getPixelRect(wnd);

            bounds.left(ceguimin(bounds.left(), compRect.left()));
            bounds.top(ceguimin(bounds.top(), compRect.top()));
            bounds.right(ceguimax(bounds.right(), compRect.right()));
            bounds.bottom(ceguimax(bounds.bottom(), compRect.bottom()));
        }
        // measure all imagery components
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            compRect = (*image).getComponentArea().getPixelRect(wnd);

            bounds.left(ceguimin(bounds.left(), compRect.left()));
            bounds.top(ceguimin(bounds.top(), compRect.top()));
            bounds.right(ceguimax(bounds.right(), compRect.right()));
            bounds.bottom(ceguimax(bounds.bottom(), compRect.bottom()));
        }
        // measure all text components
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            compRect = (*text).getComponentArea().getPixelRect(wnd);

            bounds.left(ceguimin(bounds.left(), compRect.left()));
            bounds.top(ceguimin(bounds.top(), compRect.top()));
            bounds.right(ceguimax(bounds.right(), compRect.right()));
            bounds.bottom(ceguimax(bounds.bottom(), compRect.bottom()));
        }

        return bounds;
    }

    Rectf ImagerySection::getBoundingRect(const Window& wnd, const Rectf& rect) const
    {
        Rectf compRect;
        Rectf bounds(std::numeric_limits<float>::max(), 
                     std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::min(),
                     std::numeric_limits<float>::min());

        // measure all frame components
        for(FrameList::const_iterator frame = d_frames.begin(); frame != d_frames.end(); ++frame)
        {
            compRect = (*frame).getComponentArea().getPixelRect(wnd, rect);

            bounds.left(ceguimin(bounds.left(), compRect.left()));
            bounds.top(ceguimin(bounds.top(), compRect.top()));
            bounds.right(ceguimax(bounds.right(), compRect.right()));
            bounds.bottom(ceguimax(bounds.bottom(), compRect.bottom()));
        }
        // measure all imagery components
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            compRect = (*image).getComponentArea().getPixelRect(wnd, rect);

            bounds.left(ceguimin(bounds.left(), compRect.left()));
            bounds.top(ceguimin(bounds.top(), compRect.top()));
            bounds.right(ceguimax(bounds.right(), compRect.right()));
            bounds.bottom(ceguimax(bounds.bottom(), compRect.bottom()));
        }
        // measure all text components
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            compRect = (*text).getComponentArea().getPixelRect(wnd, rect);

            bounds.left(ceguimin(bounds.left(), compRect.left()));
            bounds.top(ceguimin(bounds.top(), compRect.top()));
            bounds.right(ceguimax(bounds.right(), compRect.right()));
            bounds.bottom(ceguimax(bounds.bottom(), compRect.bottom()));
        }

        return bounds;
    }

    void ImagerySection::writeXMLToStream(XMLSerializer& xml_stream) const
    {
        // output opening tag
        xml_stream.openTag(Falagard_xmlHandler::ImagerySectionElement)
            .attribute(Falagard_xmlHandler::NameAttribute, d_name);

        // output modulative colours for this section
        if (!d_colourPropertyName.empty())
        {
            xml_stream.openTag(Falagard_xmlHandler::ColourRectPropertyElement);
            xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_colourPropertyName)
                .closeTag();
        }
        else if (!d_masterColours.isMonochromatic() || d_masterColours.d_top_left != Colour(1,1,1,1))
        {
            xml_stream.openTag(Falagard_xmlHandler::ColoursElement)
                .attribute(Falagard_xmlHandler::TopLeftAttribute, PropertyHelper<Colour>::toString(d_masterColours.d_top_left))
                .attribute(Falagard_xmlHandler::TopRightAttribute, PropertyHelper<Colour>::toString(d_masterColours.d_top_right))
                .attribute(Falagard_xmlHandler::BottomLeftAttribute, PropertyHelper<Colour>::toString(d_masterColours.d_bottom_left))
                .attribute(Falagard_xmlHandler::BottomRightAttribute, PropertyHelper<Colour>::toString(d_masterColours.d_bottom_right))
                .closeTag();
        }

        // output all frame components.
        for(FrameList::const_iterator frame = d_frames.begin(); frame != d_frames.end(); ++frame)
        {
            (*frame).writeXMLToStream(xml_stream);
        }

        // output all imagery components
        for(ImageryList::const_iterator image = d_images.begin(); image != d_images.end(); ++image)
        {
            (*image).writeXMLToStream(xml_stream);
        }

        // output all text components
        for(TextList::const_iterator text = d_texts.begin(); text != d_texts.end(); ++text)
        {
            (*text).writeXMLToStream(xml_stream);
        }

        // output closing tag
        xml_stream.closeTag();
    }

    bool ImagerySection::handleFontRenderSizeChange(Window& window,
                                                    const Font* font) const
    {
        bool result = false;

        for(TextList::const_iterator text = d_texts.begin();
            text != d_texts.end();
            ++text)
        {
            result |= (*text).handleFontRenderSizeChange(window, font);
        }

        return result;
    }

    ImagerySection::ImageryComponentIterator
    ImagerySection::getImageryComponentIterator() const
    {
        return ImageryComponentIterator(d_images.begin(), d_images.end());
    }
    ImagerySection::TextComponentIterator
    ImagerySection::getTextComponentIterator() const
    {
        return TextComponentIterator(d_texts.begin(), d_texts.end());
    }
    ImagerySection::FrameComponentIterator
    ImagerySection::getFrameComponentIterator() const
    {
        return FrameComponentIterator(d_frames.begin(), d_frames.end());
    }

    ImagerySection::ImageryComponentPointerList ImagerySection::getImageryComponentPointers()
    {
        ImagerySection::ImageryComponentPointerList pointerList;

        ImageryList::iterator imageryComponentIter = d_images.begin();
        ImageryList::iterator imageryComponentEnd = d_images.end();
        while( imageryComponentIter != imageryComponentEnd )
        {
            pointerList.push_back(&(*imageryComponentIter));
            ++imageryComponentIter;
        }

        return pointerList;
    }

    ImagerySection::TextComponentPointerList ImagerySection::getTextComponentPointers()
    {
        ImagerySection::TextComponentPointerList pointerList;

        TextList::iterator textComponentIter = d_texts.begin();
        TextList::iterator textComponentEnd = d_texts.end();
        while( textComponentIter != textComponentEnd )
        {
            pointerList.push_back(&(*textComponentIter));
            ++textComponentIter;
        }

        return pointerList;
    }

    ImagerySection::FrameComponentPointerList ImagerySection::getFrameComponentPointers()
    {
        ImagerySection::FrameComponentPointerList pointerList;

        FrameList::iterator frameComponentIter = d_frames.begin();
        FrameList::iterator frameComponentEnd = d_frames.end();
        while( frameComponentIter != frameComponentEnd )
        {
            pointerList.push_back(&(*frameComponentIter));
            ++frameComponentIter;
        }

        return pointerList;
    }

} // End of  CEGUI namespace section
