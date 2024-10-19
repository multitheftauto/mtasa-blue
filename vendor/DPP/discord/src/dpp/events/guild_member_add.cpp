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
void guild_member_add::handle(discord_client* client, json &j, const std::string &raw) {
	json d = j["d"];
	dpp::snowflake guild_id = snowflake_not_null(&d, "guild_id");
	dpp::guild* g = dpp::find_guild(guild_id);
	dpp::guild_member_add_t gmr(client, raw);
	if (client->creator->cache_policy.user_policy == dpp::cp_none) {
		dpp::guild_member gm;
		gm.fill_from_json(&d, guild_id, snowflake_not_null(&(d["user"]), "id"));
		gmr.added = gm;
		if (!client->creator->on_guild_member_add.empty()) {
			gmr.adding_guild = g;
			client->creator->on_guild_member_add.call(gmr);
		}
	} else {
		dpp::user* u = dpp::find_user(snowflake_not_null(&(d["user"]), "id"));
		if (!u) {
			u = new dpp::user();
			u->fill_from_json(&(d["user"]));
			dpp::get_user_cache()->store(u);
		} else {
			u->refcount++;
		}
		dpp::guild_member gm;
		gmr.added = {};
		if (g && u && u->id && g->members.find(u->id) == g->members.end()) {
			gm.fill_from_json(&d, g->id, u->id);
			g->members[u->id] = gm;
			gmr.added = gm;
		} else if (g && u && u->id) {
			gmr.added = g->members.find(u->id)->second;
		}
		if (!client->creator->on_guild_member_add.empty()) {
			gmr.adding_guild = g;
			client->creator->on_guild_member_add.call(gmr);
		}
	}
}

};