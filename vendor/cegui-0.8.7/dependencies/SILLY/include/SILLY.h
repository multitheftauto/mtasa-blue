/***********************************************************************
    filename:   SILLY.h
    created:    10 Jun 2006
    author:     Olivier Delannoy

    purpose:    Master include  for the SILLY library 
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
#ifndef _SILLY_h_ 
#define _SILLY_h_
/*!  \mainpage 

   The Simple Image Loading LibrarY is a compagnion
   library of the CEGUI project. This library released under MIT aims
   at providing a simple and easy to use library for image loading. 

   At the time being it supports the following image format: 
   - Targa
   - JPEG (Joint Photographic Experts Group)
   - Portable Network Graphics

   The 0.1.0 release does not support any palettized version. 
  

   The library rests on third party library : 
   - jpeg-6b available at http://www.ijg.org/
   - libpng 1.2.10 available at http://www.libpng.org/
 
   In order to help you getting started with the library, the
   following example shows how to load an image from a memory area. In
   order to load an image one need to know only a few class : 
   SILLY::MemoryDataSource which is an adaptator around a memory area and 
   SILLY::Image 

   \code 
   SILLY::SILLYInit(); // Init the library

   SILLY::byte* rawData;
   // set rawData with whatever pleased you (an image should be perfect ;p) 
   // We assume here that rawData is set to a memory area of dataSize bytes. 

   // Create the data source of the image 
   SILLY::MemoryDataSource mds(rawData, dataSize);

   // You can also use a FileDataSource for the image : 
   // SILLY::FileDataSource mds(filename);

   // Create the image object 
   SILLY::Image img(mds);
   // Load the image header 
   if (!img.loadImageHeader())
   {
      // It's an error, the data does not corresponds to an image 
      // or the image data are not supported by SILLY 
      // return/exit/abort/throw ... 
   }
   std::cout << "Image Loader: " << img.getLoaderIdentifierString() << std::endl
             << "Width: " << img.getWidth() << std::endl
	     << "Height: " << img.getHeight() << std::endl;
	     
   // Load the content of the image (pixels) 
   // Here we want the image data to be stored in memory using 
   // RGBA and the first line of the image to be stored first 
   
   if (!img.loadImageData(SILLY::PF_RGBA, SILLY::PO_TOP_LEFT))
   {
       // loading of the image data failed 
       // return/exit/abort/throw
   }
   // Return a pointer to a byte array containing the 
   // pixels stored as RGBA 
   img.getPixelsDataPtr(); 
   // Get the size of the pixel array 
   img.getPixelsDataSize();
   SILLY::SILLYCleanup(); // Free all memory used by the library 
   \endcode

 */


/*!
  \brief 
  Simple Image Loading LibrarY namespace 

  This namespace contains all the symbols and types of the 
  library 
*/
namespace SILLY
{
  
}




#include "SILLYBase.h"
#include "SILLYDataSource.h" 
#include "SILLYFileDataSource.h" 
#include "SILLYMemoryDataSource.h" 
#include "SILLYImageLoader.h"
#include "SILLYImageContext.h" 
#include "SILLYImageLoaderManager.h" 
#include "SILLYImage.h"

#endif // end of guard _SILLY_h_
