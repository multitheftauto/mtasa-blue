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
#include <dpp/utility.h>
#include <dpp/user.h>
#include <dpp/guild.h>
#include <dpp/permissions.h>
#include <dpp/json_fwd.h>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief status of a member of a team who maintain a bot/application
 */
enum team_member_status : uint8_t {
	/**
	 * @brief User was invited to the team.
	 */
	tms_invited = 1,
	/**
	 * @brief User has accepted membership onto the team.
	 */
	tms_accepted = 2
};

/**
 * @brief Flags for a bot or application
 */
enum application_flags : uint32_t {
	/**
	 * @brief Indicates if an app uses the Auto Moderation API
	 */
	apf_application_automod_rule_create_badge = (1 << 6),

	/**
	 * @brief Has gateway presence intent
	 */
	apf_gateway_presence = (1 << 12),

	/**
	 * @brief Has gateway presence intent for <100 guilds
	 */
	apf_gateway_presence_limited = (1 << 13),

	/**
	 * @brief Has guild members intent
	 */
	apf_gateway_guild_members = (1 << 14),

	/**
	 * @brief Has guild members intent for <100 guilds
	 */
	apf_gateway_guild_members_limited = (1 << 15),

	/**
	 * @brief Verification is pending
	 */
	apf_verification_pending_guild_limit = (1 << 16),

	/**
	 * @brief Embedded
	 */
	apf_embedded = (1 << 17),

	/**
	 * @brief Has approval for message content
	 */
	apf_gateway_message_content = (1 << 18),

	/**
	 * @brief Has message content, but <100 guilds
	 */
	apf_gateway_message_content_limited = (1 << 19),

	/**
	 * @brief Indicates if the app has registered global application commands
	 */
	apf_application_command_badge = (1 << 23)
};

/**
 * @brief Represents the settings for the bot/application's in-app authorization link
 */
struct DPP_EXPORT application_install_params {
	/**
	 * @brief A bitmask of dpp::permissions to request for the bot role.
	 */
	permission permissions;

	/**
	 * @brief The scopes as strings to add the application to the server with.
	 *
	 * @see https://discord.com/developers/docs/topics/oauth2#shared-resources-oauth2-scopes
	 */
	std::vector<std::string> scopes;
};

/**
 * @brief Team member role types for application team members.
 * 
 * These are hard coded to string forms by discord. If further types are added,
 * this enum will be extended to support them.
 */
enum team_member_role_t : uint8_t {
	/**
	 * @brief Team owner.
	 */
	tmr_owner,

	/**
	 * @brief Team admin.
	 */
	tmr_admin,

	/**
	 * @brief Developer
	 */
	tmr_developer,

	/**
	 * @brief Read-Only
	 */
	tmr_readonly,
};

/**
 * @brief Represents a team member on a team who maintain a bot/application
 */
class DPP_EXPORT team_member {
public:
	/**
	 * @brief The user's membership state on the team.
	 */
	team_member_status membership_state;

	/**
	 * @brief Will always be "".
	 */
	std::string permissions;

	/**
	 * @brief The id of the parent team of which they are a member.
	 */
	snowflake team_id;

	/**
	 * @brief The avatar, discriminator, id, and username, of the user.
	 */
	user member_user;

	/**
	 * @brief The role of the user in the team.
	 */
	team_member_role_t member_role;
};

/**
 * @brief Represents a team of users who maintain a bot/application
 */
class DPP_EXPORT app_team {
public:
	/**
	 * @brief A hash of the image of the team's icon (may be empty).
	 */
	utility::iconhash icon;

	/**
	 * @brief The id of the team.
	 */
	snowflake id;

	/**
	 * @brief The members of the team.
	 */
	std::vector<team_member> members;

	/**
	 * @brief The name of the team.
	 */
	std::string name;

	/**
	 * @brief The user id of the current team owner.
	 */
	snowflake owner_user_id;
};

/**
 * @brief The application class represents details of a bot application
 */
class DPP_EXPORT application : public managed, public json_interface<application> {
protected:
	friend struct json_interface<application>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	application& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief The name of the app.
	 */
	std::string name;

	/**
	 * @brief The icon hash of the app (may be empty).
	 */
	utility::iconhash icon;

	/**
	 * @brief The description of the app.
	 */
	std::string description;

	/**
	 * @brief Optional: an array of rpc origin urls, if rpc is enabled.
	 */
	std::vector<std::string> rpc_origins;

	/**
	 * @brief When false, only app owner add the bot to guilds.
	 */
	bool bot_public;

	/**
	 * @brief When true, the app's bot will only join upon completion of the full oauth2 code grant flow
	 */
	bool bot_require_code_grant;

	/**
	 * @brief Optional: Partial user object for the bot user associated with the app.
	 */
	user bot;

	/**
	 * @brief Optional: the url of the app's terms of service.
	 */
	std::string terms_of_service_url;

	/**
	 * @brief Optional: the url of the app's privacy policy.
	 */
	std::string privacy_policy_url;

	/**
	 * @brief Optional: partial user object containing info on the owner of the application.
	 */
	user owner;

	/**
	 * @brief If this application is a game sold on Discord, this field will be the summary field for the store page of its primary SKU.
	 *
	 * @deprecated Will be removed in v11
	 */
	std::string summary;

	/**
	 * @brief The hex encoded key for verification in interactions and the GameSDK's GetTicket.
	 */
	std::string verify_key;

	/**
	 * @brief If the application belongs to a team, this will be a list of the members of that team (may be empty).
	 */
	app_team team;

	/**
	 * @brief Optional: if this application is a game sold on Discord, this field will be the guild to which it has been linked.
	 */
	snowflake guild_id;

	/**
	 * @brief Partial object of the associated guild.
	 */
	guild guild_obj;

	/**
	 * @brief Optional: if this application is a game sold on Discord, this field will be the id of the "Game SKU" that is created, if exists.
	 */
	snowflake primary_sku_id;

	/**
	 * @brief Optional: if this application is a game sold on Discord, this field will be the URL slug that links to the store page.
	 */
	std::string slug;

	/**
	 * @brief Optional: the application's default rich presence invite cover image hash
	 */
	utility::iconhash cover_image;

	/**
	 * @brief Optional: the application's public flags.
	 */
	uint32_t flags;

	/**
	 * @brief Optional: Approximate count of guilds the app has been added to.
	 */
	uint64_t approximate_guild_count;

	/**
	 * @brief Optional: Array of redirect URIs for the app.
	 */
	std::vector<std::string> redirect_uris;

	/**
	 * @brief Optional: Interactions endpoint URL for the app.
	 */
	std::string interactions_endpoint_url;

	/**
	 * @brief The application's role connection verification entry point
	 * which, when configured, will render the app as a verification method
	 * in the guild role verification configuration.
	 */
	std::string role_connections_verification_url;

	/**
	 * @brief Up to 5 tags describing the content and functionality of the application.
	 */
	std::vector<std::string> tags;

	/**
	 * @brief Settings for the application's default in-app authorization link, if enabled.
	 */
	application_install_params install_params;

	/**
	 * @brief The application's default custom authorization link, if enabled.
	 */
	std::string custom_install_url;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint8_t	discoverability_state;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint32_t discovery_eligibility_flags;
	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint8_t	explicit_content_filter;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint8_t	creator_monetization_state;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	bool integration_public;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	bool integration_require_code_grant;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	std::vector<std::string> interactions_event_types;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint8_t	interactions_version;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	bool is_monetized;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint32_t monetization_eligibility_flags;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint8_t	monetization_state;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	bool hook;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint8_t	rpc_application_state;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint8_t	store_application_state;

	/**
	 * @warning This variable is not documented by discord, we have no idea what it means and how it works. Use at your own risk.
	 */
	uint8_t	verification_state;

	/** Constructor */
	application();

	/** Destructor */
	~application();

	/**
	 * @brief Get the application's cover image url if they have one, otherwise returns an empty string
	 *
	 * @param size The size of the cover image in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized cover image is returned.
	 * @param format The format to use for the avatar. It can be one of `i_webp`, `i_jpg` or `i_png`.
	 * @return std::string cover image url or an empty string, if required attributes are missing or an invalid format was passed
	 */
	std::string get_cover_image_url(uint16_t size = 0, const image_type format = i_png) const;

	/**
	 * @brief Get the application's icon url if they have one, otherwise returns an empty string
	 *
	 * @param size The size of the icon in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized icon is returned.
	 * @param format The format to use for the avatar. It can be one of `i_webp`, `i_jpg` or `i_png`.
	 * @return std::string icon url or an empty string, if required attributes are missing or an invalid format was passed
	 */
	std::string get_icon_url(uint16_t size = 0, const image_type format = i_png) const;
};

/**
 * @brief A group of applications.
 *
 * This is not currently ever sent by Discord API but the DPP standard setup for
 * objects that can be received by REST has the possibility for this, so this exists.
 * Don't ever expect to see one at present.
 */
typedef std::unordered_map<snowflake, application> application_map;

} // namespace dpp
