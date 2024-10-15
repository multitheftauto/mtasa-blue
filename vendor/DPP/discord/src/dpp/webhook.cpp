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
#include <dpp/webhook.h>
#include <dpp/discordevents.h>
#include <dpp/json.h>
#include <dpp/exception.h>

namespace dpp {

using json = nlohmann::json;

const size_t MAX_ICON_SIZE = 256 * 1024;

webhook::webhook() : managed(), type(w_incoming), guild_id(0), channel_id(0), application_id(0)
{
}

webhook::webhook(const std::string& webhook_url) : webhook()
{
	auto pos = webhook_url.find_last_of('/');
	if (pos == std::string::npos) { // throw when the url doesn't contain a slash at all
		throw dpp::logic_exception(err_invalid_webhook, std::string("Failed to parse webhook URL: No '/' found in the webhook url"));
	}
	try {
		token = webhook_url.substr(pos + 1);
		std::string endpoint = "https://discord.com/api/webhooks/";
		id = std::stoull(webhook_url.substr(endpoint.size(), pos));
	}
	catch (const std::exception& e) {
		throw dpp::logic_exception(err_invalid_webhook, std::string("Failed to parse webhook URL: ") + e.what());
	}
}

webhook::webhook(const snowflake webhook_id, const std::string& webhook_token) : webhook()
{
	token = webhook_token;
	id = webhook_id;
}

webhook& webhook::fill_from_json_impl(nlohmann::json* j) {
	set_snowflake_not_null(j, "id", id);
	set_int8_not_null(j, "type", type);
	set_snowflake_not_null(j, "guild_id", guild_id);
	set_snowflake_not_null(j, "channel_id", channel_id);
	if (j->contains("user")) {
		user_obj = user().fill_from_json(&((*j)["user"]));
	}
	set_string_not_null(j, "name", name);
	set_iconhash_not_null(j, "avatar", avatar);
	set_string_not_null(j, "token", token);
	set_snowflake_not_null(j, "application_id", application_id);
	if (j->contains("source_guild")) {
		source_guild = guild().fill_from_json(&((*j)["source_guild"]));
	}
	if (j->contains("source_channel")) {
		source_channel = channel().fill_from_json(&((*j)["source_channel"]));
	}
	set_string_not_null(j, "url", url);

	return *this;
}

json webhook::to_json_impl(bool with_id) const {
	json j;
	j["name"] = name;
	if (channel_id) {
		j["channel_id"] = channel_id;
	}
	if (!image_data.empty()) {
		j["avatar"] = image_data;
	}
	return j;
}

webhook& webhook::load_image(const std::string &image_blob, const image_type type, bool is_base64_encoded) {
	if (image_blob.size() > MAX_ICON_SIZE) {
		throw dpp::length_exception(err_icon_size, "Webhook icon file exceeds discord limit of 256 kilobytes");
	}

	image_data = "data:" + utility::mime_type(type) + ";base64," + (is_base64_encoded ? image_blob : base64_encode(reinterpret_cast<unsigned char const*>(image_blob.data()), static_cast<unsigned int>(image_blob.length())));

	return *this;
}

} // namespace dpp

