/************************************************************************
	filename: 	CEGUIPopupMenu.cpp
	created:	1/4/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Implementation of PopupMenu widget base class
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
#include "elements/CEGUIPopupMenu.h"
#include "elements/CEGUIMenuItem.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Definition of Properties for this class
*************************************************************************/
PopupMenuProperties::FadeInTime		PopupMenu::d_fadeInTimeProperty;
PopupMenuProperties::FadeOutTime	PopupMenu::d_fadeOutTimeProperty;


/*************************************************************************
	Constants
*************************************************************************/
const String PopupMenu::EventNamespace("PopupMenu");


/*************************************************************************
	Constructor for PopupMenu base class.
*************************************************************************/
PopupMenu::PopupMenu(const String& type, const String& name)
	: MenuBase(type, name),
	d_origAlpha(d_alpha),
	d_fadeOutTime(0),
	d_fadeInTime(0),
	d_fading(false),
	d_fadingOut(false)
{
	d_itemSpacing = 2;
	d_horzPadding = 10;
	d_vertPadding = 2;
	d_borderWidth = 0;

	addPopupMenuProperties();

	// enable auto resizing
	d_autoResize = true;

	// disable parent clipping
	setClippedByParent(false);

	// hide by default
	hide();
}


/*************************************************************************
	Destructor for PopupMenu base class.
*************************************************************************/
PopupMenu::~PopupMenu(void)
{
}


/*************************************************************************
	Tells the popup menu to open.
*************************************************************************/
void PopupMenu::openPopupMenu(void)
{
	// are we fading?
	if (d_fading)
	{
		// fading in!
		if (!d_fadingOut)
		{
			// dont want to restart fade in!
			return;
		}
		// fading out!
		else
		{
			// make sure the "fade back in" is smooth - if possible !
			if (d_fadeInTime>0.0f&&d_fadeOutTime>0.0f)
			{
				// jump to the point of the fade in that has the same alpha as right now - this keeps it smooth
				d_fadeElapsed = ((d_fadeOutTime-d_fadeElapsed)/d_fadeOutTime)*d_fadeInTime;
			}
			else
			{
				// start the fade in from the beginning
				d_fadeElapsed = 0;
			}
			// change to fade in
			d_fadingOut=false;
		}

	}
	// start normal fade in!
	else if (d_fadeInTime>0.0f)
	{
		d_fading = true;
		d_fadingOut=false;
		setAlpha(0.0f);
		d_fadeElapsed = 0;
	}
	// should not fade!
	else
	{
		d_fading = false;
		setAlpha(d_origAlpha);
	}
	
	show();
	moveToFront();
}


/*************************************************************************
	Tells the popup menu to close.
*************************************************************************/
void PopupMenu::closePopupMenu(void)
{
	// are we fading?
	if (d_fading)
	{
		// fading out!
		if (d_fadingOut)
		{
			// dont want to restart fade out!
			return;
		}
		// fading in!
		else
		{
			// make sure the "fade back out" is smooth - if possible !
			if (d_fadeInTime>0.0f&&d_fadeOutTime>0.0f)
			{
				// jump to the point of the fade in that has the same alpha as right now - this keeps it smooth
				d_fadeElapsed = ((d_fadeInTime-d_fadeElapsed)/d_fadeInTime)*d_fadeOutTime;
			}
			else
			{
				// start the fade in from the beginning
				d_fadeElapsed = 0;
			}
			// change to fade in
			d_fadingOut=true;
		}

	}
	// start normal fade out!
	else if (d_fadeOutTime>0.0f)
	{
		d_fading = true;
		d_fadingOut=true;
		setAlpha(d_origAlpha);
		d_fadeElapsed = 0;
	}
	// should not fade!
	else
	{
		d_fading = false;
		hide();
	}

}


/*************************************************************************
    Perform actual update processing for this Window.
*************************************************************************/
void PopupMenu::updateSelf(float elapsed)
{
    ItemListBase::updateSelf(elapsed);
    
    // handle fading
    if (d_fading)
    {
		d_fadeElapsed+=elapsed;

		// fading out
		if (d_fadingOut)
		{
			if (d_fadeElapsed>=d_fadeOutTime)
			{
				hide();
				d_fading=false;
			}
			else
			{
				setAlpha(d_origAlpha*(d_fadeOutTime-d_fadeElapsed)/d_fadeOutTime);
			}

		}

		// fading in
		else
		{
			if (d_fadeElapsed>=d_fadeInTime)
			{
				d_fading=false;
				setAlpha(d_origAlpha);
			}
			else
			{
				setAlpha(d_origAlpha*d_fadeElapsed/d_fadeInTime);
			}

		}

	}

}


/*************************************************************************
	Sets up sizes and positions for attached ItemEntry children.
*************************************************************************/
void PopupMenu::layoutItemWidgets()
{
	// get render area
	Rect render_rect = getItemRenderArea();

	// get starting position
	const float x0 = render_rect.d_left+d_borderWidth;
	float y0 = render_rect.d_top+d_borderWidth;

	Rect rect;
	Size sz(render_rect.getWidth()-d_borderWidth-d_borderWidth,0); // set item width

	// iterate through all items attached to this window
	ItemEntryList::iterator item = d_listItems.begin();
	while ( item != d_listItems.end() )
	{
		// get the "optimal" height of the item and use that!
		sz.d_height = (*item)->getItemPixelSize().d_height; // fix rounding errors
		sz.d_height += d_vertPadding+d_vertPadding;

		// set destination rect
		rect.setPosition( Point(x0, y0) );
		rect.setSize( sz );
		(*item)->setRect(Absolute,rect);

		// next position
		y0 += sz.d_height + d_itemSpacing;

		item++; // next item
	}
}


/*************************************************************************
	Returns the "optimal" size for the content in unclipped pixels
*************************************************************************/
Size PopupMenu::getContentSize()
{
	// find the content sizes
	float widest = 0;
	float total_height = 0;
	
	size_t count = 0;
	size_t max = d_listItems.size();
	while (count < max)
	{
		const Size sz = d_listItems[count]->getItemPixelSize();
		if (sz.d_width > widest)
			widest = sz.d_width;
		total_height += sz.d_height;

		count++;
	}
	
	const float dbl_border = d_borderWidth+d_borderWidth;

	// add vert padding
	total_height += 2.0f*count*d_vertPadding;
	// spacing
	total_height += (count-1)*d_itemSpacing;
	// border
	total_height += dbl_border;

	// add horz padding
	widest += d_horzPadding+d_horzPadding;
	// border
	widest += dbl_border;

	// return the content size
	return Size( widest, total_height );
}


/*************************************************************************
	Handler for when window alpha changes
*************************************************************************/
void PopupMenu::onAlphaChanged(WindowEventArgs& e)
{
	ItemListBase::onAlphaChanged(e);
	
	// if we are not fading, this is a real alpha change request and we save a copy of the value
	if (!d_fading)
	{
		d_origAlpha = d_alpha;
	}
}


/*************************************************************************
	Add PopupMenu specific properties
*************************************************************************/
void PopupMenu::addPopupMenuProperties( bool bCommon )
{
	if ( bCommon == false )
    {
        addProperty(&d_fadeInTimeProperty);
        addProperty(&d_fadeOutTimeProperty);
    }
}

} // End of  CEGUI namespace section
