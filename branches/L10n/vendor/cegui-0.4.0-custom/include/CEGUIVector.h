/************************************************************************
	filename: 	CEGUIVector.h
	created:	14/3/2004
	author:		Paul D Turner
	
	purpose:	Defines interfaces for Vector classes
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
#ifndef _CEGUIVector_h_
#define _CEGUIVector_h_

#include "CEGUIBase.h"
#include "CEGUISize.h"


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Class used as a two dimensional vector (aka a Point)
*/
class CEGUIEXPORT Vector2
{
public:
	Vector2(void) {}
	Vector2(float x, float y) : d_x(x), d_y(y) {}

	Vector2& operator*=(const Vector2& vec)
	{
		d_x *= vec.d_x;
		d_y *= vec.d_y;

		return *this;
	}

	Vector2& operator/=(const Vector2& vec)
	{
		d_x /= vec.d_x;
		d_y /= vec.d_y;

		return *this;
	}

	Vector2& operator+=(const Vector2& vec)
	{
		d_x += vec.d_x;
		d_y += vec.d_y;

		return *this;
	}

	Vector2& operator-=(const Vector2& vec)
	{
		d_x -= vec.d_x;
		d_y -= vec.d_y;

		return *this;
	}

	Vector2	operator+(const Vector2& vec) const
	{
		return Vector2(d_x + vec.d_x, d_y + vec.d_y);
	}

	Vector2	operator-(const Vector2& vec) const
	{
		return Vector2(d_x - vec.d_x, d_y - vec.d_y);
	}

	Vector2	operator*(const Vector2& vec) const
	{
		return Vector2(d_x * vec.d_x, d_y * vec.d_y);
	}

	bool	operator==(const Vector2& vec) const
	{
		return ((d_x == vec.d_x) && (d_y == vec.d_y));
	}

	bool	operator!=(const Vector2& vec) const
	{
		return !(operator==(vec));
	}

    Size asSize() const     { return Size(d_x, d_y); }

	float d_x, d_y;
};

/*!
\brief
	Point class
*/
typedef	Vector2		Point;


/*!
\brief
	Class used as a three dimensional vector
*/
class CEGUIEXPORT Vector3
{
public:
	Vector3(void) {}
	Vector3(float x, float y, float z) : d_x(x), d_y(y), d_z(z) {}

	float	d_x, d_y, d_z;
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIVector_h_
