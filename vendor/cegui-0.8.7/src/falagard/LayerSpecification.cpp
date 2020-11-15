/***********************************************************************
    created:    Mon Jun 13 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "CEGUI/falagard/LayerSpecification.h"
#include "CEGUI/falagard/XMLHandler.h"
#include <iostream>
#include "CEGUI/PropertyHelper.h"
// Start of CEGUI namespace section
namespace CEGUI
{

LayerSpecification::LayerSpecification(uint priority) :
    d_layerPriority(priority)
{}

void LayerSpecification::render(Window& srcWindow, const ColourRect* modcols, const Rectf* clipper, bool clipToDisplay) const
{
    // render all sections in this layer
    for(SectionList::const_iterator curr = d_sections.begin(); curr != d_sections.end(); ++curr)
    {
        (*curr).render(srcWindow, modcols, clipper, clipToDisplay);
    }
}

void LayerSpecification::render(Window& srcWindow, const Rectf& baseRect, const ColourRect* modcols, const Rectf* clipper, bool clipToDisplay) const
{
    // render all sections in this layer
    for(SectionList::const_iterator curr = d_sections.begin(); curr != d_sections.end(); ++curr)
    {
        (*curr).render(srcWindow, baseRect, modcols, clipper, clipToDisplay);
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

void LayerSpecification::setLayerPriority(uint priority)
{
    d_layerPriority = priority;
}

void LayerSpecification::writeXMLToStream(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::LayerElement);

    if (d_layerPriority != 0)
        xml_stream.attribute(Falagard_xmlHandler::PriorityAttribute, PropertyHelper<uint>::toString(d_layerPriority));

    // ouput all sections in this layer
    for(SectionList::const_iterator curr = d_sections.begin(); curr != d_sections.end(); ++curr)
    {
        (*curr).writeXMLToStream(xml_stream);
    }

    xml_stream.closeTag();
}

LayerSpecification::SectionIterator
LayerSpecification::getSectionIterator() const
{
    return SectionIterator(d_sections.begin(), d_sections.end());
}

LayerSpecification::SectionSpecificationPointerList LayerSpecification::getSectionSpecificationPointers()
{
    LayerSpecification::SectionSpecificationPointerList pointerList;

    SectionList::iterator sectionSpecificationIter = d_sections.begin();
    SectionList::iterator sectionSpecificationEnd = d_sections.end();
    while( sectionSpecificationIter != sectionSpecificationEnd )
    {
        pointerList.push_back(&(*sectionSpecificationIter));
        ++sectionSpecificationIter;
    }

    return pointerList;
}

bool LayerSpecification::operator< (const LayerSpecification& otherLayerSpec) const
{
    return d_layerPriority < otherLayerSpec.getLayerPriority();
}

} // End of  CEGUI namespace section
