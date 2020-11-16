/***********************************************************************
    filename:   SILLYDataSource.h
    created:    10 Jun 2006
    author:     Olivier Delannoy 

    purpose:    SILLY::DataSource declaration for the SILLY library 
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
#ifndef _SILLYDataSource_h_ 
#define _SILLYDataSource_h_
#include "SILLYBase.h" 

// Start of section namespace SILLY 
namespace SILLY
{

/*! 
  \brief 
  This is an abstract class used to provide data to the loader 
  
  This class defines abstract data loading operation required for 
  the library. 
  
*/
class SILLY_EXPORT DataSource
{
public:
  /*! 
    \brief 
    Destructor 
  */
  virtual ~DataSource();
  /*! 
    \brief 
    Get raw access to the image data 
  */
  virtual const byte * getDataPtr() const = 0;
  /*! 
    \brief 
    Retieve the byte at @em offset 
    
  */
  byte operator[] (size_t offset) const;
  /*! 
    \brief 
    Return the size of the data 
  */
  virtual size_t getSize() const = 0;
};
  
  
} // End of section namespace SILLY 

// Inclue inline function when needed 
#ifdef SILLY_OPT_INLINE
#include "SILLYDataSource.icpp"
#endif 

#endif // end of guard _SILLYDataSource_h_
