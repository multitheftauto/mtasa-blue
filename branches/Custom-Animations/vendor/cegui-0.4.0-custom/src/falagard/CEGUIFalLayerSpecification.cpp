/************************************************************************
    filename:   CEGUIFalLayerSpecification.cpp
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
#include "StdInc.h"
#include "falagard/CEGUIFalLayerSpecification.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
    LayerSpecification::LayerSpecification(uint priority) :
        d_layerPriority(priority)
    {}

    void LayerSpecification::render(Window& srcWindow, float base_z, const ColourRect* modcols, const Rect* clipper, bool clipToDisplay) const
    {
        // render all sections in this layer
        for(SectionList::const_iterator curr = d_sections.begin(); curr != d_sections.end(); ++curr)
        {
            (*curr).render(srcWindow, base_z, modcols, clipper, clipToDisplay);
        }
    }

    void LayerSpecification::render(Window& srcWindow, const Rect& baseRect, float base_z, const ColourRect* modcols, const Rect* clipper, bool clipToDisplay) const
    {
        // render all sections in this layer
        for(SectionList::const_iterator curr = d_sections.begin(); curr != d_sections.end(); ++curr)
        {
            (*curr).render(srcWindow, baseRect, base_z, modcols, clipper, clipToDisplay);
        }
    }

    void LayerSpecification::addSectionSpecification(const SectionSpecification& section)
    {
        d_sections.push_back(section);
    }

    void LayerSpecification::clearSectionSpecifications()
    {
        d_sections.clear();
    }

    uint LayerSpecification::getLayerPriority() const
    {
        return d_layerPriority;
    }

    bool LayerSpecification::operator<(const LayerSpecification& other) const
    {
        return d_layerPriority < other.d_layerPriority;
    }

    void LayerSpecification::writeXMLToStream(OutStream& out_stream) const
    {
        out_stream << "<Layer";

        if (d_layerPriority != 0)
            out_stream << " priority=\"" << d_layerPriority << "\"";

        out_stream << ">" << std::endl;

        // ouput all sections in this layer
        for(SectionList::const_iterator curr = d_sections.begin(); curr != d_sections.end(); ++curr)
        {
            (*curr).writeXMLToStream(out_stream);
        }

        out_stream << "</Layer>" << std::endl;
    }

} // End of  CEGUI namespace section
