/************************************************************************
	filename: 	CEGUIGlobalEventSet.h
	created:	16/1/2005
	author:		Paul D Turner
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http:www.cegui.org.uk)
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
#ifndef _CEGUIGlobalEventSet_h_
#define _CEGUIGlobalEventSet_h_

#include "CEGUIEventSet.h"
#include "CEGUISingleton.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4275)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
    The GlobalEventSet singleton allows you to subscribe to an event for all
    instances of a class.  The GlobalEventSet effectively supports "late binding"
    to events; which means you can subscribe to some event that does not actually
    exist (yet).
*/
class CEGUIEXPORT GlobalEventSet : public EventSet, public Singleton<GlobalEventSet>
{
public:
	GlobalEventSet();
	~GlobalEventSet();


	/*!
	\brief
		Return singleton System object

	\return
		Singleton System object
	*/
	static	GlobalEventSet&	getSingleton(void);


	/*!
	\brief
		Return pointer to singleton System object

	\return
		Pointer to singleton System object
	*/
	static	GlobalEventSet*	getSingletonPtr(void);


	/*!
	\brief
		Subscribes the the named Event.

	\note
		If the named event does not exist it is added.

	\param name
		String object containing the name of the Event to subscribe to.

	\param subscriber
		Function or object that is to be subscribed to the Event.

	\return
		Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.
	*/
	virtual Event::Connection	subscribeEvent(const String& name, Event::Subscriber subscriber);


	/*!
	\brief
		Subscribes the the specified group of the named Event.

	/note
		If the named event does not exist it is added.

	\param name
		String object containing the name of the Event to subscribe to.

	\param group
		Group which is to be subscribed to.  Subscription groups are called in ascending order.

	\param subscriber
		Function or object that is to be subscribed to the Event.

	\return
		Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.
	*/
	virtual Event::Connection	subscribeEvent(const String& name, Event::Group group, Event::Subscriber subscriber);


	/*!
	\brief
		Fires the named event passing the given EventArgs object.

	\param name
		String object holding the name of the Event that is to be fired (triggered)

	\param args
		The EventArgs (or derived) object that is to be bassed to each subscriber of the Event.  Once all subscribers
		have been called the 'handled' field of the event is updated appropriately.
		
	\param eventNamespace
		String object describing the namespace prefix to use when firing the global event.

	\return
		Nothing.
	*/
	virtual void	fireEvent(const String& name, EventArgs& args, const String& eventNamespace = "");
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIGlobalEventSet_h_
