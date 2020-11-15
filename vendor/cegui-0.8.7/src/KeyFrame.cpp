/***********************************************************************
    created:    7/8/2010
    author:     Martin Preisler

    purpose:    Implements the KeyFrame class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2010 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/KeyFrame.h"
#include "CEGUI/AnimationInstance.h"
#include "CEGUI/Affector.h"
#include "CEGUI/Animation_xmlHandler.h"
#include "CEGUI/XMLSerializer.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/Exceptions.h"
#include <cmath>

// Start of CEGUI namespace section
namespace CEGUI
{

//----------------------------------------------------------------------------//
KeyFrame::KeyFrame(Affector* parent, float position):
        d_parent(parent),
        d_position(position),

        d_progression(P_Linear)
{}

//----------------------------------------------------------------------------//
KeyFrame::~KeyFrame(void)
{}

//----------------------------------------------------------------------------//
Affector* KeyFrame::getParent() const
{
    return d_parent;
}

//----------------------------------------------------------------------------//
size_t KeyFrame::getIdxInParent() const
{
    const Affector* parent = getParent();
    assert(getParent() && "No parent, no index in parent!");

    size_t i = 0;
    while (i < parent->getNumKeyFrames())
    {
        if (parent->getKeyFrameAtIdx(i) == this)
        {
            return i;
        }

        ++i;
    }

    CEGUI_THROW(UnknownObjectException(
        "KeyFrame wasn't found in parent, therefore its index is unknown!"));
}

//----------------------------------------------------------------------------//
void KeyFrame::moveToPosition(float newPosition)
{
    assert(d_parent);

    d_parent->moveKeyFrameToPosition(this, newPosition);
}

//----------------------------------------------------------------------------//
float KeyFrame::getPosition() const
{
    return d_position;
}

//----------------------------------------------------------------------------//
void KeyFrame::setValue(const String& value)
{
    d_value = value;
}

//----------------------------------------------------------------------------//
const String& KeyFrame::getValue() const
{
    return d_value;
}

//----------------------------------------------------------------------------//
void KeyFrame::setSourceProperty(const String& sourceProperty)
{
    d_sourceProperty = sourceProperty;
}

//----------------------------------------------------------------------------//
const String& KeyFrame::getSourceProperty() const
{
    return d_sourceProperty;
}

//----------------------------------------------------------------------------//
const String& KeyFrame::getValueForAnimation(AnimationInstance* instance) const
{
    if (!d_sourceProperty.empty())
    {
        return instance->getSavedPropertyValue(d_sourceProperty);
    }
    else
    {
        return d_value;
    }
}

//----------------------------------------------------------------------------//
void KeyFrame::setProgression(Progression p)
{
    d_progression = p;
}

//----------------------------------------------------------------------------//
KeyFrame::Progression KeyFrame::getProgression() const
{
    return d_progression;
}

//----------------------------------------------------------------------------//
float KeyFrame::alterInterpolationPosition(float position)
{
    switch (d_progression)
    {
    case P_Linear:
        return position;

    case P_QuadraticAccelerating:
        return position * position;

    case P_QuadraticDecelerating:
        return sqrtf(position);

    case P_Discrete:
        return position < 1.0f ? 0.0f : 1.0f;
    }

    // todo: more progression methods?
    assert(0);
    return position;
}

//----------------------------------------------------------------------------//
void KeyFrame::savePropertyValue(AnimationInstance* instance)
{
    if (!d_sourceProperty.empty())
    {
        instance->savePropertyValue(d_sourceProperty);
    }
}

//----------------------------------------------------------------------------//
void KeyFrame::notifyPositionChanged(float newPosition)
{
    d_position = newPosition;
}

void KeyFrame::writeXMLToStream(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(AnimationKeyFrameHandler::ElementName);

    xml_stream.attribute(AnimationKeyFrameHandler::PositionAttribute, PropertyHelper<float>::toString(getPosition()));

    if (!d_sourceProperty.empty())
    {
        xml_stream.attribute(AnimationKeyFrameHandler::SourcePropertyAttribute, getSourceProperty());
    }
    else
    {
        xml_stream.attribute(AnimationKeyFrameHandler::ValueAttribute, getValue());
    }

    String progression;
    switch(getProgression())
    {
    case P_Linear:
        progression = AnimationKeyFrameHandler::ProgressionLinear;
        break;
    case P_Discrete:
        progression = AnimationKeyFrameHandler::ProgressionDiscrete;
        break;
    case P_QuadraticAccelerating:
        progression = AnimationKeyFrameHandler::ProgressionQuadraticAccelerating;
        break;
    case P_QuadraticDecelerating:
        progression = AnimationKeyFrameHandler::ProgressionQuadraticDecelerating;
        break;

    default:
        assert(0 && "How did we get here?");
        break;
    }
    xml_stream.attribute(AnimationKeyFrameHandler::ProgressionAttribute, progression);

    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
