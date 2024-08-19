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
#include <dpp/message.h>
#include <dpp/restrequest.h>

namespace dpp {

void cluster::message_add_reaction(const struct message &m, const std::string &reaction, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(m.channel_id), "messages/" + std::to_string(m.id) + "/reactions/" + utility::url_encode(reaction) + "/@me", m_put, "", callback);
}

void cluster::message_add_reaction(snowflake message_id, snowflake channel_id, const std::string &reaction, command_completion_event_t callback) {
	message m(channel_id, "");
	m.id = message_id;
	message_add_reaction(m, reaction, callback);
}



void cluster::message_create(const message &m, command_completion_event_t callback) {
	this->post_rest_multipart(API_PATH "/channels", std::to_string(m.channel_id), "messages", m_post, m.build_json(), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, message(this).fill_from_json(&j), http));
		}
	}, m.file_data);
}


void cluster::message_crosspost(snowflake message_id, snowflake channel_id, command_completion_event_t callback) {
	rest_request<message>(this, API_PATH "/channels", std::to_string(channel_id), "messages/" + std::to_string(message_id) + "/crosspost", m_post, "", callback);
}


void cluster::message_delete_all_reactions(const struct message &m, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(m.channel_id), "messages/" + std::to_string(m.id) + "/reactions", m_delete, "", callback);
}

void cluster::message_delete_all_reactions(snowflake message_id, snowflake channel_id, command_completion_event_t callback) {
	message m(channel_id, "");
	m.id = message_id;
	m.owner = this;
	message_delete_all_reactions(m, callback);
}


void cluster::message_delete_bulk(const std::vector<snowflake>& message_ids, snowflake channel_id, command_completion_event_t callback) {
	json j;
	for (auto & m : message_ids) {
		j["messages"].push_back(std::to_string(m));
	}
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(channel_id), "messages/bulk-delete", m_post, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}


void cluster::message_delete(snowflake message_id, snowflake channel_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(channel_id), "messages/" + std::to_string(message_id), m_delete, "", callback);
}


void cluster::message_delete_own_reaction(const struct message &m, const std::string &reaction, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(m.channel_id), "messages/" + std::to_string(m.id) + "/reactions/" + utility::url_encode(reaction) + "/@me", m_delete, "", callback);
}

void cluster::message_delete_own_reaction(snowflake message_id, snowflake channel_id, const std::string &reaction, command_completion_event_t callback) {
	message m(channel_id, "");
	m.id = message_id;
	m.owner = this;
	message_delete_own_reaction(m, reaction, callback);
}


void cluster::message_delete_reaction(const struct message &m, snowflake user_id, const std::string &reaction, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(m.channel_id), "messages/" + std::to_string(m.id) + "/reactions/" + utility::url_encode(reaction) + "/" + std::to_string(user_id), m_delete, "", callback);
}

void cluster::message_delete_reaction(snowflake message_id, snowflake channel_id, snowflake user_id, const std::string &reaction, command_completion_event_t callback) {
	message m(channel_id, "");
	m.id = message_id;
	m.owner = this;
	message_delete_reaction(m, user_id, reaction, callback);
}


void cluster::message_delete_reaction_emoji(const struct message &m, const std::string &reaction, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(m.channel_id), "messages/" + std::to_string(m.id) + "/reactions/" + utility::url_encode(reaction), m_delete, "", callback);
}

void cluster::message_delete_reaction_emoji(snowflake message_id, snowflake channel_id, const std::string &reaction, command_completion_event_t callback) {
	message m(channel_id, "");
	m.id = message_id;
	m.owner = this;
	message_delete_reaction_emoji(m, reaction, callback);
}


void cluster::message_edit(const message &m, command_completion_event_t callback) {
	this->post_rest_multipart(API_PATH "/channels", std::to_string(m.channel_id), "messages/" + std::to_string(m.id), m_patch, m.build_json(true), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, message(this).fill_from_json(&j), http));
		}
	}, m.file_data);
}

void cluster::message_edit_flags(const message &m, command_completion_event_t callback) {
	this->post_rest_multipart(API_PATH "/channels", std::to_string(m.channel_id), "messages/" + std::to_string(m.id), m_patch, nlohmann::json{
		{"flags", m.flags},
	}.dump(-1, ' ', false, json::error_handler_t::replace), [this, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(this, message(this).fill_from_json(&j), http));
		}
	}, m.file_data);
}


void cluster::message_get(snowflake message_id, snowflake channel_id, command_completion_event_t callback) {
	rest_request<message>(this, API_PATH "/channels", std::to_string(channel_id), "messages/" + std::to_string(message_id), m_get, "", callback);
}


void cluster::message_get_reactions(const struct message &m, const std::string &reaction, snowflake before, snowflake after, snowflake limit, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"before", before},
		{"after", after},
		{"limit", limit},
	});
	rest_request_list<user>(this, API_PATH "/channels", std::to_string(m.channel_id), "messages/" + std::to_string(m.id) + "/reactions/" + utility::url_encode(reaction) + parameters, m_get, "", callback);
}

void cluster::message_get_reactions(snowflake message_id, snowflake channel_id, const std::string &reaction, snowflake before, snowflake after, snowflake limit, command_completion_event_t callback) {
	message m(channel_id, "");
	m.id = message_id;
	m.owner = this;
	message_get_reactions(m, reaction, before, after, limit, callback);
}


void cluster::message_pin(snowflake channel_id, snowflake message_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(channel_id), "pins/" + std::to_string(message_id), m_put, "", callback);
}

void cluster::messages_get(snowflake channel_id, snowflake around, snowflake before, snowflake after, uint64_t limit, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"around", around},
		{"before", before},
		{"after", after},
		{"limit", limit > 100 ? 100 : limit},
	});
	rest_request_list<message>(this, API_PATH "/channels", std::to_string(channel_id), "messages" + parameters, m_get, "", callback);
}


void cluster::message_unpin(snowflake channel_id, snowflake message_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/channels", std::to_string(channel_id), "pins/" + std::to_string(message_id), m_delete, "", callback);
}


void cluster::poll_get_answer_voters(const message& m, uint32_t answer_id, snowflake after, uint64_t limit, command_completion_event_t callback) {
	std::map<std::string, std::string> parameters {
		{"limit", std::to_string(limit > 100 ? 100 : limit)}
	};

	if (after > 0) {
		parameters["after"] = after;
	}
	rest_request_list<user>(this, API_PATH "/channels", std::to_string(m.channel_id), "polls/" + std::to_string(m.id) + "/answers/" + std::to_string(answer_id) + utility::make_url_parameters(parameters), m_get, "", std::move(callback), "id", "users");
}

void cluster::poll_get_answer_voters(snowflake message_id, snowflake channel_id, uint32_t answer_id, snowflake after, uint64_t limit, command_completion_event_t callback) {
	std::map<std::string, std::string> parameters {
		{"limit", std::to_string(limit > 100 ? 100 : limit)}
	};

	if (after > 0) {
		parameters["after"] = after;
	}
	rest_request_list<user>(this, API_PATH "/channels", std::to_string(channel_id), "polls/" + std::to_string(message_id) + "/answers/" + std::to_string(answer_id) + utility::make_url_parameters(parameters), m_get, "", std::move(callback), "id", "users");
}


void cluster::poll_end(const message &m, command_completion_event_t callback) {
	rest_request<message>(this, API_PATH "/channels", std::to_string(m.channel_id), "polls/" + std::to_string(m.id) + "/expire", m_post, "", std::move(callback));
}

void cluster::poll_end(snowflake message_id, snowflake channel_id, command_completion_event_t callback) {
	rest_request<message>(this, API_PATH "/channels", std::to_string(channel_id), "polls/" + std::to_string(message_id) + "/expire", m_post, "", std::move(callback));
}


void cluster::channel_pins_get(snowflake channel_id, command_completion_event_t callback) {
	rest_request_list<message>(this, API_PATH "/channels", std::to_string(channel_id), "pins", m_get, "", callback);
}

} // namespace dpp
