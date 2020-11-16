/***********************************************************************
    filename:   SILLYJPGImageContext.h
    created:    11 Jun 2006
    author:     Olivier Delannoy

    purpose:    Declaration of the JPGImageContext class 
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
#ifndef _SILLYJPGImageContext_h_ 
#define _SILLYJPGImageContext_h_
#include "SILLYBase.h" 
#include "SILLYImageContext.h" 
#include "SILLYDataSource.h" 
// Start of section namespace SILLY 
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
extern "C" 
{
#include <jpeglib.h>
}
namespace SILLY
{

/*! 
  \brief
    Image Context for JPG image loader

    Most of the code of this loader is taken from Corona and adapted
    to our image loading framework
 
    Multiple loading is not handle yet. 
*/
  
class JPGImageContext : public ImageContext
{
public:
    JPGImageContext();
    ~JPGImageContext();

    void setImageSize();
    
    
    jpeg_source_mgr src_mgr;
    jpeg_decompress_struct cinfo;
    
    jpeg_error_mgr d_error_mgr;
    jmp_buf setjmp_buffer;
    DataSource* d_source;
};
  
  
} // End of section namespace SILLY 

// Inclue inline function when needed 
#ifdef SILLY_OPT_INLINE
#include "SILLYJPGImageContext.icpp"
#endif 

#endif // end of guard _SILLYJPGImageContext_h_
