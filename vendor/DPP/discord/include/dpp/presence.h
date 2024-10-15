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
#include <dpp/emoji.h>
#include <dpp/json_fwd.h>
#include <unordered_map>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Presence flags bitmask
 */
enum presence_flags {
	/**
	 * @brief Desktop: Online.
	 */
	p_desktop_online = 0b00000001,

	/**
	 * @brief Desktop: DND.
	 */
	p_desktop_dnd = 0b00000010,

	/**
	 * @brief Desktop: Idle.
	 */
	p_desktop_idle = 0b00000011,

	/**
	 * @brief Web: Online.
	 */
	p_web_online = 0b00000100,

	/**
	 * @brief Web: DND.
	 */
	p_web_dnd = 0b00001000,

	/**
	 * @brief Web: Idle.
	 */
	p_web_idle = 0b00001100,

	/**
	 * @brief Mobile: Online.
	 */
	p_mobile_online	= 0b00010000,

	/**
	 * @brief Mobile: DND.
	 */
	p_mobile_dnd = 0b00100000,

	/**
	 * @brief Mobile: Idle.
	 */
	p_mobile_idle = 0b00110000,

	/**
	 * @brief General: Online.
	 */
	p_status_online	= 0b01000000,

	/**
	 * @brief General: DND.
	 */
	p_status_dnd = 0b10000000,

	/**
	 * @brief General: Idle.
	 */
	p_status_idle = 0b11000000
};

/**
 * @brief Online presence status values
 */
enum presence_status : uint8_t {
	/**
	 * @brief Offline.
	 */
	ps_offline = 0,

	/**
	 * @brief Online.
	 */
	ps_online = 1,

	/**
	 * @brief DND.
	 */
	ps_dnd = 2,

	/**
	 * @brief Idle.
	 */
	ps_idle	= 3,

	/**
	 * @brief Invisible (show as offline).
	 */
	ps_invisible = 4,
};

/**
 * @brief Bit shift for desktop status.
 */
#define PF_SHIFT_DESKTOP	0

/**
 * @brief Bit shift for web status.
 */
#define PF_SHIFT_WEB		2

/**
 * @brief Bit shift for mobile status.
 */
#define PF_SHIFT_MOBILE		4

/**
 * @brief Bit shift for main status.
 */
#define PF_SHIFT_MAIN		6

/**
 * @brief Bit mask for status.
 */
#define PF_STATUS_MASK		0b00000011

/**
 * @brief Bit mask for clearing desktop status.
 */
#define PF_CLEAR_DESKTOP	0b11111100

/**
 * @brief Bit mask for clearing web status.
 */
#define PF_CLEAR_WEB		0b11110011

/**
 * @brief Bit mask for clearing mobile status.
 */
#define PF_CLEAR_MOBILE		0b11001111

/**
 * @brief Bit mask for clearing main status.
 */
#define PF_CLEAR_STATUS		0b00111111

/**
 * @brief Game types
 */
enum activity_type : uint8_t {
	/**
	 * @brief "Playing ..."
	 */
	at_game		=	0,

	/**
	 * @brief "Streaming ..."
	 */
	at_streaming	=	1,

	/**
	 * @brief "Listening to..."
	 */
	at_listening	=	2,

	/**
	 * @brief "Watching..."
	 */
	at_watching	=	3,

	/**
	 * @brief "Emoji..."
	 */
	at_custom	=	4,

	/**
	 * @brief "Competing in..."
	 */
	at_competing	=	5
};

/**
 * @brief Activity types for rich presence
 */
enum activity_flags {
	/**
	 * @brief In an instance.
	 */
	af_instance					= 0b000000001,

	/**
	 * @brief Joining.
	 */
	af_join						= 0b000000010,

	/**
	 * @brief Spectating.
	 */
	af_spectate					= 0b000000100,

	/**
	 * @brief Sending join request.
	 */
	af_join_request					= 0b000001000,

	/**
	 * @brief Synchronising.
	 */
	af_sync						= 0b000010000,

	/**
	 * @brief Playing.
	 */
	af_play						= 0b000100000,

	/**
	 * @brief Party privacy friends.
	 */
	af_party_privacy_friends 			= 0b001000000,

	/**
	 * @brief Party privacy voice channel.
	 */
	af_party_privacy_voice_channel			= 0b010000000,

	/**
	 * @brief Embedded.
	 */
	af_embedded 					= 0b100000000
};

/**
 * @brief An activity button is a custom button shown in the rich presence. Can be to join a game or whatever
 */
struct DPP_EXPORT activity_button {
public:
	/**
	 * @brief The text shown on the button (1-32 characters).
	 */
	std::string label;

	/**
	 * @brief The url opened when clicking the button (1-512 characters, can be empty).
	 *
	 * @note Bots cannot access the activity button URLs.
	 */
	std::string url;

	/** Constructor */
	activity_button() = default;
};

/**
 * @brief An activity asset are the images and the hover text displayed in the rich presence
 */
struct DPP_EXPORT activity_assets {
public:
	/**
	 * @brief The large asset image which usually contain snowflake ID or prefixed image ID.
	 */
	std::string large_image;

	/**
	 * @brief Text displayed when hovering over the large image of the activity.
	 */
	std::string large_text;

	/**
	 * @brief The small asset image which usually contain snowflake ID or prefixed image ID.
	 */
	std::string small_image;

	/**
	 * @brief Text displayed when hovering over the small image of the activity.
	 */
	std::string small_text;

	/** Constructor */
	activity_assets() = default;
};

/**
 * @brief Secrets for Rich Presence joining and spectating.
 */
struct DPP_EXPORT activity_secrets {
public:
	/**
	 * @brief The secret for joining a party.
	 */
	std::string join;

	/**
	 * @brief The secret for spectating a game.
	 */
	std::string spectate;

	/**
	 * @brief The secret for a specific instanced match.
	 */
	std::string match;

	/** Constructor */
	activity_secrets() = default;
};

/**
 * @brief Information for the current party of the player
 */
struct DPP_EXPORT activity_party {
public:
	/**
	 * @brief The ID of the party.
	 */
	snowflake id;

	/**
	 * @brief The party's current size.
	 * Used to show the party's current size.
	 */
	int32_t current_size;

	/**
	 * @brief The party's maximum size.
	 * Used to show the party's maximum size.
	 */
	int32_t maximum_size;

	/** Constructor */
	activity_party();
};

/**
 * @brief An activity is a representation of what a user is doing. It might be a game, or a website, or a movie. Whatever.
 */
class DPP_EXPORT activity {
public:
	/**
	 * @brief Name of activity.
	 * e.g. "Fortnite", "Mr Boom's Firework Factory", etc.
	 */
	std::string name;

	/**
	 * @brief State of activity or the custom user status.
	 * e.g. "Waiting in lobby".
	 */
	std::string state;

	/**
	 * @brief What the player is currently doing.
	 */
	std::string details;

	/**
	 * @brief Images for the presence and their hover texts.
	 */
	activity_assets assets;

	/**
	 * @brief URL of activity (this is also named details).
	 *
	 * @note Only applicable for certain sites such a YouTube
	 */
	std::string url;

	/**
	 * @brief The custom buttons shown in the Rich Presence (max 2).
	 */
	std::vector<activity_button> buttons;

	/**
	 * @brief The emoji used for the custom status.
	 */
	dpp::emoji emoji;

	/**
	 * @brief Information of the current party if there is one.
	 */
	activity_party party;

	/**
	 * @brief Secrets for rich presence joining and spectating.
	 */
	activity_secrets secrets;

	/**
	 * @brief Activity type.
	 */
	activity_type type;

	/**
	 * @brief Time activity was created.
	 */
	time_t created_at;

	/**
	 * @brief Start time.
	 * e.g. when game was started.
	 */
	time_t start;

	/**
	 * @brief End time.
	 * e.g. for songs on spotify.
	 */
	time_t end;

	/**
	 * @brief Creating application.
	 * e.g. a linked account on the user's client.
	 */
	snowflake application_id;

	/**
	 * @brief Flags bitmask from dpp::activity_flags.
	 */
	uint8_t flags;

	/**
	 * @brief Whether or not the activity is an instanced game session.
	 */
	bool is_instance;

	/**
	 * @brief Get the assets large image url if they have one, otherwise returns an empty string. In case of prefixed image IDs (mp:{image_id}) it returns an empty string.
	 *
	 * @see https://discord.com/developers/docs/topics/gateway-events#activity-object-activity-asset-image
	 *
	 * @param size The size of the image in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized image is returned.
	 * @param format The format to use for the avatar. It can be one of `i_webp`, `i_jpg` or `i_png`.
	 * @return std::string image url or an empty string, if required attributes are missing or an invalid format was passed
	 */
	std::string get_large_asset_url(uint16_t size = 0, const image_type format = i_png) const;

	/**
	 * @brief Get the assets small image url if they have one, otherwise returns an empty string. In case of prefixed image IDs (mp:{image_id}) it returns an empty string.
	 *
	 * @see https://discord.com/developers/docs/topics/gateway-events#activity-object-activity-asset-image
	 *
	 * @param size The size of the image in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized image is returned.
	 * @param format The format to use for the avatar. It can be one of `i_webp`, `i_jpg` or `i_png`.
	 * @return std::string image url or an empty string, if required attributes are missing or an invalid format was passed
	 */
	std::string get_small_asset_url(uint16_t size = 0, const image_type format = i_png) const;

	activity();

	/**
	 * @brief Construct a new activity
	 * 
	 * @param typ activity type
	 * @param nam Name of the activity
	 * @param stat State of the activity
	 * @param url_ url of the activity, only works for certain sites, such as YouTube
	 */
	activity(const activity_type typ, const std::string& nam, const std::string& stat, const std::string& url_);
};

/**
 * @brief Represents user presence, e.g. what game they are playing and if they are online
 */
class DPP_EXPORT presence : public json_interface<presence> {
protected:
	friend struct json_interface<presence>;

	/** Fill this object from json.
	 * @param j JSON object to fill from
	 * @return A reference to self
	 */
	presence& fill_from_json_impl(nlohmann::json* j);

	/** Build JSON from this object.
	 *
	 * @note This excludes any part of the presence object that are not valid for websockets and bots,
	 * and includes websocket opcode 3. You will not get what you expect if you call this on a user's
	 * presence received from on_presence_update or on_guild_create!
	 *
	 * @param with_id Add ID to output
	 * @return The JSON text of the presence
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief The user the presence applies to.
	 */
	snowflake user_id;

	/**
	 * @brief Guild ID.
	 *
	 * @note Apparently, Discord supports this internally, but the client doesn't...
	 */
	snowflake guild_id;

	/**
	 * @brief Flags bitmask containing dpp::presence_flags
	 */
	uint8_t	flags;

	/**
	 * @brief List of activities.
	 */
	std::vector<activity> activities;

	/** Constructor */
	presence();

	/**
	 * @brief Construct a new presence object with some parameters for sending to a websocket
	 * 
	 * @param status Status of the activity
	 * @param type Type of activity
	 * @param activity_description Description of the activity
	 */
	presence(presence_status status, activity_type type, const std::string& activity_description);

	/**
	 * @brief Construct a new presence object with some parameters for sending to a websocket.
	 * 
	 * @param status Status of the activity
	 * @param a Activity itself 
	 */
	presence(presence_status status, const activity& a);

	/** Destructor */
	~presence();

	/**
	 * @brief The users status on desktop
	 * @return The user's status on desktop
	 */
	presence_status desktop_status() const;

	/**
	 * @brief The user's status on web
	 * @return The user's status on web
	 */
	presence_status web_status() const;

	/**
	 * @brief The user's status on mobile
	 * @return The user's status on mobile
	 */
	presence_status mobile_status() const;

	/**
	 * @brief The user's status as shown to other users
	 * @return The user's status as shown to other users
	 */
	presence_status status() const;

	/**
	 * @brief Build JSON from this object.
	 *
	 * @note This excludes any part of the presence object that are not valid for websockets and bots,
	 * and includes websocket opcode 3. You will not get what you expect if you call this on a user's
	 * presence received from on_presence_update or on_guild_create!
	 *
	 * @param with_id Add ID to output
	 * @return The JSON of the presence
	 */
	json to_json(bool with_id = false) const; // Intentional shadow of json_interface, mostly present for documentation
};

/**
 * @brief A container of presences
 */
typedef std::unordered_map<snowflake, presence> presence_map;

} // namespace dpp
