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
void guild_update::handle(discord_client* client, json &j, const std::string &raw) {
	json& d = j["d"];
	guild newguild;
	dpp::guild* g = nullptr;
	if (client->creator->cache_policy.guild_policy == cp_none) {
		newguild.fill_from_json(client, &d);
		g = &newguild;
	} else {
		g = dpp::find_guild(snowflake_not_null(&d, "id"));
		if (g) {
			g->fill_from_json(client, &d);
			if (!g->is_unavailable()) {
				if (client->creator->cache_policy.role_policy != dpp::cp_none && d.find("roles") != d.end()) {
					for (size_t rc = 0; rc < g->roles.size(); ++rc) {
						dpp::role* oldrole = dpp::find_role(g->roles[rc]);
						dpp::get_role_cache()->remove(oldrole);
					}
					g->roles.clear();
					for (auto & role : d["roles"]) {
						dpp::role *r = new dpp::role();
						r->fill_from_json(g->id, &role);
						dpp::get_role_cache()->store(r);
						g->roles.push_back(r->id);
					}
				}
			}
		}
	}
	if (!client->creator->on_guild_update.empty()) {
		dpp::guild_update_t gu(client, raw);
		gu.updated = g;
		client->creator->on_guild_update.call(gu);
	}
}

};