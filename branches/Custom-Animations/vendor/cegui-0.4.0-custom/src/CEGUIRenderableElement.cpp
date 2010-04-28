/************************************************************************
	filename: 	CEGUIRenderableElement.cpp
	created:	14/4/2004
	author:		Paul D Turner
	
	purpose:	Implements RenderableElement base class
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
#include "CEGUIRenderableElement.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Constructor
*************************************************************************/
RenderableElement::RenderableElement(void) :
	d_next(NULL),
	d_colours(colour(1, 1, 1, 1)),
	d_area(0, 0, 0, 0),
	d_useColoursPerImage(false)
{
}


/*************************************************************************
	Destructor
*************************************************************************/
RenderableElement::~RenderableElement(void)
{
}


/*************************************************************************
	Sets the colours to be applied when rendering the element.
*************************************************************************/
void RenderableElement::setColours(const colour& top_left_colour, const colour& top_right_colour, const colour& bottom_left_colour, const colour& bottom_right_colour)
{
	d_colours.d_top_left		= top_left_colour;
	d_colours.d_top_right		= top_right_colour;
	d_colours.d_bottom_left		= bottom_left_colour;
	d_colours.d_bottom_right	= bottom_right_colour;
}


/*************************************************************************
	Draw the element chain starting with this element.
*************************************************************************/
void RenderableElement::draw(const Vector3& position, const Rect& clip_rect)
{
	Vector3	final_pos(position);
	final_pos.d_x += d_area.d_left;
	final_pos.d_y += d_area.d_top;

	// call implementation function to perform actual rendering
	draw_impl(final_pos, clip_rect);

	// render next element in the chain if any.
	if (d_next != NULL)
	{
		d_next->draw(position, clip_rect);
	}

}

void RenderableElement::draw(RenderCache& renderCache) const
{
    draw_impl(renderCache);

    // also draw next element in the chain
    if (d_next)
    {
        d_next->draw(renderCache);
    }
}

} // End of  CEGUI namespace section
