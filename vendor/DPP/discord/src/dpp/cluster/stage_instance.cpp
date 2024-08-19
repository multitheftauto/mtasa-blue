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
#include <dpp/stage_instance.h>
#include <dpp/restrequest.h>

namespace dpp {

void cluster::stage_instance_create(const stage_instance& si, command_completion_event_t callback) {
	rest_request<stage_instance>(this, API_PATH "/stage-instances", "", "", m_post, si.build_json(), callback);
}

void cluster::stage_instance_get(const snowflake channel_id, command_completion_event_t callback) {
	rest_request<stage_instance>(this, API_PATH "/stage-instances", std::to_string(channel_id), "", m_get, "", callback);
}

void cluster::stage_instance_edit(const stage_instance& si, command_completion_event_t callback) {
	rest_request<stage_instance>(this, API_PATH "/stage-instances", std::to_string(si.channel_id), "", m_patch, si.build_json(), callback);
}

void cluster::stage_instance_delete(const snowflake channel_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/stage-instances", std::to_string(channel_id), "", m_delete, "", callback);
}

} // namespace dpp
