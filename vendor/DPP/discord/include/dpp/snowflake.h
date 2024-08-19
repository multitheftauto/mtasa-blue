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
#include <dpp/json_fwd.h>
#include <dpp/exception.h>
#include <cstdint>
#include <type_traits>

/**
 * @brief The main namespace for D++ functions. classes and types
 */
namespace dpp {

/** @brief A container for a 64 bit unsigned value representing many things on discord.
 * This value is known in distributed computing as a snowflake value.
 *
 * Snowflakes are:
 *
 * - Performant (very fast to generate at source and to compare in code)
 * - Uncoordinated (allowing high availability across clusters, data centres etc)
 * - Time ordered (newer snowflakes have higher IDs)
 * - Directly Sortable (due to time ordering)
 * - Compact (64 bit numbers, not 128 bit, or string)
 *
 * An identical format of snowflake is used by Twitter, Instagram and several other platforms.
 *
 * @see https://en.wikipedia.org/wiki/Snowflake_ID
 * @see https://github.com/twitter-archive/snowflake/tree/b3f6a3c6ca8e1b6847baa6ff42bf72201e2c2231
 */
class DPP_EXPORT snowflake final {
	friend struct std::hash<dpp::snowflake>;
protected:
	/**
	 * @brief The snowflake value
	 */
	uint64_t value = 0;

public:
	/**
	 * @brief Construct a snowflake object
	 */
	constexpr snowflake() noexcept = default;

	/**
	 * @brief Copy a snowflake object
	 */
	constexpr snowflake(const snowflake &rhs) noexcept = default;

	/**
	 * @brief Move a snowflake object
	 */
	constexpr snowflake(snowflake &&rhs) noexcept = default;

	/**
	 * @brief Construct a snowflake from an integer value
	 *
	 * @throw dpp::logic_exception on assigning a negative value. The function is noexcept if the type given is unsigned
	 * @param snowflake_val snowflake value as an integer type
	 */
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>>
	constexpr snowflake(T snowflake_val) noexcept(std::is_unsigned_v<T>) : value(static_cast<std::make_unsigned_t<T>>(snowflake_val)) {
		/**
		 * we cast to the unsigned version of the type given - this maintains "possible loss of data" warnings for sizeof(T) > sizeof(value)
		 * while suppressing them for signed to unsigned conversion (for example snowflake(42) will call snowflake(int) which is a signed type)
		 */
		if constexpr (!std::is_unsigned_v<T>) {
			/* if the type is signed, at compile-time, add a check at runtime that the value is unsigned */
			if (snowflake_val < 0) {
				value = 0;
				throw dpp::logic_exception{"cannot assign a negative value to dpp::snowflake"};
			}
		}
	}

	/**
	 * @brief Construct a snowflake object from an unsigned integer in a string
	 *
	 * On invalid string the value will be 0
	 * @param string_value A snowflake value
	 */
	snowflake(std::string_view string_value) noexcept;

	/**
	 * @brief Construct a snowflake object from an unsigned integer in a string
	 *
	 * On invalid string the value will be 0
	 * @param string_value A snowflake value
	 */
	template <typename T, typename = std::enable_if_t<std::is_same_v<T, std::string>>>
	snowflake(const T &string_value) noexcept : snowflake(std::string_view{string_value}) {}
	/* ^ this exists to preserve `message_cache.find(std::get<std::string>(event.get_parameter("message_id")));` */

	/**
	 * @brief Copy value from another snowflake
	 *
	 * @param rhs The snowflake to copy from
	 */
	constexpr dpp::snowflake &operator=(const dpp::snowflake& rhs) noexcept = default;

	/**
	 * @brief Move value from another snowflake
	 *
	 * @param rhs The snowflake to move from
	 */
	constexpr dpp::snowflake &operator=(dpp::snowflake&& rhs) noexcept = default;

	/**
	 * @brief Assign integer value to the snowflake
	 *
	 * @throw dpp::logic_exception on assigning a negative value. The function is noexcept if the type given is unsigned
	 * @param snowflake_val snowflake value as an integer type
	 */
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	constexpr dpp::snowflake &operator=(T snowflake_val) noexcept(std::is_unsigned_v<T>) {
		return *this = dpp::snowflake{snowflake_val};
	}

	/**
	 * @brief Assign value converted from a string to the snowflake
	 *
	 * On invalid string the value will be 0
	 * @param snowflake_val snowflake value as a string
	 */
	template <typename T, typename = std::enable_if_t<std::is_convertible_v<T, std::string_view>>>
	constexpr dpp::snowflake &operator=(T&& snowflake_val) noexcept {
		return *this = dpp::snowflake{std::forward<T>(snowflake_val)};
	}

	/**
	 * @brief Returns true if the snowflake holds an empty value (is 0)
	 *
	 * @return true if empty (zero)
	 */
	constexpr bool empty() const noexcept {
		return value == 0;
	}

	/**
	 * @brief Returns the stringified version of the snowflake value
	 *
	 * @return std::string string form of snowflake value
	 */
	inline std::string str() const {
		return std::to_string(value);
	}

	/**
	 * @brief Comparison operator with another snowflake
	 *
	 * @param snowflake_val snowflake
	 */
	constexpr bool operator==(dpp::snowflake snowflake_val) const noexcept {
		return value == snowflake_val.value;
	}

	/**
	 * @brief Comparison operator with a string
	 *
	 * @param snowflake_val snowflake value as a string
	 */
	bool operator==(std::string_view snowflake_val) const noexcept;

	/**
	 * @brief Comparison operator with an integer
	 *
	 * @param snowflake_val snowflake value as an integer type
	 */
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	constexpr bool operator==(T snowflake_val) const noexcept {
		/* We use the std::enable_if_t trick to disable implicit conversions so there is a perfect candidate for overload resolution for integers, and it isn't ambiguous */
		return *this == dpp::snowflake{snowflake_val};
	}

	/**
	 * @brief For acting like an integer
	 * @return The snowflake value
	 */
	constexpr operator uint64_t() const noexcept {
		return value;
	}

	/**
	 * @brief For acting like an integer
	 * @return A reference to the snowflake value
	 */
	constexpr operator uint64_t &() noexcept {
		return value;
	}

	/**
	 * @brief For building json
	 * @return The snowflake value as a string
	 */
	operator json() const;

	/**
	 * @brief Get the creation time of this snowflake according to Discord.
	 *
	 * @return double creation time inferred from the snowflake ID.
	 * The minimum possible value is the first second of 2015.
	 */
	constexpr double get_creation_time() const noexcept {
		constexpr uint64_t first_january_2016 = 1420070400000ull;
		return static_cast<double>((value >> 22) + first_january_2016) / 1000.0;
	}

	/**
	 * @brief Get the worker id that produced this snowflake value
	 *
	 * @return uint8_t worker id
	 */
	constexpr uint8_t get_worker_id() const noexcept {
		return static_cast<uint8_t>((value & 0x3E0000) >> 17);
	}

	/**
	 * @brief Get the process id that produced this snowflake value
	 *
	 * @return uint8_t process id
	 */
	constexpr uint8_t get_process_id() const noexcept {
		return static_cast<uint8_t>((value & 0x1F000) >> 12);
	}

	/**
	 * @brief Get the increment, which is incremented for every snowflake
	 * created over the one millisecond resolution in the timestamp.
	 *
	 * @return uint64_t millisecond increment
	 */
	constexpr uint16_t get_increment() const noexcept {
		return static_cast<uint16_t>(value & 0xFFF);
	}

	/**
	 * @brief Helper function for libfmt so that a snowflake can be directly formatted as a uint64_t.
	 *
	 * @see https://fmt.dev/latest/api.html#formatting-user-defined-types
	 * @return uint64_t snowflake ID
	 */
	friend constexpr uint64_t format_as(snowflake s) noexcept {
		/* note: this function must stay as "friend" - this declares it as a free function but makes it invisible unless the argument is snowflake
		 * this effectively means no implicit conversions are performed to snowflake, for example format_as(0) doesn't call this function */
		return s.value;
	}
};

} // namespace dpp

template<>
struct std::hash<dpp::snowflake>
{
	/**
	 * @brief Hashing function for dpp::snowflake
	 * Used by std::unordered_map. This just calls std::hash<uint64_t>.
	 *
	 * @param s Snowflake value to hash
	 * @return std::size_t hash value
	 */
	std::size_t operator()(dpp::snowflake s) const noexcept {
		return std::hash<uint64_t>{}(s.value);
	}
};
