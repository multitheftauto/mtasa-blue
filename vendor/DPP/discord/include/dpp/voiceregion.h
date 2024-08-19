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
#include <unordered_map>
#include <dpp/json_fwd.h>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Flags related to a voice region
 */
enum voiceregion_flags {
    	/**
	 * @brief The closest (optimal) voice region.
	 */
	v_optimal = 0x00000001,

	/**
	 * @brief A Deprecated voice region (avoid switching to these).
	 */
	v_deprecated = 0x00000010,

	/**
	 * @brief A custom voice region (used for events/etc).
	 */
	v_custom = 0x00000100
};

/**
 * @brief Represents a voice region on discord
 */
class DPP_EXPORT voiceregion : public json_interface<voiceregion> {
protected:
	friend struct json_interface<voiceregion>;

	/**
	 * @brief Fill object properties from JSON
	 * 
	 * @param j JSON to fill from
	 * @return voiceregion& Reference to self
	 */
	voiceregion& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build a json for this object
	 *
	 * @param with_id Add ID to output
	 * @return json JSON string
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Voice server ID
	 */
	std::string id;

	/**
	 * @brief Voice server name
	 */
	std::string name;

	/**
	 * @brief Flags bitmap
	 */
	uint8_t flags;

	/**
	 * @brief Construct a new voiceregion object
	 */
	voiceregion();

	/**
	 * @brief Destroy the voiceregion object
	 */
	virtual ~voiceregion() = default;

	/**
	 * @brief True if is the optimal voice server
	 * 
	 * @return true if optimal 
	 */
	bool is_optimal() const;

	/**
	 * @brief True if is a deprecated voice server
	 * 
	 * @return true if deprecated
	 */
	bool is_deprecated() const;

	/**
	 * @brief True if is a custom voice server
	 * 
	 * @return true if custom
	 */
	bool is_custom() const;
};

/**
 * @brief A group of voice regions
 */
typedef std::unordered_map<std::string, voiceregion> voiceregion_map;

} // namespace dpp
