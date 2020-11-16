/***********************************************************************
    filename:   SILLYTGAImageLoader.cpp
    created:    11 Jun 2006
    author:     Olivier Delannoy 

    purpose:    Load TGA Image  
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

#include "loaders/SILLYTGAImageLoader.h"

#ifndef SILLY_OPT_INLINE
#define inline 
#include "loaders/SILLYTGAImageLoader.icpp"
#undef inline
#endif 
#include "SILLYDataSource.h" 
#include "loaders/SILLYTGAImageContext.h" 

#include <cstring> 
#include <cstdio>

// Start section of namespace SILLY
namespace SILLY
{

TGAImageLoader::TGAImageLoader()
    : ImageLoader("TGA Image loader (builtin)")
{
}

TGAImageLoader::~TGAImageLoader()
{
}

#ifdef SILLY_BE
#define READ_WORD(res, start)                   \
    (res) = 0;                                  \
    (res) = data->getDataPtr()[(start) + 1];               \
    (res) = (res) << 8;                         \
    (res) |= data->getDataPtr()[(start)];
#else 
#define READ_WORD(res, start)                   \
    (res) = 0;                                  \
    (res) = data->getDataPtr()[(start)];                   \
    (res) = (res) << 8;                          \
    (res) |= data->getDataPtr()[(start) + 1];
#endif 

ImageContext* TGAImageLoader::loadHeader(PixelFormat& formatSource, DataSource* data)
{
    byte idLength = data->getDataPtr()[0];
    byte colorMapType = data->getDataPtr()[1];
    byte imageType = data->getDataPtr()[2];
    size_t width;
    size_t height;
    byte depth;
    byte description;
    if ((imageType != 2 && imageType != 10) || colorMapType)
    {
        // Unsupported image format
        return 0;
    }
    // offset: 3 Skip color map + 5 bytes 
    // offset: 8 Skip xorg / yorg + 4 bytes 
#ifdef SILLY_BE 
    width = data->getDataPtr()[12];
    width = width << 8;
    width |= data->getDataPtr()[13];
    height =  data->getDataPtr()[14];
    height = height << 8;
    height |= data->getDataPtr()[15];
#else 
    width = data->getDataPtr()[13];
    width = width << 8;
    width |= data->getDataPtr()[12];
    height = data->getDataPtr()[15];
    height = height << 8;
    height |= data->getDataPtr()[14];
#endif 
    depth = data->getDataPtr()[16] >> 3;
    switch (depth)
    {
    case 2:
        formatSource = PF_A1B5G5R5;
        break;
    case 3:
        formatSource = PF_RGB;
        break;
    case 4:
        formatSource = PF_RGBA;
        break;
    default:
        return 0;
    }
    description = (*data)[17];


    TGAImageContext* context = new TGAImageContext(width, height);
    if (context)
    {
        context->d_idLength = idLength;
        context->d_imageType = imageType;
        context->d_depth = depth;
        context->d_description = description;
    }
    return context;
}

bool TGAImageLoader::loadImageData(PixelOrigin origin, DataSource* data, ImageContext* context)
{
    TGAImageContext* tga = static_cast<TGAImageContext*>(context);
    size_t bpp = tga->d_depth;
    size_t w = tga->getWidth();
    size_t h = tga->getHeight();    
    size_t imgSize = w * h * bpp;
    size_t offset = 18 + tga->d_idLength;
    size_t numPixels = w * h;
    const byte* input = data->getDataPtr() + offset;
    // Read image data 
    byte red;
    byte green;
    byte blue;
    byte alpha;    
    // Uncompressed 
    if (tga->d_imageType == 2)
    {
        
        switch(bpp)
        {
        case 2: //A1B5G5R5
            for(size_t i = 0 ; i < numPixels ; ++i)
            {
                unsigned short pixel;
                pixel = *(input++);
                pixel = pixel << 8;
                pixel |= *(input++);
                
                alpha = pixel & 0xf000 ? 0xff : 0x00;
                blue = static_cast<byte>((pixel & 0x1f) << 3);
				green = static_cast<byte>(((pixel >> 5) & 0x1f) << 3);
				red = static_cast<byte>(((pixel >> 10) & 0x1f) << 3);
                context->setNextPixel(red, green, blue, alpha);
            }
            break;
            
        case 3: // BGR_24
            alpha = 0xff;
            for(size_t i = 0 ; i < numPixels ; ++i)
            {
                blue = *(input++);
                green = *(input++);
                red = *(input++);
                context->setNextPixel(red, green, blue, alpha);
            }
            break;
            
        case 4: // BGRA_32
            alpha = 0xff;
            for(size_t i = 0 ; i < numPixels ; ++i)
            {
                blue = *(input++);
                green = *(input++);
                red = *(input++);
                alpha = *(input++);
                context->setNextPixel(red, green, blue, alpha);
            }
            break;
        }
    }
    else 
    {
        size_t pixelsRead = 0;
        size_t num, i;
        byte header;
        
        switch(bpp)
        {
        case 2: //A1B5G5R5
            while(pixelsRead < numPixels)
            {
                header = *(input++);
                num  = 1 + (header & 0x7f);
                if (header & 0x80)
                {
                    unsigned short pixel;
                    pixel = *(input++);
                    pixel = pixel << 8;
                    pixel |= *(input++);
                    
                    alpha = pixel & 0xf000 ? 0xff : 0x00;
                    blue = static_cast<byte>((pixel & 0x1f) << 3);
                    green = static_cast<byte>(((pixel >> 5) & 0x1f) << 3);
                    red = static_cast<byte>(((pixel >> 10) & 0x1f) << 3);
                    for(i = 0 ; i < num ; ++i)
                    {
                        context->setNextPixel(red, green, blue, alpha);
                    }
                }
                else 
                {
                    for (i = 0 ; i < num ; ++i)
                    {
                        unsigned short pixel;
                        pixel = *(input++);
                        pixel = pixel << 8;
                        pixel |= *(input++);    
                        alpha = pixel & 0xf000 ? 0xff : 0x00;
                        blue = static_cast<byte>((pixel & 0x1f) << 3);
                        green = static_cast<byte>(((pixel >> 5) & 0x1f) << 3);
                        red = static_cast<byte>(((pixel >> 10) & 0x1f) << 3);
                        context->setNextPixel(red, green, blue, alpha);
                    }
                }
                pixelsRead += num;
            }
            
            break;
        case 3:
            alpha = 0xff;
            while(pixelsRead < numPixels)
            {
                header = *(input++);
                num  = 1 + (header & 0x7f);
                if (header & 0x80)
                {
                    blue = *(input++);
                    green = *(input++);
                    red = *(input++);
                    for(i = 0 ; i < num ; ++i)
                    {
                        context->setNextPixel(red, green, blue, alpha);
                    }
                }
                else 
                {
                    for (i = 0 ; i < num ; ++i)
                    {
                        blue = *(input++);
                        green = *(input++);
                        red = *(input++);
                        context->setNextPixel(red, green, blue, alpha);
                    }
                }
                pixelsRead += num;
            }
            break;
            
        case 4:
            while(pixelsRead < numPixels)
            {
                header = *(input++);
                num  = 1 + (header & 0x7f);
                if (header & 0x80)
                {
                    blue = *(input++);
                    green = *(input++);
                    red = *(input++);
                    alpha = *(input++);
                    for(i = 0 ; i < num ; ++i)
                    {
                        context->setNextPixel(red, green, blue, alpha);
                    }
                }
                else 
                {
                    for (i = 0 ; i < num ; ++i)
                    {
                        blue = *(input++);
                        green = *(input++);
                        red = *(input++);
                        alpha = *(input++);
                        context->setNextPixel(red, green, blue, alpha);
                    }
                }
                pixelsRead += num;
            }    
            break;
        }    
    }
    // Flip or not flip that is the question 
    if ((tga->d_description & 0x20) == 0x20) // Upper Left origin 
    {
        if  (origin == PO_BOTTOM_LEFT)
            return tga->flipVertically();
    }
    else 
    {
        if (origin == PO_TOP_LEFT)
            return tga->flipVertically();
    }
    return true;
}

} // End section of namespace SILLY 
