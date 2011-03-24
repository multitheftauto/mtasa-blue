/************************************************************************
	filename: 	CEGUIListboxItem.cpp
	created:	12/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of base class for list items
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
#include "elements/CEGUIListboxItem.h"
#include "CEGUISystem.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
const colour	ListboxItem::DefaultSelectionColour	= 0xFF607FFF;

/*************************************************************************
	Base class constructor
*************************************************************************/
ListboxItem::ListboxItem(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete) :
	d_itemText(text),
	d_itemID(item_id),
	d_itemData(item_data),
    d_selected(false),
	d_disabled(disabled),
    d_autoDelete(auto_delete),
	d_owner(NULL),
    d_selectCols(DefaultSelectionColour, DefaultSelectionColour, DefaultSelectionColour, DefaultSelectionColour),
	d_selectBrush(NULL),
    d_bSizeChanged(true),
    d_savedPixelSize(10,10)
{
}


void ListboxItem::setText(const String& text, const char* sortText)
{
    d_bSizeChanged |= (d_itemText != text);
    d_itemText = text;
    if ( sortText )
        d_itemSortText.assign ( sortText );
    else
        d_itemSortText.clear ();
}


/*************************************************************************
	Set the selection highlighting brush image.
*************************************************************************/
void ListboxItem::setSelectionBrushImage(const String& imageset, const String& image)
{
	setSelectionBrushImage(&ImagesetManager::getSingleton().getImageset(imageset)->getImage(image));
}


/*************************************************************************
	Return a ColourRect object describing the colours in 'cols' after
	having their alpha component modulated by the value 'alpha'.
*************************************************************************/
ColourRect ListboxItem::getModulateAlphaColourRect(const ColourRect& cols, float alpha) const
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
colour ListboxItem::calculateModulatedAlphaColour(colour col, float alpha) const
{
	colour temp(col);
	temp.setAlpha(temp.getAlpha() * alpha);
	return temp;
}


/*************************************************************************
	Set the colours used for selection highlighting.	
*************************************************************************/
void ListboxItem::setSelectionColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour)
{
	d_selectCols.d_top_left		= top_left_colour;
	d_selectCols.d_top_right	= top_right_colour;
	d_selectCols.d_bottom_left	= bottom_left_colour;
	d_selectCols.d_bottom_right	= bottom_right_colour;
}

} // End of  CEGUI namespace section
