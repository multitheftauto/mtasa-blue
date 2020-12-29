/************************************************************************
	filename: 	CEGUIColourRect.cpp
	created:	8/3/2004
	author:		Paul D Turner
	
	purpose:	Implements ColourRect class
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
#include "CEGUIColourRect.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constructor
*************************************************************************/
ColourRect::ColourRect(const colour& top_left, const colour& top_right, const colour& bottom_left, const colour& bottom_right) :
	d_top_left(top_left),
	d_top_right(top_right),
	d_bottom_left(bottom_left),
	d_bottom_right(bottom_right)
{
}


/*************************************************************************
	Constructor for ColourRect objects (via single colour).
*************************************************************************/
ColourRect::ColourRect(const colour& col) :
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
colour ColourRect::getColourAtPoint( float x, float y ) const
{
    colour h1((d_top_right - d_top_left) * x + d_top_left);
	colour h2((d_bottom_right - d_bottom_left) * x + d_bottom_left);
	return colour((h2 - h1) * y + h1);
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
void ColourRect::setColours(const colour& col)
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
