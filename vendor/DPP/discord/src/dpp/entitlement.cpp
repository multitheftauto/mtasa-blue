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
#include <dpp/entitlement.h>
#include <dpp/discordevents.h>
#include <dpp/json.h>

namespace dpp {

using json = nlohmann::json;

entitlement::entitlement(const snowflake _sku_id, const snowflake _id, const snowflake _application_id, const entitlement_type _type, const uint8_t _flags)
	: managed(_id), sku_id(_sku_id), application_id(_application_id), type(_type), flags(_flags) {}

entitlement& entitlement::fill_from_json_impl(nlohmann::json* j) {
	set_snowflake_not_null(j, "id", id);
	set_snowflake_not_null(j, "sku_id", sku_id);
	set_snowflake_not_null(j, "application_id", application_id);

	/* Discord does separate these values, but asks for them as "owner_id" in the create event, just makes sense to make them as one as only one is ever set. */
	if(snowflake_not_null(j, "user_id")) {
		set_snowflake_not_null(j, "user_id", owner_id);
	} else if(snowflake_not_null(j, "guild_id")) {
		set_snowflake_not_null(j, "guild_id", owner_id);
	}

	type = static_cast<entitlement_type>(int8_not_null(j, "type"));

	if (bool_not_null(j, "deleted")) {
		flags |= ent_deleted;
	}

	set_ts_not_null(j, "starts_at", starts_at);
	set_ts_not_null(j, "ends_at", ends_at);

	/*
	 * TODO: Look at the entitlement example on docs and see what we're missing, add it here after. Discord seems to be missing information in their structure as their example shows more data.
	 */

	return *this;
}

json entitlement::to_json_impl(bool with_id) const {
	json j;
	if (with_id) {
		j["id"] = id.str();
	}
	j["sku_id"] = sku_id.str();
	return j;
}

entitlement_type entitlement::get_type() const {
	return type;
}

bool entitlement::is_deleted() const {
	return flags & entitlement_flags::ent_deleted;
}

} // namespace dpp
