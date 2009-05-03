/************************************************************************
	filename: 	CEGUIEventSet.h
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Defines class for a named collection of Event objects
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
#ifndef _CEGUIEventSet_h_
#define _CEGUIEventSet_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIEvent.h"
#include "CEGUIIteratorBase.h"
#include <map>

#if defined (_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Class that collects together a set of Event objects.

	The EventSet used to collect Event objects together, and allow access
	to them by a unique name
*/
class CEGUIEXPORT EventSet
{
public:
	/*!
	\brief
		Constructor for EventSet objects
	*/
	EventSet();


	/*!
	\brief
		Destructor for EventSet objects
	*/
	virtual ~EventSet(void);


	/*!
	\brief
		Add a new Event to the EventSet with the given name.

	\param name
		String object containing the name to give the new Event.  The name must be unique for the EventSet.

	\return
		Nothing

	\exception AlreadyExistsException	Thrown if an Event already exists named \a name.
	*/
	void	addEvent(const String& name);


	/*!
	\brief
		Removes the Event with the given name.  All connections to the event are disconnected.

	\param name
		String object containing the name of the Event to remove.  If no such Event exists, nothing happens.

	\return
		Nothing.
	*/
	void	removeEvent(const String& name);


	/*!
	\brief
		Remove all Event objects from the EventSet

	\return
		Nothing
	*/
	void	removeAllEvents(void);


	/*!
	\brief
		Checks to see if an Event with the given name is present in the EventSet.

	\return
		true if an Event named \a name was found, or false if the Event was not found
	*/
	bool	isEventPresent(const String& name);


	/*!
	\brief
		Subscribes the named Event.

	\param name
		String object containing the name of the Event to subscribe to.

	\param subscriber
		Function or object that is to be subscribed to the Event.

	\return
		Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.

	\exception UnknownObjectException	Thrown if an Event named \a name is not in the EventSet
	*/
	virtual Event::Connection	subscribeEvent(const String& name, Event::Subscriber subscriber);


	/*!
	\brief
		Subscribes the specified group of the named Event.

	\param name
		String object containing the name of the Event to subscribe to.

	\param group
		Group which is to be subscribed to.  Subscription groups are called in ascending order.

	\param subscriber
		Function or object that is to be subscribed to the Event.

	\return
		Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.

	\exception UnknownObjectException	Thrown if an Event named \a name is not in the EventSet
	*/
	virtual Event::Connection	subscribeEvent(const String& name, Event::Group group, Event::Subscriber subscriber);
	
	
	/*!
	\brief
		Subscribes the named Event to a scripted funtion

	\param name
		String object containing the name of the Event to subscribe to.

	\param subscriber_name
		String object containing the name of the script funtion that is to be subscribed to the Event.

	\return
		Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.

	\exception UnknownObjectException	Thrown if an Event named \a name is not in the EventSet
	*/
	virtual Event::Connection	subscribeScriptedEvent(const String& name, const String& subscriber_name);
	
	
	/*!
	\brief
		Subscribes the specified group of the named Event to a scripted funtion.

	\param name
		String object containing the name of the Event to subscribe to.

	\param group
		Group which is to be subscribed to.  Subscription groups are called in ascending order.

	\param subscriber_name
		String object containing the name of the script funtion that is to be subscribed to the Event.

	\return
		Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.

	\exception UnknownObjectException	Thrown if an Event named \a name is not in the EventSet
	*/
	virtual Event::Connection	subscribeScriptedEvent(const String& name, Event::Group group, const String& subscriber_name);


	/*!
	\brief
		Fires the named event passing the given EventArgs object.

	\param name
		String object holding the name of the Event that is to be fired (triggered)

	\param args
		The EventArgs (or derived) object that is to be bassed to each subscriber of the Event.  Once all subscribers
		have been called the 'handled' field of the event is updated appropriately.
		
	\param eventNamespace
		String object describing the global event namespace prefix for this event.

	\return
		Nothing.

	\exception UnknownObjectException	Thrown if no Event named \a name was found in the EventSet.
	*/
	virtual void	fireEvent(const String& name, EventArgs& args, const String& eventNamespace = "");


	/*!
	\brief
		Return whether the EventSet is muted or not.

	\return
		- true if the EventSet is muted.  All requests to fire events will be ignored.
		- false if the EventSet is not muted.  All requests to fire events are processed as normal.
	*/
	bool	isMuted(void) const;


	/*!
	\brief
		Set the mute state for this EventSet.

	\param setting
		- true if the EventSet is to be muted (no further event firing requests will be honoured until EventSet is unmuted).
		- false if the EventSet is not to be muted and all events should fired as requested.

	\return
		Nothing.
	*/
	void	setMutedState(bool setting);


protected:
	// Do not allow copying, assignment, or any other usage than simple creation.
	EventSet(EventSet& e) {}
	EventSet& operator=(EventSet& e) {return *this;}

	typedef std::map<String, Event*>	EventMap;
	EventMap	d_events;

	bool	d_muted;	//!< true if events for this EventSet have been muted.

public:
	/*************************************************************************
		Iterator stuff
	*************************************************************************/
	typedef	ConstBaseIterator<EventMap>	EventIterator;

	/*!
	\brief
		Return a EventSet::EventIterator object to iterate over the available events.
	*/
	EventIterator	getIterator(void) const;
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIEventSet_h_
