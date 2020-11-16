/***********************************************************************
    filename:   SILLYPNGImageContext.cpp
    created:    11 Jun 2006
    author:     Olivier Delannoy 

    purpose:    Definition of PNGImageContext methods  
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

#include "loaders/SILLYPNGImageContext.h"

#ifndef SILLY_OPT_INLINE
#define inline 
#include "loaders/SILLYPNGImageContext.icpp"
#undef inline
#endif
#include <string.h>

// Start section of namespace SILLY
namespace SILLY
{
int PNGImageContext::read(png_bytep data, png_size_t length)
{
    //printf("PNG Read: %d bytes offset: %d, size %d\n", 
    //       length, d_offset, d_data->getSize());
    
    if (d_offset + length > d_data->getSize())
        return -1;
    memcpy(data, d_data->getDataPtr() + d_offset, length);
    d_offset += length;
    return length;
}

  
PNGImageContext::PNGImageContext(DataSource* data)
    : ImageContext(0,0), d_offset(0), d_data(data), d_png_ptr(0), d_info_ptr(0)
{
    
}

PNGImageContext::~PNGImageContext()
{
    if (d_info_ptr)
        png_destroy_read_struct(&d_png_ptr, &d_info_ptr, 0);    
    if (d_png_ptr)
        png_destroy_read_struct(&d_png_ptr, 0, 0);    
}


void PNGImageContext::setImageSize()
{
    setWidth(png_get_image_width(d_png_ptr, d_info_ptr));
    setHeight(png_get_image_height(d_png_ptr, d_info_ptr));
    
}


} // End section of namespace SILLY 
