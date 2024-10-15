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
#include <dpp/snowflake.h>
#include <dpp/json_fwd.h>
#include <unordered_map>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Represents a guild template
 */
class DPP_EXPORT dtemplate : public json_interface<dtemplate> {
protected:
	friend struct json_interface<dtemplate>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	dtemplate& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build the JSON for this object
	 *
	 * @param with_id Add ID to output
	 * @return json JSON content
	 */
	json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Template code
	 */
	std::string code;

	/**
	 * @brief Template name
	 */
	std::string name;

	/**
	 * @brief Template description
	 */
	std::string description;

	/**
	 * @brief Usage counter
	 */
	uint32_t usage_count;

	/**
	 * @brief User ID of creator
	 */
	snowflake creator_id;

	/**
	 * @brief Creation date/time
	 * 
	 */
	time_t created_at;

	/**
	 * @brief Last update date/time
	 */
	time_t updated_at;

	/**
	 * @brief Guild id the template is created from
	 */
	snowflake source_guild_id;

	/**
	 * @brief True if needs synchronising
	 */
	bool is_dirty;

	/**
	 * @brief Construct a new dtemplate object
	 */
	dtemplate();

	/**
	 * @brief Destroy the dtemplate object
	 */
	virtual ~dtemplate() = default;
};

/**
 * @brief A container of invites
 */
typedef std::unordered_map<snowflake, dtemplate> dtemplate_map;

} // namespace dpp
