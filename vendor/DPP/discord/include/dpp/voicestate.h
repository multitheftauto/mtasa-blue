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
 * @brief Bit mask flags relating to voice states
 */
enum voicestate_flags {
	/**
	 * @brief Deafened by the server.
	 */
	vs_deaf	= 0b00000001,

	/**
	 * @brief Muted by the server.
	 */
	vs_mute	= 0b00000010,

	/**
	 * @brief Locally Muted.
	 */
	vs_self_mute = 0b00000100,

	/**
	 * @brief Locally deafened.
	 */
	vs_self_deaf = 0b00001000,

	/**
	 * @brief Whether this user is streaming using "Go Live".
	 */
	vs_self_stream = 0b00010000,

	/**
	 * @brief Whether this user's camera is enabled.
	 */
	vs_self_video =	0b00100000,

	/**
	 * @brief Whether this user's permission to speak is denied.
	 */
	vs_suppress = 0b01000000
};

/**
 * @brief Represents the voice state of a user on a guild
 * These are stored in the dpp::guild object, and accessible there,
 * or via dpp::channel::get_voice_members
 */
class DPP_EXPORT voicestate : public json_interface<voicestate> {
protected:
	friend struct json_interface<voicestate>;

	/**
	 * @brief Fill voicestate object from json data
	 * 
	 * @param j JSON data to fill from
	 * @return voicestate& Reference to self
	 */
	voicestate& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief Owning shard.
	 */
	class discord_client* shard;

	/**
	 * @brief Optional: The guild id this voice state is for.
	 */
	snowflake guild_id{0};

	/**
	 * @brief The channel id this user is connected to.
	 *
	 * @note This may be empty.
	 */
	snowflake channel_id{0};

	/**
	 * @brief The user id this voice state is for.
	 */
	snowflake user_id{0};

	/**
	 * @brief The session id for this voice state.
	 */
	std::string session_id;

	/**
	 * @brief Voice state flags from dpp::voicestate_flags.
	 */
	uint8_t	flags{0};

	/**
	 * @brief The time at which the user requested to speak.
	 *
	 * @note If the user never requested to speak, this is 0.
	 */
	time_t request_to_speak{0};

	/**
	 * @brief Construct a new voicestate object
	 */
	voicestate();

	/**
	 * @brief Destroy the voicestate object
	 */
	virtual ~voicestate() = default;

	/**
	 * @brief Return true if the user is deafened by the server.
	 */
	bool is_deaf() const;

	/**
	 * @brief Return true if the user is muted by the server.
	 */
	bool is_mute() const;

	/**
	 * @brief Return true if user muted themselves.
	 */
	bool is_self_mute() const;

	/**
	 * @brief Return true if user deafened themselves.
	 */
	bool is_self_deaf() const;

	/**
	 * @brief Return true if the user is streaming using "Go Live".
	 */
	bool self_stream() const;

	/**
	 * @brief Return true if the user's camera is enabled.
	 */
	bool self_video() const;

	/**
	 * @brief Return true if user is suppressed.
	 *
	 * "HELP HELP I'M BEING SUPPRESSED!"
	 */
	bool is_suppressed() const;
};

/** A container of voicestates */
typedef std::unordered_map<std::string, voicestate> voicestate_map;

} // namespace dpp
