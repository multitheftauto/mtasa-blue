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
#include <dpp/once.h>

namespace dpp {

void cluster::guild_current_member_edit(snowflake guild_id, const std::string &nickname, command_completion_event_t callback) {
	std::string o = (nickname.empty() ? json({{"nick", json::value_t::null }}) : json({{"nick", nickname }})).dump(-1, ' ', false, json::error_handler_t::replace);
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/@me", m_patch, o, callback);
}


void cluster::guild_auditlog_get(snowflake guild_id, snowflake user_id, uint32_t action_type, snowflake before, snowflake after, uint32_t limit, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"user_id", user_id},
		{"action_type", action_type},
		{"before", before},
		{"after", after},
		{"limit", limit},
	});
	rest_request<auditlog>(this, API_PATH "/guilds", std::to_string(guild_id), "audit-logs" + parameters, m_get, "", callback);
}


void cluster::guild_ban_add(snowflake guild_id, snowflake user_id, uint32_t delete_message_seconds, command_completion_event_t callback) {
	json j;
	if (delete_message_seconds) {
		j["delete_message_seconds"] = delete_message_seconds > 604800 ? 604800 : delete_message_seconds;
		if (delete_message_seconds >= 1 && delete_message_seconds <= 7) {
			// this prints out a warning for backwards compatibility
			if (dpp::run_once<struct ban_add_seconds_not_days_t>()) {
				this->log(ll_warning, "It looks like you may have confused seconds and days in cluster::guild_ban_add - Please double check your parameters!");
			}
		}
	}
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "bans/" + std::to_string(user_id), m_put, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}


void cluster::guild_ban_delete(snowflake guild_id, snowflake user_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "bans/" + std::to_string(user_id), m_delete, "", callback);
}


void cluster::guild_create(const class guild &g, command_completion_event_t callback) {
	rest_request<guild>(this, API_PATH "/guilds", "", "", m_post, g.build_json(), callback);
}


void cluster::guild_delete(snowflake guild_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "", m_delete, "", callback);
}


void cluster::guild_delete_integration(snowflake guild_id, snowflake integration_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "integrations/" + std::to_string(integration_id), m_delete, "", callback);
}

void cluster::guild_edit(const class guild &g, command_completion_event_t callback) {
	rest_request<guild>(this, API_PATH "/guilds", std::to_string(g.id), "", m_patch, g.build_json(true), callback);
}


void cluster::guild_edit_widget(snowflake guild_id, const class guild_widget &gw, command_completion_event_t callback) {
	rest_request<guild_widget>(this, API_PATH "/guilds", std::to_string(guild_id), "widget", m_patch, gw.build_json(), callback);
}


void cluster::guild_get_ban(snowflake guild_id, snowflake user_id, command_completion_event_t callback) {
	rest_request<ban>(this, API_PATH "/guilds", std::to_string(guild_id), "bans/" + std::to_string(user_id), m_get, "", callback);
}


void cluster::guild_get_bans(snowflake guild_id, snowflake before, snowflake after, snowflake limit, command_completion_event_t callback) {
	std::string parameters = utility::make_url_parameters({
		{"before", before},
		{"after", after},
		{"limit", limit},
	});
	rest_request_list<ban>(this, API_PATH "/guilds", std::to_string(guild_id), "bans" + parameters, m_get, "", callback);
}


void cluster::guild_get(snowflake guild_id, command_completion_event_t callback) {
	rest_request<guild>(this, API_PATH "/guilds", std::to_string(guild_id), "", m_get, "", callback);
}


void cluster::guild_get_integrations(snowflake guild_id, command_completion_event_t callback) {
	rest_request_list<integration>(this, API_PATH "/guilds", std::to_string(guild_id), "integrations", m_get, "", callback);
}


void cluster::guild_get_preview(snowflake guild_id, command_completion_event_t callback) {
	rest_request<guild>(this, API_PATH "/guilds", std::to_string(guild_id), "preview", m_get, "", callback);
}


void cluster::guild_get_vanity(snowflake guild_id, command_completion_event_t callback) {
	rest_request<invite>(this, API_PATH "/guilds", std::to_string(guild_id), "vanity-url", m_get, "", callback);
}


void cluster::guild_get_widget(snowflake guild_id, command_completion_event_t callback) {
	rest_request<guild_widget>(this, API_PATH "/guilds", std::to_string(guild_id), "widget", m_get, "", callback);
}


void cluster::guild_modify_integration(snowflake guild_id, const class integration &i, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "integrations/" + std::to_string(i.id), m_patch, i.build_json(), callback);
}


void cluster::guild_get_prune_counts(snowflake guild_id, const struct prune& pruneinfo, command_completion_event_t callback) {
	rest_request<prune>(this, API_PATH "/guilds", std::to_string(guild_id), "prune", m_get, pruneinfo.build_json(false), callback);
}

void cluster::guild_begin_prune(snowflake guild_id, const struct prune& pruneinfo, command_completion_event_t callback) {
	rest_request<prune>(this, API_PATH "/guilds", std::to_string(guild_id), "prune", m_post, pruneinfo.build_json(true), callback);
}


void cluster::guild_set_nickname(snowflake guild_id, const std::string &nickname, command_completion_event_t callback) {
	std::string o = (nickname.empty() ? json({{"nick", json::value_t::null }}) : json({{"nick", nickname }})).dump(-1, ' ', false, json::error_handler_t::replace);
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "members/@me/nick", m_patch, o, callback);
}


void cluster::guild_sync_integration(snowflake guild_id, snowflake integration_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/guilds", std::to_string(guild_id), "integrations/" + std::to_string(integration_id), m_post, "", callback);
}


void cluster::guild_get_onboarding(snowflake guild_id, command_completion_event_t callback) {
	rest_request<onboarding>(this, API_PATH "/guilds", std::to_string(guild_id), "onboarding", m_get, "", callback);
}

void cluster::guild_edit_onboarding(const struct onboarding& o, command_completion_event_t callback) {
	rest_request<onboarding>(this, API_PATH "/guilds", std::to_string(o.guild_id), "onboarding", m_put, o.build_json(), callback);
}

void cluster::guild_get_welcome_screen(snowflake guild_id, command_completion_event_t callback) {
	rest_request<dpp::welcome_screen>(this, API_PATH "/guilds", std::to_string(guild_id), "welcome-screen", m_get, "", callback);
}

void cluster::guild_edit_welcome_screen(snowflake guild_id, const struct welcome_screen& welcome_screen, bool enabled, command_completion_event_t callback) {
	json j = welcome_screen.to_json();
	j["enabled"] = enabled;
	rest_request<dpp::welcome_screen>(this, API_PATH "/guilds", std::to_string(guild_id), "welcome-screen", m_patch, j.dump(-1, ' ', false, json::error_handler_t::replace), callback);
}


} // namespace dpp
