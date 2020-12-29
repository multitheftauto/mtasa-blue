/************************************************************************
    filename:   CEGUIFalComponentBase.h
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
#ifndef _CEGUIFalComponentBase_h_
#define _CEGUIFalComponentBase_h_

#include "falagard/CEGUIFalDimensions.h"
#include "CEGUIWindow.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Common base class used for renderable components within an ImagerySection.
    */
    class CEGUIEXPORT FalagardComponentBase
    {
    public:
        /*!
        \brief
            Constructor
        */
        FalagardComponentBase();

        /*!
        \brief
            Destructor
        */
        virtual ~FalagardComponentBase();

        /*!
        \brief
            Render this component.  More correctly, the component is cached for rendering.

        \param srcWindow
            Window to use as the base for translating the component's ComponentArea into pixel values.

        \param base_z
            The z value to use for rendering the component.  Note that this is not the final z value to use, but
            some z offset from a currently unknown starting value.

        \param modColours
            ColourRect describing colours that are to be modulated with the component's stored colour values
            to calculate a set of 'final' colour values to be used.  May be 0.

        \return
            Nothing.
        */
        void render(Window& srcWindow, float base_z, const CEGUI::ColourRect* modColours = 0, const Rect* clipper = 0, bool clipToDisplay = false) const;

        /*!
        \brief
            Render this component.  More correctly, the component is cached for rendering.

        \param srcWindow
            Window to use as the base for translating the component's ComponentArea into pixel values.

        \param baseRect
            Rect to use as the base for translating the component's ComponentArea into pixel values.

        \param base_z
            The z value to use for rendering the component.  Note that this is not the final z value to use, but
            some z offset from a currently unknown starting value.

        \param modColours
            ColourRect describing colours that are to be modulated with the component's stored colour values
            to calculate a set of 'final' colour values to be used.  May be 0.

        \return
            Nothing.
        */
        void render(Window& srcWindow, const Rect& baseRect, float base_z, const CEGUI::ColourRect* modColours = 0, const Rect* clipper = 0, bool clipToDisplay = false) const;

        /*!
        \brief
            Return the ComponentArea of this ImageryComponent.

        \return
            ComponentArea object describing the ImageryComponent's current target area.
        */
        const ComponentArea& getComponentArea() const;

        /*!
        \brief
            Set the ImageryComponent's ComponentArea.

        \param area
            ComponentArea object describing a new target area for the ImageryComponent.

        \return
            Nothing.
        */
        void setComponentArea(const ComponentArea& area);

        /*!
        \brief
            Return the ColourRect set for use by this ImageryComponent.

        \return
            ColourRect object holding the colours currently in use by this ImageryComponent.
        */
        const ColourRect& getColours() const;

        /*!
        \brief
            Set the colours to be used by this ImageryComponent.

        \param cols
            ColourRect object describing the colours to be used by this ImageryComponent.
        */
        void setColours(const ColourRect& cols);

        /*!
        \brief
            Set the name of the property where colour values can be obtained.

        \param property
            String containing the name of the property.

        \return
            Nothing.
        */
        void setColoursPropertySource(const String& property);

        /*!
        \brief
            Set whether the colours property source represents a full ColourRect.

        \param setting
            - true if the colours property will access a ColourRect object.
            - false if the colours property will access a colour object.

        \return
            Nothing.
        */
        void setColoursPropertyIsColourRect(bool setting = true);

        /*!
        \brief
            Set the name of the property where vertical formatting option can be obtained.

        \param property
            String containing the name of the property.

        \return
            Nothing.
        */
        void setVertFormattingPropertySource(const String& property);

        /*!
        \brief
            Set the name of the property where horizontal formatting option can be obtained.

        \param property
            String containing the name of the property.

        \return
            Nothing.
        */
        void setHorzFormattingPropertySource(const String& property);

    protected:
        /*!
        \brief
            Helper method to initialise a ColourRect with appropriate values according to the way the
            ImageryComponent is set up.

            This will try and get values from multiple places:
                - a property attached to \a wnd
                - or the integral d_colours value.
        */
        void initColoursRect(const Window& wnd, const ColourRect* modCols, ColourRect& cr) const;

        /*!
        \brief
            Method to do main render caching work.
        */
        virtual void render_impl(Window& srcWindow, Rect& destRect, float base_z, const CEGUI::ColourRect* modColours, const Rect* clipper, bool clipToDisplay) const = 0;

        /*!
        \brief
            Writes xml for the colours to a OutStream.  Will prefer property colours before explicit.

        \note
            This is intended as a helper method for sub-classes when outputting xml to a stream.

        \return
            - true if xml element was written.
            - false if nothing was output due to the formatting not being set (sub-class may then choose to do something else.)
        */
        bool writeColoursXML(OutStream& out_stream) const;

        /*!
        \brief
            Writes xml for the vertical formatting to a OutStream if such a property is defined.

        \note
            This is intended as a helper method for sub-classes when outputting xml to a stream.

        \return
            - true if xml element was written.
            - false if nothing was output due to the formatting not being set (sub-class may then choose to do something else.)
        */
        bool writeVertFormatXML(OutStream& out_stream) const;

        /*!
        \brief
            Writes xml for the horizontal formatting to a OutStream if such a property is defined.

        \note
            This is intended as a helper method for sub-classes when outputting xml to a stream.

        \return
            - true if xml element was written.
            - false if nothing was output due to the formatting not being set (sub-class may then choose to do something else.)
        */
        bool writeHorzFormatXML(OutStream& out_stream) const;


        // data fields
        ComponentArea   d_area;                 //!< Destination area for this component.
        ColourRect      d_colours;              //!< base colours to be applied when rendering the image component.
        String          d_colourPropertyName;   //!< name of property to fetch colours from.
        bool            d_colourProperyIsRect;  //!< true if the colour property will fetch a full ColourRect.
        String          d_vertFormatPropertyName;   //!< name of property to fetch vertical formatting setting from.
        String          d_horzFormatPropertyName;   //!< name of property to fetch horizontal formatting setting from.
    };
} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalComponentBase_h_
