/***********************************************************************
    filename:   SILLYImage.cpp
    created:    10 Jun 2006
    author:     Olivier Delannoy 

    purpose:    Implementation of the Image class  
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

#include "SILLYImage.h"

#ifndef SILLY_OPT_INLINE
#define inline 
#include "SILLYImage.icpp"
#undef inline
#endif 
#include "SILLYImageLoaderManager.h"


// Start section of namespace SILLY
namespace SILLY
{

Image::Image(DataSource& source)
  :  d_bpp(0), d_pixels(0), d_data(&source), d_imageContext(0), d_imageLoader(0)
{
}


bool Image::loadImageHeader()
{
    ImageLoaderList::iterator iter = ImageLoaderManager::getSingleton().begin();
    for (; ! d_imageLoader && iter != ImageLoaderManager::getSingleton().end() ; ++iter)
    {
        d_imageContext = (*iter)->loadHeader(d_pfSource, d_data);
        if (d_imageContext)
            d_imageLoader = (*iter);
    }
    assert((! d_imageLoader || d_imageContext) && "ASSERT: Internal state of image invalid");
    return d_imageLoader != 0;
    
}

bool Image::loadImageData(PixelFormat resultFormat, PixelOrigin order)
{
    switch (resultFormat)
    {
    case PF_A1B5G5R5:
        d_bpp = 2;
        break;
    case PF_RGB:
        d_bpp = 3;
        break;

    case PF_RGBA:    
        d_bpp = 4;
        break;
        //default:
        // Unsupported format 
    };

    if (! allocate())
    {
        return false;
    }
    d_imageContext->setDestination(d_pixels, getWidth() * getHeight() * d_bpp, resultFormat);

    if (! d_imageLoader->loadImageData(order, d_data, d_imageContext))
    {
        delete [] d_pixels;
        return false;
    }
    return true;
}

bool Image::allocate()
{
    delete [] d_pixels;
    d_pixels = 0;
    d_pixels = new byte[d_bpp * getWidth() * getHeight()];
    return d_pixels != 0;
}

} // End section of namespace SILLY 
