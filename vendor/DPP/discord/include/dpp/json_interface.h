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
#include <dpp/json.h>

namespace dpp {

/**
 * @brief Represents an interface for an object that can optionally implement functions
 * for converting to and from nlohmann::json. The methods are only present if the actual object
 * also has those methods.
 *
 * @tparam T Type of class that implements the interface
 */
template<typename T>
struct json_interface {
	/**
	 * @brief Convert object from nlohmann::json
	 *
	 * @param j nlohmann::json object
	 * @return T& Reference to self for fluent calling
	 */
	template <typename U = T, typename = decltype(std::declval<U&>().fill_from_json_impl(std::declval<nlohmann::json*>()))>
	T& fill_from_json(nlohmann::json* j) {
		return static_cast<T*>(this)->fill_from_json_impl(j);
	}

	/**
	 * @brief Convert object to nlohmann::json
	 *
	 * @param with_id Whether to include the ID or not
	 * @note Some fields are conditionally filled, do not rely on all fields being present
	 * @return json Json built from the structure
	 */
	template <typename U = T, typename = decltype(std::declval<U&>().to_json_impl(bool{}))>
	auto to_json(bool with_id = false) const {
		return static_cast<const T*>(this)->to_json_impl(with_id);
	}

	/**
	 * @brief Convert object to json string
	 *
	 * @param with_id Whether to include the ID or not
	 * @note Some fields are conditionally filled, do not rely on all fields being present
	 * @return std::string Json built from the structure
	 */
	template <typename U = T, typename = decltype(std::declval<U&>().to_json_impl(bool{}))>
	std::string build_json(bool with_id = false) const {
		return to_json(with_id).dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
	}
};

} // namespace dpp
