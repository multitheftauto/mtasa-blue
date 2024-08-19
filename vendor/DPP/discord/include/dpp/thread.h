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
#include <dpp/channel.h>
#include <dpp/message.h>

namespace dpp {

/**
 * @brief represents membership of a user with a thread
 */
struct DPP_EXPORT thread_member : public json_interface<thread_member> {
protected:
	friend struct json_interface<thread_member>;

	/**
	 * @brief Read struct values from a json object 
	 * @param j json to read values from
	 * @return A reference to self	
	 */
	thread_member& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief ID of the thread member is part of.
	 */
	snowflake thread_id = {};

	/**
	 * @brief ID of the member.
	 */
	snowflake user_id = {};

	/**
	 * @brief The time when user last joined the thread.
	 */
	time_t joined = 0;

	/**
	 * @brief Any user-thread settings, currently only used for notifications.
	 */
	uint32_t flags = 0;
};

/**
 * @brief A group of thread member objects. the key is the user_id of the dpp::thread_member
 */
typedef std::unordered_map<snowflake, thread_member> thread_member_map;

/**
 * @brief metadata for threads
 */
struct DPP_EXPORT thread_metadata {
	/**
	 * @brief Timestamp when the thread's archive status was last changed, used for calculating recent activity.
	 */
	time_t archive_timestamp;

	/**
	 * @brief The duration in minutes to automatically archive the thread after recent activity (60, 1440, 4320, 10080).
	 */
	uint16_t auto_archive_duration;

	/**
	 * @brief Whether a thread is archived
	 */
	bool archived;

	/**
	 * @brief Whether a thread is locked. When a thread is locked,
	 * only users with `MANAGE_THREADS` can un-archive it.
	 */
	bool locked;

	/**
	 * @brief Whether non-moderators can add other non-moderators. Only for private threads.
	 */
	bool invitable;
};

/** @brief A definition of a discord thread.
 * A thread is a superset of a channel. Not to be confused with `std::thread`!
 */
class DPP_EXPORT thread : public channel, public json_interface<thread> {
protected:
	friend struct json_interface<thread>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	thread& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build json for this thread object
	 *
	 * @param with_id include the ID in the json
	 * @return std::string JSON string
	 */
	json to_json_impl(bool with_id = false) const override;

public:
	using json_interface<thread>::fill_from_json;
	using json_interface<thread>::build_json;
	using json_interface<thread>::to_json;

	/**
	 * @brief Thread member of current user if joined to the thread.
	 * Note this is only set by certain api calls otherwise contains default data
	 */
	thread_member member = {};

	/**
	 * @brief Thread metadata (threads)
	 */
	thread_metadata metadata = {};

	/**
	 * @brief Created message. Only filled within the cluster::thread_create_in_forum() method
	 */
	message msg = {};

	/**
	 * @brief A list of dpp::forum_tag IDs that have been applied to a thread in a forum or media channel.
	 */
	std::vector<snowflake> applied_tags = {};

	/**
	 * @brief Number of messages ever sent in the thread.
	 * It's similar to thread::message_count on message creation, but will not decrement the number when a message is deleted
	 */
	uint32_t total_messages_sent = 0;

	/**
	 * @brief Number of messages (not including the initial message or deleted messages) of the thread.
	 * For threads created before July 1, 2022, the message count is inaccurate when it's greater than 50.
	 */
	uint8_t message_count = 0;

	/**
	 * @brief Approximate count of members in a thread (stops counting at 50)
	 */
	uint8_t member_count = 0;

	/**
	 * @brief Was this thread newly created?
	 * @note This will only show in dpp::cluster::on_thread_create if the thread was just made.
	 */
	bool newly_created{false};

	/**
	 * @brief Returns true if the thread is within an announcement channel
	 *
	 * @return true if announcement thread
	 */
	constexpr bool is_news_thread() const noexcept {
		return (flags & channel::CHANNEL_TYPE_MASK) == CHANNEL_ANNOUNCEMENT_THREAD;
	}

	/**
	 * @brief Returns true if the channel is a public thread
	 *
	 * @return true if public thread
	 */
	constexpr bool is_public_thread() const noexcept {
		return (flags & channel::CHANNEL_TYPE_MASK) == CHANNEL_PUBLIC_THREAD;
	}

	/**
	 * @brief Returns true if the channel is a private thread
	 *
	 * @return true if private thread
	 */
	constexpr bool is_private_thread() const noexcept {
		return (flags & channel::CHANNEL_TYPE_MASK) == CHANNEL_PRIVATE_THREAD;
	}
};


/**
 * @brief Serialize a thread_metadata object to json
 *
 * @param j JSON object to serialize to
 * @param tmdata object to serialize
 */
void to_json(nlohmann::json& j, const thread_metadata& tmdata);

/**
 * @brief A group of threads
 */
typedef std::unordered_map<snowflake, thread> thread_map;

/**
 * @brief A thread alongside the bot's optional thread_member object tied to it
 */
struct active_thread_info {
	/**
	 * @brief The thread object
	 */
	thread active_thread;

	/**
	 * @brief The bot as a thread member, only present if the bot is in the thread
	 */
	std::optional<thread_member> bot_member;
};

/**
 * @brief A map of threads alongside optionally the thread_member tied to the bot if it is in the thread. The map's key is the thread id. Returned from the cluster::threads_get_active method
 */
using active_threads = std::map<snowflake, active_thread_info>;

}
