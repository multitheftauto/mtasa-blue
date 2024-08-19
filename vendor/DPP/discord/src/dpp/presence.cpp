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
#include <dpp/presence.h>
#include <dpp/discordevents.h>
#include <dpp/utility.h>
#include <dpp/emoji.h>
#include <dpp/json.h>



namespace dpp {

using json = nlohmann::json;

std::string activity::get_large_asset_url(uint16_t size, const image_type format) const {
	if (!this->assets.large_image.empty() && this->application_id &&
		this->assets.large_image.find(':') == std::string::npos) { // make sure it's not a prefixed proxy image
		return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp },
										 "app-assets/" + std::to_string(this->application_id) + "/" + this->assets.large_image,
										 format, size);
	} else {
		return std::string();
	}
}

std::string activity::get_small_asset_url(uint16_t size, const image_type format) const {
	if (!this->assets.small_image.empty() && this->application_id &&
		this->assets.small_image.find(':') == std::string::npos) { // make sure it's not a prefixed proxy image
		return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp },
										 "app-assets/" + std::to_string(this->application_id) + "/" + this->assets.small_image,
										 format, size);
	} else {
		return std::string();
	}
}

activity_party::activity_party() : id(0), current_size(0), maximum_size(0)
{
}

activity::activity(const activity_type typ, const std::string& nam, const std::string& stat, const std::string& url_) :
	 name(nam), state(stat), url(url_), type(typ), created_at(0), start(0), end(0), application_id(0), flags(0), is_instance(false)
{
}

activity::activity(): created_at(0), start(0), end(0), application_id(0), flags(0), is_instance(false)
{
}

presence::presence() : user_id(0), guild_id(0), flags(0)
{
}

presence::presence(presence_status status, activity_type type, const std::string& activity_description) {
	dpp::activity a;

	/* Even if type is custom, a name is still required.
	 * We'll just set the name as this activity_description as it won't be used if custom either way. */
	a.name = activity_description;

	/* If the type is custom, set the state as "activity_description" */
	if (type == activity_type::at_custom) {
	    	a.state = activity_description;
	}

	a.type = type;
	activities.clear();
	activities.emplace_back(a);
	flags &= PF_CLEAR_STATUS;
	if (status == ps_online) {
		flags |= p_status_online;
	} else if (status == ps_idle) {
		flags |= p_status_idle;
	} else if (status == ps_dnd) {
		flags |= p_status_dnd;
	}
}

presence::presence(presence_status status, const activity &a) {
	activities.clear();
	activities.emplace_back(a);
	flags &= PF_CLEAR_STATUS;
	if (status == ps_online) {
		flags |= p_status_online;
	} else if (status == ps_idle) {
		flags |= p_status_idle;
	} else if (status == ps_dnd) {
		flags |= p_status_dnd;
	}
}

presence::~presence() = default;

presence& presence::fill_from_json_impl(nlohmann::json* j) {
	guild_id = snowflake_not_null(j, "guild_id");
	user_id = snowflake_not_null(&((*j)["user"]), "id");

	auto f = j->find("client_status");
	if (f != j->end()) {

		bool update_desktop = false, update_web = false, update_mobile = false;
		std::string desktop, mobile, web;

		if (f->find("desktop") != f->end()) {
			desktop = string_not_null(&((*j)["client_status"]), "desktop");
			update_desktop = true;
		}
		if (f->find("mobile") != f->end()) {
			mobile = string_not_null(&((*j)["client_status"]), "mobile");
			update_mobile = true;
		}
		if (f->find("web") != f->end()) {
			web = string_not_null(&((*j)["client_status"]), "web");
			update_web = true;
		}

		if (update_desktop) {
			flags &= PF_CLEAR_DESKTOP;
			if (desktop == "online") {
				flags |= p_desktop_online;
			} else if (desktop == "idle") {
				flags |= p_desktop_idle;
			} else if (desktop == "dnd") {
				flags |= p_desktop_dnd;
			}
		}

		if (update_mobile) {
			flags &= PF_CLEAR_MOBILE;
			if (mobile == "online") {
				flags |= p_mobile_online;
			} else if (mobile == "idle") {
				flags |= p_mobile_idle;
			} else if (mobile == "dnd") {
				flags |= p_mobile_dnd;
			}
		}

		if (update_web) {
			flags &= PF_CLEAR_WEB;
			if (web == "online") {
				flags |= p_web_online;
			} else if (web == "idle") {
				flags |= p_web_idle;
			} else if (web == "dnd") {
				flags |= p_web_dnd;
			}
		}
	}

	if (j->contains("status")) {
		flags &= PF_CLEAR_STATUS;
		std::string main = string_not_null(j, "status");
		if (main == "online") {
			flags |= p_status_online;
		} else if (main == "idle") {
			flags |= p_status_idle;
		} else if (main == "dnd") {
			flags |= p_status_dnd;
		}
	}


	if (j->contains("activities")) {
		activities.clear();
		for (auto & act : (*j)["activities"]) {
			activity a;
			a.name = string_not_null(&act, "name");
			a.details = string_not_null(&act, "details");
			if (act.find("assets") != act.end()) {
				a.assets.large_image = string_not_null(&act["assets"], "large_image");
				a.assets.large_text = string_not_null(&act["assets"], "large_text");
				a.assets.small_image = string_not_null(&act["assets"], "small_image");
				a.assets.small_text = string_not_null(&act["assets"], "small_text");
			}
			a.state = string_not_null(&act, "state");
			a.type = (activity_type)int8_not_null(&act, "type");
			a.url = string_not_null(&act, "url");
			if (act.find("buttons") != act.end()) {
				for (auto &b : act["buttons"]) {
					activity_button btn;
					if (b.is_string()) { // its may be just a string (label) because normal bots cannot access the button URLs
						btn.label = b.get<std::string>();;
					} else {
						btn.label = string_not_null(&b, "label");
						btn.url = string_not_null(&b, "url");
					}
					a.buttons.push_back(btn);
				}
			}
			if (act.find("emoji") != act.end()) {
				a.emoji.name = string_not_null(&act["emoji"], "name");
				a.emoji.id = snowflake_not_null(&act["emoji"], "id");
				if (bool_not_null(&act["emoji"], "animated")) {
					a.emoji.flags |= e_animated;
				}
			}
			if (act.find("party") != act.end()) {
				a.party.id = snowflake_not_null(&act["party"], "id");
				if (act["party"].find("size") != act["party"].end()) { // "size" is an array of two integers
					try {
						a.party.current_size = act["party"]["size"][0].get<int32_t>();
						a.party.maximum_size = act["party"]["size"][1].get<int32_t>();
					} catch ([[maybe_unused]] std::exception &exception) {}
				}
			}
			if (act.find("secrets") != act.end()) {
				a.secrets.join = string_not_null(&act["secret"], "join");
				a.secrets.spectate = string_not_null(&act["secret"], "spectate");
				a.secrets.match = string_not_null(&act["secret"], "match");
			}
			a.created_at = int64_not_null(&act, "created_at");
			if (act.find("timestamps") != act.end()) {
				a.start = int64_not_null(&act["timestamps"], "start");
				a.end = int64_not_null(&act["timestamps"], "end");
			}
			a.application_id = snowflake_not_null(&act, "application_id");
			a.flags = int8_not_null(&act, "flags");
			a.is_instance = bool_not_null(&act, "instance");

			activities.push_back(a);
		}
	}

	return *this;
}

json presence::to_json_impl(bool with_id) const {
	std::map<presence_status, std::string> status_name_mapping = {
		{ps_online, "online"},
		{ps_offline, "offline"},
		{ps_idle, "idle"},
		{ps_invisible, "invisible"},
		{ps_dnd, "dnd"}
	};
	json j({

		{"op", 3},
		{"d",	
			{
				{ "status", status_name_mapping[status()] },
				{ "since", json::value_t::null },
				{ "afk", false }
			}
		}
	});
	if (activities.size()) {
		for(const auto& i : activities){
			json j2({
				{ "name", i.name },
				{ "type", i.type }
			});
			if (!i.url.empty()) j2["url"] = i.url;

			if (i.type == activity_type::at_custom) {
			    	if (!i.state.empty()) j2["state"] = i.state; /* When type is custom, bot needs to use "state" */
			} else {
			    	if (!i.state.empty()) j2["details"] = i.state; /* Otherwise, the bot needs to use "details" */
			}

			j["d"]["activities"].push_back(j2);
		}
		/*j["d"]["game"] = json({ // use activities instead.
			{ "name", activities[0].name },
			{ "type", activities[0].type }
		});*/
	}

	return j;
}

json presence::to_json(bool with_id) const {
	return to_json_impl(with_id);
}

presence_status presence::desktop_status() const {
	return (presence_status)((flags >> PF_SHIFT_DESKTOP) & PF_STATUS_MASK);
}

presence_status presence::web_status() const {
	return (presence_status)((flags >> PF_SHIFT_WEB) & PF_STATUS_MASK);
}

presence_status presence::mobile_status() const {
	return (presence_status)((flags >> PF_SHIFT_MOBILE) & PF_STATUS_MASK);
}

presence_status presence::status() const {
	return (presence_status)((flags >> PF_SHIFT_MAIN) & PF_STATUS_MASK);
}

} // namespace dpp
