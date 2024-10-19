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
#include <dpp/json_fwd.h>
#include <dpp/snowflake.h>
#include <dpp/managed.h>
#include <dpp/utility.h>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Various bitmask flags used to represent information about a dpp::user
 */
enum user_flags : uint32_t {
	/**
	 * @brief User is a bot.
	 */
	u_bot =	0b00000000000000000000000000000001,

	/**
	 * @brief User is a system user (Clyde!).
	 */
	u_system = 0b00000000000000000000000000000010,

	/**
	 * @brief User has multi-factor authentication enabled.
	 */
	u_mfa_enabled = 0b00000000000000000000000000000100,

	/**
	 * @brief User is verified (verified email address).
	 */
	u_verified = 0b00000000000000000000000000001000,

	/**
	 * @brief User has full nitro.
	 */
	u_nitro_full = 0b00000000000000000000000000010000,

	/**
	 * @brief User has nitro classic.
	 */
	u_nitro_classic = 0b00000000000000000000000000100000,

	/**
	 * @brief User is discord staff.
	 */
	u_discord_employee = 0b00000000000000000000000001000000,

	/**
	 * @brief User owns a partnered server.
	 */
	u_partnered_owner = 0b00000000000000000000000010000000,

	/**
	 * @brief User is a member of hypesquad events.
	 */
	u_hypesquad_events = 0b00000000000000000000000100000000,

	/**
	 * @brief User has BugHunter level 1.
	 */
	u_bughunter_1 =	0b00000000000000000000001000000000,

	/**
	 * @brief User is a member of House Bravery.
	 */
	u_house_bravery = 0b00000000000000000000010000000000,

	/**
	 * @brief User is a member of House Brilliance.
	 */
	u_house_brilliance = 0b00000000000000000000100000000000,

	/**
	 * @brief User is a member of House Balance.
	 */
	u_house_balance = 0b00000000000000000001000000000000,

	/**
	 * @brief User is an early supporter.
	 */
	u_early_supporter = 0b00000000000000000010000000000000,

	/**
	 * @brief User is a team user.
	 */
	u_team_user = 0b00000000000000000100000000000000,

	/**
	 * @brief User is has Bug Hunter level 2.
	 */
	u_bughunter_2 =	0b00000000000000001000000000000000,

	/**
	 * @brief User is a verified bot.
	 */
	u_verified_bot = 0b00000000000000010000000000000000,

	/**
	 * @brief User has the Early Verified Bot Developer badge.
	 */
	u_verified_bot_dev = 0b00000000000000100000000000000000,

	/**
	 * @brief User's icon is animated.
	 */
	u_animated_icon = 0b00000000000001000000000000000000,

	/**
	 * @brief User is a certified moderator.
	 */
	u_certified_moderator =	0b00000000000010000000000000000000,

	/**
	 * @brief User is a bot using HTTP interactions.
	 *
	 * @note shows online even when not connected to a websocket.
	 */
	u_bot_http_interactions = 0b00000000000100000000000000000000,

	/**
	 * @brief User has nitro basic.
	 */
	u_nitro_basic = 0b00000000001000000000000000000000,

	/**
	 * @brief User has the active developer badge.
	 */
	u_active_developer = 0b00000000010000000000000000000000,

	/**
	 * @brief User's banner is animated.
	 */
	u_animated_banner =			0b00000000100000000000000000000000,
};

/**
 * @brief Represents a user on discord. May or may not be a member of a dpp::guild.
 */
class DPP_EXPORT user : public managed, public json_interface<user> {
protected:
	friend struct json_interface<user>;

	/** Fill this record from json.
	 * @param j The json to fill this record from
	 * @return Reference to self
	 */
	user& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Convert to JSON
	 *
	 * @param with_id include ID in output
	 * @return json JSON output
	 */
	virtual json to_json_impl(bool with_id = true) const;

public:
	/**
	 * @brief Discord username.
	 */
	std::string username;

	/**
	 * @brief Global display name.
	 */
	std::string global_name;

	/**
	 * @brief Avatar hash.
	 */
	utility::iconhash avatar;

	/**
	 * @brief Avatar decoration hash.
	 */
	utility::iconhash avatar_decoration;

	/**
	 * @brief Flags built from a bitmask of values in dpp::user_flags.
	 */
	uint32_t flags;

	/**
	 * @brief Discriminator (aka tag), 4 digits usually displayed with leading zeroes.
	 *
	 * @note To print the discriminator with leading zeroes, use format_username().
	 * 0 for users that have migrated to the new username format.
	 */
	uint16_t discriminator;

	/**
	 * @brief Reference count of how many guilds this user is in.
	 */
	uint8_t refcount;

	/**
	 * @brief Construct a new user object
	 */
	user();

	/**
	 * @brief Destroy the user object
	 */
	virtual ~user() = default;

	/**
	 * @brief Create a mentionable user.
	 * @param id The ID of the user.
	 * @return std::string The formatted mention of the user.
	 */
	static std::string get_mention(const snowflake& id);

	/**
	 * @brief Get the avatar url of the user
	 *
	 * @note If the user doesn't have an avatar, the default user avatar url is returned which is always in `png` format!
	 *
	 * @param size The size of the avatar in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized avatar is returned.
	 * @param format The format to use for the avatar. It can be one of `i_webp`, `i_jpg`, `i_png` or `i_gif`.
	 * When passing `i_gif`, it returns an empty string for non-animated images. Consider using the `prefer_animated` parameter instead.
	 * @param prefer_animated Whether you prefer gif format.
	 * If true, it'll return gif format whenever the image is available as animated.
	 * @return std::string avatar url or an empty string, if required attributes are missing or an invalid format was passed
	 */
	std::string get_avatar_url(uint16_t size = 0, const image_type format = i_png, bool prefer_animated = true) const;

	/**
	 * @brief Get the default avatar url of the user. This is calculated by the discriminator.
	 *
	 * @return std::string avatar url or an empty string, if the discriminator is empty
	 */
	std::string get_default_avatar_url() const;

	/**
	 * @brief Get the avatar decoration url of the user if they have one, otherwise returns an empty string.
	 *
	 * @param size The size of the avatar decoration in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized avatar decoration is returned.
	 * @return std::string avatar url or an empty string
	 */
	std::string get_avatar_decoration_url(uint16_t size = 0) const;

	/**
	 * @brief Return a ping/mention for the user
	 * 
	 * @return std::string mention
	 */
	std::string get_mention() const;

	/**
	 * @brief Returns URL to user 
	 * 
	 * @return string of URL to user
	 */
	std::string get_url() const;

	/**
	 * @brief Return true if user has the active Developer badge
	 *
	 * @return true if has active developer
	 */
	bool is_active_developer() const;

	/**
	 * @brief User is a bot
	 *
	 * @return True if the user is a bot
	 */
	bool is_bot() const;

	/**
	 * @brief User is a system user (Clyde)
	 *
	 * @return true  if user is a system user
	 */
	bool is_system() const;

	/**
	 * @brief User has multi-factor authentication enabled
	 *
	 * @return true if multi-factor is enabled
	 */
	bool is_mfa_enabled() const;

	/**
	 * @brief Return true if user has verified account
	 *
	 * @return true if verified
	 */
	bool is_verified() const;

	/**
	 * @brief Return true if user has full nitro.
	 * This is mutually exclusive with full nitro.
	 *
	 * @return true if user has full nitro
	 */
	bool has_nitro_full() const;

	/**
	 * @brief Return true if user has nitro classic.
	 * This is mutually exclusive with nitro classic.
	 *
	 * @return true if user has nitro classic
	 */
	bool has_nitro_classic() const;

	/**
	 * @brief Return true if user has nitro basic.
	 * This is mutually exclusive with nitro basic.
	 *
	 * @return true if user has nitro basic
	 */
	bool has_nitro_basic() const;

	/**
	 * @brief Return true if user is a discord employee
	 *
	 * @return true if user is discord staff
	 */
	bool is_discord_employee() const;

	/**
	 * @brief Return true if user owns a partnered server
	 *
	 * @return true if user has partnered server
	 */
	bool is_partnered_owner() const;

	/**
	 * @brief Return true if user has hypesquad events
	 *
	 * @return true if has hypesquad events
	 */
	bool has_hypesquad_events() const;

	/**
	 * @brief Return true if user has the bughunter level 1 badge
	 *
	 * @return true if has bughunter level 1
	 */
	bool is_bughunter_1() const;

	/**
	 * @brief Return true if user is in house bravery
	 *
	 * @return true if in house bravery
	 */
	bool is_house_bravery() const;

	/**
	 * @brief Return true if user is in house brilliance
	 *
	 * @return true if in house brilliance
	 */
	bool is_house_brilliance() const;

	/**
	 * @brief Return true if user is in house balance
	 *
	 * @return true if in house brilliance
	 */
	bool is_house_balance() const;

	/**
	 * @brief Return true if user is an early supporter
	 *
	 * @return true if early supporter
	 */
	bool is_early_supporter() const;

	/**
	 * @brief Return true if user is a team user
	 *
	 * @return true if a team user
	 */
	bool is_team_user() const;

	/**
	 * @brief Return true if user has the bughunter level 2 badge
	 *
	 * @return true if has bughunter level 2
	 */
	bool is_bughunter_2() const;

	/**
	 * @brief Return true if user has the verified bot badge
	 *
	 * @return true if verified bot
	 */
	bool is_verified_bot() const;

	/**
	 * @brief Return true if user is an early verified bot developer
	 *
	 * @return true if verified bot developer
	 */
	bool is_verified_bot_dev() const;

	/**
	 * @brief Return true if user is a certified moderator
	 *
	 * @return true if certified moderator
	 */
	bool is_certified_moderator() const;

	/**
	 * @brief Return true if user is a bot which exclusively uses HTTP interactions.
	 * Bots using HTTP interactions are always considered online even when not connected to a websocket.
	 *
	 * @return true if is a http interactions only bot
	 */
	bool is_bot_http_interactions() const;

	/**
	 * @brief Return true if user has an animated icon
	 *
	 * @return true if icon is animated (gif)
	 */
	bool has_animated_icon() const;

	/**
	 * @brief Format a username into user\#discriminator
	 * 
	 * For example Brain#0001
	 *
	 * @note This will, most often, return something like Brain#0000 due to discriminators slowly being removed.
	 * Some accounts, along with most bots, still have discriminators, so they will still show as Bot#1234.
	 * 
	 * @return Formatted username and discriminator
	 */
	std::string format_username() const;
};

/**
 * @brief A user with additional fields only available via the oauth2 identify scope.
 * These are not included in dpp::user as additional scopes are needed to fetch them
 * which bots do not normally have.
 */
class DPP_EXPORT user_identified : public user, public json_interface<user_identified> {
protected:
	friend struct json_interface<user_identified>;

	/** Fill this record from json.
	 * @param j The json to fill this record from
	 * @return Reference to self
	 */
	user_identified& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Convert to JSON
	 *
	 * @param with_id include ID in output
	 * @return json JSON output
	 */
	virtual json to_json_impl(bool with_id = true) const;

public:
	/**
	 * @brief Optional: The user's chosen language option identify.
	 */
	std::string locale;

	/**
	 * @brief Optional: The user's email.
	 *
	 * @note This may be empty.
	 */
	std::string email;

	/**
	 * @brief Optional: The user's banner hash identify.
	 *
	 * @note This may be empty.
	 */
	utility::iconhash banner;

	/**
	 * @brief Optional: The user's banner color encoded as an integer representation of hexadecimal color code identify.
	 *
	 * @note This may be empty.
	 */
	uint32_t accent_color;


	/**
	 * @brief Optional: Whether the email on this account has been verified email.
	 */
	bool verified;

	/**
	 * @brief Construct a new user identified object
	 */
	user_identified();

	/**
	 * @brief Construct a new user identified object from a user object
	 * 
	 * @param u user object
	 */
	user_identified(const user& u);

	/**
	 * @brief Destroy the user identified object
	 */
	virtual ~user_identified() = default;

	using json_interface<user_identified>::fill_from_json;
	using json_interface<user_identified>::build_json;
	using json_interface<user_identified>::to_json;

	/**
	 * @brief Return true if user has an animated banner
	 *
	 * @return true if banner is animated (gif)
	 */
	bool has_animated_banner() const;

	/**
	 * @brief Get the user identified's banner url if they have one, otherwise returns an empty string
	 *
	 * @param size The size of the banner in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized banner is returned.
	 * @param format The format to use for the avatar. It can be one of `i_webp`, `i_jpg`, `i_png` or `i_gif`.
	 * When passing `i_gif`, it returns an empty string for non-animated images. Consider using the `prefer_animated` parameter instead.
	 * @param prefer_animated Whether you prefer gif format.
	 * If true, it'll return gif format whenever the image is available as animated.
	 * @return std::string banner url or an empty string, if required attributes are missing or an invalid format was passed
	 */
	std::string get_banner_url(uint16_t size = 0, const image_type format = i_png, bool prefer_animated = true) const;

};

/**
 * @brief helper function to deserialize a user from json
 *
 * @see https://github.com/nlohmann/json#arbitrary-types-conversions
 *
 * @param j output json object
 * @param u user to be deserialized
 */
void from_json(const nlohmann::json& j, user& u);

/**
 * @brief helper function to deserialize a user_identified from json
 *
 * @see https://github.com/nlohmann/json#arbitrary-types-conversions
 *
 * @param j output json object
 * @param u user to be deserialized
 */
void from_json(const nlohmann::json& j, user_identified& u);

/**
 * @brief A group of users.
 */
typedef std::unordered_map<snowflake, user> user_map;

} // namespace dpp
