/***********************************************************************
    filename:   SILLYFileDataSource.cpp
    created:    20 Jul 2006
    author:     Olivier Delannoy 

    purpose:    Implementation of the FileDataSource methods  
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

#include "SILLYFileDataSource.h"

#ifndef SILLY_OPT_INLINE
#define inline 
#include "SILLYFileDataSource.icpp"
#undef inline
#endif 
#include <stdio.h> 
// Start section of namespace SILLY
namespace SILLY
{

FileDataSource::FileDataSource(const char* filename)
    : d_error(true), d_bytes(0), d_size(0)
{
    FILE* data = fopen(filename, "rb");
    if (data)
    {
        long status = fseek(data, 0, SEEK_END);
        if (status == -1)
        {
            fclose(data);
            return;
        }
        status  = ftell(data);
        if (status == -1)
        {
            fclose(data);
            return;
        }
        d_size = status;
        if (d_size)
            d_bytes = new byte[d_size];
        if (! d_bytes)
        {
            fclose(data);
            return;
        }
        rewind(data);
        status = fread(reinterpret_cast<void*>(d_bytes), 1, d_size, data);
        if (status != d_size)
        {
            delete [] d_bytes;
            d_bytes = 0;
            fclose(data);
        }
        fclose(data);
        d_error = false;        
    }
}


FileDataSource::~FileDataSource()
{
    delete [] d_bytes;
}


} // End section of namespace SILLY 
