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
#include <dpp/emoji.h>
#include <dpp/discordevents.h>
#include <dpp/json.h>
#include <dpp/exception.h>

namespace dpp {

using json = nlohmann::json;

emoji::emoji(const std::string_view n, const snowflake i, const uint8_t f) : managed(i), name(n), flags(f) {}

std::string emoji::get_mention(std::string_view name, snowflake id, bool is_animated) {
	return utility::emoji_mention(name,id,is_animated);
}

emoji& emoji::fill_from_json_impl(nlohmann::json* j) {
	id = snowflake_not_null(j, "id");
	name = string_not_null(j, "name");
	if (j->contains("user")) {
		json & user = (*j)["user"];
		user_id = snowflake_not_null(&user, "id");
	}

	if(j->contains("roles")) {
		for (const auto& role : (*j)["roles"]) {
			this->roles.emplace_back(to_string(role));
		}
	}

	if (bool_not_null(j, "require_colons")) {
		flags |= e_require_colons;
	}
	if (bool_not_null(j, "managed")) {
		flags |= e_managed;
	}
	if (bool_not_null(j, "animated")) {
		flags |= e_animated;
	}
	if (bool_not_null(j, "available")) {
		flags |= e_available;
	}
	return *this;
}

json emoji::to_json_impl(bool with_id) const {
	json j;
	if (with_id) {
		j["id"] = std::to_string(id);
	}
	j["name"] = name;
	if (!image_data.empty()) {
		j["image"] = image_data.to_nullable_json();
	}
	j["roles"] = json::array();
	for (const auto& role : roles) {
		j["roles"].push_back(role.str());
	}
	return j;
}

bool emoji::requires_colons() const {
	return flags & e_require_colons;
}

bool emoji::is_managed() const {
	return flags & e_managed;
}

bool emoji::is_animated() const {
	return flags & e_animated;
}

bool emoji::is_available() const {
	return flags & e_available;
}

emoji& emoji::load_image(std::string_view image_blob, const image_type type) {
	if (image_blob.size() > MAX_EMOJI_SIZE) {
		throw dpp::length_exception(err_icon_size, "Emoji file exceeds discord limit of 256 kilobytes");
	}
	image_data = utility::image_data{type, image_blob};
	return *this;
}

emoji& emoji::load_image(const std::byte *data, uint32_t size, const image_type type) {
	if (size > MAX_EMOJI_SIZE) {
		throw dpp::length_exception(err_icon_size, "Emoji file exceeds discord limit of 256 kilobytes");
	}
	image_data = utility::image_data{type, data, size};
	return *this;
}

std::string emoji::format() const {
	return id ? ((is_animated() ? "a:" : "") + name + ":" + std::to_string(id)) : name;
}

std::string emoji::get_mention() const {
	return utility::emoji_mention(name,id,is_animated());
}

std::string emoji::get_url(uint16_t size, const dpp::image_type format, bool prefer_animated) const {
	if (this->id) {
		return utility::cdn_endpoint_url({ i_jpg, i_png, i_webp, i_gif },
	 		"emojis/" + std::to_string(this->id),
		 	format, size, prefer_animated, is_animated());
	}

	return "";
}


} // namespace dpp

