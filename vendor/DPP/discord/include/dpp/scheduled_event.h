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
#include <dpp/user.h>
#include <dpp/guild.h>
#include <dpp/json_fwd.h>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Represents the privacy of an event.
 */
enum event_privacy_level : uint8_t {
	/**
	 * @brief The event is visible to only guild members.
	 */
	ep_guild_only = 2
};

/**
 * @brief Event entity types.
 */
enum event_entity_type : uint8_t {
	/**
	 * @brief A stage instance.
	 */
	eet_stage_instance = 1,

	/**
	 * @brief A voice channel.
	 */
	eet_voice = 2,

	/**
	 * @brief External to discord, or a text channel etc.
	 */
	eet_external = 3
};

/**
 * @brief Event status types.
 */
enum event_status : uint8_t {
	/**
	 * @brief Scheduled.
	 */
	es_scheduled = 1,

	/**
	 * @brief Active now.
	 */
	es_active = 2,

	/**
	 * @brief Completed.
	 */
	es_completed = 3,

	/**
	 * @brief Cancelled.
	 */
	es_cancelled = 4
};

/**
 * @brief Entities for the event.
 */
struct DPP_EXPORT event_entities {
	/**
	 * @brief Location of the event.
	 */
	std::string location;
};

/**
 * @brief Represents a guild member/user who has registered interest in an event.
 * 
 */
struct DPP_EXPORT event_member {
	/**
	 * @brief Event ID associated with.
	 */
        snowflake guild_scheduled_event_id;

	/**
	 * @brief User details of associated user.
	 * 
	 */
        dpp::user user;

	/**
	 * @brief Member details of user on the associated guild.
	 */
        dpp::guild_member member;
};

/**
 * @brief A scheduled event.
 */
struct DPP_EXPORT scheduled_event : public managed, public json_interface<scheduled_event> {
protected:
	friend struct json_interface<scheduled_event>;

	/**
	 * @brief Serialise a scheduled_event object from json
	 *
	 * @return scheduled_event& a reference to self
	 */
	scheduled_event& fill_from_json_impl(const nlohmann::json* j);

	/**
	 * @brief Build json for this object
	 * @param with_id Include id field in json
	 *
	 * @return std::string Json of this object
	 */
	json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief The guild ID which the scheduled event belongs to.
	 */
	snowflake guild_id;

	/**
	 * @brief The channel ID in which the scheduled event will be hosted, or null if scheduled entity type is EXTERNAL.
	 *
	 * @note This may be empty.
	 */
	snowflake channel_id;

	/**
	 * @brief Optional: The ID of the user that created the scheduled event.
	 */
	snowflake creator_id;

	/**
	 * @brief The name of the scheduled event.
	 */
	std::string name;

	/**
	 * @brief Optional: The description of the scheduled event (1-1000 characters).
	 */
	std::string description;

	/**
	 * @brief The image of the scheduled event.
	 *
	 * @note This may be empty.
	 */
	utility::icon image;

	/**
	 * @brief The time the scheduled event will start.
	 */
	time_t scheduled_start_time;

	/**
	 * @brief The time the scheduled event will end, or null if the event does not have a scheduled time to end.
	 *
	 * @note This may be empty.
	 */
	time_t scheduled_end_time;

	/**
	 * @brief The privacy level of the scheduled event.
	 */
	event_privacy_level privacy_level;

	/**
	 * @brief The status of the scheduled event.
	 */
	event_status status;

	/**
	 * @brief The type of hosting entity associated with a scheduled event.
	 * e.g. voice channel or stage channel.
	 */
	event_entity_type entity_type;

	/**
	 * @brief Any additional ID of the hosting entity associated with event.
	 * e.g. stage instance ID.
	 *
	 * @note This may be empty.
	 */
	snowflake entity_id;

	/**
	 * @brief The entity metadata for the scheduled event.
	 *
	 * @note This may be empty.
	 */
	event_entities entity_metadata;

	/**
	 * @brief Optional: The creator of the scheduled event.
	 */
	user creator;

	/**
	 * @brief Optional: The number of users subscribed to the scheduled event.
	 */
	uint32_t user_count;

	/**
	 * @brief Create a scheduled_event object.
	 */
	scheduled_event();

	/**
	 * @brief Set the name of the event.
	 * Minimum length: 1, Maximum length: 100
	 * @param n event name
	 * @return scheduled_event& reference to self
	 * @throw dpp::length_error if length < 1
	 */
	scheduled_event& set_name(const std::string& n);

	/**
	 * @brief Set the description of the event.
	 * Minimum length: 1 (if set), Maximum length: 100
	 * @param d event description
	 * @return scheduled_event& reference to self
	 * @throw dpp::length_error if length < 1
	 */
	scheduled_event& set_description(const std::string& d);

	/**
	 * @brief Clear the description of the event.
	 * @return scheduled_event& reference to self
	 */
	scheduled_event& clear_description();

	/**
	 * @brief Set the location of the event.
	 * Minimum length: 1, Maximum length: 1000
	 * @note Clears channel_id
	 * @param l event location
	 * @return scheduled_event& reference to self
	 * @throw dpp::length_error if length < 1
	 */
	scheduled_event& set_location(const std::string& l);

	/**
	 * @brief Set the voice channel id of the event.
	 * @note clears location
	 * @param c channel ID
	 * @return scheduled_event& reference to self
	 */
	scheduled_event& set_channel_id(snowflake c);

	/**
	 * @brief Set the creator id of the event.
	 * @param c creator user ID
	 * @return scheduled_event& reference to self
	 */
	scheduled_event& set_creator_id(snowflake c);

	/**
	 * @brief Set the status of the event.
	 * @param s status to set
	 * @return scheduled_event& reference to self
	 * @throw dpp::logic_exception if status change is not valid
	 */
	scheduled_event& set_status(event_status s);

	/**
	 * @brief Set the start time of the event.
	 * @param t starting time
	 * @return scheduled_event& reference to self
	 * @throw dpp::length_error if time is before now
	 */
	scheduled_event& set_start_time(time_t t);

	/**
	 * @brief Set the end time of the event.
	 * @param t ending time
	 * @return scheduled_event& reference to self
	 * @throw dpp::length_error if time is before now
	 */
	scheduled_event& set_end_time(time_t t);

	/**
	 * @brief Load an image for the event cover.
	 *
	 * @param image_blob Image binary data
	 * @param type Type of image. It can be one of `i_gif`, `i_jpg` or `i_png`.
	 * @return emoji& Reference to self
	 */
	scheduled_event& load_image(std::string_view image_blob, const image_type type);

	/**
	 * @brief Load an image for the event cover.
	 *
	 * @param data Image binary data
	 * @param size Size of the image.
	 * @param type Type of image. It can be one of `i_gif`, `i_jpg` or `i_png`.
	 * @return emoji& Reference to self
	 */
	scheduled_event& load_image(const std::byte* data, uint32_t size, const image_type type);
};

/**
 * @brief A group of scheduled events.
 */
typedef std::unordered_map<snowflake, scheduled_event> scheduled_event_map;

/**
 * @brief A group of scheduled event members.
 */
typedef std::unordered_map<snowflake, event_member> event_member_map;


} // namespace dpp
