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
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Defines a request to count prunable users, or start a prune operation
 */
struct DPP_EXPORT prune : public json_interface<prune> {
protected:
	friend struct json_interface<prune>;

	/** Fill this object from json.
	 * @param j JSON object to fill from
	 * @return A reference to self
	 */
	prune& fill_from_json_impl(nlohmann::json* j);

	/** Build JSON from this object.
	 * @param with_prune_count True if the prune count boolean is to be set in the built JSON
	 * @return The JSON of the prune object
	 */
	virtual json to_json_impl(bool with_prune_count = false) const;

public:
	/**
	 * @brief Destroy this prune object
	 */
	virtual ~prune() = default;

	/**
	 * @brief Number of days to include in the prune.
	 */
	uint32_t days = 0;

	/**
	 * @brief Roles to include in the prune (empty to include everyone).
	 */
	std::vector<snowflake> include_roles;

	/**
	 * @brief True if the count of pruneable users should be returned.
	 * @warning Discord recommend not using this on big guilds.
	 */
	bool compute_prune_count;

	/**
	 * @brief Build JSON from this object.
	 *
	 * @param with_id True if the prune count boolean is to be set in the built JSON
	 * @return The JSON of the prune object
	 */
	json to_json(bool with_id = false) const; // Intentional shadow of json_interface, mostly present for documentation
};

} // namespace dpp
