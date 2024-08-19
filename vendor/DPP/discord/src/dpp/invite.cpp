/************************************************************************************invi
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
#include <dpp/invite.h>
#include <dpp/discordevents.h>
#include <dpp/json.h>



namespace dpp {

using json = nlohmann::json;

invite::invite() : expires_at(0), guild_id(0), channel_id(0), inviter_id(0), target_user_id(0), target_type(itt_none), approximate_presence_count(0), approximate_member_count(0), max_age(86400), max_uses(0), temporary(false), unique(false), uses(0), created_at(0)
{
}

invite& invite::fill_from_json_impl(nlohmann::json* j) {
	code = string_not_null(j, "code");
	expires_at = (j->contains("expires_at")) ? ts_not_null(j, "expires_at") : 0;
	created_at = (j->contains("created_at")) ? ts_not_null(j, "created_at") : 0;
	if (j->contains("guild") && !j->at("guild").is_null()) {
		destination_guild = dpp::guild().fill_from_json(&((*j)["guild"]));
		guild_id = destination_guild.id;
	} else if (j->contains("guild_id")) { // check ID for invite gateway events
		guild_id = snowflake_not_null(j, "guild_id");
	}
	if (j->contains("channel") && !j->at("channel").is_null()) {
		destination_channel = dpp::channel().fill_from_json(&((*j)["channel"]));
		channel_id = destination_channel.id;
	} else if (j->contains("channel_id")) { // check ID for invite gateway events
		channel_id = snowflake_not_null(j, "channel_id");
	}
	if (j->contains("inviter") && !j->at("inviter").is_null()) {
		inviter = dpp::user().fill_from_json(&((*j)["inviter"]));
		inviter_id = inviter.id;
	}
	target_user_id = (j->contains("target_user")) ? snowflake_not_null(&((*j)["target_user"]), "id") : 0;
	target_type = static_cast<invite_target_t>(int8_not_null(j, "target_type"));
	approximate_presence_count = int32_not_null(j, "approximate_presence_count");
	approximate_member_count = int32_not_null(j, "approximate_member_count");
	max_age = int32_not_null(j, "max_age");
	max_uses = int8_not_null(j, "max_uses");
	temporary = bool_not_null(j, "temporary");
	unique = bool_not_null(j, "unique");
	uses = (j->contains("uses")) ? int32_not_null(j, "uses") : 0;
	if (j->contains("stage_instance")) {
		stage = stage_instance().fill_from_json(&((*j)["stage_instance"]));
	}
	return *this;
}

json invite::to_json_impl(bool with_id) const {
	json j;
	j["max_age"] = max_age;
	j["max_uses"] = max_uses;
	if (target_user_id > 0) {
		j["target_user"] = target_user_id;
	}
	if (target_type != itt_none) {
		j["target_type"] = target_type;
	}
	if (temporary) {
		j["temporary"] = temporary;
	}
	if (unique) {
		j["unique"] = unique;
	}
	return j;
}

invite &invite::set_max_age(const uint32_t max_age_) {
	this->max_age = max_age_;
	return *this;
}

invite &invite::set_max_uses(const uint8_t max_uses_) {
	this->max_uses = max_uses_;
	return *this;
}

invite &invite::set_target_user_id(const snowflake user_id) {
	this->target_user_id = user_id;
	return *this;
}

invite &invite::set_target_type(const invite_target_t type) {
	this->target_type = type;
	return *this;
}

invite &invite::set_temporary(const bool is_temporary) {
	this->temporary = is_temporary;
	return *this;
}

invite &invite::set_unique(const bool is_unique) {
	this->unique = is_unique;
	return *this;
}

} // namespace dpp
