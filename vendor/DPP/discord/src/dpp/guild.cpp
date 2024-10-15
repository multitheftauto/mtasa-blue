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
#include <dpp/cache.h>
#include <dpp/discordclient.h>
#include <dpp/voicestate.h>
#include <dpp/exception.h>
#include <dpp/guild.h>
#include <dpp/discordevents.h>
#include <dpp/stringops.h>
#include <dpp/json.h>

namespace dpp {

using json = nlohmann::json;

/* A mapping of discord's flag values to our bitmap (they're different bit positions to fit other stuff in) */
std::map<uint16_t , dpp::guild_member_flags> membermap = {
		{ 1 << 0,       dpp::gm_did_rejoin },
		{ 1 << 1,       dpp::gm_completed_onboarding },
		{ 1 << 2,       dpp::gm_bypasses_verification },
		{ 1 << 3,       dpp::gm_started_onboarding },
};

const std::map<std::string, std::variant<dpp::guild_flags, dpp::guild_flags_extra>> featuremap = {
	{"ANIMATED_BANNER", dpp::g_animated_banner },
	{"ANIMATED_ICON", dpp::g_animated_icon },
	{"AUTO_MODERATION", dpp::g_auto_moderation },
	{"BANNER", dpp::g_banner },
	{"COMMUNITY", dpp::g_community },
	{"CREATOR_MONETIZABLE_PROVISIONAL", dpp::g_monetization_enabled },
	{"CREATOR_STORE_PAGE", dpp::g_creator_store_page_enabled },
	{"DEVELOPER_SUPPORT_SERVER", dpp::g_developer_support_server },
	{"DISCOVERABLE", dpp::g_discoverable },
	{"FEATURABLE", dpp::g_featureable },
	{"INVITES_DISABLED", dpp::g_invites_disabled },
	{"INVITE_SPLASH", dpp::g_invite_splash },
	{"MEMBER_VERIFICATION_GATE_ENABLED", dpp::g_member_verification_gate },
	{"MORE_STICKERS", dpp::g_more_stickers },
	{"NEWS", dpp::g_news },
	{"PARTNERED", dpp::g_partnered },
	{"PREVIEW_ENABLED", dpp::g_preview_enabled },
	{"RAID_ALERTS_DISABLED", dpp::g_raid_alerts_disabled },
	{"ROLE_ICONS", dpp::g_role_icons },
	{"ROLE_SUBSCRIPTIONS_AVAILABLE_FOR_PURCHASE", dpp::g_role_subscriptions_available_for_purchase },
	{"ROLE_SUBSCRIPTIONS_ENABLED", dpp::g_role_subscription_enabled },
	{"TICKETED_EVENTS_ENABLED", dpp::g_ticketed_events },
	{"VANITY_URL", dpp::g_vanity_url },
	{"VERIFIED", dpp::g_verified },
	{"VIP_REGIONS", dpp::g_vip_regions },
	{"WELCOME_SCREEN_ENABLED", dpp::g_welcome_screen_enabled },
};

guild::guild() :
	managed(),
	owner_id(0),
	afk_channel_id(0),
	application_id(0),
	system_channel_id(0),
	rules_channel_id(0),
	public_updates_channel_id(0),
	widget_channel_id(0),
	member_count(0),
	flags(0),
	max_presences(0),
	max_members(0),
	flags_extra(0),
	shard_id(0),
	premium_subscription_count(0),
	afk_timeout(afk_off),
	max_video_channel_users(0),
	default_message_notifications(dmn_all),
	premium_tier(tier_none),
	verification_level(ver_none),
	explicit_content_filter(expl_disabled),
	mfa_level(mfa_none),
	nsfw_level(nsfw_default)
{
}


guild_member::guild_member() :
	flags(0),
	guild_id(0),
	user_id(0),
	communication_disabled_until(0),
	joined_at(0),
	premium_since(0)
{
}

std::string guild_member::get_mention() const {
	return "<@" + std::to_string(user_id) + ">";
}

guild_member& guild_member::set_nickname(const std::string& nick) {
	this->nickname = nick;
	this->flags |= gm_nickname_action;
	return *this;
}

guild_member& guild_member::add_role(dpp::snowflake role_id) {
	roles.emplace_back(role_id);
	flags |= gm_roles_action;
	return *this;
}

guild_member& guild_member::remove_role(dpp::snowflake role_id) {
	roles.erase(std::remove(roles.begin(), roles.end(), role_id), roles.end());
	flags |= gm_roles_action;
	return *this;
}

std::string guild_member::get_nickname() const {
	return nickname;
}

const std::vector<dpp::snowflake>& guild_member::get_roles() const {
	return roles;
}


guild_member& guild_member::set_roles(const std::vector<dpp::snowflake> &role_ids) {
	roles = role_ids;
	flags |= gm_roles_action;
	return *this;
}

guild_member& guild_member::set_bypasses_verification(const bool is_bypassing_verification) {
	this->flags = (is_bypassing_verification) ? flags | gm_bypasses_verification : flags & ~gm_bypasses_verification;
	return *this;
}

guild_member& guild_member::set_mute(const bool is_muted) {
	this->flags = (is_muted) ? flags | gm_mute : flags & ~gm_mute;
	this->flags |= gm_voice_action;
	return *this;
}

guild_member& guild_member::set_deaf(const bool is_deafened) {
	this->flags = (is_deafened) ? flags | gm_deaf : flags & ~gm_deaf;
	this->flags |= gm_voice_action;
	return *this;
}

guild_member& guild_member::set_communication_disabled_until(const time_t disabled_timestamp) {
	this->communication_disabled_until = disabled_timestamp;
	return *this;
}

bool guild_member::operator == (guild_member const& other_member) const {
	if ((this->user_id == other_member.user_id && this->user_id.empty()) || (this->guild_id == other_member.guild_id && this->guild_id.empty()))
		return false;
	return this->user_id == other_member.user_id && this->guild_id == other_member.guild_id;
}

guild_member& guild_member::fill_from_json(nlohmann::json* j, snowflake g_id, snowflake u_id) {
	this->guild_id = g_id;
	this->user_id = u_id;
	j->get_to(*this);

	return *this;
}

bool guild_member::is_communication_disabled() const {
	return communication_disabled_until > time(nullptr);
}

void from_json(const nlohmann::json& j, guild_member& gm) {
	set_string_not_null(&j, "nick", gm.nickname);
	set_ts_not_null(&j, "joined_at", gm.joined_at);
	set_ts_not_null(&j, "premium_since", gm.premium_since);
	set_ts_not_null(&j, "communication_disabled_until", gm.communication_disabled_until);
	/* Note: The permissions of the guild member are stored in the resolved set in the interaction event to
	 * reduce storage as they would be mostly empty anyway and only retrieved from interaction events
	 */

	uint16_t flags = int16_not_null(&j, "flags");
	for (auto & flag : membermap) {
		if (flags & flag.first) {
			gm.flags |= flag.second;
		}
	}

	set_snowflake_array_not_null(&j, "roles", gm.roles);

	if (j.contains("avatar") && !j.at("avatar").is_null()) {
		std::string av = string_not_null(&j, "avatar");
		if (av.substr(0, 2) == "a_") {
			gm.flags |= gm_animated_avatar;
		}
		gm.avatar = av;
	}

	gm.flags |= bool_not_null(&j, "deaf") ? gm_deaf : 0;
	gm.flags |= bool_not_null(&j, "mute") ? gm_mute : 0;
	gm.flags |= bool_not_null(&j, "pending") ? gm_pending : 0;
}

std::string guild_member::get_avatar_url(uint16_t size, const image_type format, bool prefer_animated) const {
	if (this->guild_id && this->user_id && !this->avatar.to_string().empty()) {
		return utility::cdn_endpoint_url_hash({ i_jpg, i_png, i_webp, i_gif },
			"guilds/" + std::to_string(this->guild_id) + "/" + std::to_string(this->user_id), this->avatar.to_string(),
			format, size, prefer_animated, has_animated_guild_avatar());
	} else {
		return std::string();
	}
}


bool guild_member::has_animated_guild_avatar() const {
	return this->flags & gm_animated_avatar;
}

json guild_member::to_json_impl(bool with_id) const {
	json j;
	if (this->communication_disabled_until > 0) {
		if (this->communication_disabled_until > std::time(nullptr)) {
			j["communication_disabled_until"] = ts_to_string(this->communication_disabled_until);
		} else {
			j["communication_disabled_until"] = json::value_t::null;
		}
	}

	if (this->flags & gm_nickname_action) {
		if (!this->nickname.empty()) {
			j["nick"] = this->nickname;
		} else {
			j["nick"] = json::value_t::null;
		}
	}

	if (this->flags & gm_roles_action) {
		j["roles"] = {};
		for (const auto & role : this->roles) {
			j["roles"].push_back(std::to_string(role));
		}
	}

	if (this->flags & gm_voice_action) {
		j["mute"] = is_muted();
		j["deaf"] = is_deaf();
	}

	return j;
}

guild& guild::set_name(const std::string& n) {
	this->name = utility::validate(trim(n), 2, 100, "Guild names cannot be less than 2 characters");
	return *this;
}

guild &guild::remove_banner() {
	this->banner = utility::image_data{};
	return *this;
}

guild& guild::set_banner(image_type format, std::string_view data) {
	this->banner = utility::image_data{format, data};
	return *this;
}

guild& guild::set_banner(image_type format, const std::byte* data, uint32_t size) {
	this->banner = utility::image_data{format, data, size};
	return *this;
}

guild &guild::remove_discovery_splash() {
	this->discovery_splash = utility::image_data{};
	return *this;
}

guild& guild::set_discovery_splash(image_type format, std::string_view data) {
	this->discovery_splash = utility::image_data{format, data};
	return *this;
}

guild& guild::set_discovery_splash(image_type format, const std::byte* data, uint32_t size) {
	this->discovery_splash = utility::image_data{format, data, size};
	return *this;
}

guild &guild::remove_splash() {
	this->splash = utility::image_data{};
	return *this;
}

guild& guild::set_splash(image_type format, std::string_view data) {
	this->splash = utility::image_data{format, data};
	return *this;
}

guild& guild::set_splash(image_type format, const std::byte* data, uint32_t size) {
	this->splash = utility::image_data{format, data, size};
	return *this;
}

guild &guild::remove_icon() {
	this->icon = utility::image_data{};
	return *this;
}

guild& guild::set_icon(image_type format, std::string_view data) {
	this->icon = utility::image_data{format, data};
	return *this;
}

guild& guild::set_icon(image_type format, const std::byte* data, uint32_t size) {
	this->icon = utility::image_data{format, data, size};
	return *this;
}

user* guild_member::get_user() const {
	return find_user(user_id);
}

bool guild_member::is_deaf() const {
	return flags & dpp::gm_deaf;
}

bool guild_member::is_muted() const {
	return flags & dpp::gm_mute;
}

bool guild_member::is_pending() const {
	return flags & dpp::gm_pending;
}

bool guild_member::has_rejoined() const {
	return flags & dpp::gm_did_rejoin;
}

bool guild_member::is_guild_owner() const {
	auto* _guild = find_guild(guild_id);
	return _guild != nullptr && _guild->owner_id == this->user_id;
}

bool guild_member::has_completed_onboarding() const {
	return flags & dpp::gm_completed_onboarding;
}

bool guild_member::has_started_onboarding() const {
	return flags & dpp::gm_started_onboarding;
}

bool guild_member::has_bypasses_verification() const {
	return flags & dpp::gm_bypasses_verification;
}


welcome_channel::welcome_channel(): channel_id(0), emoji_id(0) {
}

welcome_channel &welcome_channel::fill_from_json_impl(nlohmann::json *j) {
	channel_id = snowflake_not_null(j, "channel_id");
	description = string_not_null(j, "channel_id");
	emoji_id = snowflake_not_null(j, "emoji_id");
	emoji_name = string_not_null(j, "emoji_name");
	return *this;
}

json welcome_channel::to_json_impl(bool with_id) const {
	json j;
	j["channel_id"] = std::to_string(channel_id);
	j["description"] = description;
	if (!emoji_id.empty()) {
		j["emoji_id"] = std::to_string(emoji_id);
	}
	if (!emoji_name.empty()) {
		j["emoji_name"] = emoji_name;
	}
	return j;
}

welcome_channel &welcome_channel::set_channel_id(const snowflake _channel_id) {
	this->channel_id = _channel_id;
	return *this;
}

welcome_channel &welcome_channel::set_description(const std::string &_description) {
	this->description = _description;
	return *this;
}

welcome_screen &welcome_screen::fill_from_json_impl(nlohmann::json *j) {
	description = string_not_null(j, "description");

	set_object_array_not_null<welcome_channel>(j, "welcome_channels", welcome_channels);
	return *this;
}

json welcome_screen::to_json_impl(bool with_id) const {
	json j;
	if (!description.empty()) {
		j["description"] = description;
	}

	if (!welcome_channels.empty()) {
		j["welcome_channels"] = json::array();
		for (const auto &welcome_channel : welcome_channels) {
			j["welcome_channels"].push_back(welcome_channel.to_json());
		}
	}

	return j;
}

welcome_screen &welcome_screen::set_description(const std::string &s){
	this->description = s;
	return *this;
}

bool guild::is_large() const {
	return this->flags & g_large;
}

bool guild::is_unavailable() const {
	return this->flags & g_unavailable;
}

bool guild::widget_enabled() const {
	return this->flags & g_widget_enabled;
}

bool guild::has_invite_splash() const {
	return this->flags & g_invite_splash;
}

bool guild::has_vip_regions() const {
	return this->flags & g_vip_regions;
}

bool guild::has_vanity_url() const {
	return this->flags & g_vanity_url;
}

bool guild::has_premium_progress_bar_enabled() const {
	return this->flags_extra & g_premium_progress_bar_enabled;
}

bool guild::has_invites_disabled() const {
	return this->flags_extra & g_invites_disabled;
}

bool guild::has_channel_banners() const {
	return this->flags & g_channel_banners;
}

bool guild::is_verified() const {
	return this->flags & g_verified;
}

bool guild::is_partnered() const {
	return this->flags & g_partnered;
}

bool guild::is_community() const {
	return this->flags & g_community;
}

bool guild::has_role_subscriptions() const {
	return this->flags & g_role_subscription_enabled;
}

bool guild::has_news() const {
	return this->flags & g_news;
}

bool guild::is_discoverable() const {
	return this->flags & g_discoverable;
}

bool guild::is_featureable() const {
	return this->flags & g_featureable;
}

bool guild::has_animated_banner() const {
	return this->flags_extra & g_animated_banner;
}

bool guild::has_auto_moderation() const {
	return this->flags_extra & g_auto_moderation;
}

bool guild::has_support_server() const {
	return this->flags_extra & g_developer_support_server;
}

bool guild::has_role_subscriptions_available_for_purchase() const {
	return this->flags_extra & g_role_subscriptions_available_for_purchase;
}

bool guild::has_raid_alerts_disabled() const {
	return this->flags_extra & g_raid_alerts_disabled;
}

bool guild::has_animated_icon() const {
	return this->flags & g_animated_icon;
}

bool guild::has_banner() const {
	return this->flags & g_banner;
}

bool guild::is_welcome_screen_enabled() const {
	return this->flags & g_welcome_screen_enabled;
}

bool guild::has_member_verification_gate() const {
	return this->flags & g_member_verification_gate;
}

bool guild::is_preview_enabled() const {
	return this->flags & g_preview_enabled;
}

bool guild::has_animated_icon_hash() const {
	return this->flags & g_has_animated_icon;
}

bool guild::has_animated_banner_hash() const {
	return this->flags & g_has_animated_banner;
}

bool guild::has_monetization_enabled() const {
	return this->flags & g_monetization_enabled;
}

bool guild::has_more_stickers() const {
	return this->flags & g_more_stickers;
}

bool guild::has_creator_store_page() const {
	return this->flags & g_creator_store_page_enabled;
}

bool guild::has_role_icons() const {
	return this->flags & g_role_icons;
}

bool guild::has_seven_day_thread_archive() const {
	return this->flags & g_seven_day_thread_archive;
}

bool guild::has_three_day_thread_archive() const {
	return this->flags & g_three_day_thread_archive;
}

bool guild::has_ticketed_events() const {
	return this->flags & g_ticketed_events;
}

json guild::to_json_impl(bool with_id) const {
	json j;
	if (with_id) {
		j["id"] = std::to_string(id);
	}
	if (!name.empty()) {
		j["name"] = name;
	}
	j["widget_enabled"] = widget_enabled();
	if (afk_channel_id) {
		j["afk_channel_id"] = afk_channel_id;
	}
	if (afk_timeout) {
		if (afk_timeout == afk_60) {
			j["afk_timeout"] = 60;
		} else if (afk_timeout == afk_300) {
			j["afk_timeout"] = 300;
		} else if (afk_timeout == afk_900) {
			j["afk_timeout"] = 900;
		} else if (afk_timeout == afk_1800) {
			j["afk_timeout"] = 1800;
		} else if (afk_timeout == afk_3600) {
			j["afk_timeout"] = 3600;
		}
	}
	if (widget_enabled()) {
		j["widget_channel_id"] = widget_channel_id;
	}
	j["default_message_notifications"] = default_message_notifications;
	j["explicit_content_filter"] = explicit_content_filter;
	j["mfa_level"] = mfa_level;
	if (system_channel_id) {
		j["system_channel_id"] = system_channel_id;
	}
	j["premium_progress_bar_enabled"] = (bool)(flags_extra & g_premium_progress_bar_enabled);
	if (rules_channel_id) {
		j["rules_channel_id"] = rules_channel_id;
	}
	if (!vanity_url_code.empty()) {
		j["vanity_url_code"] = vanity_url_code;
	}
	if (!description.empty()) {
		j["description"] = description;
	}
	if (!safety_alerts_channel_id.empty()) {
		j["safety_alerts_channel_id"] = safety_alerts_channel_id;
	}
	if (banner.is_image_data()) {
		j["banner"] = banner.as_image_data().to_nullable_json();
	}
	if (discovery_splash.is_image_data()) {
		j["discovery_splash"] = discovery_splash.as_image_data().to_nullable_json();
	}
	if (splash.is_image_data()) {
		j["splash"] = splash.as_image_data().to_nullable_json();
	}
	if (icon.is_image_data()) {
		j["icon"] = icon.as_image_data().to_nullable_json();
	}
	return j;
}

void guild::rehash_members() {
	members_container n;
	n.reserve(members.size());
	for (auto t = members.begin(); t != members.end(); ++t) {
		n.insert(*t);
	}
	members = n;
}

guild& guild::fill_from_json_impl(nlohmann::json* d) {
	return fill_from_json(nullptr, d);
}


guild& guild::fill_from_json(discord_client* shard, nlohmann::json* d) {
	/* NOTE: This can be called by GUILD_UPDATE and also GUILD_CREATE.
	 * GUILD_UPDATE sends a partial guild object, so we use Set*NotNull functions
	 * for a lot of the values under the assumption they may sometimes be missing.
	 */
	this->id = snowflake_not_null(d, "id");
	if (d->find("unavailable") == d->end() || (*d)["unavailable"].get<bool>() == false) {
		/* Clear unavailable flag */
		this->flags &= ~dpp::g_unavailable;
		set_string_not_null(d, "name", this->name);
		/* Special case for guild icon to allow for animated icons.
		 * Animated icons start with a_ on the name, so we use this to set a flag
		 * in the flags field and then just store the iconhash separately.
		 */
		std::string _icon = string_not_null(d, "icon");
		if (!_icon.empty()) {
			if (_icon.length() > 2 && _icon.substr(0, 2) == "a_") {
				_icon = _icon.substr(2, _icon.length());
				this->flags |= g_has_animated_icon;
			}
			this->icon = _icon;
		}
		std::string _dsplash = string_not_null(d, "discovery_splash");
		if (!_dsplash.empty()) {
			this->discovery_splash = _dsplash;
		}
		set_snowflake_not_null(d, "owner_id", this->owner_id);

		this->flags |= bool_not_null(d, "large") ? dpp::g_large : 0;
		this->flags |= bool_not_null(d, "widget_enabled") ? dpp::g_widget_enabled : 0;

		this->flags_extra |= bool_not_null(d, "premium_progress_bar_enabled") ? dpp::g_premium_progress_bar_enabled : 0;

		for (auto & feature : (*d)["features"]) {
			auto f = featuremap.find(feature.get<std::string>());
			if (f != featuremap.end()) {
				if (std::holds_alternative<guild_flags_extra>(f->second)) {
					this->flags_extra |= std::get<guild_flags_extra>(f->second);
				} else {
					this->flags |= std::get<guild_flags>(f->second);
				}
			}
		}
		uint8_t scf = int8_not_null(d, "system_channel_flags");
		if (scf & (1 << 0)) {
			this->flags |= dpp::g_no_join_notifications;
		}
		if (scf & (1 << 1)) {
			this->flags |= dpp::g_no_boost_notifications;
		}
		if (scf & (1 << 2)) {
			this->flags |= dpp::g_no_setup_tips;
		}
		if (scf & (1 << 3)) {
			this->flags |= dpp::g_no_sticker_greeting;
		}
		if (scf & (1 << 4)) {
			this->flags |= dpp::g_no_role_subscription_notifications;
		}
		if (scf & (1 << 5)) {
			this->flags |= dpp::g_no_role_subscription_notification_replies;
		}

		if (d->contains("afk_timeout")) {
			if ((*d)["afk_timeout"] == 60) {
				this->afk_timeout = afk_60;
			} else if ((*d)["afk_timeout"] == 300) {
				this->afk_timeout = afk_300;
			} else if ((*d)["afk_timeout"] == 900) {
				this->afk_timeout = afk_900;
			} else if ((*d)["afk_timeout"] == 1800) {
				this->afk_timeout = afk_1800;
			} else if ((*d)["afk_timeout"] == 3600) {
				this->afk_timeout = afk_3600;
			}
		}
		set_snowflake_not_null(d, "afk_channel_id", this->afk_channel_id);
		set_snowflake_not_null(d, "widget_channel_id", this->widget_channel_id);
		this->verification_level = (verification_level_t)int8_not_null(d, "verification_level");
		this->default_message_notifications = (default_message_notification_t)int8_not_null(d, "default_message_notifications");
		this->explicit_content_filter = (guild_explicit_content_t)int8_not_null(d, "explicit_content_filter");
		this->mfa_level = (mfa_level_t)int8_not_null(d, "mfa_level");
		set_snowflake_not_null(d, "application_id", this->application_id);
		set_snowflake_not_null(d, "system_channel_id", this->system_channel_id);
		set_snowflake_not_null(d, "rules_channel_id", this->rules_channel_id);
		set_int32_not_null(d, "member_count", this->member_count);
		set_string_not_null(d, "vanity_url_code", this->vanity_url_code);
		set_string_not_null(d, "description", this->description);
		if (d->find("voice_states") != d->end()) {
			this->voice_members.clear();
			for (auto & vm : (*d)["voice_states"]) {
				voicestate vs;
				vs.fill_from_json(&vm);
				vs.shard = shard;
				vs.guild_id = this->id;
				this->voice_members[vs.user_id] = vs;
			}
		}

		std::string _banner = string_not_null(d, "banner");
		if (!_banner.empty()) {
			if (_banner.length() > 2 && _banner.substr(0, 2) == "a_") {
				this->flags |= dpp::g_has_animated_banner;
			}
			this->banner = _banner;
		}
		this->premium_tier = (guild_premium_tier_t)int8_not_null(d, "premium_tier");
		set_int16_not_null(d, "premium_subscription_count", this->premium_subscription_count);
		set_snowflake_not_null(d, "public_updates_channel_id", this->public_updates_channel_id);
		set_int8_not_null(d, "max_video_channel_users", this->max_video_channel_users);

		set_int32_not_null(d, "max_presences", this->max_presences);
		set_int32_not_null(d, "max_members", this->max_members);

		this->nsfw_level = (guild_nsfw_level_t)int8_not_null(d, "nsfw_level");

		if (d->find("welcome_screen") != d->end()) {
			this->welcome_screen = dpp::welcome_screen().fill_from_json(&d->at("welcome_screen"));
		}

		set_snowflake_not_null(d, "safety_alerts_channel_id", this->safety_alerts_channel_id);
	} else {
		this->flags |= dpp::g_unavailable;
	}
	return *this;
}

guild_widget::guild_widget() : channel_id(0), enabled(false)
{
}

guild_widget& guild_widget::fill_from_json_impl(nlohmann::json* j) {
	enabled = bool_not_null(j, "enabled");
	channel_id = snowflake_not_null(j, "channel_id");
	return *this;
}

json guild_widget::to_json_impl(bool with_id) const {
	return json({{"channel_id", channel_id}, {"enabled", enabled}}).dump(-1, ' ', false, json::error_handler_t::replace);
}


permission guild::base_permissions(const user* user) const {
	if (user == nullptr) {
		return 0;
	}

	auto mi = members.find(user->id);
	if (mi == members.end()) {
		return 0;
	}
	guild_member gm = mi->second;

	return base_permissions(gm);
}

permission guild::base_permissions(const guild_member &member) const {

	/* this method is written with the help of discord's pseudocode available here https://discord.com/developers/docs/topics/permissions#permission-overwrites */

	if (owner_id == member.user_id) {
		return ~0; // return all permissions if it's the owner of the guild
	}

	role* everyone = dpp::find_role(id);
	if (everyone == nullptr) {
		return 0;
	}

	permission permissions = everyone->permissions;

	for (auto& rid : member.get_roles()) {
		role* r = dpp::find_role(rid);
		if (r) {
			permissions |= r->permissions;
		}
	}

	if (permissions & p_administrator) {
		return ~0;
	}

	return permissions;
}

permission guild::permission_overwrites(const uint64_t base_permissions, const user* user, const channel* channel) const {
	if (user == nullptr || channel == nullptr) {
		return 0;
	}

	/* this method is written with the help of discord's pseudocode available here https://discord.com/developers/docs/topics/permissions#permission-overwrites */

	// ADMINISTRATOR overrides any potential permission overwrites, so there is nothing to do here.
	if (base_permissions & p_administrator) {
		return ~0;
	}

	permission permissions = base_permissions;

	// find \@everyone role overwrite and apply it.
	for (auto it = channel->permission_overwrites.begin(); it != channel->permission_overwrites.end(); ++it) {
		if (it->id == this->id && it->type == ot_role) {
			permissions &= ~it->deny;
			permissions |= it->allow;
			break;
		}
	}

	auto mi = members.find(user->id);
	if (mi == members.end()) {
		return 0;
	}
	guild_member gm = mi->second;

	// Apply role specific overwrites.
	uint64_t allow = 0;
	uint64_t deny = 0;

	for (auto& rid : gm.get_roles()) {

		/* Skip \@everyone role to not break the hierarchy. It's calculated above */
		if (rid == this->id) {
			continue;
		}

		for (auto it = channel->permission_overwrites.begin(); it != channel->permission_overwrites.end(); ++it) {
			if (rid == it->id && it->type == ot_role) {
				deny |= it->deny;
				allow |= it->allow;
				break;
			}
		}
	}

	permissions &= ~deny;
	permissions |= allow;

	// Apply member specific overwrite if exists.
	for (auto it = channel->permission_overwrites.begin(); it != channel->permission_overwrites.end(); ++it) {
		if (gm.user_id == it->id && it->type == ot_member) {
			permissions &= ~it->deny;
			permissions |= it->allow;
			break;
		}
	}

	return permissions;
}

permission guild::permission_overwrites(const guild_member &member, const channel &channel) const {

	permission base_permissions = this->base_permissions(member);

	/* this method is written with the help of discord's pseudocode available here https://discord.com/developers/docs/topics/permissions#permission-overwrites */

	// ADMINISTRATOR overrides any potential permission overwrites, so there is nothing to do here.
	if (base_permissions & p_administrator)
		return ~0;

	permission permissions = base_permissions;

	// find \@everyone role overwrite and apply it.
	for (auto it = channel.permission_overwrites.begin(); it != channel.permission_overwrites.end(); ++it) {
		if (it->id == this->id && it->type == ot_role) {
			permissions &= ~it->deny;
			permissions |= it->allow;
			break;
		}
	}

	// Apply role specific overwrites.
	uint64_t allow = 0;
	uint64_t deny = 0;

	for (auto& rid : member.get_roles()) {

		/* Skip \@everyone role to not break the hierarchy. It's calculated above */
		if (rid == this->id) {
			continue;
		}

		for (auto it = channel.permission_overwrites.begin(); it != channel.permission_overwrites.end(); ++it) {
			if (rid == it->id && it->type == ot_role) {
				deny |= it->deny;
				allow |= it->allow;
				break;
			}
		}
	}

	permissions &= ~deny;
	permissions |= allow;

	// Apply member specific overwrite if exists.
	for (auto it = channel.permission_overwrites.begin(); it != channel.permission_overwrites.end(); ++it) {
		if (member.user_id == it->id && it->type == ot_member) {
			permissions &= ~it->deny;
			permissions |= it->allow;
			break;
		}
	}

	return permissions;
}

bool guild::connect_member_voice(snowflake user_id, bool self_mute, bool self_deaf) {
	for (auto & c : channels) {
		channel* ch = dpp::find_channel(c);
		if (!ch || (!ch->is_voice_channel() && !ch->is_stage_channel())) {
			continue;
		}
		auto vcmembers = ch->get_voice_members();
		auto vsi = vcmembers.find(user_id);
		if (vsi != vcmembers.end()) {
			if (vsi->second.shard) {
				vsi->second.shard->connect_voice(this->id, vsi->second.channel_id, self_mute, self_deaf);
				return true;
			}
		}
	}
	return false;
}

std::string guild::get_banner_url(uint16_t size, const image_type format, bool prefer_animated) const {
	if (this->banner.is_iconhash() && this->id) {
		std::string as_str = this->banner.as_iconhash().to_string();

		if (!as_str.empty()) {
			return utility::cdn_endpoint_url_hash({ i_jpg, i_png, i_webp, i_gif },
				"banners/" + std::to_string(this->id), as_str,
				format, size, prefer_animated, has_animated_banner_hash());
		}
	}
	return std::string{};
}

std::string guild::get_discovery_splash_url(uint16_t size, const image_type format) const {
	if (this->discovery_splash.is_iconhash() && this->id) {
		std::string as_str = this->discovery_splash.as_iconhash().to_string();

		if (!as_str.empty()) {
			return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp },
				"discovery-splashes/" + std::to_string(this->id) + "/" + as_str,
				format, size);
		}
	}
	return std::string{};
}

std::string guild::get_icon_url(uint16_t size, const image_type format, bool prefer_animated) const {
	if (this->icon.is_iconhash() && this->id) {
		std::string as_str = this->icon.as_iconhash().to_string();

		if (!as_str.empty()) {
			return utility::cdn_endpoint_url_hash({ i_jpg, i_png, i_webp, i_gif },
				"icons/" + std::to_string(this->id), as_str,
				format, size, prefer_animated, has_animated_icon_hash());
		}
	}
	return std::string{};
}

std::string guild::get_splash_url(uint16_t size, const image_type format) const {
	if (this->splash.is_iconhash() && this->id) {
		std::string as_str = this->splash.as_iconhash().to_string();

		if (!as_str.empty()) {
			return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp, i_gif },
				"splashes/" + std::to_string(this->id) + "/" + as_str,
				format, size);
		}
	}
	return std::string{};
}

guild_member find_guild_member(const snowflake guild_id, const snowflake user_id) {
	guild* g = find_guild(guild_id);
	if (g) {
		auto gm = g->members.find(user_id);
		if (gm != g->members.end()) {
			return gm->second;
		}

		throw dpp::cache_exception(err_cache, "Requested member not found in the guild cache!");
	}
	throw dpp::cache_exception(err_cache, "Requested guild cache not found!");
}


onboarding_prompt_option::onboarding_prompt_option(): managed(0) {
}

onboarding_prompt::onboarding_prompt(): managed(0), type(opt_multiple_choice), flags(0) {
}

onboarding::onboarding(): guild_id(0), mode(gom_default), enabled(false) {
}

onboarding_prompt_option &onboarding_prompt_option::fill_from_json_impl(nlohmann::json *j) {
	this->id = snowflake_not_null(j, "id");
	if (j->contains("emoji")) {
		this->emoji = dpp::emoji().fill_from_json(&j->at("emoji"));
	}
	this->title = string_not_null(j, "title");
	this->description = string_not_null(j, "description");

	set_snowflake_array_not_null(j, "channel_ids", channel_ids);
	set_snowflake_array_not_null(j, "role_ids", role_ids);
	return *this;
}

json onboarding_prompt_option::to_json_impl(bool with_id) const {
	json j;
	j["emoji"] = emoji.to_json();
	j["title"] = title;
	if (!description.empty()) {
		j["description"] = description;
	}

	if (!channel_ids.empty()) {
		j["channel_ids"] = json::array();
		for (const auto &channel_id : channel_ids) {
			j["channel_ids"].push_back(std::to_string(channel_id));
		}
	}

	if (!role_ids.empty()) {
		j["role_ids"] = json::array();
		for (const auto &role_id : role_ids) {
			j["role_ids"].push_back(std::to_string(role_id));
		}
	}

	return j;
}

onboarding_prompt_option &onboarding_prompt_option::set_emoji(const dpp::emoji &_emoji) {
	this->emoji = _emoji;
	return *this;
}

onboarding_prompt_option &onboarding_prompt_option::set_title(const std::string &_title) {
	this->title = _title;
	return *this;
}

onboarding_prompt_option &onboarding_prompt_option::set_description(const std::string &_description) {
	this->description = _description;
	return *this;
}

onboarding_prompt &onboarding_prompt::fill_from_json_impl(nlohmann::json *j) {
	id = snowflake_not_null(j, "id");
	type = static_cast<onboarding_prompt_type>(int8_not_null(j, "type"));
	title = string_not_null(j, "title");

	set_object_array_not_null<onboarding_prompt_option>(j, "options", options);

	flags |= bool_not_null(j, "single_select") ? opf_single_select : 0;
	flags |= bool_not_null(j, "required") ? opf_required : 0;
	flags |= bool_not_null(j, "in_onboarding") ? opf_in_onboarding : 0;
	return *this;
}

json onboarding_prompt::to_json_impl(bool with_id) const {
	json j;
	j["type"] = type;
	j["title"] = title;

	if (!options.empty()) {
		j["options"] = json::array();
		for (auto const &option : options) {
			j["options"].push_back(option.to_json());
		}
	}

	j["single_select"] = is_single_select();
	j["required"] = is_required();
	j["in_onboarding"] = is_in_onboarding();
	return j;
}

bool onboarding_prompt::is_single_select() const {
	return flags & dpp::opf_single_select;
}

bool onboarding_prompt::is_required() const {
	return flags & dpp::opf_required;
}

bool onboarding_prompt::is_in_onboarding() const {
	return flags & dpp::opf_in_onboarding;
}

onboarding_prompt &onboarding_prompt::set_type(const onboarding_prompt_type _type) {
	this->type = _type;
	return *this;
}

onboarding_prompt &onboarding_prompt::set_title(const std::string& _title) {
	this->title = _title;
	return *this;
}

onboarding& onboarding::fill_from_json_impl(nlohmann::json* j) {
	guild_id = snowflake_not_null(j, "guild_id");
	enabled = bool_not_null(j, "enabled");
	mode = static_cast<onboarding_mode>(int8_not_null(j, "mode"));

	set_object_array_not_null<onboarding_prompt>(j, "prompts", prompts);
	set_snowflake_array_not_null(j, "default_channel_ids", default_channel_ids);

	return *this;
}

json onboarding::to_json_impl(bool with_id) const {
	json j;

	if (!prompts.empty()) {
		j["prompts"] = json::array();
		for (auto const &prompt : prompts) {
			j["prompts"].push_back(prompt.to_json());
		}
	}

	if (!default_channel_ids.empty()) {
		j["default_channel_ids"] = json::array();
		for (auto &default_channel_id : default_channel_ids) {
			j["default_channel_ids"].push_back(std::to_string(default_channel_id));
		}
	}

	j["enabled"] = enabled;
	j["mode"] = mode;
	return j;
}

onboarding &onboarding::set_guild_id(const snowflake id) {
	this->guild_id = id;
	return *this;
}

onboarding &onboarding::set_mode(const onboarding_mode m) {
	this->mode = m;
	return *this;
}

onboarding &onboarding::set_enabled(const bool is_enabled) {
	this->enabled = is_enabled;
	return *this;
}


} // namespace dpp
