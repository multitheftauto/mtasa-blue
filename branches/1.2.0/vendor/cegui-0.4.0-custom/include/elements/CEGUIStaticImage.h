/************************************************************************
	filename: 	CEGUIStaticImage.h
	created:	4/6/2004
	author:		Paul D Turner
	
	purpose:	Interface for the static image widget.
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
#ifndef _CEGUIStaticImage_h_
#define _CEGUIStaticImage_h_

#include "elements/CEGUIStatic.h"
#include "CEGUIRenderableImage.h"
#include "elements/CEGUIStaticImageProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Static image widget class.

	This base class performs it's own rendering.  There is no need to override this widget to perform rendering
	of static images.
*/
class CEGUIEXPORT StaticImage : public Static
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Formatting Enumerations
	*************************************************************************/
	/*!
	\brief
		Enumeration of horizontal formatting options for static image widgets
	*/
	enum HorzFormatting
	{
		LeftAligned,	//!< Image will be rendered at it's natural width, with it's left edge aligned with the left edge of the widget.
		RightAligned,	//!< Image will be rendered at it's natural width, with it's right edge aligned with the right edge of the widget.
		HorzCentred,	//!< Image will be rendered at it's natural width, horizontally centred within the widget.
		HorzStretched,	//!< Image will be horizontally stretched to cover the entire width of the widget.
		HorzTiled		//!< Image will be tiled horizontally across the width of the widget.  The rightmost tile may be clipped.
	};


	/*!
	\brief
		Enumeration of vertical formatting options for a static image widgets
	*/
	enum VertFormatting
	{
		TopAligned,		//!< Image will be rendered at it's natural height, with it's top edge aligned with the top edge of the widget.
		BottomAligned,	//!< Image will be rendered at it's natural height, with it's bottom edge aligned with the bottom edge of the widget.
		VertCentred,	//!< Image will be rendered at it's natural height, vertically centred within the widget.
		VertStretched,	//!< Image will be vertically stretched to cover the entire height of the widget.
		VertTiled		//!< Image will be tiled vertically down the height of the widget.  The bottommost tile may be clipped.
	};


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for StaticImage widgets.
	*/
	StaticImage(const String& type, const String& name);


	/*!
	\brief
		Destructor for StaticImage widgets.
	*/
	virtual ~StaticImage(void);


	/*************************************************************************
		Accessors
	*************************************************************************/
	/*!
	\brief
		Return a pointer to the current image displayed by this static image widget.

	\return
		Pointer to the Image object that is used to render this widget, or NULL for none.
	*/
	const Image*	getImage(void) const		{return d_image.getImage();}


	/*!
	\brief
		Return a ColourRect object containing the colours used when rendering this widget.

	\return
		ColourRect object initialised with the colours used when rendering the image for this widget.
	*/
	ColourRect	getImageColours(void) const		{return d_imageCols;}

	
	/*!
	\brief
		Return the current horizontal formatting option set for this widget.

	\return
		One of the StaticImage::HorzFormatting enumerated values specifying the horizontal formatting currently set.
	*/
	HorzFormatting	getHorizontalFormatting(void) const		{return	(HorzFormatting)d_image.getHorzFormatting();}


	/*!
	\brief
		Return the current vertical formatting option set for this widget.

	\return
		One of the StaticImage::VertFormatting enumerated values specifying the vertical formatting currently set.
	*/
	VertFormatting	getVerticalFormatting(void) const		{return	(VertFormatting)d_image.getVertFormatting();}


	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
		Set the Image object to be drawn by this widget.

	\param image
		Pointer to the Image object to be rendered.  Can be NULL to specify no image is to be rendered.
	
	\return
		Nothing.
	*/
	void	setImage(const Image* image);


	/*!
	\brief
		Set the Image object to be drawn by this widget.

	\param imageset
		String object specifying the name of the Imageset that holds the required image.

	\param image
		Name of the Image on the specified Imageset that is to be used.
	
	\return
		Nothing.

	\exception UnknownObjectException	thrown if Imageset \a imageset does not exist in the system or if \a imageset contains no Image named \a image.
	*/
	void	setImage(const String& imageset, const String& image);


	/*!
	\brief
		Sets the colours to be applied when rendering the image.

	\param colours
		ColourRect object describing the colours to be used.

	\return 
		Nothing.
	*/
	void	setImageColours(const ColourRect& colours);


	/*!
	\brief
		Sets the colours to be applied when rendering the image.

	\param top_left_colour
		Colour to be applied to the top-left corner of each Image used in the image.

	\param top_right_colour
		Colour to be applied to the top-right corner of each Image used in the image.

	\param bottom_left_colour
		Colour to be applied to the bottom-left corner of each Image used in the image.

	\param bottom_right_colour
		Colour to be applied to the bottom-right corner of each Image used in the image.

	\return 
		Nothing.
	*/
	void	setImageColours(const colour& top_left_colour, const colour& top_right_colour, const colour& bottom_left_colour, const colour& bottom_right_colour);


	/*!
	\brief
		Sets the colour to be applied when rendering the image.

	\param col
		colour value to be used when rendering.

	\return
		Nothing.
	*/
	void	setImageColours(const colour& col)		{setImageColours(col, col, col, col);}


	/*!
	\brief
		Set the formatting required for the image.

	\param h_fmt
		One of the HorzFormatting enumerated values specifying the formatting required.

	\param v_fmt
		One of the VertFormatting enumerated values specifying the formatting required.

	\return
		Nothing
	*/
	void	setFormatting(HorzFormatting h_fmt, VertFormatting v_fmt);


	/*!
	\brief
		Set the formatting required for the image.

	\param v_fmt
		One of the VertFormatting enumerated values specifying the formatting required.

	\return
		Nothing
	*/
	void	setVerticalFormatting(VertFormatting v_fmt);


	/*!
	\brief
		Set the formatting required for the image.

	\param h_fmt
		One of the HorzFormatting enumerated values specifying the formatting required.

	\return
		Nothing
	*/
	void	setHorizontalFormatting(HorzFormatting h_fmt);


protected:
	/*************************************************************************
		Overridden from base class
	*************************************************************************/
	virtual	void populateRenderCache();

	
	/*************************************************************************
		Event handling
	*************************************************************************/
	virtual void	onSized(WindowEventArgs& e);
	virtual void	onAlphaChanged(WindowEventArgs& e);
	virtual void	onStaticFrameChanged(WindowEventArgs& e);


	/*************************************************************************
		Implementation Methods
	*************************************************************************/
	/*!
	\brief
		update the internal RenderableImage with currently set colours and
		alpha settings.
	*/
	void	updateRenderableImageColours(void);


	/*!
	\brief
		Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

	\param class_name
		The class name that is to be checked.

	\return
		true if this window was inherited from \a class_name. false if not.
	*/
	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"StaticImage")	return true;
		return Static::testClassName_impl(class_name);
	}


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	RenderableImage	d_image;		//!< RenderableImage that does most of the work for us.
	ColourRect		d_imageCols;	//!< Colours to use for the image.


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static StaticImageProperties::Image				d_imageProperty;
	static StaticImageProperties::ImageColours		d_imageColoursProperty;
	static StaticImageProperties::VertFormatting	d_vertFormattingProperty;
	static StaticImageProperties::HorzFormatting	d_horzFormattingProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addStaticImageProperties( bool bCommon = true );
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addStaticImageProperties(false); }
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIStaticImage_h_
