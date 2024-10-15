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
#include <dpp/auditlog.h>
#include <dpp/discordevents.h>
#include <dpp/json.h>

namespace dpp {

using json = nlohmann::json;

audit_entry::audit_entry(): type(aut_guild_update) {
}

audit_entry &audit_entry::fill_from_json_impl(nlohmann::json *j) {
	this->id = snowflake_not_null(j, "id");
	this->type = (audit_type)int8_not_null(j, "action_type");
	this->user_id = snowflake_not_null(j, "user_id");
	this->target_id = snowflake_not_null(j, "target_id");
	this->reason = string_not_null(j, "reason");
	if (j->contains("changes")) {
		auto &c = (*j)["changes"];
		for (auto & change : c) {
			audit_change ac;
			ac.key = string_not_null(&change, "key");
			if (change.find("new_value") != change.end()) {
				ac.new_value = change["new_value"].dump(-1, ' ', false, json::error_handler_t::replace);
			}
			if (change.find("old_value") != change.end()) {
				ac.old_value = change["old_value"].dump(-1, ' ', false, json::error_handler_t::replace);
			}
			this->changes.push_back(ac);
		}
	}
	if (j->contains("options")) {
		auto &o = (*j)["options"];
		audit_extra opts;
		opts.automod_rule_name = string_not_null(&o, "auto_moderation_rule_name");
		opts.automod_rule_trigger_type = string_not_null(&o, "auto_moderation_rule_trigger_type");
		opts.channel_id = snowflake_not_null(&o, "channel_id");
		opts.count = string_not_null(&o, "count");
		opts.delete_member_days = string_not_null(&o, "delete_member_days");
		opts.id = snowflake_not_null(&o, "id");
		opts.members_removed = string_not_null(&o, "members_removed");
		opts.message_id = snowflake_not_null(&o, "message_id");
		opts.role_name = string_not_null(&o, "role_name");
		opts.type = string_not_null(&o, "type");
		opts.application_id = snowflake_not_null(&o, "application_id");
		this->extra = opts;
	}
	return *this;
}

auditlog& auditlog::fill_from_json_impl(nlohmann::json* j) {
	set_object_array_not_null<audit_entry>(j, "audit_log_entries", this->entries);
	return *this;
}

} // namespace dpp

