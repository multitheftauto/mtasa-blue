/***********************************************************************
    created:    Wed Aug 11 2010
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "CEGUI/Animation_xmlHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"
#include "CEGUI/XMLAttributes.h"

#include "CEGUI/AnimationManager.h"
#include "CEGUI/Animation.h"
#include "CEGUI/Affector.h"
#include "CEGUI/KeyFrame.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//

const String AnimationDefinitionHandler::ElementName("AnimationDefinition");
const String AnimationAffectorHandler::ElementName("Affector");
const String AnimationKeyFrameHandler::ElementName("KeyFrame");
const String AnimationSubscriptionHandler::ElementName("Subscription");

const String Animation_xmlHandler::ElementName("Animations");

const String AnimationDefinitionHandler::NameAttribute("name");
const String AnimationDefinitionHandler::DurationAttribute("duration");
const String AnimationDefinitionHandler::ReplayModeAttribute("replayMode");
const String AnimationDefinitionHandler::ReplayModeOnce("once");
const String AnimationDefinitionHandler::ReplayModeLoop("loop");
const String AnimationDefinitionHandler::ReplayModeBounce("bounce");
const String AnimationDefinitionHandler::AutoStartAttribute("autoStart");

const String AnimationAffectorHandler::InterpolatorAttribute("interpolator");
const String AnimationAffectorHandler::TargetPropertyAttribute("property");
const String AnimationAffectorHandler::ApplicationMethodAttribute("applicationMethod");
const String AnimationAffectorHandler::ApplicationMethodAbsolute("absolute");
const String AnimationAffectorHandler::ApplicationMethodRelative("relative");
const String AnimationAffectorHandler::ApplicationMethodRelativeMultiply("relative multiply");

const String AnimationKeyFrameHandler::PositionAttribute("position");
const String AnimationKeyFrameHandler::ValueAttribute("value");
const String AnimationKeyFrameHandler::SourcePropertyAttribute("sourceProperty");
const String AnimationKeyFrameHandler::ProgressionAttribute("progression");
const String AnimationKeyFrameHandler::ProgressionLinear("linear");
const String AnimationKeyFrameHandler::ProgressionDiscrete("discrete");
const String AnimationKeyFrameHandler::ProgressionQuadraticAccelerating("quadratic accelerating");
const String AnimationKeyFrameHandler::ProgressionQuadraticDecelerating("quadratic decelerating");

const String AnimationSubscriptionHandler::EventAttribute("event");
const String AnimationSubscriptionHandler::ActionAttribute("action");

//----------------------------------------------------------------------------//
Animation_xmlHandler::Animation_xmlHandler()
{}

//----------------------------------------------------------------------------//
Animation_xmlHandler::~Animation_xmlHandler()
{}

//----------------------------------------------------------------------------//
const String& Animation_xmlHandler::getSchemaName() const
{
    return AnimationManager::XMLSchemaName;
}

//----------------------------------------------------------------------------//
const String& Animation_xmlHandler::getDefaultResourceGroup() const
{
    return AnimationManager::getDefaultResourceGroup();
}

//----------------------------------------------------------------------------//
void Animation_xmlHandler::elementStartLocal(const String& element,
                                             const XMLAttributes& attributes)
{
    if (element == ElementName)
    {
        CEGUI_LOGINSANE("===== Begin Animations parsing =====");
    }
    else if (element == AnimationDefinitionHandler::ElementName)
    {
        d_chainedHandler = CEGUI_NEW_AO AnimationDefinitionHandler(attributes, "");
    }
    else
        Logger::getSingleton().logEvent("Animation_xmlHandler::elementStart: "
            "<" + element + "> is invalid at this location.", Errors);
}

//----------------------------------------------------------------------------//
void Animation_xmlHandler::elementEndLocal(const String& element)
{
    if (element == ElementName)
    {
        CEGUI_LOGINSANE("===== End Animations parsing =====");
    }
    else
        Logger::getSingleton().logEvent("Animation_xmlHandler::elementEnd: "
            "</" + element + "> is invalid at this location.", Errors);
}

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
AnimationDefinitionHandler::AnimationDefinitionHandler(
                                const XMLAttributes& attributes,
                                const String& name_prefix) :
    d_anim(0)
{
    const String anim_name(name_prefix +
                           attributes.getValueAsString(NameAttribute));

    CEGUI_LOGINSANE(
        "Defining animation named: " +
        anim_name +
        "  Duration: " +
        attributes.getValueAsString(DurationAttribute) +
        "  Replay mode: " +
        attributes.getValueAsString(ReplayModeAttribute) +
        "  Auto start: " +
        attributes.getValueAsString(AutoStartAttribute, "false"));

    d_anim = AnimationManager::getSingleton().createAnimation(anim_name);

    d_anim->setDuration(attributes.getValueAsFloat(DurationAttribute));

    const String replayMode(attributes.getValueAsString(ReplayModeAttribute,
                                                        ReplayModeLoop));
    if (replayMode == ReplayModeOnce)
        d_anim->setReplayMode(Animation::RM_Once);
    else if (replayMode == ReplayModeBounce)
        d_anim->setReplayMode(Animation::RM_Bounce);
    else
        d_anim->setReplayMode(Animation::RM_Loop);

    d_anim->setAutoStart(attributes.getValueAsBool(AutoStartAttribute));
}

//----------------------------------------------------------------------------//
AnimationDefinitionHandler::~AnimationDefinitionHandler()
{
}

//----------------------------------------------------------------------------//
void AnimationDefinitionHandler::elementStartLocal(
                                            const String& element,
                                            const XMLAttributes& attributes)
{
    if (element == AnimationAffectorHandler::ElementName)
        d_chainedHandler = CEGUI_NEW_AO AnimationAffectorHandler(attributes, *d_anim);
    else if (element == AnimationSubscriptionHandler::ElementName)
        d_chainedHandler = CEGUI_NEW_AO AnimationSubscriptionHandler(attributes, *d_anim);
    else
        Logger::getSingleton().logEvent(
            "AnimationDefinitionHandler::elementStart: "
            "<" + element + "> is invalid at this location.", Errors);
}

//----------------------------------------------------------------------------//
void AnimationDefinitionHandler::elementEndLocal(const String& element)
{
    // set completed status when we encounter our own end element
    if (element == ElementName)
        d_completed = true;
}

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
AnimationAffectorHandler::AnimationAffectorHandler(
                                            const XMLAttributes& attributes,
                                            Animation& anim) :
    d_affector(0)
{
    CEGUI_LOGINSANE(
        "\tAdding affector for property: " +
        attributes.getValueAsString(TargetPropertyAttribute) +
        "  Interpolator: " +
        attributes.getValueAsString(InterpolatorAttribute) +
        "  Application method: " +
        attributes.getValueAsString(ApplicationMethodAttribute, "absolute"));

    d_affector = anim.createAffector(
        attributes.getValueAsString(TargetPropertyAttribute),
        attributes.getValueAsString(InterpolatorAttribute));

    if (attributes.getValueAsString(ApplicationMethodAttribute) ==
        ApplicationMethodRelative)
    {
        d_affector->setApplicationMethod(Affector::AM_Relative);
    }
	else if (attributes.getValueAsString(ApplicationMethodAttribute) ==
        ApplicationMethodRelativeMultiply)
    {
        d_affector->setApplicationMethod(Affector::AM_RelativeMultiply);
    }
    else
    {
        d_affector->setApplicationMethod(Affector::AM_Absolute);
    }
}

//----------------------------------------------------------------------------//
AnimationAffectorHandler::~AnimationAffectorHandler()
{
}

//----------------------------------------------------------------------------//
void AnimationAffectorHandler::elementStartLocal(
                                        const String& element,
                                        const XMLAttributes& attributes)
{
    if (element == AnimationKeyFrameHandler::ElementName)
        d_chainedHandler = CEGUI_NEW_AO AnimationKeyFrameHandler(attributes, *d_affector);
    else
        Logger::getSingleton().logEvent(
            "AnimationAffectorHandler::elementStart: "
            "<" + element + "> is invalid at this location.", Errors);
}

//----------------------------------------------------------------------------//
void AnimationAffectorHandler::elementEndLocal(const String& element)
{
    // set completed status when we encounter our own end element
    if (element == ElementName)
        d_completed = true;
}

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
AnimationKeyFrameHandler::AnimationKeyFrameHandler(
                                        const XMLAttributes& attributes,
                                        Affector& affector)
{
    const String progressionStr(
        attributes.getValueAsString(ProgressionAttribute));

    String log_event(
        "\t\tAdding KeyFrame at position: " +
        attributes.getValueAsString(PositionAttribute) +
        "  Value: " +
        attributes.getValueAsString(ValueAttribute));

    if (!progressionStr.empty())
        log_event.append("  Progression: " +
            attributes.getValueAsString(ProgressionAttribute, ProgressionLinear));

    CEGUI_LOGINSANE(log_event);

    KeyFrame::Progression progression;
    if (progressionStr == ProgressionDiscrete)
        progression = KeyFrame::P_Discrete;
    else if (progressionStr == ProgressionQuadraticAccelerating)
        progression = KeyFrame::P_QuadraticAccelerating;
    else if (progressionStr == ProgressionQuadraticDecelerating)
        progression = KeyFrame::P_QuadraticDecelerating;
    else
        progression = KeyFrame::P_Linear;

    affector.createKeyFrame(
        attributes.getValueAsFloat(PositionAttribute),
        attributes.getValueAsString(ValueAttribute),
        progression,
        attributes.getValueAsString(SourcePropertyAttribute));
    
    if (affector.getNumKeyFrames() == 1 && !progressionStr.empty())
        Logger::getSingleton().logEvent(
            "WARNING: progression type specified for first keyframe in "
            "animation will be ignored.");

    d_completed = true;
}

//----------------------------------------------------------------------------//
AnimationKeyFrameHandler::~AnimationKeyFrameHandler()
{
}

//----------------------------------------------------------------------------//
void AnimationKeyFrameHandler::elementStartLocal(
                                        const String& element,
                                        const XMLAttributes& /*attributes*/)
{
    Logger::getSingleton().logEvent(
        "AnimationAffectorHandler::elementStart: "
        "<" + element + "> is invalid at this location.", Errors);
}

//----------------------------------------------------------------------------//
void AnimationKeyFrameHandler::elementEndLocal(const String& element)
{
    // set completed status when we encounter our own end element
    if (element == ElementName)
        d_completed = true;
}

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
AnimationSubscriptionHandler::AnimationSubscriptionHandler(
                                        const XMLAttributes& attributes,
                                        Animation& anim)
{
    CEGUI_LOGINSANE(
        "\tAdding subscription to event: " +
        attributes.getValueAsString(EventAttribute) + 
        "  Action: " +
        attributes.getValueAsString(ActionAttribute));

    anim.defineAutoSubscription(
        attributes.getValueAsString(EventAttribute),
        attributes.getValueAsString(ActionAttribute));

    d_completed = true;
}

//----------------------------------------------------------------------------//
AnimationSubscriptionHandler::~AnimationSubscriptionHandler()
{
}

//----------------------------------------------------------------------------//
void AnimationSubscriptionHandler::elementStartLocal(
                                            const String& element,
                                            const XMLAttributes& /*attributes*/)
{
    Logger::getSingleton().logEvent(
        "AnimationAffectorHandler::elementStart: "
        "</" + element + "> is invalid at this location.", Errors);
}

//----------------------------------------------------------------------------//
void AnimationSubscriptionHandler::elementEndLocal(const String& element)
{
    // set completed status when we encounter our own end element
    if (element == ElementName)
        d_completed = true;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

