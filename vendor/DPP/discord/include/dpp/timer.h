/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include <stdint.h>
#include <map>
#include <unordered_map>
#include <stddef.h>
#include <ctime>
#include <functional>

namespace dpp {

/**
 * @brief Represents a timer handle.
 * Returned from cluster::start_timer and used by cluster::stop_timer.
 * This is obtained from a simple incrementing value, internally.
 */
typedef size_t timer;

/**
 * @brief The type for a timer callback
 */
typedef std::function<void(timer)> timer_callback_t;

/**
 * @brief Used internally to store state of active timers
 */
struct DPP_EXPORT timer_t {
	/**
	 * @brief Timer handle
	 */
	timer handle;

	/**
	 * @brief Next timer tick as unix epoch
	 */
	time_t next_tick;

	/**
	 * @brief Frequency between ticks
	 */
	uint64_t frequency;

	/**
	 * @brief Lambda to call on tick
	 */
	timer_callback_t on_tick;

	/**
	 * @brief Lambda to call on stop (optional)
	 */
	timer_callback_t on_stop;
};

/**
 * @brief A map of timers, ordered by earliest first so that map::begin() is always the 
 * soonest to be due.
 */
typedef std::multimap<time_t, timer_t*> timer_next_t;

/**
 * @brief A map of timers stored by handle
 */
typedef std::unordered_map<timer, timer_t*> timer_reg_t;

/**
 * @brief Trigger a timed event once.
 * The provided callback is called only once.
 */
class DPP_EXPORT oneshot_timer
{
private:
	/**
	 * @brief Owning cluster.
	 */
	class cluster* owner;

	/**
	 * @brief Timer handle.
	 */
	timer th;
public:
	/**
	 * @brief Construct a new oneshot timer object
	 * 
	 * @param cl cluster owner
	 * @param duration duration before firing
	 * @param callback callback to call on firing
	 */
	oneshot_timer(class cluster* cl, uint64_t duration, timer_callback_t callback);

	/**
	 * @brief Get the handle for the created one-shot timer
	 * 
	 * @return timer handle for use with stop_timer
	 */
	timer get_handle();

	/**
	 * @brief Cancel the one shot timer immediately.
	 * Callback function is not called.
	 */
	void cancel();

	/**
	 * @brief Destroy the oneshot timer object
	 */
	~oneshot_timer();
};

} // namespace dpp
