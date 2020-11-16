/***********************************************************************
    filename:   SILLYFileDataSource.h
    created:    20 Jul 2006
    author:     Olivier Delannoy

    purpose:    Provide file based image loading 
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
#ifndef _SILLYFileDataSource_h_ 
#define _SILLYFileDataSource_h_
#include "SILLYBase.h" 
#include "SILLYDataSource.h" 
// Start of section namespace SILLY 
namespace SILLY
{

/*! 
  \brief
    This class allow the loading of an image directly from a file

    This class provides the services required to load an from a file.
    The loading is done once at initialization. We wanted to avoid
    exception. This is why user must check whether the object is valid
    or not after creation. 
*/
  
class SILLY_EXPORT FileDataSource : public DataSource
{
public:
    /*!
      \brief 
      Create a data source from an existing file.
      
      The FileDataSource manage the loading and the caching of the raw image source.
      The object manage the life time of the data. 
      
      \param filename the name of the file containing the data 
    */
    FileDataSource(const char* filename);
    /*!
      \brief destructor 
    */
    ~FileDataSource();
    /*!
      \brief Check wether the loading is successfull or not 
      
      One must call this function after creating this object in order
      to be sure the loading was successfull.

      \return true if the loading is successfull.
    */
    bool isValid() const;

    const byte* getDataPtr() const;
    size_t getSize() const;
private:
    bool d_error;  //!< Store wether the loading was successfull or not
    byte* d_bytes; //!< Store a pointer to the byte array storing the data 
    size_t d_size; //!< number of bytes
};
  
  
} // End of section namespace SILLY 

// Inclue inline function when needed 
#ifdef SILLY_OPT_INLINE
#include "SILLYFileDataSource.icpp"
#endif 

#endif // end of guard _SILLYFileDataSource_h_
