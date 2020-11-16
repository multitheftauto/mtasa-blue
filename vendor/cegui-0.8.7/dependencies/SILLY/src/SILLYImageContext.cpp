/***********************************************************************
    filename:   SILLYImageContext.cpp
    created:    10 Jun 2006
    author:     Olivier Delannoy 

    purpose:    Implementation file for ImageContext class  
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SILLYImageContext.h"
#ifndef SILLY_OPT_INLINE
#define inline 
#include "SILLYImageContext.icpp"
#undef inline
#endif
#include <cstring>
// Start section of namespace SILLY
namespace SILLY
{ 

bool ImageContext::flipVertically()
{
    size_t depth;
    switch(d_format)
    {
    case PF_A1B5G5R5:
        depth = 2;
        break;
    case PF_RGB:
        depth = 3;
        break;
    case PF_RGBA:
        depth = 4;
        break;
    }
       
    // Do the flipping 
    byte *p1,*p2,*tmp;      // tmp pointers
    size_t len = d_width * depth; // length of horizontal line in bytes
    tmp = new byte[len];    // Allocate a temporary line
    if ( !tmp )
        return false;
    
    size_t y , y2;
    y2 = d_height - 1;
    for ( y = 0; y < d_height >> 1; y++ )
    {
        p1 = (d_pixels) + len * y;
        p2 = (d_pixels) + len * y2;
        memcpy( tmp, p1, len );
        memcpy( p1, p2, len );
        memcpy( p2, tmp, len );
        y2--;
    }
    delete [] tmp;
    return true;
}


} // End section of namespace SILLY 
