/************************************************************************
    filename:   FalEditbox.cpp
    created:    Sat Jun 25 2005
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
#include "FalEditbox.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardEditbox::WidgetTypeName[] = "Falagard/Editbox";

    FalagardEditbox::FalagardEditbox(const String& type, const String& name) :
        Editbox(type, name),
        d_lastTextOffset(0)
    {
    }

    FalagardEditbox::~FalagardEditbox()
    {
    }

    void FalagardEditbox::populateRenderCache()
    {
        const StateImagery* imagery;

        // draw container etc
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        // try and get imagery for the approprite state.
        imagery = &wlf.getStateImagery(isDisabled() ? "Disabled" : (isReadOnly() ? "ReadOnly" : "Enabled"));

        // peform the rendering operation for the container.
        imagery->render(*this);

        // get destination area for text
        const Rect textArea(wlf.getNamedArea("TextArea").getArea().getPixelRect(*this));

        //
        // Required preliminary work for text rendering operations
        //
        const Font* font = getFont();

        // no font == no more rendering
        if (!font)
            return;

        // This will point to the final string to be used for rendering.  Useful because it means we
        // do not have to have duplicate code or be copying d_text for handling masked/unmasked text.
        String* editText;

        // Create a 'masked' version of the string if needed.
        String maskedText;
        if (isTextMasked())
        {
            maskedText.insert(0, d_text.length(), getMaskCodePoint());
            editText = &maskedText;
        }
        // text not masked to editText will be the windows d_text String.
        else
        {
            editText = &d_text;
        }

        // calculate best position to render text to ensure carat is always visible
        float textOffset;
        float extentToCarat = font->getTextExtent(editText->substr(0, getCaratIndex()));

        // get carat imagery
        const ImagerySection& caratImagery = wlf.getImagerySection("Carat");
        // store carat width
        float caratWidth = caratImagery.getBoundingRect(*this, textArea).getWidth();

        // if box is inactive
        if (!hasInputFocus())
        {
            textOffset = d_lastTextOffset;
        }
        // if carat is to the left of the box
        else if ((d_lastTextOffset + extentToCarat) < 0)
        {
            textOffset = -extentToCarat;
        }
        // if carat is off to the right.
        else if ((d_lastTextOffset + extentToCarat) >= (textArea.getWidth() - caratWidth))
        {
            textOffset = textArea.getWidth() - extentToCarat - caratWidth;
        }
        // else carat is already within the box
        else
        {
            textOffset = d_lastTextOffset;
        }

        ColourRect colours;
        float alpha_comp = getEffectiveAlpha();

        //
        // Draw label text
        //
        // setup initial rect for text formatting
        Rect text_part_rect(textArea);
        // allow for scroll position
        text_part_rect.d_left += textOffset;
        // centre text vertically within the defined text area
        text_part_rect.d_top += (textArea.getHeight() - font->getLineSpacing()) * 0.5f;

        // draw pre-highlight text
        String sect = editText->substr(0, getSelectionStartIndex());
        colours.setColours(d_normalTextColour);
        colours.modulateAlpha(alpha_comp);
        d_renderCache.cacheText(sect, font, LeftAligned, text_part_rect, 0, colours, &textArea);

        // adjust rect for next section
        text_part_rect.d_left += font->getTextExtent(sect);

        // draw highlight text
        sect = editText->substr(getSelectionStartIndex(), getSelectionLength());
        colours.setColours(d_selectTextColour);
        colours.modulateAlpha(alpha_comp);
        d_renderCache.cacheText(sect, font, LeftAligned, text_part_rect, 0, colours, &textArea);

        // adjust rect for next section
        text_part_rect.d_left += font->getTextExtent(sect);

        // draw post-highlight text
        sect = editText->substr(getSelectionEndIndex());
        colours.setColours(d_normalTextColour);
        colours.modulateAlpha(alpha_comp);
        d_renderCache.cacheText(sect, font, LeftAligned, text_part_rect, 0, colours, &textArea);

        // remember this for next time.
        d_lastTextOffset = textOffset;

        // see if the editbox is active or inactive.
        bool active = (!isReadOnly()) && hasInputFocus();

        //
        // Render selection imagery.
        //
        if (getSelectionLength() != 0)
        {
            // calculate required start and end offsets of selection imagery.
            float selStartOffset = font->getTextExtent(editText->substr(0, getSelectionStartIndex()));
            float selEndOffset   = font->getTextExtent(editText->substr(0, getSelectionEndIndex()));

            // calculate area for selection imagery.
            Rect hlarea(textArea);
            hlarea.d_left += textOffset + selStartOffset;
            hlarea.d_right = hlarea.d_left + (selEndOffset - selStartOffset);

            // render the selection imagery.
            wlf.getStateImagery(active ? "ActiveSelection" : "InactiveSelection").render(*this, hlarea, 0, &textArea);
        }

        //
        // Render carat
        //
        if (active)
        {
            Rect caratRect(textArea);
            caratRect.d_left += extentToCarat + textOffset;

            caratImagery.render(*this, caratRect, 0, 0, &textArea);
        }
    }

    size_t FalagardEditbox::getTextIndexFromPosition(const Point& pt) const
    {
        //
        // calculate final window position to be checked
        //
        float wndx = screenToWindowX(pt.d_x);

        if (getMetricsMode() == Relative)
        {
            wndx = relativeToAbsoluteX(wndx);
        }

        wndx -= d_lastTextOffset;

        //
        // Return the proper index
        //
        if (isTextMasked())
        {
            return getFont()->getCharAtPixel(String(d_text.length(), getMaskCodePoint()), wndx);
        }
        else
        {
            return getFont()->getCharAtPixel(d_text, wndx);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardEditboxFactory::createWindow(const String& name)
    {
        return new FalagardEditbox(d_type, name);
    }

    void FalagardEditboxFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section
