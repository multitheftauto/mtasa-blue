/***********************************************************************
	created:	20/8/2004
	author:		Paul D Turner (with code from Jeff Leigh)
	
	purpose:	Implementation of colour class methods.
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
#include "CEGUI/Colour.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Construction & Destruction
*************************************************************************/
Colour::Colour(void) :
	d_alpha(1.0f),
	d_red(0.0f),
	d_green(0.0f),
	d_blue(0.0f),
	d_argb(0xFF000000),
	d_argbValid(true)
{
}


Colour::Colour(const Colour& val)
{
	this->operator=(val);
}


Colour::Colour(float red, float green, float blue, float alpha) :
	d_alpha(alpha),
	d_red(red),
	d_green(green),
	d_blue(blue),
	d_argbValid(false)
{
}


Colour::Colour(argb_t argb)
{
	setARGB(argb);
}


float Colour::getHue(void) const
{
	float pRed = d_red;
	float pGreen = d_green;
	float pBlue = d_blue;

	float pMax = ceguimax(ceguimax(d_red, d_green), d_blue);
	float pMin = ceguimin(ceguimin(d_red, d_green), d_blue);

	float pHue;

	if( pMax == pMin )
	{
		pHue = 0;
	}
	else
	{
		if( pMax == pRed )
		{
			pHue = (pGreen - pBlue) / (pMax - pMin);
		}
		else if( pMax == pGreen )
		{
			pHue = 2 + (pBlue - pRed) / (pMax - pMin);
		}
		else
		{
			pHue = 4 + (pRed - pGreen) / (pMax - pMin);
		}
	}

	float Hue = pHue / 6;
	if( Hue < 0 )
		Hue += 1;

	return Hue;
}


float Colour::getSaturation(void) const
{
	float pMax = ceguimax(ceguimax(d_red, d_green), d_blue);
	float pMin = ceguimin(ceguimin(d_red, d_green), d_blue);

	float pLum = (pMax + pMin) / 2;
	float pSat;

	if( pMax == pMin )
	{
		pSat = 0;
	}
	else
	{
		if( pLum < 0.5 )
			pSat = (pMax - pMin) / (pMax + pMin);
		else
			pSat = (pMax - pMin) / (2 - pMax - pMin);
	}

	return pSat;
}


float Colour::getLumination(void) const
{
	float pMax = ceguimax(ceguimax(d_red, d_green), d_blue);
	float pMin = ceguimin(ceguimin(d_red, d_green), d_blue);

	float pLum = (pMax + pMin) / 2;
	return pLum;
}


void Colour::setARGB(argb_t argb)
{
	d_argb = argb;

	d_blue	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	d_green	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	d_red	= static_cast<float>(argb & 0xFF) / 255.0f;
	argb >>= 8;
	d_alpha	= static_cast<float>(argb & 0xFF) / 255.0f;

	d_argbValid = true;
}


void Colour::setHSL(float hue, float saturation, float luminance, float alpha)
{
	d_alpha = alpha;

	float temp3[3];

	float pHue = hue;
	float pSat = saturation;
	float pLum = luminance;

	if( pSat == 0 )
	{
		d_red = pLum;
		d_green = pLum;
		d_blue = pLum;
	}
	else
	{
		float temp2;
		if( pLum < 0.5f )
		{
			temp2 = pLum * (1 + pSat);
		}
		else
		{
			temp2 = pLum + pSat - pLum * pSat;
		}

		float temp1 = 2 * pLum - temp2;

		temp3[0] = pHue + (1.0f / 3);
		temp3[1] = pHue;
		temp3[2] = pHue - (1.0f / 3);

		for( int n = 0; n < 3; n ++ )
		{
			if( temp3[n] < 0 )
				temp3[n] ++;
			if( temp3[n] > 1 )
				temp3[n] --;

			if( (temp3[n] * 6) < 1 )
			{
				temp3[n] = temp1 + (temp2 - temp1) * 6 * temp3[n];
			}
			else
			{
				if( (temp3[n] * 2) < 1 )
				{
					temp3[n] = temp2;
				}
				else
				{
					if( (temp3[n] * 3) < 2 )
					{
						temp3[n] = temp1 + (temp2 - temp1) * ((2.0f / 3) - temp3[n]) * 6;
					}
					else
					{
						temp3[n] = temp1;
					}
				}
			}
		}

		d_red = temp3[0];
		d_green = temp3[1];
		d_blue = temp3[2];
	}

	d_argbValid = false;
}


argb_t Colour::calculateARGB(void) const
{
    return (
		static_cast<argb_t>(d_alpha * 255) << 24 |
		static_cast<argb_t>(d_red * 255) << 16 |
		static_cast<argb_t>(d_green * 255) << 8 |
		static_cast<argb_t>(d_blue * 255)
	);
}


void Colour::invertColour(void)
{
	d_red	= 1.0f - d_red;
	d_green	= 1.0f - d_green;
	d_blue	= 1.0f - d_blue;
}


void Colour::invertColourWithAlpha(void)
{
	d_alpha	= 1.0f - d_alpha;
	d_red	= 1.0f - d_red;
	d_green	= 1.0f - d_green;
	d_blue	= 1.0f - d_blue;
}

} // End of  CEGUI namespace section
