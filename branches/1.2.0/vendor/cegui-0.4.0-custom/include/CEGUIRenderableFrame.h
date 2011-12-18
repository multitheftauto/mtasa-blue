/************************************************************************
	filename: 	CEGUIRenderableFrame.h
	created:	14/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to RenderableFrame UI construct
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
#ifndef _CEGUIRenderableFrame_h_
#define _CEGUIRenderableFrame_h_

#include "CEGUIBase.h"
#include "CEGUIRenderableElement.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Enumeration of positions for a frame.
*/
enum FrameLocation
{
	Invalid,			// Value reserved to indicate some invalid or unknown location.
	TopLeftCorner,		// Top-Left corner of a frame / rectangle.
	TopRightCorner,		// Top-Right corner of a frame / rectangle.
	BottomLeftCorner,	// Bottom-Left corner of a frame / rectangle.
	BottomRightCorner,	// Bottom-Right corner of a frame / rectangle.
	LeftEdge,			// Left edge of a frame / rectangle.	
	TopEdge,			// Top edge of a frame / rectangle.
	RightEdge,			// Right edge of a frame / rectangle.
	BottomEdge			// Bottom edge of a frame / rectangle.
};


/*!
\brief
	A higher order GUI entity that represents a renderable frame.

	This class is intended to be used where a (usually top-level) GUI element needs to draw a frame that is constructed from
	a collection of Images.  It is possible to specify the image to use for each of the four corners, which are placed appropriately
	at their natural size, and the images for the four edges, which are stretched to cover the area between any corner images.  Any
	of the Images may be omitted, in which case that part of the frame is not drawn.  If the GUI element uses only one image for its
	frame (usually stretched over the entire area of the element) then a better choice would be to use a RenderableImage, or perform the
	rendering directly instead.
*/
class CEGUIEXPORT RenderableFrame : public RenderableElement
{
public:
	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Default constructor for RenderableFrame
	*/
	RenderableFrame(void);


	/*!
	\brief
		Destructor for RenderableFrame
	*/
	virtual ~RenderableFrame(void);


	/*!
	\brief
		specify the Image objects to use for each part of the frame.  A NULL may be used to omit any part.

	\param topleft
		Pointer to an Image object to render as the top-left corner of the frame.  Specify NULL to omit this part of the frame.

	\param topright
		Pointer to an Image object to render as the top-right corner of the frame.  Specify NULL to omit this part of the frame.

	\param bottomleft
		Pointer to an Image object to render as the bottom-left corner of the frame.  Specify NULL to omit this part of the frame.

	\param bottomright
		Pointer to an Image object to render as the bottom-right corner of the frame.  Specify NULL to omit this part of the frame.

	\param left
		Pointer to an Image object to render as the left edge of the frame.  Specify NULL to omit this part of the frame.

	\param top
		Pointer to an Image object to render as the top edge of the frame.  Specify NULL to omit this part of the frame.

	\param right
		Pointer to an Image object to render as the right edge of the frame.  Specify NULL to omit this part of the frame.

	\param bottom
		Pointer to an Image object to render as the bottom edge of the frame.  Specify NULL to omit this part of the frame.

	\return
		Nothing
	*/
	void	setImages(const Image* topleft, const Image* topright, const Image* bottomleft, const Image* bottomright, const Image* left, const Image* top, const Image* right, const Image* bottom);


	/*!
	\brief
		Set the Image to use for the specified location of the frame.

	\param location
		One of the FrameLocation enumerated values specifying the image to be returned.

	\param image
		Pointer to the Image to use for the frame location specified in \a location.  May be NULL to indicate the
		frame component is not required.

	\return
		Nothing.
	*/
	void	setImageForLocation(FrameLocation location, const Image* image);



	/*!
	\brief
		Return the Image being used for the specified location of the frame.

	\param location
		One of the FrameLocation enumerated values specifying the image to be returned.

	\return
		Pointer to the Image object currently set for the frame location specified in \a location.  May return NULL if no
		Image is set for the requested position.
	*/
	const Image*	getImageForLocation(FrameLocation location) const;


protected:
	/*************************************************************************
		Implementation functions
	*************************************************************************/
	/*!
	\brief
		Renders the imagery for a RenderableFrame element.

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

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	// image pointers
	const Image*	d_topleft;			//!< Image to draw for the top-left corner.
	const Image*	d_topright;			//!< Image to draw for the top-right corner.
	const Image*	d_bottomleft;		//!< Image to draw for the bottom-left corner.
	const Image*	d_bottomright;		//!< Image to draw for the bottom-right corner.
	const Image*	d_left;				//!< Image to draw for the left edge.
	const Image*	d_right;			//!< Image to draw for the right edge.
	const Image*	d_top;				//!< Image to draw for the top edge.
	const Image*	d_bottom;			//!< Image to draw for the bottom edge.
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIRenderableFrame_h_
