/***********************************************************************
    filename:   SILLYJPGImageLoader.cpp
    created:    11 Jun 2006
    author:     Olivier Delannoy 

    purpose:    Definition of JPGImageLoader methods  
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

#include "loaders/SILLYJPGImageLoader.h"

#ifndef SILLY_OPT_INLINE
#define inline 
#include "loaders/SILLYJPGImageLoader.icpp"
#undef inline
#endif
#include "loaders/SILLYJPGImageContext.h" 

// Start section of namespace SILLY
namespace SILLY
{
JPGImageLoader::JPGImageLoader()
    : ImageLoader("JPG Image Loader based on jpeg-6b")
{
}

JPGImageLoader::~JPGImageLoader()
{
}


ImageContext* JPGImageLoader::loadHeader(PixelFormat& formatSource, DataSource* data)
{
    JPGImageContext* jpg = new JPGImageContext;
    jpg->d_source = data;
    
    if (! jpg)
        return 0;
    if (setjmp(jpg->setjmp_buffer))
    {
        delete jpg;
        return 0;
    }
    jpeg_read_header(&(jpg->cinfo), TRUE);
    if (! jpeg_start_decompress(&(jpg->cinfo)))
    {
        delete jpg;
        return 0;
    }
        
    
    if (jpg->cinfo.output_components != 1 && jpg->cinfo.output_components != 3)
    {
        printf("JPG unsupported bpp: %d\n", jpg->cinfo.output_components );
        jpeg_finish_decompress(& jpg->cinfo);
        delete jpg;
        return 0;
    }
    formatSource  = PF_RGB; // It's not true but we handle both the same way
    //printf("Image info: size: %dx%d - bpp: %d\n",  jpg->cinfo.output_width, jpg->cinfo.output_height, jpg->cinfo.output_components);
    
    jpg->setImageSize();
    return jpg;
}

bool JPGImageLoader::loadImageData(PixelOrigin origin, 
                                   DataSource* data, 
                                   ImageContext* context)
{
    JPGImageContext* jpg = static_cast<JPGImageContext*>(context);
    
    // Allocate a buffer 
    int row_stride = jpg->getWidth() * jpg->cinfo.output_components;
    JSAMPARRAY buffer = (* jpg->cinfo.mem->alloc_sarray)(
        (j_common_ptr)(& jpg->cinfo), 
        JPOOL_IMAGE, 
        row_stride, 
        1);
    bool finished = true;
    size_t height = jpg->getHeight();
    size_t width = jpg->getWidth();
    
    
    while(jpg->cinfo.output_scanline < height)
    {
        int num_rows = jpeg_read_scanlines(& jpg->cinfo, buffer, 1);
        if (num_rows != 1)
        {
            jpeg_finish_decompress(& jpg->cinfo);
            return false;
        }
        byte red;
        byte green;
        byte blue;
        const byte alpha = 0xff;
        byte* in = (byte*)(*buffer);
        // Grayscale image 
        if (jpg->cinfo.output_components == 1)
        {
            for(size_t i  = 0 ; i < width * num_rows ; ++i)
            {
                red = *in;
                green = *in;
                blue = *in;
                jpg->setNextPixel(red, green, blue, alpha);
                in += 1;
            }
        }
        // RGB image 
        else 
        {
            for(size_t i  = 0 ; i < width * num_rows ; ++i)
            {
                red = *(in++);
                green = *(in++);
                blue = *(in++);
                jpg->setNextPixel(red, green, blue, alpha);
            }
        }
    }
    jpeg_finish_decompress(& jpg->cinfo);
    
    if (origin == PO_BOTTOM_LEFT)
        return jpg->flipVertically();
    return true;
}


                                         

 

} // End section of namespace SILLY 
