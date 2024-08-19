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
#include <dpp/automod.h>
#include <dpp/restrequest.h>

namespace dpp {

void cluster::automod_rules_get(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<automod_rule>(this, API_PATH "/guilds", std::to_string(guild_id), "/auto-moderation/rules", m_get, "", callback);
}

void cluster::automod_rule_get(snowflake guild_id, snowflake rule_id, command_completion_event_t callback) {
	rest_request<automod_rule>(this, API_PATH "/guilds", std::to_string(guild_id), "/auto-moderation/rules/" + std::to_string(rule_id), m_get, "", callback);
}

void cluster::automod_rule_create(snowflake guild_id, const automod_rule& r, command_completion_event_t callback) {
	rest_request<automod_rule>(this, API_PATH "/guilds", std::to_string(guild_id), "/auto-moderation/rules", m_post, r.build_json(), callback);
}

void cluster::automod_rule_edit(snowflake guild_id, const automod_rule& r, command_completion_event_t callback) {
	rest_request<automod_rule>(this, API_PATH "/guilds", std::to_string(guild_id), "/auto-moderation/rules/" + std::to_string(r.id), m_patch, r.build_json(true), callback);
}

void cluster::automod_rule_delete(snowflake guild_id, snowflake rule_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "/auto-moderation/rules/" + std::to_string(rule_id), m_delete, "", callback);
}

} // namespace dpp
