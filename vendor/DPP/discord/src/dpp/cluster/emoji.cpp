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

void cluster::guild_emoji_create(snowflake guild_id, const class emoji& newemoji, command_completion_event_t callback) {
	rest_request<emoji>(this, API_PATH "/guilds", std::to_string(guild_id), "emojis", m_post, newemoji.build_json(), callback);
}

void cluster::guild_emoji_delete(snowflake guild_id, snowflake emoji_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "emojis/" + std::to_string(emoji_id), m_delete, "", callback);
}

void cluster::guild_emoji_edit(snowflake guild_id, const class emoji& newemoji, command_completion_event_t callback) {
	rest_request<emoji>(this, API_PATH "/guilds", std::to_string(guild_id), "emojis/" + std::to_string(newemoji.id), m_patch, newemoji.build_json(), callback);
}

void cluster::guild_emoji_get(snowflake guild_id, snowflake emoji_id, command_completion_event_t callback) {
	rest_request<emoji>(this, API_PATH "/guilds", std::to_string(guild_id), "emojis/" + std::to_string(emoji_id), m_get, "", callback);
}

void cluster::guild_emojis_get(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<emoji>(this, API_PATH "/guilds", std::to_string(guild_id), "emojis", m_get, "", callback);
}

} // namespace dpp
