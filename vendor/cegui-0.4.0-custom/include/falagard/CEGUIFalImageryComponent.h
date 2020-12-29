/************************************************************************
    filename:   CEGUIFalImageryComponent.h
    created:    Mon Jun 13 2005
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
#ifndef _CEGUIFalImageryComponent_h_
#define _CEGUIFalImageryComponent_h_

#include "falagard/CEGUIFalComponentBase.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Class that encapsulates information for a single image component.
    */
    class CEGUIEXPORT ImageryComponent : public FalagardComponentBase
    {
    public:
        /*!
        \brief
            Constructor
        */
        ImageryComponent();

        /*!
        \brief
            Return the Image object that will be drawn by this ImageryComponent.

        \return
            Image object.
        */
        const Image* getImage() const;

        /*!
        \brief
            Set the Image that will be drawn by this ImageryComponent.

        \param
            Pointer to the Image object to be drawn by this ImageryComponent.

        \return
            Nothing.
        */
        void setImage(const Image* image);

        /*!
        \brief
            Set the Image that will be drawn by this ImageryComponent.

        \param imageset
            String holding the name of the Imagset that contains the Image to be rendered.

        \param image
            String holding the name of the Image to be rendered.

        \return
            Nothing.
        */
        void setImage(const String& imageset, const String& image);

        /*!
        \brief
            Return the current vertical formatting setting for this ImageryComponent.

        \return
            One of the VerticalFormatting enumerated values.
        */
        VerticalFormatting getVerticalFormatting() const;

        /*!
        \brief
            Set the vertical formatting setting for this ImageryComponent.

        \param fmt
            One of the VerticalFormatting enumerated values.

        \return
            Nothing.
        */
        void setVerticalFormatting(VerticalFormatting fmt);

        /*!
        \brief
            Return the current horizontal formatting setting for this ImageryComponent.

        \return
            One of the HorizontalFormatting enumerated values.
        */
        HorizontalFormatting getHorizontalFormatting() const;

        /*!
        \brief
            Set the horizontal formatting setting for this ImageryComponent.

        \param fmt
            One of the HorizontalFormatting enumerated values.

        \return
            Nothing.
        */
        void setHorizontalFormatting(HorizontalFormatting fmt);

        /*!
        \brief
            Writes an xml representation of this ImageryComponent to \a out_stream.

        \param out_stream
            Stream where xml data should be output.

        \return
            Nothing.
        */
        void writeXMLToStream(OutStream& out_stream) const;

        /*!
        \brief
            Return whether this ImageryComponent fetches it's image via a property on the target window.

        \return
            - true if the image comes via a Propery.
            - false if the image is defined explicitly.
        */
        bool isImageFetchedFromProperty() const;

        /*!
        \brief
            Return the name of the property that will be used to determine the image for this ImageryComponent.

        \return
            String object holding the name of a Propery.
        */
        const String& getImagePropertySource() const;

        /*!
        \brief
            Set the name of the property that will be used to determine the image for this ImageryComponent.

        \param property
            String object holding the name of a Propery.  The property should access a imageset & image specification.

        \return
            Nothing.
        */
        void setImagePropertySource(const String& property);

    protected:
        // implemets abstract from base
        void render_impl(Window& srcWindow, Rect& destRect, float base_z, const CEGUI::ColourRect* modColours, const Rect* clipper, bool clipToDisplay) const;

        const Image*         d_image;           //!< CEGUI::Image to be drawn by this image component.
        VerticalFormatting   d_vertFormatting;  //!< Vertical formatting to be applied when rendering the image component.
        HorizontalFormatting d_horzFormatting;  //!< Horizontal formatting to be applied when rendering the image component.
        String  d_imagePropertyName;            //!< Name of the property to access to obtain the image to be used.
    };

} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalImageryComponent_h_
