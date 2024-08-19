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

void cluster::guild_get_invites(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<invite>(this, API_PATH "/guilds", std::to_string(guild_id), "invites", m_get, "", callback, "code");
}

void cluster::invite_delete(const std::string &invitecode, command_completion_event_t callback) {
	rest_request<invite>(this, API_PATH "/invites", utility::url_encode(invitecode), "", m_delete, "", callback);
}


void cluster::invite_get(const std::string &invite_code, command_completion_event_t callback) {
	rest_request<invite>(this, API_PATH "/invites", utility::url_encode(invite_code) + "?with_counts=true&with_expiration=true", "", m_get, "", callback);
}

} // namespace dpp
