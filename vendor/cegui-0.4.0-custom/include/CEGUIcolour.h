/************************************************************************
	filename: 	CEGUIcolour.h
	created:	20/8/2004
	author:		Paul D Turner (with code from Jeff Leigh)
	
	purpose:	Defines interface to the colour class used to represent
				colour values within the system
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
#ifndef _CEGUIcolour_h_
#define _CEGUIcolour_h_

#include "CEGUIBase.h"

// Start of CEGUI namespace section
namespace CEGUI
{
typedef uint32 argb_t;    //!< 32 bit ARGB representation of a colour.

/*!
\brief
	Class representing colour values within the system.
*/
class CEGUIEXPORT colour
{
public:
	/*************************************************************************
		Construction & Destruction
	*************************************************************************/
	colour(void);
	colour(const colour& val);
	colour(float red, float green, float blue, float alpha = 1.0f);
	colour(argb_t argb);

	/*************************************************************************
		Accessors
	*************************************************************************/
	argb_t	getARGB(void) const
	{
		if (!d_argbValid)
		{
			d_argb = calculateARGB();
			d_argbValid = true;
		}

		return d_argb;
	}
	
	float	getAlpha(void) const	{return d_alpha;}
	float	getRed(void) const		{return d_red;}
	float	getGreen(void) const	{return d_green;}
	float	getBlue(void) const		{return d_blue;}

	float	getHue(void) const;
	float	getSaturation(void) const;
	float	getLumination(void) const;


	/*************************************************************************
		Manipulators
	*************************************************************************/
	void	setARGB(argb_t argb);
	inline void setAlpha(float alpha)
    {
        d_argbValid = false;
        d_alpha = alpha;
    }

	inline void setRed(float red)
    {   
        d_argbValid = false;
        d_red = red;
    }

	inline void setGreen(float green)
    {
        d_argbValid = false;
        d_green = green;
    }

	inline void setBlue(float blue)
    {
        d_argbValid = false;
        d_blue = blue;
    }

	inline void set(float red, float green, float blue, float alpha = 1.0f)
    {
        d_argbValid = false;
        d_alpha = alpha;
        d_red = red;
        d_green = green;
        d_blue = blue;
    }

	inline void setRGB(float red, float green, float blue)
    {
        d_argbValid = false;
        d_red = red;
        d_green = green;
        d_blue = blue;
    }

	inline void setRGB(const colour& val)
    {
        d_red = val.d_red;
        d_green = val.d_green;
        d_blue = val.d_blue;
        if (d_argbValid)
        {
            d_argbValid = val.d_argbValid;
            if (d_argbValid)
                d_argb = (d_argb & 0xFF000000) | (val.d_argb & 0x00FFFFFF);
        }
    }

	void	setHSL(float hue, float saturation, float luminance, float alpha = 1.0f);

	void	invertColour(void);
	void	invertColourWithAlpha(void);

	/*************************************************************************
		Operators
	*************************************************************************/
	inline colour& operator=(argb_t val)
    {
        setARGB(val);
        return *this;
    }

	inline colour& operator=(const colour& val)
    {
        d_alpha = val.d_alpha;
        d_red   = val.d_red;
        d_green = val.d_green;
        d_blue  = val.d_blue;
        d_argb  = val.d_argb;
        d_argbValid = val.d_argbValid;

        return *this;
    }

	inline colour& operator&=(argb_t val)
    {
        setARGB(getARGB() & val);
        return *this;
    }

	inline colour& operator&=(const colour& val)
    {
        setARGB(getARGB() & val.getARGB());
        return *this;
    }

	inline colour& operator|=(argb_t val)
    {
        setARGB(getARGB() | val);
        return *this;
    }

	inline colour& operator|=(const colour& val)
    {
        setARGB(getARGB() | val.getARGB());
        return *this;
    }

	inline colour& operator<<=(int val)
    {
        setARGB(getARGB() << val);
        return *this;
    }

	inline colour& operator>>=(int val)
    {
        setARGB(getARGB() >> val);
        return *this;
    }

	inline colour operator+(const colour& val) const
    {
        return colour(
            d_red   + val.d_red, 
            d_green + val.d_green, 
            d_blue  + val.d_blue,
            d_alpha + val.d_alpha
        );
    }

	inline colour operator-(const colour& val) const
    {
        return colour(
            d_red   - val.d_red,
            d_green - val.d_green,
            d_blue  - val.d_blue,
            d_alpha - val.d_alpha
        );
    }

	inline colour operator*(const float val) const
    {       
        return colour(
            d_red   * val, 
            d_green * val, 
            d_blue  * val,
            d_alpha * val 
        );  
    }

    inline colour& operator*=(const colour& val)
    {
        d_red *= val.d_red;
        d_blue *= val.d_blue;
        d_green *= val.d_green;
        d_alpha *= val.d_alpha;

		d_argbValid = false;

        return *this;
    }

	/*************************************************************************
		Compare operators
	*************************************************************************/
	inline bool operator==(const colour& rhs) const
    {
        return d_red   == rhs.d_red   &&
               d_green == rhs.d_green &&
               d_blue  == rhs.d_blue  &&
               d_alpha == rhs.d_alpha;
    }

	inline bool operator!=(const colour& rhs) const
    {
        return !(*this == rhs);
    }

	//
	// Conversion operators
	//
	operator argb_t() const		{return getARGB();}

private:
	/*************************************************************************
		Implementation Methods
	*************************************************************************/
	/*!
	\brief
		calculate and return the ARGB value based on the current colour component values.
	*/
	argb_t	calculateARGB(void) const;

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	float d_alpha, d_red, d_green, d_blue;		//!< Colour components.
	mutable argb_t d_argb;						//!< Colour as ARGB value.
	mutable bool d_argbValid;					//!< True if argb value is valid.
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIcolour_h_
