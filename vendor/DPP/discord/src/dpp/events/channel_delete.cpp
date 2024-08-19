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
#include <dpp/channel.h>
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
void channel_delete::handle(discord_client* client, json &j, const std::string &raw) {
	json& d = j["d"];
	const channel c = channel().fill_from_json(&d);
	guild* g = find_guild(c.guild_id);
	if (g) {
		g->channels.erase(std::remove(g->channels.begin(), g->channels.end(), c.id), g->channels.end());
	}
	if (client->creator->cache_policy.channel_policy != cp_none) {
		/* We must only pass pointers found by find_channel into here, any other ptr is an invalid non-op */
		get_channel_cache()->remove(find_channel(c.id));
	}
	if (!client->creator->on_channel_delete.empty()) {
		channel_delete_t cd(client, raw);
		cd.deleted = c;
		cd.deleting_guild = g;
		client->creator->on_channel_delete.call(cd);
	}
}

};