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
#include <dpp/dtemplate.h>
#include <dpp/restrequest.h>

namespace dpp {

void cluster::guild_create_from_template(const std::string &code, const std::string &name, command_completion_event_t callback) {
	json params({{"name", name}});
	rest_request<guild>(this, API_PATH "/guilds", "templates", code, m_post, params.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}


void cluster::guild_template_create(snowflake guild_id, const std::string &name, const std::string &description, command_completion_event_t callback) {
	json params({{"name", name}, {"description", description}});
	rest_request<dtemplate>(this, API_PATH "/guilds", std::to_string(guild_id), "templates", m_post, params.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}


void cluster::guild_template_delete(snowflake guild_id, const std::string &code, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "templates/" + code, m_delete, "", callback);
}


void cluster::guild_template_modify(snowflake guild_id, const std::string &code, const std::string &name, const std::string &description, command_completion_event_t callback) {
	json params({{"name", name}, {"description", description}});
	rest_request<dtemplate>(this, API_PATH "/guilds", std::to_string(guild_id), "templates/" + code, m_patch, params.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}


void cluster::guild_templates_get(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<dtemplate>(this, API_PATH "/guilds", std::to_string(guild_id), "templates", m_get, "", callback);
}


void cluster::guild_template_sync(snowflake guild_id, const std::string &code, command_completion_event_t callback) {
	rest_request<dtemplate>(this, API_PATH "/guilds", std::to_string(guild_id), "templates/" + code, m_put, "", callback);
}


void cluster::template_get(const std::string &code, command_completion_event_t callback) {
	rest_request<dtemplate>(this, API_PATH "/guilds", "templates", code, m_get, "", callback);
}

} // namespace dpp
