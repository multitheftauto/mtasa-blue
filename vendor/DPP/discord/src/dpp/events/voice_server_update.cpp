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
#include <dpp/voicestate.h>
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
void voice_server_update::handle(discord_client* client, json &j, const std::string &raw) {

	json &d = j["d"];
	dpp::voice_server_update_t vsu(client, raw);
	vsu.guild_id = snowflake_not_null(&d, "guild_id");
	vsu.token = string_not_null(&d, "token");
	vsu.endpoint = string_not_null(&d, "endpoint");

	{
		std::shared_lock lock(client->voice_mutex);
		auto v = client->connecting_voice_channels.find(vsu.guild_id);
		/* Check to see if there is a connection in progress for a voice channel on this guild */
		if (v != client->connecting_voice_channels.end()) {
			if (!v->second->is_ready()) {
				v->second->token = vsu.token;
				v->second->websocket_hostname = vsu.endpoint;
				if (!v->second->is_active()) {
					v->second->connect(vsu.guild_id);
				}
			}
		}
	}

	if (!client->creator->on_voice_server_update.empty()) {
		client->creator->on_voice_server_update.call(vsu);
	}
}

};