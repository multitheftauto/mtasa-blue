/************************************************************************
    filename:   CEGUIFalFrameComponent.h
    created:    Mon Jul 18 2005
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
#ifndef _CEGUIFalFrameComponent_h_
#define _CEGUIFalFrameComponent_h_

#include "falagard/CEGUIFalComponentBase.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Class that encapsulates information for a frame with background (9 images in total)

        Corner images are always drawn at their natural size, edges are stretched between the corner
        pieces for a particular edge, the background image will cover the inner rectangle formed by
        the edge images and can be stretched or tiled in either dimension.
    */
    class CEGUIEXPORT FrameComponent : public FalagardComponentBase
    {
    public:
        /*!
        \brief
            Constructor
        */
        FrameComponent();

        /*!
        \brief
            Return the current vertical formatting setting for this FrameComponent.

        \return
            One of the VerticalFormatting enumerated values.
        */
        VerticalFormatting getBackgroundVerticalFormatting() const;

        /*!
        \brief
            Set the vertical formatting setting for this FrameComponent.

        \param fmt
            One of the VerticalFormatting enumerated values.

        \return
            Nothing.
        */
        void setBackgroundVerticalFormatting(VerticalFormatting fmt);

        /*!
        \brief
            Return the current horizontal formatting setting for this FrameComponent.

        \return
            One of the HorizontalFormatting enumerated values.
        */
        HorizontalFormatting getBackgroundHorizontalFormatting() const;

        /*!
        \brief
            Set the horizontal formatting setting for this FrameComponent.

        \param fmt
            One of the HorizontalFormatting enumerated values.

        \return
            Nothing.
        */
        void setBackgroundHorizontalFormatting(HorizontalFormatting fmt);

        /*!
        \brief
            Return the Image object that will be drawn by this FrameComponent for a specified frame part.

        \param part
            One of the FrameImageComponent enumerated values specifying the component image to be accessed.

        \return
            Image object.
        */
        const Image* getImage(FrameImageComponent part) const;

        /*!
        \brief
            Set the Image that will be drawn by this ImageryComponent.

        \param part
            One of the FrameImageComponent enumerated values specifying the component image to be accessed.

        \param image
            Pointer to the Image object to be drawn by this FrameComponent.

        \return
            Nothing.
        */
        void setImage(FrameImageComponent part, const Image* image);

        /*!
        \brief
            Set the Image that will be drawn by this FrameComponent.

        \param part
            One of the FrameImageComponent enumerated values specifying the component image to be accessed.

        \param imageset
            String holding the name of the Imagset that contains the Image to be rendered.

        \param image
            String holding the name of the Image to be rendered.

        \return
            Nothing.
        */
        void setImage(FrameImageComponent part, const String& imageset, const String& image);

        /*!
        \brief
            Writes an xml representation of this FrameComponent to \a out_stream.

        \param out_stream
            Stream where xml data should be output.

        \return
            Nothing.
        */
        void writeXMLToStream(OutStream& out_stream) const;

    protected:
        // implemets abstract from base
        void render_impl(Window& srcWindow, Rect& destRect, float base_z, const CEGUI::ColourRect* modColours, const Rect* clipper, bool clipToDisplay) const;

        // renders the background image (basically a clone of render_impl from ImageryComponent - maybe we need a helper class?)
        void doBackgroundRender(Window& srcWindow, Rect& destRect, float base_z, const ColourRect& colours, const Rect* clipper, bool clipToDisplay) const;

        // formatting options for background
        VerticalFormatting   d_vertFormatting;  //!< Vertical formatting to be applied when rendering the background for the component.
        HorizontalFormatting d_horzFormatting;  //!< Horizontal formatting to be applied when rendering the background for the component.
        // images for the frame
        const Image* d_frameImages[FIC_FRAME_IMAGE_COUNT];  //!< Array that holds the assigned images.
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalFrameComponent_h_
