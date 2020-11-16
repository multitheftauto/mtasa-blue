/***********************************************************************
    filename:   SILLYBase.h
    created:    2006/06/10
    author:     Olivier Delannoy

    purpose:    Common header for the SILLY library 
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
#ifndef _SILLYBase_h_ 
#define _SILLYBase_h_ 
#include <cassert>
#include <cstddef>

#if defined(USE_SILLYOPTIONS_H)
#include "SILLYOptions.h"
#endif

#if defined(_WIN32) || defined(__WIN32__)
#   undef SILLY_OPT_INLINE // No inlining 
#   undef SILLY_BE // Little Endian 
#	ifdef SILLY_STATIC
#		define SILLY_EXPORT
#	else
#		ifdef SILLY_EXPORTS
#			define SILLY_EXPORT __declspec(dllexport)
#		else
#			define SILLY_EXPORT __declspec(dllimport)
#		endif
#	endif
#else
#   define SILLY_EXPORT
#endif

// Start of section namespace SILLY 
namespace SILLY
{

/*! 
  \brief 
  List all pixel format supported 
 */
enum PixelFormat
{
    PF_A1B5G5R5,    //<! 16 bits, pixel format is A1R5G5B5 (GGGBBBBB,ARRRRRGG on disk)
    PF_RGB,         //<! 24 bits, pixel format is 8 bits for R, G, B 
    PF_RGBA,        //<! 32 bits, pixel format is 8 bits for R, G, B and A channel     
    PF_ORIGIN,      //<! This format is used when the input storage use a specific pixel format 
};
/*! 
  \brief 
  List all pixel origin supported 
*/
enum PixelOrigin 
{
    PO_TOP_LEFT,     //<! The resulting image origine is Top Left
    PO_BOTTOM_LEFT,  //<! The resulting image origine is Bottom Left
};

/*!
  \brief 
  Typename for a byte 
*/
typedef unsigned char byte;
/*! 
  \brief 
  Initialize the SILLY Library 

  \return 
  True if the initialization was successfull false otherwise.
*/ 
bool SILLY_EXPORT SILLYInit();

/*! 
  \brief 
  Cleanup SILLY library internal 
*/ 
void SILLY_EXPORT SILLYCleanup();

} // End of section namespace SILLY 

#endif // end of guard _SILLYBase_h_

