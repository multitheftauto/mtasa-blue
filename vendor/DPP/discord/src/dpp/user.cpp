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
#include <dpp/discordevents.h>
#include <dpp/json.h>
#include <dpp/stringops.h>

namespace dpp {
using json = nlohmann::
json;

/* A mapping of discord's flag values to our bitmap (they're different bit positions to fit other stuff in) */
std::map<uint32_t, dpp::user_flags> usermap = {
	{ 1 << 0,       dpp::u_discord_employee },
	{ 1 << 1,       dpp::u_partnered_owner },
	{ 1 << 2,       dpp::u_hypesquad_events },
	{ 1 << 3,       dpp::u_bughunter_1 },
	{ 1 << 6,       dpp::u_house_bravery },
	{ 1 << 7,       dpp::u_house_brilliance },
	{ 1 << 8,       dpp::u_house_balance },
	{ 1 << 9,       dpp::u_early_supporter },
	{ 1 << 10,      dpp::u_team_user },
	{ 1 << 14,      dpp::u_bughunter_2 },
	{ 1 << 16,      dpp::u_verified_bot },
	{ 1 << 17,      dpp::u_verified_bot_dev },
	{ 1 << 18,      dpp::u_certified_moderator },
	{ 1 << 19,      dpp::u_bot_http_interactions },
	{ 1 << 22, 	dpp::u_active_developer},
};

user::user() :
	managed(),
	flags(0),
	discriminator(0),
	refcount(1)
{
}

std::string user::get_mention(const snowflake& id) {
	return utility::user_mention(id);
}

json user::to_json_impl(bool with_id) const {
	json j;

	j["id"] = id;
	j["username"] = username;
	j["global_name"] = global_name; 
	j["avatar"] = avatar.to_string();
	j["discriminator"] = discriminator;
	j["bot"] = is_bot();
	j["system"] = is_system();
	j["mfa_enabled"] = is_mfa_enabled();
	j["verified"] = is_verified();
	j["premium_type"] = (has_nitro_classic() ? 1 : (has_nitro_full() ? 2 : has_nitro_basic() ? 3 : 0));
	uint32_t out_flags = 0;
	for (auto & flag : usermap) {
		if (flags & flag.second) {
			out_flags |= flag.first;
		}
	}
	j["flags"] = out_flags;

	return j;
}

json user_identified::to_json_impl(bool with_id) const {
	return {};
}

user_identified::user_identified() : user(), accent_color(0), verified(false) {

}

user_identified::user_identified(const user& u): user(u), accent_color(0), verified(false) {
}

std::string user::get_avatar_url(uint16_t size, const image_type format, bool prefer_animated) const {
	if (this->avatar.to_string().empty()) {
		return get_default_avatar_url();
	} else if (this->id) {
		return utility::cdn_endpoint_url_hash({ i_jpg, i_png, i_webp, i_gif },
											  "avatars/" + std::to_string(this->id), this->avatar.to_string(),
											  format, size, prefer_animated, has_animated_icon());
	} else {
		return std::string();
	}
}

std::string user::get_default_avatar_url() const {
	if (this->discriminator) {
		return utility::cdn_endpoint_url({ i_png },
										 "embed/avatars/" + std::to_string(this->discriminator % 5),
										 i_png, 0);
	} else if (this->id){
		return utility::cdn_endpoint_url({ i_png },
										 "embed/avatars/" + std::to_string((this->id >> 22) % 6),
										 i_png, 0);
	} else {
		return std::string();
	}
}

std::string user::get_avatar_decoration_url(uint16_t size) const {
	if (this->id) {
		return utility::cdn_endpoint_url_hash({ i_png },
											  "avatar-decorations/" + std::to_string(this->id), this->avatar_decoration.to_string(),
											  i_png, size);
	} else {
		return std::string();
	}
}

std::string user::format_username() const {
	if (!global_name.empty()) {
		return global_name;
	}
	return username + '#' + leading_zeroes(discriminator, 4);
}

std::string user::get_mention() const {
	return utility::user_mention(id);
}

std::string user::get_url() const{
	return utility::user_url(id);
}

bool user::is_active_developer() const {
	return this->flags & u_active_developer;
}

bool user::is_bot() const {
	 return this->flags & u_bot;
}

bool user::is_system() const {
	 return this->flags & u_system;
}

bool user::is_mfa_enabled() const {
	 return this->flags & u_mfa_enabled;
}

bool user::is_verified() const {
	 return this->flags & u_verified;
}

bool user::has_nitro_full() const {
	 return this->flags & u_nitro_full;
}

bool user::has_nitro_classic() const {
	 return this->flags & u_nitro_classic;
}

bool user::has_nitro_basic() const {
	return this->flags & u_nitro_basic;
}

bool user::is_discord_employee() const {
	 return this->flags & u_discord_employee;
}

bool user::is_partnered_owner() const {
	 return this->flags & u_partnered_owner;
}

bool user::has_hypesquad_events() const {
	 return this->flags & u_hypesquad_events;
}

bool user::is_bughunter_1() const {
	 return this->flags & u_bughunter_1;
}

bool user::is_house_bravery() const {
	 return this->flags & u_house_bravery;
}

bool user::is_house_brilliance() const {
	 return this->flags & u_house_brilliance;
}

bool user::is_house_balance() const {
	 return this->flags & u_house_balance;
}

bool user::is_early_supporter() const {
	 return this->flags & u_early_supporter;
}

bool user::is_team_user() const {
	 return this->flags & u_team_user;
}

bool user::is_bughunter_2() const {
	 return this->flags & u_bughunter_2;
}

bool user::is_verified_bot() const {
	 return this->flags & u_verified_bot;
}

bool user::is_verified_bot_dev() const {
	 return this->flags & u_verified_bot_dev;
}

bool user::is_certified_moderator() const {
	 return this->flags & u_certified_moderator;
}

bool user::is_bot_http_interactions() const {
	 return this->flags & u_bot_http_interactions;
}

bool user::has_animated_icon() const {
	return this->flags & u_animated_icon;
}

user& user::fill_from_json_impl(json* j) {
	j->get_to(*this);
	return *this;
}

user_identified& user_identified::fill_from_json_impl(json* j) {
	j->get_to(*this);
	return *this;
}

bool user_identified::has_animated_banner() const {
	return this->flags & u_animated_banner;
}

std::string user_identified::get_banner_url(uint16_t size, const image_type format, bool prefer_animated) const {
	if (!this->banner.to_string().empty() && this->id) {
		return utility::cdn_endpoint_url_hash({ i_jpg, i_png, i_webp, i_gif },
											  "banners/" + std::to_string(this->id), this->banner.to_string(),
											  format, size, prefer_animated, has_animated_banner());
	} else {
		return std::string();
	}
}

void from_json(const nlohmann::json& j, user_identified& u) {
	dpp::user* user_type = dynamic_cast<user*>(&u);
	from_json(j, *user_type);
	u.email = string_not_null(&j, "email");
	u.locale = string_not_null(&j, "locale");
	u.accent_color = int32_not_null(&j, "accent_color");
	u.verified = bool_not_null(&j, "verified");
	if (j.find("banner") != j.end()) {
		std::string b = string_not_null(&j, "banner");
		if (b.length() > 2 && b.substr(0, 2) == "a_") {
			b = b.substr(2, b.length());
			u.flags |= u_animated_banner;
		}
		u.banner = b;
	}
}

void from_json(const nlohmann::json& j, user& u) {
	u.id = snowflake_not_null(&j, "id");
	u.username = string_not_null(&j, "username");
	u.global_name = string_not_null(&j, "global_name");

	std::string av = string_not_null(&j, "avatar");
	if (av.length() > 2 && av.substr(0, 2) == "a_") {
		av = av.substr(2, av.length());
		u.flags |= u_animated_icon;
	}
	u.avatar = av;

	u.avatar_decoration = string_not_null(&j, "avatar_decoration");

	u.discriminator = (uint16_t)snowflake_not_null(&j, "discriminator");

	u.flags |= bool_not_null(&j, "bot") ? dpp::u_bot : 0;
	u.flags |= bool_not_null(&j, "system") ? dpp::u_system : 0;
	u.flags |= bool_not_null(&j, "mfa_enabled") ? dpp::u_mfa_enabled : 0;
	u.flags |= bool_not_null(&j, "verified") ? dpp::u_verified : 0;
	u.flags |= int8_not_null(&j, "premium_type") == 1 ? dpp::u_nitro_classic : 0;
	u.flags |= int8_not_null(&j, "premium_type") == 2 ? dpp::u_nitro_full : 0;
	u.flags |= int8_not_null(&j, "premium_type") == 3 ? dpp::u_nitro_basic : 0;
	uint32_t flags = int32_not_null(&j, "flags");
	flags |= int32_not_null(&j, "public_flags");
	for (auto & flag : usermap) {
		if (flags & flag.first) {
			u.flags |= flag.second;
		}
	}
}

} // namespace dpp
