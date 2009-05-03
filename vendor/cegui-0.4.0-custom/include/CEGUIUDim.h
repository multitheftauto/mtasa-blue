/************************************************************************
    filename:   CEGUIUDim.h
    created:    Tue May 31 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#ifndef _CEGUIUDim_h_
#define _CEGUIUDim_h_

#include "CEGUIRect.h"
#include "CEGUIVector.h"

// some macros to aid in the creation of UDims
#define cegui_absdim(x)     UDim(0,(x))
#define cegui_reldim(x)     UDim((x),0)


// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Class representing a unified dimension; that is a dimension that has both
        a relative 'scale' portion and and absolute 'offset' portion.
    */
    class CEGUIEXPORT UDim
    {
    public:
        UDim() {}
        UDim(float scale, float offset) : d_scale(scale), d_offset(offset) {}
        ~UDim() {}

        float asAbsolute(float base) const    { return PixelAligned(base * d_scale) + d_offset; }
        float asRelative(float base) const    { return (base != 0.0f) ? d_offset / base + d_scale : 0.0f; }

        UDim operator+(const UDim& other) const     { return UDim(d_scale + other.d_scale, d_offset + other.d_offset); }
        UDim operator-(const UDim& other) const     { return UDim(d_scale - other.d_scale, d_offset - other.d_offset); }
        UDim operator/(const UDim& other) const     { return UDim(d_scale / other.d_scale, d_offset / other.d_offset); }
        UDim operator*(const UDim& other) const     { return UDim(d_scale * other.d_scale, d_offset * other.d_offset); }

        const UDim& operator+=(const UDim& other)   { d_scale += other.d_scale; d_offset += other.d_offset; return *this; }
        const UDim& operator-=(const UDim& other)   { d_scale -= other.d_scale; d_offset -= other.d_offset; return *this; }
        const UDim& operator/=(const UDim& other)   { d_scale /= other.d_scale; d_offset /= other.d_offset; return *this; }
        const UDim& operator*=(const UDim& other)   { d_scale *= other.d_scale; d_offset *= other.d_offset; return *this; }

        bool operator==(const UDim& other) const    { return d_scale == other.d_scale && d_offset == other.d_offset; }
        bool operator!=(const UDim& other) const    { return !operator==(other); }

        float d_scale, d_offset;
    };

    /*!
    \brief
        Two dimensional vector class built using unified dimensions (UDims).
        The UVector2 class is used for representing both positions and sizes.
    */
    class CEGUIEXPORT UVector2
    {
    public:
        UVector2() {}
        UVector2(const UDim& x, const UDim& y) : d_x(x), d_y(y) {}
        ~UVector2() {}

        Vector2 asAbsolute(const Size& base) const    { return Vector2(d_x.asAbsolute(base.d_width), d_y.asAbsolute(base.d_height)); }
        Vector2 asRelative(const Size& base) const    { return Vector2(d_x.asRelative(base.d_width), d_y.asRelative(base.d_height)); }

        UVector2 operator+(const UVector2& other) const     { return UVector2(d_x + other.d_x, d_y + other.d_y); }
        UVector2 operator-(const UVector2& other) const     { return UVector2(d_x - other.d_x, d_y - other.d_y); }
        UVector2 operator/(const UVector2& other) const     { return UVector2(d_x / other.d_x, d_y / other.d_y); }
        UVector2 operator*(const UVector2& other) const     { return UVector2(d_x * other.d_x, d_y * other.d_y); }

        const UVector2& operator+=(const UVector2& other)   { d_x += other.d_x; d_y += other.d_y; return *this; }
        const UVector2& operator-=(const UVector2& other)   { d_x -= other.d_x; d_y -= other.d_y; return *this; }
        const UVector2& operator/=(const UVector2& other)   { d_x /= other.d_x; d_y /= other.d_y; return *this; }
        const UVector2& operator*=(const UVector2& other)   { d_x *= other.d_x; d_y *= other.d_y; return *this; }
        
        bool operator==(const UVector2& other) const    { return d_x == other.d_x && d_y == other.d_y; }
        bool operator!=(const UVector2& other) const    { return !operator==(other); }

        UDim d_x, d_y;
    };

    /*!
    \brief
        Area rectangle class built using unified dimensions (UDims).
    */
    class CEGUIEXPORT URect
    {
    public:
        URect() {}
        
        URect(const UVector2& min, const UVector2& max) : d_min(min), d_max(max) {}
        
        URect(const UDim& left, const UDim& top, const UDim& right, const UDim& bottom)
        {
            d_min.d_x = left;
            d_min.d_y = top;
            d_max.d_x = right;
            d_max.d_y = bottom;
        }
        
        ~URect() {}
    
        Rect asAbsolute(const Size& base) const
        {
            return Rect(
                    d_min.d_x.asAbsolute(base.d_width),
                    d_min.d_y.asAbsolute(base.d_height),
                    d_max.d_x.asAbsolute(base.d_width),
                    d_max.d_y.asAbsolute(base.d_height)
                );
        }

        Rect asRelative(const Size& base) const
        {
            return Rect(
                    d_min.d_x.asRelative(base.d_width),
                    d_min.d_y.asRelative(base.d_height),
                    d_max.d_x.asRelative(base.d_width),
                    d_max.d_y.asRelative(base.d_height)
                );
        }

        const UVector2& getPosition() const     { return d_min; }
        UVector2 getSize() const                { return d_max - d_min; }
        UDim getWidth() const                   { return d_max.d_x - d_min.d_x; }
        UDim getHeight() const                  { return d_max.d_y - d_min.d_y; }

        void setPosition(const UVector2& pos)
        {
            UVector2 sz(d_max - d_min);
            d_min = pos;
            d_max = d_min + sz;
        }

        void setSize(const UVector2& sz)
        {
            d_max = d_min + sz;
        }

        void setWidth(const UDim& w)        { d_max.d_x = d_min.d_x + w; }
        void setHeight(const UDim& h)       { d_max.d_y = d_min.d_y + h; }

        void offset(const UVector2& sz)
        {
            d_min += sz;
            d_max += sz;
        }
        
        UVector2 d_min, d_max;
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIUDim_h_
