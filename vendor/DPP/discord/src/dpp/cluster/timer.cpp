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
#include <dpp/timer.h>
#include <dpp/cluster.h>
#include <dpp/json.h>

namespace dpp {

timer lasthandle = 1;
std::mutex timer_guard;

timer cluster::start_timer(timer_callback_t on_tick, uint64_t frequency, timer_callback_t on_stop) {
	std::lock_guard<std::mutex> l(timer_guard);
	timer_t* newtimer = new timer_t();

	newtimer->handle = lasthandle++;
	newtimer->next_tick = time(nullptr) + frequency;
	newtimer->on_tick = on_tick;
	newtimer->on_stop = on_stop;
	newtimer->frequency = frequency;
	timer_list[newtimer->handle] = newtimer;
	next_timer.emplace(newtimer->next_tick, newtimer);

	return newtimer->handle;
}

bool cluster::stop_timer(timer t) {
	std::lock_guard<std::mutex> l(timer_guard);

	auto i = timer_list.find(t);
	if (i != timer_list.end()) {
		timer_t* tptr = i->second;
		if (tptr->on_stop) {
			/* If there is an on_stop event, call it */
			tptr->on_stop(t);
		}
		timer_list.erase(i);
		auto j = next_timer.find(tptr->next_tick);
		if (j != next_timer.end()) {
			next_timer.erase(j);
		}
		delete tptr;
		return true;
	}
	return false;
}

void cluster::timer_reschedule(timer_t* t) {
	std::lock_guard<std::mutex> l(timer_guard);
	for (auto i = next_timer.begin(); i != next_timer.end(); ++i) {
		/* Rescheduling the timer means finding it in the next tick map.
		 * It should be pretty much near the start of the map so this loop
		 * should only be at most a handful of iterations.
		 */
		if (i->second->handle == t->handle) {
			next_timer.erase(i);
			t->next_tick = time(nullptr) + t->frequency;
			next_timer.emplace(t->next_tick, t);
			break;
		}
	}
}

void cluster::tick_timers() {
	std::vector<timer_t*> scheduled;
	{
		time_t now = time(nullptr);
		std::lock_guard<std::mutex> l(timer_guard);
		for (auto i = next_timer.begin(); i != next_timer.end(); ++i) {
			if (now >= i->second->next_tick) {
				scheduled.push_back(i->second);
			} else {
				/* The first time we encounter an entry which is not due,
				 * we can bail out, because std::map is ordered storage so
				 * we know at this point no more will match either.
				 */
				break;
			}
		}
	}
	for (auto & t : scheduled) {
		timer handle = t->handle;
		/* Call handler */
		t->on_tick(t->handle);
		/* Reschedule if it wasn't deleted.
		 * Note: We wrap the .contains() check in a lambda as it needs locking
		 * for thread safety, but timer_rescheudle also locks the container, so this
		 * is the cleanest way to do it.
		 */
		bool not_deleted = ([handle, this]() -> bool {
			std::lock_guard<std::mutex> l(timer_guard);
			return timer_list.find(handle) != timer_list.end();
		}());
		if (not_deleted) {
			timer_reschedule(t);
		}
	}
}

#ifdef DPP_CORO
async<timer> cluster::co_sleep(uint64_t seconds) {
	return async<timer>{[this, seconds] (auto &&cb) mutable {
		start_timer([this, cb] (dpp::timer handle) {
			cb(handle);
			stop_timer(handle);
		}, seconds);
	}};
}
#endif

oneshot_timer::oneshot_timer(class cluster* cl, uint64_t duration, timer_callback_t callback) : owner(cl) {
	/* Create timer */
	th = cl->start_timer([callback, this](dpp::timer timer_handle) {
		callback(this->get_handle());
		this->owner->stop_timer(this->th);
	}, duration);
}

timer oneshot_timer::get_handle() {
	return this->th;
}

void oneshot_timer::cancel() {
	owner->stop_timer(this->th);
}

oneshot_timer::~oneshot_timer() {
	cancel();
}

} // namespace dpp
