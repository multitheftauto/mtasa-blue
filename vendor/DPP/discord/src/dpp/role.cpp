/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include <dpp/exception.h>
#include <dpp/role.h>
#include <dpp/cache.h>
#include <dpp/discordevents.h>
#include <dpp/permissions.h>
#include <dpp/stringops.h>
#include <dpp/json.h>



namespace dpp {

using json = nlohmann::json;

/* A mapping of discord's flag values to our bitmap (they're different bit positions to fit other stuff in) */
std::map<uint8_t, dpp::role_flags> rolemap = {
		{ 1 << 0,       dpp::r_in_prompt },
};

std::string role::get_mention(const snowflake& id){
	return utility::role_mention(id);
}

role& role::fill_from_json_impl(nlohmann::json* j)
{
	return fill_from_json(0, j);
}

role& role::fill_from_json(snowflake _guild_id, nlohmann::json* j)
{
	this->guild_id = _guild_id;
	this->name = string_not_null(j, "name");
	if (auto it = j->find("icon"); it != j->end() && !it->is_null())
		this->icon = utility::iconhash{it->get<std::string>()};
	this->unicode_emoji = string_not_null(j, "unicode_emoji");
	this->id = snowflake_not_null(j, "id");
	this->colour = int32_not_null(j, "color");
	this->position = int8_not_null(j, "position");
	this->permissions = snowflake_not_null(j, "permissions");

	uint8_t f = int8_not_null(j, "flags");
	for (auto & flag : rolemap) {
		if (f & flag.first) {
			this->flags |= flag.second;
		}
	}

	this->flags |= bool_not_null(j, "hoist") ? dpp::r_hoist : 0;
	this->flags |= bool_not_null(j, "managed") ? dpp::r_managed : 0;
	this->flags |= bool_not_null(j, "mentionable") ? dpp::r_mentionable : 0;
	if (j->contains("tags")) {
		auto t = (*j)["tags"];
		/* This is broken on the Discord API.
		 * Confirmed 25/11/2021, by quin#3017. If the value exists
		 * as a null, this is the nitro role. If it doesn't exist at all, it is
		 * NOT the nitro role. How obtuse.
		 */
		if (t.find("premium_subscriber") != t.end()) {
			this->flags |= dpp::r_premium_subscriber;
		}
		if (t.find("available_for_purchase") != t.end()) {
			this->flags |= dpp::r_available_for_purchase;
		}
		if (t.find("guild_connections") != t.end()) {
			this->flags |= dpp::r_guild_connections;
		}
		this->bot_id = snowflake_not_null(&t, "bot_id");
		this->integration_id = snowflake_not_null(&t, "integration_id");
		this->subscription_listing_id = snowflake_not_null(&t, "subscription_listing_id");
	}
	return *this;
}

json role::to_json_impl(bool with_id) const {
	json j;

	if (with_id) {
		j["id"] = std::to_string(id);
	}
	if (!name.empty()) {
		j["name"] = name;
	}
	if (colour) {
		j["color"] = colour;
	}
	j["position"] = position;
	j["permissions"] = permissions;
	j["hoist"] = is_hoisted();
	j["mentionable"] = is_mentionable();
	if (icon.is_image_data()) {
		j["icon"] = icon.as_image_data().to_nullable_json();
	}
	if (!unicode_emoji.empty()) {
		j["unicode_emoji"] = unicode_emoji;
	}

	return j;
}

std::string role::get_mention() const {
	return utility::role_mention(id);
}

role& role::load_image(std::string_view image_blob, const image_type type) {
	icon = utility::image_data{type, image_blob};
	return *this;
}

role& role::load_image(const std::byte* data, uint32_t size, const image_type type) {
	icon = utility::image_data{type, data, size};
	return *this;
}


bool role::is_hoisted() const {
	return this->flags & dpp::r_hoist;
}

bool role::is_mentionable() const {
	return this->flags & dpp::r_mentionable;
}

bool role::is_managed() const {
	return this->flags & dpp::r_managed;
}

bool role::is_premium_subscriber() const {
	return this->flags & dpp::r_premium_subscriber;
}

bool role::is_available_for_purchase() const {
	return this->flags & dpp::r_available_for_purchase;
}

bool role::is_linked() const {
	return this->flags & dpp::r_guild_connections;
}

bool role::is_selectable_in_prompt() const {
	return this->flags & dpp::r_in_prompt;
}

bool role::has_create_instant_invite() const {
	return has_administrator() || permissions.has(p_create_instant_invite);
}

bool role::has_kick_members() const {
	return has_administrator() || permissions.has(p_kick_members);
}

bool role::has_ban_members() const {
	return has_administrator() || permissions.has(p_ban_members);
}

bool role::has_administrator() const {
	return permissions.has(p_administrator);
}

bool role::has_manage_channels() const {
	return has_administrator() || permissions.has(p_manage_channels);
}

bool role::has_manage_guild() const {
	return has_administrator() || permissions.has(p_manage_guild);
}

bool role::has_add_reactions() const {
	return has_administrator() || permissions.has(p_add_reactions);
}

bool role::has_view_audit_log() const {
	return has_administrator() || permissions.has(p_view_audit_log);
}

bool role::has_priority_speaker() const {
	return has_administrator() || permissions.has(p_priority_speaker);
}

bool role::has_stream() const {
	return has_administrator() || permissions.has(p_stream);
}

bool role::has_view_channel() const {
	return has_administrator() || permissions.has(p_view_channel);
}

bool role::has_send_messages() const {
	return has_administrator() || permissions.has(p_send_messages);
}

bool role::has_send_tts_messages() const {
	return has_administrator() || permissions.has(p_send_tts_messages);
}

bool role::has_manage_messages() const {
	return has_administrator() || permissions.has(p_manage_messages);
}

bool role::has_embed_links() const {
	return has_administrator() || permissions.has(p_embed_links);
}

bool role::has_attach_files() const {
	return has_administrator() || permissions.has(p_attach_files);
}

bool role::has_read_message_history() const {
	return has_administrator() || permissions.has(p_read_message_history);
}

bool role::has_mention_everyone() const {
	return has_administrator() || permissions.has(p_mention_everyone);
}

bool role::has_use_external_emojis() const {
	return has_administrator() || permissions.has(p_use_external_emojis);
}

bool role::has_view_guild_insights() const {
	return has_administrator() || permissions.has(p_view_guild_insights);
}

bool role::has_connect() const {
	return has_administrator() || permissions.has(p_connect);
}

bool role::has_speak() const {
	return has_administrator() || permissions.has(p_speak);
}

bool role::has_mute_members() const {
	return has_administrator() || permissions.has(p_mute_members);
}

bool role::has_deafen_members() const {
	return has_administrator() || permissions.has(p_deafen_members);
}

bool role::has_move_members() const {
	return has_administrator() || permissions.has(p_move_members);
}

bool role::has_use_vad() const {
	return has_administrator() || permissions.has(p_use_vad);
}

bool role::has_change_nickname() const {
	return has_administrator() || permissions.has(p_change_nickname);
}

bool role::has_manage_nicknames() const {
	return has_administrator() || permissions.has(p_manage_nicknames);
}

bool role::has_manage_roles() const {
	return has_administrator() || permissions.has(p_manage_roles);
}

bool role::has_manage_webhooks() const {
	return has_administrator() || permissions.has(p_manage_webhooks);
}

bool role::has_manage_emojis_and_stickers() const {
	return has_administrator() || permissions.has(p_manage_emojis_and_stickers);
}

bool role::has_use_application_commands() const {
	return has_administrator() || permissions.has(p_use_application_commands);
}

bool role::has_request_to_speak() const {
	return has_administrator() || permissions.has(p_request_to_speak);
}

bool role::has_manage_threads() const {
	return has_administrator() || permissions.has(p_manage_threads);
}

bool role::has_create_public_threads() const {
	return has_administrator() || permissions.has(p_create_public_threads);
}

bool role::has_create_private_threads() const {
	return has_administrator() || permissions.has(p_create_private_threads);
}

bool role::has_use_external_stickers() const {
	return has_administrator() || permissions.has(p_use_external_stickers);
}

bool role::has_send_messages_in_threads() const {
	return has_administrator() || permissions.has(p_send_messages_in_threads);
}

bool role::has_use_embedded_activities() const {
	return has_administrator() || permissions.has(p_use_embedded_activities);
}

bool role::has_manage_events() const {
	return has_administrator() || permissions.has(p_manage_events);
}

bool role::has_moderate_members() const {
	return has_administrator() || permissions.has(p_moderate_members);
}

bool role::has_view_creator_monetization_analytics() const {
	return has_administrator() || permissions.has(p_view_creator_monetization_analytics);
}

bool role::has_use_soundboard() const {
	return has_administrator() || permissions.has(p_use_soundboard);
}

bool role::has_use_external_sounds() const {
	return has_administrator() || permissions.has(p_use_external_sounds);
}

bool role::has_send_voice_messages() const {
	return has_administrator() || permissions.has(p_send_voice_messages);
}

bool role::has_use_clyde_ai() const {
	return has_administrator() || permissions.has(p_use_clyde_ai);
}

role& role::set_name(const std::string& n) {
	name = utility::validate(n, 1, 100, "Role name too short");
	return *this;
}

role& role::set_colour(uint32_t c) {
	colour = c;
	return *this;
}

role& role::set_color(uint32_t c) {
	return set_colour(c);
}

role& role::set_flags(uint8_t f) {
	flags = f;
	return *this;
}

role& role::set_integration_id(snowflake i) {
	integration_id = i;
	return *this;
}

role& role::set_bot_id(snowflake b) {
	bot_id = b;
	return *this;
}

role& role::set_guild_id(snowflake gid) {
	guild_id = gid;
	return *this;
}

members_container role::get_members() const {
	members_container gm;
	guild* g = dpp::find_guild(this->guild_id);
	if (g) {
		if (this->guild_id == this->id) {
			/* Special shortcircuit for everyone-role. Always includes all users. */
			return g->members;
		}
		for (auto & m : g->members) {
			/* Iterate all members and use std::find on their role list to see who has this role */
			const auto& r = m.second.get_roles();
			auto i = std::find(r.begin(), r.end(), this->id);
			if (i != r.end()) {
				gm[m.second.user_id] = m.second;
			}
		}
	}
	return gm;
}

std::string role::get_icon_url(uint16_t size, const image_type format) const {
	if (this->icon.is_iconhash() && this->id) {
		std::string as_str = this->icon.as_iconhash().to_string();

		if (!as_str.empty()) {
			return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp },
				"role-icons/" + std::to_string(this->id) + "/" + as_str,
				format, size);
		}
	}
	return std::string{};
}

application_role_connection_metadata::application_role_connection_metadata() : key(""), name(""), description("") {
}

application_role_connection_metadata &application_role_connection_metadata::fill_from_json_impl(nlohmann::json *j) {
	type = (application_role_connection_metadata_type)int8_not_null(j, "type");
	key = string_not_null(j, "key");
	name = string_not_null(j, "name");
	if (j->contains("name_localizations")) {
		for (auto loc = (*j)["name_localizations"].begin(); loc != (*j)["name_localizations"].end(); ++loc) {
			name_localizations[loc.key()] = loc.value().get<std::string>();
		}
	}
	description = string_not_null(j, "description");
	if (j->contains("description_localizations")) {
		for(auto loc = (*j)["description_localizations"].begin(); loc != (*j)["description_localizations"].end(); ++loc) {
			description_localizations[loc.key()] = loc.value().get<std::string>();
		}
	}
	return *this;
}

json application_role_connection_metadata::to_json_impl(bool with_id) const {
	json j;
	j["type"] = type;
	j["key"] = key;
	j["name"] = name;
	if (!name_localizations.empty()) {
		j["name_localizations"] = json::object();
		for(auto& loc : name_localizations) {
			j["name_localizations"][loc.first] = loc.second;
		}
	}
	j["description"] = description;
	if (!description_localizations.empty()) {
		j["description_localizations"] = json::object();
		for(auto& loc : description_localizations) {
			j["description_localizations"][loc.first] = loc.second;
		}
	}
	return j;
}


application_role_connection::application_role_connection() : platform_name(""), platform_username("") {
}

application_role_connection &application_role_connection::fill_from_json_impl(nlohmann::json *j) {
	platform_name = string_not_null(j, "platform_name");
	platform_username = string_not_null(j, "platform_username");
	metadata = application_role_connection_metadata().fill_from_json(&((*j)["metadata"]));
	return *this;
}

json application_role_connection::to_json_impl(bool with_id) const {
	json j;
	if (!platform_name.empty()) {
		j["platform_name"] = platform_name;
	}
	if (!platform_username.empty()) {
		j["platform_username"] = platform_username;
	}
	if (std::holds_alternative<application_role_connection_metadata>(metadata)) {
		try {
			j["metadata"] = json::parse(std::get<application_role_connection_metadata>(metadata).build_json());
		}
		catch (const std::exception &e) {
			/* Protection against malformed json in metadata */
		}
	}
	return j;
}


} // namespace dpp
