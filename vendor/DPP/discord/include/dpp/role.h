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
#include <variant>
#include <dpp/export.h>
#include <dpp/managed.h>
#include <dpp/json_fwd.h>
#include <dpp/permissions.h>
#include <dpp/guild.h>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Various flags related to dpp::role
 */
enum role_flags : uint8_t {
	/**
	 * @brief Hoisted role (if the role is pinned in the user listing).
	 */
	r_hoist = 0b00000001,

	/**
	 * @brief Managed role (introduced by a bot or application).
	 */
	r_managed = 0b00000010,

	/**
	 * @brief Whether this role is mentionable with a ping.
	 */
	r_mentionable =	0b00000100,

	/**
	 * @brief Whether this is the guild's booster role.
	 */
	r_premium_subscriber = 0b00001000,

	/**
	 * @brief Whether the role is available for purchase.
	 */
	r_available_for_purchase = 0b00010000,

	/**
	 * @brief Whether the role is a guild's linked role.
	 */
	r_guild_connections = 0b00100000,

	/**
	 * @brief Whether the role can be selected by members in an onboarding prompt.
	 */
	r_in_prompt = 0b01000000,
};

/**
 * @brief Represents a role within a dpp::guild.
 * Roles are combined via logical OR of the permission bitmasks, then channel-specific overrides
 * can be applied on top, deny types apply a logic NOT to the bit mask, and allows apply a logical OR.
 *
 * @note Every guild has at least one role, called the 'everyone' role, which always has the same role
 * ID as the guild's ID. This is the base permission set applied to all users where no other role or override
 * applies, and is the starting value of the bit mask looped through to calculate channel permissions.
 */
class DPP_EXPORT role : public managed, public json_interface<role> {
protected:
	friend struct json_interface<role>;

	/**
	 * @brief Fill this role from json.
	 * 
	 * @param j The json data
	 * @return A reference to self
	 */
	role& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build a json from this object.
	 * 
	 * @param with_id true if the ID is to be included in the json
	 * @return The json of the role
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Role name
	 * Between 1 and 100 characters.
	 */
	std::string name{};

	/**
	 * @brief Guild ID
	 */
	snowflake guild_id{0};

	/**
	 * @brief Role colour.
	 * A colour of 0 means no colour. If you want a black role,
	 * you must use the value 0x000001.
	 */
	uint32_t colour{0};

	/**
	 * @brief Role position.
	 */
	uint8_t position{0};

	/**
	 * @brief Role permissions bitmask values from dpp::permissions.
	 */
	permission permissions{};

	/**
	 * @brief Role flags from dpp::role_flags
	 */
	uint8_t flags{0};

	/**
	 * @brief Integration id if any.
	 * (e.g. role is a bot's role created when it was invited).
	 */
	snowflake integration_id{};

	/**
	 * @brief Bot id if any.
	 * (e.g. role is a bot's role created when it was invited)
	 */
	snowflake bot_id{};

	/**
	 * @brief The id of the role's subscription sku and listing.
	 */
	snowflake subscription_listing_id{};

	/**
	 * @brief The unicode emoji used for the role's icon.
	 *
	 * @note This can be an empty string.
	 */
	std::string unicode_emoji{};

	/**
	 * @brief The role icon.
	 */
	utility::icon icon{};

	/**
	 * @brief Construct a new role object
	 */
	role() = default;

	/**
	 * @brief Construct a new role object.
	 *
	 * @param rhs Role object to copy
	 */
	role(const role& rhs) = default;

	/**
	 * @brief Construct a new role object.
	 *
	 * @param rhs Role object to move
	 */
	role(role&& rhs) = default;

	/**
	 * @brief Copy another role object
	 *
	 * @param rhs Role object to copy
	 */
	role &operator=(const role& rhs) = default;

	/**
	 * @brief Move from another role object
	 *
	 * @param rhs Role object to copy
	 */
	role &operator=(role&& rhs) = default;

	/**
	 * @brief Destroy the role object
	 */
	virtual ~role() = default;

	/**
	* @brief Create a mentionable role.
	* @param id The ID of the role.
	* @return std::string The formatted mention of the role.
	*/
	static std::string get_mention(const snowflake& id);

	/**
	 * @brief Set the name of the role.
	 * Maximum length: 100
	 * Minimum length: 1
	 * @param n Name to set
	 * @return role& reference to self
	 * @throw dpp::exception thrown if role length is less than 1 character
	 */
	role& set_name(const std::string& n);

	/**
	 * @brief Set the colour.
	 * 
	 * @param c Colour to set
	 * @note There is an americanised version of this method, role::set_color().
	 * @return role& reference to self
	 */
	role& set_colour(uint32_t c);

	/**
	 * @brief Set the color.
	 * 
	 * @param c Colour to set
	 * @note This is an alias of role::set_colour for American spelling.
	 * @return role& reference to self
	 */
	role& set_color(uint32_t c);

	/**
	 * @brief Set the flags.
	 * 
	 * @param f Flags to set from dpp::role_flags
	 * @return role& reference to self
	 */
	role& set_flags(uint8_t f);

	/**
	 * @brief Set the integration ID.
	 * 
	 * @param i Integration ID to set
	 * @return role& reference to self
	 */
	role& set_integration_id(snowflake i);

	/**
	 * @brief Set the bot ID.
	 * 
	 * @param b Bot ID to set
	 * @return role& reference to self
	 */
	role& set_bot_id(snowflake b);

	/**
	 * @brief Set the guild ID.
	 * 
	 * @param gid Guild ID to set
	 * @return role& reference to self
	 */
	role& set_guild_id(snowflake gid);

	using json_interface<role>::fill_from_json;

	/**
	 * @brief Fill this role from json.
	 * 
	 * @param guild_id the guild id to place in the json
	 * @param j The json data
	 * @return A reference to self
	 */
	role& fill_from_json(snowflake guild_id, nlohmann::json* j);

	/**
	 * @brief Get the mention/ping for the role.
	 * 
	 * @return std::string mention
	 */
	std::string get_mention() const;

	/**
	 * @brief Returns the role's icon url if they have one, otherwise returns an empty string.
	 *
	 * @param size The size of the icon in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized icon is returned.
	 * @param format The format to use for the avatar. It can be one of `i_webp`, `i_jpg` or `i_png`.
	 * @return std::string icon url or an empty string, if required attributes are missing or an invalid format was passed
	 */
	std::string get_icon_url(uint16_t size = 0, const image_type format = i_png) const;

	/**
	 * @brief Load a role icon.
	 *
	 * @param image_blob Image binary data
	 * @param type Type of image. It can be one of `i_gif`, `i_jpg` or `i_png`.
	 * @return emoji& Reference to self
	 */
	role& load_image(std::string_view image_blob, const image_type type);

	/**
	 * @brief Load a role icon.
	 *
	 * @param data Image binary data
	 * @param size Size of the image.
	 * @param type Type of image. It can be one of `i_gif`, `i_jpg` or `i_png`.
	 * @return emoji& Reference to self
	 */
	role& load_image(const std::byte* data, uint32_t size, const image_type type);

	/**
	 * @brief Operator less than, used for checking if a role is below another.
	 *
	 * @param lhs first role to compare
	 * @param rhs second role to compare
	 * @return true if lhs is less than rhs
	 */
	friend inline bool operator< (const role& lhs, const role& rhs) {
		return lhs.position < rhs.position;
	}

	/**
	 * @brief Operator greater than, used for checking if a role is above another.
	 *
	 * @param lhs first role to compare
	 * @param rhs second role to compare
	 * @return true if lhs is greater than rhs
	 */
	friend inline bool operator> (const role& lhs, const role& rhs) {
		return lhs.position > rhs.position;
	}

	/**
	 * @brief Operator equals, used for checking if a role is ranked equal to another.
	 *
	 * @param other role to compare
	 * @return true if is equal to other
	 */
	inline bool operator== (const role& other) const {
		return this->position == other.position;
	}

	/**
	 * @brief Operator not equals, used for checking if a role is ranked equal to another.
	 *
	 * @param other role to compare
	 * @return true if is not equal to other
	 */
	inline bool operator!= (const role& other) const {
		return this->position != other.position;
	}

	/**
	 * @brief True if the role is hoisted.
	 *
	 * @return bool Role appears separated from others in the member list
	 */
	bool is_hoisted() const;

	/**
	 * @brief True if the role is mentionable.
	 *
	 * @return bool Role is mentionable
	 */
	bool is_mentionable() const;

	/**
	 * @brief True if the role is managed (belongs to a bot or application).
	 *
	 * @return bool True if the role is managed (introduced for a bot or other application by Discord)
	 */
	bool is_managed() const;

	/**
	 * @brief True if the role is the guild's Booster role.
	 *
	 * @return bool whether the role is the premium subscriber, AKA "boost", role for the guild
	 */
	bool is_premium_subscriber() const;

	/**
	 * @brief True if the role is available for purchase.
	 *
	 * @return bool whether this role is available for purchase
	 */
	bool is_available_for_purchase() const;

	/**
	 * @brief True if the role is a linked role.
	 *
	 * @return bool True if the role is a linked role
	 */
	bool is_linked() const;

	/**
	 * @brief True if the role can be selected by members in an onboarding prompt.
	 *
	 * @return bool True if the role can be selected by members in an onboarding prompt
	 */
	bool is_selectable_in_prompt() const;

	/**
	 * @brief True if has create instant invite permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the instant invite permission or is administrator.
	 */
	bool has_create_instant_invite() const;

	/**
	 * @brief True if has the kick members permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the kick members permission or is administrator.
	 */
	bool has_kick_members() const;

	/**
	 * @brief True if has the ban members permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the ban members permission or is administrator.
	 */
	bool has_ban_members() const;

	/**
	 * @brief True if has the administrator permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the administrator permission or is administrator.
	 */
	bool has_administrator() const;

	/**
	 * @brief True if has the manage channels permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage channels permission or is administrator.
	 */
	bool has_manage_channels() const;

	/**
	 * @brief True if has the manage guild permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage guild permission or is administrator.
	 */
	bool has_manage_guild() const;

	/**
	 * @brief True if has the add reactions permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the add reactions permission or is administrator.
	 */
	bool has_add_reactions() const;

	/**
	 * @brief True if has the view audit log permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the view audit log permission or is administrator.
	 */
	bool has_view_audit_log() const;

	/**
	 * @brief True if has the priority speaker permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the priority speaker permission or is administrator.
	 */
	bool has_priority_speaker() const;

	/**
	 * @brief True if has the stream permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the stream permission or is administrator.
	 */
	bool has_stream() const;

	/**
	 * @brief True if has the view channel permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the view channel permission or is administrator.
	 */
	bool has_view_channel() const;

	/**
	 * @brief True if has the send messages permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the send messages permission or is administrator.
	 */
	bool has_send_messages() const;

	/**
	 * @brief True if has the send TTS messages permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the send TTS messages permission or is administrator.
	 */
	bool has_send_tts_messages() const;

	/**
	 * @brief True if has the manage messages permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage messages permission or is administrator.
	 */
	bool has_manage_messages() const;

	/**
	 * @brief True if has the embed links permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the embed links permission or is administrator.
	 */
	bool has_embed_links() const;

	/**
	 * @brief True if has the attach files permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the attach files permission or is administrator.
	 */
	bool has_attach_files() const;

	/**
	 * @brief True if has the read message history permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the read message history permission or is administrator.
	 */
	bool has_read_message_history() const;

	/**
	 * @brief True if has the mention \@everyone and \@here permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the mention \@everyone and \@here permission or is administrator.
	 */
	bool has_mention_everyone() const;

	/**
	 * @brief True if has the use external emojis permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the use external emojis permission or is administrator.
	 */
	bool has_use_external_emojis() const;

	/**
	 * @brief True if has the view guild insights permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the view guild insights permission or is administrator.
	 */
	bool has_view_guild_insights() const;

	/**
	 * @brief True if has the connect voice permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the connect voice permission or is administrator.
	 */
	bool has_connect() const;

	/**
	 * @brief True if has the speak permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the speak permission or is administrator.
	 */
	bool has_speak() const;

	/**
	 * @brief True if has the mute members permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the mute members permission or is administrator.
	 */
	bool has_mute_members() const;

	/**
	 * @brief True if has the deafen members permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the deafen members permission or is administrator.
	 */
	bool has_deafen_members() const;

	/**
	 * @brief True if has the move members permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the move members permission or is administrator.
	 */
	bool has_move_members() const;

	/**
	 * @brief True if has use voice activity detection permission
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has use voice activity detection permission or is administrator.
	 */
	bool has_use_vad() const;

	/**
	 * @brief True if has the change nickname permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the change nickname permission or is administrator.
	 */
	bool has_change_nickname() const;

	/**
	 * @brief True if has the manage nicknames permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage nicknames permission or is administrator.
	 */
	bool has_manage_nicknames() const;

	/**
	 * @brief True if has the manage roles permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage roles permission or is administrator.
	 */
	bool has_manage_roles() const;

	/**
	 * @brief True if has the manage webhooks permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage webhooks permission or is administrator.
	 */
	bool has_manage_webhooks() const;

	/**
	 * @brief True if has the manage emojis and stickers permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage emojis and stickers permission or is administrator.
	 */
	bool has_manage_emojis_and_stickers() const;

	/**
	 * @brief True if has the use application commands permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the use application commands permission or is administrator.
	 */
	bool has_use_application_commands() const;

	/**
	 * @brief True if has the request to speak permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the request to speak permission or is administrator.
	 */
	bool has_request_to_speak() const;

	/**
	 * @brief True if has the manage threads permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage threads permission or is administrator.
	 */
	bool has_manage_threads() const;

	/**
	 * @brief True if has the create public threads permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the create public threads permission or is administrator.
	 */
	bool has_create_public_threads() const;

	/**
	 * @brief True if has the create private threads permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the create private threads permission or is administrator.
	 */
	bool has_create_private_threads() const;

	/**
	 * @brief True if has the use external stickers permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the use external stickers permission or is administrator.
	 */

	bool has_use_external_stickers() const;
	/**
	 * @brief True if has the send messages in threads permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the send messages in threads permission or is administrator.
	 */
	bool has_send_messages_in_threads() const;

	/**
	 * @brief True if has the start embedded activities permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the start embedded activities permission or is administrator.
	 */
	bool has_use_embedded_activities() const;

	/**
	 * @brief True if has the manage events permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the manage events permission or is administrator.
	 */
	bool has_manage_events() const;

	/**
	 * @brief True if has the moderate users permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the moderate users permission or is administrator.
	 */
	bool has_moderate_members() const;

	/**
	 * @brief True if has the view creator monetization analytics permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the view creator monetization analytics permission or is administrator.
	 */
	bool has_view_creator_monetization_analytics() const;

	/**
	 * @brief True if has the use soundboard permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the use soundboard permission or is administrator.
	 */
	bool has_use_soundboard() const;

	/**
	 * @brief True if has the use external sounds permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the use external sounds permission or is administrator.
	 */
	bool has_use_external_sounds() const;

	/**
	 * @brief True if has the send voice messages permission.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the send voice messages permission or is administrator.
	 */
	bool has_send_voice_messages() const;

	/**
	 * @brief True if has permission to use clyde AI.
	 *
	 * @note Having the administrator permission causes this method to always return true
	 * Channel specific overrides may apply to permissions.
	 * @return bool True if user has the clyde AI permission or is administrator.
	 */
	bool has_use_clyde_ai() const;

	/**
	 * @brief Get guild members who have this role.
	 *
	 * @note This method requires user/members cache to be active
	 * @return members_container List of members who have this role
	 */
	members_container get_members() const;
};

/**
 * @brief Application Role Connection Metadata Type
 *
 * @note Each metadata type offers a comparison operation that allows guilds
 * to configure role requirements based on metadata values stored by the bot.
 * Bots specify a `metadata value` for each user and guilds specify the
 * required `guild's configured value` within the guild role settings.
 */
enum application_role_connection_metadata_type : uint8_t {
	/**
	 * @brief The metadata value (integer) is less than or equal to the guild's configured value (integer).
	 */
	rc_integer_less_than_or_equal = 1,

	/**
	 * @brief The metadata value (integer) is greater than or equal to the guild's configured value (integer).
	 */
	rc_integer_greater_than_or_equal = 2,

	/**
	 * @brief The metadata value (integer) is equal to the guild's configured value (integer).
	 */
	rc_integer_equal = 3,

	/**
	 * @brief The metadata value (integer) is not equal to the guild's configured value (integer).
	 */
	rc_integer_not_equal = 4,

	/**
	 * @brief The metadata value (ISO8601 string) is less than or equal to the guild's configured value (integer; days before current date).
	 */
	rc_datetime_less_than_or_equal = 5,

	/**
	 * @brief The metadata value (ISO8601 string) is greater than or equal to the guild's configured value (integer; days before current date).
	 */
	rc_datetime_greater_than_or_equal = 6,

	/**
	 * @brief The metadata value (integer) is equal to the guild's configured value (integer; 1).
	 */
	rc_boolean_equal = 7,

	/**
	 * @brief The metadata value (integer) is not equal to the guild's configured value (integer; 1).
	 */
	rc_boolean_not_equal = 8,
};

/**
 * @brief Application Role Connection Metadata. Represents a role connection metadata for an dpp::application
 */
class DPP_EXPORT application_role_connection_metadata : public json_interface<application_role_connection_metadata> {
protected:
	friend struct json_interface<application_role_connection_metadata>;

	/** Fill this record from json.
	 * @param j The json to fill this record from
	 * @return Reference to self
	 */
	application_role_connection_metadata& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Convert to JSON
	 *
	 * @param with_id include ID in output
	 * @return json JSON output
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Type of metadata value.
	 */
	application_role_connection_metadata_type type;

	/**
	 * @brief Dictionary key for the metadata field (must be `a-z`, `0-9`, or `_` characters; 1-50 characters).
	 */
	std::string key;

	/**
	 * @brief Name of the metadata field (1-100 characters).
	 */
	std::string name;

	/**
	 * @brief Translations of the name.
	 */
	std::map<std::string, std::string> name_localizations;

	/**
	 * @brief Description of the metadata field (1-200 characters).
	 */
	std::string description;

	/**
	 * @brief Translations of the description.
	 */
	std::map<std::string, std::string> description_localizations;

	/**
	 * @brief Constructor
	 */
	application_role_connection_metadata();

	virtual ~application_role_connection_metadata() = default;
};

/**
 * @brief The application role connection that an application has attached to a user.
 */
class DPP_EXPORT application_role_connection : public json_interface<application_role_connection> {
protected:
	friend struct json_interface<application_role_connection>;

	/**
	 * @brief Fill this record from json.
	 * @param j The json to fill this record from
	 * @return Reference to self
	 */
	application_role_connection& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Convert to JSON
	 *
	 * @param with_id include ID in output
	 * @return json JSON output
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Optional: The vanity name of the platform a bot has connected (max 50 characters).
	 */
	std::string platform_name;

	/**
	 * @brief Optional: The username on the platform a bot has connected (max 100 characters).
	 */
	std::string platform_username;

	/**
	 * @brief Optional: Object mapping application role connection metadata keys to their "stringified" value (max 100 characters) for the user on the platform a bot has connected.
	 */
	std::variant<std::monostate, application_role_connection_metadata> metadata;

	/**
	 * @brief Constructor
	 */
	application_role_connection();

	virtual ~application_role_connection() = default;
};

/**
 * @brief A group of roles.
 */
typedef std::unordered_map<snowflake, role> role_map;

/**
 * @brief A group of dpp::application_role_connection_metadata objects.
 */
typedef std::vector<application_role_connection_metadata> application_role_connection_metadata_list;

} // namespace dpp

