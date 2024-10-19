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
#include <dpp/user.h>
#include <dpp/exception.h>
#include <dpp/restrequest.h>

namespace dpp {

void cluster::current_user_edit(const std::string &nickname, const std::string& image_blob, const image_type type, command_completion_event_t callback) {
	json j = json::parse("{\"nickname\": null}");
	if (!nickname.empty()) {
		j["nickname"] = nickname;
	}
	if (!image_blob.empty()) {
		static const std::map<image_type, std::string> mimetypes = {
			{ i_gif, "image/gif" },
			{ i_jpg, "image/jpeg" },
			{ i_png, "image/png" },
			{ i_webp, "image/webp" },
		};
		if (image_blob.size() > MAX_EMOJI_SIZE) {
			throw dpp::length_exception(err_icon_size, "User icon file exceeds discord limit of 256 kilobytes");
		}
		j["avatar"] = "data:" + mimetypes.find(type)->second + ";base64," + base64_encode((unsigned char const*)image_blob.data(), (unsigned int)image_blob.length());
	}
	rest_request<user>(this, API_PATH "/users", "@me", "", m_patch, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::current_application_get(command_completion_event_t callback) {
	rest_request<application>(this, API_PATH "/oauth2/applications", "@me", "", m_get, "", callback);
}

void cluster::current_user_get(command_completion_event_t callback) {
	rest_request<user_identified>(this, API_PATH "/users", "@me", "", m_get, "", callback);
}

void cluster::current_user_set_voice_state(snowflake guild_id, snowflake channel_id, bool suppress, time_t request_to_speak_timestamp, command_completion_event_t callback) {
	json j({
		{"channel_id", channel_id},
		{"suppress", suppress}
	});
	if (request_to_speak_timestamp) {
		if (request_to_speak_timestamp < time(nullptr)) {
			throw dpp::logic_exception(err_voice_state_timestamp, "Cannot set voice state request to speak timestamp to before current time");
		}
		j["request_to_speak_timestamp"] = ts_to_string(request_to_speak_timestamp);
	} else {
		j["request_to_speak_timestamp"] = json::value_t::null;
	}
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "/voice-states/@me", m_patch, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::user_set_voice_state(snowflake user_id, snowflake guild_id, snowflake channel_id, bool suppress, command_completion_event_t callback) {
	json j({
		{"channel_id", channel_id},
		{"suppress", suppress}
	});
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "/voice-states/" + std::to_string(user_id), m_patch, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}

void cluster::current_user_connections_get(command_completion_event_t callback) {
	rest_request_list<connection>(this, API_PATH "/users", "@me", "connections", m_get, "", callback);
}

void cluster::current_user_get_guilds(command_completion_event_t callback) {
	rest_request_list<guild>(this, API_PATH "/users", "@me", "guilds", m_get, "", callback);
}

void cluster::current_user_leave_guild(snowflake guild_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/users", "@me", "guilds/" + std::to_string(guild_id), m_delete, "", callback);
}

void cluster::user_get(snowflake user_id, command_completion_event_t callback) {
	rest_request<user_identified>(this, API_PATH "/users", std::to_string(user_id), "", m_get, "", callback);
}

void cluster::user_get_cached(snowflake user_id, command_completion_event_t callback) {
	user* u = find_user(user_id);
	if (u) {
		/* We can't simply down-cast to user_identified with dynamic_cast,
		 * this will cause a segmentation fault. We have to re-build the more complex
		 * user_identified from a user, by calling a constructor that builds it from
		 * the user object.
		 */
		confirmation_callback_t cb(this, user_identified(*u), http_request_completion_t());
		callback(cb);
		return;
	}
	/* If the user isn't in the cache, make the API call */
	rest_request<user_identified>(this, API_PATH "/users", std::to_string(user_id), "", m_get, "", callback);
}

} // namespace dpp
