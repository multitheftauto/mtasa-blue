/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * Copyright 2022 Craig Edwards and D++ contributors
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
#include <dpp/snowflake.h>
#include <future>
#include <utility>
#include <dpp/exception.h>

namespace dpp {

/**
 * @brief Call a D++ REST function synchronously.
 *
 * Synchronously calling a REST function means *IT WILL BLOCK* - This is a Bad Thing™ and strongly discouraged.
 * There are very few circumstances you actually need this. If you do need to use this, you'll know it.
 *
 * Example:
 *
 * ```cpp
 * dpp::message m = dpp::sync<dpp::message>(&bot, &dpp::cluster::message_create, dpp::message(channel_id, "moo."));
 * ```
 *
 * @warning As previously mentioned, this template will block. It is ill-advised to call this outside of
 * a separate thread and this should never be directly used in any event such as dpp::cluster::on_interaction_create!
 * @tparam T type of expected return value, should match up with the method called
 * @tparam F Type of class method in dpp::cluster to call.
 * @tparam Ts Function parameters in method call
 * @param c A pointer to dpp::cluster object
 * @param func pointer to class method in dpp::cluster to call. This can call any
 * dpp::cluster member function who's last parameter is a dpp::command_completion_event_t callback type.
 * @param args Zero or more arguments for the method call
 * @return An instantiated object of type T
 * @throw dpp::rest_exception On failure of the method call, an exception is thrown
 */
template<typename T, class F, class... Ts> T sync(class cluster* c, F func, Ts&&... args) {
	std::promise<T> _p;
	std::future<T> _f = _p.get_future();
	/* (obj ->* func) is the obscure syntax for calling a method pointer on an object instance */
	(c ->* func)(std::forward<Ts>(args)..., [&_p](const auto& cc) {
		try {
			if (cc.is_error()) {
				const auto& error = cc.get_error();
				throw dpp::rest_exception((exception_error_code)error.code, error.message);
			} else {
				try {
					_p.set_value(std::get<T>(cc.value));
				} catch (const std::exception& e) {
					throw dpp::rest_exception(err_unknown, e.what());
				}
			}
		} catch (const dpp::rest_exception&) {
			_p.set_exception(std::current_exception());
		}
	});

	/* Blocking calling thread until rest request is completed. 
	* Exceptions encountered on the other thread are re-thrown.
	*/
	return _f.get();
}

} // namespace dpp
