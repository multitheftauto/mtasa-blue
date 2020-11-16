/***********************************************************************
    filename:   SILLYJPGImageContext.cpp
    created:    11 Jun 2006
    author:     Olivier Delannoy 

    purpose:    Definition of the JPGImageContext class methods 
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

#include "loaders/SILLYJPGImageContext.h"

#ifndef SILLY_OPT_INLINE
#define inline 
#include "loaders/SILLYJPGImageContext.icpp"
#undef inline
#endif

#include <jpeglib.h> 

// Start section of namespace SILLY
namespace SILLY
{


void JPG_init_source(j_decompress_ptr cinfo)
{
    // Nothing to do 
}

boolean JPG_fill_input_buffer(j_decompress_ptr cinfo)    
{
    JPGImageContext* jpg = reinterpret_cast<JPGImageContext*>(cinfo->client_data);
    cinfo->src->next_input_byte = jpg->d_source->getDataPtr();
    cinfo->src->bytes_in_buffer = jpg->d_source->getSize();
    return TRUE;
}

void JPG_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    if (num_bytes > 0)
    {
        cinfo->src->next_input_byte += (size_t)num_bytes;
        cinfo->src->bytes_in_buffer -= (size_t)num_bytes;
    }    
}

void JPG_term_source(j_decompress_ptr cinfo)    
{
    // Nothing to do 
}

void JPG_error_exit(j_common_ptr cinfo)
{
    JPGImageContext* jpg = reinterpret_cast<JPGImageContext*>(cinfo->client_data);
    // TODO 
    longjmp(jpg->setjmp_buffer, 1);
    
}

void JPG_emit_message(j_common_ptr cinfo, int msg_level)
{
    // Ignore 
}


JPGImageContext::JPGImageContext()
    : ImageContext(0, 0)
{
    src_mgr.bytes_in_buffer = 0;
    src_mgr.next_input_byte = 0;
    src_mgr.init_source = JPG_init_source;
    src_mgr.fill_input_buffer = JPG_fill_input_buffer;
    src_mgr.skip_input_data = JPG_skip_input_data;
    src_mgr.resync_to_restart = jpeg_resync_to_restart;
    src_mgr.term_source = JPG_term_source;
    jpeg_create_decompress(&cinfo);
    cinfo.src = &src_mgr;
    cinfo.client_data = this;
    cinfo.err = jpeg_std_error(&d_error_mgr);
    d_error_mgr.error_exit = JPG_error_exit;
    
}


JPGImageContext::~JPGImageContext()
{
    jpeg_destroy_decompress(&cinfo);
}


void JPGImageContext::setImageSize() 
{
    setWidth(cinfo.output_width);
    setHeight(cinfo.output_height);
}

} // End section of namespace SILLY 
