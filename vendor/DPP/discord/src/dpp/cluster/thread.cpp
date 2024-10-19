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
#include <dpp/channel.h>
#include <dpp/restrequest.h>

namespace dpp {

void cluster::current_user_join_thread(snowflake thread_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(thread_id), "/thread-members/@me", m_put, "", callback);
}

void cluster::current_user_leave_thread(snowflake thread_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(thread_id), "/thread-members/@me", m_delete, "", callback);
}

void cluster::threads_get_active(snowflake guild_id, command_completion_event_t callback) {
	this->post_rest(API_PATH "/guilds", std::to_string(guild_id), "/threads/active", m_get, "", [this, callback](json &j, const http_request_completion_t& http) {
		active_threads list;
		confirmation_callback_t e(this, confirmation(), http);
		if (!e.is_error()) {
			if (j.contains("threads")) {
				for (auto &curr_item: j["threads"]) {
					list[snowflake_not_null(&curr_item, "id")].active_thread.fill_from_json(&curr_item);
				}
			}
			if (j.contains("members")) {
				for (auto &curr_item: j["members"]) {
					snowflake thread_id = snowflake_not_null(&curr_item, "id");
					snowflake user_id = snowflake_not_null(&curr_item, "user_id");
					if (user_id == me.id) {
						list[thread_id].bot_member = thread_member().fill_from_json(&curr_item);
					}
				}
			}
		}
		if (callback) {
			callback(confirmation_callback_t(this, list, http));
		}
	});
}

void cluster::threads_get_joined_private_archived(snowflake channel_id, snowflake before_id, uint16_t limit, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"before", before_id},
		{"limit", limit},
	});
	rest_request_list<thread>(this, API_PATH "/channels", std::to_string(channel_id), "/users/@me/threads/archived/private" + parameters, m_get, "", callback);
}

void cluster::threads_get_private_archived(snowflake channel_id, time_t before_timestamp, uint16_t limit, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"before", before_timestamp},
		{"limit", limit},
	});
	this->post_rest(API_PATH "/channels", std::to_string(channel_id), "/threads/archived/private" + parameters, m_get, "", [this, callback](json &j, const http_request_completion_t& http) {
		std::unordered_map<snowflake, thread> list;
		confirmation_callback_t e(this, confirmation(), http);
		if (!e.is_error()) {
			if (j.contains("threads")) {
				for (auto &curr_item: j["threads"]) {
					list[snowflake_not_null(&curr_item, "id")].fill_from_json(&curr_item);
				}
			}
		}
		if (callback) {
			callback(confirmation_callback_t(this, list, http));
		}
	});	
}

void cluster::threads_get_public_archived(snowflake channel_id, time_t before_timestamp, uint16_t limit, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"before", before_timestamp},
		{"limit", limit},
	});
	this->post_rest(API_PATH "/channels", std::to_string(channel_id), "/threads/archived/public" + parameters, m_get, "", [this, callback](json &j, const http_request_completion_t& http) {
		std::unordered_map<snowflake, thread> list;
		confirmation_callback_t e(this, confirmation(), http);
		if (!e.is_error()) {
			if (j.contains("threads")) {
				for (auto &curr_item: j["threads"]) {
					list[snowflake_not_null(&curr_item, "id")].fill_from_json(&curr_item);
				}
			}
		}
		if (callback) {
			callback(confirmation_callback_t(this, list, http));
		}
	});
}

void cluster::thread_member_get(const snowflake thread_id, const snowflake user_id, command_completion_event_t callback) {
	rest_request<thread_member>(this, API_PATH "/channels", std::to_string(thread_id), "/thread-members/" + std::to_string(user_id), m_get, "", callback);
}

void cluster::thread_members_get(snowflake thread_id, command_completion_event_t callback) {
	rest_request_list<thread_member>(this, API_PATH "/channels", std::to_string(thread_id), "/thread-members", m_get, "", callback, "user_id");
}

void cluster::thread_create_in_forum(const std::string& thread_name, snowflake channel_id, const message& msg, auto_archive_duration_t auto_archive_duration, uint16_t rate_limit_per_user, std::vector<snowflake> applied_tags, command_completion_event_t callback)
{
	json j({
		{"name",                  thread_name},
		{"rate_limit_per_user",   rate_limit_per_user},
		{"message",               msg.to_json()},
		{"applied_tags",          applied_tags},
	});
	switch (auto_archive_duration) {
		case arc_1_hour:
			j["auto_archive_duration"] = 60;
			break;
		case arc_1_day:
			j["auto_archive_duration"] = 1440;
			break;
		case arc_3_days:
			j["auto_archive_duration"] = 4320;
			break;
		case arc_1_week:
			j["auto_archive_duration"] = 10080;
			break;
	}

	this->post_rest_multipart(API_PATH "/channels", std::to_string(channel_id), "threads", m_post, j.dump(-1, ' ', false, json::error_handler_t::replace), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			auto t = thread().fill_from_json(&j);
			confirmation_callback_t e(this, confirmation(), http);
			if (!e.is_error()) {
				if (j.contains("message")) {
					t.msg = message().fill_from_json(&(j["message"]));
				}
			}
			callback(confirmation_callback_t(this, t, http));
		}
	}, msg.file_data);
}

void cluster::thread_create(const std::string& thread_name, snowflake channel_id, uint16_t auto_archive_duration, channel_type thread_type, bool invitable, uint16_t rate_limit_per_user, command_completion_event_t callback)
{
	json j({
		{"name", thread_name},
		{"auto_archive_duration", auto_archive_duration},
		{"type", thread_type},
		{"invitable", invitable},
		{"rate_limit_per_user", rate_limit_per_user}
	});
	rest_request<thread>(this, API_PATH "/channels", std::to_string(channel_id), "threads", m_post, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::thread_edit(const thread &t, command_completion_event_t callback)
{
	rest_request<thread>(this, API_PATH "/channels", std::to_string(t.id), "", m_patch, t.build_json(), callback);
}

void cluster::thread_create_with_message(const std::string& thread_name, snowflake channel_id, snowflake message_id, uint16_t auto_archive_duration, uint16_t rate_limit_per_user, command_completion_event_t callback)
{
	json j({
		{"name", thread_name},
		{"auto_archive_duration", auto_archive_duration},
		{"rate_limit_per_user", rate_limit_per_user}
	});
	rest_request<thread>(this, API_PATH "/channels", std::to_string(channel_id), "messages/" + std::to_string(message_id) + "/threads", m_post, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::thread_member_add(snowflake thread_id, snowflake user_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(thread_id), "/thread-members/" + std::to_string(user_id), m_put, "", callback);
}

void cluster::thread_member_remove(snowflake thread_id, snowflake user_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(thread_id), "/thread-members/" + std::to_string(user_id), m_delete, "", callback);
}

void cluster::thread_get(snowflake thread_id, command_completion_event_t callback) {
	rest_request<thread>(this, API_PATH "/channels", std::to_string(thread_id), "", m_get, "", callback);
}

} // namespace dpp
