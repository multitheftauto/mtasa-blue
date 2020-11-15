/***********************************************************************
	created:	12/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of List box text items
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
#include "CEGUI/widgets/ListboxTextItem.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/Font.h"
#include "CEGUI/Window.h"
#include "CEGUI/Image.h"
#include "CEGUI/CoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
BasicRenderedStringParser ListboxTextItem::d_stringParser;
DefaultRenderedStringParser ListboxTextItem::d_noTagsStringParser;

/*************************************************************************
	Constants
*************************************************************************/
const Colour	ListboxTextItem::DefaultTextColour		= 0xFFFFFFFF;


/*************************************************************************
	Constructor
*************************************************************************/
ListboxTextItem::ListboxTextItem(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete) :
	ListboxItem(text, item_id, item_data, disabled, auto_delete),
	d_textCols(DefaultTextColour, DefaultTextColour, DefaultTextColour, DefaultTextColour),
	d_font(0),
    d_renderedStringValid(false),
    d_textParsingEnabled(true)
{
}


/*************************************************************************
	Return a pointer to the font being used by this ListboxTextItem
*************************************************************************/
const Font* ListboxTextItem::getFont(void) const
{
	// prefer out own font
	if (d_font)
	{
		return d_font;
	}
	// try our owner window's font setting (may be null if owner uses no existant default font)
	else if (d_owner)
	{
		return d_owner->getFont();
	}
    // no owner, so the default font is ambiguous (it could be of any context)
    else
        return 0;  
}


/*************************************************************************
	Set the font to be used by this ListboxTextItem
*************************************************************************/
void ListboxTextItem::setFont(const String& font_name)
{
	setFont(&FontManager::getSingleton().get(font_name));
}

//----------------------------------------------------------------------------//
void ListboxTextItem::setFont(Font* font)
{
    d_font = font;

    d_renderedStringValid = false;
}


/*************************************************************************
	Return the rendered pixel size of this list box item.
*************************************************************************/
Sizef ListboxTextItem::getPixelSize(void) const
{
    const Font* fnt = getFont();

    if (!fnt)
        return Sizef(0, 0);

    if (!d_renderedStringValid)
        parseTextString();

    Sizef sz(0.0f, 0.0f);

    for (size_t i = 0; i < d_renderedString.getLineCount(); ++i)
    {
        const Sizef line_sz(d_renderedString.getPixelSize(d_owner, i));
        sz.d_height += line_sz.d_height;

        if (line_sz.d_width > sz.d_width)
            sz.d_width = line_sz.d_width;
    }

    return sz;
}


/*************************************************************************
	Draw the list box item in its current state.
*************************************************************************/
void ListboxTextItem::draw(GeometryBuffer& buffer, const Rectf& targetRect,
                           float alpha, const Rectf* clipper) const
{
    if (d_selected && d_selectBrush != 0)
        d_selectBrush->render(buffer, targetRect, clipper,
                            getModulateAlphaColourRect(d_selectCols, alpha));

    const Font* font = getFont();

    if (!font)
        return;

    Vector2f draw_pos(targetRect.getPosition());

    draw_pos.d_y += CoordConverter::alignToPixels(
        (font->getLineSpacing() - font->getFontHeight()) * 0.5f);

    if (!d_renderedStringValid)
        parseTextString();

    const ColourRect final_colours(
        getModulateAlphaColourRect(ColourRect(0xFFFFFFFF), alpha));

    for (size_t i = 0; i < d_renderedString.getLineCount(); ++i)
    {
        d_renderedString.draw(d_owner, i, buffer, draw_pos, &final_colours, clipper, 0.0f);
        draw_pos.d_y += d_renderedString.getPixelSize(d_owner, i).d_height;
    }
}


/*************************************************************************
	Set the colours used for text rendering.	
*************************************************************************/
void ListboxTextItem::setTextColours(Colour top_left_colour,
                                     Colour top_right_colour,
                                     Colour bottom_left_colour,
                                     Colour bottom_right_colour)
{
	d_textCols.d_top_left		= top_left_colour;
	d_textCols.d_top_right		= top_right_colour;
	d_textCols.d_bottom_left	= bottom_left_colour;
	d_textCols.d_bottom_right	= bottom_right_colour;

    d_renderedStringValid = false;
}

//----------------------------------------------------------------------------//
void ListboxTextItem::setText(const String& text)
{
    ListboxItem::setText(text);

    d_renderedStringValid = false;
}

//----------------------------------------------------------------------------//
void ListboxTextItem::parseTextString() const
{
    if (d_textParsingEnabled)
        d_renderedString =
            d_stringParser.parse(getTextVisual(), 0, &d_textCols);
    else
        d_renderedString =
            d_noTagsStringParser.parse(getTextVisual(), 0, &d_textCols);

    d_renderedStringValid = true;
}

//----------------------------------------------------------------------------//
void ListboxTextItem::setTextParsingEnabled(const bool enable)
{
    d_textParsingEnabled = enable;
    d_renderedStringValid = false;
}

//----------------------------------------------------------------------------//
bool ListboxTextItem::isTextParsingEnabled() const
{
    return d_textParsingEnabled;
}

//----------------------------------------------------------------------------//
bool ListboxTextItem::handleFontRenderSizeChange(const Font* const font)
{
    return getFont() == font;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
