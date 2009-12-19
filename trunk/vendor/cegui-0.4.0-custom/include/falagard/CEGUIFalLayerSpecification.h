/************************************************************************
    filename:   CEGUIFalLayerSpecification.h
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
#ifndef _CEGUIFalLayerSpecification_h_
#define _CEGUIFalLayerSpecification_h_

#include "falagard/CEGUIFalSectionSpecification.h"
#include "CEGUIWindow.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Class that encapsulates a single layer of imagery.
    */
    class CEGUIEXPORT LayerSpecification
    {
    public:
        /*!
        \brief
            Constructor.

        \param priority
            Specifies the priority of the layer.  Layers with higher priorities will be drawn on top
            of layers with lower priorities.
        */
        LayerSpecification(uint priority);

        /*!
        \brief
            Render this layer.

        \param srcWindow
            Window to use when calculating pixel values from BaseDim values.

        \param base_z
            base level z value to use for all imagery in the layer.

        \return
            Nothing.
        */
        void render(Window& srcWindow, float base_z, const ColourRect* modcols = 0, const Rect* clipper = 0, bool clipToDisplay = false) const;

        /*!
        \brief
            Render this layer.

        \param srcWindow
            Window to use when calculating pixel values from BaseDim values.

        \param baseRect
            Rect to use when calculating pixel values from BaseDim values.

        \param base_z
            base level z value to use for all imagery in the layer.

        \return
            Nothing.
        */
        void render(Window& srcWindow, const Rect& baseRect, float base_z, const ColourRect* modcols = 0, const Rect* clipper = 0, bool clipToDisplay = false) const;

        /*!
        \brief
            Add a section specification to the layer.

            A section specification is a reference to a named ImagerySection within the WidgetLook.

        \param section
            SectionSpecification object descibing the section that should be added to this layer.

        \return
            Nothing,
        */
        void addSectionSpecification(const SectionSpecification& section);

        /*!
        \brief
            Clear all section specifications from this layer,

        \return
            Nothing.
        */
        void clearSectionSpecifications();

        /*!
        \brief
            Return the priority of this layer.

        \return
            uint value descibing the priority of this LayerSpecification.
        */
        uint getLayerPriority() const;

        // required to sort layers according to priority
        bool operator<(const LayerSpecification& other) const;

        /*!
        \brief
            Writes an xml representation of this Layer to \a out_stream.

        \param out_stream
            Stream where xml data should be output.

        \return
            Nothing.
        */
        void writeXMLToStream(OutStream& out_stream) const;

    private:
        typedef std::vector<SectionSpecification> SectionList;

        SectionList d_sections;         //!< Collection of SectionSpecification objects descibing the sections to be drawn for this layer.
        uint        d_layerPriority;    //!< Priority of the layer.
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _CEGUIFalLayerSpecification_h_
