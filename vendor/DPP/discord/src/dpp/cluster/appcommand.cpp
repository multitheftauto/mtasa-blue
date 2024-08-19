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
#include <dpp/appcommand.h>
#include <dpp/restrequest.h>

namespace dpp {

void cluster::global_bulk_command_create(const std::vector<slashcommand> &commands, command_completion_event_t callback) {
	json j = json::array();
	for (auto & s : commands) {
		j.push_back(s.to_json(false));
	}
	rest_request_list<slashcommand>(this, API_PATH "/applications", std::to_string(commands.size() > 0 && commands[0].application_id ? commands[0].application_id : me.id), "commands", m_put, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::global_bulk_command_delete(command_completion_event_t callback) {
	rest_request_list<slashcommand>(this, API_PATH "/applications", std::to_string(me.id), "commands", m_put, "{}", callback);
}

void cluster::global_command_create(const slashcommand &s, command_completion_event_t callback) {
	rest_request<slashcommand>(this, API_PATH "/applications", std::to_string(s.application_id ? s.application_id : me.id), "commands", m_post, s.build_json(false), callback);
}

void cluster::global_command_get(snowflake id, command_completion_event_t callback) {
	rest_request<slashcommand>(this, API_PATH "/applications", std::to_string(me.id), "commands/" + std::to_string(id), m_get, "", callback);
}

void cluster::global_command_delete(snowflake id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/applications", std::to_string(me.id), "commands/" + std::to_string(id), m_delete, "", callback);
}

void cluster::global_command_edit(const slashcommand &s, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/applications", std::to_string(s.application_id ? s.application_id : me.id), "commands/" + std::to_string(s.id), m_patch, s.build_json(true), callback);
}

void cluster::global_commands_get(command_completion_event_t callback) {
	rest_request_list<slashcommand>(this, API_PATH "/applications", std::to_string(me.id), "commands", m_get, "", callback);
}

void cluster::guild_bulk_command_create(const std::vector<slashcommand> &commands, snowflake guild_id, command_completion_event_t callback) {
	json j = json::array();
	for (auto & s : commands) {
		j.push_back(s.to_json(false));
	}
	rest_request_list<slashcommand>(this, API_PATH "/applications", std::to_string(commands.size() > 0 && commands[0].application_id ? commands[0].application_id : me.id), "guilds/" + std::to_string(guild_id) + "/commands", m_put, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::guild_bulk_command_delete(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<slashcommand>(this, API_PATH "/applications", std::to_string(me.id), "guilds/" + std::to_string(guild_id) + "/commands", m_put, "{}", callback);
}

void cluster::guild_commands_get_permissions(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<guild_command_permissions>(this, API_PATH "/applications", std::to_string(me.id), "guilds/" + std::to_string(guild_id) + "/commands/permissions", m_get, "", callback);
}

void cluster::guild_bulk_command_edit_permissions(const std::vector<slashcommand> &commands, snowflake guild_id, command_completion_event_t callback) {
	if (commands.empty()) {
		return;
	}
	json j = json::array();
	for (auto & s : commands) {
		json jcommand;
		jcommand["id"] = s.id;
		jcommand["permissions"] = json::array();
		for (auto & c : s.permissions) {
			jcommand["permissions"].push_back(c);
		}
		j.push_back(jcommand);
	}
	rest_request_list<guild_command_permissions>(this, API_PATH "/applications", std::to_string(me.id), "guilds/" + std::to_string(guild_id) + "/commands/permissions", m_put, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::guild_command_create(const slashcommand &s, snowflake guild_id, command_completion_event_t callback) {
	this->post_rest(API_PATH "/applications", std::to_string(s.application_id ? s.application_id : me.id), "guilds/" + std::to_string(guild_id) + "/commands", m_post, s.build_json(false), [s, this, guild_id, callback] (json &j, const http_request_completion_t& http) mutable {
		if (callback) {
			callback(confirmation_callback_t(this, slashcommand().fill_from_json(&j), http));
		}

		if (http.status < 300 && s.permissions.size()) {
			slashcommand n;
			n.fill_from_json(&j);
			n.permissions = s.permissions;
			guild_command_edit_permissions(n, guild_id);
		}
	});
}

void cluster::guild_command_delete(snowflake id, snowflake guild_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/applications", std::to_string(me.id), "guilds/" + std::to_string(guild_id) + "/commands/" + std::to_string(id), m_delete, "", callback);
}

void cluster::guild_command_edit_permissions(const slashcommand &s, snowflake guild_id, command_completion_event_t callback) {
	json j;
	if (!s.permissions.empty()) {
		j["permissions"] = json();
		for (const auto& perm : s.permissions) {
			json jperm = perm;
			j["permissions"].push_back(jperm);
		}
	}
	rest_request<confirmation>(this, API_PATH "/applications", std::to_string(s.application_id ? s.application_id : me.id), "guilds/" + std::to_string(guild_id) + "/commands/" + std::to_string(s.id) + "/permissions", m_put, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::guild_command_get(snowflake id, snowflake guild_id, command_completion_event_t callback) {
	rest_request<slashcommand>(this, API_PATH "/applications", std::to_string(me.id), "guilds/" + std::to_string(guild_id) + "/commands/" + std::to_string(id), m_get, "", callback);
}

void cluster::guild_command_get_permissions(snowflake id, snowflake guild_id, command_completion_event_t callback) {
	rest_request<guild_command_permissions>(this, API_PATH "/applications", std::to_string(me.id), "guilds/" + std::to_string(guild_id) + "/commands/" + std::to_string(id) + "/permissions", m_get, "", callback);
}

void cluster::guild_command_edit(const slashcommand &s, snowflake guild_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/applications", std::to_string(s.application_id ? s.application_id : me.id), "guilds/" + std::to_string(guild_id) + "/commands/" + std::to_string(s.id), m_patch, s.build_json(true), callback);
}

void cluster::guild_commands_get(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<slashcommand>(this, API_PATH "/applications", std::to_string(me.id), "/guilds/" + std::to_string(guild_id) + "/commands", m_get, "", callback);
}

void cluster::interaction_response_create(snowflake interaction_id, const std::string &token, const interaction_response &r, command_completion_event_t callback) {
	this->post_rest_multipart(API_PATH "/interactions", std::to_string(interaction_id), utility::url_encode(token) + "/callback", m_post, r.build_json(), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, confirmation(), http));
		}
	}, r.msg.file_data);
}

void cluster::interaction_response_edit(const std::string &token, const message &m, command_completion_event_t callback) {
	this->post_rest_multipart(API_PATH "/webhooks", std::to_string(me.id), utility::url_encode(token) + "/messages/@original", m_patch, m.build_json(), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, confirmation(), http));
		}
	}, m.file_data);
}

void cluster::interaction_response_get_original(const std::string &token, command_completion_event_t callback) {
	rest_request<message>(this, API_PATH "/webhooks",std::to_string(me.id), utility::url_encode(token) + "/messages/@original", m_get, "", callback);
}

void cluster::interaction_followup_create(const std::string &token, const message &m, command_completion_event_t callback) {
	this->post_rest_multipart(API_PATH "/webhooks", std::to_string(me.id), utility::url_encode(token), m_post, m.build_json(), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, confirmation(), http));
		}
	}, m.file_data);
}

void cluster::interaction_followup_edit_original(const std::string &token, const message &m, command_completion_event_t callback) {
	this->post_rest_multipart(API_PATH "/webhooks", std::to_string(me.id), utility::url_encode(token) + "/messages/@original", m_patch, m.build_json(), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, confirmation(), http));
		}
	}, m.file_data);
}

void cluster::interaction_followup_delete(const std::string &token, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/webhooks",std::to_string(me.id), utility::url_encode(token) + "/messages/@original", m_delete, "", callback);
}

void cluster::interaction_followup_edit(const std::string &token, const message &m, command_completion_event_t callback) {
	this->post_rest_multipart(API_PATH "/webhooks", std::to_string(me.id), utility::url_encode(token) + "/messages/" + std::to_string(m.id), m_patch, m.build_json(), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, confirmation(), http));
		}
	}, m.file_data);
}

void cluster::interaction_followup_get(const std::string &token, snowflake message_id, command_completion_event_t callback) {
	rest_request<message>(this, API_PATH "/webhooks",std::to_string(me.id), utility::url_encode(token) + "/messages/" + std::to_string(message_id), m_get, "", callback);
}

void cluster::interaction_followup_get_original(const std::string &token, command_completion_event_t callback) {
	rest_request<message>(this, API_PATH "/webhooks",std::to_string(me.id), utility::url_encode(token) + "/messages/@original", m_get, "", callback);
}

} // namespace dpp
