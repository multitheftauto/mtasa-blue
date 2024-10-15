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

#include <utility>

namespace dpp {

void cluster::channel_create(const class channel &c, command_completion_event_t callback) {
	rest_request<channel>(this, API_PATH "/guilds", std::to_string(c.guild_id), "channels", m_post, c.build_json(), callback);
}

void cluster::channel_delete_permission(const class channel &c, snowflake overwrite_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(c.id), "permissions/" + std::to_string(overwrite_id), m_delete, "", callback);
}

void cluster::channel_delete(snowflake channel_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(channel_id), "", m_delete, "", callback);
}

void cluster::channel_edit_permissions(const class channel &c, const snowflake overwrite_id, const uint64_t allow, const uint64_t deny, const bool member, command_completion_event_t callback) {
	channel_edit_permissions(c.id, overwrite_id, allow, deny, member, callback);
}

void cluster::channel_edit_permissions(const snowflake channel_id, const snowflake overwrite_id, const uint64_t allow, const uint64_t deny, const bool member, command_completion_event_t callback) {
	json j({ {"allow", std::to_string(allow)}, {"deny", std::to_string(deny)}, {"type", member ? 1 : 0}  });
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(channel_id), "permissions/" + std::to_string(overwrite_id), m_put, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::channel_edit_positions(const std::vector<channel> &c, command_completion_event_t callback) {
	json j = json::array();
	if (c.empty()) {
		return;
	}
	for (auto & ch : c) {
		json cj({ {"id", ch.id}, {"position", ch.position}  });	
		if (ch.parent_id) {
			cj["parent_id"] = std::to_string(ch.parent_id);
		}
		if (ch.flags & c_lock_permissions) {
			cj["lock_permissions"] = true;
		}
		j.push_back(cj);
	}
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(c[0].guild_id), "channels/" + std::to_string(c[0].id), m_patch, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::channel_edit(const class channel &c, command_completion_event_t callback) {
	rest_request<channel>(this, API_PATH "/channels", std::to_string(c.id), "", m_patch, c.build_json(true), callback);
}

void cluster::channel_follow_news(const class channel &c, snowflake target_channel_id, command_completion_event_t callback) {
	json j({ {"webhook_channel_id", target_channel_id} });
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(c.id), "followers", m_post, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::channel_get(snowflake c, command_completion_event_t callback) {
	rest_request<channel>(this, API_PATH "/channels", std::to_string(c), "", m_get, "", callback);
}

void cluster::channel_invite_create(const class channel &c, const class invite &i, command_completion_event_t callback) {
	rest_request<invite>(this, API_PATH "/channels", std::to_string(c.id), "invites", m_post, i.build_json(), callback);
}

void cluster::channel_invites_get(const class channel &c, command_completion_event_t callback) {
	rest_request_list<invite>(this, API_PATH "/channels", std::to_string(c.id), "", m_get, "", callback);
}

void cluster::channel_typing(const class channel &c, command_completion_event_t callback) {
	channel_typing(c.id, callback);
}

void cluster::channel_typing(snowflake cid, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(cid), "typing", m_post, "", callback);
}


void cluster::channels_get(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<channel>(this, API_PATH "/guilds", std::to_string(guild_id), "channels", m_get, "", callback);
}

void cluster::channel_set_voice_status(snowflake channel_id, const std::string& status, command_completion_event_t callback) {
	json j({ {"status", status} });
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(channel_id), "voice-status", m_put, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

} // namespace dpp
