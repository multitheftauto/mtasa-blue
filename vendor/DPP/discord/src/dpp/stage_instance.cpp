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
#include <dpp/stage_instance.h>
#include <dpp/discordevents.h>
#include <dpp/stringops.h>
#include <dpp/json.h>

namespace dpp {

using json = nlohmann::json;

stage_instance::stage_instance() :
	managed(0),
	guild_id(0),
	channel_id(0),	
	privacy_level(sp_public),
	discoverable_disabled(false)
{
}

stage_instance& stage_instance::fill_from_json_impl(const json* j) {
	set_snowflake_not_null(j, "id", this->id);
	set_snowflake_not_null(j, "guild_id", this->guild_id);
	set_snowflake_not_null(j, "channel_id", this->channel_id);
	set_string_not_null(j, "topic", this->topic) ;
	this->privacy_level = static_cast<dpp::stage_privacy_level>(int8_not_null(j, "privacy_level"));
	set_bool_not_null(j, "discoverable_disabled", this->discoverable_disabled);
	return *this;
}

json stage_instance::to_json_impl(bool with_id) const {
	json j;
	j["topic"] = this->topic;
	j["privacy_level"] = this->privacy_level;
	j["channel_id"] = std::to_string(this->channel_id);

	return j;
}

} // namespace dpp
