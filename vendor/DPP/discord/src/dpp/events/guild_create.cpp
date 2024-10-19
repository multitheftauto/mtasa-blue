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
void guild_create::handle(discord_client* client, json &j, const std::string &raw) {
	json& d = j["d"];
	dpp::guild newguild;
	dpp::guild* g = nullptr;

	if (snowflake_not_null(&d, "id") == 0) {
		/* This shouldnt ever happen, but it has been seen in the wild i guess?
		 * Either way a guild with invalid or missing ID doesnt want to cause events.
		 */
		return;
	}

	if (client->creator->cache_policy.guild_policy == cp_none) {
		newguild.fill_from_json(client, &d);
		g = &newguild;
	} else {
		bool is_new_guild = false;
		g = dpp::find_guild(snowflake_not_null(&d, "id"));
		if (!g) {
			g = new dpp::guild();
			is_new_guild = true;
		}
		g->fill_from_json(client, &d);
		g->shard_id = client->shard_id;
		if (!g->is_unavailable() && is_new_guild) {
			if (client->creator->cache_policy.role_policy != dpp::cp_none) {
				/* Store guild roles */
				g->roles.clear();
				g->roles.reserve(d["roles"].size());
				for (auto & role : d["roles"]) {
					dpp::role *r = dpp::find_role(snowflake_not_null(&role, "id"));
					if (!r) {
						r = new dpp::role();
					}
					r->fill_from_json(g->id, &role);
					dpp::get_role_cache()->store(r);
					g->roles.push_back(r->id);
				}
			}

			/* Store guild channels */
			g->channels.clear();
			g->channels.reserve(d["channels"].size());
			for (auto & channel : d["channels"]) {
				dpp::channel* c = dpp::find_channel(snowflake_not_null(&channel, "id"));
				if (!c) {
					c = new dpp::channel();
				}
				c->fill_from_json(&channel);
				c->guild_id = g->id;
				dpp::get_channel_cache()->store(c);
				g->channels.push_back(c->id);
			}

			/* Store guild threads */
			g->threads.clear();
			g->threads.reserve(d["threads"].size());
			for (auto & channel : d["threads"]) {
				g->threads.push_back(snowflake_not_null(&channel, "id"));
			}

			/* Store guild members */
			if (client->creator->cache_policy.user_policy == cp_aggressive) {
				g->members.reserve(d["members"].size());
				for (auto & user : d["members"]) {
					snowflake userid = snowflake_not_null(&(user["user"]), "id");
					/* Only store ones we don't have already otherwise gm will leak */
					if (g->members.find(userid) == g->members.end()) {
						dpp::user* u = dpp::find_user(userid);
						if (!u) {
							u = new dpp::user();
							u->fill_from_json(&(user["user"]));
							dpp::get_user_cache()->store(u);
						} else {
							u->refcount++;
						}
						dpp::guild_member gm;
						gm.fill_from_json(&user, g->id, userid);
						g->members[userid] = gm;
					}
				}
			}
			if (client->creator->cache_policy.emoji_policy != dpp::cp_none) {
				/* Store emojis */
				g->emojis.reserve(d["emojis"].size());
				g->emojis = {};
				for (auto & emoji : d["emojis"]) {
					dpp::emoji* e = dpp::find_emoji(snowflake_not_null(&emoji, "id"));
					if (!e) {
						e = new dpp::emoji();
						e->fill_from_json(&emoji);
						dpp::get_emoji_cache()->store(e);
					}
					g->emojis.push_back(e->id);
				}
			}
		}
		dpp::get_guild_cache()->store(g);
		if (is_new_guild && g->id && (client->intents & dpp::i_guild_members)) {
			if (client->creator->cache_policy.user_policy == cp_aggressive) {
				json chunk_req = json({{"op", 8}, {"d", {{"guild_id",std::to_string(g->id)},{"query",""},{"limit",0}}}});
				if (client->intents & dpp::i_guild_presences) {
					chunk_req["d"]["presences"] = true;
				}
				client->queue_message(client->jsonobj_to_string(chunk_req));
			}
		}
	}

	if (!client->creator->on_guild_create.empty()) {
		dpp::guild_create_t gc(client, raw);
		gc.created = g;

		/* Fill presences if there are any */
		if (d.find("presences") != d.end()) {
			for (auto & p : d["presences"]) {
				try {
					snowflake user_id = std::stoull(p["user"]["id"].get<std::string>());
					gc.presences.emplace(user_id, presence().fill_from_json(&p));
				}
				catch (std::exception&) {
					/*
				     	 * std::invalid_argument if no conversion could be performed
					 * std::out_of_range if the converted value would fall out of the range of
					 * the result type or if the underlying function (std::strtoul or std::strtoull)
					 * sets errno to ERANGE. 
					 */
				}
			}
		}

		/* Fill in scheduled events, if there are any */
		if (d.find("guild_scheduled_events") != d.end()) {
			for (auto & p : d["guild_scheduled_events"]) {
				scheduled_event s;
				s.fill_from_json(&p);
				gc.scheduled_events.emplace(s.id, s);
			}
		}

		/* Fill in stage instances, if there are any */
		if (d.find("stage_instances") != d.end()) {
			for (auto & p : d["stage_instances"]) {
				stage_instance s;
				s.fill_from_json(&p);
				gc.stage_instances.emplace(s.id, s);
			}
		}

		/* Fill in threads, if there are any */
		if (d.find("threads") != d.end()) {
			for (auto & p : d["threads"]) {
				dpp::thread t;
				t.fill_from_json(&p);
				gc.threads.emplace(t.id, t);
			}
		}

		/* Fill in stickers, if there are any */
		if (d.find("stickers") != d.end()) {
			for (auto & p : d["stickers"]) {
				sticker st;
				st.fill_from_json(&p);
				gc.stickers.emplace(st.id, st);
			}
		}

		client->creator->on_guild_create.call(gc);
	}
}

};
