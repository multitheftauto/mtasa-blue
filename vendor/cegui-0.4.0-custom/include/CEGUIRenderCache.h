/************************************************************************
    filename:   CEGUIRenderCache.h
    created:    Fri Jun 17 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#ifndef _CEGUIRenderCache_h_
#define _CEGUIRenderCache_h_

#include "CEGUIVector.h"
#include "CEGUIRect.h"
#include "CEGUIImage.h"
#include "CEGUIFont.h"
#include <vector>


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Class that acts as a cache for images that need to be rendered.

        This is in many ways an optimisation cache, it allows a full image redraw
        to occur while limiting the amount of information that needs to be re-calculated.
    \par
        Basically, unless the actual images (or their size) change, or the colours (or alpha) change
        then imagery cached in here will suffice for a full redraw.  The reasoning behind this is that
        when some window underneath window 'X' changes, a full image redraw is required by the
        renderer, however, since window 'X' is unchanged, performing a total recalculation of all
        imagery is very wasteful, and so we use this cache to limit such waste.
    \par
        As another example, when a window is simply moved, there is no need to perform a total imagery
        recalculation; we can still use the imagery cached here since it is position independant.
    */
    class CEGUIEXPORT RenderCache
    {
    public:
        /*!
        \brief
            Constructor
        */
        RenderCache();

        /*!
        \brief
            Destructor
        */
        ~RenderCache();

        /*!
        \brief
            Return whether the cache contains anything to draw.

        \return
            - true if the cache contains information about images to be drawn.
            - false if the cache is empty.
        */
        bool hasCachedImagery() const;

        /*!
        \brief
            Send the contents of the cache to the Renderer.

        \param basePos
            Point that describes a screen offset that cached imagery will be rendered relative to.

        \param baseZ
            Z value that cached imagery will use as a base figure when calculating final z values.

        \param clipper
            Rect object describing a rect to which imagery will be clipped.

        \return
            Nothing
        */
        void render(const Point& basePos, float baseZ, const Rect& clipper) const;

        /*!
        \brief
            Erase any stored image information.
        */
        void clearCachedImagery();

        /*!
        \brief
            Add an image to the cache.

        \param image
            Image object to be cached.

        \param destArea
            Destination area over which the Image object will be rendered.  This area should be position
            independant; so position (0,0) will be to top-left corner of whatever it is you're rendering
            (like a Window for example).

        \param zOffset
            Zero based z offset for this image.  Allows imagery to be layered.

        \param cols
            ColourRect object describing the colours to be applied when rendering this image.

        \return
            Nothing
        */
        void cacheImage(const Image& image, const Rect& destArea, float zOffset, const ColourRect& cols, const Rect* clipper = 0, bool clipToDisplay = false);

        /*!
        \brief
            Add a text to the cache.

        \param text
            String object to be cached.

        \param font
            Font to be used when rendering.

        \param format
            TextFormatting value specifying the formatting to use when rendering.

        \param destArea
            Destination area over which the Image object will be rendered.  This area should be position
            independant; so position (0,0) will be to top-left corner of whatever it is you're rendering
            (like a Window for example).

        \param zOffset
            Zero based z offset for this image.  Allows imagery to be layered.

        \param cols
            ColourRect object describing the colours to be applied when rendering this image.

        \return
            Nothing
        */
        void cacheText(const String& text, const Font* font, TextFormatting format, const Rect& destArea, float zOffset, const ColourRect& cols, const Rect* clipper = 0, bool clipToDisplay = false);

    private:
        /*!
        \brief
            internal struct that holds info about a single image to be drawn.
        */
        struct ImageInfo
        {
            const Image* source_image;
            Rect target_area;
            float z_offset;
            ColourRect colours;
            Rect customClipper;
            bool usingCustomClipper;
            bool clipToDisplay;
        };

        /*!
        \brief
            internal struct that holds info about text to be drawn.
        */
        struct TextInfo
        {
            String text;
            const Font* source_font;
            TextFormatting formatting;
            Rect target_area;
            float z_offset;
            ColourRect colours;
            Rect customClipper;
            bool usingCustomClipper;
            bool clipToDisplay;
        };

        typedef std::vector<ImageInfo>  ImageryList;
        typedef std::vector<TextInfo>   TextList;

        ImageryList d_cachedImages;     //!< Collection of ImageInfo structs.
        TextList d_cachedTexts;         //!< Collection of TextInfo structs.
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif


#endif  // end of guard _CEGUIRenderCache_h_
