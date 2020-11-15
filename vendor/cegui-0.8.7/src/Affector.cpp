/***********************************************************************
    created:    7/8/2010
    author:     Martin Preisler

    purpose:    Implements the Affector class
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
#include "CEGUI/Affector.h"
#include "CEGUI/KeyFrame.h"
#include "CEGUI/Animation.h"
#include "CEGUI/AnimationManager.h"
#include "CEGUI/Interpolator.h"
#include "CEGUI/PropertySet.h"
#include "CEGUI/AnimationInstance.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"
#include "CEGUI/Animation_xmlHandler.h"

// Start of CEGUI namespace section
namespace CEGUI
{

//----------------------------------------------------------------------------//
Affector::Affector(Animation* parent):
    d_parent(parent),
    d_applicationMethod(AM_Absolute),
    d_targetProperty(""),
    d_interpolator(0)
{}

//----------------------------------------------------------------------------//
Affector::~Affector(void)
{
    while (d_keyFrames.size() > 0)
    {
        destroyKeyFrame(getKeyFrameAtIdx(0));
    }
}

//----------------------------------------------------------------------------//
size_t Affector::getIdxInParent() const
{
    const Animation* parent = getParent();
    assert(getParent() && "No parent, no index in parent!");

    size_t i = 0;
    while (i < parent->getNumAffectors())
    {
        if (parent->getAffectorAtIdx(i) == this)
        {
            return i;
        }

        ++i;
    }

    CEGUI_THROW(UnknownObjectException(
        "Affector wasn't found in parent, therefore its index is unknown!"));
}

//----------------------------------------------------------------------------//
void Affector::setApplicationMethod(ApplicationMethod method)
{
    d_applicationMethod = method;
}

//----------------------------------------------------------------------------//
Affector::ApplicationMethod Affector::getApplicationMethod() const
{
    return d_applicationMethod;
}


//----------------------------------------------------------------------------//
void Affector::setTargetProperty(const String& target)
{
    d_targetProperty = target;
}

//----------------------------------------------------------------------------//
const String& Affector::getTargetProperty() const
{
    return d_targetProperty;
}

//----------------------------------------------------------------------------//
void Affector::setInterpolator(Interpolator* interpolator)
{
    d_interpolator = interpolator;
}

//----------------------------------------------------------------------------//
void Affector::setInterpolator(const String& name)
{
    d_interpolator = AnimationManager::getSingleton().getInterpolator(name);
}

//----------------------------------------------------------------------------//
Interpolator* Affector::getInterpolator() const
{
    return d_interpolator;
}

//----------------------------------------------------------------------------//
KeyFrame* Affector::createKeyFrame(float position)
{
    if (d_keyFrames.find(position) != d_keyFrames.end())
    {
        CEGUI_THROW(InvalidRequestException(
                        "Unable to create KeyFrame at given position, there "
                        "already is a KeyFrame on that position."));
    }

    KeyFrame* ret = CEGUI_NEW_AO KeyFrame(this, position);
    d_keyFrames.insert(std::make_pair(position, ret));

    return ret;
}

//----------------------------------------------------------------------------//
KeyFrame* Affector::createKeyFrame(float position, const String& value,
                                   KeyFrame::Progression progression, const String& sourceProperty)
{
    KeyFrame* ret = createKeyFrame(position);
    ret->setValue(value);
    ret->setProgression(progression);
    ret->setSourceProperty(sourceProperty);

    return ret;
}

//----------------------------------------------------------------------------//
void Affector::destroyKeyFrame(KeyFrame* keyframe)
{
    KeyFrameMap::iterator it = d_keyFrames.find(keyframe->getPosition());

    if (it == d_keyFrames.end())
    {
        CEGUI_THROW(InvalidRequestException(
                        "Unable to destroy given KeyFrame! "
                        "No such KeyFrame was found."));
    }

    d_keyFrames.erase(it);
    CEGUI_DELETE_AO keyframe;
}

//----------------------------------------------------------------------------//
KeyFrame* Affector::getKeyFrameAtPosition(float position) const
{
    KeyFrameMap::const_iterator it = d_keyFrames.find(position);

    if (it == d_keyFrames.end())
    {
        CEGUI_THROW(InvalidRequestException(
                        "Can't find a KeyFrame with given position."));
    }

    return it->second;
}

//----------------------------------------------------------------------------//
bool Affector::hasKeyFrameAtPosition(float position) const
{
	return d_keyFrames.find(position) != d_keyFrames.end();
}

//----------------------------------------------------------------------------//
KeyFrame* Affector::getKeyFrameAtIdx(size_t index) const
{
    if (index >= d_keyFrames.size())
    {
        CEGUI_THROW(InvalidRequestException("Out of bounds!"));
    }

    KeyFrameMap::const_iterator it = d_keyFrames.begin();
    std::advance(it, index);

    return it->second;
}

//----------------------------------------------------------------------------//
size_t Affector::getNumKeyFrames() const
{
    return d_keyFrames.size();
}

//----------------------------------------------------------------------------//
void Affector::moveKeyFrameToPosition(KeyFrame* keyframe, float newPosition)
{
	if (keyframe->getPosition() == newPosition)
		return;

    if (d_keyFrames.find(newPosition) != d_keyFrames.end())
    {
        CEGUI_THROW(InvalidRequestException(
                    "There is already a key frame at position: " +
                    PropertyHelper<float>::toString(newPosition) + "."));
	}

    for (KeyFrameMap::iterator it = d_keyFrames.begin(); it != d_keyFrames.end(); ++it)
    {
        if (it->second == keyframe)
        {
            d_keyFrames.erase(it);
            d_keyFrames.insert(std::make_pair(newPosition, keyframe));

            keyframe->notifyPositionChanged(newPosition);
            return;
        }
    }

    CEGUI_THROW(UnknownObjectException(
        "passed key frame wasn't found within this affector"));
}

//----------------------------------------------------------------------------//
void Affector::moveKeyFrameToPosition(float oldPosition, float newPosition)
{
    KeyFrame* kf = getKeyFrameAtPosition(oldPosition);

    moveKeyFrameToPosition(kf, newPosition);
}

//----------------------------------------------------------------------------//
void Affector::savePropertyValues(AnimationInstance* instance)
{
    switch (d_applicationMethod)
    {
    case AM_Relative:
    case AM_RelativeMultiply:
        instance->savePropertyValue(d_targetProperty);
        break;

    default:
        break;
    }

    // now let all keyframes save their desired property values too
    for (KeyFrameMap::const_iterator it = d_keyFrames.begin();
         it != d_keyFrames.end(); ++it)
    {
        it->second->savePropertyValue(instance);
    }
}

//----------------------------------------------------------------------------//
void Affector::apply(AnimationInstance* instance)
{
    PropertySet* target = instance->getTarget();
    const float position = instance->getPosition();

    // special case
    if (d_keyFrames.empty())
    {
        return;
    }

    if (d_targetProperty.empty())
    {
        Logger::getSingleton().logEvent(
            "Affector can't be applied when target property is empty!", Warnings);
        return;
    }

    if (!d_interpolator)
    {
        Logger::getSingleton().logEvent(
            "Affector can't be applied when no interpolator is set!", Warnings);
        return;
    }

    KeyFrame* left = 0;
    KeyFrame* right = 0;

    // find 2 neighbouring keyframes
    for (KeyFrameMap::const_iterator it = d_keyFrames.begin();
         it != d_keyFrames.end(); ++it)
    {
        KeyFrame* current = it->second;

        if (current->getPosition() <= position)
        {
            left = current;
        }

        if (current->getPosition() >= position && !right)
        {
            right = current;
        }
    }

    float leftDistance, rightDistance;

    if (left)
    {
        leftDistance = position - left->getPosition();
    }
    else
        // if no keyframe is suitable for left neighbour, pick the first one
    {
        left = d_keyFrames.begin()->second;
        leftDistance = 0;
    }

    if (right)
    {
        rightDistance = right->getPosition() - position;
    }
    else
        // if no keyframe is suitable for the right neighbour, pick the last one
    {
        right = d_keyFrames.rbegin()->second;
        rightDistance = 0;
    }

    // if there is just one keyframe and we are right on it
    if (leftDistance + rightDistance == 0)
    {
        leftDistance = rightDistance = 0.5;
    }

    // alter interpolation position using the right neighbours progression
    // method
    const float interpolationPosition =
        right->alterInterpolationPosition(
            leftDistance / (leftDistance + rightDistance));

    // absolute application method
    if (d_applicationMethod == AM_Absolute)
    {
        const String result = d_interpolator->interpolateAbsolute(
                                  left->getValueForAnimation(instance),
                                  right->getValueForAnimation(instance),
                                  interpolationPosition);

        target->setProperty(d_targetProperty, result);
    }
    // relative application method
    else if (d_applicationMethod == AM_Relative)
    {
        const String& base = instance->getSavedPropertyValue(getTargetProperty());

        const String result = d_interpolator->interpolateRelative(
                                  base,
                                  left->getValueForAnimation(instance),
                                  right->getValueForAnimation(instance),
                                  interpolationPosition);

        target->setProperty(d_targetProperty, result);
    }
    // relative multiply application method
    else if (d_applicationMethod == AM_RelativeMultiply)
    {
        const String& base = instance->getSavedPropertyValue(getTargetProperty());

        const String result = d_interpolator->interpolateRelativeMultiply(
                                  base,
                                  left->getValueForAnimation(instance),
                                  right->getValueForAnimation(instance),
                                  interpolationPosition);

        target->setProperty(d_targetProperty, result);
    }
    // todo: more application methods?
    else
    {
        assert(0);
    }
}

void Affector::writeXMLToStream(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(AnimationAffectorHandler::ElementName);

    String applicationMethod;
    switch(getApplicationMethod())
    {
    case AM_Absolute:
        applicationMethod = AnimationAffectorHandler::ApplicationMethodAbsolute;
        break;
    case AM_Relative:
        applicationMethod = AnimationAffectorHandler::ApplicationMethodRelative;
        break;
    case AM_RelativeMultiply:
        applicationMethod = AnimationAffectorHandler::ApplicationMethodRelativeMultiply;
        break;

    default:
        assert(0 && "How did we get here?");
        break;
    }

    xml_stream.attribute(AnimationAffectorHandler::ApplicationMethodAttribute, applicationMethod);

    xml_stream.attribute(AnimationAffectorHandler::TargetPropertyAttribute, getTargetProperty());

    if (getInterpolator())
    {
        xml_stream.attribute(AnimationAffectorHandler::InterpolatorAttribute, getInterpolator()->getType());
    }

    for (KeyFrameMap::const_iterator it = d_keyFrames.begin();
         it != d_keyFrames.end(); ++it)
    {
        it->second->writeXMLToStream(xml_stream);
    }

    xml_stream.closeTag();
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

