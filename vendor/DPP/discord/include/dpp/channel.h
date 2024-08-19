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
#include <dpp/voicestate.h>
#include <dpp/json_fwd.h>
#include <dpp/permissions.h>
#include <dpp/json_interface.h>
#include <unordered_map>
#include <variant>

namespace dpp {

/** @brief Flag integers as received from and sent to discord */
enum channel_type : uint8_t {
	/**
	 * @brief A text channel within a server.
	 */
	CHANNEL_TEXT = 0,

	/**
	 * @brief A direct message between users.
	 */
	DM = 1,

	/**
	 * @brief A voice channel within a server.
	 */
	CHANNEL_VOICE = 2,

	/**
	 * @brief a direct message between multiple users
	 * @deprecated this channel type was intended to be used with the now deprecated GameBridge SDK.
	 * Existing group dms with bots will continue to function, but newly created channels will be unusable.
	 */
	GROUP_DM = 3,

	/**
	 * @brief An organizational category that contains up to 50 channels.
	 */
	CHANNEL_CATEGORY = 4,

	/**
	 * @brief A channel that users can follow and cross-post into their own server.
	 */
	CHANNEL_ANNOUNCEMENT = 5,

	/**
	 * @brief A channel in which game developers can sell their game on Discord.
	 * @deprecated Store channels are deprecated by Discord.
	 */
	CHANNEL_STORE = 6,

	/**
	 * @brief A temporary sub-channel within a `GUILD_ANNOUNCEMENT` channel.
	 */
	CHANNEL_ANNOUNCEMENT_THREAD = 10,

	/**
	 * @brief A temporary sub-channel within a `GUILD_TEXT` or `GUILD_FORUM` channel.
	 */
	CHANNEL_PUBLIC_THREAD = 11,

	/**
	 * @brief A temporary sub-channel within a `GUILD_TEXT` channel
	 * that is only viewable by those invited and those with the `MANAGE_THREADS` permission.
	 */
	CHANNEL_PRIVATE_THREAD = 12,

	/**
	 * @brief A "stage" channel, like a voice channel with one authorised speaker.
	 */
	CHANNEL_STAGE = 13,

	/**
	 * @brief The channel in a hub containing the listed servers.
	 *
	 * @see https://support.discord.com/hc/en-us/articles/4406046651927-Discord-Student-Hubs-FAQ
	 */
	CHANNEL_DIRECTORY = 14,

	/**
	 * @brief Forum channel that can only contain threads.
	 */
	CHANNEL_FORUM = 15,

	/**
	 * @brief Media channel that can only contain threads, similar to forum channels.
	 */
	CHANNEL_MEDIA = 16,
};

/** @brief Our flags as stored in the object
 * @note The bottom four bits of this flag are reserved to contain the channel_type values
 * listed above as provided by Discord. If discord add another value > 15, we will have to
 * shuffle these values upwards by one bit.
 */
enum channel_flags : uint16_t {
	/* Note that bits 1 to 4 are used for the channel type mask */
	/**
	 * @brief NSFW Gated Channel
	 */
	c_nsfw =		0b0000000000010000,

	/**
	 * @brief Video quality forced to 720p
	 */
	c_video_quality_720p =	0b0000000000100000,

	/**
	 * @brief Lock permissions (only used when updating channel positions)
	 */
	c_lock_permissions =	0b0000000001000000,

	/**
	 * @brief Thread is pinned to the top of its parent forum or media channel
	 */
	c_pinned_thread =	0b0000000010000000,

	/**
	 * @brief Whether a tag is required to be specified when creating a thread in a forum or media channel.
	 * Tags are specified in the thread::applied_tags field.
	 */
	c_require_tag =		0b0000000100000000,

	/* Note that the 9th and 10th bit are used for the forum layout type. */
	/**
	 * @brief When set hides the embedded media download options. Available only for media channels
	 */
	c_hide_media_download_options = 0b0001000000000000,
};

/**
 * @brief Types for sort posts in a forum channel
 */
enum default_forum_sort_order_t : uint8_t {
	/**
	 * @brief Sort forum posts by activity (default)
	 */
	so_latest_activity = 0,

	/**
	 * @brief Sort forum posts by creation time (from most recent to oldest)
	 */
	so_creation_date = 1,
};

/**
 * @brief Types of forum layout views that indicates how the threads in a forum channel will be displayed for users by default
 */
enum forum_layout_type : uint8_t {
	/**
	 * @brief No default has been set for the forum channel
	 */
	fl_not_set = 0,

	/**
	 * @brief Display posts as a list
	 */
	fl_list_view = 1,

	/**
	 * @brief Display posts as a collection of tiles
	 */
	fl_gallery_view = 2,
};

/**
 * @brief channel permission overwrite types
 */
enum overwrite_type : uint8_t {
	/**
	 * @brief Role
	 */
	ot_role = 0,

	/**
	 * @brief Member
	 */
	ot_member = 1
};

/**
 * @brief Channel permission overwrites
 */
struct DPP_EXPORT permission_overwrite {
	/**
	 * @brief ID of the role or the member
	 */
	snowflake id;

	/**
	 * @brief Bitmask of allowed permissions
	 */
	permission allow;

	/**
	 * @brief Bitmask of denied permissions
	 */
	permission deny;

	/**
	 * @brief Type of overwrite. See dpp::overwrite_type
	 */
	uint8_t type;

	/**
	 * @brief Construct a new permission_overwrite object
	 */
	permission_overwrite();

	/**
	 * @brief Construct a new permission_overwrite object
	 * @param id ID of the role or the member to create the overwrite for
	 * @param allow Bitmask of allowed permissions (refer to enum dpp::permissions) for this user/role in this channel
	 * @param deny Bitmask of denied permissions (refer to enum dpp::permissions) for this user/role in this channel
	 * @param type Type of overwrite
	 */
	permission_overwrite(snowflake id, uint64_t allow, uint64_t deny, overwrite_type type);
};

/**
 * @brief Auto archive duration of threads which will stop showing in the channel list after the specified period of inactivity.
 * Defined as an enum to fit into 1 byte. Internally it'll be translated to minutes to match the API
 */
enum auto_archive_duration_t : uint8_t {
	/**
	 * @brief Auto archive duration of 1 hour (60 minutes).
	 */
	arc_1_hour = 1,

	/**
	 * @brief Auto archive duration of 1 day (1440 minutes).
	 */
	arc_1_day = 2,

	/**
	 * @brief Auto archive duration of 3 days (4320 minutes).
	 */
	arc_3_days = 3,

	/**
	 * @brief Auto archive duration of 1 week (10080 minutes).
	 */
	arc_1_week = 4,
};

/**
 * @brief Represents a tag that is able to be applied to a thread in a forum or media channel
 */
struct DPP_EXPORT forum_tag : public managed, public json_interface<forum_tag> {
protected:
	friend struct json_interface<forum_tag>;

	/**
	 * @brief Read struct values from a json object
	 * @param j json to read values from
	 * @return A reference to self
	 */
	forum_tag& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build json for this forum_tag object
	 *
	 * @param with_id include the ID in the json
	 * @return json JSON object
	 */
	json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief The name of the tag (0-20 characters).
	 */
	std::string name;

	/**
	 * @brief The emoji of the tag.
	 * Contains either nothing, the id of a guild's custom emoji or the unicode character of the emoji.
	 */
	std::variant<std::monostate, snowflake, std::string> emoji;

	/**
	 * @brief Whether this tag can only be added to or removed from threads
	 * by a member with the `MANAGE_THREADS` permission.
	 */
	bool moderated;

	/** Constructor */
	forum_tag();

	/**
	 * @brief Constructor
	 *
	 * @param name The name of the tag. It will be truncated to the maximum length of 20 UTF-8 characters.
	 */
	forum_tag(const std::string& name);

	/** Destructor */
	virtual ~forum_tag() = default;

	/**
	 * @brief Set name of this forum_tag object
	 *
	 * @param name Name to set
	 * @return Reference to self, so these method calls may be chained
	 *
	 * @note name will be truncated to 20 chars, if longer
	 */
	forum_tag& set_name(const std::string& name);
};

/**
 * @brief A definition of a discord channel.
 * There are one of these for every channel type except threads. Threads are
 * special snowflakes. Get it? A Discord pun. Hahaha. .... I'll get my coat.
 */ 
class DPP_EXPORT channel : public managed, public json_interface<channel> {
protected:
	friend struct json_interface<channel>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	channel& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build json for this channel object
	 *
	 * @param with_id include the ID in the json
	 * @return json JSON object
	 */
	virtual json to_json_impl(bool with_id = false) const;

	static constexpr uint16_t CHANNEL_TYPE_MASK = 0b0000000000001111;

public:
	/**
	 * @brief Channel name (1-100 characters).
	 */
	std::string name;

	/**
	 * @brief Channel topic (0-4096 characters for forum and media channels, 0-1024 characters for all others).
	 */
	std::string topic;

	/**
	 * @brief Voice region if set for voice channel, otherwise empty string.
	 */
	std::string rtc_region;

	/**
	 * @brief DM recipients.
	 */
	std::vector<snowflake> recipients;

	/**
	 * @brief Permission overwrites to apply to base permissions.
	 */
	std::vector<permission_overwrite> permission_overwrites;

	/**
	 * @brief A set of tags that can be used in a forum or media channel.
	 */
	std::vector<forum_tag> available_tags;

	/**
	 * @brief The emoji to show as the default reaction button on a thread in a forum or media channel.
	 * Contains either nothing, the id of a guild's custom emoji or the unicode character of the emoji.
	 */
	std::variant<std::monostate, snowflake, std::string> default_reaction;

	/**
	 * @brief Channel icon (for group DMs).
	 */
	utility::iconhash icon;

	/**
	 * @brief User ID of the creator for group DMs or threads.
	 */
	snowflake owner_id;

	/**
	 * @brief Parent ID (for guild channels: id of the parent category, for threads: id of the text channel this thread was created).
	 */
	snowflake parent_id;

	/**
	 * @brief Guild id of the guild that owns the channel.
	 */
	snowflake guild_id;

	/**
	 * @brief ID of last message to be sent to the channel.
	 *
	 * @warning may not point to an existing or valid message/thread.
	 */
	snowflake last_message_id;

	/**
	 * @brief Timestamp of last pinned message.
	 */
	time_t last_pin_timestamp;

	/**
	 * @brief This is only filled when the channel is part of the `resolved` set
	 * sent within an interaction. Any other time it contains zero. When filled,
	 * it contains the calculated permission bitmask of the user issuing the command
	 * within this channel.
	 */
	permission permissions;

	/**
	 * @brief Sorting position, lower number means higher up the list
	 */
	uint16_t position;

	/**
	 * @brief The bitrate (in kilobits) of the voice channel.
	 */
	uint16_t bitrate;

	/**
	 * @brief Amount of seconds a user has to wait before sending another message (0-21600).
	 * Bots, as well as users with the permission manage_messages or manage_channel, are unaffected
	 */
	uint16_t rate_limit_per_user;

	/**
	 * @brief The initial `rate_limit_per_user` to set on newly created threads in a channel.
	 * This field is copied to the thread at creation time and does not live update.
	 */
	uint16_t default_thread_rate_limit_per_user;

	/**
	 * @brief Default duration, copied onto newly created threads. Used by the clients, not the API.
	 * Threads will stop showing in the channel list after the specified period of inactivity. Defaults to dpp::arc_1_day.
	 */
	auto_archive_duration_t default_auto_archive_duration;

	/**
	 * @brief The default sort order type used to order posts in forum and media channels.
	 */
	default_forum_sort_order_t default_sort_order;

	/**
	 * @brief Flags bitmap (dpp::channel_flags)
	 */
	uint16_t flags;

	/**
	 * @brief Maximum user limit for voice channels (0-99)
	 */
	uint8_t user_limit;

	/** Constructor */
	channel();

	/** Destructor */
	virtual ~channel();

	/**
	* @brief Create a mentionable channel.
	* @param id The ID of the channel.
	* @return std::string The formatted mention of the channel.
	*/
	static std::string get_mention(const snowflake& id);

	/**
	 * @brief Set name of this channel object
	 *
	 * @param name Name to set
	 * @return Reference to self, so these method calls may be chained 
	 *
	 * @note name will be truncated to 100 chars, if longer
	 * @throw dpp::length_exception if length < 1
	 */
	channel& set_name(const std::string& name);

	/**
	 * @brief Set topic of this channel object
	 *
	 * @param topic Topic to set
	 * @return Reference to self, so these method calls may be chained 
	 *
	 * @note topic will be truncated to 1024 chars, if longer
	 */
	channel& set_topic(const std::string& topic);

	/**
	 * @brief Set type of this channel object
	 *
	 * @param type Channel type to set
	 * @return Reference to self, so these method calls may be chained
	 */
	channel& set_type(channel_type type);

	/**
	 * @brief Set the default forum layout type for the forum channel
	 *
	 * @param layout_type The layout type
	 * @return Reference to self, so these method calls may be chained
	 */
	channel& set_default_forum_layout(forum_layout_type layout_type);

	/**
	 * @brief Set the default forum sort order for the forum channel
	 *
	 * @param sort_order The sort order
	 * @return Reference to self, so these method calls may be chained
	 */
	channel& set_default_sort_order(default_forum_sort_order_t sort_order);

	/**
	 * @brief Set flags for this channel object
	 *
	 * @param flags Flag bitmask to set from dpp::channel_flags
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_flags(const uint16_t flags);

	/**
	 * @brief Add (bitwise OR) a flag to this channel object
	 * 	
	 * @param flag Flag bit to add from dpp::channel_flags
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& add_flag(const channel_flags flag);

	/**
	 * @brief Remove (bitwise NOT AND) a flag from this channel object
	 * 	
	 * @param flag Flag bit to remove from dpp::channel_flags
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& remove_flag(const channel_flags flag);

	/**
	 * @brief Set position of this channel object
	 *
	 * @param position Position to set
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_position(const uint16_t position);

	/**
	 * @brief Set guild_id of this channel object
	 *
	 * @param guild_id Guild ID to set
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_guild_id(const snowflake guild_id);

	/**
	 * @brief Set parent_id of this channel object
	 *
	 * @param parent_id Parent ID to set
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_parent_id(const snowflake parent_id);

	/**
	 * @brief Set user_limit of this channel object
	 *
	 * @param user_limit Limit to set
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_user_limit(const uint8_t user_limit);

	/**
	 * @brief Set bitrate of this channel object
	 *
	 * @param bitrate Bitrate to set (in kilobits)
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_bitrate(const uint16_t bitrate);

	/**
	 * @brief Set nsfw property of this channel object
	 *
	 * @param is_nsfw true, if channel is nsfw
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_nsfw(const bool is_nsfw);

	/**
	 * @brief Set lock permissions property of this channel object
	 * Used only with the reorder channels method
	 *
	 * @param is_lock_permissions true, if we are to inherit permissions from the category
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_lock_permissions(const bool is_lock_permissions);

	/**
	 * @brief Set rate_limit_per_user of this channel object
	 *
	 * @param rate_limit_per_user rate_limit_per_user (slowmode in sec) to set
	 * @return Reference to self, so these method calls may be chained 
	 */
	channel& set_rate_limit_per_user(const uint16_t rate_limit_per_user);

	/**
	 * @brief Add permission overwrites for a user or role.
	 * If the channel already has permission overwrites for the passed target, the existing ones will be adjusted by the passed permissions
	 *
	 * @param target ID of the role or the member you want to adjust overwrites for
	 * @param type type of overwrite
	 * @param allowed_permissions bitmask of dpp::permissions you want to allow for this user/role in this channel. Note: You can use the dpp::permission class
	 * @param denied_permissions bitmask of dpp::permissions you want to deny for this user/role in this channel. Note: You can use the dpp::permission class
	 *
	 * @return Reference to self, so these method calls may be chained
	 */
	channel& add_permission_overwrite(const snowflake target, const overwrite_type type, const uint64_t allowed_permissions, const uint64_t denied_permissions);
	/**
	 * @brief Set permission overwrites for a user or role on this channel object. Old permission overwrites for the target will be overwritten
	 *
	 * @param target ID of the role or the member you want to set overwrites for
	 * @param type type of overwrite
	 * @param allowed_permissions bitmask of allowed dpp::permissions for this user/role in this channel. Note: You can use the dpp::permission class
	 * @param denied_permissions bitmask of denied dpp::permissions for this user/role in this channel. Note: You can use the dpp::permission class
	 *
	 * @return Reference to self, so these method calls may be chained
	 *
	 * @note If both `allowed_permissions` and `denied_permissions` parameters are 0, the permission overwrite for the target will be removed
	 */
	channel& set_permission_overwrite(const snowflake target, const overwrite_type type, const uint64_t allowed_permissions, const uint64_t denied_permissions);
	/**
	 * @brief Remove channel specific permission overwrites of a user or role
	 *
	 * @param target ID of the role or the member you want to remove permission overwrites of
	 * @param type type of overwrite
	 *
	 * @return Reference to self, so these method calls may be chained
	 */
	channel& remove_permission_overwrite(const snowflake target, const overwrite_type type);

	/**
	 * @brief Get the channel type
	 *
	 * @return channel_type Channel type
	 */
	channel_type get_type() const;

	/**
	 * @brief Get the default forum layout type used to display posts in forum channels
	 *
	 * @return forum_layout_types Forum layout type
	 */
	forum_layout_type get_default_forum_layout() const;

	/**
	 * @brief Get the mention ping for the channel
	 * 
	 * @return std::string mention
	 */
	std::string get_mention() const;

	/**
	 * @brief Get the overall permissions for a member in this channel, including channel overwrites, role permissions and admin privileges.
	 * 
	 * @param user The user to resolve the permissions for
	 * @return permission Permission overwrites for the member. Made of bits in dpp::permissions.
	 * @note Requires role cache to be enabled (it's enabled by default).
	 *
	 * @note This is an alias for guild::permission_overwrites and searches for the guild in the cache,
	 * so consider using guild::permission_overwrites if you already have the guild object.
	 *
	 * @warning The method will search for the guild member in the cache by the users id.
	 * If the guild member is not in cache, the method will always return 0.
	 */
	permission get_user_permissions(const class user* user) const;

	/**
	 * @brief Get the overall permissions for a member in this channel, including channel overwrites, role permissions and admin privileges.
	 *
	 * @param member The member to resolve the permissions for
	 * @return permission Permission overwrites for the member. Made of bits in dpp::permissions.
	 * @note Requires role cache to be enabled (it's enabled by default).
	 *
	 * @note This is an alias for guild::permission_overwrites and searches for the guild in the cache,
	 * so consider using guild::permission_overwrites if you already have the guild object.
	 */
	permission get_user_permissions(const class guild_member &member) const;

	/**
	 * @brief Return a map of members on the channel, built from the guild's
	 * member list based on which members have the VIEW_CHANNEL permission.
	 * Does not return reliable information for voice channels, use
	 * dpp::channel::get_voice_members() instead for this.
	 * @return A map of guild members keyed by user id.
	 * @note If the guild this channel belongs to is not in the cache, the function will always return 0.
	 */
	std::map<snowflake, class guild_member*> get_members();

	/**
	 * @brief Get a map of members in this channel, if it is a voice channel.
	 * The map is keyed by snowflake id of the user.
	 * 
	 * @return std::map<snowflake, voicestate> The voice members of the channel
	 */
	std::map<snowflake, voicestate> get_voice_members();

	/**
	 * @brief Get the channel's icon url (if its a group DM), otherwise returns an empty string
	 *
	 * @param size The size of the icon in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized icon is returned.
	 * @param format The format to use for the avatar. It can be one of `i_webp`, `i_jpg` or `i_png`.
	 * @return std::string icon url or an empty string, if required attributes are missing or an invalid format was passed
	 */
	std::string get_icon_url(uint16_t size = 0, const image_type format = i_png) const;

	/**
	 * @brief Returns string of URL to channel
	 * 
	 * @return string of URL to channel
	 */
	std::string get_url() const;

	/**
	 * @brief Returns true if the channel is NSFW gated
	 * 
	 * @return true if NSFW
	 */
	bool is_nsfw() const;

	/**
	 * @brief Returns true if the permissions are to be synced with the category it is in.
	 * Used only and set manually when using the reorder channels method.
	 * 
	 * @return true if keeping permissions
	 */
	bool is_locked_permissions() const;

	/**
	 * @brief Returns true if the channel is a text channel
	 * 
	 * @return true if text channel
	 */
	bool is_text_channel() const;

	/**
	 * @brief Returns true if the channel is a DM
	 * 
	 * @return true if is a DM
	 */
	bool is_dm() const;

	/**
	 * @brief Returns true if the channel is a voice channel
	 * 
	 * @return true if voice channel
	 */
	bool is_voice_channel() const;

	/**
	 * @brief Returns true if the channel is a group DM channel
	 * 
	 * @return true if group DM
	 */
	bool is_group_dm() const;

	/**
	 * @brief Returns true if the channel is a category
	 * 
	 * @return true if a category
	 */
	bool is_category() const;

	/**
	 * @brief Returns true if the channel is a forum
	 * 
	 * @return true if a forum
	 */
	bool is_forum() const;

	/**
	 * @brief Returns true if the channel is a media channel
	 *
	 * @return true if media channel
	 */
	bool is_media_channel() const;

	/**
	 * @brief Returns true if the channel is an announcement channel
	 * 
	 * @return true if announcement channel
	 */
	bool is_news_channel() const;

	/**
	 * @brief Returns true if the channel is a store channel
	 * @deprecated store channels are deprecated by Discord
	 * 
	 * @return true if store channel
	 */
	bool is_store_channel() const;

	/**
	 * @brief Returns true if the channel is a stage channel
	 * 
	 * @return true if stage channel
	 */
	bool is_stage_channel() const;

	/**
	 * @brief Returns true if video quality is auto
	 * 
	 * @return true if video quality is auto
	 */
	bool is_video_auto() const;

	/**
	 * @brief Returns true if video quality is 720p
	 * 
	 * @return true if video quality is 720p
	 */
	bool is_video_720p() const;

	/**
	 * @brief Returns true if channel is a pinned thread in forum
	 *
	 * @return true, if channel is a pinned thread in forum
	 */
	bool is_pinned_thread() const;

	/**
	 * @brief Returns true if a tag is required to be specified when creating a thread in a forum channel
	 *
	 * @return true, if a tag is required to be specified when creating a thread in a forum channel
	 */
	bool is_tag_required() const;

	/**
	 * @brief Returns true if embedded media download options are hidden in a media channel
	 *
	 * @return true, if embedded media download options are hidden in a media channel
	 */
	bool is_download_options_hidden() const;

};

/**
 * @brief Serialize a permission_overwrite object to json
 *
 * @param j JSON object to serialize to
 * @param po object to serialize
 */
void to_json(nlohmann::json& j, const permission_overwrite& po);

/**
 * @brief A group of channels
 */
typedef std::unordered_map<snowflake, channel> channel_map;

} // namespace dpp

