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
#include <dpp/user.h>

namespace dpp {

/**
 * @brief Integration types
 */
enum integration_type {
	/**
	 * @brief Twitch integration
	 */
	i_twitch,

	/**
	 * @brief YouTube integration
	 */
	i_youtube,

	/**
	 * @brief Discord integration
	 */
	i_discord,

	/**
	 * @brief Subscription
	 */
	i_guild_subscription,
};

/**
 * @brief Integration flags
 */
enum integration_flags {
	/**
	 * @brief Is this integration enabled?
	 */
	if_enabled = 0b00000001,

	/**
	 * @brief Is this integration syncing?
	 * @warning This is not provided for discord bot integrations.
	 */
	if_syncing = 0b00000010,

	/**
	 * @brief Whether emoticons should be synced for this integration (twitch only currently).
	 * @warning This is not provided for discord bot integrations.
	 */
	if_emoticons = 0b00000100,

	/**
	 * @brief Has this integration been revoked?
	 * @warning This is not provided for discord bot integrations.
	 */
	if_revoked = 0b00001000,

	/**
	 * @brief Kick user when their subscription expires, otherwise only remove the role that is specified by `role_id`.
	 * @warning This is not provided for discord bot integrations.
	 */
	if_expire_kick = 0b00010000,
};

/**
 * @brief An application that has been integrated
 */
struct DPP_EXPORT integration_app {
	/**
	 * @brief The id of the app.
	 */
	snowflake id;

	/**
	 * @brief The name of the app.
	 */
	std::string name;

	/**
	 * @brief The icon hash of the app.
	 */
	utility::iconhash icon;

	/**
	 * @brief The description of the app
	 */
	std::string description;

	/**
	 * @brief The bot associated with this application.
	 */
	class user* bot;
};

/**
 * @brief The account information for an integration.
 */
struct DPP_EXPORT integration_account {
	/**
	 * @brief ID of the account
	 */
	snowflake id;

	/**
	 * @brief Name of the account.
	 */
	std::string name;
};

/**
 * @brief Represents an integration on a guild, e.g. a connection to twitch.
 */
class DPP_EXPORT integration : public managed, public json_interface<integration> {
protected:
	friend struct json_interface<integration>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	integration& fill_from_json_impl(nlohmann::json* j);

	/** Build a json from this object.
	 * @param with_id Add ID to output
	 * @return JSON of the object
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Integration name.
	 */
	std::string name;

	/**
	 * @brief Integration type (twitch, youtube, discord, or guild_subscription).
	 */
	integration_type type;

	/**
	 * @brief Integration flags from dpp::integration_flags
	 */
	uint8_t flags;

	/**
	 * @brief ID that this integration uses for "subscribers".
	 *
	 * @warning This is not provided for discord bot integrations.
	 */
	snowflake role_id;

	/**
	 * @brief The grace period (in days) before expiring subscribers.
	 *
	 * @warning This is not provided for discord bot integrations.
	 */
	uint32_t expire_grace_period;

	/**
	 * @brief User for this integration
	 */
	user user_obj;

	/**
	 * @brief Integration account information
	 */
	integration_account account;

	/**
	 * @brief When this integration was last synced.
	 *
	 * @warning This is not provided for discord bot integrations.
	 */
	time_t synced_at;

	/**
	 * @brief How many subscribers this integration has.
	 *
	 * @warning This is not provided for discord bot integrations.
	 */
	uint32_t subscriber_count;

	/**
	 * @brief The bot/OAuth2 application for discord integrations.
	 */
	integration_app app;

	/**
	 * @brief The scopes the application has been authorized for.
	 */
	std::vector<std::string> scopes;

	/** Default constructor */
	integration();

	/** Default destructor */
	~integration() = default;

	/**
	 * Are emoticons enabled for this integration?
	 * @warning This is not provided for discord bot integrations.
	 */
	bool emoticons_enabled() const;

	/**
	 * Is the integration enabled?
	 * @warning This is not provided for discord bot integrations.
	 */
	bool is_enabled() const;

	/**
	 * Is the integration syncing?
	 * @warning This is not provided for discord bot integrations.
	 */
	bool is_syncing() const;

	/**
	 * Has this integration been revoked?
	 * @warning This is not provided for discord bot integrations.
	 */
	bool is_revoked() const;

	/**
	 * Will the user be kicked if their subscription runs out to the integration?
	 * If false, the integration will simply remove the role that is specified by `role_id`.
	 * @warning This is not provided for discord bot integrations.
	 */
	bool expiry_kicks_user() const;
};

/**
 * @brief The connection object that the user has attached.
 */
class DPP_EXPORT connection : public json_interface<connection> {
protected:
	friend struct json_interface<connection>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	connection& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief ID of the connection account.
	 */
	std::string id;

	/**
	 * @brief the username of the connection account.
	 */
	std::string name;

	/**
	 * @brief the service of the connection (twitch, youtube, discord, or guild_subscription).
	 */
	std::string type;

	/**
	 * @brief Optional: whether the connection is revoked.
	 */
	bool revoked;

	/**
	 * @brief Optional: an array of partial server integrations.
	 */
	std::vector<integration> integrations;

	/**
	 * @brief Whether the connection is verified.
	 */
	bool verified;

	/**
	 * @brief Whether friend sync is enabled for this connection.
	 */
	bool friend_sync;

	/**
	 * @brief Whether activities related to this connection will be shown in presence updates.
	 */
	bool show_activity;

	/**
	 * @brief Whether this connection has a corresponding third party OAuth2 token.
	 */
	bool two_way_link;

	/**
	 * @brief Visibility of this connection.
	 */
	bool visible;

	/**
	 * @brief Construct a new connection object
	 */
	connection();
};

/**
 * @brief A group of integrations
 */
typedef std::unordered_map<snowflake, integration> integration_map;

/**
 * @brief A group of connections
 */
typedef std::unordered_map<snowflake, connection> connection_map;

} // namespace dpp

