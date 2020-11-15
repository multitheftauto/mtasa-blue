/***********************************************************************
     created:	5-13-07
     author:		Jonathan Welch (Based on Code by David Durant)
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
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUI/widgets/Tree.h"
#include "CEGUI/widgets/TreeItem.h"
#include "CEGUI/System.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/Image.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/Font.h"
#include "CEGUI/Window.h"
#include <algorithm>

#if defined (CEGUI_USE_FRIBIDI)
    #include "CEGUI/FribidiVisualMapping.h"
#elif defined (CEGUI_USE_MINIBIDI)
    #include "CEGUI/MinibidiVisualMapping.h"
#else
    #include "CEGUI/BidiVisualMapping.h"
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
BasicRenderedStringParser TreeItem::d_stringParser;

/*************************************************************************
    Constants
*************************************************************************/
const Colour TreeItem::DefaultSelectionColour = 0xFF4444AA;
const Colour TreeItem::DefaultTextColour = 0xFFFFFFFF;

/*************************************************************************
    Base class constructor
*************************************************************************/
TreeItem::TreeItem(const String& text, uint item_id, void* item_data,
                   bool disabled, bool auto_delete) :
#ifndef CEGUI_BIDI_SUPPORT
    d_bidiVisualMapping(0),
#elif defined (CEGUI_USE_FRIBIDI)
    d_bidiVisualMapping(CEGUI_NEW_AO FribidiVisualMapping),
#elif defined (CEGUI_USE_MINIBIDI)
    d_bidiVisualMapping(CEGUI_NEW_AO MinibidiVisualMapping),
#else
    #error "BIDI Configuration is inconsistant, check your config!"
#endif
    d_bidiDataValid(false),
    d_itemID(item_id),
    d_itemData(item_data),
    d_selected(false),
    d_disabled(disabled),
    d_autoDelete(auto_delete),
    d_buttonLocation(Rectf(0, 0, 0, 0)),
    d_owner(0),
    d_selectCols(DefaultSelectionColour, DefaultSelectionColour,
                 DefaultSelectionColour, DefaultSelectionColour),
    d_selectBrush(0),
    d_textCols(DefaultTextColour, DefaultTextColour,
               DefaultTextColour, DefaultTextColour),
    d_font(0),
    d_iconImage(0),
    d_isOpen(false),
    d_renderedStringValid(false)
{
    setText(text);
}

//----------------------------------------------------------------------------//
TreeItem::~TreeItem(void)
{
    // delete any items we are supposed to
    for (size_t i = 0; i < getItemCount(); ++i)
    {
        // if item is supposed to be deleted by us
        if (d_listItems[i]->isAutoDeleted())
        {
            // clean up this item.
            CEGUI_DELETE_AO d_listItems[i];
        }
    }

    CEGUI_DELETE_AO d_bidiVisualMapping;
}

/*************************************************************************
    Set the selection highlighting brush image.
*************************************************************************/
void TreeItem::setSelectionBrushImage(const String& name)
{
    setSelectionBrushImage(
        &ImageManager::getSingleton().get(name));
}

/*************************************************************************
    Return a ColourRect object describing the colours in 'cols' after
    having their alpha component modulated by the value 'alpha'.
*************************************************************************/
ColourRect TreeItem::getModulateAlphaColourRect(const ColourRect& cols,
                                                float alpha) const
{
    return ColourRect
    (
     calculateModulatedAlphaColour(cols.d_top_left, alpha),
     calculateModulatedAlphaColour(cols.d_top_right, alpha),
     calculateModulatedAlphaColour(cols.d_bottom_left, alpha),
     calculateModulatedAlphaColour(cols.d_bottom_right, alpha)
     );
}

/*************************************************************************
    Return a colour value describing the colour specified by 'col' after
    having its alpha component modulated by the value 'alpha'.
*************************************************************************/
Colour TreeItem::calculateModulatedAlphaColour(Colour col, float alpha) const
{
    Colour temp(col);
    temp.setAlpha(temp.getAlpha() * alpha);
    return temp;
}

/*************************************************************************
    Return a pointer to the font being used by this TreeItem
*************************************************************************/
const Font* TreeItem::getFont(void) const
{
    // prefer out own font
    if (d_font != 0)
        return d_font;
    // try our owner window's font setting
    // (may be null if owner uses non existant default font)
    else if (d_owner != 0)
        return d_owner->getFont();
    // no owner, so the default font is ambiguous (it could be of any context)
    else
        return 0;  
}

/*************************************************************************
    Set the font to be used by this TreeItem
*************************************************************************/
void TreeItem::setFont(const String& font_name)
{
    setFont(&FontManager::getSingleton().get(font_name));
}

//----------------------------------------------------------------------------//
void TreeItem::setFont(const Font* font)
{
    d_font = font;

    d_renderedStringValid = false;
}

//----------------------------------------------------------------------------//

/*************************************************************************
    Return the rendered pixel size of this tree item.
*************************************************************************/
Sizef TreeItem::getPixelSize(void) const
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
    Add the given TreeItem to this item's list.
*************************************************************************/
void TreeItem::addItem(TreeItem* item)
{
    if (item != 0)
    {
        Tree* parentWindow = (Tree*)getOwnerWindow();
        
        // establish ownership
        item->setOwnerWindow(parentWindow);
        
        // if sorting is enabled, re-sort the tree
        if (parentWindow->isSortEnabled())
        {
            d_listItems.insert(
                std::upper_bound(d_listItems.begin(),
                                 d_listItems.end(), item, &lbi_less),
                item);
        }
        // not sorted, just stick it on the end.
        else
        {
            d_listItems.push_back(item);
        }
        
        WindowEventArgs args(parentWindow);
        parentWindow->onListContentsChanged(args);
    }
}

/*************************************************************************
    Remove the given TreeItem from this item's list.
*************************************************************************/
void TreeItem::removeItem(const TreeItem* item) 
{ 
    if (item)
    {
        Tree* parentWindow = (Tree*)getOwnerWindow();

        LBItemList::iterator pos = std::find(d_listItems.begin(),
                                             d_listItems.end(),
                                             item);
        if (pos != d_listItems.end())
        {
            (*pos)->setOwnerWindow(0);
            d_listItems.erase(pos);

            if (item == parentWindow->d_lastSelected)
                parentWindow->d_lastSelected = 0;

            if (item->isAutoDeleted())
                CEGUI_DELETE_AO item;

            WindowEventArgs args(parentWindow);
            parentWindow->onListContentsChanged(args);
        }
    }
}

TreeItem *TreeItem::getTreeItemFromIndex(size_t itemIndex)
{
    if (itemIndex > d_listItems.size())
        return 0;
    
    return d_listItems[itemIndex];
}

/*************************************************************************
    Draw the tree item in its current state.
*************************************************************************/
void TreeItem::draw(GeometryBuffer& buffer, const Rectf& targetRect,
                    float alpha, const Rectf* clipper) const
{
    Rectf finalRect(targetRect);

    if (d_iconImage != 0)
    {
        Rectf finalPos(finalRect);
        finalPos.setWidth(targetRect.getHeight());
        finalPos.setHeight(targetRect.getHeight());
        d_iconImage->render(buffer, finalPos, clipper,
                          ColourRect(Colour(1,1,1,alpha)));
        finalRect.d_min.d_x += targetRect.getHeight();
    }

    if (d_selected && d_selectBrush != 0)
        d_selectBrush->render(buffer, finalRect, clipper,
                            getModulateAlphaColourRect(d_selectCols, alpha));

    const Font* font = getFont();

    if (!font)
        return;

    Vector2f draw_pos(finalRect.getPosition());
    draw_pos.d_y -= (font->getLineSpacing() - font->getBaseline()) * 0.5f;

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
    Set the colours used for selection highlighting.
*************************************************************************/
void TreeItem::setSelectionColours(Colour top_left_colour,
                                   Colour top_right_colour,
                                   Colour bottom_left_colour,
                                   Colour bottom_right_colour)
{
    d_selectCols.d_top_left		= top_left_colour;
    d_selectCols.d_top_right	= top_right_colour;
    d_selectCols.d_bottom_left	= bottom_left_colour;
    d_selectCols.d_bottom_right	= bottom_right_colour;
}

/*************************************************************************
    Set the colours used for text rendering.
*************************************************************************/
void TreeItem::setTextColours(Colour top_left_colour,
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
void TreeItem::setText( const String& text )
{
    d_textLogical = text;
    d_bidiDataValid = false;
    d_renderedStringValid = false;
}

//----------------------------------------------------------------------------//
void TreeItem::parseTextString() const
{
    d_renderedString =
        d_stringParser.parse(getTextVisual(), 0, &d_textCols);
    d_renderedStringValid = true;
}

//----------------------------------------------------------------------------//
const String& TreeItem::getTextVisual() const
{
    // no bidi support
    if (!d_bidiVisualMapping)
        return d_textLogical;

    if (!d_bidiDataValid)
    {
        d_bidiVisualMapping->updateVisual(d_textLogical);
        d_bidiDataValid = true;
    }

    return d_bidiVisualMapping->getTextVisual();
}

//----------------------------------------------------------------------------//
bool TreeItem::handleFontRenderSizeChange(const Font* const font)
{
    if (getFont() == font)
        return true;

    for (size_t i = 0; i < getItemCount(); ++i)
    {
        if (d_listItems[i]->handleFontRenderSizeChange(font))
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
