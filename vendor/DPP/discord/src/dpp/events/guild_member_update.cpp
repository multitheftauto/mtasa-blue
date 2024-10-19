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
#include <dpp/guild.h>
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
void guild_member_update::handle(discord_client* client, json &j, const std::string &raw) {
	json& d = j["d"];
	dpp::snowflake guild_id = snowflake_not_null(&d, "guild_id");
	dpp::guild* g = dpp::find_guild(guild_id);
	if (client->creator->cache_policy.user_policy == dpp::cp_none) {
		dpp::user u;
		u.fill_from_json(&(d["user"]));
		dpp::guild_member_update_t gmu(client, raw);
		gmu.updating_guild = g;
		if (!client->creator->on_guild_member_update.empty()) {
			guild_member m;
			auto& user = d; // d contains roles and other member stuff already
			m.fill_from_json(&user, guild_id, u.id);
			gmu.updated = m;
		}
		client->creator->on_guild_member_update.call(gmu);
	} else {
		dpp::user* u = dpp::find_user(from_string<uint64_t>(d["user"]["id"].get<std::string>()));
		if (u) {
			auto& user = d;//d["user"]; // d contains roles and other member stuff already
			guild_member m;
			m.fill_from_json(&user, guild_id, u->id);
			if (g) {
				g->members[u->id] = m;
			}

			if (!client->creator->on_guild_member_update.empty()) {
				dpp::guild_member_update_t gmu(client, raw);
				gmu.updating_guild = g;
				gmu.updated = m;
				client->creator->on_guild_member_update.call(gmu);
			}
		}
	}
}

};