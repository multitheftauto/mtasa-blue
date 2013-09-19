/************************************************************************
	filename: 	CEGUIEventSet.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements the EventSet class
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
#include "CEGUIEventSet.h"
#include "CEGUIExceptions.h"
#include "CEGUIGlobalEventSet.h"
#include "CEGUIScriptModule.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constructor
*************************************************************************/
EventSet::EventSet() :
	d_muted(false),
	d_addedUncommonEvents(false)
{
}


/*************************************************************************
	Destructor
*************************************************************************/
EventSet::~EventSet(void)
{
	removeAllEvents();
}


/*************************************************************************
	Add a new event to the EventSet
*************************************************************************/
void EventSet::addEvent(const String& name)
{
	if (isEventPresent(name))
	{
		throw AlreadyExistsException("An event named '" + name + "' already exists in the EventSet.");
	}

	d_events[name] = new Event(name);
}


/*************************************************************************
	Remove an event from the EventSet
*************************************************************************/
void EventSet::removeEvent(const String& name)
{
	EventMap::iterator pos = d_events.find(name);

	if (pos != d_events.end())
	{
		delete pos->second;
		d_events.erase(pos);
	}

}


/*************************************************************************
	Remove all events from the EventSet
*************************************************************************/
void EventSet::removeAllEvents(void)
{
	EventMap::iterator pos = d_events.begin();
	EventMap::iterator end = d_events.end()	;

	for (; pos != end; ++pos)
	{
		delete pos->second;
	}

	d_events.clear();
}


/*************************************************************************
	Check to see if an event is available
*************************************************************************/
bool EventSet::isEventPresent(const String& name)
{
	return (d_events.find(name) != d_events.end());
}


/*************************************************************************
	Subscribe to a scripted event (no group)
*************************************************************************/
Event::Connection EventSet::subscribeScriptedEvent(const String& name, const String& subscriber_name)
{
	return subscribeEvent(name, Event::Subscriber(ScriptFunctor(subscriber_name)));
}


/*************************************************************************
	Subscribe to a scripted event
*************************************************************************/
Event::Connection EventSet::subscribeScriptedEvent(const String& name, Event::Group group, const String& subscriber_name)
{
	return subscribeEvent(name, group, Event::Subscriber(ScriptFunctor(subscriber_name)));
}


/*************************************************************************
	Subscribe to an event (no group)
*************************************************************************/
Event::Connection EventSet::subscribeEvent(const String& name, Event::Subscriber subscriber)
{
	EventMap::iterator pos = d_events.find(name);

	if (pos == d_events.end())
	{
        // See if we should initialize all events for this instance
        maybeAddUncommonEvents ( name );
        // Re-check for match
        pos = d_events.find ( name ) ;
	    if ( pos == d_events.end () )
    		throw UnknownObjectException("No event named '" + name + "' is defined for this EventSet");	
	}

	return pos->second->subscribe(subscriber);
}


/*************************************************************************
	Subscribe to an event group
*************************************************************************/
Event::Connection EventSet::subscribeEvent(const String& name, Event::Group group, Event::Subscriber subscriber)
{
	EventMap::iterator pos = d_events.find(name);

	if (pos == d_events.end())
	{
        // See if we should initialize all events for this instance
        maybeAddUncommonEvents ( name );
        // Re-check for match
        pos = d_events.find ( name ) ;
	    if ( pos == d_events.end () )
		    throw UnknownObjectException("No event named '" + name + "' is defined for this EventSet");	
	}

	return pos->second->subscribe(group, subscriber);
}

/*************************************************************************
	Fire / Trigger an event
*************************************************************************/
void EventSet::fireEvent(const String& name, EventArgs& args, const String& eventNamespace)
{
    // handle global events
    GlobalEventSet::getSingleton().fireEvent(name, args, eventNamespace);

    EventMap::iterator pos = d_events.find(name);

	if (pos == d_events.end())
	{
        return;
		//throw UnknownObjectException("No event named '" + name + "' is defined for this EventSet");	
	}

	// fire the event
	if (!d_muted)
	{
		(*pos->second)(args);
	}

}


/*************************************************************************
	Return whether the EventSet is muted or not.	
*************************************************************************/
bool EventSet::isMuted(void) const
{
	return d_muted;
}


/*************************************************************************
	Set the mute state for this EventSet.	
*************************************************************************/
void EventSet::setMutedState(bool setting)
{
	d_muted = setting;
}


/*************************************************************************
	Return a EventSet::EventIterator object to iterate over the available
	events.
*************************************************************************/
EventSet::EventIterator EventSet::getIterator(void) const
{
    maybeAddUncommonEvents ( "getIterator" );
	return EventIterator(d_events.begin(), d_events.end());
}


/*************************************************************************
	Return a dummy type name - Should never be called
*************************************************************************/
const String& EventSet::getType(void) const
{
    static String dummy;
    return dummy;
}


/*************************************************************************
	maybeAddUncommonEvents	
*************************************************************************/
void EventSet::maybeAddUncommonEvents ( const String& name ) const
{
    // Const cast hack
    const_cast < EventSet* > ( this )->maybeAddUncommonEvents ( name ) ;
}


/*************************************************************************
	maybeAddUncommonEvents	
*************************************************************************/
void EventSet::maybeAddUncommonEvents ( const String& name )
{
    if ( !d_addedUncommonEvents )
    {
        Logger::getSingleton ().logEvent ( SString ( "** Adding uncommon events for a %s, to find missing event %s", getType ().c_str (), name.c_str() ), Informative);
        d_addedUncommonEvents = true;
        addUncommonEvents ();

        // See if event there now
        if ( d_events.find ( name ) != d_events.end () )
        {
            return;
        }
    }

    Logger::getSingleton ().logEvent ( SString ( "** Trouble with %s, can't find event %s", getType ().c_str (), name.c_str() ), Informative);
}


} // End of  CEGUI namespace section
