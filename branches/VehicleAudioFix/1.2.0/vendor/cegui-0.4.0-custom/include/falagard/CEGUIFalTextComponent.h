/************************************************************************
    filename:   CEGUIFalTextComponent.h
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
#ifndef _CEGUIFalTextComponent_h_
#define _CEGUIFalTextComponent_h_

#include "falagard/CEGUIFalComponentBase.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Class that encapsulates information for a text component.
    */
    class CEGUIEXPORT TextComponent : public FalagardComponentBase
    {
    public:
        /*!
        \brief
            Constructor
        */
        TextComponent();

        /*!
        \brief
            Return the text object that will be rendered by this TextComponent.

        \return
            String object containing the text that will be rendered.
        */
        const String& getText() const;

        /*!
        \brief
            Set the text that will be rendered by this TextComponent.

            Note that setting this to the empty string ("") will cause the text from the
            base window passed when rendering to be used instead.

        \param text
            String containing text to render, or "" to render text from window.

        \return
            Nothing.
        */
        void setText(const String& text);

        /*!
        \brief
            Return the name of the font to be used when rendering this TextComponent.

        \return
            String object containing the name of a font, or "" if the window font is to be used.
        */
        const String& getFont() const;

        /*!
        \brief
            Set the name of the font to be used when rendering this TextComponent.

            Note that setting this to the empty string ("") will cause the font from the
            base window passed when rendering to be used instead.

        \param font
            String containing name of a font

        \return
            Nothing.
        */
        void setFont(const String& font);

        /*!
        \brief
            Return the current vertical formatting setting for this TextComponent.

        \return
            One of the VerticalTextFormatting enumerated values.
        */
        VerticalTextFormatting getVerticalFormatting() const;

        /*!
        \brief
            Set the vertical formatting setting for this TextComponent.

        \param fmt
            One of the VerticalTextFormatting enumerated values.

        \return
            Nothing.
        */
        void setVerticalFormatting(VerticalTextFormatting fmt);

        /*!
        \brief
            Return the current horizontal formatting setting for this TextComponent.

        \return
            One of the HorizontalTextFormatting enumerated values.
        */
        HorizontalTextFormatting getHorizontalFormatting() const;

        /*!
        \brief
            Set the horizontal formatting setting for this TextComponent.

        \param fmt
            One of the HorizontalTextFormatting enumerated values.

        \return
            Nothing.
        */
        void setHorizontalFormatting(HorizontalTextFormatting fmt);

        /*!
        \brief
            Writes an xml representation of this TextComponent to \a out_stream.

        \param out_stream
            Stream where xml data should be output.

        \return
            Nothing.
        */
        void writeXMLToStream(OutStream& out_stream) const;

    protected:
        // implemets abstract from base
        void render_impl(Window& srcWindow, Rect& destRect, float base_z, const CEGUI::ColourRect* modColours, const Rect* clipper, bool clipToDisplay) const;

    private:
        String               d_text;            //!< text rendered by this component.
        String               d_font;            //!< name of font to use.
        VerticalTextFormatting   d_vertFormatting;  //!< Vertical formatting to be applied when rendering the component.
        HorizontalTextFormatting d_horzFormatting;  //!< Horizontal formatting to be applied when rendering the component.
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalTextComponent_h_
