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
#include "CEGUI/falagard/StateImagery.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/System.h"
#include "CEGUI/Renderer.h"
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{

StateImagery::StateImagery(const String& name) :
    d_stateName(name),
    d_clipToDisplay(false)
{}

void StateImagery::render(Window& srcWindow, const ColourRect* modcols, const Rectf* clipper) const
{
    srcWindow.getGeometryBuffer().setClippingActive(!d_clipToDisplay);

    // render all layers defined for this state
    for(LayersList::const_iterator curr = d_layers.begin(); curr != d_layers.end(); ++curr)
        (*curr).render(srcWindow, modcols, clipper, d_clipToDisplay);
}

void StateImagery::render(Window& srcWindow, const Rectf& baseRect, const ColourRect* modcols, const Rectf* clipper) const
{
    srcWindow.getGeometryBuffer().setClippingActive(!d_clipToDisplay);

    // render all layers defined for this state
    for(LayersList::const_iterator curr = d_layers.begin(); curr != d_layers.end(); ++curr)
        (*curr).render(srcWindow, baseRect, modcols, clipper, d_clipToDisplay);
}

void StateImagery::addLayer(const LayerSpecification& layer)
{
    d_layers.insert(layer);
}

void StateImagery::clearLayers()
{
    d_layers.clear();
}

const String& StateImagery::getName() const
{
    return d_stateName;
}

void StateImagery::setName(const String& name)
{
    d_stateName = name;
}

bool StateImagery::isClippedToDisplay() const
{
    return d_clipToDisplay;
}

void StateImagery::setClippedToDisplay(bool setting)
{
    d_clipToDisplay = setting;
}

void StateImagery::writeXMLToStream(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::StateImageryElement)
        .attribute(Falagard_xmlHandler::NameAttribute, d_stateName);

    if (d_clipToDisplay)
        xml_stream.attribute(Falagard_xmlHandler::ClippedAttribute, PropertyHelper<bool>::False);

    // output all layers defined for this state
    for(LayersList::const_iterator curr = d_layers.begin(); curr != d_layers.end(); ++curr)
        (*curr).writeXMLToStream(xml_stream);
    // write closing </StateImagery> tag
    xml_stream.closeTag();
}

StateImagery::LayerIterator
StateImagery::getLayerIterator() const
{
    return LayerIterator(d_layers.begin(),d_layers.end());
}

StateImagery::LayerSpecificationPointerList StateImagery::getLayerSpecificationPointers()
{
    StateImagery::LayerSpecificationPointerList pointerList;

    LayersList::iterator layerSpecIter = d_layers.begin();
    LayersList::iterator layerSpecIterEnd = d_layers.end();
    while( layerSpecIter != layerSpecIterEnd )
    {
        //! This hack is necessary because in newer C++ versions the multiset and sets return only const iterators.
        //! \deprecated This will be replaced with proper types and behaviour in the next version.
        LayerSpecification* layerSpec = const_cast<LayerSpecification*>(&(*layerSpecIter));
        pointerList.push_back(layerSpec);
        ++layerSpecIter;
    }

    return pointerList;
}

void StateImagery::sort()
{
    //! \deprecated Deprecated behaviour: We have to remove all elements and re-add them to force them to be sorted.
    std::vector<LayerSpecification> temporaryList;

    LayersList::iterator layerSpecMapIter = d_layers.begin();
    LayersList::iterator layerSpecMapIterEnd = d_layers.end();
    while( layerSpecMapIter != layerSpecMapIterEnd )
    {
        temporaryList.push_back(*layerSpecMapIter);
        ++layerSpecMapIter;
    }

    clearLayers();
    
    std::vector<LayerSpecification>::iterator layerSpecVecIter = temporaryList.begin();
    std::vector<LayerSpecification>::iterator layerSpecVecIterEnd = temporaryList.end();
    while( layerSpecVecIter != layerSpecVecIterEnd )
    {
        addLayer(*layerSpecVecIter);
        ++layerSpecVecIter;
    }
}

} // End of  CEGUI namespace section
