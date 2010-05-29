/************************************************************************
    filename:   CEGUIFalTextComponent.cpp
    created:    Sun Jun 19 2005
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
#include "falagard/CEGUIFalTextComponent.h"
#include "falagard/CEGUIFalXMLEnumHelper.h"
#include "CEGUIFontManager.h"
#include "CEGUIExceptions.h"
#include "CEGUIPropertyHelper.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    TextComponent::TextComponent() :
        d_vertFormatting(VTF_TOP_ALIGNED),
        d_horzFormatting(HTF_LEFT_ALIGNED)
    {}

    const String& TextComponent::getText() const
    {
        return d_text;
    }

    void TextComponent::setText(const String& text)
    {
        d_text = text;
    }

    const String& TextComponent::getFont() const
    {
        return d_font;
    }

    void TextComponent::setFont(const String& font)
    {
        d_font = font;
    }

    VerticalTextFormatting TextComponent::getVerticalFormatting() const
    {
        return d_vertFormatting;
    }

    void TextComponent::setVerticalFormatting(VerticalTextFormatting fmt)
    {
        d_vertFormatting = fmt;
    }

    HorizontalTextFormatting TextComponent::getHorizontalFormatting() const
    {
        return d_horzFormatting;
    }

    void TextComponent::setHorizontalFormatting(HorizontalTextFormatting fmt)
    {
        d_horzFormatting = fmt;
    }

    void TextComponent::render_impl(Window& srcWindow, Rect& destRect, float base_z, const CEGUI::ColourRect* modColours, const Rect* clipper, bool clipToDisplay) const
    {
        // get font to use
        const Font* font;

        try
        {
            font = d_font.empty() ? srcWindow.getFont() : FontManager::getSingleton().getFont(d_font);
        }
        catch (UnknownObjectException)
        {
            font = 0;
        }

        // exit if we have no font to use.
        if (!font)
            return;

        HorizontalTextFormatting horzFormatting = d_horzFormatPropertyName.empty() ? d_horzFormatting :
            FalagardXMLHelper::stringToHorzTextFormat(srcWindow.getProperty(d_horzFormatPropertyName));

        VerticalTextFormatting vertFormatting = d_vertFormatPropertyName.empty() ? d_vertFormatting :
            FalagardXMLHelper::stringToVertTextFormat(srcWindow.getProperty(d_vertFormatPropertyName));

        // calculate final colours to be used
        ColourRect finalColours;
        initColoursRect(srcWindow, modColours, finalColours);

        // decide which string to render.
        const String& renderString = d_text.empty() ? srcWindow.getText() : d_text;

        // calculate height of formatted text
        float textHeight = font->getFormattedLineCount(renderString, destRect, (TextFormatting)horzFormatting) * font->getLineSpacing();

        // handle dest area adjustments for vertical formatting.
        switch(vertFormatting)
        {
        case VTF_CENTRE_ALIGNED:
            destRect.d_top += (destRect.getHeight() - textHeight) * 0.5f;
            break;

        case VTF_BOTTOM_ALIGNED:
            destRect.d_top = destRect.d_bottom - textHeight;
            break;

        default:
            // default is VTF_TOP_ALIGNED, for which we take no action.
            break;
        }

        // add text to the rendering cache for the target window.
        srcWindow.getRenderCache().cacheText(renderString, font, (TextFormatting)horzFormatting, destRect, base_z, finalColours, clipper, clipToDisplay);
    }

    void TextComponent::writeXMLToStream(OutStream& out_stream) const
    {
        // opening tag
        out_stream << "<TextComponent>" << std::endl;
        // write out area
        d_area.writeXMLToStream(out_stream);

        // write text element
        out_stream << "<Text font=\"" << d_font << "\" string=\"" << d_text << "\" />" << std::endl;

        // get base class to write colours
        writeColoursXML(out_stream);

        // write vert format, allowing base class to do this for us if a propety is in use
        if (!writeVertFormatXML(out_stream))
        {
            // was not a property, so write out explicit formatting in use
            out_stream << "<VertFormat type=\"" << FalagardXMLHelper::vertTextFormatToString(d_vertFormatting) << "\" />" << std::endl;
        }

        // write horz format, allowing base class to do this for us if a propety is in use
        if (!writeHorzFormatXML(out_stream))
        {
            // was not a property, so write out explicit formatting in use
            out_stream << "<HorzFormat type=\"" << FalagardXMLHelper::horzTextFormatToString(d_horzFormatting) << "\" />" << std::endl;
        }

        // closing tag
        out_stream << "</TextComponent>" << std::endl;
    }

} // End of  CEGUI namespace section
