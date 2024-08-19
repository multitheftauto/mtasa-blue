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
#include <time.h>
#include <map>
#include <functional>
#include <string>

namespace dpp {

/**
 * @brief A timed_listener is a way to temporarily attach to an event for a specific timeframe, then detach when complete.
 * A lambda may also be optionally called when the timeout is reached. Destructing the timed_listener detaches any attached
 * event listeners, and cancels any created timers, but does not call any timeout lambda.
 * 
 * @tparam attached_event Event within cluster to attach to within the cluster::dispatch member (dpp::dispatcher object)
 * @tparam listening_function Definition of lambda function that matches up with the attached_event.
 */
template <typename attached_event, class listening_function> class timed_listener 
{
private:
	/**
	 * @brief Owning cluster.
	 */
	cluster* owner;

	/**
	 * @brief Duration of listen.
	 */
	time_t duration;

	/**
	 * @brief Reference to attached event in cluster.
	 */
	//event_router_t<thread_member_update_t> on_thread_member_update;
	attached_event& ev;

	/**
	 * @brief Timer handle.
	 */
	timer th;

	/**
	 * @brief Event handle.
	 */
	event_handle listener_handle;
	
public:
	/**
	 * @brief Construct a new timed listener object
	 * 
	 * @param cl Owning cluster
	 * @param _duration Duration of timed event in seconds
	 * @param event Event to hook, e.g. cluster.on_message_create
	 * @param on_end An optional void() lambda to trigger when the timed_listener times out.
	 * Calling the destructor before the timeout is reached does not call this lambda.
	 * @param listener Lambda to receive events. Type must match up properly with that passed into the 'event' parameter.
	 */
	timed_listener(cluster* cl, uint64_t _duration, attached_event& event, listening_function listener, timer_callback_t on_end = {})
	: owner(cl), duration(_duration), ev(event)
	{
		/* Attach event */
		listener_handle = ev(listener);
		/* Create timer */
		th = cl->start_timer([this]([[maybe_unused]] dpp::timer timer_handle) {
			/* Timer has finished, detach it from event.
			 * Only allowed to tick once.
			 */
			ev.detach(listener_handle);
			owner->stop_timer(th);
		}, duration, on_end);
	}

	/**
	 * @brief Destroy the timed listener object
	 */
	~timed_listener() {
		/* Stop timer and detach event, but do not call on_end */
		ev.detach(listener_handle);
		owner->stop_timer(th);
	}
};

} // namespace dpp
