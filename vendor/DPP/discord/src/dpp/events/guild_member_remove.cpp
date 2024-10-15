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
void guild_member_remove::handle(discord_client* client, json &j, const std::string &raw) {
	json d = j["d"];

	dpp::guild_member_remove_t gmr(client, raw);
	gmr.removed.fill_from_json(&(d["user"]));
	gmr.guild_id = snowflake_not_null(&d, "guild_id");
	gmr.removing_guild = dpp::find_guild(gmr.guild_id);

	if (!client->creator->on_guild_member_remove.empty()) {
		client->creator->on_guild_member_remove.call(gmr);
	}

	if (client->creator->cache_policy.user_policy != dpp::cp_none && gmr.removing_guild) {
		auto i = gmr.removing_guild->members.find(gmr.removed.id);
		if (i != gmr.removing_guild->members.end()) {
			dpp::user* u = dpp::find_user(gmr.removed.id);
			if (u) {
				u->refcount--;
				if (u->refcount < 1) {
					dpp::get_user_cache()->remove(u);
				}
			}
			gmr.removing_guild->members.erase(i);
		}
	}
}

};