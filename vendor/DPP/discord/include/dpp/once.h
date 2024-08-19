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
#include <utility>

namespace dpp {

/**
 * @brief Run some code within an if() statement only once.
 *
 * Use this template like this:
 *
 * ```
 * if (dpp::run_once<struct any_unique_name_you_like_here>()) {
 *     // Your code here
 * }
 * ```
 *
 * @tparam T any unique 'tag' identifier name
 * @return auto a true/false return to say if we should execute or not
 */
template <typename T> auto run_once() {
	static auto called = false;
	return !std::exchange(called, true);
};

} // namespace dpp
