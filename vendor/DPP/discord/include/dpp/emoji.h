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
#include <dpp/utility.h>
#include <dpp/json_fwd.h>
#include <unordered_map>
#include <dpp/json_interface.h>

namespace dpp {

#define MAX_EMOJI_SIZE 256 * 1024

/**
 * @brief Flags for dpp::emoji
 */
enum emoji_flags : uint8_t {
	/**
	 * @brief Emoji requires colons.
	 */
	e_require_colons = 0b00000001,

	/**
	 * @brief Managed (introduced by application)
	 */
	e_managed = 0b00000010,

	/**
	 * @brief Animated emoji.
	 */
	e_animated = 0b00000100,

	/**
	 * @brief Available (false if the guild doesn't meet boosting criteria, etc)
	 */
	e_available = 0b00001000,
};

/**
 * @brief Represents an emoji for a dpp::guild
 */
class DPP_EXPORT emoji : public managed, public json_interface<emoji> {
protected:
	friend struct json_interface<emoji>;

	/**
	 * @brief Read class values from json object
	 *
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	emoji& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build the json for this object
	 *
	 * @param with_id include the id in the JSON
	 * @return std::string json data
	 */
	json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief Emoji name.
	 */
	std::string name{};

	/**
	 * @brief Roles allowed to use this emoji.
	 */
	std::vector<snowflake> roles;

	/**
	 * @brief The id of the user that created this emoji.
	 */
	snowflake user_id;

	/**
	 * @brief Image data for the emoji, if uploading.
	 */
	utility::image_data image_data;

	/**
	 * @brief Flags for the emoji from dpp::emoji_flags.
	 */
	uint8_t flags{0};

	/**
	 * @brief Construct a new emoji object
	 */
	emoji() = default;

	/**
	 * @brief Construct a new emoji object with name, ID and flags
	 *
	 * @param name The emoji's name
	 * @param id ID, if it has one (unicode does not)
	 * @param flags Emoji flags (emoji_flags)
	 */
	emoji(const std::string_view name, const snowflake id = 0, const uint8_t flags = 0);

	/**
	 * @brief Copy constructor, copies another emoji's data
	 *
	 * @param rhs Emoji to copy
	 */
	emoji(const emoji &rhs) = default;

	/**
	 * @brief Move constructor, moves another emoji's data to this
	 *
	 * @param rhs Emoji to move from
	 */
	emoji(emoji &&rhs) noexcept = default;

	/**
	 * @brief Destroy the emoji object
	 */
	~emoji() override = default;

	/**
	 * @brief Copy assignment operator, copies another emoji's data
	 *
	 * @param rhs Emoji to copy
	 */
	emoji &operator=(const emoji &rhs) = default;

	/**
	 * @brief Move constructor, moves another emoji's data to this
	 *
	 * @param rhs Emoji to move from
	 */
	emoji &operator=(emoji &&rhs) noexcept = default;

	/**
	* @brief Create a mentionable emoji
	* @param name The name of the emoji.
	* @param id The ID of the emoji.
	* @param is_animated is emoji animated.
	* @return std::string The formatted mention of the emoji.
	*/
	static std::string get_mention(std::string_view name, snowflake id, bool is_animated = false);

	/**
	 * @brief Emoji requires colons
	 *
	 * @return true Requires colons
	 * @return false Does not require colons
	 */
	bool requires_colons() const;

	/**
	 * @brief Emoji is managed
	 *
	 * @return true Is managed
	 * @return false Is not managed
	 */
	bool is_managed() const;

	/**
	 * @brief Emoji is animated
	 * 
	 * @return true Is animated
	 * @return false Is noy animated
	 */
	bool is_animated() const;

	/**
	 * @brief Is available
	 *
	 * @return true Is available
	 * @return false Is unavailable
	 */
	bool is_available() const;

	/**
	 * @brief Load an image into the object
	 *
	 * @param image_blob Image binary data
	 * @param type Type of image. It can be one of `i_gif`, `i_jpg` or `i_png`.
	 * @return emoji& Reference to self
	 * @throw dpp::length_exception Image content exceeds discord maximum of 256 kilobytes
	 */
	emoji& load_image(std::string_view image_blob, const image_type type);

	/**
	 * @brief Load an image into the object
	 *
	 * @param data Image binary data
	 * @param size Size of the image.
	 * @param type Type of image. It can be one of `i_gif`, `i_jpg` or `i_png`.
	 * @return emoji& Reference to self
	 * @throw dpp::length_exception Image content exceeds discord maximum of 256 kilobytes
	 */
	emoji& load_image(const std::byte* data, uint32_t size, const image_type type);

	/**
	 * @brief Format to name if unicode, name:id if has id or a:name:id if animated
	 *
	 * @return Formatted name for reactions
	 */
	std::string format() const;

	/**
	 * @brief Get the mention/ping for the emoji
	 * 
	 * @return std::string mention
	 */
	std::string get_mention() const;

	/**
	 * @brief Get the custom emoji url
	 *
	 * @param size The size of the emoji in pixels. It can be any power of two between 16 and 4096,
	 * otherwise the default sized emoji is returned.
	 * @param format The format to use for the emoji. It can be one of `i_webp`, `i_jpg`, `i_png` or `i_gif`.
	 * When passing `i_gif`, it returns an empty string for non-animated emojis. Consider using the `prefer_animated` parameter instead.
	 * @param prefer_animated Whether you prefer gif format.
	 * If true, it'll return gif format whenever the emoji is available as animated.
	 * @return std::string emoji url or an empty string, if the id is not set
	 */
	std::string get_url(uint16_t size = 0, const image_type format = i_png, bool prefer_animated = true) const;
};

/**
 * @brief Group of emojis
 */
typedef std::unordered_map<snowflake, emoji> emoji_map;

} // namespace dpp
