/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * Copyright 2021 Craig Edwards and D++ contributors
 * (https://github.com/brainboxdotcc/DPP/graphs/contributors)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************************/
#pragma once

#include <dpp/export.h>
#include <dpp/cluster.h>
#include <dpp/timed_listener.h>
#include <time.h>
#include <vector>
#include <functional>
#include <string>

namespace dpp {

/**
 * @brief Collects objects from events during a specified time period.
 * 
 * This template must be specialised. There are premade specialisations which you can use
 * such as dpp::reaction_collector and dpp::message_collector. For these specialised instances
 * all you need to do is derive a simple class from them which implements collector::completed().
 * 
 * A collector will run for the specified number of seconds, attaching itself to the
 * given event. During this time any events pass through the collector and collector::filter().
 * This function can return a pointer to an object to allow a copy of that object to be stored
 * to a vector, or it can return nullptr to do nothing with that object. For example a collector
 * attached to on_message_create would receive an event with the type message_create_t, and from
 * this may decide to extract the message_create_t::msg structure, returning a pointer to it, or
 * instead may choose to return a nullptr.
 * 
 * When either the predetermined timeout is reached, or the collector::cancel() method is called,
 * or the collector is destroyed, the collector::completed() method is called, which will be
 * passed a list of collected objects in the order they were collected.
 * 
 * @tparam T parameter type of the event this collector will monitor
 * @tparam C object type this collector will store
 */
template<class T, class C> class collector
{
protected:
	/**
	 * @brief Owning cluster.
	 */
	class cluster* owner;
private:
	/**
	 * @brief Timed listener.
	 */
	timed_listener<event_router_t<T>, std::function<void(const T&)>>* tl;

	/**
	 * @brief Stored list.
	 */
	std::vector<C> stored;

	/**
	 * @brief Trigger flag.
	 */
	bool triggered;
public:
	/**
	 * @brief Construct a new collector object.
	 * 
	 * The timer for the collector begins immediately on construction of the object.
	 * 
	 * @param cl Pointer to cluster which manages this collector
	 * @param duration Duration in seconds to run the collector for
	 * @param event Event to attach to, e.g. cluster::on_message_create
	 */
	collector(class cluster* cl, uint64_t duration, event_router_t<T> & event) : owner(cl), triggered(false) {
		std::function<void(const T&)> f = [this](const T& event) {
			const C* v = filter(event);
			if (v) {
				stored.push_back(*v);
			}
		};
		tl = new dpp::timed_listener<event_router_t<T>, std::function<void(const T&)>>(cl, duration, event, f, [this]([[maybe_unused]] dpp::timer timer_handle) {
			if (!triggered) {
				triggered = true;
				completed(stored);
			}
		});
	}

	/**
	 * @brief You must implement this function to receive the completed list of
	 * captured objects.
	 * @param list The list of captured objects in captured order
	 */
	virtual void completed(const std::vector<C>& list) = 0;

	/**
	 * @brief Filter the list of elements.
	 * 
	 * Every time an event is fired on the collector, this method wil be called
	 * to determine if we should add an object to the list or not. This function
	 * can then process the `element` value, extract the parts which are to be
	 * saved to a list (e.g. a dpp::message out of a dpp::message_create_t) and
	 * return it as the return value. Returning a value of nullptr causes no
	 * object to be stored.
	 * 
	 * Here is an example of how to filter messages which have specific text in them.
	 * This should be used with the specialised type dpp::message_collector
	 * 
	 * ```cpp
	 * virtual const dpp::message* filter(const dpp::message_create_t& m) {
		 *	 if (m.msg.content.find("something i want") != std::string::npos) {
	 *		 return &m.msg;
	 *	 } else {
	 *		 return nullptr;
	 *	 }
	 * }
	 * ```
	 * 
	 * @param element The event data to filter
	 * @return const C* Returned object or nullptr
	 */
	virtual const C* filter(const T& element) = 0;

	/**
	 * @brief Immediately cancels the collector.
	 * 
	 * Use this if you have met the conditions for which you are collecting objects
	 * early, e.g. you were watching for a message containing 'yes' or 'no' and have
	 * received it before the time is up.
	 * 
	 * @note Causes calling of the completed() method if it has not yet been called.
	 */
	virtual void cancel() {
		delete tl;
		tl = nullptr;
	}

	/**
	 * @brief Destroy the collector object.
	 * @note Causes calling of the completed() method if it has not yet been called.
	 */
	virtual ~collector() {
		delete tl;
	}
};

/**
 * @brief Represents a reaction.
 * Can be filled for use in a collector
 */
class collected_reaction : public managed {
public:
	/**
	 * @brief Reacting user.
	 */
	user react_user;

	/**
	 * @brief Reacting guild.
	 */
	guild* react_guild{};

	/**
	 * @brief Reacting guild member.
	 */
	guild_member react_member;

	/**
	 * @brief Reacting channel.
	 */
	channel* react_channel{};

	/**
	 * @brief Reacted emoji.
	 */
	emoji react_emoji;

	/**
	 * @brief Optional: ID of the user who authored the message which was reacted to.
	 */
	snowflake message_author_id{};
};

/**
 * @brief Template type for base class of channel collector
 */
typedef dpp::collector<dpp::channel_create_t, dpp::channel> channel_collector_t;

/**
 * @brief Template type for base class of thread collector
 */
typedef dpp::collector<dpp::thread_create_t, dpp::thread> thread_collector_t;

/**
 * @brief Template type for base class of role collector
 */
typedef dpp::collector<dpp::guild_role_create_t, dpp::role> role_collector_t;

/**
 * @brief Template type for base class of scheduled event collector
 */
typedef dpp::collector<dpp::guild_scheduled_event_create_t, dpp::scheduled_event> scheduled_event_collector_t;

/**
 * @brief Template type for base class of message collector
 */
typedef dpp::collector<dpp::message_create_t, dpp::message> message_collector_t;

/**
 * @brief Template type for base class of message reaction collector
 */
typedef dpp::collector<dpp::message_reaction_add_t, dpp::collected_reaction> reaction_collector_t;

/**
 * @brief Message collector.
 * Collects messages during a set timeframe and returns them in a list via the completed() method.
 */
class message_collector : public message_collector_t {
public:
	/**
	 * @brief Construct a new message collector object
	 * 
	 * @param cl cluster to associate the collector with
	 * @param duration Duration of time to run the collector for in seconds
	 */
	message_collector(cluster* cl, uint64_t duration) : message_collector_t::collector(cl, duration, cl->on_message_create) { }

	/**
	 * @brief Return the completed collection
	 * 
	 * @param list items collected during the timeframe specified
	 */
	virtual void completed(const std::vector<dpp::message>& list) = 0;

	/**
	 * @brief Select and filter the items which are to appear in the list
	 * This is called every time a new event is fired, to filter the event and determine which
	 * of the items is sent to the list. Returning nullptr excludes the item from the list.
	 * 
	 * @param element element to filter
	 * @return Returned item to add to the list, or nullptr to skip adding this element
	 */
	virtual const dpp::message* filter(const dpp::message_create_t& element) { return &element.msg; }

	/**
	 * @brief Destroy the message collector object
	 */
	virtual ~message_collector() = default;
};

/**
 * @brief Reaction collector.
 * Collects message reactions during a set timeframe and returns them in a list via the completed() method.
 */
class reaction_collector : public reaction_collector_t {
	/**
	 * @brief The ID of the message.
	 */
	snowflake message_id;

	/**
	 * @brief The reaction.
	 */
	collected_reaction react;
public:
	/**
	 * @brief Construct a new reaction collector object
	 * 
	 * @param cl cluster to associate the collector with
	 * @param duration Duration of time to run the collector for in seconds
	 * @param msg_id Optional message ID. If specified, only collects reactions for the given message
	 */
	reaction_collector(cluster* cl, uint64_t duration, snowflake msg_id = 0) : reaction_collector_t::collector(cl, duration, cl->on_message_reaction_add), message_id(msg_id) { }

	/**
	 * @brief Return the completed collection
	 * 
	 * @param list items collected during the timeframe specified
	 */
	virtual void completed(const std::vector<dpp::collected_reaction>& list) = 0;

	/**
	 * @brief Select and filter the items which are to appear in the list
	 * This is called every time a new event is fired, to filter the event and determine which
	 * of the items is sent to the list. Returning nullptr excludes the item from the list.
	 * 
	 * @param element element to filter
	 * @return Returned item to add to the list, or nullptr to skip adding this element
	 */
	virtual const dpp::collected_reaction* filter(const dpp::message_reaction_add_t& element) {
		/* Capture reactions for given message ID only */
		if (message_id.empty() || element.message_id == message_id) {
			react.id = element.message_id;
			react.react_user = element.reacting_user;
			react.react_guild = element.reacting_guild;
			react.react_member = element.reacting_member;
			react.react_channel = element.reacting_channel;
			react.react_emoji = element.reacting_emoji;
			react.message_author_id = element.message_author_id;
			return &react;
		} else {
			return nullptr;
		}
	}

	/**
	 * @brief Destroy the reaction collector object
	 */
	virtual ~reaction_collector() = default;
};

/**
 * @brief Channel collector.
 * Collects channels during a set timeframe and returns them in a list via the completed() method.
 */
class channel_collector : public channel_collector_t {
public:
	/**
	 * @brief Construct a new channel collector object
	 * 
	 * @param cl cluster to associate the collector with
	 * @param duration Duration of time to run the collector for in seconds
	 */
	channel_collector(cluster* cl, uint64_t duration) : channel_collector_t::collector(cl, duration, cl->on_channel_create) { }

	/**
	 * @brief Return the completed collection
	 * 
	 * @param list items collected during the timeframe specified
	 */
	virtual void completed(const std::vector<dpp::channel>& list) = 0;

	/**
	 * @brief Select and filter the items which are to appear in the list
	 * This is called every time a new event is fired, to filter the event and determine which
	 * of the items is sent to the list. Returning nullptr excludes the item from the list.
	 * 
	 * @param element element to filter
	 * @return Returned item to add to the list, or nullptr to skip adding this element
	 */
	virtual const dpp::channel* filter(const dpp::channel_create_t& element) { return element.created; }

	/**
	 * @brief Destroy the channel collector object
	 */
	virtual ~channel_collector() = default;
};

/**
 * @brief Thread collector.
 * Collects threads during a set timeframe and returns them in a list via the completed() method.
 */
class thread_collector : public thread_collector_t {
public:
	/**
	 * @brief Construct a new thread collector object
	 * 
	 * @param cl cluster to associate the collector with
	 * @param duration Duration of time to run the collector for in seconds
	 */
	thread_collector(cluster* cl, uint64_t duration) : thread_collector_t::collector(cl, duration, cl->on_thread_create) { }

	/**
	 * @brief Return the completed collection
	 * 
	 * @param list items collected during the timeframe specified
	 */
	virtual void completed(const std::vector<dpp::thread>& list) = 0;

	/**
	 * @brief Select and filter the items which are to appear in the list
	 * This is called every time a new event is fired, to filter the event and determine which
	 * of the items is sent to the list. Returning nullptr excludes the item from the list.
	 * 
	 * @param element element to filter
	 * @return Returned item to add to the list, or nullptr to skip adding this element
	 */
	virtual const dpp::thread* filter(const dpp::thread_create_t& element) { return &element.created; }

	/**
	 * @brief Destroy the thread collector object
	 */
	virtual ~thread_collector() = default;
};

/**
 * @brief Role collector.
 * Collects guild roles during a set timeframe and returns them in a list via the completed() method.
 */
class role_collector : public role_collector_t {
public:
	/**
	 * @brief Construct a new role collector object
	 * 
	 * @param cl cluster to associate the collector with
	 * @param duration Duration of time to run the collector for in seconds
	 */
	role_collector(cluster* cl, uint64_t duration) : role_collector_t::collector(cl, duration, cl->on_guild_role_create) { }

	/**
	 * @brief Return the completed collection
	 * 
	 * @param list items collected during the timeframe specified
	 */
	virtual void completed(const std::vector<dpp::role>& list) = 0;

	/**
	 * @brief Select and filter the items which are to appear in the list
	 * This is called every time a new event is fired, to filter the event and determine which
	 * of the items is sent to the list. Returning nullptr excludes the item from the list.
	 * 
	 * @param element element to filter
	 * @return Returned item to add to the list, or nullptr to skip adding this element
	 */
	virtual const dpp::role* filter(const dpp::guild_role_create_t& element) { return element.created; }

	/**
	 * @brief Destroy the role collector object
	 */
	virtual ~role_collector() = default;
};

/**
 * @brief Scheduled event collector.
 * Collects messages during a set timeframe and returns them in a list via the completed() method.
 */
class scheduled_event_collector : public scheduled_event_collector_t {
public:
	/**
	 * @brief Construct a new scheduled event collector object
	 * 
	 * @param cl cluster to associate the collector with
	 * @param duration Duration of time to run the collector for in seconds
	 */
	scheduled_event_collector(cluster* cl, uint64_t duration) : scheduled_event_collector_t::collector(cl, duration, cl->on_guild_scheduled_event_create) { }

	/**
	 * @brief Return the completed collection
	 * 
	 * @param list items collected during the timeframe specified
	 */
	virtual void completed(const std::vector<dpp::scheduled_event>& list) = 0;

	/**
	 * @brief Select and filter the items which are to appear in the list
	 * This is called every time a new event is fired, to filter the event and determine which
	 * of the items is sent to the list. Returning nullptr excludes the item from the list.
	 * 
	 * @param element element to filter
	 * @return Returned item to add to the list, or nullptr to skip adding this element
	 */
	virtual const dpp::scheduled_event* filter(const dpp::guild_scheduled_event_create_t& element) { return &element.created; }

	/**
	 * @brief Destroy the scheduled event collector object
	 */
	virtual ~scheduled_event_collector() = default;
};

} // namespace dpp
