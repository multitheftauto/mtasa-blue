/***********************************************************************
	created:	8/3/2004
	author:		Paul D Turner
	
	purpose:	Implements ColourRect class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/ColourRect.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constructor
*************************************************************************/
ColourRect::ColourRect(const Colour& top_left, const Colour& top_right, const Colour& bottom_left, const Colour& bottom_right) :
	d_top_left(top_left),
	d_top_right(top_right),
	d_bottom_left(bottom_left),
	d_bottom_right(bottom_right)
{
}


/*************************************************************************
	Constructor for ColourRect objects (via single colour).
*************************************************************************/
ColourRect::ColourRect(const Colour& col) :
	d_top_left(col),
	d_top_right(col),
	d_bottom_left(col),
	d_bottom_right(col)
{
}


/*************************************************************************
	Default constructor
*************************************************************************/
ColourRect::ColourRect(void) :
	d_top_left(),
	d_top_right(),
	d_bottom_left(),
	d_bottom_right()
{
}


/*************************************************************************
	Set the alpha value to use for all four corners of the ColourRect.	
*************************************************************************/
void ColourRect::setAlpha(float alpha)
{
	d_top_left.setAlpha(alpha);
	d_top_right.setAlpha(alpha);
	d_bottom_left.setAlpha(alpha);
	d_bottom_right.setAlpha(alpha);
}


/*************************************************************************
	Set the alpha value to use for the top edge of the ColourRect.	
*************************************************************************/
void ColourRect::setTopAlpha(float alpha)
{
	d_top_left.setAlpha(alpha);
	d_top_right.setAlpha(alpha);
}


/*************************************************************************
	Set the alpha value to use for the bottom edge of the ColourRect.	
*************************************************************************/
void ColourRect::setBottomAlpha(float alpha)
{
	d_bottom_left.setAlpha(alpha);
	d_bottom_right.setAlpha(alpha);
}


/*************************************************************************
	Set the alpha value to use for the left edge of the ColourRect.	
*************************************************************************/
void ColourRect::setLeftAlpha(float alpha)
{
	d_top_left.setAlpha(alpha);
	d_bottom_left.setAlpha(alpha);
}


/*************************************************************************
	Set the alpha value to use for the right edge of the ColourRect.	
*************************************************************************/
void ColourRect::setRightAlpha(float alpha)
{
	d_top_right.setAlpha(alpha);
	d_bottom_right.setAlpha(alpha);
}

/*************************************************************************
	Determinate whehter the ColourRect is monochromatic or variegated
*************************************************************************/
bool ColourRect::isMonochromatic() const
{
	return d_top_left == d_top_right &&
		   d_top_left == d_bottom_left &&
		   d_top_left == d_bottom_right;
}

/*************************************************************************
	Get the colour at a specified point
*************************************************************************/
Colour ColourRect::getColourAtPoint( float x, float y ) const
{
    Colour h1((d_top_right - d_top_left) * x + d_top_left);
	Colour h2((d_bottom_right - d_bottom_left) * x + d_bottom_left);
	return Colour((h2 - h1) * y + h1);
}

/*************************************************************************
	Get a ColourRectangle from the specified Region
*************************************************************************/
ColourRect ColourRect::getSubRectangle( float left, float right, float top, float bottom ) const
{
	return ColourRect(
		getColourAtPoint(left, top),
		getColourAtPoint(right, top),
		getColourAtPoint(left, bottom),
		getColourAtPoint(right, bottom)
	);
}


/*************************************************************************
	Set the colour of all four corners simultaneously.	
*************************************************************************/
void ColourRect::setColours(const Colour& col)
{
	d_top_left = d_top_right = d_bottom_left = d_bottom_right = col;
}


/*************************************************************************
	Module the alpha components of each corner's colour by a constant.
*************************************************************************/
void ColourRect::modulateAlpha(float alpha)
{
	d_top_left.setAlpha(d_top_left.getAlpha()*alpha);
	d_top_right.setAlpha(d_top_right.getAlpha()*alpha);
	d_bottom_left.setAlpha(d_bottom_left.getAlpha()*alpha);
	d_bottom_right.setAlpha(d_bottom_right.getAlpha()*alpha);
}

/*************************************************************************
    Modulate all components of this colour rect with corresponding
    components from another colour rect.
*************************************************************************/
ColourRect& ColourRect::operator *=(const ColourRect& other)
{
    d_top_left *= other.d_top_left;
    d_top_right *= other.d_top_right;
    d_bottom_left *= other.d_bottom_left;
    d_bottom_right *= other.d_bottom_right;

    return *this;
}


} // End of  CEGUI namespace section
