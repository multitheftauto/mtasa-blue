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
 * @brief The type of entitlement.
 * */
enum entitlement_type : uint8_t {
	/**
	 * @brief A subscription for a guild.
	 * @warning This can only be used when creating a test entitlement.
	 */
	GUILD_SUBSCRIPTION = 1,

	/**
	 * @brief A subscription for a user.
	 * @warning This can only be used when creating a test entitlement.
	 */
	USER_SUBSCRIPTION = 2,

	/**
	 * @brief Entitlement was purchased as an app subscription.
	 */
	APPLICATION_SUBSCRIPTION = 8
};

/**
 * @brief Entitlement flags.
 */
enum entitlement_flags : uint16_t {
	/**
	 * @brief Entitlement was deleted
	 */
	ent_deleted =		0b000000000000001,
};

/**
 * @brief A definition of a discord entitlement.
 */
class DPP_EXPORT entitlement : public managed, public json_interface<entitlement> {
protected:
	friend struct json_interface<entitlement>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	entitlement& fill_from_json_impl(nlohmann::json* j);

	/**
	 * @brief Build json for this entitlement object
	 *
	 * @param with_id include the ID in the json
	 * @return json JSON object
	 */
	json to_json_impl(bool with_id = false) const;

public:
	/**
	 * @brief ID of the SKU
	 */
	snowflake sku_id{0};

	/**
	 * @brief ID of the parent application
	 */
	snowflake application_id{0};

	/**
	 * @brief Optional: ID of the user/guild that is granted access to the entitlement's SKU
	 */
	snowflake owner_id{0};

	/**
	 * @brief The type of entitlement.
	 */
	entitlement_type type = entitlement_type::APPLICATION_SUBSCRIPTION;

	/**
	 * @brief Optional: Start date at which the entitlement is valid.
	 *
	 * @note Not present when using test entitlements.
	 */
	time_t starts_at{0};

	/**
	 * @brief Optional: Date at which the entitlement is no longer valid.
	 *
	 * @note Not present when using test entitlements.
	 */
	time_t ends_at{0};

	/**
	 * @brief Flags bitmap from dpp::entitlement_flags
	 */
	uint16_t flags{0};

	/**
	 * @brief Construct a new entitlement object
	 */
	entitlement() = default;

	/**
	 * @brief Construct a new entitlement object with sku_id, ID, application_id, type, and flags.
	 *
	 * @param sku_id The ID of the SKU.
	 * @param id The ID of the entitlement.
	 * @param application_id The ID of the parent application.
	 * @param type The type of entitlement (Should only ever be APPLICATION_SUBSCRIPTION unless you going to use this object as a parameter for dpp::cluster::entitlement_test_create).
	 * @param flags The flags for the SKU from dpp::entitlement_flags.
	 */
	entitlement(const snowflake sku_id, const snowflake id = 0, const snowflake application_id = 0, const entitlement_type type = dpp::entitlement_type::APPLICATION_SUBSCRIPTION, const uint8_t flags = 0);

	/**
	 * @brief Get the type of entitlement.
	 *
	 * @return entitlement_type Entitlement type
	 */
	entitlement_type get_type() const;

	/**
	 * @brief Was the entitlement deleted?
	 *
	 * @return true if the entitlement was deleted.
	 */
	bool is_deleted() const;
};

/**
 * @brief Group of entitlements.
 */
typedef std::unordered_map<snowflake, entitlement> entitlement_map;

} // namespace dpp
