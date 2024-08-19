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
#include <dpp/application.h>
#include <dpp/discordevents.h>
#include <dpp/snowflake.h>
#include <dpp/managed.h>
#include <dpp/json.h>
#include <iostream>

namespace dpp {

using json = nlohmann::json;

application::application() : managed(0), bot_public(false), bot_require_code_grant(false), guild_id(0), primary_sku_id(0), flags(0)
{
}

application::~application() = default;

application& application::fill_from_json_impl(nlohmann::json* j) {
	set_snowflake_not_null(j, "id", id);
	set_string_not_null(j, "name", name);
	set_iconhash_not_null(j, "icon", icon);
	set_string_not_null(j, "description", description);

	if (j->contains("rpc_origins")) {
		for (const auto& rpc : (*j)["rpc_origins"]) {
			this->rpc_origins.push_back(to_string(rpc));
		}
	}

	set_bool_not_null(j, "bot_public", bot_public);
	set_bool_not_null(j, "bot_require_code_grant", bot_require_code_grant);
	bot = user().fill_from_json(&((*j)["bot"]));
	set_string_not_null(j, "terms_of_service_url", terms_of_service_url);
	set_string_not_null(j, "privacy_policy_url", privacy_policy_url);
	owner = user().fill_from_json(&((*j)["owner"]));
	// TODO: Remove the setting of 'summary' when v11 is released.
	set_string_not_null(j, "summary", summary);
	set_string_not_null(j, "verify_key", verify_key);

	if (j->contains("team")) {
		json& t = (*j)["team"];
		std::string i = string_not_null(&t, "icon");
		if (!i.empty()) {
			this->team.icon = i;
		}
		set_snowflake_not_null(&t, "id", this->team.id);
		set_string_not_null(&t, "name", this->team.name);
		set_snowflake_not_null(&t, "owner_user_id", this->team.owner_user_id);
		for (auto m : t["members"]) {
			team_member tm;
			tm.membership_state = (team_member_status)int32_not_null(&m, "membership_state");
			std::string member_role = string_not_null(&m, "role");
			if (member_role == "owner") {
				tm.member_role = tmr_owner;
			} else if (member_role == "admin") {
				tm.member_role = tmr_admin;
			} else if (member_role == "developer") {
				tm.member_role = tmr_developer;
			} else {
				tm.member_role = tmr_readonly;
			}
			set_string_not_null(&m, "permissions", tm.permissions);
			set_snowflake_not_null(&m, "team_id", tm.team_id);
			tm.member_user = user().fill_from_json(&m["user"]);
			this->team.members.emplace_back(tm);
		}
	}

	set_snowflake_not_null(j, "guild_id", guild_id);
	guild_obj = guild().fill_from_json(&((*j)["guild"]));
	set_snowflake_not_null(j, "primary_sku_id", primary_sku_id);
	set_string_not_null(j, "slug", slug);
	set_iconhash_not_null(j, "cover_image", cover_image);
	set_int32_not_null(j, "flags", flags);
	set_int64_not_null(j, "approximate_guild_count", approximate_guild_count);

	if (j->contains("redirect_uris")) {
		for (const auto& uri : (*j)["redirect_uris"]) {
			this->redirect_uris.push_back(to_string(uri));
		}
	}

	set_string_not_null(j, "interactions_endpoint_url", interactions_endpoint_url);
	set_string_not_null(j, "role_connections_verification_url", role_connections_verification_url);

	if (j->contains("tags")) {
		for (const auto& tag : (*j)["tags"]) {
			this->tags.push_back(to_string(tag));
		}
	}

	if (j->contains("install_params")) {
		json& p = (*j)["install_params"];
		set_snowflake_not_null(&p, "permissions", this->install_params.permissions);
		for (const auto& scope : p["scopes"]) {
			this->install_params.scopes.push_back(to_string(scope));
		}
	}

	set_string_not_null(j, "custom_install_url", custom_install_url);

	// TODO: Investigate https://discord.com/developers/docs/resources/application#application-resource when v11 releases. See if the variables below are documented.

	set_int8_not_null(j, "discoverability_state", discoverability_state);
	set_int32_not_null(j, "discovery_eligibility_flags", discovery_eligibility_flags);
	set_int8_not_null(j, "explicit_content_filter", explicit_content_filter);
	set_int8_not_null(j, "creator_monetization_state", creator_monetization_state);
	set_bool_not_null(j, "integration_public", integration_public);
	set_bool_not_null(j, "integration_require_code_grant", integration_require_code_grant);

	if (j->contains("interactions_event_types")) {
		for (const auto& event_type : (*j)["interactions_event_types"]) {
			this->interactions_event_types.push_back(to_string(event_type));
		}
	}

	set_int8_not_null(j, "interactions_version", interactions_version);
	set_bool_not_null(j, "is_monetized", is_monetized);
	set_int32_not_null(j, "monetization_eligibility_flags", monetization_eligibility_flags);
	set_int8_not_null(j, "monetization_state", monetization_state);
	set_bool_not_null(j, "hook", hook);
	set_int8_not_null(j, "rpc_application_state", rpc_application_state);
	set_int8_not_null(j, "store_application_state", store_application_state);
	set_int8_not_null(j, "verification_state", verification_state);

	return *this;
}

std::string application::get_cover_image_url(uint16_t size, const image_type format) const {
	if (!this->cover_image.to_string().empty() && this->id) {
		return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp },
	 		"app-icons/" + std::to_string(this->id) + "/" + this->cover_image.to_string(),
			format, size);
	}

	return "";
}

std::string application::get_icon_url(uint16_t size, const image_type format) const {
	if (!this->icon.to_string().empty() && this->id) {
		return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp },
	 		"app-icons/" + std::to_string(this->id) + "/" + this->icon.to_string(),
		 	format, size);
	}

	return "";
}

} // namespace dpp

