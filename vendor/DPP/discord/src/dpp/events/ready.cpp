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
#include <dpp/stringops.h>
#include <dpp/dns.h>
#include <dpp/json.h>


namespace dpp::events {

#ifndef _DOXYGEN_
std::mutex protect_the_loot;
#endif

/**
 * @brief Handle event
 * 
 * @param client Websocket client (current shard)
 * @param j JSON data for the event
 * @param raw Raw JSON string
 */
void ready::handle(discord_client* client, json &j, const std::string &raw) {
	client->log(dpp::ll_info, "Shard id " + std::to_string(client->shard_id) + " (" + std::to_string(client->shard_id + 1) + "/" + std::to_string(client->max_shards) + ") ready!");
	client->sessionid = j["d"]["session_id"].get<std::string>();
	/* Session-specific gateway resume url
	 * https://discord.com/developers/docs/change-log#sessionspecific-gateway-resume-urls
	 *
	 * Discord give us the hostname wrapped in wss://crap/ like we're going to pass it to
	 * some top-heavy lib. Let's strip all this out if given to us so we just have a
	 * hostname.
	 */
	std::string ugly(j["d"]["resume_gateway_url"]);
	if (ugly.substr(0, 6) == "wss://") {
		client->resume_gateway_url = ugly.substr(6, ugly.length() - (*ugly.rbegin() == '/' ? 7 : 6));
	} else {
		client->resume_gateway_url = ugly;
	}
	/* Pre-resolve it into our cache so that we aren't waiting on this when we need it later */
	static_cast<void>(resolve_hostname(client->resume_gateway_url, "443"));
	client->log(ll_debug, "Resume URL for session " + client->sessionid + " is " + ugly + " (host: " + client->resume_gateway_url + ")");

	client->ready = true;

	/* Mutex this to make sure multiple threads don't change it at the same time */
	{
		std::lock_guard<std::mutex> lockit(protect_the_loot);
		client->creator->me.fill_from_json(&(j["d"]["user"]));
	}

	if (!client->creator->on_ready.empty()) {
		dpp::ready_t r(client, raw);
		r.session_id = client->sessionid;
		r.shard_id = client->shard_id;
		for (const auto& guild : j["d"]["guilds"]) {
			r.guilds.emplace_back(snowflake_not_null(&guild, "id"));
		}
		r.guild_count = r.guilds.size();
		client->creator->on_ready.call(r);
	}
}

};