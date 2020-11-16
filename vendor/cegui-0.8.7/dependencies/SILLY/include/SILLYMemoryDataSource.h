/***********************************************************************
    filename:   SILLYMemoryDataSource.h
    created:    10 Jun 2006
    author:     Olivier Delannoy 

    purpose:    SILLYMemoryDataSource declaration for the SILLY library 
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
#ifndef _SILLYMemoryDataSource_h_ 
#define _SILLYMemoryDataSource_h_
#include "SILLYBase.h" 
#include "SILLYDataSource.h" 
// Start of section namespace SILLY 
namespace SILLY
{

/*! 
  \brief 
  Load an image from a memory area.
  
  This class provides the services required to load an image from a
  byte array. The object does not change the memory area and does no
  take owner chip of the data.
 
*/
  
class SILLY_EXPORT MemoryDataSource : public DataSource 
{
public:
    /*!
      \brief
      Create a data source from an existing memory area.

      The MemoryDataSource does not take the ownership of the memory 
      pointed by data. 

      \param data a byte array of size \em size 
      
      \param size the size of data 
    */
      
    MemoryDataSource(const byte* data, size_t size);


    const byte* getDataPtr() const;

    size_t getSize() const;
private:
    const byte* d_bytes;               //!< a pointer to the byte array
    size_t d_size;              //!< the size of the memory area
};
  
  
} // End of section namespace SILLY 

// Inclue inline function when needed 
#ifdef SILLY_OPT_INLINE
#include "SILLYMemoryDataSource.icpp"
#endif 

#endif // end of guard _SILLYMemoryDataSource_h_
