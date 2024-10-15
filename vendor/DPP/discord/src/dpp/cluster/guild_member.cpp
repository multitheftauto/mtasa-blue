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
#include <dpp/restrequest.h>

namespace dpp {

void cluster::guild_add_member(const guild_member& gm, const std::string &access_token, command_completion_event_t callback) {
	json j = gm.to_json();
	j["access_token"] = access_token;
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(gm.guild_id), "members/" + std::to_string(gm.user_id), m_put, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}


void cluster::guild_edit_member(const guild_member& gm, command_completion_event_t callback) {
	this->post_rest(API_PATH "/guilds", std::to_string(gm.guild_id), "members/" + std::to_string(gm.user_id), m_patch, gm.build_json(), [this, &gm, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, guild_member().fill_from_json(&j, gm.guild_id, gm.user_id), http));
		}
	});
}


void cluster::guild_get_member(snowflake guild_id, snowflake user_id, command_completion_event_t callback) {
	this->post_rest(API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id), m_get, "", [this, callback, guild_id, user_id](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, guild_member().fill_from_json(&j, guild_id, user_id), http));
		}
	});
}


void cluster::guild_get_members(snowflake guild_id, uint16_t limit, snowflake after, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"limit", std::to_string(limit)},
		{"after", std::to_string(after)},
	});
	this->post_rest(API_PATH "/guilds", std::to_string(guild_id), "members" + parameters, m_get, "", [this, callback, guild_id](json &j, const http_request_completion_t& http) {
		guild_member_map guild_members;
		confirmation_callback_t e(this, confirmation(), http);
		if (!e.is_error()) {
			for (auto & curr_member : j) {
				if (curr_member.find("user") != curr_member.end()) {
					snowflake user_id = snowflake_not_null(&(curr_member["user"]), "id");
					guild_members[user_id] = guild_member().fill_from_json(&curr_member, guild_id, user_id);
				}
			}
		}
		if (callback) {
			callback(confirmation_callback_t(this, guild_members, http));
		}
	});
}


void cluster::guild_member_add_role(snowflake guild_id, snowflake user_id, snowflake role_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id) + "/roles/" + std::to_string(role_id), m_put, "", callback);
}


void cluster::guild_member_delete(snowflake guild_id, snowflake user_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id), m_delete, "", callback);
}


void cluster::guild_member_kick(snowflake guild_id, snowflake user_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id), m_delete, "", callback);
}


void cluster::guild_member_timeout(snowflake guild_id, snowflake user_id, time_t communication_disabled_until, command_completion_event_t callback) {
	json j;
	if (communication_disabled_until > 0) {
		j["communication_disabled_until"] = ts_to_string(communication_disabled_until);
	} else {
		j["communication_disabled_until"] = json::value_t::null;
	}

	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id), m_patch, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::guild_member_timeout_remove(snowflake guild_id, snowflake user_id, command_completion_event_t callback) {
	json j;
	j["communication_disabled_until"] = json::value_t::null;
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id), m_patch, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}


void cluster::guild_member_delete_role(snowflake guild_id, snowflake user_id, snowflake role_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id) + "/roles/" + std::to_string(role_id), m_delete, "", callback);
}


void cluster::guild_member_remove_role(snowflake guild_id, snowflake user_id, snowflake role_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id) + "/roles/" + std::to_string(role_id), m_delete, "", callback);
}


void cluster::guild_member_move(const snowflake channel_id, const snowflake guild_id, const snowflake user_id, command_completion_event_t callback) {
    json j;
    if (channel_id) {
        j["channel_id"] = channel_id;
    }
    else {
        j["channel_id"] = json::value_t::null;
    }

    this->post_rest(API_PATH "/guilds", std::to_string(guild_id), "members/" + std::to_string(user_id), m_patch, j.dump(-1, ' ', false, json::error_handler_t::replace), [this, guild_id, user_id, callback](json &j, const http_request_completion_t& http) {
	if (callback) {
	    callback(confirmation_callback_t(this, guild_member().fill_from_json(&j, guild_id, user_id), http));
	}
    });
}


void cluster::guild_search_members(snowflake guild_id, const std::string& query, uint16_t limit, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"query", query},
		{"limit", std::to_string(limit)},
	});
	this->post_rest(API_PATH "/guilds", std::to_string(guild_id), "members/search" + parameters, m_get, "", [this, callback, guild_id] (json &j, const http_request_completion_t& http) {
		guild_member_map guild_members;
		confirmation_callback_t e(this, confirmation(), http);
		if (!e.is_error()) {
			for (auto & curr_member : j) {
				if (curr_member.find("user") != curr_member.end()) {
					snowflake user_id = snowflake_not_null(&(curr_member["user"]), "id");
					guild_members[user_id] = guild_member().fill_from_json(&curr_member, guild_id, user_id);
				}
			}
		}
		if (callback) {
			callback(confirmation_callback_t(this, guild_members, http));
		}
	});
}

} // namespace dpp
