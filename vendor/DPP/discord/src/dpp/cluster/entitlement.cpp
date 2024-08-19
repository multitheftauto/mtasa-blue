/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
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
#include <dpp/restrequest.h>
#include <dpp/entitlement.h>

namespace dpp {

void cluster::entitlements_get(snowflake user_id, const std::vector<snowflake>& sku_ids, snowflake before_id, snowflake after_id, uint8_t limit, snowflake guild_id, bool exclude_ended, command_completion_event_t callback) {
	json j;

	if(user_id) {
		j["user_id"] = user_id.str();
	}

	if(!sku_ids.empty()) {
		/* Why can't Discord just be consistent and accept an array of ids????
		 * Why just out of nowhere introduce a "comma-delimited set of snowflakes", like what.
		 * Just allow an array like you normally do!!!!!!!!!!!!
		 */
		std::string ids = "";
		for(size_t i = 0; i<ids.length(); i++) {
			if(ids.length() != i) {
				ids += (sku_ids[i].str() + ",");
			} else {
				ids += sku_ids[i].str();
			}
		}
		j["sku_ids"] = ids;
	}

	if(before_id) {
		j["before_id"] = before_id.str();
	}

	if(after_id) {
		j["after_id"] = after_id.str();
	}

	j["limit"] = limit;

	if(guild_id) {
		j["guild_id"] = guild_id.str();
	}

	j["exclude_ended"] = exclude_ended;

	rest_request_list<entitlement>(this, API_PATH "/applications", me.id.str(), "entitlements", m_get, j, callback);
}

void cluster::entitlement_test_create(const class entitlement& new_entitlement, command_completion_event_t callback) {
	json j;
	j["sku_id"] = new_entitlement.sku_id.str();
	j["owner_id"] = new_entitlement.owner_id.str();
	j["owner_type"] = new_entitlement.type;
	rest_request<entitlement>(this, API_PATH "/applications", me.id.str(), "entitlements", m_post, j, callback);
}

void cluster::entitlement_test_delete(const class snowflake entitlement_id, command_completion_event_t callback) {
	rest_request<confirmation>(this, API_PATH "/applications", me.id.str(), "entitlements/" + entitlement_id.str(), m_delete, "", callback);
}

} // namespace dpp
