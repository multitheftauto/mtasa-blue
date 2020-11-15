/***********************************************************************
	created:	21/2/2004
	author:		Paul D Turner

	purpose:	Implements MouseCursor class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/MouseCursor.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"
#include "CEGUI/System.h"
#include "CEGUI/Renderer.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/Image.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/CoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Static Data Definitions
*************************************************************************/
bool MouseCursor::s_initialPositionSet = false;
Vector2f MouseCursor::s_initialPosition(0.0f, 0.0f);

/*************************************************************************
	Event name constants
*************************************************************************/
const String MouseCursor::EventNamespace("MouseCursor");
const String MouseCursor::EventImageChanged("ImageChanged");
const String MouseCursor::EventDefaultImageChanged("DefaultImageChanged");


/*************************************************************************
	constructor
*************************************************************************/
MouseCursor::MouseCursor(void) :
    d_cursorImage(0),
    d_defaultCursorImage(0),
    d_position(0.0f, 0.0f),
    d_visible(true),
    d_geometry(&System::getSingleton().getRenderer()->createGeometryBuffer()),
    d_customSize(0.0f, 0.0f),
    d_customOffset(0.0f, 0.0f),
    d_cachedGeometryValid(false)
{
    const Rectf screenArea(Vector2f(0, 0),
                            System::getSingleton().getRenderer()->getDisplaySize());
    d_geometry->setClippingRegion(screenArea);

	// default constraint is to whole screen
	setConstraintArea(&screenArea);

    if (s_initialPositionSet)
        setPosition(s_initialPosition);
    else
    	// mouse defaults to middle of the constrained area
        setPosition(Vector2f(screenArea.getWidth() / 2,
                              screenArea.getHeight() / 2));
}


/*************************************************************************
	Destructor
*************************************************************************/
MouseCursor::~MouseCursor(void)
{
    System::getSingleton().getRenderer()->destroyGeometryBuffer(*d_geometry);
}


/*************************************************************************
	Set the current mouse cursor image
*************************************************************************/
void MouseCursor::setImage(const Image* image)
{
    if (image == d_cursorImage)
        return;

	d_cursorImage = image;
    d_cachedGeometryValid = false;

	MouseCursorEventArgs args(this);
	args.image = image;
	onImageChanged(args);
}


/*************************************************************************
	Set the current mouse cursor image
*************************************************************************/
void MouseCursor::setImage(const String& name)
{
	setImage(&ImageManager::getSingleton().get(name));
}

//----------------------------------------------------------------------------//
void MouseCursor::setDefaultImage(const Image* image)
{
    if (image == d_defaultCursorImage)
        return;

	d_defaultCursorImage = image;
    d_cachedGeometryValid = d_cursorImage != 0;

	MouseCursorEventArgs args(this);
	args.image = image;
	onDefaultImageChanged(args);
}

//----------------------------------------------------------------------------//
void MouseCursor::setDefaultImage(const String& name)
{
	setDefaultImage(&ImageManager::getSingleton().get(name));
}

//----------------------------------------------------------------------------//
const Image* MouseCursor::getDefaultImage() const
{
    return d_defaultCursorImage;
}

/*************************************************************************
	Draw the mouse cursor
*************************************************************************/
void MouseCursor::draw(void) const
{
    if (!d_visible || !d_cursorImage)
        return;

    if (!d_cachedGeometryValid)
        cacheGeometry();

    d_geometry->draw();
}


/*************************************************************************
	Set the current mouse cursor position
*************************************************************************/
void MouseCursor::setPosition(const Vector2f& position)
{
    d_position = position;
	constrainPosition();

    d_geometry->setTranslation(Vector3f(d_position.d_x, d_position.d_y, 0));
}


/*************************************************************************
	Offset the mouse cursor position by the deltas specified in 'offset'.
*************************************************************************/
void MouseCursor::offsetPosition(const Vector2f& offset)
{
	d_position.d_x += offset.d_x;
	d_position.d_y += offset.d_y;
	constrainPosition();

    d_geometry->setTranslation(Vector3f(d_position.d_x, d_position.d_y, 0));
}


/*************************************************************************
	Checks the mouse cursor position is within the current 'constrain'
	Rect and adjusts as required.
*************************************************************************/
void MouseCursor::constrainPosition(void)
{
    Rectf absarea(getConstraintArea());

	if (d_position.d_x >= absarea.d_max.d_x)
		d_position.d_x = absarea.d_max.d_x -1;

	if (d_position.d_y >= absarea.d_max.d_y)
		d_position.d_y = absarea.d_max.d_y -1;

	if (d_position.d_y < absarea.d_min.d_y)
		d_position.d_y = absarea.d_min.d_y;

	if (d_position.d_x < absarea.d_min.d_x)
		d_position.d_x = absarea.d_min.d_x;
}


/*************************************************************************
	Set the area that the mouse cursor is constrained to.
*************************************************************************/
void MouseCursor::setConstraintArea(const Rectf* area)
{
    const Rectf renderer_area(Vector2f(0, 0),
                               System::getSingleton().getRenderer()->getDisplaySize());

	if (!area)
	{
		d_constraints.d_min.d_x = cegui_reldim(renderer_area.d_min.d_x / renderer_area.getWidth());
		d_constraints.d_min.d_y = cegui_reldim(renderer_area.d_min.d_y / renderer_area.getHeight());
		d_constraints.d_max.d_x = cegui_reldim(renderer_area.d_max.d_x / renderer_area.getWidth());
		d_constraints.d_max.d_y = cegui_reldim(renderer_area.d_max.d_y / renderer_area.getHeight());
	}
	else
	{
        Rectf finalArea(area->getIntersection(renderer_area));
		d_constraints.d_min.d_x = cegui_reldim(finalArea.d_min.d_x / renderer_area.getWidth());
		d_constraints.d_min.d_y = cegui_reldim(finalArea.d_min.d_y / renderer_area.getHeight());
		d_constraints.d_max.d_x = cegui_reldim(finalArea.d_max.d_x / renderer_area.getWidth());
		d_constraints.d_max.d_y = cegui_reldim(finalArea.d_max.d_y / renderer_area.getHeight());
	}

	constrainPosition();
}


/*************************************************************************
	Set the area that the mouse cursor is constrained to.
*************************************************************************/
void MouseCursor::setUnifiedConstraintArea(const URect* area)
{
    const Rectf renderer_area(Vector2f(0, 0),
                               System::getSingleton().getRenderer()->getDisplaySize());

	if (area)
	{
        d_constraints = *area;
	}
	else
	{
		d_constraints.d_min.d_x = cegui_reldim(renderer_area.d_min.d_x / renderer_area.getWidth());
		d_constraints.d_min.d_y = cegui_reldim(renderer_area.d_min.d_y / renderer_area.getHeight());
		d_constraints.d_max.d_x = cegui_reldim(renderer_area.d_max.d_x / renderer_area.getWidth());
		d_constraints.d_max.d_y = cegui_reldim(renderer_area.d_max.d_y / renderer_area.getHeight());
	}

	constrainPosition();
}

/*************************************************************************
	Set the area that the mouse cursor is constrained to.
*************************************************************************/
Rectf MouseCursor::getConstraintArea(void) const
{
    return Rectf(CoordConverter::asAbsolute(d_constraints, System::getSingleton().getRenderer()->getDisplaySize()));
}

/*************************************************************************
	Set the area that the mouse cursor is constrained to.
*************************************************************************/
const URect& MouseCursor::getUnifiedConstraintArea(void) const
{
    return d_constraints;
}

/*************************************************************************
	Return the current mouse cursor position in display resolution
	independant values.
*************************************************************************/
Vector2f MouseCursor::getDisplayIndependantPosition(void) const
{
    Sizef dsz(System::getSingleton().getRenderer()->getDisplaySize());

    return Vector2f(d_position.d_x / (dsz.d_width - 1.0f),
                 d_position.d_y / (dsz.d_height - 1.0f));
}

//----------------------------------------------------------------------------//
void MouseCursor::notifyDisplaySizeChanged(const Sizef& new_size)
{
    const Rectf screenArea(Vector2f(0, 0), new_size);
    d_geometry->setClippingRegion(screenArea);

    // invalidate to regenerate geometry at (maybe) new size
    d_cachedGeometryValid = false;
}

//----------------------------------------------------------------------------//
void MouseCursor::setExplicitRenderSize(const Sizef& size)
{
    d_customSize = size;
    d_cachedGeometryValid = false;
}

//----------------------------------------------------------------------------//
const Sizef& MouseCursor::getExplicitRenderSize() const
{
    return d_customSize;
}

//----------------------------------------------------------------------------//
void MouseCursor::cacheGeometry() const
{
    d_cachedGeometryValid = true;
    d_geometry->reset();

    // if no image, nothing more to do.
    if (!d_cursorImage)
        return;

    if (d_customSize.d_width != 0.0f || d_customSize.d_height != 0.0f)
    {
        calculateCustomOffset();
        d_cursorImage->render(*d_geometry, d_customOffset, d_customSize);
    }
    else
    {
        d_cursorImage->render(*d_geometry, Vector2f(0, 0));
    }
}

//----------------------------------------------------------------------------//
void MouseCursor::calculateCustomOffset() const
{
    const Sizef sz(d_cursorImage->getRenderedSize());
    const Vector2f offset(d_cursorImage->getRenderedOffset());

    d_customOffset.d_x =
        d_customSize.d_width / sz.d_width * offset.d_x - offset.d_x;
    d_customOffset.d_y =
        d_customSize.d_height / sz.d_height * offset.d_y - offset.d_y;
}

//----------------------------------------------------------------------------//
void MouseCursor::setInitialMousePosition(const Vector2f& position)
{
    s_initialPosition = position; 
    s_initialPositionSet = true;
}

//----------------------------------------------------------------------------//
void MouseCursor::invalidate()
{
    d_cachedGeometryValid = false;
}

//----------------------------------------------------------------------------//
void MouseCursor::onImageChanged(MouseCursorEventArgs& e)
{
    fireEvent(EventImageChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void MouseCursor::onDefaultImageChanged(MouseCursorEventArgs& e)
{
    fireEvent(EventDefaultImageChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
