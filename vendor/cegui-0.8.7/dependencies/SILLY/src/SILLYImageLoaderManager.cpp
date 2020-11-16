/***********************************************************************
    filename:   ImageLoaderManager.cpp
    created:    10 Jun 2006
    author:     Olivier Delannoy 

    purpose:    Manage the list of existing ImageLoader  
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

#include "SILLYImageLoaderManager.h"

#ifndef SILLY_OPT_INLINE
#define inline 
#include "SILLYImageLoaderManager.icpp"
#undef inline
#endif 
#include "SILLYImageLoader.h"
#include "loaders/SILLYTGAImageLoader.h"

#ifdef SILLY_HAVE_JPG
#include "loaders/SILLYJPGImageLoader.h" 
#endif 

#ifdef SILLY_HAVE_PNG
#include "loaders/SILLYPNGImageLoader.h" 
#endif 

// Start of SILLY namespace section 
namespace SILLY
{

// we need to do "reference counting" for init/exit to allow silly to be used
// from inside multiple independent libraries simultaneously.
static size_t silly_init_counter = 0;

ImageLoaderManager* ImageLoaderManager::d_instance = 0;

ImageLoaderManager::ImageLoaderManager()
{
    assert(d_instance == 0);
    d_instance = this;
    add(new TGAImageLoader);
#ifdef SILLY_HAVE_JPG
    add(new JPGImageLoader);
#endif
#ifdef SILLY_HAVE_PNG
    add(new PNGImageLoader);
#endif 
// Add other builtins loader here 

}

ImageLoaderManager::~ImageLoaderManager()
{
    for(ImageLoaderList::iterator iter = d_loaders.begin() ; iter != d_loaders.end() ; ++iter) 
    {
        delete (*iter);
    }
    d_instance = 0;
}


bool SILLYInit()
{
    if (ImageLoaderManager::getSingletonPtr() == 0)
    {
        if (!new ImageLoaderManager)
        {
            return false;
        }
    }
    ++silly_init_counter;
    return true;
}

void SILLYCleanup()
{
    if (--silly_init_counter == 0)
    {
        delete ImageLoaderManager::getSingletonPtr();
    }
}

} // End of SILLY namespace section 


