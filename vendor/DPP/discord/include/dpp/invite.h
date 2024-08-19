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
#include <dpp/stage_instance.h>
#include <unordered_map>
#include <dpp/json_interface.h>
#include <dpp/channel.h>
#include <dpp/user.h>
#include <dpp/guild.h>

namespace dpp {

/**
 * @brief Invite target types for dpp::invite
 */
enum invite_target_t : uint8_t {
	/**
	 * @brief Undefined invite target type.
	 */
	itt_none = 0,

	/**
	 * @brief Stream target type.
	 */
	itt_stream = 1,

	/**
	 * @brief Embedded Application target type.
	 */
	itt_embedded_application = 2,
};

/**
 * @brief Represents an invite to a discord guild or channel
 */
class DPP_EXPORT invite : public json_interface<invite> {
protected:
	friend struct json_interface<invite>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	invite& fill_from_json_impl(nlohmann::json* j);

	/** Build JSON from this object.
	 * @param with_id Include ID in JSON
	 * @return The JSON of the invite
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Invite code.
	 */
	std::string code;

	/**
	 * @brief Readonly expiration timestamp of this invite or 0 if the invite doesn't expire.
	 * @note Only returned from cluster::invite_get
	 */
	time_t expires_at;

	/**
	 * @brief Guild ID this invite is for.
	 */
	snowflake guild_id;

	/**
	 * @brief The partial guild this invite is for.
	 * @note Only filled in retrieved invites.
	 */
	guild destination_guild;

	/**
	 * @brief Channel ID this invite is for.
	 */
	snowflake channel_id;

	/**
	 * @brief The partial channel this invite is for.
	 * @note Only filled in retrieved invites.
	 */
	channel destination_channel;

	/**
	 * @brief User ID who created this invite.
	 * @deprecated Use the `inviter` field instead
	 */
	snowflake inviter_id;

	/**
	 * @brief User who created this invite.
	 */
	user inviter;

	/**
	 * @brief The user ID whose stream to display for this voice channel stream invite.
	 */
	snowflake target_user_id;

	/**
	 * @brief Target type for this voice channel invite.
	 */
	invite_target_t target_type;

	/**
	 * @brief Approximate number of online users.
	 * @note Only returned from cluster::invite_get
	 */
	uint32_t approximate_presence_count;

	/**
	 * @brief Approximate number of total users online and offline.
	 * @note Only returned from cluster::invite_get.
	 */
	uint32_t approximate_member_count;

	/**
	 * @brief Duration (in seconds) after which the invite expires, or 0 for no expiration. Defaults to 86400 (1 day).
	 *
	 * @note Must be between 0 and 604800 (7 days).
	 */
	uint32_t max_age;

	/**
	 * @brief Maximum number of uses, or 0 for unlimited. Defaults to 0.
	 *
	 * @note Must be between 0 and 100.
	 */
	uint8_t max_uses;

	/**
	 * @brief Whether this invite only grants temporary membership.
	 */
	bool temporary;

	/**
	 * @brief True if this invite should not replace or "attach to" similar invites.
	 */
	bool unique;

	/**
	 * @brief How many times this invite has been used.
	 */
	uint32_t uses;

	/**
	 * @note The stage instance data if there is a public stage instance in the stage channel this invite is for.
	 * @deprecated Deprecated
	 */
	stage_instance stage;

	/**
	 * @brief Timestamp at which the invite was created.
	 */
	time_t created_at;

	/**
	 * @brief Constructor.
	 */
	invite();

	/**
	 * @brief Destructor.
	 */
	virtual ~invite() = default;

	/**
	 * @brief Set the max age after which the invite expires
	 *
	 * @param max_age_ The duration in seconds, or 0 for no expiration. Must be between 0 and 604800 (7 days)
	 * @return invite& reference to self for chaining of calls
	 */
	invite& set_max_age(const uint32_t max_age_);

	/**
	 * @brief Set the maximum number of uses for this invite
	 *
	 * @param max_uses_ Maximum number of uses, or 0 for unlimited. Must be between 0 and 100
	 * @return invite& reference to self for chaining of calls
	 */
	invite& set_max_uses(const uint8_t max_uses_);

	/**
	 * @brief Set the target user id
	 *
	 * @param user_id The user ID whose stream to display for this voice channel stream invite
	 * @return invite& reference to self for chaining of calls
	 */
	invite& set_target_user_id(const snowflake user_id);

	/**
	 * @brief Set the target type for this voice channel invite
	 *
	 * @param type invite_target_t Target type
	 * @return invite& reference to self for chaining of calls
	 */
	invite& set_target_type(const invite_target_t type);

	/**
	 * @brief Set temporary property of this invite object
	 *
	 * @param is_temporary Whether this invite only grants temporary membership
	 * @return invite& reference to self for chaining of calls
	 */
	invite& set_temporary(const bool is_temporary);

	/**
	 * @brief Set unique property of this invite object
	 *
	 * @param is_unique True if this invite should not replace or "attach to" similar invites
	 * @return invite& reference to self for chaining of calls
	 */
	invite& set_unique(const bool is_unique);
};

/**
 * @brief A container of invites
 */
typedef std::unordered_map<std::string, invite> invite_map;

} // namespace dpp
