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
#include <dpp/thread.h>
#include <dpp/discordevents.h>


namespace dpp {

thread_member& thread_member::fill_from_json_impl(nlohmann::json* j) {
	set_snowflake_not_null(j, "id", this->thread_id);
	set_snowflake_not_null(j, "user_id", this->user_id);
	set_ts_not_null(j, "join_timestamp", this->joined);
	set_int32_not_null(j, "flags", this->flags);
	return *this;
}

thread& thread::fill_from_json_impl(json* j) {
	channel::fill_from_json(j);

	uint8_t type = int8_not_null(j, "type");
	this->flags |= (type & channel::CHANNEL_TYPE_MASK);

	set_snowflake_array_not_null(j, "applied_tags", this->applied_tags);

	set_int32_not_null(j, "total_message_sent", this->total_messages_sent);
	set_int8_not_null(j, "message_count", this->message_count);
	set_int8_not_null(j, "member_count", this->member_count);
	set_bool_not_null(j, "newly_created", this->newly_created);

	auto json_metadata = (*j)["thread_metadata"];
	metadata.archived = bool_not_null(&json_metadata, "archived");
	metadata.archive_timestamp = ts_not_null(&json_metadata, "archive_timestamp");
	metadata.auto_archive_duration = int16_not_null(&json_metadata, "auto_archive_duration");
	metadata.locked = bool_not_null(&json_metadata, "locked");
	metadata.invitable = bool_not_null(&json_metadata, "invitable");

	/* Only certain events set this */
	if (j->contains("member"))  {
		member.fill_from_json(&((*j)["member"]));
	}
	return *this;
}

json thread::to_json_impl(bool with_id) const {
	json j = channel::to_json_impl(with_id);
	j["type"] = (flags & CHANNEL_TYPE_MASK);
	j["archived"] = this->metadata.archived;
	j["auto_archive_duration"] = this->metadata.auto_archive_duration;
	j["locked"] = this->metadata.locked;

	if(this->get_type() == dpp::channel_type::CHANNEL_PRIVATE_THREAD) {
		j["invitable"] = this->metadata.invitable;
	}

	if (!this->applied_tags.empty()) {
		j["applied_tags"] = json::array();
		for (auto &tag_id: this->applied_tags) {
			if (tag_id) {
				j["applied_tags"].push_back(tag_id);
			}
		}
	}
	return j;
}

void to_json(nlohmann::json& j, const thread_metadata& tmdata) {
	j["archived"] = tmdata.archived;
	j["auto_archive_duration"] = tmdata.auto_archive_duration;
	j["locked"] = tmdata.locked;
	j["invitable"] = tmdata.invitable;
}

}
