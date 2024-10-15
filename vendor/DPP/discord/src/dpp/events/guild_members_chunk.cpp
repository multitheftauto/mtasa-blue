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
#include <dpp/cache.h>
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
void guild_members_chunk::handle(discord_client* client, json &j, const std::string &raw) {
	json &d = j["d"];
	dpp::guild_member_map um;
	dpp::guild* g = dpp::find_guild(snowflake_not_null(&d, "guild_id"));
	if (g) {
		/* Store guild members */
		if (client->creator->cache_policy.user_policy == cp_aggressive) {
			for (auto & userrec : d["members"]) {
				json & userspart = userrec["user"];
				dpp::user* u = dpp::find_user(snowflake_not_null(&userspart, "id"));
				if (!u) {
					u = new dpp::user();
					u->fill_from_json(&userspart);
					dpp::get_user_cache()->store(u);
				}
				if (g->members.find(u->id) == g->members.end()) {
					dpp::guild_member gm;
					gm.fill_from_json(&userrec, g->id, u->id);
					g->members[u->id] = gm;
					if (!client->creator->on_guild_members_chunk.empty()) {
						um[u->id] = gm;
					}
				}
			}
		}
	}
	if (!client->creator->on_guild_members_chunk.empty()) {
		dpp::guild_members_chunk_t gmc(client, raw);
		gmc.adding = g;
		gmc.members = &um;
		client->creator->on_guild_members_chunk.call(gmc);
	}
}

};