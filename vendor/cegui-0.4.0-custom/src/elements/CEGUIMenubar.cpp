/************************************************************************
	filename: 	CEGUIMenubar.cpp
	created:	31/3/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Implementation of Menubar widget base class
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
#include "elements/CEGUIMenubar.h"
#include "elements/CEGUIPopupMenu.h"
#include "elements/CEGUIMenuItem.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Constants
*************************************************************************/
const String Menubar::EventNamespace("Menubar");

/*************************************************************************
	Constructor for Menubar base class.
*************************************************************************/
Menubar::Menubar(const String& type, const String& name)
	: MenuBase(type, name)
{
	d_itemSpacing = 10;
	d_horzPadding = d_vertPadding = 3;
	d_borderWidth = 5;
}


/*************************************************************************
	Destructor for Menubar base class.
*************************************************************************/
Menubar::~Menubar(void)
{
}


/*************************************************************************
	Sets up sizes and positions for attached ItemEntry children.
*************************************************************************/
void Menubar::layoutItemWidgets()
{
	Rect render_rect = getItemRenderArea();
	float x0 = PixelAligned(render_rect.d_left+d_borderWidth);
	
	Rect rect;

	ItemEntryList::iterator item = d_listItems.begin();
	while ( item != d_listItems.end() )
	{
		Size optimal = (*item)->getItemPixelSize();
		optimal.d_width += 2*d_horzPadding;
		optimal.d_height += 2*d_vertPadding;

		(*item)->setVerticalAlignment(VA_CENTRE);
		rect.setPosition( Point(x0,0) );
		rect.setSize( Size( PixelAligned(optimal.d_width), PixelAligned(optimal.d_height) ) );

		(*item)->setRect(Absolute,rect);
		
		x0 += optimal.d_width + d_itemSpacing;

		item++;
	}

}


/*************************************************************************
	Returns the "optimal" size for the content in unclipped pixels
*************************************************************************/
Size Menubar::getContentSize()
{
	// find the content sizes
	float tallest = 0;
	float total_width = 0;
	
	size_t count = 0;
	size_t max = d_listItems.size();
	while (count < max)
	{
		const Size sz = d_listItems[count]->getItemPixelSize();
		if (sz.d_height > tallest)
			tallest = sz.d_height;
		total_width += sz.d_width;

		count++;
	}
	
	const float dbl_border = d_borderWidth+d_borderWidth;

	// add horz padding
	total_width += 2.0f*count*d_horzPadding;
	// spacing
	total_width += (count-1)*d_itemSpacing;
	// border
	total_width += dbl_border;

	// add vert padding
	tallest += d_vertPadding+d_vertPadding;
	// border
	tallest += dbl_border;

	// return the content size
	return Size( total_width, tallest);
}

} // End of  CEGUI namespace section
