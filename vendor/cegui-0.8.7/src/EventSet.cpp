/***********************************************************************
    created:    21/2/2004
    author:     Paul D Turner

    purpose:    Implements the EventSet class
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
#include "CEGUI/EventSet.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/GlobalEventSet.h"
#include "CEGUI/ScriptModule.h"
#include "CEGUI/System.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
EventSet::EventSet() :
    d_muted(false)
{
}

//----------------------------------------------------------------------------//
EventSet::~EventSet(void)
{
    removeAllEvents();
}

//----------------------------------------------------------------------------//
void EventSet::addEvent(const String& name)
{
    addEvent(*CEGUI_NEW_AO Event(name));
}

//----------------------------------------------------------------------------//
void EventSet::addEvent(Event& event)
{
    const String name(event.getName());

    if (isEventPresent(name))
    {
        CEGUI_DELETE_AO &event;

        CEGUI_THROW(AlreadyExistsException(
            "An event named '" + name + "' already exists in the EventSet."));
    }

    d_events.insert(std::make_pair(name, &event));
}

//----------------------------------------------------------------------------//
void EventSet::removeEvent(const String& name)
{
    EventMap::iterator pos = d_events.find(name);

	if (pos != d_events.end())
	{
		CEGUI_DELETE_AO pos->second;
		d_events.erase(pos);
	}
}

//----------------------------------------------------------------------------//
void EventSet::removeEvent(Event& event)
{
    removeEvent(event.getName());
}

//----------------------------------------------------------------------------//
void EventSet::removeAllEvents(void)
{
	EventMap::const_iterator pos = d_events.begin();
	EventMap::const_iterator end = d_events.end()	;

	for (; pos != end; ++pos)
		CEGUI_DELETE_AO pos->second;

    d_events.clear();
}

//----------------------------------------------------------------------------//
bool EventSet::isEventPresent(const String& name)
{
    return (d_events.find(name) != d_events.end());
}

//----------------------------------------------------------------------------//
Event::Connection EventSet::subscribeScriptedEvent(const String& name,
                                                   const String& subscriber_name)
{
    return getScriptModule()->subscribeEvent(this, name, subscriber_name);
}

//----------------------------------------------------------------------------//
Event::Connection EventSet::subscribeScriptedEvent(const String& name,
                                                   Event::Group group,
                                                   const String& subscriber_name)
{
    return getScriptModule()->subscribeEvent(this, name, group, subscriber_name);
}

//----------------------------------------------------------------------------//
ScriptModule* EventSet::getScriptModule() const
{
    ScriptModule* sm = System::getSingletonPtr()->getScriptingModule();

    if (sm)
        return sm;

    CEGUI_THROW(InvalidRequestException(
        "No scripting module is available."));
}

//----------------------------------------------------------------------------//
Event::Connection EventSet::subscribeEvent(const String& name,
                                           Event::Subscriber subscriber)
{
    return getEventObject(name, true)->subscribe(subscriber);
}

//----------------------------------------------------------------------------//
Event::Connection EventSet::subscribeEvent(const String& name,
                                           Event::Group group,
                                           Event::Subscriber subscriber)
{
    return getEventObject(name, true)->subscribe(group, subscriber);
}

//----------------------------------------------------------------------------//
void EventSet::fireEvent(const String& name,
                         EventArgs& args,
                         const String& eventNamespace)
{
    if (GlobalEventSet* ges = GlobalEventSet::getSingletonPtr())
        ges->fireEvent(name, args, eventNamespace);

    fireEvent_impl(name, args);
}

//----------------------------------------------------------------------------//
bool EventSet::isMuted(void) const
{
    return d_muted;
}

//----------------------------------------------------------------------------//
void EventSet::setMutedState(bool setting)
{
    d_muted = setting;
}

//----------------------------------------------------------------------------//
Event* EventSet::getEventObject(const String& name, bool autoAdd)
{
    EventMap::const_iterator pos = d_events.find(name);

    // if event did not exist, add it as needed and then find it.
    if (pos == d_events.end())
    {
        if (!autoAdd)
            return 0;

        addEvent(name);
        pos = d_events.find(name);
    }

    return pos->second;
}

//----------------------------------------------------------------------------//
void EventSet::fireEvent_impl(const String& name, EventArgs& args)
{
    Event* ev = getEventObject(name);

    if ((ev != 0) && !d_muted)
        (*ev)(args);
}

//----------------------------------------------------------------------------//
EventSet::EventIterator EventSet::getEventIterator(void) const
{
    return EventIterator(d_events.begin(), d_events.end());
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

