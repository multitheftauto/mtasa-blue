/***********************************************************************
    filename:   SILLYImageContext.h
    created:    10 Jun 2006
    author:     Olivier Delannoy

    purpose:    ImageContext is an abstract class used to hide the loader specific data needed during the parsing of an image 
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
#ifndef _SILLYImageContext_h_ 
#define _SILLYImageContext_h_
#include "SILLYBase.h" 

// Start of section namespace SILLY 
namespace SILLY
{

/*! 
  \brief
    Store the data needed by an ImageLoader object during the parsing of an image 
*/
  
class SILLY_EXPORT ImageContext
{
public:
    /*!
      \brief 
      Destructor 
    */
    virtual ~ImageContext();
    
    /*!
      \brief 
      Set the destination of the loading 
    */
    void setDestination(byte* pixels, size_t length, PixelFormat format);
    
    /*! 
      \brief 
      Set the next pixel of the image 
    */
    void setNextPixel(byte red, byte green, byte bleu, byte alpha);

    size_t getWidth() const;
    size_t getHeight() const;
    PixelFormat getPixelFormat() const;
    
    /*!
      \brief 
      Flip pixel ordering 

      \return true on success false otherwise 
    */
    bool  flipVertically();
 protected:
    void setWidth(size_t width);
    
    void setHeight(size_t height);
      
    /*! 
      \brief 
      Constructor

      \param width the width of the image 

      \param height the height of the image 
    */
    ImageContext(size_t width, size_t height);

private:    
    byte* d_pixels;
    size_t d_length;
    size_t d_width;
    size_t d_height;
    size_t d_currentOffset;
    PixelFormat d_format;

    // Disabled operation
    ImageContext(ImageContext&);
    ImageContext& operator=(ImageContext&);
};
  
  
} // End of section namespace SILLY 

// Inclue inline function when needed 
#ifdef SILLY_OPT_INLINE
#include "SILLYImageContext.icpp"
#endif 

#endif // end of guard _SILLYImageContext_h_
