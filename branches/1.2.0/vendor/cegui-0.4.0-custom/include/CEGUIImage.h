/************************************************************************
	filename: 	CEGUIImage.h
	created:	13/3/2004
	author:		Paul D Turner
	
	purpose:	Defines interface for Image class
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
#ifndef _CEGUIImage_h_
#define _CEGUIImage_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIRect.h"
#include "CEGUIColourRect.h"
#include "CEGUIVector.h"
#include "CEGUISize.h"
#include "CEGUIRenderer.h"
#include <map>


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Class that represents a single Image of an Imageset.
*/
class CEGUIEXPORT Image
{
public:
	/*!
	\brief
		Return a Size object containing the dimensions of the Image.

	\return
		Size object holding the width and height of the Image.
	*/
	Size	getSize(void) const			{return Size(d_scaledWidth, d_scaledHeight);}
	

	/*!
	\brief
		Return the pixel width of the image.

	\return
		Width of this Image in pixels.
	*/
	float	getWidth(void) const		{return d_scaledWidth;}


	/*!
	\brief
		Return the pixel height of the image.

	\return
		Height of this Image in pixels
	*/
	float	getHeight(void) const		{return d_scaledHeight;}


	/*!
	\brief
		Return a Point object that contains the offset applied when rendering this Image

	\return
		Point object containing the offsets applied when rendering this Image
	*/
	Point	getOffsets(void) const		{return d_scaledOffset;}


	/*!
	\brief
		Return the X rendering offset

	\return
		X rendering offset.  This is the number of pixels that the image is offset by when rendering at any given location.
	*/
	float	getOffsetX(void) const		{return	d_scaledOffset.d_x;}


	/*!
	\brief
		Return the Y rendering offset

	\return
		Y rendering offset.  This is the number of pixels that the image is offset by when rendering at any given location.
	*/
	float	getOffsetY(void) const		{return d_scaledOffset.d_y;}


	/*!
	\brief
		Return the name of this Image object.

	\return
		String object containing the name of this Image
	*/
	const String&	getName(void) const;


	/*!
	\brief
		Return the name of the Imageset that contains this Image

	\return
		String object containing the name of the Imageset which this Image is a part of.
	*/
	const String&	getImagesetName(void) const;

    /*!
    \brief
        Return Rect describing the source texture area used by this Image.

    \return
        Rect object that describes, in pixels, the area upon the source texture
        which is used when rendering this Image.
    */
    const Rect& getSourceTextureArea(void) const;

	/*!
	\brief
		Queue the image to be drawn. 
		
	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param position
		Vector3 object containing the location where the Image is to be drawn

	\param size
		Size object describing the size that the Image is to be drawn at.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param top_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-left corner of the Image.

	\param top_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-right corner of the Image.

	\param bottom_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-left corner of the Image.

	\param bottom_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-right corner of the Image.
	
	\param quad_split_mode
		One of the QuadSplitMode values specifying the way quads are split into triangles

	\return
		Nothing
	*/
	void	draw(const Vector3& position, const Size& size, const Rect& clip_rect, const colour& top_left_colour = 0xFFFFFFFF, const colour& top_right_colour = 0xFFFFFFFF, const colour& bottom_left_colour = 0xFFFFFFFF, const colour& bottom_right_colour = 0xFFFFFFFF, QuadSplitMode quad_split_mode = TopLeftToBottomRight) const
	{
		draw(Rect(position.d_x, position.d_y, position.d_x + size.d_width, position.d_y + size.d_height), position.d_z, clip_rect, 
			ColourRect(top_left_colour, top_right_colour, bottom_left_colour, bottom_right_colour), quad_split_mode);
	}


	/*!
	\brief
		Queue the image to be drawn.

	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param dest_rect
		Rect object defining the area on-screen where the Image is to be drawn.  The Image will be scaled to fit the area as required.

	\param z
		Z-order position for the image.  Positions increase "into the screen", so 0.0f is at the top of the z-order.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param top_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-left corner of the Image.

	\param top_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-right corner of the Image.

	\param bottom_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-left corner of the Image.

	\param bottom_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-right corner of the Image.
	
	\param quad_split_mode
		One of the QuadSplitMode values specifying the way quads are split into triangles

	\return
		Nothing
	*/
	void	draw(const Rect& dest_rect, float z, const Rect& clip_rect, const colour& top_left_colour = 0xFFFFFFFF, const colour& top_right_colour = 0xFFFFFFFF, const colour& bottom_left_colour = 0xFFFFFFFF, const colour& bottom_right_colour = 0xFFFFFFFF, QuadSplitMode quad_split_mode = TopLeftToBottomRight) const
	{
		draw(dest_rect, z, clip_rect, ColourRect(top_left_colour, top_right_colour, bottom_left_colour, bottom_right_colour), quad_split_mode);
	}


	/*!
	\brief
		Queue the image to be drawn. 
		
	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param position
		Vector3 object containing the location where the Image is to be drawn

	\param size
		Size object describing the size that the Image is to be drawn at.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param colours
		ColourRect object that describes the colour values to use for each corner of the Image.
	
	\param quad_split_mode
		One of the QuadSplitMode values specifying the way quads are split into triangles

	\return
		Nothing
	*/
	void	draw(const Vector3& position, const Size& size, const Rect& clip_rect, const ColourRect& colours, QuadSplitMode quad_split_mode = TopLeftToBottomRight) const
	{
		draw(Rect(position.d_x, position.d_y, position.d_x + size.d_width, position.d_y + size.d_height), position.d_z, clip_rect, colours, quad_split_mode);
	}


	/*!
	\brief
		Queue the image to be drawn.

	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param position
		Vector3 object containing the location where the Image is to be drawn

	\note
		The image will be drawn at it's internally defined size.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param colours
		ColourRect object that describes the colour values to use for each corner of the Image.
	
	\param quad_split_mode
		One of the QuadSplitMode values specifying the way quads are split into triangles

	\return
		Nothing
	*/
	void	draw(const Vector3& position, const Rect& clip_rect, const ColourRect& colours, QuadSplitMode quad_split_mode = TopLeftToBottomRight) const
	{
		draw(Rect(position.d_x, position.d_y, position.d_x + getWidth(), position.d_y + getHeight()), position.d_z, clip_rect, colours, quad_split_mode);
	}


	/*!
	\brief
		Queue the image to be drawn.

	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param position
		Vector3 object containing the location where the Image is to be drawn

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param top_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-left corner of the Image.

	\param top_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-right corner of the Image.

	\param bottom_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-left corner of the Image.

	\param bottom_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-right corner of the Image.
	
	\param quad_split_mode
		One of the QuadSplitMode values specifying the way quads are split into triangles

	\return
		Nothing
	*/
	void	draw(const Vector3& position, const Rect& clip_rect, const colour& top_left_colour = 0xFFFFFFFF, const colour& top_right_colour = 0xFFFFFFFF, const colour& bottom_left_colour = 0xFFFFFFFF, const colour& bottom_right_colour = 0xFFFFFFFF, QuadSplitMode quad_split_mode = TopLeftToBottomRight) const
	{
		draw(Rect(position.d_x, position.d_y, position.d_x + getWidth(), position.d_y + getHeight()), position.d_z, clip_rect, ColourRect(top_left_colour, top_right_colour, bottom_left_colour, bottom_right_colour), quad_split_mode);
	}


	/*!
	\brief
		Queue the image to be drawn.

	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param dest_rect
		Rect object defining the area on-screen where the Image is to be drawn.  The Image will be scaled to fit the area as required.

	\param z
		Z-order position for the image.  Positions increase "into the screen", so 0.0f is at the top of the z-order.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param colours
		ColourRect object that describes the colour values to use for each corner of the Image.
	
	\param quad_split_mode
		One of the QuadSplitMode values specifying the way quads are split into triangles

	\return
		Nothing
	*/
	void	draw(const Rect& dest_rect, float z, const Rect& clip_rect,const ColourRect& colours, QuadSplitMode quad_split_mode = TopLeftToBottomRight) const;


    /*!
    \brief
        Writes an xml representation of this Image object to \a out_stream.

    \param out_stream
        Stream where xml data should be output.

    \return
        Nothing.
    */
    void writeXMLToStream(OutStream& out_stream) const;

    //Talihax
    Font* getFont(void) const { return d_font; }
    unsigned long getCodepoint(void) const { return d_codepoint; }


	friend class std::map<String, Image>;
	friend struct std::pair<const String, Image>;


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Default constructor (only used by std::map)
	*/
	Image(void) {}


	/*!
	\brief
		Constructor for Image objects.  This is not normally used directly by client code, use the Imageset interface instead.

	\param owner
		Pointer to a Imageset object that owns this Image.  This must not be NULL.
		
	\param name
		String object describing the name of the image being created.
		
	\param area
		Rect object describing an area that will be associated with this image.
		
	\param render_offset
		Point object that describes the offset to be applied when rendering this image.
		
	\param horzScaling
		float value indicating the initial horizontal scaling to be applied to this image.

	\param vertScaling
		float value indicating the initial vertical scaling to be applied to this image.

	\exception NullObjectException	Thrown if \a owner was NULL.
	*/
	Image(const Imageset* owner, const String& name, const Rect& area, const Point& render_offset, float horzScaling = 1.0f, float vertScaling = 1.0f, unsigned long ulCodepoint = 0, Font* pFont = NULL );



	/*!
	\brief
		Copy constructor
	*/
	Image(const Image& image);


	/*!
	\brief
		Destructor for Image objects.
	*/
	~Image(void);


private:
	/*************************************************************************
		Friends
	*************************************************************************/
	friend class Imageset;

		
	/*************************************************************************
		Implementation Methods
	*************************************************************************/
	/*!
	\brief
		set the horizontal scaling factor to be applied to this Image

	\param factor
		float value describing the scaling factor required.

	\return
		Nothing.
	*/
	void	setHorzScaling(float factor);


	/*!
	\brief
		set the vertical scaling factor to be applied to this Image

	\param factor
		float value describing the scaling factor required.

	\return
		Nothing.
	*/
	void	setVertScaling(float factor);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	const Imageset*	d_owner;		//!< Link back to Imageset that owns this image
	Rect			d_area;			//!< Rect defining the area on the texture that makes up this image.
	Point			d_offset;		//!< Offset to use when rendering

	// image auto-scaling fields.
	float	d_scaledWidth;		//!< scaled image width.
	float	d_scaledHeight;		//!< scaled image height.
	Point	d_scaledOffset;		//!< scaled rendering offset.
	String	d_name;				//!< name of this image.
    Font* d_font;
    unsigned long d_codepoint;
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIImage_h_
