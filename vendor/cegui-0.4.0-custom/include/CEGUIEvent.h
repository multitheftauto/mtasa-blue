/************************************************************************
	filename: CEGUIEvent.h
	created:  15/10/2004
	authors:  Paul D Turner (High-level design) 
            Gerald Lindsly (Coder)
	
	purpose:  Defines interface for Event class
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
#ifndef _CEGUIEvent_h_
#define _CEGUIEvent_h_

#if defined (_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4786)
#	pragma warning(disable : 4251)
#	if !defined (_MSC_EXTENSIONS)
#		pragma warning (disable : 4224)
#	endif
#endif

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIEventArgs.h"
#include "CEGUIRefPtr.h"

#include <map>


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
    The base class for the various binders.  This provides a consistent
    interface for firing functions bound to an event.
*/
template <typename Ret, typename Args>
class SubscriberInterface {
public:
  virtual Ret operator()(Args) const = 0;
  virtual ~SubscriberInterface() {}
};


/*!
\brief
    This class binds a free function.
*/
template <typename Ret, typename Args>
class _freeBinder : public SubscriberInterface<Ret,Args>
{
public:
  virtual Ret operator()(Args args) const 
  {
    return d_f(args);
  }
  typedef Ret (*SlotFunction)(Args);
  _freeBinder(SlotFunction f) : d_f(f) {}
protected:
  SlotFunction d_f;
};


/*!
\brief
    This class binds a copy of a functor.
*/
template <class Functor, typename Ret, typename Args>
class _functorBinder : public SubscriberInterface<Ret,Args>
{
public:
  virtual Ret operator()(Args args) const 
  {
    return d_f(args);
  }
  _functorBinder(const Functor& f) : d_f(f) {}
protected:
  Functor d_f;
};


/*!
\brief
    This class binds a member function along with a target object.
*/
template <class T, typename Ret, typename Args>
class _memberBinder : public SubscriberInterface<Ret,Args>
{
  typedef Ret (T::*F)(Args);
public:
  virtual Ret operator()(Args args) const
  {
    return (d_t->*d_f)(args);
  }
  _memberBinder(F f, T* t) : d_f(f), d_t(t) {}
protected:
  F  d_f;
  T* d_t;
};


/*!
\brief
    This template describes the Subscriber class.  It is a wrapper
    for a pointer to a SubscriberInterface with various constructors
    that will by implicit conversion construct the various binders.
*/
template <typename Ret, typename Args>
class SubscriberTemplate
{
public:
  Ret operator()(Args args) const
  {
    return (*d_si)(args);  // call the bound function
  }

  typedef Ret (*SlotFunction)(Args);

  //! construct from a free function
  SubscriberTemplate(SlotFunction f)
  {
    d_si = new _freeBinder<Ret,Args>(f);
  }

  //! construct from a member function and a pointer to the target object.
  template <class T>
  SubscriberTemplate(Ret (T::*f)(Args), T* target)
  {
    d_si = new _memberBinder<T,Ret,Args>(f, target);
  }

  //! construct from a generalized functor by copying it
  template <typename Functor> 
  SubscriberTemplate(const Functor& f)
  {
    d_si = new _functorBinder<Functor,Ret,Args>(f);
  }

  /*!
  \brief
    construct from a preconstructed SubscriberInterface.
    used for SubscriberRef().
  */
  SubscriberTemplate(SubscriberInterface<Ret,Args>* si) : d_si(si) {}

  //! copy constructor
  SubscriberTemplate(const SubscriberTemplate<Ret,Args>& copy) : d_si(copy.d_si) {}

  //! 'less than' comparable for insertion in a map
  bool operator<(const SubscriberTemplate<Ret,Args>& rhs) const { return d_si < rhs.d_si; }

  //! release the binding -- called upon disconnection
  void release() const
  {
    delete d_si;
  }

protected:
  SubscriberInterface<Ret,Args>* d_si;
};


/*!
\brief
    This class binds a const reference to a generalized functor.
    Sometimes it may not be appropriate for the functor to be
    cloned.  In which case, use SubscriberRef() (which uses this).
*/
template <class Functor, typename Ret, typename Args>
class _refBinder : public SubscriberInterface<Ret,Args>
{
public:
  virtual Ret operator()(Args args) const
  {
    return d_f(args);
  }
  _refBinder(const Functor& f) : d_f(f) {}
protected:
  const Functor& d_f;
};

/*!
\brief
    This helper function produces a const reference binding
*/
template <class Functor>
SubscriberInterface<bool, const EventArgs&>*
SubscriberRef(const Functor& f)
{
  return new _refBinder<Functor,bool,const EventArgs&>(f);
}


/*!
\brief
	Defines an 'event' which can be subscribed to by interested parties.

	An Event can be subscribed by a function, a member function, or a function object.  Whichever option
	is taken, the function signature needs to be as follows
	\par
	<em>bool function_name(const EventArgs& args);</em>
	\note
		An Event object may not be copied.
*/
class CEGUIEXPORT Event
{
public:
    /*!
    \brief
        Interface to be implemented by connection objects.
    */
	class ConnectionInterface : public Referenced {
	public:
		virtual bool connected() { return false; }
		virtual void disconnect() {}
	};
	typedef RefPtr<ConnectionInterface> Connection;

    /*!
    \brief
        A Connection object that automatically disconnects from the event
        when the ScropedConnection object goes out of scope (and is deleted).
    */
	class ScopedConnection {
	public:
		ScopedConnection(Connection conn_) : conn(conn_) {}
		~ScopedConnection() { conn->disconnect(); }
		Connection conn;
	};


	typedef SubscriberTemplate<bool, const EventArgs&> Subscriber;
	typedef int Group;

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructs a new Event object with the specified name
	*/
	Event(const String& name);

	/*!
	\brief
		Destructor for Event objects
	*/
	virtual ~Event(void);


	/*!
	\brief
		Return the name given to this Event object when it was created.

	\return
		String object containing the name of the Event object.
	*/
	const String& getName(void) const	{return d_name;}


	/*!
	\brief
		Subscribes some function / object to the Event

	\param subscriber
		A function, static member function, or function object, with the signature void function_name(const EventArgs& args)

	\return
		A Connection pointer which can be used to disconnect (unsubscribe) from the Event, and to check the connection state.
	*/
	Connection subscribe(Subscriber subscriber) { return subscribe(0, subscriber); }


	/*!
	\brief
		Subscribes some function / object to the Event

	\param group
		The Event group to subscribe to, subscription groups are called in ascending order, followed by subscriptions with no group.
		connections to the same group may be called in any order.

	\param subscriber
		A function, static member function, or function object, with the signature void function_name(const EventArgs& args)

	\return
		A Connection which can be used to disconnect (unsubscribe) from the Event, and to check the connection state.
	*/
	Connection subscribe(Group group, Subscriber subscriber);
  

	/*
	\brief
		Fires the event.  All event subscribers get called in the appropriate sequence.

	\param args
		An object derived from EventArgs to be passed to each event subscriber.

	\return
		Nothing.
	*/
	void	operator()(EventArgs& args);

private:
	/*************************************************************************
		Copy constructor and assignment are not allowed for events
	*************************************************************************/
	Event(const Event& evt)	{}
	Event& operator=(const Event& evt)	{return *this;}

	/*
	\brief
		removes the subscriber from the event.

	\param subscriber
		A pointer to a SubscriberInterface which is to be removed from the event.

	\return
		- true, if the subscriber was registered with the event in the specified group and it was removed.
		- false, if not.
	*/
	bool unsubscribe(Subscriber subscriber, Group group=0);

	class GroupSubscriber {
	public:
		Group group;
		Subscriber subscriber;
		GroupSubscriber(Group group_, Subscriber subscriber_) 
			: group(group_), subscriber(subscriber_) {}
	};

	struct ltGroupSubscriber
	{
		bool operator()(const GroupSubscriber& gs1, const GroupSubscriber& gs2) const
		{
			return gs1.group <  gs2.group ||
				gs1.group == gs2.group && gs1.subscriber < gs2.subscriber;
		}
	};
	typedef std::map<GroupSubscriber, Connection, ltGroupSubscriber> ConnectionOrdering;


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	const String	d_name;		//!< Name of this event
	ConnectionOrdering connectionOrdering;
	friend class ConnectionImpl;
};


} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIEvent_h_
