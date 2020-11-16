
/***********************************************************************
    filename:   SILLYImage.h
    created:    10 Jun 2006
    author:     Olivier Delannoy

    purpose:    Image class definition
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
#ifndef _SILLYImage_h_ 
#define _SILLYImage_h_
#include "SILLYBase.h" 
#include "SILLYImageContext.h"
#include "SILLYDataSource.h"
#include "SILLYImageLoader.h" 

// Start of section namespace SILLY 
namespace SILLY
{

/*! 
  \brief
    Image is the main user class of the library 
*/
class SILLY_EXPORT Image
{
public:
    /*! 
      \brief 
      Constructor 
      
      \param data the raw input containing the image 
    */
    Image(DataSource& data);

    /*!
      \brief 
      Destructor 
    */
    ~Image();

    /*! 
      \brief 
      Return true if the image is valid after its loading 
    */
    bool isValid() const;
    

    /*! 
      \brief 
      Retrieve the information concerning the image object 
      
      \return true on success 
    */
    bool loadImageHeader();

    /*!
      \brief 
      Load the image pixels in memory and store them in \em resultFormat 
      
      \param resultFormat the pixel format to be used for storing the result 

      \param origin expected origin position for the pixels 
      
      \return true on success false if an error occured 
      
      \note this function can be called several time if the pixel
      format are different the format is reloaded 
    */
    bool loadImageData(PixelFormat resultFormat = PF_RGBA, PixelOrigin origin = PO_TOP_LEFT);

    /*! 
      \brief 
      Retrieve the width of the image 
    */
    size_t getWidth() const;
    
    /*!
      \brief 
      Retrieve the height of the image 
    */
    size_t getHeight() const;
    
    /*!
      \brief 
      Retrieve the pixel format used for the image storage 
    */
    PixelFormat getSourcePixelFormat() const;

    /*! 
      \brief 
      Retrieve the pixel format of the resulting image 
    */
    PixelFormat getPixelFormat() const;
    
    /*! 
      \brief Get a pointer to the pixels data 
    */
    const byte* getPixelsDataPtr() const;
    /*! 
      \brief 
      Get the size of the pixel buffer 
    */
    size_t getPixelsDataSize() const;
    /*! 
      \brief 
      Get Image loader identifier string 
    */
    const char* getLoaderIdentifierString() const;
private:
    bool allocate();

private:

    size_t d_bpp;		          //!< Store the number of byte per pixel used 
    PixelFormat d_pfSource;       //!< Store the pixel format in the source image 
    byte* d_pixels;		          //!< Store the pixel of the resulting image
    DataSource* d_data;           //!< Store a pointer to the input data
    ImageContext* d_imageContext; //!< Store a pointer to the image context 
    ImageLoader* d_imageLoader;   //!< Store a pointer to the image loader used

    // Disabled operation
    Image(Image&);
    Image& operator=(Image&);
};

} // End of section namespace SILLY 

// Inclue inline function when needed 
#ifdef SILLY_OPT_INLINE
#include "SILLYImage.icpp"
#endif 

#endif // end of guard _SILLYImage_h_
