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


void thread_members_update::handle(discord_client* client, json& j, const std::string& raw) {
	json& d = j["d"];

	dpp::guild* g = dpp::find_guild(snowflake_not_null(&d, "guild_id"));
	if (!client->creator->on_thread_members_update.empty()) {
		dpp::thread_members_update_t tms(client, raw);
		tms.updating_guild = g;
		set_snowflake_not_null(&d, "id", tms.thread_id);
		set_int8_not_null(&d, "member_count", tms.member_count);
		if (d.find("added_members") != d.end()) {
			for (auto& tm : d["added_members"]) {
				tms.added.emplace_back(thread_member().fill_from_json(&tm));
			}
		}
		if (d.find("removed_member_ids") != d.end()) {
			try {
				for (auto& rm : d["removed_member_ids"]) {
					tms.removed_ids.push_back(std::stoull(static_cast<std::string>(rm)));
				}
			} catch (const std::exception& e) {
				client->creator->log(dpp::ll_error, std::string("thread_members_update: {}") + e.what());
			}
		}
		client->creator->on_thread_members_update.call(tms);
	}
}
};
