/************************************************************************
    filename:   CEGUIFalImagerySection.h
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
#ifndef _CEGUIFalImagerySection_h_
#define _CEGUIFalImagerySection_h_

#include "falagard/CEGUIFalImageryComponent.h"
#include "falagard/CEGUIFalTextComponent.h"
#include "falagard/CEGUIFalFrameComponent.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Class that encapsulates a re-usable collection of imagery specifications.
    */
    class CEGUIEXPORT ImagerySection
    {
    public:
        /*!
        \brief
            Constructor.
        */
        ImagerySection();

        /*!
        \brief
            ImagerySection constructor.  Name must be supplied, masterColours are set to 0xFFFFFFFF by default.

        \param name
            Name of the new ImagerySection.
        */
        ImagerySection(const String& name);

        /*!
        \brief
            Render the ImagerySection.

        \param srcWindow
            Window object to be used when calculating pixel values from BaseDim values.

        \param base_z
            base z value to be used for all imagery in the section.

        \param modColours
            ColourRect specifying colours to be modulated with the ImagerySection's master colours.  May be 0.

        \return
            Nothing.
        */
        void render(Window& srcWindow, float base_z, const CEGUI::ColourRect* modColours = 0, const Rect* clipper = 0, bool clipToDisplay = false) const;

        /*!
        \brief
            Render the ImagerySection.

        \param srcWindow
            Window object to be used when calculating pixel values from BaseDim values.

        \param baseRect
            Rect object to be used when calculating pixel values from BaseDim values.

        \param base_z
            base z value to be used for all imagery in the section.

        \param modColours
            ColourRect specifying colours to be modulated with the ImagerySection's master colours.  May be 0.

        \return
            Nothing.
        */
        void render(Window& srcWindow, const Rect& baseRect, float base_z, const CEGUI::ColourRect* modColours = 0, const Rect* clipper = 0, bool clipToDisplay = false) const;

        /*!
        \brief
            Add an ImageryComponent to this ImagerySection.

        \param img
            ImageryComponent to be added to the section (a copy is made)

        \return
            Nothing
        */
        void addImageryComponent(const ImageryComponent& img);

        /*!
        \brief
            Clear all ImageryComponents from this ImagerySection.

        \return
            Nothing
        */
        void clearImageryComponents();

        /*!
        \brief
            Add a TextComponent to this ImagerySection.

        \param text
            TextComponent to be added to the section (a copy is made)

        \return
            Nothing
        */
        void addTextComponent(const TextComponent& text);

        /*!
        \brief
            Clear all TextComponents from this ImagerySection.

        \return
            Nothing
        */
        void clearTextComponents();

        /*!
        \brief
            Clear all FrameComponents from this ImagerySection.

        \return
            Nothing
        */
        void clearFrameComponents();

        /*!
        \brief
            Add a FrameComponent to this ImagerySection.

        \param frame
            FrameComponent to be added to the section (a copy is made)

        \return
            Nothing
        */
        void addFrameComponent(const FrameComponent& frame);

        /*!
        \brief
            Return the current master colours set for this ImagerySection.

        \return
            ColourRect describing the master colour values in use for this ImagerySection.
        */
        const ColourRect& getMasterColours() const;

        /*!
        \brief
            Set the master colours to be used for this ImagerySection.

        \param cols
            ColourRect describing the colours to be set as the master colours for this ImagerySection.

        \return
            Nothing.
        */
        void setMasterColours(const ColourRect& cols);

        /*!
        \brief
            Return the name of this ImagerySection.

        \return
            String object holding the name of the ImagerySection.
        */
        const String& getName() const;

        /*!
        \brief
            Set the name of the property where master colour values can be obtained.

        \param property
            String containing the name of the property.

        \return
            Nothing.
        */
        void setMasterColoursPropertySource(const String& property);

        /*!
        \brief
            Set whether the master colours property source represents a full ColourRect.

        \param setting
            - true if the master colours property will access a ColourRect object.
            - false if the master colours property will access a colour object.

        \return
            Nothing.
        */
        void setMasterColoursPropertyIsColourRect(bool setting = true);

        /*!
        \brief
            Return smallest Rect that could contain all imagery within this section.
        */
        Rect getBoundingRect(const Window& wnd) const;

        /*!
        \brief
            Return smallest Rect that could contain all imagery within this section.
        */
        Rect getBoundingRect(const Window& wnd, const Rect& rect) const;

        /*!
        \brief
            Writes an xml representation of this ImagerySection to \a out_stream.

        \param out_stream
            Stream where xml data should be output.

        \return
            Nothing.
        */
        void writeXMLToStream(OutStream& out_stream) const;

    protected:
        /*!
        \brief
            Helper method to initialise a ColourRect with appropriate values according to the way the
            ImagerySection is set up.

            This will try and get values from multiple places:
                - a property attached to \a wnd
                - or the integral d_masterColours value.
        */
        void initMasterColourRect(const Window& wnd, ColourRect& cr) const;

    private:
        typedef std::vector<ImageryComponent> ImageryList;
        typedef std::vector<TextComponent> TextList;
        typedef std::vector<FrameComponent> FrameList;

        CEGUI::String       d_name;             //!< Holds the name of the ImagerySection.
        CEGUI::ColourRect   d_masterColours;    //!< Naster colours for the the ImagerySection (combined with colours of each ImageryComponent).
        FrameList           d_frames;           //!< Collection of FrameComponent objects to be drawn for this ImagerySection.
        ImageryList         d_images;           //!< Collection of ImageryComponent objects to be drawn for this ImagerySection.
        TextList            d_texts;            //!< Collection of TextComponent objects to be drawn for this ImagerySection.
        String              d_colourPropertyName;   //!< name of property to fetch colours from.
        bool                d_colourProperyIsRect;  //!< true if the colour property will fetch a full ColourRect.
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _CEGUIFalImagerySection_h_
