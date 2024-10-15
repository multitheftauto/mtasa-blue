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
#include <dpp/emoji.h>
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
void guild_emojis_update::handle(discord_client* client, json &j, const std::string &raw) {
	json& d = j["d"];
	dpp::snowflake guild_id = snowflake_not_null(&d, "guild_id");
	dpp::guild* g = dpp::find_guild(guild_id);
	std::vector<dpp::snowflake> emojis;
	if (client->creator->cache_policy.emoji_policy != dpp::cp_none) {
		if (g) {
			for (auto & ee : g->emojis) {
				dpp::emoji* fe = dpp::find_emoji(ee);
				if (fe) {
					dpp::get_emoji_cache()->remove(fe);
				}
			}
		}
		for (auto & emoji : d["emojis"]) {
			dpp::emoji* e = dpp::find_emoji(snowflake_not_null(&emoji, "id"));
			if (!e) {
				e = new dpp::emoji();
				e->fill_from_json(&emoji);
				dpp::get_emoji_cache()->store(e);
			}
			emojis.push_back(e->id);
		}
		if (g) {
			g->emojis = emojis;
		}
	} else {
		for (auto & emoji : d["emojis"]) {
			emojis.push_back(snowflake_not_null(&emoji, "id"));
		}
	}
	if (!client->creator->on_guild_emojis_update.empty()) {
		dpp::guild_emojis_update_t geu(client, raw);
		geu.emojis = emojis;
		geu.updating_guild = g;
		client->creator->on_guild_emojis_update.call(geu);
	}
}

};