/************************************************************************
	filename: 	CEGUIRenderableImage.h
	created:	17/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to renderable image UI entity
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
#ifndef _CEGUIRenderableImage_h_
#define _CEGUIRenderableImage_h_

#include "CEGUIBase.h"
#include "CEGUIRenderableElement.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	A higher order GUI entity that represents a renderable image with formatting options.

	This class is intended to be used where a (usually top-level) GUI element needs to draw an image that requires some additional
	formatting.  It is possible to specify the Image that is to be rendered, as well as the horizontal and vertical formatting
	required.
*/
class CEGUIEXPORT RenderableImage : public RenderableElement
{
public:
	/*************************************************************************
		Formatting Enumerations
	*************************************************************************/
	/*!
	\brief
		Enumeration of horizontal formatting options for a RenderableImage
	*/
	enum HorzFormatting
	{
		LeftAligned,	//!< Image will be rendered at it's natural size and with it's left edge aligned with the left edge of the RenderableImage rect.
		RightAligned,	//!< Image will be rendered at it's natural size and with it's right edge aligned with the right edge of the RenderableImage rect.
		HorzCentred,	//!< Image will be rendered at it's natural size and horizontally centred within the RenderableImage rect.
		HorzStretched,	//!< Image will be horizontally stretched to cover the entire width of the RenderableImage rect.
		HorzTiled		//!< Image will be tiled horizontally across the width of the RenderableImage rect.  The rightmost tile will be clipped to remain within the rect.
	};


	/*!
	\brief
		Enumeration of vertical formatting options for a RenderableImage
	*/
	enum VertFormatting
	{
		TopAligned,		//!< Image will be rendered at it's natural size and with it's top edge aligned with the top edge of the RenderableImage rect.
		BottomAligned,	//!< Image will be rendered at it's natural size and with it's bottom edge aligned with the bottom edge of the RenderableImage rect.
		VertCentred,	//!< Image will be rendered at it's natural size and vertically centred within the RenderableImage rect.
		VertStretched,	//!< Image will be vertically stretched to cover the entire height of the RenderableImage rect.
		VertTiled		//!< Image will be tiled vertically down the height of the RenderableImage rect.  The bottommost tile will be clipped to remain within the rect.
	};


	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Default constructor for RenderableImage objects.
	*/
	RenderableImage(void);


	/*!
	\brief
		Destructor for RenderableImage objects
	*/
	virtual ~RenderableImage(void);


	/*************************************************************************
		Public Interface
	*************************************************************************/
	/*!
	\brief
		Set the Image object to be drawn by this RenderableImage.

	\param image
		Pointer to the Image object to be rendered.  Can be NULL to specify no image is to be rendered.
	
	\return
		Nothing.
	*/
	void	setImage(const Image* image)		{d_image = image;}


	/*!
	\brief
		Set the required horizontal formatting

	\param formatting
		One of the HorzFormatting values specifying the formatting required.

	\return
		Nothing
	*/
	void	setHorzFormatting(HorzFormatting formatting)			{d_horzFormat = formatting;}
		

	/*!
	\brief
		Set the required vertical formatting

	\param formatting
		One of the VertFormatting values specifying the formatting required.

	\return
		Nothing
	*/
	void	setVertFormatting(VertFormatting formatting)			{d_vertFormat = formatting;}
	
	
	/*!
	\brief
		Set the required quad split mode

	\param split_mode
		One of the QuadSplitMode values specifying the way quads are split into triangles.

	\return
		Nothing
	*/
	void	setQuadSplitMode(QuadSplitMode split_mode)				{d_quadSplitMode = split_mode;}


	/*!
	\brief
		Return the Image object currently set to be rendered.

	\return
		Pointer to the current Image object set for this RenderableImage.  May return NULL if no image is set.
	*/
	const Image*	getImage(void) const		{return d_image;}


	/*!
	\brief
		Return current horizontal formatting setting.

	\return
		One of the HorzFormatting values specifying the formatting set.
	*/
	HorzFormatting	getHorzFormatting(void) const				{return d_horzFormat;}


	/*!
	\brief
		Return current vertical formatting setting.

	\return
		One of the VertFormatting values specifying the formatting set.
	*/
	VertFormatting	getVertFormatting(void) const				{return d_vertFormat;}
	
	
	/*!
	\brief
		Return current quad split mode setting.

	\return
		One of the QuadSplitMode values specifying the way quads are split into triangles.
	*/
	QuadSplitMode	getQuadSplitMode(void) const				{return d_quadSplitMode;}


protected:
	/*************************************************************************
		Implementation functions
	*************************************************************************/
	/*!
	\brief
		Renders the imagery for a RenderableImage element.

	\param position
		Vector3 object describing the final rendering position for the object.

	\param clip_rect
		Rect object describing the clipping area for the rendering.  No rendering will be performed outside this area.

	\return
		Nothing.
	*/
	void draw_impl(const Vector3& position, const Rect& clip_rect) const;

    // implements abstract method
    void draw_impl(RenderCache& renderCache) const;

    /*!
    \brief
        return number of horizontal tiles required.
    */
    uint getHorzTileCount() const;

    /*!
    \brief
        return number of vertical tiles required.
    */
    uint getVertTileCount() const;

    /*!
    \brief
        Return base x co-ord for first tile.
    */
    float getBaseXCoord(const Size& sz) const;

    /*!
    \brief
        Return base y co-ord for first tile.
    */
    float getBaseYCoord(const Size& sz) const;

    /*!
    \brief
        Return target size to be used for each tile.
    */
    Size getDestinationSize() const;

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	HorzFormatting	d_horzFormat;		//!< Currently set horizontal formatting option.
	VertFormatting	d_vertFormat;		//!< Currently set vertical formatting option.
	QuadSplitMode	d_quadSplitMode;	//!< Currently set quad split mode
	const Image*	d_image;			//!< Pointer to the actual Image to be displayed.
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIRenderableImage_h_
