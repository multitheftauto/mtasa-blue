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
#include <dpp/discordevents.h>
#include <dpp/cluster.h>
#include <dpp/cache.h>
#include <dpp/user.h>
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
void user_update::handle(discord_client* client, json &j, const std::string &raw) {
	json& d = j["d"];

	dpp::snowflake user_id = snowflake_not_null(&d, "id");
	if (user_id) {
		if (client->creator->cache_policy.user_policy != dpp::cp_none) {
			dpp::user* u = dpp::find_user(user_id);
			if (u) {
				u->fill_from_json(&d);
			}
			if (!client->creator->on_user_update.empty()) {
				dpp::user_update_t uu(client, raw);
				uu.updated = *u;
				client->creator->on_user_update.call(uu);
			}
		} else {
			if (!client->creator->on_user_update.empty()) {
				dpp::user u;
				u.fill_from_json(&d);
				dpp::user_update_t uu(client, raw);
				uu.updated = u;
				client->creator->on_user_update.call(uu);
			}
		}
	}
}

};