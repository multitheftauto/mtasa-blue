/************************************************************************
	filename: 	CEGUIRenderableImage.cpp
	created:	17/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of RenderableImage UI entity
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
#include "CEGUIRenderableImage.h"
#include "CEGUIImage.h"
#include "CEGUIExceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Constructor		
*************************************************************************/
RenderableImage::RenderableImage(void) :
	d_horzFormat(LeftAligned),
	d_vertFormat(TopAligned),
	d_quadSplitMode(TopLeftToBottomRight),
	d_image(NULL)
{
}


/*************************************************************************
	Destructor
*************************************************************************/
RenderableImage::~RenderableImage(void)
{
}


/*************************************************************************
	Renders the imagery for a RenderableImage element.
*************************************************************************/
void RenderableImage::draw_impl(const Vector3& position, const Rect& clip_rect) const
{
	// do not draw anything if image is not set.
	if (d_image == NULL)
		return;

	// calculate final clipping rect which is intersection of RenderableImage area and supplied clipping area
	Rect final_clipper(position.d_x, position.d_y, 0, 0);
	final_clipper.setSize(d_area.getSize());
	final_clipper = clip_rect.getIntersection(final_clipper);

	Size imgSize(getDestinationSize());
	// calculate number of times to tile image based of formatting options
	uint horzTiles = getHorzTileCount();
	uint vertTiles = getVertTileCount();
	// calculate 'base' X co-ordinate, depending upon formatting
	float baseX = getBaseXCoord(imgSize) + position.d_x;
	// calculate 'base' Y co-ordinate, depending upon formatting
	float baseY = getBaseYCoord(imgSize) + position.d_y;

	Vector3 drawpos(0,baseY, position.d_z);
	ColourRect final_colours(d_colours);
	bool calcColoursPerImage = !(d_useColoursPerImage || d_colours.isMonochromatic());

	// perform actual rendering
	for (uint row = 0; row < vertTiles; ++row)
	{
		drawpos.d_x = baseX;

		for (uint col = 0; col < horzTiles; ++col)
		{
			if (calcColoursPerImage)
			{
				float leftfactor = (drawpos.d_x - baseX + d_image->getOffsetX()) / d_area.getWidth();
				float rightfactor = leftfactor + imgSize.d_width / d_area.getWidth();
				float topfactor = (drawpos.d_y - baseY + d_image->getOffsetY()) / d_area.getHeight();
				float bottomfactor = topfactor + imgSize.d_height / d_area.getHeight();

				final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
			}

			d_image->draw(drawpos, imgSize, final_clipper, final_colours, d_quadSplitMode);
			drawpos.d_x += imgSize.d_width;
		}

		drawpos.d_y += imgSize.d_height;
	}

}

void RenderableImage::draw_impl(RenderCache& renderCache) const
{
    // do not do anything if image is not set.
    if (!d_image)
        return;

    // get size of destination image(s)
    Size imgSize(getDestinationSize());
    // get starting co-ords for image(s)
    float baseX = getBaseXCoord(imgSize) + d_area.d_left;
    float baseY = getBaseYCoord(imgSize) + d_area.d_top;
    // get number of image (tiles) to draw
    uint horzTiles = getHorzTileCount();
    uint vertTiles = getVertTileCount();

    // get initial colours and decide how to handle those for each drawn tile.
    ColourRect final_colours(d_colours);
    bool calcColoursPerImage = !(d_useColoursPerImage || d_colours.isMonochromatic());

    // init target area vertically
    Rect targetRect;
    targetRect.d_top    = baseY;
    targetRect.d_bottom = baseY + imgSize.d_height;

    // perform actual rendering
    for (uint row = 0; row < vertTiles; ++row)
    {
        // reset horizontal target position
        targetRect.d_left  = baseX;
        targetRect.d_right = baseX + imgSize.d_width;

        for (uint col = 0; col < horzTiles; ++col)
        {
            // calculate colours to be used as required.
            if (calcColoursPerImage)
            {
                float leftfactor = (targetRect.d_left - baseX + d_image->getOffsetX()) / d_area.getWidth();
                float rightfactor = leftfactor + imgSize.d_width / d_area.getWidth();
                float topfactor = (targetRect.d_top - baseY + d_image->getOffsetY()) / d_area.getHeight();
                float bottomfactor = topfactor + imgSize.d_height / d_area.getHeight();
                final_colours = d_colours.getSubRectangle( leftfactor, rightfactor, topfactor, bottomfactor);
            }

            // cache the image
            renderCache.cacheImage(*d_image, targetRect, 0, final_colours);
            // advance to next horizontal position
            targetRect.d_left  += imgSize.d_width;
            targetRect.d_right += imgSize.d_width;
        }

        // advance to next vertical position.
        targetRect.d_top    += imgSize.d_height;
        targetRect.d_bottom += imgSize.d_height;
    }
}

uint RenderableImage::getHorzTileCount() const
{
    return (d_horzFormat == HorzTiled) ? (uint)((d_area.getWidth() + (d_image->getWidth() - 1)) / d_image->getWidth()) : 1;
}

uint RenderableImage::getVertTileCount() const
{
    return (d_vertFormat == VertTiled) ? (uint)((d_area.getHeight() + (d_image->getHeight() - 1)) / d_image->getHeight()) : 1;
}

float RenderableImage::getBaseXCoord(const Size& sz) const
{
    switch (d_horzFormat)
    {
        case HorzStretched:
        case HorzTiled:
        case LeftAligned:
            return 0;
            break;

        case HorzCentred:
            return PixelAligned((d_area.getWidth() - sz.d_width) * 0.5f);
            break;

        case RightAligned:
            return d_area.getWidth() - sz.d_width;
            break;

        default:
            throw InvalidRequestException("An unknown horizontal formatting value was specified in a RenderableImage object.");
    }
}

float RenderableImage::getBaseYCoord(const Size& sz) const
{
    switch (d_vertFormat)
    {
        case VertStretched:
        case VertTiled:
        case TopAligned:
            return 0;
            break;

        case VertCentred:
            return PixelAligned((d_area.getHeight() - sz.d_height) * 0.5f);
            break;

        case BottomAligned:
            return d_area.getHeight() - sz.d_height;
            break;

        default:
            throw InvalidRequestException("An unknown vertical formatting value was specified in a RenderableImage object.");
    }
}

Size RenderableImage::getDestinationSize() const
{
    return Size (
        (d_horzFormat == HorzStretched) ? d_area.getWidth() : d_image->getWidth(),
        (d_vertFormat == VertStretched) ? d_area.getHeight() : d_image->getHeight()
    );
}

} // End of  CEGUI namespace section
