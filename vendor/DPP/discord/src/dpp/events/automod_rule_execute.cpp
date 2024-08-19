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
#include <dpp/automod.h>
#include <dpp/cluster.h>
#include <dpp/stringops.h>
#include <dpp/json.h>

namespace dpp::events {


/**
 * @brief Handle event
 * 
 * @param client Websocket client (current shard)
 * @param j JSON data for the event
 * @param raw Raw JSON string
 */
void automod_rule_execute::handle(discord_client* client, json &j, const std::string &raw) {
	if (!client->creator->on_automod_rule_execute.empty()) {
		json& d = j["d"];
		automod_rule_execute_t are(client, raw);
		are.guild_id = snowflake_not_null(&d, "guild_id");
		are.action = dpp::automod_action().fill_from_json(&(d["action"]));
		are.rule_id = snowflake_not_null(&d, "rule_id");
		are.rule_trigger_type = (automod_trigger_type)int8_not_null(&d, "rule_trigger_type");
		are.user_id = snowflake_not_null(&d, "user_id");
		are.channel_id = snowflake_not_null(&d, "channel_id");
		are.message_id = snowflake_not_null(&d, "message_id");
		are.alert_system_message_id = snowflake_not_null(&d, "alert_system_message_id");
		are.content = string_not_null(&d, "content");
		are.matched_keyword = string_not_null(&d, "matched_keyword");
		are.matched_content = string_not_null(&d, "matched_content");
		client->creator->on_automod_rule_execute.call(are);
	}
}

};