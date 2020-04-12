/************************************************************************
	filename: 	CEGUIStaticText.cpp
	created:	4/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of the static text widget class
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
#include "elements/CEGUIStaticText.h"
#include "CEGUIFont.h"
#include "CEGUIWindowManager.h"
#include "CEGUIExceptions.h"
#include "elements/CEGUIScrollbar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String StaticText::EventNamespace("StaticText");

/*************************************************************************
Static Properties for this class
*************************************************************************/
StaticTextProperties::TextColours		StaticText::d_textColoursProperty;
StaticTextProperties::VertFormatting	StaticText::d_vertFormattingProperty;
StaticTextProperties::HorzFormatting	StaticText::d_horzFormattingProperty;
StaticTextProperties::VertScrollbar		StaticText::d_vertScrollbarProperty;
StaticTextProperties::HorzScrollbar		StaticText::d_horzScrollbarProperty;


/*************************************************************************
	Constructor for static text widgets.	
*************************************************************************/
StaticText::StaticText(const String& type, const String& name) :
	Static(type, name),
	d_horzFormatting(LeftAligned),
	d_vertFormatting(VertCentred),
    d_textCols(0xFFFFFFFF),
	d_enableVertScrollbar(false),
	d_enableHorzScrollbar(false)
{
	addStaticTextProperties();
}


/*************************************************************************
	Destructor for static text widgets.
*************************************************************************/
StaticText::~StaticText(void)
{
}


/*************************************************************************
	Sets the colours to be applied when rendering the text.	
*************************************************************************/
void StaticText::setTextColours(const ColourRect& colours)
{
	d_textCols = colours;
	requestRedraw();
}


/*************************************************************************
	Sets the colours to be applied when rendering the text.	
*************************************************************************/
void StaticText::setTextColours(const colour& top_left_colour, const colour& top_right_colour, const colour& bottom_left_colour, const colour& bottom_right_colour)
{
	d_textCols.d_top_left		= top_left_colour;
	d_textCols.d_top_right		= top_right_colour;
	d_textCols.d_bottom_left	= bottom_left_colour;
	d_textCols.d_bottom_right	= bottom_right_colour;
	requestRedraw();
}


/*************************************************************************
	Set the formatting required for the text.
*************************************************************************/
void StaticText::setFormatting(HorzFormatting h_fmt, VertFormatting v_fmt)
{
	d_horzFormatting = h_fmt;
	d_vertFormatting = v_fmt;
	requestRedraw();
}


/*************************************************************************
	Set the formatting required for the text.	
*************************************************************************/
void StaticText::setVerticalFormatting(VertFormatting v_fmt)
{
	d_vertFormatting = v_fmt;
	requestRedraw();
}


/*************************************************************************
	Set the formatting required for the text.	
*************************************************************************/
void StaticText::setHorizontalFormatting(HorzFormatting h_fmt)
{
	d_horzFormatting = h_fmt;
	requestRedraw();
}


/*************************************************************************
	Perform the actual rendering for this Window.
*************************************************************************/
void StaticText::populateRenderCache()
{
	// get whatever base class needs to render.
	Static::populateRenderCache();

	const Font* font = getFont();
    // can't render text without a font :)
    if (font == 0)
        return;

	// get destination area for the text.
	Rect absarea(getTextRenderArea());
	Rect clipper(absarea);

	float textHeight = font->getFormattedLineCount(d_text, absarea, (TextFormatting)d_horzFormatting) * font->getLineSpacing();

	// see if we may need to adjust horizontal position
	if (d_horzScrollbar->isVisible())
	{
		switch(d_horzFormatting)
		{
		case LeftAligned:
		case WordWrapLeftAligned:
		case Justified:
		case WordWrapJustified:
			absarea.offset(Point(-d_horzScrollbar->getScrollPosition(), 0));
			break;

		case Centred:
		case WordWrapCentred:
			absarea.setWidth(d_horzScrollbar->getDocumentSize());
			absarea.offset(Point(-d_horzScrollbar->getScrollPosition(), 0));
			break;

		case RightAligned:
		case WordWrapRightAligned:
			absarea.offset(Point(d_horzScrollbar->getScrollPosition(), 0));
			break;
		}

	}

	// adjust y positioning according to formatting option
	switch(d_vertFormatting)
	{
	case TopAligned:
		absarea.d_top -= d_vertScrollbar->getScrollPosition();
		break;

	case VertCentred:
		// if scroll bar is in use, act like TopAligned
		if (d_vertScrollbar->isVisible())
		{
			absarea.d_top -= d_vertScrollbar->getScrollPosition();
		}
		// no scroll bar, so centre text instead.
		else
		{
			absarea.d_top += PixelAligned((absarea.getHeight() - textHeight) * 0.5f);
		}

		break;

	case BottomAligned:
		absarea.d_top = absarea.d_bottom - textHeight;
		absarea.d_top += d_vertScrollbar->getScrollPosition();
		break;
	}

    // calculate final colours
    ColourRect final_cols(d_textCols);
    final_cols.modulateAlpha(getEffectiveAlpha());
    // cache the text for rendering.
    d_renderCache.cacheText(d_text, font, (TextFormatting)d_horzFormatting, absarea, 0, final_cols, &clipper);
}


/*************************************************************************
	Add properties for static text
*************************************************************************/
void StaticText::addStaticTextProperties( bool bCommon )
{
	if ( bCommon == true )
    {
        addProperty(&d_textColoursProperty);
    }
    else
    {
	    addProperty(&d_vertFormattingProperty);
	    addProperty(&d_horzFormattingProperty);
	    addProperty(&d_vertScrollbarProperty);
	    addProperty(&d_horzScrollbarProperty);
    }
}


/*************************************************************************
	Perform initialisation for the widget.
*************************************************************************/
void StaticText::initialise(void)
{
	Static::initialise();

	// create the component sub-widgets
	d_vertScrollbar = createVertScrollbar(getName() + "__auto_vscrollbar__");
	d_horzScrollbar = createHorzScrollbar(getName() + "__auto_hscrollbar__");

	d_vertScrollbar->hide();
	d_horzScrollbar->hide();

	addChildWindow(d_vertScrollbar);
	addChildWindow(d_horzScrollbar);

	performChildWindowLayout();

	// event subscription
	d_vertScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&StaticText::handleScrollbarChange, this));
	d_horzScrollbar->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&StaticText::handleScrollbarChange, this));
}


/*************************************************************************
	Return a Rect object describing, in un-clipped pixels, the window
	relative area that the text should be rendered in to.
*************************************************************************/
Rect StaticText::getTextRenderArea(void) const
{
	Rect area(Point(0,0), getAbsoluteSize());

	if (d_horzScrollbar->isVisible())
	{
		area.d_bottom -= d_horzScrollbar->getAbsoluteHeight();
	}
	else if (d_frameEnabled)
	{
		area.d_bottom -= d_bottom_height;
	}

	if (d_vertScrollbar->isVisible())
	{
		area.d_right -= d_vertScrollbar->getAbsoluteWidth();
	}
	else if (d_frameEnabled)
	{
		area.d_right -= d_right_width;
	}

	if (d_frameEnabled)
	{
		area.d_left	+= d_left_width;
		area.d_top	+= d_top_height;
	}

	return area;
}


/*************************************************************************
	display required integrated scroll bars according to current state
	of the edit box and update their values.
*************************************************************************/
void StaticText::configureScrollbars(void)
{
    Scrollbar* vertScrollbar;
    Scrollbar* horzScrollbar;

    try
    {
        vertScrollbar = static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(getName() + "__auto_vscrollbar__"));
        horzScrollbar = static_cast<Scrollbar*>(WindowManager::getSingleton().getWindow(getName() + "__auto_hscrollbar__"));
    }
    catch (UnknownObjectException)
    {
        // no scrollbars?  Can't configure then!
        return;
    }

	const Font* font = getFont();

    // must have a font to measure text!
	if (font == 0)
	   return;

	Rect initialArea(getTextRenderArea());

	float totalHeight	= font->getFormattedLineCount(d_text, initialArea, (TextFormatting)d_horzFormatting) * font->getLineSpacing();
	float widestItem	= font->getFormattedTextExtent(d_text, initialArea, (TextFormatting)d_horzFormatting);

	//
	// First show or hide the scroll bars as needed (or requested)
	//
	// show or hide vertical scroll bar as required (or as specified by option)
	if ((totalHeight > getTextRenderArea().getHeight()) && d_enableVertScrollbar)
	{
		vertScrollbar->show();

		// show or hide horizontal scroll bar as required (or as specified by option)
		if ((widestItem > getTextRenderArea().getWidth()) && d_enableHorzScrollbar)
		{
			horzScrollbar->show();
		}
		else
		{
			horzScrollbar->hide();
		}

	}
	else
	{
		// show or hide horizontal scroll bar as required (or as specified by option)
		if ((widestItem > getTextRenderArea().getWidth()) && d_enableHorzScrollbar)
		{
			horzScrollbar->show();

			// show or hide vertical scroll bar as required (or as specified by option)
			if ((totalHeight > getTextRenderArea().getHeight()) && d_enableVertScrollbar)
			{
				vertScrollbar->show();
			}
			else
			{
				vertScrollbar->hide();
			}

		}
		else
		{
			vertScrollbar->hide();
			horzScrollbar->hide();
		}

	}

	//
	// Set up scroll bar values
	//
	Rect renderArea(getTextRenderArea());

	vertScrollbar->setDocumentSize(totalHeight);
	vertScrollbar->setPageSize(renderArea.getHeight());
	vertScrollbar->setStepSize(ceguimax(1.0f, renderArea.getHeight() / 10.0f));
	vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition());

	horzScrollbar->setDocumentSize(widestItem);
	horzScrollbar->setPageSize(renderArea.getWidth());
	horzScrollbar->setStepSize(ceguimax(1.0f, renderArea.getWidth() / 10.0f));
	horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition());
}


/*************************************************************************
	Return whether the vertical scroll bar is set to be shown if needed.	
*************************************************************************/
bool StaticText::isVerticalScrollbarEnabled(void) const
{
	return d_enableVertScrollbar;
}


/*************************************************************************
	Return whether the horizontal scroll bar is set to be shown if needed.	
*************************************************************************/
bool StaticText::isHorizontalScrollbarEnabled(void) const
{
	return d_enableHorzScrollbar;
}


/*************************************************************************
	Set whether the vertical scroll bar will be shown if needed.	
*************************************************************************/
void StaticText::setVerticalScrollbarEnabled(bool setting)
{
	d_enableVertScrollbar = setting;
	configureScrollbars();
	performChildWindowLayout();
}


/*************************************************************************
	Set whether the horizontal scroll bar will be shown if needed.	
*************************************************************************/
void StaticText::setHorizontalScrollbarEnabled(bool setting)
{
	d_enableHorzScrollbar = setting;
	configureScrollbars();
	performChildWindowLayout();
}


/*************************************************************************
	Handler called when text is changed.
*************************************************************************/
void StaticText::onTextChanged(WindowEventArgs& e)
{
	Static::onTextChanged(e);

	configureScrollbars();
	requestRedraw();
}


/*************************************************************************
	Handler called when size is changed
*************************************************************************/
void StaticText::onSized(WindowEventArgs& e)
{
	Static::onSized(e);

	configureScrollbars();
}


/*************************************************************************
	Handler called when font is changed.
*************************************************************************/
void StaticText::onFontChanged(WindowEventArgs& e)
{
	Static::onFontChanged(e);

	configureScrollbars();
	requestRedraw();
}


/*************************************************************************
	Handler for mouse wheel changes
*************************************************************************/
void StaticText::onMouseWheel(MouseEventArgs& e)
{
	// base class processing.
	Static::onMouseWheel(e);

	if (d_vertScrollbar->isVisible() && (d_vertScrollbar->getDocumentSize() > d_vertScrollbar->getPageSize()))
	{
		d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition() + d_vertScrollbar->getStepSize() * -e.wheelChange);
	}
	else if (d_horzScrollbar->isVisible() && (d_horzScrollbar->getDocumentSize() > d_horzScrollbar->getPageSize()))
	{
		d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition() + d_horzScrollbar->getStepSize() * -e.wheelChange);
	}

	e.handled = true;
}


/*************************************************************************
	Handler called when the scroll bar positions change
*************************************************************************/
bool StaticText::handleScrollbarChange(const EventArgs& e)
{
	requestRedraw();

	return true;
}

/*************************************************************************
	overridden so scroll bars are not partially clipped when active
*************************************************************************/
Rect StaticText::getUnclippedInnerRect(void) const
{
	// use default area from _Window_
	// (not from immediate base class Static, since that's what we're modifying)
	return Window::getUnclippedInnerRect();
}

} // End of  CEGUI namespace section
