/************************************************************************
    filename:   CEGUIFalSectionSpecification.h
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
#ifndef _CEGUIFalSectionSpecification_h_
#define _CEGUIFalSectionSpecification_h_

#include "CEGUIWindow.h"


// Start of CEGUI namespace section
namespace CEGUI
{
    // forward refs
    class WidgetLookFeel;

    /*!
    \brief
        Class that represents a simple 'link' to an ImagerySection.

        This class enables sections to be easily re-used, by different states and/or layers, by allowing
        sections to be specified by name rather than having mutiple copies of the same thing all over the place.
    */
    class CEGUIEXPORT SectionSpecification
    {
    public:
        /*!
        \brief
            Constructor

        \param owner
            String holding the name of the WidgetLookFeel object that contains the target section.

        \param sectionName
            String holding the name of the target section.
        */
        SectionSpecification(const String& owner, const String& sectionName);

        /*!
        \brief
            Constructor

        \param owner
            String holding the name of the WidgetLookFeel object that contains the target section.

        \param sectionName
            String holding the name of the target section.

        \param cols
            Override colours to be used (modulates sections master colours).
        */
        SectionSpecification(const String& owner, const String& sectionName, const ColourRect& cols);

        /*!
        \brief
            Render the section specified by this SectionSpecification.

        \param srcWindow
            Window object to be used when calculating pixel values from BaseDim values.

        \param base_z
            base z co-ordinate to use for all imagery in the linked section.

        \return
            Nothing.
        */
        void render(Window& srcWindow, float base_z, const ColourRect* modcols = 0, const Rect* clipper = 0, bool clipToDisplay = false) const;

        /*!
        \brief
            Render the section specified by this SectionSpecification.

        \param srcWindow
            Window object to be used when calculating pixel values from BaseDim values.

        \param baseRect
            Rect object to be used when calculating pixel values from BaseDim values.

        \param base_z
            base z co-ordinate to use for all imagery in the linked section.

        \return
            Nothing.
        */
        void render(Window& srcWindow, const Rect& baseRect, float base_z, const ColourRect* modcols = 0, const Rect* clipper = 0, bool clipToDisplay = false) const;

        /*!
        \brief
            Return the name of the WidgetLookFeel object containing the target section.

        \return
            String object holding the name of the WidgetLookFeel that contains the target ImagerySection.
        */
        const String& getOwnerWidgetLookFeel() const;

        /*!
        \brief
            Return the name of the target ImagerySection.

        \return
            String object holding the name of the target ImagerySection.
        */
        const String& getSectionName() const;

        /*!
        \brief
            Return the current override colours.

        \return
            ColourRect holding the colours that will be modulated with the sections master colours if
            colour override is enabled on this SectionSpecification.
        */
        const ColourRect& getOverrideColours() const;

        /*!
        \brief
            Set the override colours to be used by this SectionSpecification.

        \param cols
            ColourRect describing the override colours to set for this SectionSpecification.

        \return
            Nothing.
        */
        void setOverrideColours(const ColourRect& cols);

        /*!
        \brief
            return whether the use of override colours is enabled on this SectionSpecification.

        \return
            - true if override colours will be used for this SectionSpecification.
            - false if override colours will not be used for this SectionSpecification.
        */
        bool isUsingOverrideColours() const;

        /*!
        \brief
            Enable or disable the use of override colours for this section.

        \param setting
            - true if override colours should be used for this SectionSpecification.
            - false if override colours should not be used for this SectionSpecification.

        \return
            Nothing.
        */
        void setUsingOverrideColours(bool setting = true);

        /*!
        \brief
            Set the name of the property where override colour values can be obtained.

        \param property
            String containing the name of the property.

        \return
            Nothing.
        */
        void setOverrideColoursPropertySource(const String& property);

        /*!
        \brief
            Set whether the override colours property source represents a full ColourRect.

        \param setting
            - true if the override colours property will access a ColourRect object.
            - false if the override colours property will access a colour object.

        \return
            Nothing.
        */
        void setOverrideColoursPropertyIsColourRect(bool setting = true);

        /*!
        \brief
            Writes an xml representation of this SectionSpecification to \a out_stream.

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
            section sepcification is set up.

            This will try and get values from multiple places:
                - a property attached to \a wnd
                - the integral d_coloursOverride values.
                - or default to colour(1,1,1,1);
        */
        void initColourRectForOverride(const Window& wnd, ColourRect& cr) const;

    private:
        String          d_owner;                //!< Name of the WidgetLookFeel containing the required section.
        String          d_sectionName;          //!< Name of the required section within the specified WidgetLookFeel.
        ColourRect      d_coloursOverride;      //!< Colours to use when override is enabled.
        bool            d_usingColourOverride;  //!< true if colour override is enabled.
        String          d_colourPropertyName;   //!< name of property to fetch colours from.
        bool            d_colourProperyIsRect;  //!< true if the colour property will fetch a full ColourRect.
    };


} // End of  CEGUI namespace section


#endif  // end of guard _CEGUIFalSectionSpecification_h_
