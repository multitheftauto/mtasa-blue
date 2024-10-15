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
#include <dpp/managed.h>
#include <dpp/json_fwd.h>
#include <unordered_map>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Represents the privacy of a stage instance
 */
enum stage_privacy_level : uint8_t {
	/**
	 * @brief The Stage instance is visible publicly, such as on Stage Discovery.
	 */
	sp_public = 1,

	/**
	 * @brief The Stage instance is visible to only guild members.
	 */
	sp_guild_only = 2
};

/**
 * @brief A stage instance.
 * Stage instances are like a conference facility, with moderators/speakers and listeners.
 */
struct DPP_EXPORT stage_instance : public managed, public json_interface<stage_instance> {
protected:
	friend struct json_interface<stage_instance>;

	/**
	 * @brief Serialise a stage_instance object rom json
	 *
	 * @return stage_instance& a reference to self
	 */
	 stage_instance& fill_from_json_impl(const nlohmann::json* j);

	/**
	 * @brief Build json for this object
	 *
	 * @param with_id include ID
	 * @return json Json of this object
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief The guild ID of the associated Stage channel.
	 */
	snowflake guild_id;

	/**
	 * @brief The ID of the associated Stage channel.
	 */
	snowflake channel_id;

	/**
	 * @brief The topic of the Stage instance (1-120 characters).
	 */
	std::string topic;

	/**
	 * @brief The privacy level of the Stage instance.
	 */
	stage_privacy_level privacy_level;

	/**
	 * @brief Whether or not Stage Discovery is disabled.
	 */
	bool discoverable_disabled;

	/**
	 * @brief Create a stage_instance object
	 */
	stage_instance();

	/**
	 * @brief Destroy the stage_instance object
	 */
	~stage_instance() = default;
};

/**
 * @brief A group of stage instances
 */
typedef std::unordered_map<snowflake, stage_instance> stage_instance_map;

} // namespace dpp
