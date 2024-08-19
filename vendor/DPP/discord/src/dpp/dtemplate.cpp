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
#include <dpp/dtemplate.h>
#include <dpp/discordevents.h>
#include <dpp/json.h>

namespace dpp {

using json = nlohmann::json;

dtemplate::dtemplate() : code(""), name(""), description(""), usage_count(0), creator_id(0), source_guild_id(0)
{
}

dtemplate& dtemplate::fill_from_json_impl(nlohmann::json* j) {
	code = string_not_null(j, "code");
	name = string_not_null(j, "name");
	description = string_not_null(j, "description");
	usage_count = int32_not_null(j, "usage_count");
	creator_id = snowflake_not_null(j, "creator_id");
	created_at = ts_not_null(j, "created_at");
	updated_at = ts_not_null(j, "updated_at");
	source_guild_id = snowflake_not_null(j, "source_guild_id");
	is_dirty = bool_not_null(j, "is_dirty");
	return *this;
}

json dtemplate::to_json_impl(bool with_id) const {
	return {
		{"code", code},
		{"name", name},
		{"description", description},
		{"usage_count", usage_count},
		{"creator_id", creator_id},
		{"updated_at", updated_at},
		{"source_guild_id", source_guild_id,
		"is_dirty", is_dirty}
	};
}

} // namespace dpp
