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
#include <dpp/sku.h>
#include <dpp/discordevents.h>
#include <dpp/json.h>

namespace dpp {

using json = nlohmann::json;

sku::sku(const snowflake _id, const sku_type _type, const snowflake _application_id, const std::string _name, const std::string _slug, const uint16_t _flags)
	: managed(_id), type(_type), application_id(_application_id), name(_name), slug(_slug), flags(_flags) {}

sku& sku::fill_from_json_impl(nlohmann::json* j) {
	set_snowflake_not_null(j, "id", id);

	type = static_cast<sku_type>(int8_not_null(j, "type"));

	set_snowflake_not_null(j, "application_id", application_id);

	set_string_not_null(j, "name", name);
	set_string_not_null(j, "slug", slug);

	uint8_t sku_flags = int8_not_null(j, "flags");
	if (sku_flags & (1 << 2)) {
		flags |= sku_flags::sku_available;
	}
	if (sku_flags & (1 << 7)) {
		flags |= sku_flags::sku_guild_subscription;
	}
	if (sku_flags & (1 << 8)) {
		flags |= sku_flags::sku_user_subscription;
	}

	/*
	 * TODO: Look at the SKU example on docs and see what we're missing, add it here after. Discord seems to be missing information in their structure as their example shows more data.
	 * yes this is copied from dpp/entitlement.cpp, Discord's fault for their docs being inconsistent.
	 */

	return *this;
}

json sku::to_json_impl(bool with_id) const {
	json j;
	if (with_id) {
		j["id"] = id.str();
	}

	/* There's no reason to ever use this as you can only get SKUs, so no point putting any data here */

	return j;
}

sku_type sku::get_type() const {
	return type;
}

bool sku::is_available() const {
	return flags & sku_flags::sku_available;
}

bool sku::is_guild_subscription() const {
	return flags & sku_flags::sku_guild_subscription;
}

bool sku::is_user_subscription() const {
	return flags & sku_flags::sku_user_subscription;
}

} // namespace dpp
