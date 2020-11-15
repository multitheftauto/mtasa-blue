/***********************************************************************
    created:    7/8/2010
    author:     Martin Preisler

    purpose:    Implements the AnimationManager class
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
#include "CEGUI/AnimationManager.h"
#include "CEGUI/Logger.h"
#include "CEGUI/Animation.h"
#include "CEGUI/AnimationInstance.h"
#include "CEGUI/TplInterpolators.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/System.h"
#include "CEGUI/XMLParser.h"
#include "CEGUI/Animation_xmlHandler.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
    Static Data Definitions
*************************************************************************/
// singleton instance pointer
template<> AnimationManager* Singleton<AnimationManager>::ms_Singleton  = 0;
// Name of the xsd schema file used to validate animation XML files.
const String AnimationManager::XMLSchemaName("Animation.xsd");
// String that holds the default resource group for loading animations
String AnimationManager::s_defaultResourceGroup("");
const String AnimationManager::GeneratedAnimationNameBase("__ceanim_uid_");

/*************************************************************************
    Constructor
*************************************************************************/
AnimationManager::AnimationManager(void)
{
    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::AnimationManager singleton created " + String(addr_buff));

    // todo: is this too dirty?
#   define addBasicInterpolator(i) { Interpolator* in = i; addInterpolator(in); d_basicInterpolators.push_back(in); }

    // create and add basic interpolators shipped with CEGUI
    addBasicInterpolator(CEGUI_NEW_AO TplDiscreteRelativeInterpolator<String>("String"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<float>("float"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<int>("int"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<uint>("uint"));
    addBasicInterpolator(CEGUI_NEW_AO TplDiscreteInterpolator<bool>("bool"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<Sizef >("Sizef"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<Vector2f >("Vector2f"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<Vector3f >("Vector3f"));
    addBasicInterpolator(CEGUI_NEW_AO QuaternionSlerpInterpolator());
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<Rectf >("Rectf"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<Colour>("Colour"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<ColourRect>("ColourRect"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<UDim>("UDim"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<UVector2>("UVector2"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<URect>("URect"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<UBox>("UBox"));
    addBasicInterpolator(CEGUI_NEW_AO TplLinearInterpolator<USize>("USize"));
}


/*************************************************************************
    Destructor
*************************************************************************/
AnimationManager::~AnimationManager()
{
    // by destroying all animations their instances also get deleted
    destroyAllAnimations();

    // and lastly, we remove all interpolators, but we don't delete them!
    // it is the creator's responsibility to delete them
    d_interpolators.clear();

    // we only destroy inbuilt interpolators
    for (BasicInterpolatorList::const_iterator it = d_basicInterpolators.begin();
         it != d_basicInterpolators.end(); ++it)
    {
        CEGUI_DELETE_AO *it;
    }

    d_basicInterpolators.clear();

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::AnimationManager singleton destroyed " + String(addr_buff));
}

//----------------------------------------------------------------------------//
void AnimationManager::addInterpolator(Interpolator* interpolator)
{
    if (d_interpolators.find(interpolator->getType()) != d_interpolators.end())
    {
        CEGUI_THROW(AlreadyExistsException("Interpolator of type '"
            + interpolator->getType() + "' already exists."));
    }

    d_interpolators.insert(
        std::make_pair(interpolator->getType(), interpolator));
}

//----------------------------------------------------------------------------//
void AnimationManager::removeInterpolator(Interpolator* interpolator)
{
    InterpolatorMap::iterator it = d_interpolators.find(interpolator->getType());

    if (it == d_interpolators.end())
    {
        CEGUI_THROW(UnknownObjectException("Interpolator of type '"
            + interpolator->getType() + "' not found."));
    }

    d_interpolators.erase(it);
}

//----------------------------------------------------------------------------//
Interpolator* AnimationManager::getInterpolator(const String& type) const
{
    InterpolatorMap::const_iterator it = d_interpolators.find(type);

    if (it == d_interpolators.end())
    {
        CEGUI_THROW(UnknownObjectException("Interpolator of type '" + type +
            "' not found."));
    }

    return it->second;
}

//----------------------------------------------------------------------------//
Animation* AnimationManager::createAnimation(const String& name)
{
    if (isAnimationPresent(name))
    {
        CEGUI_THROW(UnknownObjectException("Animation with name '"
            + name + "' already exists."));
    }

    String finalName(name.empty() ? generateUniqueAnimationName() : name);

    Animation* ret = CEGUI_NEW_AO Animation(finalName);
    d_animations.insert(std::make_pair(finalName, ret));

    return ret;
}

//----------------------------------------------------------------------------//
void AnimationManager::destroyAnimation(Animation* animation)
{
    destroyAnimation(animation->getName());
}

//----------------------------------------------------------------------------//
void AnimationManager::destroyAnimation(const String& name)
{
    AnimationMap::iterator it = d_animations.find(name);

    if (it == d_animations.end())
    {
        CEGUI_THROW(UnknownObjectException("Animation with name '" + name
            + "' not found."));
    }

    Animation* animation = it->second;
    destroyAllInstancesOfAnimation(animation);

    d_animations.erase(it);
    CEGUI_DELETE_AO animation;
}

//----------------------------------------------------------------------------//
void AnimationManager::destroyAllAnimations()
{
    // we have to destroy all instances to avoid dangling pointers
    // destroying all instances now is also faster than doing that for each
    // animation that is being destroyed
    destroyAllAnimationInstances();
    
    for (AnimationMap::const_iterator it = d_animations.begin();
         it != d_animations.end(); ++it)
    {
        CEGUI_DELETE_AO it->second;
    }

    d_animations.clear();
}

//----------------------------------------------------------------------------//
Animation* AnimationManager::getAnimation(const String& name) const
{
    AnimationMap::const_iterator it = d_animations.find(name);

    if (it == d_animations.end())
    {
        CEGUI_THROW(UnknownObjectException("Animation with name '" + name
            + "' not found."));
    }

    return it->second;
}

//----------------------------------------------------------------------------//
bool AnimationManager::isAnimationPresent(const String& name) const
{
    return (d_animations.find(name) != d_animations.end());
}

//----------------------------------------------------------------------------//
Animation* AnimationManager::getAnimationAtIdx(size_t index) const
{
    if (index >= d_animations.size())
    {
        CEGUI_THROW(InvalidRequestException("Out of bounds."));
    }

    AnimationMap::const_iterator it = d_animations.begin();
    std::advance(it, index);

    return it->second;
}

//----------------------------------------------------------------------------//
size_t AnimationManager::getNumAnimations() const
{
    return d_animations.size();
}

//----------------------------------------------------------------------------//
AnimationInstance* AnimationManager::instantiateAnimation(Animation* animation)
{
	if (!animation)
	{
		CEGUI_THROW(InvalidRequestException("I refuse to instantiate NULL "
            "animation, please provide a valid pointer."));
	}

    AnimationInstance* ret = CEGUI_NEW_AO AnimationInstance(animation);
    d_animationInstances.insert(std::make_pair(animation, ret));

    return ret;
}

//----------------------------------------------------------------------------//
AnimationInstance* AnimationManager::instantiateAnimation(const String& name)
{
    return instantiateAnimation(getAnimation(name));
}

//----------------------------------------------------------------------------//
void AnimationManager::destroyAnimationInstance(AnimationInstance* instance)
{
    AnimationInstanceMap::iterator it =
        d_animationInstances.find(instance->getDefinition());

    for (; it != d_animationInstances.end(); ++it)
    {
        if (it->second == instance)
        {
            d_animationInstances.erase(it);
            CEGUI_DELETE_AO instance;
            return;
        }
    }

    CEGUI_THROW(InvalidRequestException("Given animation instance not found."));
}

//----------------------------------------------------------------------------//
void AnimationManager::destroyAllInstancesOfAnimation(Animation* animation)
{
    AnimationInstanceMap::iterator it = d_animationInstances.find(animation);

    // the first instance of given animation is now it->second (if there is any)
    while (it != d_animationInstances.end() && it->first == animation)
    {
        AnimationInstanceMap::iterator toErase = it;
        ++it;

        CEGUI_DELETE_AO toErase->second;
        d_animationInstances.erase(toErase);
    }
}

//----------------------------------------------------------------------------//
void AnimationManager::destroyAllAnimationInstances()
{
    for (AnimationInstanceMap::const_iterator it = d_animationInstances.begin();
         it != d_animationInstances.end(); ++it)
    {
        CEGUI_DELETE_AO it->second;
    }

    d_animationInstances.clear();
}

//----------------------------------------------------------------------------//
AnimationInstance* AnimationManager::getAnimationInstanceAtIdx(size_t index) const
{
    if (index >= d_animationInstances.size())
    {
        CEGUI_THROW(InvalidRequestException("Out of bounds."));
    }

    AnimationInstanceMap::const_iterator it = d_animationInstances.begin();
    std::advance(it, index);

    return it->second;
}

//----------------------------------------------------------------------------//
size_t AnimationManager::getNumAnimationInstances() const
{
    return d_animationInstances.size();
}

//----------------------------------------------------------------------------//
void AnimationManager::autoStepInstances(float delta)
{
    for (AnimationInstanceMap::const_iterator it = d_animationInstances.begin();
         it != d_animationInstances.end(); ++it)
    {
    	if (it->second->isAutoSteppingEnabled())
    		it->second->step(delta);
    }
}

//----------------------------------------------------------------------------//
void AnimationManager::loadAnimationsFromXML(const String& filename,
                                             const String& resourceGroup)
{
    if (filename.empty())
        CEGUI_THROW(InvalidRequestException(
            "filename supplied for file loading must be valid."));

    Animation_xmlHandler handler;

    // do parse (which uses handler to create actual data)
    CEGUI_TRY
    {
        System::getSingleton().getXMLParser()->
            parseXMLFile(handler, filename, XMLSchemaName,
                         resourceGroup.empty() ? s_defaultResourceGroup :
                                                 resourceGroup);
    }
    CEGUI_CATCH(...)
    {
        Logger::getSingleton().logEvent(
            "AnimationManager::loadAnimationsFromXML: "
            "loading of animations from file '" + filename + "' has failed.",
            Errors);

        CEGUI_RETHROW;
    }
}

void AnimationManager::loadAnimationsFromString(const String& source)
{
    Animation_xmlHandler handler;

    // do parse (which uses handler to create actual data)
    CEGUI_TRY
    {
        System::getSingleton().getXMLParser()->parseXMLString(handler, source, XMLSchemaName);
    }
    CEGUI_CATCH(...)
    {
        Logger::getSingleton().logEvent("AnimationManager::loadAnimationsFromString - loading of animations from string failed.", Errors);
        CEGUI_RETHROW;
    }
}

//---------------------------------------------------------------------------//
void AnimationManager::writeAnimationDefinitionToStream(const Animation& animation, OutStream& out_stream) const
{
    XMLSerializer xml(out_stream);

    animation.writeXMLToStream(xml);
}

//---------------------------------------------------------------------------//
String AnimationManager::getAnimationDefinitionAsString(const Animation& animation) const
{
    std::ostringstream str;
    writeAnimationDefinitionToStream(animation, str);

    return String(reinterpret_cast<const encoded_char*>(str.str().c_str()));
}

//---------------------------------------------------------------------------//
String AnimationManager::generateUniqueAnimationName()
{
    const String ret = GeneratedAnimationNameBase +
        PropertyHelper<unsigned long>::toString(d_uid_counter);

    // update counter for next time
    unsigned long old_uid = d_uid_counter;
    ++d_uid_counter;

    // log if we ever wrap-around (which should be pretty unlikely)
    if (d_uid_counter < old_uid)
        Logger::getSingleton().logEvent("UID counter for generated Animation "
            "names has wrapped around - the fun shall now commence!");

    return ret;
}

} // End of  CEGUI namespace section
