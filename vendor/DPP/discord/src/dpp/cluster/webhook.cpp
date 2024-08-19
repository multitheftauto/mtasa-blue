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
#include <dpp/webhook.h>
#include <dpp/restrequest.h>

namespace dpp {

void cluster::create_webhook(const class webhook &wh, command_completion_event_t callback) {
	rest_request<webhook>(this, API_PATH "/channels", std::to_string(wh.channel_id), "webhooks", m_post, wh.build_json(false), callback);
}

void cluster::delete_webhook(snowflake webhook_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/webhooks", std::to_string(webhook_id), "", m_delete, "", callback);
}

void cluster::delete_webhook_message(const class webhook &wh, snowflake message_id, snowflake thread_id, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"thread_id", thread_id},
	});
	rest_request<confirmation>(this, API_PATH "/webhooks", std::to_string(wh.id), utility::url_encode(!wh.token.empty() ? wh.token: token) + "/messages/" + std::to_string(message_id) + parameters, m_delete, "", callback);
}

void cluster::delete_webhook_with_token(snowflake webhook_id, const std::string &token, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/webhooks", std::to_string(webhook_id), utility::url_encode(token), m_delete, "", callback);
}

void cluster::edit_webhook(const class webhook& wh, command_completion_event_t callback) {
	rest_request<webhook>(this, API_PATH "/webhooks", std::to_string(wh.id), "", m_patch, wh.build_json(false), callback);
}

void cluster::edit_webhook_message(const class webhook &wh, const struct message& m, snowflake thread_id, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"thread_id", thread_id},
	});

	this->post_rest_multipart(API_PATH "/webhooks", std::to_string(wh.id), utility::url_encode(!wh.token.empty() ? wh.token: token) + "/messages/" + std::to_string(m.id) + parameters, m_patch, m.build_json(false), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, message(this).fill_from_json(&j), http));
		}
	}, m.file_data);
}

void cluster::edit_webhook_with_token(const class webhook& wh, command_completion_event_t callback) {
	json jwh = wh.to_json(true);
	if (jwh.find("channel_id") != jwh.end()) {
		jwh.erase(jwh.find("channel_id"));
	}
	rest_request<webhook>(this, API_PATH "/webhooks", std::to_string(wh.id), utility::url_encode(wh.token), m_patch, jwh.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::execute_webhook(const class webhook &wh, const struct message& m, bool wait, snowflake thread_id, const std::string& thread_name, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"wait", wait},
		{"thread_id", thread_id},
	});
	std::string body;
	if (!thread_name.empty() || !wh.avatar.to_string().empty() || !wh.name.empty()) { // only use json::parse if thread_name is set
		json j = m.to_json(false);
		if (!thread_name.empty()) {
			j["thread_name"] = thread_name;
		}
		if (!wh.avatar.to_string().empty()) {
			j["avatar_url"] = wh.avatar.to_string();
		}
		if (!wh.name.empty()) {
			j["username"] = wh.name;
		}
		body = j.dump(-1, ' ', false, json::error_handler_t::replace);
	}

	this->post_rest_multipart(API_PATH "/webhooks", std::to_string(wh.id), utility::url_encode(!wh.token.empty() ? wh.token : token) + parameters, m_post, !body.empty() ? body : m.build_json(false), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, message(this).fill_from_json(&j), http));
		}
	}, m.file_data);
}

void cluster::get_channel_webhooks(snowflake channel_id, command_completion_event_t callback) {
	rest_request_list<webhook>(this, API_PATH "/channels", std::to_string(channel_id), "webhooks", m_get, "", callback);
}


void cluster::get_guild_webhooks(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<webhook>(this, API_PATH "/guilds", std::to_string(guild_id), "webhooks", m_get, "", callback);
}

void cluster::get_webhook(snowflake webhook_id, command_completion_event_t callback) {
	rest_request<webhook>(this, API_PATH "/webhooks", std::to_string(webhook_id), "", m_get, "", callback);
}

void cluster::get_webhook_message(const class webhook &wh, snowflake message_id, snowflake thread_id, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"thread_id", thread_id},
	});
	rest_request<message>(this, API_PATH "/webhooks", std::to_string(wh.id), utility::url_encode(!wh.token.empty() ? wh.token: token) + "/messages/" + std::to_string(message_id) + parameters, m_get, "", callback);
}

void cluster::get_webhook_with_token(snowflake webhook_id, const std::string &token, command_completion_event_t callback) {
	rest_request<webhook>(this, API_PATH "/webhooks", std::to_string(webhook_id), utility::url_encode(token), m_get, "", callback);
}

} // namespace dpp
