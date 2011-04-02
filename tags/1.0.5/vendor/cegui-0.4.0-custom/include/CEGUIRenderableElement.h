/************************************************************************
	filename: 	CEGUIRenderableElement.h
	created:	14/4/2004
	author:		Paul D Turner
	
	purpose:	Interface for base class of a 'higher-level' renderable
				UI entity.
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
#ifndef _CEGUIRenderableElement_h_
#define _CEGUIRenderableElement_h_

#include "CEGUIBase.h"
#include "CEGUISystem.h"
#include "CEGUIColourRect.h"
#include "CEGUIVector.h"
#include "CEGUIRenderCache.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief

*/
class CEGUIEXPORT RenderableElement
{
public:
	/*************************************************************************
		Drawing methods
	*************************************************************************/
	/*!
	\brief
		Draw the element chain starting with this element.

	\param position
		Vector3 object describing the base position to be used when rendering the element chain.  Each element
		in the chain will be offset from this position by it's own internal position setting.

	\param clip_rect
		Rect object describing the clipping area.  No rendering will appear outside this area.
	*/
	void	draw(const Vector3& position, const Rect& clip_rect);

    /*!
    \brief
        Draw the element chain into the given RenderCache

    \param renderCache
        RenderCache object where element imagery is to be queued.

    \return
        Nothing.
    */
    void draw(RenderCache& renderCache) const;

	/*************************************************************************
		Accessors
	*************************************************************************/
	/*!
	\brief
		Return a pointer to the next RenderableElement in the chain.

	\return
		Pointer to a RenderableElement object that is linked this this one.  May be NULL for none.
	*/
	RenderableElement*	getNextElement(void) const			{return d_next;}


	/*!
	\brief
		Return the rendering colours set for this RenderableElement

	\return
		ColourRect object describing the colours to be used when rendering this RenderableElement.
	*/
	const ColourRect&	getColours(void) const						{return d_colours;}


	/*!
	\brief
		Return the offset position of this RenderableElement.

	\return
		Point object describing the offset position that this RenderableElement is to be rendered at.  This
		value is added to whatever position is specified when the RenderableElement::draw method is called to
		obtain the final rendering position.
	*/
	Point	getPosition(void) const							{return Point(d_area.d_left, d_area.d_top);}


	/*!
	\brief
		Return the current size of the element.

	\return
		Size object describing the current size of the RenderableFrame.
	*/
	Size	getSize(void) const				{return Size(d_area.getWidth(), d_area.getHeight());}


	/*!
	\brief
		Return the area for the element.

	\return
		Rect object describing the pixel area (offset from some unknown location) of the frame.
	*/
	const Rect&	getRect(void) const				{return d_area;}


	/*!
	\brief
		return whether the element colours will be applied locally to each image drawn as part of the RenderableElement, or
		applied across the whole of the RenderableElement area.

	\return
		- true if the RenderableElement colours will be applied locally to each sub-image drawn as part of this RenderableElement.
		- false if the RenderableElement colours will be applied evenly across the face of the entire RenderableElement.
	*/
	bool	isColourRectPerImage(bool setting) const		{return d_useColoursPerImage;}


	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
	Link another RenderableElement to this one.

	The linked element will be drawn whenever this element is drawn using the same base position and clipping area
	as provided when the RenderableElement::draw method is called.  Whole chains of Renderable Elements can be
	created using this system.

	\param element
		Pointer to a RenderableElement object that will be linked to this element.

	\return
		Nothing.
	*/
	void	setNextElement(RenderableElement* element)		{d_next = element;}


	/*!
	\brief
		Sets the colours to be applied when rendering the element.

	\param colours
		ColourRect object describing the colours to be used.

	\return 
		Nothing.
	*/
	void	setColours(const ColourRect& colours)			{d_colours = colours;}


	/*!
	\brief
		Sets the colours to be applied when rendering the element.

	\param top_left_colour
		Colour to be applied to the top-left corner of each Image used in the element.

	\param top_right_colour
		Colour to be applied to the top-right corner of each Image used in the element.

	\param bottom_left_colour
		Colour to be applied to the bottom-left corner of each Image used in the element.

	\param bottom_right_colour
		Colour to be applied to the bottom-right corner of each Image used in the element.

	\return 
		Nothing.
	*/
	void	setColours(const colour& top_left_colour, const colour& top_right_colour, const colour& bottom_left_colour, const colour& bottom_right_colour);


	/*!
	\brief
		Set the rendering offset position for this element.

	\param position
		Point object describing the offset position to use for this element.  This value is added to whatever
		position is specified when the RenderableElement::draw method is called to obtain the final rendering
		position.

	\return
		Nothing.
	*/
	void	setPosition(const Point& position)				{d_area.setPosition(position);}


	/*!
	\brief
		set the dimensions for the frame.

	\param size
		Size object describing the new size for the frame (in pixels)
	*/
	void	setSize(const Size& size)		{d_area.setSize(size);}


	/*!
	\brief
		Set the area for the frame

	\param area
		Rect object describing the pixel area (offset from some unknown location) of the frame.

	\return
		None.
	*/
	void	setRect(const Rect& area)		{d_area = area;}


	/*!
	\brief
		set whether the element colours should be applied locally to each image drawn as part of the RenderableElement, or
		applied across the whole of the RenderableElement area.

	\param setting
		- true to specify that RenderableElement colours be applied locally to each sub-image drawn as part of this RenderableElement.
		- false to specify that RenderableElement colours should be applied evenly across the face of the entire RenderableElement.

	\return
		Nothing.
	*/
	void	setColourRectPerImage(bool setting)		{d_useColoursPerImage = setting;}


	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for RenderableElement base class
	*/
	RenderableElement(void);



	/*!
	\brief
		Destructor for RenderableElement base class
	*/
	virtual ~RenderableElement(void);


protected:
	/*************************************************************************
		Implementation methods
	*************************************************************************/
	/*!
	\brief
		This function performs the required rendering for this element.

	\param position
		Vector3 object describing the final rendering position for the object.

	\param clip_rect
		Rect object describing the clipping area for the rendering.  No rendering will be performed outside this area.

	\return
		Nothing.
	*/
	virtual void	draw_impl(const Vector3& position, const Rect& clip_rect) const = 0;

    /*!
    \brief
        Implementation method to draw the element into the given RenderCache

    \param renderCache
        RenderCache object where element imagery is to be queued.

    \return
        Nothing.
    */
    virtual void draw_impl(RenderCache& renderCache) const = 0;


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	RenderableElement*	d_next;		//!< Link to another RenderableElement.
	ColourRect	d_colours;			//!< Colours to be used for this element;
	Rect		d_area;				//!< Currently defined area for this element.
	bool		d_useColoursPerImage;	//!< true if d_colours should be applied separately to each Image drawn (false to interpolate across d_area).
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIRenderableElement_h_
