/***********************************************************************
    filename:   SILLYPNGImageLoader.cpp
    created:    11 Jun 2006
    author:     Olivier Delannoy 

    purpose:    Definition of the PNGImageLoader methods  
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

#include "loaders/SILLYPNGImageLoader.h"

#ifndef SILLY_OPT_INLINE
#define inline 
#include "loaders/SILLYPNGImageLoader.icpp"
#undef inline
#endif

#include <cstring>
#include "loaders/SILLYPNGImageContext.h" 
#include <png.h>
// Start section of namespace SILLY
namespace SILLY
{
void PNG_read_function(png_structp png_ptr, png_bytep data, png_size_t length)
{
    PNGImageContext* png = reinterpret_cast<PNGImageContext*>(png_get_io_ptr(png_ptr));
    int readed = png->read(data, length);
    if (readed != (int)length)
    {
        png_error(png_ptr, "PNG_read_function error");
    }
}

void PNG_warning_function(png_structp png_ptr, 
                 png_const_charp error)
{
//    printf("PNG Warning: %s\n", error);
}

void PNG_error_function(png_structp png_ptr, 
                        png_const_charp error)
{
    //  printf("PNG Error: %s\n", error);
    // copied from libpng's pngerror.cpp
    jmp_buf buf;
    memcpy(buf, png_jmpbuf(png_ptr), sizeof(jmp_buf));
    longjmp(buf, 1);
}


PNGImageLoader::PNGImageLoader()
    : ImageLoader("PNG Image Loader based on libpng")
{
}
PNGImageLoader::~PNGImageLoader()
{
}


ImageContext* PNGImageLoader::loadHeader(PixelFormat& formatSource, DataSource* data)
{
    PNGImageContext* png = new PNGImageContext(data);
    if (!png)
    {    
        return 0;
        
    }
    // Prepare png loading 
    png->d_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png->d_png_ptr == 0)
    {
        delete png;
        return 0;
    }
    png->d_info_ptr = png_create_info_struct(png->d_png_ptr);
    if (png->d_info_ptr == 0)
    {
        delete png;
        return 0;
    }
    if (setjmp(png_jmpbuf(png->d_png_ptr))) 
    {
        delete png;
        return 0;
    }
    png_set_error_fn(png->d_png_ptr, 0, PNG_error_function, PNG_warning_function);
    png_set_read_fn(png->d_png_ptr, png, PNG_read_function);
    //png_set_sig_bytes(png->d_png_ptr, 8);
    


    // Read header Check whether PNG can depaletize transparently or not
    int png_transform = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND;
    //printf("Start reading png\n");
    png_read_png(png->d_png_ptr, png->d_info_ptr, png_transform, 0);
    png->setImageSize();
    png->d_bit_depth = png_get_bit_depth(png->d_png_ptr, png->d_info_ptr);
    png->d_num_channels = png_get_channels(png->d_png_ptr, png->d_info_ptr);
    //printf("PNG Info: width: %d height: %d bpp: %d channels: %d\n", png->getWidth(), png->getHeight(), png->d_bit_depth, png->d_num_channels);
    if (png->d_bit_depth == 8)
    {
        if (png->d_num_channels == 4)
        {    
            formatSource = PF_RGBA;
        }
        else if (png->d_num_channels == 3)
        {   
            formatSource = PF_RGB;
        }
        else 
        {
            delete png;
            return 0;
        }
    }
    // Paletized or grayscale not yet handled 
    else 
    {
        delete png;
        return 0;
    }
    return png;
}

 
bool PNGImageLoader::loadImageData(PixelOrigin origin, 
                                   DataSource* data, 
                                   ImageContext* context)
{
    PNGImageContext* png = static_cast<PNGImageContext*>(context);
    byte red;
    byte green;
    byte blue;
    byte alpha;
    size_t width = png->getWidth();
    size_t height = png->getHeight();
    png_bytepp row_pointers = png_get_rows(png->d_png_ptr, png->d_info_ptr);
    if (png->d_bit_depth == 8)
    {
        // Read RGBA 
        if (png->d_num_channels == 4)
        {
            for (size_t j = 0 ; j < height ; ++j)
            {
                for(size_t i = 0 ; i < width ; ++i)
                {
                    size_t pixel_offset = 4 * i;
                    red   = *(row_pointers[j] + pixel_offset);
                    green = *(row_pointers[j] + pixel_offset + 1);
                    blue  = *(row_pointers[j] + pixel_offset + 2);
                    alpha = *(row_pointers[j] + pixel_offset + 3);
                    png->setNextPixel(red, green, blue, alpha);
                }
            }
        }
        else if (png->d_num_channels == 3)
        {
            alpha = 0xff;
            for (size_t j = 0 ; j < height ; ++j)
            {
                for(size_t i = 0 ; i < width ; ++i)
                {
                    size_t pixel_offset = 3 * i;
                    red   = *(row_pointers[j] + pixel_offset);
                    green = *(row_pointers[j] + pixel_offset + 1);
                    blue  = *(row_pointers[j] + pixel_offset + 2);
                    png->setNextPixel(red, green, blue, alpha);
                }
            }

        }
    }
    if (origin == PO_BOTTOM_LEFT)
        return png->flipVertically();
    
    return true;    
}

} // End section of namespace SILLY 
