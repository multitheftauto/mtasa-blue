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
#include <dpp/scheduled_event.h>
#include <dpp/exception.h>
#include <dpp/discordevents.h>
#include <dpp/stringops.h>
#include <dpp/json.h>

namespace dpp {

using json = nlohmann::json;

scheduled_event::scheduled_event() :
	managed(0),
	guild_id(0),
	channel_id(0),
	creator_id(0),
	scheduled_start_time(0),
	scheduled_end_time(0),
	privacy_level(ep_guild_only),
	status(es_scheduled),
	entity_type(eet_external),
	entity_id(0),
	user_count(0)
{
}

scheduled_event& scheduled_event::set_name(const std::string& n) {
	this->name = utility::validate(n, 1, 100, "Name too short");
	return *this;
}

scheduled_event& scheduled_event::set_description(const std::string& d) {
	this->description = utility::validate(d, 1, 1000, "Description too short");
	return *this;
}

scheduled_event& scheduled_event::clear_description() {
	this->description.clear();
	return *this;
}

scheduled_event& scheduled_event::set_location(const std::string& l) {
	this->entity_metadata.location = utility::validate(l, 1, 100, "Location too short");
	this->channel_id = 0;
	return *this;
}

scheduled_event& scheduled_event::set_channel_id(snowflake c) {
	this->channel_id = c;
	this->entity_metadata.location.clear();
	return *this;
}

scheduled_event& scheduled_event::set_creator_id(snowflake c) {
	this->creator_id = c;
	return *this;
}

scheduled_event& scheduled_event::set_status(event_status s) {
	if (this->status == es_completed || this->status == es_cancelled) {
		throw dpp::logic_exception(err_cancelled_event, "Can't update status of a completed or cancelled event");
	} else {
		if (this->status == es_scheduled) {
			if (s != es_active && s != es_cancelled) {
				throw dpp::logic_exception(err_event_status, "Invalid status transition, scheduled can only transition to active or cancelled");
			}
		} else if (this->status == es_active) {
			if (s != es_completed) {
				throw dpp::logic_exception(err_event_status, "Invalid status transition, active can only transition to completed");
			}
		}
	}
	this->status = s;
	return *this;
}

scheduled_event& scheduled_event::set_start_time(time_t t) {
	if (t < time(nullptr)) {
		throw dpp::length_exception(err_event_start_time, "Start time cannot be before current date and time");
	}
	this->scheduled_start_time = t;
	return *this;
}

scheduled_event& scheduled_event::set_end_time(time_t t) {
	if (t < time(nullptr)) {
		throw dpp::length_exception(err_event_end_time, "End time cannot be before current date and time");
	}
	this->scheduled_end_time = t;
	return *this;
}

scheduled_event& scheduled_event::load_image(std::string_view image_blob, const image_type type) {
	image = utility::image_data{type, image_blob};
	return *this;
}

scheduled_event& scheduled_event::load_image(const std::byte* data, uint32_t size, const image_type type) {
	image = utility::image_data{type, data, size};
	return *this;
}

scheduled_event& scheduled_event::fill_from_json_impl(const json* j) {
	set_snowflake_not_null(j, "id", this->id);
	set_snowflake_not_null(j, "guild_id", this->guild_id);
	set_snowflake_not_null(j, "channel_id", this->channel_id);
	set_snowflake_not_null(j, "creator_id", this->creator_id);
	set_snowflake_not_null(j, "creator_id", this->creator_id);
	set_string_not_null(j, "name", this->name);
	set_string_not_null(j, "description", this->description);
	if (auto it = j->find("image"); it != j->end() && !it->is_null())
		this->image = utility::iconhash{it->get<std::string>()};
	set_ts_not_null(j, "scheduled_start_time", this->scheduled_start_time);
	set_ts_not_null(j, "scheduled_end_time", this->scheduled_end_time);
	this->privacy_level = static_cast<dpp::event_privacy_level>(int8_not_null(j, "privacy_level"));
	this->status = static_cast<dpp::event_status>(int8_not_null(j, "status"));
	this->entity_type = static_cast<dpp::event_entity_type>(int8_not_null(j, "entity_type"));
	auto i = j->find("entity_metadata");
	if (i != j->end()) {
		set_string_not_null(&((*j)["entity_metadata"]), "location", this->entity_metadata.location);
	}
	if (j->contains("creator")) {
		json u = (*j)["creator"];
		creator.fill_from_json(&u);
	}
	set_int32_not_null(j, "user_count", this->user_count);
	return *this;
}

json scheduled_event::to_json_impl(bool with_id) const {
	json j;
	if (this->id && with_id) {
		j["id"] = std::to_string(id);
	}
	j["name"] = this->name;
	if (!this->description.empty()) {
		j["description"] = this->description;
	}
	if (image.is_image_data()) {
		j["image"] = image.as_image_data().to_nullable_json();
	}
	j["privacy_level"] = this->privacy_level;
	j["status"] = this->status;
	j["entity_type"] = this->entity_type;
	if (this->entity_id) {
		j["entity_id"] = std::to_string(this->entity_id);
	}
	if (this->channel_id) {
		j["channel_id"] = std::to_string(this->channel_id);
	} else {
		if (with_id) {
			j["channel_id"] = json::value_t::null;
		}
	}
	if (this->guild_id) {
		j["guild_id"] = std::to_string(this->guild_id);
	}
	if (this->creator_id) {
		j["creator_id"] = std::to_string(this->creator_id);
	}
	if (scheduled_start_time) {
		j["scheduled_start_time"] = ts_to_string(scheduled_start_time);
	}
	if (scheduled_end_time) {
		j["scheduled_end_time"] = ts_to_string(scheduled_end_time);
	}
	j["entity_metadata"] = json::object();
	if (!entity_metadata.location.empty()) {
		j["entity_metadata"]["location"] = entity_metadata.location;
	} else {
		j["entity_metadata"]["location"] = json::value_t::null;
	}

	return j;
}

} // namespace dpp
