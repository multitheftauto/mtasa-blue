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
#include <dpp/role.h>
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
void guild_role_create::handle(discord_client* client, json &j, const std::string &raw) {
	json &d = j["d"];
	dpp::snowflake guild_id = snowflake_not_null(&d, "guild_id");
	dpp::guild* g = dpp::find_guild(guild_id);
	if (client->creator->cache_policy.role_policy == dpp::cp_none) {
		json &role = d["role"];
		dpp::role r;
		r.fill_from_json(guild_id, &role);
		if (!client->creator->on_guild_role_create.empty()) {
			dpp::guild_role_create_t grc(client, raw);
			grc.creating_guild = g;
			grc.created = &r;
			client->creator->on_guild_role_create.call(grc);
		}
	} else {
		json &role = d["role"];
		dpp::role *r = dpp::find_role(snowflake_not_null(&role, "id"));
		if (!r) {
			r = new dpp::role();
		}
		r->fill_from_json(guild_id, &role);
		dpp::get_role_cache()->store(r);
		if (g) {
			g->roles.push_back(r->id);
		}
		if (!client->creator->on_guild_role_create.empty()) {
			dpp::guild_role_create_t grc(client, raw);
			grc.creating_guild = g;
			grc.created = r;
			client->creator->on_guild_role_create.call(grc);
		}
	}
}

};