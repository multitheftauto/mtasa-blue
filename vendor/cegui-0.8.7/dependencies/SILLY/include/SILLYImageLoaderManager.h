/***********************************************************************
    filename:   ImageLoaderManager.h
    created:    10 Jun 2006
    author:     Olivier Delannoy

    purpose:    Manage all available loader
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
#ifndef _SILLYImageLoaderManager_h_ 
#define _SILLYImageLoaderManager_h_
#include "SILLYBase.h" 
#include <vector> 
// Start of section namespace SILLY 
namespace SILLY
{
class ImageLoader; // Forward declaration
/*! 
  \brief 
  Typename for a collection of image loader
*/
typedef std::vector<ImageLoader*> ImageLoaderList;

#if defined(_MSC_VER)

#   pragma warning(push)

#   pragma warning(disable : 4251)

#endif

/*! 
  \brief
    Manage the list of all supported ImageLoader 
*/
class SILLY_EXPORT ImageLoaderManager
{
    
public:
    /*! 
      \brief 
      Constructor 
    */
    ImageLoaderManager();
    /*! 
      \brief
      Destructor 
    */
    ~ImageLoaderManager();
    /*! 
      \brief 
      Get an iterator to the first ImageLoader registered 
    */
    ImageLoaderList::iterator begin();
    /*!
      \brief
      Get an iterator to the first not registered ImageLoader
    */
    ImageLoaderList::iterator end();
    /*! 
      \brief 
      Register an ImageLoader object 
      
      \param loader a pointer to an image loader object 
    */
    void add(ImageLoader* loader);
    
private:
    ImageLoaderList d_loaders;  //!< Store all image loaders

public:
  static ImageLoaderManager* getSingletonPtr();
  static ImageLoaderManager& getSingleton();

private:
  static ImageLoaderManager* d_instance; //!< Store the singleton instance
    
  // Disabled operations
  ImageLoaderManager(ImageLoaderManager&);
  ImageLoaderManager& operator=(ImageLoaderManager&);
};

#if defined(_MSC_VER)

#   pragma warning(pop)

#endif  
  
} // End of section namespace SILLY 

// Inclue inline function when needed 
#ifdef SILLY_OPT_INLINE
#include "SILLYImageLoaderManager.icpp"
#endif 

#endif // end of guard _SILLYImageLoaderManager_h_
