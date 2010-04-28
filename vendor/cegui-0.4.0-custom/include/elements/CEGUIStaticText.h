/************************************************************************
	filename: 	CEGUIStaticText.h
	created:	4/6/2004
	author:		Paul D Turner
	
	purpose:	Defines interface for a static text widget
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
#ifndef _CEGUIStaticText_h_
#define _CEGUIStaticText_h_

#include "elements/CEGUIStatic.h"
#include "elements/CEGUIStaticTextProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Base class for a static text widget.
*/
class CEGUIEXPORT StaticText : public Static
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Formatting Enumerations
	*************************************************************************/
	/*!
	\brief
		Enumeration of horizontal formatting options for static text widgets
	*/
	enum HorzFormatting
	{
		LeftAligned,			//!< Text is output as a single line of text with the first character aligned with the left edge of the widget.
		RightAligned,			//!< Text is output as a single line of text with the last character aligned with the right edge of the widget.
		HorzCentred,			//!< Text is output as a single line of text horizontally centred within the widget.
		HorzJustified,			//!< Text is output as a single line of text with the first and last characters aligned with the edges of the widget.
		WordWrapLeftAligned,	//!< Text is output as multiple word-wrapped lines of text with the first character of each line aligned with the left edge of the widget.
		WordWrapRightAligned,	//!< Text is output as multiple word-wrapped lines of text with the last character of each line aligned with the right edge of the widget.
		WordWrapCentred,		//!< Text is output as multiple word-wrapped lines of text with each line horizontally centered within the widget.
		WordWrapJustified		//!< Text is output as multiple word-wrapped lines of text with the first and last characters of each line aligned with the edges of the widget.
	};


	/*!
	\brief
		Enumeration of vertical formatting options for a static text widgets
	*/
	enum VertFormatting
	{
		TopAligned,		//!< Text is output with the top of first line of text aligned with the top edge of the widget.
		BottomAligned,	//!< Text is output with the bottom of last line of text aligned with the bottom edge of the widget.
		VertCentred     //!< Text is output vertically centred within the widget.
	};


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for static text widgets.
	*/
	StaticText(const String& type, const String& name);


	/*!
	\brief
		Destructor for static text widgets.
	*/
	virtual ~StaticText(void);


	/*************************************************************************
		Accessors
	*************************************************************************/
	/*!
	\brief
		Return a ColourRect object containing the colours used when rendering this widget.

	\return
		ColourRect object initialised with the colours used when rendering the text for this widget.
	*/
	ColourRect	getTextColours(void) const		{return d_textCols;}

	
	/*!
	\brief
		Return the current horizontal formatting option set for this widget.

	\return
		One of the StaticText::HorzFormatting enumerated values specifying the horizontal formatting currently set.
	*/
	HorzFormatting	getHorizontalFormatting(void) const		{return	d_horzFormatting;}


	/*!
	\brief
		Return the current vertical formatting option set for this widget.

	\return
		One of the StaticText::VertFormatting enumerated values specifying the vertical formatting currently set.
	*/
	VertFormatting	getVerticalFormatting(void) const		{return	d_vertFormatting;}


	/*!
	\brief
		Return whether the vertical scroll bar is set to be shown if needed.

	\return
		- true if the vertical scroll bar will be shown if needed (top or bottom aligned formatting only).
		- false if the vertical scroll bar will never be shown (default behaviour).
	*/
	bool	isVerticalScrollbarEnabled(void) const;


	/*!
	\brief
		Return whether the horizontal scroll bar is set to be shown if needed.

	\return
		- true if the horizontal scroll bar will be shown if needed (non-word wrapped, left or right aligned formatting only).
		- false if the horizontal scroll bar will never be shown (default behaviour).
	*/
	bool	isHorizontalScrollbarEnabled(void) const;


	// overridden (again) so scroll bars are not clipped when they are active
	virtual Rect	getUnclippedInnerRect(void) const;


	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
		Initialise the Window based object ready for use.

	\note
		This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	\return
		Nothing
	*/
	virtual void	initialise(void);


	/*!
	\brief
		Sets the colours to be applied when rendering the text.

	\param colours
		ColourRect object describing the colours to be used for each text glyph rendered.

	\return 
		Nothing.
	*/
	void	setTextColours(const ColourRect& colours);


	/*!
	\brief
		Sets the colours to be applied when rendering the text.

	\param top_left_colour
		Colour to be applied to the top-left corner of each text glyph rendered.

	\param top_right_colour
		Colour to be applied to the top-right corner of each text glyph rendered.

	\param bottom_left_colour
		Colour to be applied to the bottom-left corner of each text glyph rendered.

	\param bottom_right_colour
		Colour to be applied to the bottom-right corner of each text glyph rendered.

	\return 
		Nothing.
	*/
	void	setTextColours(const colour& top_left_colour, const colour& top_right_colour, const colour& bottom_left_colour, const colour& bottom_right_colour);


	/*!
	\brief
		Sets the colour to be applied when rendering the text.

	\param col
		colour value to be used when rendering.

	\return
		Nothing.
	*/
	void	setTextColours(const colour& col)		{setTextColours(col, col, col, col);}


	/*!
	\brief
		Set the formatting required for the text.

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
		Set the formatting required for the text.

	\param v_fmt
		One of the VertFormatting enumerated values specifying the formatting required.

	\return
		Nothing
	*/
	void	setVerticalFormatting(VertFormatting v_fmt);


	/*!
	\brief
		Set the formatting required for the text.

	\param h_fmt
		One of the HorzFormatting enumerated values specifying the formatting required.

	\return
		Nothing
	*/
	void	setHorizontalFormatting(HorzFormatting h_fmt);


	/*!
	\brief
		Set whether the vertical scroll bar will be shown if needed.

	\param setting
		- true if the vertical scroll bar should be shown if needed (top or bottom aligned formatting only).
		- false if the vertical scroll bar should never be shown (default behaviour).
	*/
	void	setVerticalScrollbarEnabled(bool setting);


	/*!
	\brief
		Set whether the horizontal scroll bar will be shown if needed.

	\param setting
		- true if the horizontal scroll bar should be shown if needed (non-word wrapped, left or right aligned formatting only).
		- false if the horizontal scroll bar should never be shown (default behaviour).
	*/
	void	setHorizontalScrollbarEnabled(bool setting);


protected:
	/*************************************************************************
		Overridden from base class
	*************************************************************************/
	virtual void populateRenderCache();


	/*************************************************************************
		Overridden events
	*************************************************************************/
	virtual void	onTextChanged(WindowEventArgs& e);
	virtual void	onSized(WindowEventArgs& e);
	virtual	void	onFontChanged(WindowEventArgs& e);
	virtual void	onMouseWheel(MouseEventArgs& e);


	/*************************************************************************
		Implementation methods
	*************************************************************************/
	/*!
	\brief
		Return a Rect object describing, in un-clipped pixels, the window relative area
		that the text should be rendered in to.

	\return
		Rect object describing the area of the Window to be used for rendering text.
	*/
	virtual	Rect	getTextRenderArea(void) const;


	/*!
	\brief
		display required integrated scroll bars according to current state of the edit box and update their values.
	*/
	void	configureScrollbars(void);


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
		if (class_name==(const utf8*)"StaticText")	return true;
		return Static::testClassName_impl(class_name);
	}


	/*************************************************************************
		Implementation Methods (abstract)
	*************************************************************************/
	/*!
	\brief
		create and return a pointer to a Scrollbar widget for use as vertical scroll bar

	\param name
	   String containing the name to be given to the created component.

	\return
		Pointer to a Scrollbar to be used for scrolling vertically.
	*/
	virtual Scrollbar*	createVertScrollbar(const String& name) const		= 0;
 

	/*!
	\brief
		create and return a pointer to a Scrollbar widget for use as horizontal scroll bar

	\param name
	   String containing the name to be given to the created component.

	\return
		Pointer to a Scrollbar to be used for scrolling horizontally.
	*/
	virtual Scrollbar*	createHorzScrollbar(const String& name) const		= 0;


	/*************************************************************************
		Event subscribers
	*************************************************************************/
	bool	handleScrollbarChange(const EventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	HorzFormatting	d_horzFormatting;		//!< Horizontal formatting to be applied to the text.
	VertFormatting	d_vertFormatting;		//!< Vertical formatting to be applied to the text.
	ColourRect		d_textCols;				//!< Colours used when rendering the text.
	Scrollbar*		d_vertScrollbar;		//!< Widget used as vertical scrollbar;
	Scrollbar*		d_horzScrollbar;		//!< Widget used as horizontal scrollbar;
	bool			d_enableVertScrollbar;	//!< true if vertical scroll bar is enabled.
	bool			d_enableHorzScrollbar;	//!< true if horizontal scroll bar is enabled.

private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static StaticTextProperties::TextColours	d_textColoursProperty;
	static StaticTextProperties::VertFormatting	d_vertFormattingProperty;
	static StaticTextProperties::HorzFormatting	d_horzFormattingProperty;
	static StaticTextProperties::VertScrollbar	d_vertScrollbarProperty;
	static StaticTextProperties::HorzScrollbar	d_horzScrollbarProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addStaticTextProperties( bool bCommon = true );
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addStaticTextProperties(false); }
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIStaticText_h_
