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
#pragma once

#include <dpp/export.h>
#include <dpp/json_fwd.h>
#include <dpp/json_interface.h>
#include <dpp/utility.h>
#include <string_view>
#include <functional>

namespace dpp {

/**
 * @brief Returns a snowflake id from a json field value, if defined, else returns 0
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
uint64_t DPP_EXPORT snowflake_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets a snowflake id from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_snowflake_not_null(const nlohmann::json* j, const char *keyname, uint64_t &v);

/**
 * @brief Sets an array of snowflakes from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for the values
 * @param v Value to change
 */
void DPP_EXPORT set_snowflake_array_not_null(const nlohmann::json* j, const char *keyname, std::vector<class snowflake> &v);

/**
 * @brief Applies a function to each element of a json array.
 * @param parent nlohmann::json instance to retrieve value from
 * @param key key name to check for the values
 * @param fn function to apply to each element
 */
void DPP_EXPORT for_each_json(nlohmann::json* parent, std::string_view key, const std::function<void(nlohmann::json*)> &fn);

/**
 * @brief Sets an array of objects from a json field value, if defined, else does nothing
 * @tparam T The class of which the array consists of. Must be derived from dpp::json_interface
 * @param j nlohmann::json instance to retrieve value from
 * @param key key name to check for the values
 * @param v Value to change
 */
template<class T> void set_object_array_not_null(nlohmann::json* j, std::string_view key, std::vector<T>& v) {
	v.clear();
	for_each_json(j, key, [&v](nlohmann::json* elem) {
		v.push_back(T{}.fill_from_json(elem));
	});
}

/**
 * @brief Returns a string from a json field value, if defined, else returns an empty string.
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
std::string DPP_EXPORT string_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets a string from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_string_not_null(const nlohmann::json* j, const char *keyname, std::string &v);

/**
 * @brief This is a repeat of set_string_not_null, but takes in a iconhash.
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_iconhash_not_null(const nlohmann::json* j, const char *keyname, utility::iconhash &v);

/**
 * @brief Returns a double from a json field value, if defined, else returns 0.
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
double DPP_EXPORT double_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets a double from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_double_not_null(const nlohmann::json* j, const char *keyname, double &v);

/**
 * @brief Returns a 64 bit unsigned integer from a json field value, if defined, else returns 0.
 * DO NOT use this for snowflakes, as usually snowflakes are wrapped in a string!
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
uint64_t DPP_EXPORT int64_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets an unsigned 64 bit integer from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_int64_not_null(const nlohmann::json* j, const char *keyname, uint64_t &v);

/**
 * @brief Returns a 32 bit unsigned integer from a json field value, if defined, else returns 0
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
uint32_t DPP_EXPORT int32_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets an unsigned 32 bit integer from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_int32_not_null(const nlohmann::json* j, const char *keyname, uint32_t &v);

/**
 * @brief Returns a 16 bit unsigned integer from a json field value, if defined, else returns 0
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
uint16_t DPP_EXPORT int16_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets an unsigned 16 bit integer from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_int16_not_null(const nlohmann::json* j, const char *keyname, uint16_t &v);

/**
 * @brief Returns an 8 bit unsigned integer from a json field value, if defined, else returns 0
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
uint8_t DPP_EXPORT int8_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets an unsigned 8 bit integer from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_int8_not_null(const nlohmann::json* j, const char *keyname, uint8_t &v);

/**
 * @brief Returns a boolean value from a json field value, if defined, else returns false
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
bool DPP_EXPORT bool_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets a boolean from a json field value, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_bool_not_null(const nlohmann::json* j, const char *keyname, bool &v);

/**
 * @brief Returns a time_t from an ISO8601 timestamp field in a json value, if defined, else returns
 * epoch value of 0.
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @return found value
 */
time_t DPP_EXPORT ts_not_null(const nlohmann::json* j, const char *keyname);

/**
 * @brief Sets an timestamp from a json field value containing an ISO8601 string, if defined, else does nothing
 * @param j nlohmann::json instance to retrieve value from
 * @param keyname key name to check for a value
 * @param v Value to change
 */
void DPP_EXPORT set_ts_not_null(const nlohmann::json* j, const char *keyname, time_t &v);

/**
 * @brief Base64 encode data into a string.
 * @param buf Raw binary buffer
 * @param buffer_length Buffer length to encode
 * @return The base64 encoded string
 */
std::string DPP_EXPORT base64_encode(unsigned char const* buf, unsigned int buffer_length);

/**
 * @brief Convert time_t unix epoch to std::string ISO date/time
 * 
 * @param ts Timestamp to convert
 * @return std::string Converted time/date string
 */
std::string DPP_EXPORT ts_to_string(time_t ts);

} // namespace dpp
