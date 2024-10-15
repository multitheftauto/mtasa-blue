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
#include <dpp/managed.h>
#include <dpp/json_fwd.h>
#include <dpp/json_interface.h>
#include <unordered_map>

namespace dpp {

/**
 * @brief The type of SKU.
 * */
enum sku_type : uint8_t {
	/**
	 * @brief Represents a recurring subscription
	 */
	SUBSCRIPTION = 5,

	/**
	 * @brief System-generated group for each SUBSCRIPTION SKU created
	 * @warning These are automatically created for each subscription SKU and are not used at this time. Please refrain from using these.
	 */
	SUBSCRIPTION_GROUP = 6,
};

/**
 * @brief SKU flags.
 */
enum sku_flags : uint16_t {
	/**
	 * @brief SKU is available for purchase
	 */
	sku_available =			0b000000000000100,

	/**
	 * @brief Recurring SKU that can be purchased by a user and applied to a single server. Grants access to every user in that server.
	 */
	sku_guild_subscription = 	0b000000010000000,

	/**
	 * @brief Recurring SKU purchased by a user for themselves. Grants access to the purchasing user in every server.
	 */
	sku_user_subscription =		0b000000100000000,
};

/**
 * @brief A definition of a discord SKU.
 */
class DPP_EXPORT sku : public managed, public json_interface<sku> {
protected:
	friend struct json_interface<sku>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	sku& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build json for this SKU object
	 *
	 * @param with_id include the ID in the json
	 * @return json JSON object
	 */
	json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief The type of SKU.
	 */
	sku_type type{sku_type::SUBSCRIPTION};

	/**
	 * @brief ID of the parent application
	 */
	snowflake application_id{0};

	/**
	 * @brief Customer-facing name of your premium offering
	 */
	std::string name{};

	/**
	 * @brief System-generated URL slug based on the SKU's name
	 */
	std::string slug{};

	/**
	 * @brief Flags bitmap from dpp::sku_flags
	 */
	uint16_t flags{0};

	/**
	 * @brief Construct a new SKU object
	 */
	sku() = default;

	/**
	 * @brief Construct a new SKU object with all data required.
	 *
	 * @param id ID of the SKU.
	 * @param type Type of SKU (sku_type).
	 * @param application_id ID of the parent application.
	 * @param name Customer-facing name of your premium offering.
	 * @param slug System-generated URL slug based on the SKU's name.
	 * @param flags Flags bitmap from dpp::sku_flags.
	 *
	 */
	sku(const snowflake id, const sku_type type, const snowflake application_id, const std::string name, const std::string slug, const uint16_t flags);

	/**
	 * @brief Get the type of SKU.
	 *
	 * @return sku_type SKU type
	 */
	sku_type get_type() const;

	/**
	 * @brief Is the SKU available for purchase?
	 *
	 * @return true if the SKU can be purchased.
	 */
	bool is_available() const;

	/**
	 * @brief Is the SKU a guild subscription?
	 *
	 * @return true if the SKU is a guild subscription.
	 */
	bool is_guild_subscription() const;

	/**
	 * @brief Is the SKU a user subscription?
	 *
	 * @return true if the SKU is a user subscription
	 */
	bool is_user_subscription() const;
};

/**
 * @brief Group of SKUs.
 */
typedef std::unordered_map<snowflake, sku> sku_map;

} // namespace dpp
