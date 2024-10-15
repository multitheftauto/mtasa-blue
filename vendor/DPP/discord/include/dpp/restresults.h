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

#pragma once

#include <dpp/export.h>
#include <string>
#include <map>
#include <variant>
#include <dpp/snowflake.h>
#include <dpp/dispatcher.h>
#include <dpp/misc-enum.h>
#include <dpp/timer.h>
#include <dpp/json_fwd.h>
#include <dpp/discordclient.h>
#include <dpp/voiceregion.h>
#include <dpp/dtemplate.h>
#include <dpp/prune.h>
#include <dpp/auditlog.h>
#include <dpp/queues.h>
#include <dpp/cache.h>
#include <dpp/intents.h>
#include <dpp/sync.h>
#include <algorithm>
#include <iostream>
#include <shared_mutex>
#include <cstring>
#include <dpp/entitlement.h>
#include <dpp/sku.h>

namespace dpp {

/**
 * @brief A list of shards
 */
typedef std::map<uint32_t, class discord_client*> shard_list;

/**
 * @brief Represents the various information from the 'get gateway bot' api call
 */
struct DPP_EXPORT gateway : public json_interface<gateway> {
protected:
	friend struct json_interface<gateway>;

	/**
	 * @brief Fill this object from json
	 * 
	 * @param j json to fill from
	 * @return gateway& reference to self
	 */
	gateway& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief Gateway websocket url.
	 */
	std::string url;

	/**
	 * @brief Number of suggested shards to start.
	 */
	uint32_t shards;

	/**
	 * @brief Total number of sessions that can be started.
	 */
	uint32_t session_start_total;

	/**
	 * @brief How many sessions are left.
	 */
	uint32_t session_start_remaining;

	/**
	 * @brief How many seconds until the session start quota resets.
	 */
	uint32_t session_start_reset_after;

	/**
	 * @brief How many sessions can be started at the same time.
	 */
	uint32_t session_start_max_concurrency;

	/**
	 * @brief Construct a new gateway object
	 *
	 * @param j JSON data to construct from
	 */
	gateway(nlohmann::json* j);

	/**
	 * @brief Construct a new gateway object
	 */
	gateway();
};

/**
 * @brief Confirmation object represents any true or false simple REST request
 *
 */
struct DPP_EXPORT confirmation {
	bool success;
};

/**
 * @brief A container for types that can be returned for a REST API call
 *
 */
typedef std::variant<
		active_threads,
		application_role_connection,
		application_role_connection_metadata_list,
		confirmation,
		message,
		message_map,
		user,
		user_identified,
		user_map,
		guild_member,
		guild_member_map,
		channel,
		channel_map,
		thread_member,
		thread_member_map,
		guild,
		guild_map,
		guild_command_permissions,
		guild_command_permissions_map,
		role,
		role_map,
		invite,
		invite_map,
		dtemplate,
		dtemplate_map,
		emoji,
		emoji_map,
		ban,
		ban_map,
		voiceregion,
		voiceregion_map,
		integration,
		integration_map,
		webhook,
		webhook_map,
		prune,
		guild_widget,
		gateway,
		interaction,
		interaction_response,
		auditlog,
		slashcommand,
		slashcommand_map,
		stage_instance,
		sticker,
		sticker_map,
		sticker_pack,
		sticker_pack_map,
		application,
		application_map,
		connection,
		connection_map,
		thread,
		thread_map,
		scheduled_event,
		scheduled_event_map,
		event_member,
		event_member_map,
		automod_rule,
		automod_rule_map,
		onboarding,
		welcome_screen,
		entitlement,
		entitlement_map,
		sku,
		sku_map
	> confirmable_t;

/**
 * @brief The details of a field in an error response
 */
struct DPP_EXPORT error_detail {
	/**
	 * @brief Object name which is in error
	 */
	std::string object;

	/**
	 * @brief Field name which is in error
	 */
	std::string field;

	/**
	 * @brief Error code
	 */
	std::string code;

	/**
	 * @brief Error reason (full message)
	 */
	std::string reason;

	/**
	 * @brief Object field index
	 */
	DPP_DEPRECATED("index is unused and will be removed in a future version") int index = 0;
};

/**
 * @brief The full details of an error from a REST response
 */
struct DPP_EXPORT error_info {
	/**
	 * @brief Error code
	 */
	uint32_t code = 0;

	/**
	 * @brief Error message
	 *
	 */
	std::string message;

	/**
	 * @brief Field specific error descriptions
	 */
	std::vector<error_detail> errors;

	/**
	 * @brief Human readable error message constructed from the above
	 */
	std::string human_readable;
};

/**
 * @brief The results of a REST call wrapped in a convenient struct
 */
struct DPP_EXPORT confirmation_callback_t {
	/**
	 * @brief Information about the HTTP call used to make the request.
	 */
	http_request_completion_t http_info;

	/**
	 * @brief Value returned, wrapped in variant.
	 */
	confirmable_t value;

	/**
	 * @brief Owner/creator of the callback object.
	 */
	const class cluster* bot;

	/**
	 * @brief Construct a new confirmation callback t object.
	 */
	confirmation_callback_t() = default;

	/**
	 * @brief Construct a new confirmation callback t object
	 * 
	 * @param creator owning cluster object
	 */
	confirmation_callback_t(cluster* creator);

	/**
	 * @brief Construct a new confirmation callback object
	 *
	 * @param _http The HTTP metadata from the REST call
	 */
	confirmation_callback_t(const http_request_completion_t& _http);

	/**
	 * @brief Construct a new confirmation callback object
	 *
	 * @param creator owning cluster object
	 * @param _value The value to encapsulate in the confirmable_t
	 * @param _http The HTTP metadata from the REST call
	 */
	confirmation_callback_t(cluster* creator, const confirmable_t& _value, const http_request_completion_t& _http);

	/**
	 * @brief Returns true if the call resulted in an error rather than a legitimate value in the
	 * confirmation_callback_t::value member.
	 *
	 * @return true There was an error who's details can be obtained by get_error()
	 * @return false There was no error
	 */
	bool is_error() const;

	/**
	 * @brief Get the error_info object.
	 * The error_info object contains the details of any REST error, if there is an error
	 * (to find out if there is an error check confirmation_callback_t::is_error())
	 *
	 * @return error_info The details of the error message
	 */
	error_info get_error() const;

	/**
	 * @brief Get the stored value via std::get
	 * @tparam T type to get
	 * @return stored value as type T
	 */
	template<typename T>
	T get() const {
		return std::get<T>(value);
	}
};

/**
 * @brief A callback upon command completion
 */
typedef std::function<void(const confirmation_callback_t&)> command_completion_event_t;

/**
 * @brief Automatically JSON encoded HTTP result
 */
typedef std::function<void(json&, const http_request_completion_t&)> json_encode_t;
} // namespace dpp
