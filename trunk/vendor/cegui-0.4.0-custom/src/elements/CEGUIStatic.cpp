/************************************************************************
	filename: 	CEGUIStatic.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Static widget base class
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
#include "elements/CEGUIStatic.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"


// Start of CEGUI namespace section
namespace CEGUI
{
const String Static::EventNamespace("Static");

/*************************************************************************
	Definitions of Properties for this class
*************************************************************************/
StaticProperties::FrameEnabled				Static::d_frameEnabledProperty;
StaticProperties::BackgroundEnabled			Static::d_backgroundEnabledProperty;
StaticProperties::FrameColours				Static::d_frameColoursProperty;
StaticProperties::BackgroundColours			Static::d_backgroundColoursProperty;
StaticProperties::BackgroundImage			Static::d_backgroundImageProperty;
StaticProperties::TopLeftFrameImage			Static::d_topLeftFrameProperty;
StaticProperties::TopRightFrameImage		Static::d_topRightFrameProperty;
StaticProperties::BottomLeftFrameImage		Static::d_bottomLeftFrameProperty;
StaticProperties::BottomRightFrameImage		Static::d_bottomRightFrameProperty;
StaticProperties::LeftFrameImage			Static::d_leftFrameProperty;
StaticProperties::RightFrameImage			Static::d_rightFrameProperty;
StaticProperties::TopFrameImage				Static::d_topFrameProperty;
StaticProperties::BottomFrameImage			Static::d_bottomFrameProperty;


/*************************************************************************
	Constructor for static widget base class
*************************************************************************/
Static::Static(const String& type, const String& name) :
	Window(type, name),
	d_frameEnabled(false),
	d_frameCols(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
	d_backgroundEnabled(false),
	d_backgroundCols(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
	d_background(NULL),
	d_left_width(0),
	d_right_width(0),
	d_top_height(0),
	d_bottom_height(0)
{
	addStaticProperties();
}


/*************************************************************************
	Destructor for static widget base class.
*************************************************************************/
Static::~Static(void)
{
}


/*************************************************************************
	overridden so derived classes are auto-clipped to within the inner
	area of the frame when it's active.
*************************************************************************/
Rect Static::getUnclippedInnerRect(void) const
{
	// if frame is enabled, return rect for area inside frame
	if (d_frameEnabled)
	{
		Rect tmp(Window::getUnclippedInnerRect());
		tmp.d_left		+= d_left_width;
		tmp.d_right		-= d_right_width;
		tmp.d_top		+= d_top_height;
		tmp.d_bottom	-= d_bottom_height;
		return tmp;
	}
	// no frame, so return default inner rect.
	else
	{
		return Window::getUnclippedInnerRect();
	}

}


/*************************************************************************
	Enable or disable rendering of the frame for this static widget.
*************************************************************************/
void Static::setFrameEnabled(bool setting)
{
	if (d_frameEnabled != setting)
	{
		d_frameEnabled = setting;
		WindowEventArgs args(this);
		onStaticFrameChanged(args);
		requestRedraw();
	}
}


/*************************************************************************
	specify the Image objects to use for each part of the frame.
	A NULL may be used to omit any part.	
*************************************************************************/
void Static::setFrameImages(const Image* topleft, const Image* topright, const Image* bottomleft, const Image* bottomright, const Image* left, const Image* top, const Image* right, const Image* bottom)
{
	// install the new images into the RenderableFrame
	d_frame.setImages(topleft, topright, bottomleft, bottomright, left, top, right, bottom);

	// get sizes of frame edges
	d_left_width	= (left != NULL) ? left->getWidth() : 0.0f;
	d_right_width	= (right != NULL) ? right->getWidth() : 0.0f;
	d_top_height	= (top != NULL) ? top->getHeight() : 0.0f;
	d_bottom_height	= (bottom != NULL) ? bottom->getHeight() : 0.0f;

	// redraw only if change would be seen.
	if (d_frameEnabled)
	{
		WindowEventArgs args(this);
		onStaticFrameChanged(args);
		requestRedraw();
	}

}


/*************************************************************************
	Sets the colours to be applied when rendering the frame	
*************************************************************************/
void Static::setFrameColours(const ColourRect& colours)
{
	d_frameCols = colours;
	updateRenderableFrameColours();

	// redraw only if change would be seen.
	if (d_frameEnabled)
	{
		WindowEventArgs args(this);
		onStaticFrameChanged(args);
		requestRedraw();
	}

}


/*************************************************************************
	Sets the colours to be applied when rendering the frame	
*************************************************************************/
void Static::setFrameColours(const colour& top_left_colour, const colour& top_right_colour, const colour& bottom_left_colour, const colour& bottom_right_colour)
{
	d_frameCols.d_top_left		= top_left_colour;
	d_frameCols.d_top_right		= top_right_colour;
	d_frameCols.d_bottom_left	= bottom_left_colour;
	d_frameCols.d_bottom_right	= bottom_right_colour;
	updateRenderableFrameColours();

	// redraw only if change would be seen.
	if (d_frameEnabled)
	{
		WindowEventArgs args(this);
		onStaticFrameChanged(args);
		requestRedraw();
	}

}


/*************************************************************************
	Enable or disable rendering of the background for this static widget.	
*************************************************************************/
void Static::setBackgroundEnabled(bool setting)
{
	if (d_backgroundEnabled != setting)
	{
		d_backgroundEnabled = setting;
		requestRedraw();
	}

}


/*************************************************************************
	Set the image to use as the background for the static widget.
*************************************************************************/
void Static::setBackgroundImage(const Image* image)
{
	d_background = image;

	if (d_backgroundEnabled)
	{
		requestRedraw();
	}

}


/*************************************************************************
	Set the image to use as the background for the static widget.	
*************************************************************************/
void Static::setBackgroundImage(const String& imageset, const String& image)
{
	setBackgroundImage(&ImagesetManager::getSingleton().getImageset(imageset)->getImage(image));
}


/*************************************************************************
	Sets the colours to be applied when rendering the background.	
*************************************************************************/
void Static::setBackgroundColours(const ColourRect& colours)
{
	d_backgroundCols = colours;

	if (d_backgroundEnabled)
	{
		requestRedraw();
	}

}


/*************************************************************************
	Sets the colours to be applied when rendering the background.	
*************************************************************************/
void Static::setBackgroundColours(const colour& top_left_colour, const colour& top_right_colour, const colour& bottom_left_colour, const colour& bottom_right_colour)
{
	d_backgroundCols.d_top_left		= top_left_colour;
	d_backgroundCols.d_top_right	= top_right_colour;
	d_backgroundCols.d_bottom_left	= bottom_left_colour;
	d_backgroundCols.d_bottom_right	= bottom_right_colour;

	if (d_backgroundEnabled)
	{
		requestRedraw();
	}

}


/*************************************************************************
	update the internal RenderableFrame with currently set colours and
	alpha settings.
*************************************************************************/
void Static::updateRenderableFrameColours(void)
{
	float alpha = getEffectiveAlpha();

	d_frame.setColours(
		calculateModulatedAlphaColour(d_frameCols.d_top_left, alpha),
		calculateModulatedAlphaColour(d_frameCols.d_top_right, alpha),
		calculateModulatedAlphaColour(d_frameCols.d_bottom_left, alpha),
		calculateModulatedAlphaColour(d_frameCols.d_bottom_right, alpha)
	);

}


/*************************************************************************
	given an ARGB colour value and a alpha float value return the colour
	value with the alpha component modulated by the given alpha float.
*************************************************************************/
colour Static::calculateModulatedAlphaColour(const colour& col, float alpha) const
{
	colour temp(col);
	temp.setAlpha(temp.getAlpha() * alpha);
	return temp;
}


/*************************************************************************
	Perform the actual rendering for this Window.	
*************************************************************************/
void Static::populateRenderCache()
{
	Rect backgroundRect(Point(0,0), getAbsoluteSize());

	// draw frame
	if (d_frameEnabled)
	{
        d_frame.draw(d_renderCache);

        // adjust destination area for backfrop image.
        backgroundRect.d_left		+= d_left_width;
        backgroundRect.d_right		-= d_right_width;
        backgroundRect.d_top		+= d_top_height;
        backgroundRect.d_bottom	-= d_bottom_height;
	}

	// draw backdrop
	if (d_backgroundEnabled && (d_background != NULL))
	{
        // factor window alpha into colours to use when rendering background
        ColourRect colours(d_backgroundCols);
        colours.modulateAlpha(getEffectiveAlpha());
        // cache image for drawing
        d_renderCache.cacheImage(*d_background, backgroundRect, 0, colours);
	}

}


/*************************************************************************
	Handler for when window is sized
*************************************************************************/
void Static::onSized(WindowEventArgs& e)
{
	// base class processing
	Window::onSized(e);

	// update frame size.
	d_frame.setSize(getAbsoluteSize());

	e.handled = true;
}


/*************************************************************************
	Handler for when alpha value changes
*************************************************************************/
void Static::onAlphaChanged(WindowEventArgs& e)
{
	// base class processing
	Window::onAlphaChanged(e);

	// update frame colours to use new alpha value
	updateRenderableFrameColours();

	e.handled = true;
}


/*************************************************************************
	Return the Image being used for the specified location of the frame.	
*************************************************************************/
const Image* Static::getImageForFrameLocation(FrameLocation location) const
{
	return d_frame.getImageForLocation(location);
}


/*************************************************************************
	Return the Image currently set as the background image for the widget.
*************************************************************************/
const Image* Static::getBackgroundImage(void) const
{
	return d_background;
}


/*************************************************************************
	Set the Image to use for the specified location of the frame.	
*************************************************************************/
void Static::setImageForFrameLocation(FrameLocation location, const Image* image)
{
	d_frame.setImageForLocation(location, image);

	// update our record of image size
	switch (location)
	{
	case LeftEdge:
		d_left_width = (image != NULL) ? image->getWidth() : 0;
		break;

	case RightEdge:
		d_right_width = (image != NULL) ? image->getWidth() : 0;
		break;

	case TopEdge:
		d_top_height = (image != NULL) ? image->getHeight() : 0;
		break;

	case BottomEdge:
		d_bottom_height = (image != NULL) ? image->getHeight() : 0;
		break;

	default:
		break;
	}

}

/*************************************************************************
	Adds properties for the static widget base class
*************************************************************************/
void Static::addStaticProperties( bool bCommon )
{
    if ( bCommon == true )
    {
        addProperty(&d_frameEnabledProperty);
        addProperty(&d_backgroundEnabledProperty);
    }
    else
    {
        addProperty(&d_frameColoursProperty);
        addProperty(&d_backgroundColoursProperty);
        addProperty(&d_backgroundImageProperty);
        addProperty(&d_topLeftFrameProperty);
        addProperty(&d_topRightFrameProperty);
        addProperty(&d_bottomLeftFrameProperty);
        addProperty(&d_bottomRightFrameProperty);
        addProperty(&d_leftFrameProperty);
        addProperty(&d_topFrameProperty);
        addProperty(&d_rightFrameProperty);
        addProperty(&d_bottomFrameProperty);
    }
}

} // End of  CEGUI namespace section
