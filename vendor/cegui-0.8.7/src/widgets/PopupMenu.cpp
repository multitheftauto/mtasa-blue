/***********************************************************************
	created:	1/4/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Implementation of PopupMenu widget base class
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
#include "CEGUI/widgets/PopupMenu.h"
#include "CEGUI/widgets/MenuItem.h"
#include "CEGUI/CoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Constants
*************************************************************************/
const String PopupMenu::WidgetTypeName("CEGUI/PopupMenu");
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
	d_fadingOut(false),
	d_isOpen(false)
{
	d_itemSpacing = 2;

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
void PopupMenu::openPopupMenu(bool notify)
{
    // already open and not fading, or fading in?
    if (d_isOpen && (!d_fading || !d_fadingOut))
    {
        // then don't do anything
        return;
    }

    // should we let the parent menu item initiate the open?
    Window* parent = getParent();
    if (notify && parent && dynamic_cast<MenuItem*>(parent))
    {
        static_cast<MenuItem*>(parent)->openPopupMenu();
        return; // the rest will be handled when MenuItem calls us itself
    }

    // we'll handle it ourselves then.
	// are we fading, and fading out?
	if (d_fading && d_fadingOut)
	{
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
	// otherwise just start normal fade in!
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
void PopupMenu::closePopupMenu(bool notify)
{
    // already closed?
    if (!d_isOpen)
    {
        // then do nothing
        return;
    }

    // should we let the parent menu item close initiate the close?
    Window* parent = getParent();
    if (notify && parent && dynamic_cast<MenuItem*>(parent))
    {
        static_cast<MenuItem*>(parent)->closePopupMenu();
        return; // the rest will be handled when MenuItem calls us itself
    }

    // we'll do it our selves then.
	// are we fading, and fading in?
	if (d_fading && !d_fadingOut)
	{
		// make sure the "fade back out" is smooth - if possible !
		if (d_fadeOutTime>0.0f&&d_fadeInTime>0.0f)
		{
			// jump to the point of the fade in that has the same alpha as right now - this keeps it smooth
			d_fadeElapsed = ((d_fadeInTime-d_fadeElapsed)/d_fadeInTime)*d_fadeOutTime;
		}
		else
		{
			// start the fade in from the beginning
			d_fadeElapsed = 0;
		}
		// change to fade out
		d_fadingOut=true;
	}
	// otherwise just start normal fade out!
	else if (d_fadeOutTime>0.0f)
	{
	    d_fading = true;
	    d_fadingOut = true;
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
    MenuBase::updateSelf(elapsed);

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
				setAlpha(d_origAlpha); // set real alpha so users can show directly without having to restore it
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
	Rectf render_rect = getItemRenderArea();

	// get starting position
	const float x0 = CoordConverter::alignToPixels(render_rect.d_min.d_x);
	float y0 = CoordConverter::alignToPixels(render_rect.d_min.d_y);

	URect rect;
	UVector2 sz(cegui_absdim(CoordConverter::alignToPixels(render_rect.getWidth())), cegui_absdim(0)); // set item width

	// iterate through all items attached to this window
	ItemEntryList::iterator item = d_listItems.begin();
	while ( item != d_listItems.end() )
	{
		// get the "optimal" height of the item and use that!
		sz.d_y.d_offset = CoordConverter::alignToPixels((*item)->getItemPixelSize().d_height); // rounding errors ?

		// set destination rect
		rect.setPosition(UVector2(cegui_absdim(x0), cegui_absdim(y0)) );
        // todo: vector vs size
		rect.setSize(USize(sz.d_x, sz.d_y));
		(*item)->setArea(rect);

		// next position
		y0 += CoordConverter::alignToPixels(sz.d_y.d_offset + d_itemSpacing);

		item++; // next item
	}
}


/*************************************************************************
	Returns the "optimal" size for the content in unclipped pixels
*************************************************************************/
Sizef PopupMenu::getContentSize() const
{
	// find the content sizes
	float widest = 0;
	float total_height = 0;
	
	size_t i = 0;
	size_t max = d_listItems.size();
	while (i < max)
	{
		const Sizef sz = d_listItems[i]->getItemPixelSize();
		if (sz.d_width > widest)
			widest = sz.d_width;
		total_height += sz.d_height;

		i++;
	}
	
	const float count = float(i);

	// vert item spacing
	if (count >= 2)
	{
	    total_height += (count-1)*d_itemSpacing;
	}

	// return the content size
	return Sizef(widest, total_height);
}


/*************************************************************************
	Handler for when window alpha changes
*************************************************************************/
void PopupMenu::onAlphaChanged(WindowEventArgs& e)
{
	MenuBase::onAlphaChanged(e);
	
	// if we are not fading, this is a real alpha change request and we save a copy of the value
	if (!d_fading)
	{
		d_origAlpha = d_alpha;
	}
}


/************************************************************************
    Handler for destruction started events
*************************************************************************/
void PopupMenu::onDestructionStarted(WindowEventArgs& e)
{
    // if we are attached to a menuitem, we make sure that gets updated
    Window* p = getParent();
    if (p && dynamic_cast<MenuItem*>(p))
    {
        static_cast<MenuItem*>(p)->setPopupMenu(0);
    }
    MenuBase::onDestructionStarted(e);
}


/************************************************************************
    Handler for shown events
*************************************************************************/
void PopupMenu::onShown(WindowEventArgs& e)
{
    d_isOpen = true;
    MenuBase::onShown(e);
}


/************************************************************************
    Handler for hidden events
*************************************************************************/
void PopupMenu::onHidden(WindowEventArgs& e)
{
    d_isOpen = false;
    MenuBase::onHidden(e);
}


/************************************************************************
	Handler for mouse button down events
************************************************************************/
void PopupMenu::onMouseButtonDown(MouseEventArgs& e)
{
        MenuBase::onMouseButtonDown(e);
        // dont reach our parent
        ++e.handled;
}


/************************************************************************
	Handler for mouse button up events
************************************************************************/
void PopupMenu::onMouseButtonUp(MouseEventArgs& e)
{
        MenuBase::onMouseButtonUp(e);
        // dont reach our parent
        ++e.handled;
}


/*************************************************************************
	Add PopupMenu specific properties
*************************************************************************/
void PopupMenu::addPopupMenuProperties(void)
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(PopupMenu, float,
        "FadeInTime", "Property to get/set the fade in time in seconds of the popup menu.  Value is a float.",
        &PopupMenu::setFadeInTime, &PopupMenu::getFadeInTime, 0.0f
    );
    
    CEGUI_DEFINE_PROPERTY(PopupMenu, float,
        "FadeOutTime", "Property to get/set the fade out time in seconds of the popup menu.  Value is a float.",
        &PopupMenu::setFadeOutTime, &PopupMenu::getFadeOutTime, 0.0f
    );
}

} // End of  CEGUI namespace section
