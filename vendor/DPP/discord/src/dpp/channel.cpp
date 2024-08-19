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
#include <dpp/channel.h>
#include <dpp/cache.h>
#include <dpp/guild.h>
#include <dpp/user.h>
#include <dpp/role.h>
#include <dpp/discordevents.h>
#include <dpp/stringops.h>
#include <dpp/json.h>



namespace dpp {

using json = nlohmann::json;

/* A mapping of discord's flag values to our bitmap (they're different bit positions to fit other stuff in) */
enum discord_channel_flags {
	dc_pinned_thread = 1 << 1,
	dc_require_tag =   1 << 4,
	dc_hide_media_download_options = 1 << 15,
};

permission_overwrite::permission_overwrite() : id(0), allow(0), deny(0), type(0) {}

permission_overwrite::permission_overwrite(snowflake id, uint64_t allow, uint64_t deny, overwrite_type type) : id(id), allow(allow), deny(deny), type(type) {}

forum_tag::forum_tag() : managed(), moderated(false) {}

forum_tag::forum_tag(const std::string& name) : forum_tag() {
	this->set_name(name);
}

forum_tag& forum_tag::fill_from_json_impl(nlohmann::json *j) {
	set_snowflake_not_null(j, "id", this->id);
	set_string_not_null(j, "name", this->name);
	set_bool_not_null(j, "moderated", this->moderated);
	auto emoji_id = snowflake_not_null(j, "emoji_id");
	auto emoji_name = string_not_null(j, "emoji_name");
	if (emoji_id) {
		this->emoji = emoji_id;
	} else if (!emoji_name.empty()) {
		this->emoji = emoji_name;
	}
	return *this;
}

json forum_tag::to_json_impl(bool with_id) const {
	json j;
	if (with_id && id) {
		j["id"] = std::to_string(id);
	}
	j["name"] = name;
	j["moderated"] = moderated;
	if (std::holds_alternative<snowflake>(emoji)) {
		j["emoji_id"] = std::get<snowflake>(emoji);
	} else if (std::holds_alternative<std::string>(emoji)) {
		j["emoji_name"] = std::get<std::string>(emoji);
	}
	return j;
}

forum_tag &forum_tag::set_name(const std::string &name) {
	this->name = utility::utf8substr(name, 0, 20);
	return *this;
}

const uint16_t DEFAULT_FORUM_LAYOUT_MASK = 0b0000011000000000;

void to_json(nlohmann::json& j, const permission_overwrite& po) {
	j["id"] = std::to_string(po.id);
	j["allow"] = std::to_string(po.allow);
	j["deny"] = std::to_string(po.deny);
	j["type"] = po.type;
}

channel::channel() :
	managed(),
	owner_id(0),
	parent_id(0),
	guild_id(0),
	last_message_id(0),
	last_pin_timestamp(0),
	permissions(0),
	position(0),
	bitrate(0),
	rate_limit_per_user(0),
	default_thread_rate_limit_per_user(0),
	default_auto_archive_duration(arc_1_day),
	default_sort_order(so_latest_activity),
	flags(0),
	user_limit(0)
{
}

channel::~channel(){
}

std::string channel::get_mention(const snowflake &id) {
	return utility::channel_mention(id);
}

std::string channel::get_mention() const {
	return utility::channel_mention(id);
}

channel& channel::set_name(const std::string& name) {
	this->name = utility::validate(name, 1, 100, "name must be at least 1 character");
	return *this;
}

channel& channel::set_topic(const std::string& topic) {
	this->topic = utility::utf8substr(topic, 0, 1024);
	return *this;
}

channel& channel::set_type(channel_type type) {
	this->flags &= ~CHANNEL_TYPE_MASK;
	this->flags |= type;
	return *this;
}

channel& channel::set_default_forum_layout(forum_layout_type layout_type) {
	this->flags &= ~DEFAULT_FORUM_LAYOUT_MASK;
	auto type = (uint16_t)layout_type;
	this->flags |= ((type << 9) & DEFAULT_FORUM_LAYOUT_MASK);
	return *this;
}

channel &channel::set_default_sort_order(default_forum_sort_order_t sort_order) {
	this->default_sort_order = sort_order;
	return *this;
}

channel& channel::set_guild_id(const snowflake guild_id) {
	this->guild_id = guild_id;
	return *this;
}

channel& channel::set_parent_id(const snowflake parent_id) {
	this->parent_id = parent_id;
	return *this;
}

channel& channel::set_rate_limit_per_user(const uint16_t rate_limit_per_user) {
	this->rate_limit_per_user = rate_limit_per_user;
	return *this;
}

channel& channel::set_position(const uint16_t position) {
	this->position = position;
	return *this;
}

channel& channel::set_bitrate(const uint16_t bitrate) {
	this->bitrate = bitrate;
	return *this;
}

channel& channel::set_flags(const uint16_t flags) {
	this->flags = flags;
	return *this;
}

channel& channel::add_flag(const channel_flags flag) {
	this->flags |= flag;
	return *this;
}

channel& channel::remove_flag(const channel_flags flag) {
	this->flags &= ~flag;
	return *this;
}

channel& channel::set_nsfw(const bool is_nsfw) {
	this->flags = (is_nsfw) ? this->flags | dpp::c_nsfw : this->flags & ~dpp::c_nsfw;;
	return *this;
}

channel& channel::set_lock_permissions(const bool is_lock_permissions) {
	this->flags = (is_lock_permissions) ? this->flags | dpp::c_lock_permissions : this->flags & ~dpp::c_lock_permissions;
	return *this;
}

channel& channel::set_user_limit(const uint8_t user_limit) {
	this->user_limit = user_limit;
	return *this;
}

channel& channel::add_permission_overwrite(const snowflake target, const overwrite_type type, const uint64_t allowed_permissions, const uint64_t denied_permissions) {
	for (auto &o : this->permission_overwrites) {
		if (o.id == target && o.type == type) {
			o.allow.remove(denied_permissions);
			o.allow.add(allowed_permissions);
			o.deny.remove(allowed_permissions);
			o.deny.add(denied_permissions);
			return *this;
		}
	}
	permission_overwrite po {target, allowed_permissions, denied_permissions, type};
	this->permission_overwrites.push_back(po);
	return *this;
}

channel& channel::set_permission_overwrite(const snowflake target, const overwrite_type type, const uint64_t allowed_permissions, const uint64_t denied_permissions) {
	this->remove_permission_overwrite(target, type);
	if (allowed_permissions != 0 || denied_permissions != 0) {
		permission_overwrite po{target, allowed_permissions, denied_permissions, type};
		this->permission_overwrites.push_back(po);
	}
	return *this;
}

channel& channel::remove_permission_overwrite(const dpp::snowflake target, const dpp::overwrite_type type) {
	auto it = this->permission_overwrites.begin();
	while (it != this->permission_overwrites.end()) {
		if (it->id == target && it->type == type) {
			it = this->permission_overwrites.erase(it);
		} else {
			it++;
		}
	}
	return *this;
}

bool channel::is_nsfw() const {
	return flags & dpp::c_nsfw;
}

bool channel::is_locked_permissions() const {
	return flags & dpp::c_lock_permissions;
}

bool channel::is_text_channel() const {
	return (flags & CHANNEL_TYPE_MASK) == CHANNEL_TEXT;
}

bool channel::is_dm() const {
	return (flags & CHANNEL_TYPE_MASK) == DM;
}

bool channel::is_voice_channel() const {
	return (flags & CHANNEL_TYPE_MASK) == CHANNEL_VOICE;
}

bool channel::is_group_dm() const {
	return (flags & CHANNEL_TYPE_MASK) == GROUP_DM;
}

bool channel::is_category() const {
	return (flags & CHANNEL_TYPE_MASK) == CHANNEL_CATEGORY;
}

bool channel::is_forum() const {
	return (flags & CHANNEL_TYPE_MASK) == CHANNEL_FORUM;
}

bool channel::is_media_channel() const {
	return (flags & CHANNEL_TYPE_MASK) == CHANNEL_MEDIA;
}

bool channel::is_stage_channel() const {
	return (flags & CHANNEL_TYPE_MASK) == CHANNEL_STAGE;
}

bool channel::is_news_channel() const {
	return (flags & CHANNEL_TYPE_MASK) == CHANNEL_ANNOUNCEMENT;
}

bool channel::is_store_channel() const {
	return (flags & CHANNEL_TYPE_MASK) == CHANNEL_STORE;
}

bool channel::is_video_auto() const {
	/* Note: c_video_auto has no real flag (its value is 0)
	 * as absence of the 720p FULL quality flag indicates it must be
	 * c_video_auto instead -- discord decided to put what is basically
	 * a bool into two potential values, 1 and 2. hmmm...
	 */
	return !is_video_720p();
}

bool channel::is_video_720p() const {
	return flags & dpp::c_video_quality_720p;
}

bool channel::is_pinned_thread() const {
	return flags & dpp::c_pinned_thread;
}

bool channel::is_tag_required() const {
	return flags & dpp::c_require_tag;
}

bool channel::is_download_options_hidden() const {
	return flags & dpp::c_hide_media_download_options;
}

channel& channel::fill_from_json_impl(json* j) {
	this->id = snowflake_not_null(j, "id");
	set_snowflake_not_null(j, "guild_id", this->guild_id);
	set_int16_not_null(j, "position", this->position);
	set_string_not_null(j, "name", this->name);
	set_string_not_null(j, "topic", this->topic);
	set_snowflake_not_null(j, "last_message_id", this->last_message_id);
	set_int8_not_null(j, "user_limit", this->user_limit);
	set_int16_not_null(j, "rate_limit_per_user", this->rate_limit_per_user);
	set_int16_not_null(j, "default_thread_rate_limit_per_user", this->default_thread_rate_limit_per_user);
	set_snowflake_not_null(j, "owner_id", this->owner_id);
	set_snowflake_not_null(j, "parent_id", this->parent_id);
	this->bitrate = int32_not_null(j, "bitrate")/1000;
	this->flags |= bool_not_null(j, "nsfw") ? dpp::c_nsfw : 0;

	uint16_t arc = int16_not_null(j, "default_auto_archive_duration");
	switch (arc) {
		case 60:
			this->default_auto_archive_duration = arc_1_hour;
			break;
		case 1440:
			this->default_auto_archive_duration = arc_1_day;
			break;
		case 4320:
			this->default_auto_archive_duration = arc_3_days;
			break;
		case 10080:
			this->default_auto_archive_duration = arc_1_week;
			break;
		default:
			break;
	}

	set_object_array_not_null<forum_tag>(j, "available_tags", available_tags);

	if (j->contains("default_reaction_emoji")) {
		auto emoji_id = snowflake_not_null(&(*j)["default_reaction_emoji"], "emoji_id");
		auto emoji_name = string_not_null(&(*j)["default_reaction_emoji"], "emoji_name");
		if (emoji_id) {
			this->default_reaction = emoji_id;
		} else if (!emoji_name.empty()) {
			this->default_reaction = emoji_name;
		}
	}

	this->default_sort_order = (default_forum_sort_order_t)int8_not_null(j, "default_sort_order");

	uint8_t type = int8_not_null(j, "type");
	this->flags |= (type & CHANNEL_TYPE_MASK);

	uint16_t forum_layout = int16_not_null(j, "default_forum_layout");
	this->flags |= ((forum_layout << 9) & DEFAULT_FORUM_LAYOUT_MASK);

	uint8_t dflags = int8_not_null(j, "flags");
	this->flags |= (dflags & dc_pinned_thread) ? dpp::c_pinned_thread : 0;
	this->flags |= (dflags & dc_require_tag) ? dpp::c_require_tag : 0;
	this->flags |= (dflags & dc_hide_media_download_options) ? dpp::c_hide_media_download_options : 0;

	uint8_t vqm = int8_not_null(j, "video_quality_mode");
	if (vqm == 2) {
		/* If this is set to 2, this means full quality 720p video for voice channel.
		 * Undefined, or a value of 1 (the other two possibilities right now) means
		 * video quality AUTO.
		 */
		this->flags |= dpp::c_video_quality_720p;
	}

	if (j->contains("recipients")) {
		recipients = {};
		for (auto & r : (*j)["recipients"]) {
			recipients.push_back(from_string<uint64_t>(r["id"].get<std::string>()));
		}
	}

	if (j->contains("permission_overwrites")) {
		permission_overwrites = {};
		for (auto & overwrite : (*j)["permission_overwrites"]) {
			permission_overwrite po;
			po.id = snowflake_not_null(&overwrite, "id");
			po.allow = snowflake_not_null(&overwrite, "allow");
			po.deny = snowflake_not_null(&overwrite, "deny");
			po.type = int8_not_null(&overwrite, "type");
			permission_overwrites.emplace_back(po);
		}
	}

	/* Note: This is only set when the channel is in the resolved set from an interaction.
	 * When set it contains the invokers permissions on channel. Any other time, contains 0.
	 */
	if (j->contains("permissions")) {
		set_snowflake_not_null(j, "permissions", permissions);
	}

	std::string _icon = string_not_null(j, "icon");

	if (!_icon.empty()) {
		this->icon = _icon;
	}

	set_string_not_null(j, "rtc_region", rtc_region);

	return *this;
}

json channel::to_json_impl(bool with_id) const {
	json j;
	if (with_id && id) {
		j["id"] = std::to_string(id);
	}
	j["guild_id"] = std::to_string(guild_id);
	if (position) {
		j["position"] = position;
	}
	j["name"] = name;
	if (!topic.empty()) {
		j["topic"] = topic;
	}
	if (!permission_overwrites.empty()) {
		j["permission_overwrites"] = json::array();
		for (const auto& po : permission_overwrites) {
			json jpo = po;
			j["permission_overwrites"].push_back(jpo);
		}
	}
	if (rate_limit_per_user) {
		j["rate_limit_per_user"] = rate_limit_per_user;
	}
	if (default_thread_rate_limit_per_user) {
		j["default_thread_rate_limit_per_user"] = default_thread_rate_limit_per_user;
	}
	if (is_voice_channel()) {
		j["user_limit"] = user_limit;
		if (bitrate) {
			j["bitrate"] = bitrate * 1000;
		}
	}
	if (is_forum() || is_media_channel()) {
		uint32_t _flags = (flags & dpp::c_require_tag) ? dc_require_tag : 0;
		if (is_media_channel()) {
			_flags |= (flags & dpp::c_hide_media_download_options) ? dc_hide_media_download_options : 0;
		}
		if (_flags) {
			j["flags"] = _flags;
		}
	}
	if (is_forum()) {
		if (get_default_forum_layout()) {
			j["default_forum_layout"] = get_default_forum_layout();
		}
	}
	j["type"] = (flags & CHANNEL_TYPE_MASK);
	if (!is_dm()) {
		if (parent_id) {
			j["parent_id"] = std::to_string(parent_id);
		}
		j["nsfw"] = is_nsfw();
	}
	switch (default_auto_archive_duration) {
		case arc_1_hour:
			j["default_auto_archive_duration"] = 60;
			break;
		case arc_1_day:
			j["default_auto_archive_duration"] = 1440;
			break;
		case arc_3_days:
			j["default_auto_archive_duration"] = 4320;
			break;
		case arc_1_week:
			j["default_auto_archive_duration"] = 10080;
			break;
	}
	if (!available_tags.empty()) {
		j["available_tags"] = json::array();
		for (const auto &available_tag : this->available_tags) {
			j["available_tags"].push_back(available_tag.to_json());
		}
	}
	if (std::holds_alternative<snowflake>(this->default_reaction)) {
		j["default_reaction_emoji"]["emoji_id"] = std::get<snowflake>(this->default_reaction);
	} else if (std::holds_alternative<std::string>(this->default_reaction)) {
		j["default_reaction_emoji"]["emoji_name"] = std::get<std::string>(this->default_reaction);
	}
	if (default_sort_order) {
		j["default_sort_order"] = default_sort_order;
	}
	if (flags & c_lock_permissions) {
		j["lock_permissions"] = true;
	}

	return j;
}

permission channel::get_user_permissions(const user* user) const {
	if (user == nullptr) {
		return 0;
	}

	guild* g = dpp::find_guild(guild_id);
	if (g == nullptr) {
		return 0;
	}

	return g->permission_overwrites(g->base_permissions(user), user, this);
}

permission channel::get_user_permissions(const guild_member &member) const {

	guild* g = dpp::find_guild(guild_id);
	if (g == nullptr) {
		return 0;
	}

	return g->permission_overwrites(member, *this);
}

std::map<snowflake, guild_member*> channel::get_members() {
	std::map<snowflake, guild_member*> rv;
	guild* g = dpp::find_guild(guild_id);
	if (g) {
		for (auto m = g->members.begin(); m != g->members.end(); ++m) {
			if (g->permission_overwrites(m->second, *this) & p_view_channel) {
				rv[m->second.user_id] = &(m->second);
			}
		}
	}
	return rv;
}

std::map<snowflake, voicestate> channel::get_voice_members() {
	std::map<snowflake, voicestate> rv;
	guild* g = dpp::find_guild(guild_id);
	if (g) {
		for (auto & m : g->voice_members) {
			if (m.second.channel_id == this->id) {
				rv[m.second.user_id] = m.second;
			}
		}
	}
	return rv;
}

std::string channel::get_icon_url(uint16_t size, const image_type format) const {
	if (this->id && !this->icon.to_string().empty()) {
		return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp },
										 "channel-icons/" + std::to_string(this->id) + "/" + this->icon.to_string(),
										 format, size);
	} else {
		return std::string();
	}
}

std::string channel::get_url() const{
	return utility::channel_url(guild_id, id);
}

channel_type channel::get_type() const {
	return static_cast<channel_type>(flags & CHANNEL_TYPE_MASK);
}

forum_layout_type channel::get_default_forum_layout() const {
	return static_cast<forum_layout_type>((flags & DEFAULT_FORUM_LAYOUT_MASK) >> 9);
}


} // namespace dpp
