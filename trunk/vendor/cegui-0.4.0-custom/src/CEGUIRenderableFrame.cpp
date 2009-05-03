/************************************************************************
	filename: 	CEGUIRenderableFrame.cpp
	created:	14/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of RenderableFrame
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
#include "CEGUIRenderableFrame.h"
#include "CEGUIImage.h"
#include "CEGUIExceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Default constructor for RenderableFrame
*************************************************************************/
RenderableFrame::RenderableFrame(void)
{
	d_left = d_right = d_top = d_bottom = NULL;
	d_topleft = d_topright = d_bottomleft = d_bottomright = NULL;
}


/*************************************************************************
	Destructor for RenderableFrame
*************************************************************************/
RenderableFrame::~RenderableFrame(void)
{
}


/*************************************************************************
	Set the images to use for the frame
*************************************************************************/
void RenderableFrame::setImages(const Image* topleft, const Image* topright, const Image* bottomleft, const Image* bottomright, const Image* left, const Image* top, const Image* right, const Image* bottom)
{
	d_topleft		= topleft;
	d_topright		= topright;
	d_bottomleft	= bottomleft;
	d_bottomright	= bottomright;

	d_left		= left;
	d_right		= right;
	d_top		= top;
	d_bottom	= bottom;
}


/*************************************************************************
	Drawing method for the frame
*************************************************************************/
void RenderableFrame::draw_impl(const Vector3& position, const Rect& clip_rect) const
{
	Vector3 final_pos(position);
	float	org_width = d_area.getWidth(), org_height = d_area.getHeight();
	Size	final_size;
	ColourRect final_colours(d_colours);
	bool calcColoursPerImage = !(d_useColoursPerImage || d_colours.isMonochromatic());
	float leftfactor, rightfactor, topfactor, bottomfactor;

	// calculate 'adjustments' required to accommodate corner pieces.
	float	coord_adj, size_adj;

	// draw top-edge, if required
	if (d_top != NULL) {

		// calculate adjustments required if top-left corner will be rendered.
		if (d_topleft != NULL) {
			size_adj = (d_topleft->getWidth() - d_topleft->getOffsetX());
			coord_adj = d_topleft->getWidth();
		}
		else {
			coord_adj	= 0;
			size_adj	= 0;
		}

		// calculate adjustments required if top-right corner will be rendered.
		if (d_topright != NULL) {
			size_adj += (d_topright->getWidth() + d_topright->getOffsetX());
		}

		final_size.d_width	= org_width - size_adj;
		final_size.d_height	= d_top->getHeight();
		final_pos.d_x		= position.d_x + coord_adj;
		final_pos.d_y		= position.d_y;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x - position.d_x + d_top->getOffsetX()) / org_width;
			rightfactor = leftfactor + final_size.d_width / org_width;
			topfactor = (final_pos.d_y - position.d_y + d_top->getOffsetY()) / org_height;
			bottomfactor = topfactor + final_size.d_height / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}
 		
		d_top->draw(final_pos, final_size, clip_rect, final_colours);
	}

	// draw bottom-edge, if required
	if (d_bottom != NULL) {

		// calculate adjustments required if bottom-left corner will be rendered.
		if (d_bottomleft != NULL) {
			size_adj = (d_bottomleft->getWidth() - d_bottomleft->getOffsetX());
			coord_adj = d_bottomleft->getWidth();
		}
		else {
			coord_adj	= 0;
			size_adj	= 0;
		}

		// calculate adjustments required if bottom-right corner will be rendered.
		if (d_bottomright != NULL) {
			size_adj += (d_bottomright->getWidth() + d_bottomright->getOffsetX());
		}

		final_size.d_width	= org_width - size_adj;
		final_size.d_height	= d_bottom->getHeight();
		final_pos.d_x		= position.d_x + coord_adj;
		final_pos.d_y		= position.d_y + org_height - final_size.d_height;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x - position.d_x + d_bottom->getOffsetX()) / org_width;
			rightfactor = leftfactor + final_size.d_width / org_width;
			topfactor = (final_pos.d_y - position.d_y + d_bottom->getOffsetY()) / org_height;
			bottomfactor = topfactor + final_size.d_height / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

		d_bottom->draw(final_pos, final_size, clip_rect, final_colours);
	}
	
	// draw left-edge, if required
	if (d_left != NULL) {

		// calculate adjustments required if top-left corner will be rendered.
		if (d_topleft != NULL) {
			size_adj = (d_topleft->getHeight() - d_topleft->getOffsetY());
			coord_adj = d_topleft->getHeight();
		}
		else {
			coord_adj	= 0;
			size_adj	= 0;
		}

		// calculate adjustments required if bottom-left corner will be rendered.
		if (d_bottomleft != NULL) {
			size_adj += (d_bottomleft->getHeight() + d_bottomleft->getOffsetY());
		}

		final_size.d_height	= org_height - size_adj;
		final_size.d_width	= d_left->getWidth();
		final_pos.d_y		= position.d_y + coord_adj;
		final_pos.d_x		= position.d_x;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x - position.d_x + d_left->getOffsetX()) / org_width;
			rightfactor = leftfactor + final_size.d_width / org_width;
			topfactor = (final_pos.d_y - position.d_y + d_left->getOffsetY()) / org_height;
			bottomfactor = topfactor + final_size.d_height / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

		d_left->draw(final_pos, final_size, clip_rect, final_colours);
	}

	// draw right-edge, if required
	if (d_right != NULL) {

		// calculate adjustments required if top-left corner will be rendered.
		if (d_topright != NULL) {
			size_adj = (d_topright->getHeight() - d_topright->getOffsetY());
			coord_adj = d_topright->getHeight();
		}
		else {
			coord_adj	= 0;
			size_adj	= 0;
		}

		// calculate adjustments required if bottom-left corner will be rendered.
		if (d_bottomright != NULL) {
			size_adj += (d_bottomright->getHeight() + d_bottomright->getOffsetY());
		}


		final_size.d_height	= org_height - size_adj;
		final_size.d_width	= d_right->getWidth();
		final_pos.d_y		= position.d_y + coord_adj;
		final_pos.d_x		= position.d_x + org_width - final_size.d_width;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x - position.d_x + d_right->getOffsetX()) / org_width;
			rightfactor = leftfactor + final_size.d_width / org_width;
			topfactor = (final_pos.d_y - position.d_y + d_right->getOffsetY()) / org_height;
			bottomfactor = topfactor + final_size.d_height / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

		d_right->draw(final_pos, final_size, clip_rect, final_colours);
	}

	// draw required corner pieces...
	if (d_topleft != NULL) {

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = d_topleft->getOffsetX() / org_width;
			rightfactor = leftfactor + d_topleft->getWidth() / org_width;
			topfactor = d_topleft->getOffsetY() / org_height;
			bottomfactor = topfactor + d_topleft->getHeight() / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

		d_topleft->draw(position, clip_rect, final_colours);
	}

	if (d_topright != NULL) {
		final_pos.d_x = position.d_x + org_width - d_topright->getWidth();
		final_pos.d_y = position.d_y;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x - position.d_x + d_topright->getOffsetX()) / org_width;
			rightfactor = leftfactor + d_topright->getWidth() / org_width;
			topfactor = (final_pos.d_y - position.d_y + d_topright->getOffsetY()) / org_height;
			bottomfactor = topfactor + d_topright->getHeight() / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

		d_topright->draw(final_pos, clip_rect, final_colours);
	}

	if (d_bottomleft != NULL) {
		final_pos.d_x = position.d_x;
		final_pos.d_y = position.d_y + org_height - d_bottomleft->getHeight();

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x - position.d_x + d_bottomleft->getOffsetX()) / org_width;
			rightfactor = leftfactor + d_bottomleft->getWidth() / org_width;
			topfactor = (final_pos.d_y - position.d_y + d_bottomleft->getOffsetY()) / org_height;
			bottomfactor = topfactor + d_bottomleft->getHeight() / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

		d_bottomleft->draw(final_pos, clip_rect, final_colours);
	}

	if (d_bottomright != NULL) {
		final_pos.d_x = position.d_x + org_width - d_bottomright->getWidth();
		final_pos.d_y = position.d_y + org_height - d_bottomright->getHeight();

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x - position.d_x + d_bottomright->getOffsetX()) / org_width;
			rightfactor = leftfactor + d_bottomright->getWidth() / org_width;
			topfactor = (final_pos.d_y - position.d_y + d_bottomright->getOffsetY()) / org_height;
			bottomfactor = topfactor + d_bottomright->getHeight() / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

		d_bottomright->draw(final_pos, clip_rect, final_colours);
	}

}


/*************************************************************************
	Set the Image to use for the specified location of the frame.
*************************************************************************/
void RenderableFrame::setImageForLocation(FrameLocation location, const Image* image)
{
	switch (location)
	{
	case TopLeftCorner:
		d_topleft = image;
		break;

	case TopRightCorner:
		d_topright = image;
		break;

	case BottomLeftCorner:
		d_bottomleft = image;
		break;

	case BottomRightCorner:
		d_bottomright = image;
		break;

	case LeftEdge:
		d_left = image;
		break;

	case RightEdge:
		d_right = image;
		break;

	case TopEdge:
		d_top = image;
		break;

	case BottomEdge:
		d_bottom = image;
		break;

	default:
		break;
	}

}


/*************************************************************************
	Return the Image being used for the specified location of the frame.	
*************************************************************************/
const Image* RenderableFrame::getImageForLocation(FrameLocation location) const
{
	switch (location)
	{
	case TopLeftCorner:
		return d_topleft;
		break;

	case TopRightCorner:
		return d_topright;
		break;

	case BottomLeftCorner:
		return d_bottomleft;
		break;

	case BottomRightCorner:
		return d_bottomright;
		break;

	case LeftEdge:
		return d_left;
		break;

	case RightEdge:
		return d_right;
		break;

	case TopEdge:
		return d_top;
		break;

	case BottomEdge:
		return d_bottom;
		break;

	default:
		return NULL;
		break;
	}

}

void RenderableFrame::draw_impl(RenderCache& renderCache) const
{
    // TODO: This is a fairly substantial Cut, paste, and hack job.
    // TODO: There are probably a thousand ways that this should be improved!

    Rect destArea;
	Vector3 final_pos(0,0,0);
	float	org_width = d_area.getWidth(), org_height = d_area.getHeight();
	Size	final_size;
	ColourRect final_colours(d_colours);
	bool calcColoursPerImage = !(d_useColoursPerImage || d_colours.isMonochromatic());
	float leftfactor, rightfactor, topfactor, bottomfactor;

	// calculate 'adjustments' required to accommodate corner pieces.
	float	coord_adj, size_adj;

	// draw top-edge, if required
	if (d_top != NULL) {

		// calculate adjustments required if top-left corner will be rendered.
		if (d_topleft != NULL) {
			size_adj = (d_topleft->getWidth() - d_topleft->getOffsetX());
			coord_adj = d_topleft->getWidth();
		}
		else {
			coord_adj	= 0;
			size_adj	= 0;
		}

		// calculate adjustments required if top-right corner will be rendered.
		if (d_topright != NULL) {
			size_adj += (d_topright->getWidth() + d_topright->getOffsetX());
		}

		final_size.d_width	= org_width - size_adj;
		final_size.d_height	= d_top->getHeight();
		final_pos.d_x		= coord_adj;
		final_pos.d_y		= 0;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x + d_top->getOffsetX()) / org_width;
			rightfactor = leftfactor + final_size.d_width / org_width;
			topfactor = (final_pos.d_y + d_top->getOffsetY()) / org_height;
			bottomfactor = topfactor + final_size.d_height / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

        destArea.d_left = final_pos.d_x;
        destArea.d_top = final_pos.d_y;
        destArea.d_right = final_pos.d_x + final_size.d_width;
        destArea.d_bottom = final_pos.d_y + final_size.d_height;

        renderCache.cacheImage(*d_top, destArea, 0, final_colours);
	}

	// draw bottom-edge, if required
	if (d_bottom != NULL) {

		// calculate adjustments required if bottom-left corner will be rendered.
		if (d_bottomleft != NULL) {
			size_adj = (d_bottomleft->getWidth() - d_bottomleft->getOffsetX());
			coord_adj = d_bottomleft->getWidth();
		}
		else {
			coord_adj	= 0;
			size_adj	= 0;
		}

		// calculate adjustments required if bottom-right corner will be rendered.
		if (d_bottomright != NULL) {
			size_adj += (d_bottomright->getWidth() + d_bottomright->getOffsetX());
		}

		final_size.d_width	= org_width - size_adj;
		final_size.d_height	= d_bottom->getHeight();
		final_pos.d_x		= coord_adj;
		final_pos.d_y		= org_height - final_size.d_height;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x + d_bottom->getOffsetX()) / org_width;
			rightfactor = leftfactor + final_size.d_width / org_width;
			topfactor = (final_pos.d_y + d_bottom->getOffsetY()) / org_height;
			bottomfactor = topfactor + final_size.d_height / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

        destArea.d_left = final_pos.d_x;
        destArea.d_top = final_pos.d_y;
        destArea.d_right = final_pos.d_x + final_size.d_width;
        destArea.d_bottom = final_pos.d_y + final_size.d_height;

        renderCache.cacheImage(*d_bottom, destArea, 0, final_colours);
	}
	
	// draw left-edge, if required
	if (d_left != NULL) {

		// calculate adjustments required if top-left corner will be rendered.
		if (d_topleft != NULL) {
			size_adj = (d_topleft->getHeight() - d_topleft->getOffsetY());
			coord_adj = d_topleft->getHeight();
		}
		else {
			coord_adj	= 0;
			size_adj	= 0;
		}

		// calculate adjustments required if bottom-left corner will be rendered.
		if (d_bottomleft != NULL) {
			size_adj += (d_bottomleft->getHeight() + d_bottomleft->getOffsetY());
		}

		final_size.d_height	= org_height - size_adj;
		final_size.d_width	= d_left->getWidth();
		final_pos.d_y		= coord_adj;
		final_pos.d_x		= 0;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x + d_left->getOffsetX()) / org_width;
			rightfactor = leftfactor + final_size.d_width / org_width;
			topfactor = (final_pos.d_y + d_left->getOffsetY()) / org_height;
			bottomfactor = topfactor + final_size.d_height / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

        destArea.d_left = final_pos.d_x;
        destArea.d_top = final_pos.d_y;
        destArea.d_right = final_pos.d_x + final_size.d_width;
        destArea.d_bottom = final_pos.d_y + final_size.d_height;

        renderCache.cacheImage(*d_left, destArea, 0, final_colours);
	}

	// draw right-edge, if required
	if (d_right != NULL) {

		// calculate adjustments required if top-left corner will be rendered.
		if (d_topright != NULL) {
			size_adj = (d_topright->getHeight() - d_topright->getOffsetY());
			coord_adj = d_topright->getHeight();
		}
		else {
			coord_adj	= 0;
			size_adj	= 0;
		}

		// calculate adjustments required if bottom-left corner will be rendered.
		if (d_bottomright != NULL) {
			size_adj += (d_bottomright->getHeight() + d_bottomright->getOffsetY());
		}


		final_size.d_height	= org_height - size_adj;
		final_size.d_width	= d_right->getWidth();
		final_pos.d_y		= coord_adj;
		final_pos.d_x		= org_width - final_size.d_width;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x + d_right->getOffsetX()) / org_width;
			rightfactor = leftfactor + final_size.d_width / org_width;
			topfactor = (final_pos.d_y + d_right->getOffsetY()) / org_height;
			bottomfactor = topfactor + final_size.d_height / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

        destArea.d_left = final_pos.d_x;
        destArea.d_top = final_pos.d_y;
        destArea.d_right = final_pos.d_x + final_size.d_width;
        destArea.d_bottom = final_pos.d_y + final_size.d_height;

        renderCache.cacheImage(*d_right, destArea, 0, final_colours);
	}

	// draw required corner pieces...
	if (d_topleft != NULL) {

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = d_topleft->getOffsetX() / org_width;
			rightfactor = leftfactor + d_topleft->getWidth() / org_width;
			topfactor = d_topleft->getOffsetY() / org_height;
			bottomfactor = topfactor + d_topleft->getHeight() / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

        destArea.d_left = 0;
        destArea.d_top = 0;
        destArea.d_right = d_topleft->getWidth();
        destArea.d_bottom = d_topleft->getHeight();

        renderCache.cacheImage(*d_topleft, destArea, 0, final_colours);
	}

	if (d_topright != NULL) {
		final_pos.d_x = org_width - d_topright->getWidth();
		final_pos.d_y = 0;

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x + d_topright->getOffsetX()) / org_width;
			rightfactor = leftfactor + d_topright->getWidth() / org_width;
			topfactor = (final_pos.d_y + d_topright->getOffsetY()) / org_height;
			bottomfactor = topfactor + d_topright->getHeight() / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

        destArea.d_left = final_pos.d_x;
        destArea.d_top = final_pos.d_y;
        destArea.d_right = final_pos.d_x + d_topright->getWidth();
        destArea.d_bottom = final_pos.d_y + d_topright->getHeight();

        renderCache.cacheImage(*d_topright, destArea, 0, final_colours);
	}

	if (d_bottomleft != NULL) {
		final_pos.d_x = 0;
		final_pos.d_y = org_height - d_bottomleft->getHeight();

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x + d_bottomleft->getOffsetX()) / org_width;
			rightfactor = leftfactor + d_bottomleft->getWidth() / org_width;
			topfactor = (final_pos.d_y + d_bottomleft->getOffsetY()) / org_height;
			bottomfactor = topfactor + d_bottomleft->getHeight() / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

        destArea.d_left = final_pos.d_x;
        destArea.d_top = final_pos.d_y;
        destArea.d_right = final_pos.d_x + d_bottomleft->getWidth();
        destArea.d_bottom = final_pos.d_y + d_bottomleft->getHeight();

        renderCache.cacheImage(*d_bottomleft, destArea, 0, final_colours);
	}

	if (d_bottomright != NULL) {
		final_pos.d_x = org_width - d_bottomright->getWidth();
		final_pos.d_y = org_height - d_bottomright->getHeight();

		// calculate final colours that are to be used
		if (calcColoursPerImage)
		{
			leftfactor = (final_pos.d_x + d_bottomright->getOffsetX()) / org_width;
			rightfactor = leftfactor + d_bottomright->getWidth() / org_width;
			topfactor = (final_pos.d_y + d_bottomright->getOffsetY()) / org_height;
			bottomfactor = topfactor + d_bottomright->getHeight() / org_height;

			final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
		}

        destArea.d_left = final_pos.d_x;
        destArea.d_top = final_pos.d_y;
        destArea.d_right = final_pos.d_x + d_bottomright->getWidth();
        destArea.d_bottom = final_pos.d_y + d_bottomright->getHeight();

        renderCache.cacheImage(*d_bottomright, destArea, 0, final_colours);
	}
}

} // End of  CEGUI namespace section
