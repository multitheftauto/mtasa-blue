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
#pragma once
#include <dpp/export.h>
#include <dpp/snowflake.h>
#include <dpp/misc-enum.h>
#include <dpp/managed.h>
#include <dpp/json_fwd.h>
#include <dpp/user.h>
#include <dpp/guild.h>
#include <dpp/channel.h>
#include <unordered_map>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Defines types of webhook
 */
enum webhook_type {
	/**
	 * @brief Incoming webhook.
	 */
	w_incoming = 1,

	/**
	 * @brief Channel following webhook.
	 */
	w_channel_follower = 2,

	/**
	 * @brief Application webhooks for interactions.
	 */
	w_application = 3
};

/**
 * @brief Represents a discord webhook
 */
class DPP_EXPORT webhook : public managed, public json_interface<webhook> {
protected:
	friend struct json_interface<webhook>;

	/**
	 * @brief Fill in object from json data
	 * 
	 * @param j JSON data
	 * @return webhook& Reference to self
	 */
	webhook& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build JSON string from object
	 * 
	 * @param with_id Include the ID of the webhook in the json
	 * @return std::string JSON encoded object
	 */
	virtual json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Type of the webhook from dpp::webhook_type.
	 */
	uint8_t type;

	/**
	 * @brief The guild id this webhook is for.
	 *
	 * @note This field is optional, and may also be empty.
	 */
	snowflake guild_id;

	/**
	 * @brief The channel id this webhook is for.
	 *
	 * @note This may be empty.
	 */
	snowflake channel_id;

	/**
	 * @brief The user this webhook was created by.
	 *
	 * @note This field is optional.
	 * @warning This is not returned when getting a webhook with its token!
	 */
	user user_obj;

	/**
	 * @brief The default name of the webhook.
	 *
	 * @note This may be empty.
	 */
	std::string name;

	/**
	 * @brief The default avatar of the webhook.
	 *
	 * @note This may be empty.
	 */
	utility::iconhash avatar;

	/**
	 * @brief The secure token of the webhook (returned for Incoming Webhooks).
	 *
	 * @note This field is optional.
	 */
	std::string token;

	/**
	 * @brief The bot/OAuth2 application that created this webhook.
	 *
	 * @note This may be empty.
	 */
	snowflake application_id;

	/**
	 * @brief The guild of the channel that this webhook is following (only for Channel Follower Webhooks).
	 *
	 * @warning This will be absent if the webhook creator has since lost access to the guild where the followed channel resides!
	 */
	guild source_guild;

	/**
	 * @brief The channel that this webhook is following (only for Channel Follower Webhooks).
	 *
	 * @warning This will be absent if the webhook creator has since lost access to the guild where the followed channel resides!
	 */
	channel	source_channel;

	/**
	 * @brief The url used for executing the webhook (returned by the webhooks OAuth2 flow).
	 */
	std::string url;

	/**
	 * @brief base64 encoded image data if uploading a new image.
	 *
	 * @warning You should only ever read data from here. If you want to set the data, use dpp::webhook::load_image.
	 */
	std::string image_data;

	/**
	 * @brief Construct a new webhook object
	 */
	webhook();

	/**
	 * @brief Construct a new webhook object using the Webhook URL provided by Discord
	 *
	 * @param webhook_url a fully qualified web address of an existing webhook
	 * @throw logic_exception if the webhook url could not be parsed
	 */
	webhook(const std::string& webhook_url);

	/**
	 * @brief Construct a new webhook object using the webhook ID and the webhook token
	 *
	 * @param webhook_id id taken from a link of an existing webhook
	 * @param webhook_token token taken from a link of an existing webhook
	 */
	webhook(const snowflake webhook_id, const std::string& webhook_token);

	/**
	 * @brief Base64 encode image data and allocate it to image_data
	 * 
	 * @param image_blob Binary image data
	 * @param type Image type. It can be one of `i_gif`, `i_jpg` or `i_png`.
	 * @param is_base64_encoded True if the image data is already base64 encoded
	 * @return webhook& Reference to self
	 * @throw dpp::length_exception Image data is larger than the maximum size of 256 kilobytes
	 */
	webhook& load_image(const std::string &image_blob, const image_type type, bool is_base64_encoded = false);
};

/**
 * @brief A group of webhooks
 */
typedef std::unordered_map<snowflake, webhook> webhook_map;

} // namespace dpp
