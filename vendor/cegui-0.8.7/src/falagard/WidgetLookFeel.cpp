/***********************************************************************
    created:    Mon Jun 13 2005
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
#include "CEGUI/falagard/WidgetLookFeel.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/falagard/XMLHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/AnimationManager.h"
#include "CEGUI/AnimationInstance.h"
#include <iostream>
#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
//---------------------------------------------------------------------------//
WidgetLookFeel::WidgetLookFeel(const String& name, const String& inherits) :
    d_lookName(name),
    d_inheritedLookName(inherits)
{
}

//---------------------------------------------------------------------------//
WidgetLookFeel::WidgetLookFeel(const WidgetLookFeel& other) :
    d_lookName(other.d_lookName),
    d_inheritedLookName(other.d_inheritedLookName),
    d_imagerySections(other.d_imagerySections),
    d_childWidgets(other.d_childWidgets),
    d_stateImagery(other.d_stateImagery),
    d_properties(other.d_properties),
    d_namedAreas(other.d_namedAreas),
    d_animations(other.d_animations),
    d_animationInstances(other.d_animationInstances),
    d_eventLinkDefinitions(other.d_eventLinkDefinitions)
{
    for (PropertyDefinitionList::iterator i = other.d_propertyDefinitions.begin();
        i < other.d_propertyDefinitions.end();
        ++i)
    {
        d_propertyDefinitions.push_back(
            dynamic_cast<PropertyDefinitionBase*>(
                dynamic_cast<Property*>(*i)->clone()));
    }
    
    for (PropertyLinkDefinitionList::iterator i = other.d_propertyLinkDefinitions.begin();
        i < other.d_propertyLinkDefinitions.end();
        ++i)
    {
        d_propertyLinkDefinitions.push_back(
            dynamic_cast<PropertyDefinitionBase*>(
                dynamic_cast<Property*>(*i)->clone()));
    }
}

//---------------------------------------------------------------------------//
WidgetLookFeel& WidgetLookFeel::operator=(const WidgetLookFeel& other)
{
    WidgetLookFeel tmp(other);
    swap(tmp);
    return *this;
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::swap(WidgetLookFeel& other)
{
    std::swap(d_lookName, other.d_lookName);
    std::swap(d_inheritedLookName, other.d_inheritedLookName);
    std::swap(d_imagerySections, other.d_imagerySections);
    std::swap(d_childWidgets, other.d_childWidgets);
    std::swap(d_stateImagery, other.d_stateImagery);
    std::swap(d_properties, other.d_properties);
    std::swap(d_namedAreas, other.d_namedAreas);
    std::swap(d_propertyDefinitions, other.d_propertyDefinitions);
    std::swap(d_propertyLinkDefinitions, other.d_propertyLinkDefinitions);
    std::swap(d_animations, other.d_animations);
    std::swap(d_animationInstances, other.d_animationInstances);
    std::swap(d_eventLinkDefinitions, other.d_eventLinkDefinitions);
}

//---------------------------------------------------------------------------//
WidgetLookFeel::~WidgetLookFeel()
{
    for (PropertyDefinitionList::reverse_iterator it = d_propertyDefinitions.rbegin();
        it < d_propertyDefinitions.rend(); ++it)
        CEGUI_DELETE_AO (*it);

    for (PropertyLinkDefinitionList::reverse_iterator it = d_propertyLinkDefinitions.rbegin();
        it < d_propertyLinkDefinitions.rend(); ++it)
        CEGUI_DELETE_AO (*it);
}

//---------------------------------------------------------------------------//
const StateImagery& WidgetLookFeel::getStateImagery(
                                        const CEGUI::String& state) const
{
    StateList::const_iterator imagery = d_stateImagery.find(state);

    if (imagery != d_stateImagery.end())
        return (*imagery).second;

    if (d_inheritedLookName.empty())
        CEGUI_THROW(UnknownObjectException("unknown state '" + state +
            "' in look '" + d_lookName + "'."));

    return WidgetLookManager::getSingleton().
        getWidgetLook(d_inheritedLookName).getStateImagery(state);
}

//---------------------------------------------------------------------------//
const ImagerySection& WidgetLookFeel::getImagerySection(
                                        const CEGUI::String& section) const
{
    ImageryList::const_iterator imgSect = d_imagerySections.find(section);

    if (imgSect != d_imagerySections.end())
        return (*imgSect).second;

    if (d_inheritedLookName.empty())
        CEGUI_THROW(UnknownObjectException("unknown imagery section '" +
            section +  "' in look '" + d_lookName + "'."));

    return WidgetLookManager::getSingleton().
        getWidgetLook(d_inheritedLookName).getImagerySection(section);
}

//---------------------------------------------------------------------------//
const String& WidgetLookFeel::getName() const
{
    return d_lookName;
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::addImagerySection(const ImagerySection& section)
{
    if (d_imagerySections.find(section.getName()) != d_imagerySections.end())
        Logger::getSingleton().logEvent(
            "WidgetLookFeel::addImagerySection - Defintion for imagery "
            "section '" + section.getName() + "' already exists.  "
            "Replacing previous definition.");

    d_imagerySections[section.getName()] = section;
}
//---------------------------------------------------------------------------//
void WidgetLookFeel::renameImagerySection(const String& oldName, const String& newName)
{
	ImageryList::iterator oldsection = d_imagerySections.find(oldName);
    if (oldsection == d_imagerySections.end())
        CEGUI_THROW(UnknownObjectException("unknown imagery section: '" +
            oldName + "' in look '" + d_lookName + "'."));

    if (d_imagerySections.find(newName) != d_imagerySections.end())
        CEGUI_THROW(UnknownObjectException("imagery section: '" + newName +
            "' already exists in look '" + d_lookName + "'."));

    oldsection->second.setName(newName);
    d_imagerySections[newName] = d_imagerySections[oldName];
    d_imagerySections.erase(oldsection);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::addWidgetComponent(const WidgetComponent& widget)
{
    d_childWidgets.push_back(widget);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::addStateSpecification(const StateImagery& state)
{
    if (d_stateImagery.find(state.getName()) != d_stateImagery.end())
        Logger::getSingleton().logEvent(
            "WidgetLookFeel::addStateSpecification - Defintion for state '" +
            state.getName() + "' already exists.  Replacing previous "
            "definition.");

    d_stateImagery[state.getName()] = state;
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::addPropertyInitialiser(
                        const PropertyInitialiser& initialiser)
{
    d_properties.push_back(initialiser);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::clearImagerySections()
{
    d_imagerySections.clear();
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::clearWidgetComponents()
{
    d_childWidgets.clear();
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::clearStateSpecifications()
{
    d_stateImagery.clear();
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::clearPropertyInitialisers()
{
    d_properties.clear();
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::initialiseWidget(Window& widget) const
{
    // add new property definitions
    PropertyDefinitionCollator pdc;
    appendPropertyDefinitions(pdc);
    for (PropertyDefinitionCollator::const_iterator pdi = pdc.begin();
         pdi != pdc.end();
         ++pdi)
    {
        PropertyDefinitionBase* propDefBase = *pdi;
        Property* curProperty = dynamic_cast<Property*>(propDefBase);
        // add the property to the window
        widget.addProperty(curProperty);
    }

    // add required child widgets
    WidgetComponentCollator wcc;
    appendChildWidgetComponents(wcc);
    for (WidgetComponentCollator::const_iterator wci = wcc.begin();
         wci != wcc.end();
         ++wci)
    {
        (*wci)->create(widget);
    }

    // add new property link definitions
    PropertyLinkDefinitionCollator pldc;
    appendPropertyLinkDefinitions(pldc);
    for (PropertyLinkDefinitionCollator::const_iterator pldi = pldc.begin();
         pldi != pldc.end();
         ++pldi)
    {
        // add the property to the window
        widget.addProperty(dynamic_cast<Property*>(*pldi));
    }
    // apply properties to the parent window
    PropertyInitialiserCollator pic;
    appendPropertyInitialisers(pic);
    for (PropertyInitialiserCollator::const_iterator pi = pic.begin();
         pi != pic.end();
         ++pi)
    {
        (*pi)->apply(widget);
    }

    // setup linked events
    EventLinkDefinitionCollator eldc;
    appendEventLinkDefinitions(eldc);
    for (EventLinkDefinitionCollator::const_iterator eldi = eldc.begin();
         eldi != eldc.end();
         ++eldi)
    {
        (*eldi)->initialiseWidget(widget);
    }

    // create animation instances
    AnimationNameSet ans;
    appendAnimationNames(ans);
    for (AnimationNameSet::const_iterator ani = ans.begin();
         ani != ans.end();
         ++ani)
    {
        AnimationInstance* instance =
            AnimationManager::getSingleton().instantiateAnimation(*ani);

        d_animationInstances.insert(std::make_pair(&widget, instance));
        instance->setTargetWindow(&widget);
    }
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::cleanUpWidget(Window& widget) const
{
    if (widget.getLookNFeel() != getName())
    {
        CEGUI_THROW(InvalidRequestException("The window '" + 
            widget.getNamePath() + "' does not have this WidgetLook assigned"));
    }

    // remove added child widgets
    WidgetComponentCollator wcc;
    appendChildWidgetComponents(wcc);
    for (WidgetComponentCollator::const_iterator wci = wcc.begin();
         wci != wcc.end();
         ++wci)
    {
        (*wci)->cleanup(widget);
    }

    // delete added named Events
    EventLinkDefinitionCollator eldc;
    appendEventLinkDefinitions(eldc);
    for (EventLinkDefinitionCollator::const_iterator eldi = eldc.begin();
         eldi != eldc.end();
         ++eldi)
    {
        (*eldi)->cleanUpWidget(widget);
    }

    // remove added property definitions
    PropertyDefinitionCollator pdc;
    appendPropertyDefinitions(pdc);
    for (PropertyDefinitionCollator::const_iterator pdi = pdc.begin();
         pdi != pdc.end();
         ++pdi)
    {
        // remove the property from the window
        widget.removeProperty((*pdi)->getPropertyName());
    }

    // remove added property link definitions
    PropertyLinkDefinitionCollator pldc;
    appendPropertyLinkDefinitions(pldc);
    for (PropertyLinkDefinitionCollator::const_iterator pldi = pldc.begin();
         pldi != pldc.end();
         ++pldi)
    {
        // remove the property from the window
        widget.removeProperty((*pldi)->getPropertyName());
    }

    // clean up animation instances assoicated wit the window.
    AnimationInstanceMap::iterator anim;
    while ((anim = d_animationInstances.find(&widget)) != d_animationInstances.end())
    {
        AnimationManager::getSingleton().destroyAnimationInstance(anim->second);
        d_animationInstances.erase(anim);
    }
}

//---------------------------------------------------------------------------//
bool WidgetLookFeel::isStateImageryPresent(const String& state) const
{
    StateList::const_iterator i = d_stateImagery.find(state);
    
    if (i != d_stateImagery.end())
        return true;

    if (d_inheritedLookName.empty())
        return false;
 
    return WidgetLookManager::getSingleton().
        getWidgetLook(d_inheritedLookName).isStateImageryPresent(state);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::addNamedArea(const NamedArea& area)
{
    if (d_namedAreas.find(area.getName()) != d_namedAreas.end())
        Logger::getSingleton().logEvent(
            "WidgetLookFeel::addNamedArea - Defintion for area '" +
            area.getName() + "' already exists.  Replacing previous "
            "definition.");

    d_namedAreas[area.getName()] = area;
}


//---------------------------------------------------------------------------//
void WidgetLookFeel::renameNamedArea(const String& oldName, const String& newName)
{
    NamedAreaList::iterator oldarea = d_namedAreas.find(oldName);
    NamedAreaList::const_iterator newarea = d_namedAreas.find(newName);
    if (oldarea == d_namedAreas.end())
        CEGUI_THROW(UnknownObjectException("unknown named area: '" + oldName +
            "' in look '" + d_lookName + "'."));

    if (newarea != d_namedAreas.end())
        CEGUI_THROW(UnknownObjectException("named area: '" + newName +
            "' already exists in look '" + d_lookName + "'."));

    oldarea->second.setName(newName);
    d_namedAreas[newName] = d_namedAreas[oldName];
    d_namedAreas.erase(oldarea);
}
//---------------------------------------------------------------------------//
void WidgetLookFeel::clearNamedAreas()
{
    d_namedAreas.clear();
}

//---------------------------------------------------------------------------//
const NamedArea& WidgetLookFeel::getNamedArea(const String& name) const
{
    NamedAreaList::const_iterator area = d_namedAreas.find(name);

    if (area != d_namedAreas.end())
        return (*area).second;

    if (d_inheritedLookName.empty())
        CEGUI_THROW(UnknownObjectException("unknown named area: '" + name +
            "' in look '" + d_lookName + "'."));

    return WidgetLookManager::getSingleton().
        getWidgetLook(d_inheritedLookName).getNamedArea(name);
}

//---------------------------------------------------------------------------//
bool WidgetLookFeel::isNamedAreaDefined(const String& name) const
{
    NamedAreaList::const_iterator area = d_namedAreas.find(name);

    if (area != d_namedAreas.end())
        return true;

    if (d_inheritedLookName.empty())
        return false;

    return WidgetLookManager::getSingleton().
        getWidgetLook(d_inheritedLookName).isNamedAreaDefined(name);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::layoutChildWidgets(const Window& owner) const
{
    WidgetComponentCollator wcc;
    appendChildWidgetComponents(wcc);

    for (WidgetComponentCollator::const_iterator wci = wcc.begin();
         wci != wcc.end();
         ++wci)
    {
        (*wci)->layout(owner);
    }
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::addPropertyDefinition(PropertyDefinitionBase* propdef)
{
    d_propertyDefinitions.push_back(propdef);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::clearPropertyDefinitions()
{
    d_propertyDefinitions.clear();
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::addPropertyLinkDefinition(PropertyDefinitionBase* propdef)
{
    d_propertyLinkDefinitions.push_back(propdef);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::clearPropertyLinkDefinitions()
{
    d_propertyLinkDefinitions.clear();
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet WidgetLookFeel::getStateNames(bool inherits) const
{
    return getStateImageryNames(inherits);
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet WidgetLookFeel::getStateImageryNames(bool inherits) const
{
    StringSet result;
    for(StateList::const_iterator i = d_stateImagery.begin();
        i != d_stateImagery.end();
        ++i)
    {
        result.insert(i->first);
    }
    if (!d_inheritedLookName.empty() && inherits)
    {
        StringSet temp = WidgetLookManager::getSingleton().
                getWidgetLook(d_inheritedLookName).getStateNames(true);
        result.insert(temp.begin(),temp.end());
    }
    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StateIterator
WidgetLookFeel::getStateIterator(bool inherits) const
{
    if(inherits)
    {
        StringSet names = getStateNames(true);
        StateList result;
        for(StringSet::iterator i = names.begin();i != names.end();++i)
        {
            result.insert(std::make_pair(*i, getStateImagery(*i)));
        }
        return StateIterator(result.begin(),result.end());
    }else{
        return StateIterator(d_stateImagery.begin(),d_stateImagery.end());
    }
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet WidgetLookFeel::getImageryNames(bool inherits) const
{
    return getImagerySectionNames(inherits);
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet WidgetLookFeel::getImagerySectionNames(bool inherits) const
{
    StringSet result;

    for(ImageryList::const_iterator i = d_imagerySections.begin();
        i != d_imagerySections.end();
        ++i)
    {
        result.insert(i->first);
    }
    if (!d_inheritedLookName.empty() && inherits)
    {
        StringSet temp = WidgetLookManager::getSingleton().
                getWidgetLook(d_inheritedLookName).getImageryNames(true);
        result.insert(temp.begin(),temp.end());
    }
    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::ImageryIterator
WidgetLookFeel::getImageryIterator(bool inherits) const
{
    if(inherits)
    {
        StringSet names = getImageryNames(true);
        ImageryList result;
        for(StringSet::iterator i = names.begin();i != names.end();++i)
        {
            result.insert(std::make_pair(*i, getImagerySection(*i)));
        }
        return ImageryIterator(result.begin(),result.end());
    }else{
        return ImageryIterator(d_imagerySections.begin(),d_imagerySections.end());
    }
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet
WidgetLookFeel::getNamedAreaNames(bool inherits) const
{
    StringSet result;
    for(NamedAreaList::const_iterator i = d_namedAreas.begin();
        i != d_namedAreas.end();
        ++i)
    {
        result.insert(i->first);
    }
    if (!d_inheritedLookName.empty() && inherits)
    {
        StringSet temp = WidgetLookManager::getSingleton().
                getWidgetLook(d_inheritedLookName).getNamedAreaNames(true);
        result.insert(temp.begin(),temp.end());
    }
    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::NamedAreaIterator
WidgetLookFeel::getNamedAreaIterator(bool inherits) const
{
    if(inherits)
    {
        StringSet names = getNamedAreaNames(true);
        NamedAreaList result;
        for(StringSet::iterator i = names.begin();i != names.end();++i)
        {
            result.insert(std::make_pair(*i, getNamedArea(*i)));
        }
        return NamedAreaIterator(result.begin(),result.end());
    }else{
        return NamedAreaIterator(d_namedAreas.begin(),d_namedAreas.end());
    }
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet WidgetLookFeel::getWidgetNames(bool inherits) const
{
    return getWidgetComponentNames(inherits);
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet WidgetLookFeel::getWidgetComponentNames(bool inherits) const
{
    StringSet result;
    for(WidgetList::const_iterator i = d_childWidgets.begin();
        i != d_childWidgets.end();
        ++i)
    {
        result.insert(i->getWidgetName());
    }
    if (!d_inheritedLookName.empty() && inherits)
    {
        StringSet temp = WidgetLookManager::getSingleton().
                getWidgetLook(d_inheritedLookName).getWidgetNames(true);
        result.insert(temp.begin(),temp.end());
    }
    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::WidgetComponentIterator
WidgetLookFeel::getWidgetComponentIterator(bool inherits) const
{
    WidgetComponentCollator wcc;
    appendChildWidgetComponents(wcc, inherits);

    return WidgetComponentIterator(wcc.begin(), wcc.end());
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet
WidgetLookFeel::getPropertyDefinitionNames(bool inherits) const
{
    StringSet result;
    for(PropertyDefinitionList::const_iterator i = d_propertyDefinitions.begin();
        i != d_propertyDefinitions.end();
        ++i)
    {
        result.insert((*i)->getPropertyName());
    }
    if (!d_inheritedLookName.empty() && inherits)
    {
        StringSet temp = WidgetLookManager::getSingleton().
                getWidgetLook(d_inheritedLookName).getPropertyDefinitionNames(true);
        result.insert(temp.begin(),temp.end());
    }
    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::PropertyDefinitionIterator
WidgetLookFeel::getPropertyDefinitionIterator(bool inherits) const
{
    PropertyDefinitionCollator pdc;
    appendPropertyDefinitions(pdc, inherits);

    return PropertyDefinitionIterator(pdc.begin(), pdc.end());
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet
WidgetLookFeel::getPropertyLinkDefinitionNames(bool inherits) const
{
    StringSet result;
    for(PropertyLinkDefinitionList::const_iterator i = d_propertyLinkDefinitions.begin();
        i != d_propertyLinkDefinitions.end();
        ++i)
    {
        result.insert((*i)->getPropertyName());
    }
    if (!d_inheritedLookName.empty() && inherits)
    {
        StringSet temp = WidgetLookManager::getSingleton().
                getWidgetLook(d_inheritedLookName).getPropertyLinkDefinitionNames(true);
        result.insert(temp.begin(),temp.end());
    }
    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::PropertyLinkDefinitionIterator
WidgetLookFeel::getPropertyLinkDefinitionIterator(bool inherits) const
{
    PropertyLinkDefinitionCollator pldc;
    appendPropertyLinkDefinitions(pldc, inherits);

    return PropertyLinkDefinitionIterator(pldc.begin(), pldc.end());
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet
WidgetLookFeel::getPropertyInitialiserNames(bool inherits) const
{
    StringSet result;
    for(PropertyList::const_iterator i = d_properties.begin();
        i != d_properties.end();
        ++i)
    {
        result.insert(i->getTargetPropertyName());
    }
    if (!d_inheritedLookName.empty() && inherits)
    {
        StringSet temp = WidgetLookManager::getSingleton().
                getWidgetLook(d_inheritedLookName).getPropertyInitialiserNames(true);
        result.insert(temp.begin(),temp.end());
    }
    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::PropertyInitialiserIterator
WidgetLookFeel::getPropertyInitialiserIterator(bool inherits) const
{
    PropertyInitialiserCollator pic;
    appendPropertyInitialisers(pic, inherits);

    return PropertyInitialiserIterator(pic.begin(), pic.end());
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet
WidgetLookFeel::getEventLinkDefinitionNames(bool inherits) const
{
    StringSet result;
    for(EventLinkDefinitionList::const_iterator i = d_eventLinkDefinitions.begin();
        i != d_eventLinkDefinitions.end();
        ++i)
    {
        result.insert(i->getName());
    }
    if (!d_inheritedLookName.empty() && inherits)
    {
        StringSet temp = WidgetLookManager::getSingleton().
                getWidgetLook(d_inheritedLookName).getEventLinkDefinitionNames(true);
        result.insert(temp.begin(),temp.end());
    }
    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::EventLinkDefinitionIterator
WidgetLookFeel::getEventLinkDefinitionIterator(bool inherits) const
{
    EventLinkDefinitionCollator eldc;
    appendEventLinkDefinitions(eldc, inherits);

    return EventLinkDefinitionIterator(eldc.begin(), eldc.end());
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StringSet
WidgetLookFeel::getAnimationNames(bool inherits) const
{
    AnimationNameSet ans;
    appendAnimationNames(ans,inherits);
    return ans;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::AnimationNameIterator
WidgetLookFeel::getAnimationNameIterator(bool inherits) const
{
    AnimationNameSet wcl;
    appendAnimationNames(wcl,inherits);

    return AnimationNameIterator(wcl.begin(),wcl.end());
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::writeXMLToStream(XMLSerializer& xml_stream) const
{
    xml_stream.openTag(Falagard_xmlHandler::WidgetLookElement);

    xml_stream.attribute(Falagard_xmlHandler::NameAttribute, d_lookName);

    if(!d_inheritedLookName.empty())
        xml_stream.attribute(Falagard_xmlHandler::InheritsAttribute, d_inheritedLookName);

    // These sub-scopes of the loops avoid the "'curr'-already-initialized"
    // compile error on VC6++
    {
        // output property definitions
        for (PropertyDefinitionList::const_iterator curr = d_propertyDefinitions.begin();
             curr != d_propertyDefinitions.end();
             ++curr)
        {
            (*curr)->writeDefinitionXMLToStream(xml_stream);
        }
    }

    {
        // output property link definitions
        for (PropertyLinkDefinitionList::const_iterator curr = d_propertyLinkDefinitions.begin();
             curr != d_propertyLinkDefinitions.end();
             ++curr)
        {
            (*curr)->writeDefinitionXMLToStream(xml_stream);
        }
    }

    {
        // output property initialisers.
        for (PropertyList::const_iterator curr = d_properties.begin();
             curr != d_properties.end();
             ++curr)
        {
            (*curr).writeXMLToStream(xml_stream);
        }
    }

    {
        // output named areas
        for (NamedAreaList::const_iterator curr = d_namedAreas.begin();
             curr != d_namedAreas.end();
             ++curr)
        {
            (*curr).second.writeXMLToStream(xml_stream);
        }
    }

    {
        // output child widgets
        for (WidgetList::const_iterator curr = d_childWidgets.begin();
             curr != d_childWidgets.end();
             ++curr)
        {
            (*curr).writeXMLToStream(xml_stream);
        }
    }

    {
        // output imagery sections
        for (ImageryList::const_iterator curr = d_imagerySections.begin();
             curr != d_imagerySections.end();
             ++curr)
        {
            (*curr).second.writeXMLToStream(xml_stream);
        }
    }

    {
        // output states
        for (StateList::const_iterator curr = d_stateImagery.begin();
             curr != d_stateImagery.end();
             ++curr)
        {
            (*curr).second.writeXMLToStream(xml_stream);
        }
    }

    xml_stream.closeTag();
}

//---------------------------------------------------------------------------//
const PropertyInitialiser* WidgetLookFeel::findPropertyInitialiser(const String& propertyName) const
{
    PropertyInitialiserCollator pic;
    appendPropertyInitialisers(pic);

    PropertyInitialiserCollator::const_iterator i = pic.find(propertyName);

    if (i == pic.end())
        return 0;

    return *i;
}

//---------------------------------------------------------------------------//
const WidgetComponent* WidgetLookFeel::findWidgetComponent(const String& name) const
{
    WidgetComponentCollator wcc;
    appendChildWidgetComponents(wcc);

    WidgetComponentCollator::const_iterator wci = wcc.find(name);

    if (wci == wcc.end())
        return 0;

    return *wci;
}

//---------------------------------------------------------------------------//
WidgetComponent* WidgetLookFeel::retrieveWidgetComponentFromList(const String& name, bool includeInheritedElements)
{
    WidgetList::iterator iter = d_childWidgets.begin();
    WidgetList::iterator iterEnd = d_childWidgets.end();

    while(iter != iterEnd)
    {
        if(iter->getWidgetName().compare(name) == 0)
            return &(*iter);

        ++iter;
    }
    
    if(includeInheritedElements)
    {
        WidgetLookFeel* inheritedWidgetLook = getInheritedWidgetLookFeel();
        if(inheritedWidgetLook != 0)
            return inheritedWidgetLook->retrieveWidgetComponentFromList(name, true);
    }

    return 0;
}

//---------------------------------------------------------------------------//
PropertyInitialiser* WidgetLookFeel::retrievePropertyInitialiserFromList(const String& name, bool includeInheritedElements)
{
    PropertyList::iterator iter = d_properties.begin();
    PropertyList::iterator iterEnd = d_properties.end();

    while(iter != iterEnd)
    {
        PropertyInitialiser& propInit = *iter;
        if(propInit.getTargetPropertyName().compare(name) == 0)
            return &propInit;

        ++iter;
    }

    if(includeInheritedElements)
    {
        WidgetLookFeel* inheritedWidgetLook = getInheritedWidgetLookFeel();
        if(inheritedWidgetLook != 0)
            return inheritedWidgetLook->retrievePropertyInitialiserFromList(name, true);
    }

    return 0;
}

//---------------------------------------------------------------------------//
PropertyDefinitionBase* WidgetLookFeel::retrievePropertyDefinitionFromList(const String& name, bool includeInheritedElements)
{
    PropertyDefinitionList::iterator iter = d_propertyDefinitions.begin();
    PropertyDefinitionList::iterator iterEnd = d_propertyDefinitions.end();

    while(iter != iterEnd)
    {
        PropertyDefinitionBase* propDefBase = *iter;
        if(propDefBase->getPropertyName().compare(name) == 0)
            return propDefBase;

        ++iter;
    }


    if(includeInheritedElements)
    {
        WidgetLookFeel* inheritedWidgetLook = getInheritedWidgetLookFeel();
        if(inheritedWidgetLook != 0)
            return inheritedWidgetLook->retrievePropertyDefinitionFromList(name, true);
    }

    return 0;
}


//---------------------------------------------------------------------------//
PropertyDefinitionBase* WidgetLookFeel::retrievePropertyLinkDefinitionFromList(const String& name, bool includeInheritedElements)
{
    PropertyLinkDefinitionList::iterator iter = d_propertyLinkDefinitions.begin();
    PropertyLinkDefinitionList::iterator iterEnd = d_propertyLinkDefinitions.end();

    while(iter != iterEnd)
    {
        PropertyDefinitionBase* propDefBase = *iter;
        if(propDefBase->getPropertyName().compare(name) == 0)
            return propDefBase;

        ++iter;
    }

    if(includeInheritedElements)
    {
        WidgetLookFeel* inheritedWidgetLook = getInheritedWidgetLookFeel();
        if(inheritedWidgetLook != 0)
            return inheritedWidgetLook->retrievePropertyLinkDefinitionFromList(name, true);
    }

    return 0;
}

//---------------------------------------------------------------------------//
EventLinkDefinition* WidgetLookFeel::retrieveEventLinkDefinitionFromList(const String& name, bool includeInheritedElements)
{
    EventLinkDefinitionList::iterator iter = d_eventLinkDefinitions.begin();
    EventLinkDefinitionList::iterator iterEnd = d_eventLinkDefinitions.end();

    while(iter != iterEnd)
    {
        EventLinkDefinition& eventLinkDef = *iter;
        if(eventLinkDef.getName().compare(name) == 0)
            return &eventLinkDef;

        ++iter;
    }

    if(includeInheritedElements)
    {
        WidgetLookFeel* inheritedWidgetLook = getInheritedWidgetLookFeel();
        if(inheritedWidgetLook != 0)
            return inheritedWidgetLook->retrieveEventLinkDefinitionFromList(name, true);
    }

    return 0;
}


//---------------------------------------------------------------------------//
void WidgetLookFeel::addAnimationName(const String& anim_name)
{
    AnimationList::iterator it = std::find(d_animations.begin(),
                                           d_animations.end(),
                                           anim_name);

    if (it == d_animations.end())
        d_animations.push_back(anim_name);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::addEventLinkDefinition(const EventLinkDefinition& evtdef)
{
    d_eventLinkDefinitions.push_back(evtdef);
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::clearEventLinkDefinitions()
{
    d_eventLinkDefinitions.clear();
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::appendChildWidgetComponents(WidgetComponentCollator& col,
                                                 bool inherits) const
{
    if (!d_inheritedLookName.empty() && inherits)
        WidgetLookManager::getSingleton().
            getWidgetLook(d_inheritedLookName).appendChildWidgetComponents(col);

    for (WidgetList::const_iterator i = d_childWidgets.begin();
         i != d_childWidgets.end();
         ++i)
    {
        col.set(i->getWidgetName(), &*i);
    }

}

//---------------------------------------------------------------------------//
void WidgetLookFeel::appendPropertyDefinitions(PropertyDefinitionCollator& col,
                                               bool inherits) const
{
    if (!d_inheritedLookName.empty() && inherits)
        WidgetLookManager::getSingleton().
            getWidgetLook(d_inheritedLookName).appendPropertyDefinitions(col);

    for (PropertyDefinitionList::iterator i = d_propertyDefinitions.begin();
         i != d_propertyDefinitions.end();
         ++i)
    {
        col.set((*i)->getPropertyName(), *i);
    }

}

//---------------------------------------------------------------------------//
void WidgetLookFeel::appendPropertyLinkDefinitions(
        PropertyLinkDefinitionCollator& col, bool inherits) const
{
    if (!d_inheritedLookName.empty() && inherits)
        WidgetLookManager::getSingleton().
            getWidgetLook(d_inheritedLookName).appendPropertyLinkDefinitions(col);

    for (PropertyLinkDefinitionList::iterator i = d_propertyLinkDefinitions.begin();
         i != d_propertyLinkDefinitions.end();
         ++i)
    {
        col.set((*i)->getPropertyName(), *i);
    }
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::appendPropertyInitialisers(
        PropertyInitialiserCollator& col, bool inherits) const
{
    if (!d_inheritedLookName.empty() && inherits)
        WidgetLookManager::getSingleton().
            getWidgetLook(d_inheritedLookName).appendPropertyInitialisers(col);

    for (PropertyList::const_iterator i = d_properties.begin();
         i != d_properties.end();
         ++i)
    {
        col.set(i->getTargetPropertyName(), &*i);
    }
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::appendEventLinkDefinitions(
        EventLinkDefinitionCollator& col, bool inherits) const
{
    if (!d_inheritedLookName.empty() && inherits)
        WidgetLookManager::getSingleton().
            getWidgetLook(d_inheritedLookName).appendEventLinkDefinitions(col);

    for (EventLinkDefinitionList::const_iterator i = d_eventLinkDefinitions.begin();
         i != d_eventLinkDefinitions.end();
         ++i)
    {
        col.set(i->getName(), &*i);
    }
}

//---------------------------------------------------------------------------//
void WidgetLookFeel::appendAnimationNames(AnimationNameSet& set,
                                          bool inherits) const
{
    set.insert(d_animations.begin(),d_animations.end());

    if (!d_inheritedLookName.empty() && inherits)
        WidgetLookManager::getSingleton().
            getWidgetLook(d_inheritedLookName).appendAnimationNames(set);
}

//---------------------------------------------------------------------------//
bool WidgetLookFeel::handleFontRenderSizeChange(Window& window,
                                                const Font* font) const
{
    bool result = false;

    for(ImageryList::const_iterator i = d_imagerySections.begin();
        i != d_imagerySections.end();
        ++i)
    {
        result |= i->second.handleFontRenderSizeChange(window, font);
    }

    for(WidgetList::const_iterator i = d_childWidgets.begin();
        i != d_childWidgets.end();
        ++i)
    {
        result |= i->handleFontRenderSizeChange(window, font);
    }

    if (!d_inheritedLookName.empty())
        result |= WidgetLookManager::getSingleton().
            getWidgetLook(d_inheritedLookName).
                handleFontRenderSizeChange(window, font);

    return result;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::StateImageryPointerMap WidgetLookFeel::getStateImageryMap(bool includeInheritedElements)
{
    StateImageryPointerMap pointerMap;
    StringSet nameSet = getStateNames(includeInheritedElements);

    StringSet::iterator iter = nameSet.begin();
    StringSet::iterator iterEnd = nameSet.end();
    while(iter != iterEnd)
    {
        const CEGUI::String& currentElementName = *iter;
        StateList::iterator stateImageryIter = d_stateImagery.find(currentElementName);

        if(stateImageryIter == d_stateImagery.end())
        {
            CEGUI_THROW(UnknownObjectException("Error: StateImagery with name: \"" + currentElementName
                + "\" exists in the list of names but a StateImagery with this name"
                + "could not be found in the map"));
        }
        else
        {        
            StateImagery* stateImagery = &stateImageryIter->second;
            pointerMap.insert(std::make_pair(currentElementName, stateImagery));
        }

        ++iter;
    }

    return pointerMap;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::ImagerySectionPointerMap WidgetLookFeel::getImagerySectionMap(bool includeInheritedElements)
{
    ImagerySectionPointerMap pointerMap;
    StringSet nameSet = getImageryNames(includeInheritedElements);
    
    StringSet::iterator iter = nameSet.begin();
    StringSet::iterator iterEnd = nameSet.end();
    while(iter != iterEnd)
    {
        const CEGUI::String& currentElementName = *iter;
        ImageryList::iterator imagerySectionIter = d_imagerySections.find(currentElementName);

        if(imagerySectionIter == d_imagerySections.end())
        {
            CEGUI_THROW(UnknownObjectException("Error: ImagerySection with name: \"" + currentElementName
                                               + "\" exists in the list of names but a ImagerySection with this name"
                                               + "could not be found in the map"));
        }
        else
        {        
            ImagerySection* imagerySection = &imagerySectionIter->second;
            pointerMap.insert(std::make_pair(currentElementName, imagerySection));
        }

        ++iter;
    }

    return pointerMap;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::NamedAreaPointerMap WidgetLookFeel::getNamedAreaMap(bool includeInheritedElements)
{
    NamedAreaPointerMap pointerMap;
    StringSet nameSet = getNamedAreaNames(includeInheritedElements);

    StringSet::iterator iter = nameSet.begin();
    StringSet::iterator iterEnd = nameSet.end();
    while(iter != iterEnd)
    {
        const CEGUI::String& currentElementName = *iter;
        NamedAreaList::iterator namedAreaIter = d_namedAreas.find(currentElementName);

        if(namedAreaIter == d_namedAreas.end())
        {
            CEGUI_THROW(UnknownObjectException("Error: NamedArea with name: \"" + currentElementName
                + "\" exists in the list of names but a NamedArea with this name"
                + "could not be found in the map"));
        }
        else
        {        
            NamedArea* namedArea = &namedAreaIter->second;
            pointerMap.insert(std::make_pair(currentElementName, namedArea));
        }

        ++iter;
    }

    return pointerMap;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::WidgetComponentPointerMap WidgetLookFeel::getWidgetComponentMap(bool includeInheritedElements)
{
    WidgetComponentPointerMap pointerMap;
    StringSet nameSet = getWidgetNames(includeInheritedElements);

    StringSet::iterator iter = nameSet.begin();
    StringSet::iterator iterEnd = nameSet.end();
    while(iter != iterEnd)
    {
        const CEGUI::String& currentElementName = *iter;
        // This is deprecated in the next version, instead we will directly access the WidgetComponent map, which will be added.
        WidgetComponent* widgetComponent = retrieveWidgetComponentFromList(currentElementName, includeInheritedElements);

        if(widgetComponent == 0)
        {
            CEGUI_THROW(UnknownObjectException("Error: WidgetComponent with name: \"" + currentElementName
                + "\" exists in the list of names but a WidgetComponent with this name"
                + "could not be found in the map"));
        }
        else
        {        
            pointerMap.insert(std::make_pair(currentElementName, widgetComponent));
        }

        ++iter;
    }

    return pointerMap;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::PropertyInitialiserPointerMap WidgetLookFeel::getPropertyInitialiserMap(bool includeInheritedElements)
{
    PropertyInitialiserPointerMap pointerMap;
    StringSet nameSet = getPropertyInitialiserNames(includeInheritedElements);

    StringSet::iterator iter = nameSet.begin();
    StringSet::iterator iterEnd = nameSet.end();
    while(iter != iterEnd)
    {
        const CEGUI::String& currentElementName = *iter;
        // This is deprecated in the next version, instead we will directly access the PropertyInitialiser map, which will be added.
        PropertyInitialiser* propertyInitialiser = retrievePropertyInitialiserFromList(currentElementName, includeInheritedElements);

        if(propertyInitialiser == 0)
        {
            CEGUI_THROW(UnknownObjectException("Error: PropertyInitialiser with name: \"" + currentElementName
                + "\" exists in the list of names but a PropertyInitialiser with this name"
                + "could not be found in the map"));
        }
        else
        {        
            pointerMap.insert(std::make_pair(currentElementName, propertyInitialiser));
        }

        ++iter;
    }

    return pointerMap;
}


//---------------------------------------------------------------------------//
WidgetLookFeel::PropertyDefinitionBasePointerMap WidgetLookFeel::getPropertyDefinitionMap(bool includeInheritedElements)
{
    PropertyDefinitionBasePointerMap pointerMap;
    StringSet nameSet = getPropertyDefinitionNames(includeInheritedElements);

    StringSet::iterator iter = nameSet.begin();
    StringSet::iterator iterEnd = nameSet.end();
    while(iter != iterEnd)
    {
        const CEGUI::String& currentElementName = *iter;
        // This is deprecated in the next version, instead we will directly access the PropertyDefinition map, which will be added.
        PropertyDefinitionBase* propertyDefinition = retrievePropertyDefinitionFromList(currentElementName, includeInheritedElements);

        if(propertyDefinition == 0)
        {
            CEGUI_THROW(UnknownObjectException("Error: PropertyDefinition with name: \"" + currentElementName
                + "\" exists in the list of names but a PropertyDefinition with this name"
                + "could not be found in the map"));
        }
        else
        {        
            pointerMap.insert(std::make_pair(currentElementName, propertyDefinition));
        }

        ++iter;
    }

    return pointerMap;
}

//---------------------------------------------------------------------------//
WidgetLookFeel::PropertyDefinitionBasePointerMap WidgetLookFeel::getPropertyLinkDefinitionMap(bool includeInheritedElements)
{
    PropertyDefinitionBasePointerMap pointerMap;
    StringSet nameSet = getPropertyLinkDefinitionNames(includeInheritedElements);

    StringSet::iterator iter = nameSet.begin();
    StringSet::iterator iterEnd = nameSet.end();
    while(iter != iterEnd)
    {
        const CEGUI::String& currentElementName = *iter;
        // This is deprecated in the next version, instead we will directly access the PropertyLinkDefinition map, which will be added.
        PropertyDefinitionBase* propertyLinkDefinition = retrievePropertyLinkDefinitionFromList(currentElementName, includeInheritedElements);

        if(propertyLinkDefinition == 0)
        {
            CEGUI_THROW(UnknownObjectException("Error: PropertyLinkDefinition with name: \"" + currentElementName
                + "\" exists in the list of names but a PropertyLinkDefinition with this name"
                + "could not be found in the map"));
        }
        else
        {        
            pointerMap.insert(std::make_pair(currentElementName, propertyLinkDefinition));
        }

        ++iter;
    }

    return pointerMap;
}

 
//---------------------------------------------------------------------------//
WidgetLookFeel::EventLinkDefinitionPointerMap WidgetLookFeel::getEventLinkDefinitionMap(bool includeInheritedElements)
{
    EventLinkDefinitionPointerMap pointerMap;
    StringSet nameSet = getEventLinkDefinitionNames(includeInheritedElements);

    StringSet::iterator iter = nameSet.begin();
    StringSet::iterator iterEnd = nameSet.end();
    while(iter != iterEnd)
    {
        const CEGUI::String& currentElementName = *iter;
        // This is deprecated in the next version, instead we will directly access the EventLinkDefinition map, which will be added.
        EventLinkDefinition* eventLinkDef = retrieveEventLinkDefinitionFromList(currentElementName, includeInheritedElements);

        if(eventLinkDef == 0)
        {
            CEGUI_THROW(UnknownObjectException("Error: EventLinkDefinition with name: \"" + currentElementName
                + "\" exists in the list of names but a EventLinkDefinition with this name"
                + "could not be found in the map"));
        }
        else
        {        
            pointerMap.insert(std::make_pair(currentElementName, eventLinkDef));
        }

        ++iter;
    }

    return pointerMap;
}


//---------------------------------------------------------------------------//
const CEGUI::String& WidgetLookFeel::getInheritedWidgetLookName() const
{
    return d_inheritedLookName;
}

//---------------------------------------------------------------------------//
WidgetLookFeel* WidgetLookFeel::getInheritedWidgetLookFeel()
{
    if(d_inheritedLookName.empty())
        return 0;

    WidgetLookManager::WidgetLookPointerMap pointerMap = WidgetLookManager::getSingleton().getWidgetLookPointerMap();
    WidgetLookManager::WidgetLookPointerMap::iterator foundIter = pointerMap.find(d_inheritedLookName);

    if(foundIter == pointerMap.end())
        CEGUI_THROW(UnknownObjectException("Error: Inherited WidgetLook with name: \"" + d_inheritedLookName
                                           + "\" cannot be found in the WidgetLookManager's map"));

    return foundIter->second;
}

//---------------------------------------------------------------------------//

} // End of  CEGUI namespace section

