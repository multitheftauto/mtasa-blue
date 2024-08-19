/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * Copyright 2022 Craig Edwards and D++ contributors
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
#include <dpp/cluster.h>
#include <dpp/invite.h>
#include <dpp/json_fwd.h>

namespace dpp {

/**
 * @brief Templated REST request helper to save on typing
 * 
 * @tparam T type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param callback Callback lambda
 */
template<class T> inline void rest_request(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback) {
	c->post_rest(basepath, major, minor, method, postdata, [c, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(c, T().fill_from_json(&j), http));
		}
	});
};

/**
 * @brief Templated REST request helper to save on typing (specialised for message)
 * 
 * @tparam T type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param callback Callback lambda
 */
template<> inline void rest_request<message>(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback) {
	c->post_rest(basepath, major, minor, method, postdata, [c, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(c, message(c).fill_from_json(&j), http));
		}
	});
};

/**
 * @brief Templated REST request helper to save on typing (specialised for confirmation)
 * 
 * @tparam T type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param callback Callback lambda
 */
template<> inline void rest_request<confirmation>(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback) {
	c->post_rest(basepath, major, minor, method, postdata, [c, callback](json &j, const http_request_completion_t& http) {
		if (callback) {
			callback(confirmation_callback_t(c, confirmation(), http));
		}
	});
};

/**
 * @brief Templated REST request helper to save on typing (for returned lists)
 *  
 * @tparam T singular type to return in lambda callback
 * @tparam T map type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param key Key name of elements in the json list
 * @param callback Callback lambda
 */
template<class T> inline void rest_request_list(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback, const std::string& key = "id") {
	c->post_rest(basepath, major, minor, method, postdata, [c, key, callback](json &j, const http_request_completion_t& http) {
		std::unordered_map<snowflake, T> list;
		confirmation_callback_t e(c, confirmation(), http);
		if (!e.is_error()) {
			for (auto & curr_item : j) {
				list[snowflake_not_null(&curr_item, key.c_str())] = T().fill_from_json(&curr_item);
			}
		}
		if (callback) {
			callback(confirmation_callback_t(c, list, http));
		}
	});
}

/**
 * @brief Templated REST request helper to save on typing (for returned lists, specialised for invites)
 *  
 * @tparam T singular type to return in lambda callback
 * @tparam T map type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param key Key name of elements in the json list
 * @param callback Callback lambda
 */
template<> inline void rest_request_list<invite>(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback, const std::string& key) {
	c->post_rest(basepath, major, minor, method, postdata, [c, callback](json &j, const http_request_completion_t& http) {
		invite_map list;
		confirmation_callback_t e(c, confirmation(), http);
		if (!e.is_error()) {
			for (auto & curr_item : j) {
				list[string_not_null(&curr_item, "code")] = invite().fill_from_json(&curr_item);
			}
		}
		if (callback) {
			callback(confirmation_callback_t(c, list, http));
		}
	});
}

/**
 * @brief Templated REST request helper to save on typing (for returned lists, specialised for voiceregions)
 *  
 * @tparam T singular type to return in lambda callback
 * @tparam T map type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param key Key name of elements in the json list
 * @param callback Callback lambda
 */
template<> inline void rest_request_list<voiceregion>(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback, const std::string& key) {
	c->post_rest(basepath, major, minor, method, postdata, [c, callback](json &j, const http_request_completion_t& http) {
		voiceregion_map list;
		confirmation_callback_t e(c, confirmation(), http);
		if (!e.is_error()) {
			for (auto & curr_item : j) {
				list[string_not_null(&curr_item, "id")] = voiceregion().fill_from_json(&curr_item);
			}
		}
		if (callback) {
			callback(confirmation_callback_t(c, list, http));
		}
	});
}

/**
 * @brief Templated REST request helper to save on typing (for returned lists, specialised for bans)
 *
 * @tparam T singular type to return in lambda callback
 * @tparam T map type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param key Key name of elements in the json list
 * @param callback Callback lambda
 */
template<> inline void rest_request_list<ban>(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback, const std::string& key) {
	c->post_rest(basepath, major, minor, method, postdata, [c, callback](json &j, const http_request_completion_t& http) {
		std::unordered_map<snowflake, ban> list;
		confirmation_callback_t e(c, confirmation(), http);
		if (!e.is_error()) {
			for (auto & curr_item : j) {
				ban curr_ban = ban().fill_from_json(&curr_item);
				list[curr_ban.user_id] = curr_ban;
			}
		}
		if (callback) {
			callback(confirmation_callback_t(c, list, http));
		}
	});
}

/**
 * @brief Templated REST request helper to save on typing (for returned lists, specialised for sticker packs)
 *
 * @tparam T singular type to return in lambda callback
 * @tparam T map type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param key Key name of elements in the json list
 * @param callback Callback lambda
 */
template<> inline void rest_request_list<sticker_pack>(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback, const std::string& key) {
	c->post_rest(basepath, major, minor, method, postdata, [c, key, callback](json &j, const http_request_completion_t& http) {
		std::unordered_map<snowflake, sticker_pack> list;
		confirmation_callback_t e(c, confirmation(), http);
		if (!e.is_error()) {
			if (j.contains("sticker_packs")) {
				for (auto &curr_item: j["sticker_packs"]) {
					list[snowflake_not_null(&curr_item, key.c_str())] = sticker_pack().fill_from_json(&curr_item);
				}
			}
		}
		if (callback) {
			callback(confirmation_callback_t(c, list, http));
		}
	});
}

/**
 * @brief Templated REST request helper to save on typing (for returned lists)
 *  
 * @tparam T singular type to return in lambda callback
 * @tparam T map type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param key Key name of elements in the json list
 * @param root Root element to look for
 * @param callback Callback lambda
 */
template<class T> inline void rest_request_list(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback, const std::string& key, const std::string& root) {
	c->post_rest(basepath, major, minor, method, postdata, [c, root, key, callback](json &j, const http_request_completion_t& http) {
		std::unordered_map<snowflake, T> list;
		confirmation_callback_t e(c, confirmation(), http);
		if (!e.is_error()) {
			for (auto & curr_item : j[root]) {
				list[snowflake_not_null(&curr_item, key.c_str())] = T().fill_from_json(&curr_item);
			}
		}
		if (callback) {
			callback(confirmation_callback_t(c, list, http));
		}
	});
}

/**
 * @brief Templated REST request helper to save on typing (for returned lists, specialised for objects which doesn't have ids)
 *
 * @tparam T singular type to return in lambda callback
 * @tparam T vector type to return in lambda callback
 * @param c calling cluster
 * @param basepath base path for API call
 * @param major major API function
 * @param minor minor API function
 * @param method HTTP method
 * @param postdata Post data or empty string
 * @param callback Callback lambda
 */
template<class T> inline void rest_request_vector(dpp::cluster* c, const char* basepath, const std::string &major, const std::string &minor, http_method method, const std::string& postdata, command_completion_event_t callback) {
	c->post_rest(basepath, major, minor, method, postdata, [c, callback](json &j, const http_request_completion_t& http) {
		std::vector<T> list;
		confirmation_callback_t e(c, confirmation(), http);
		if (!e.is_error()) {
			for (auto & curr_item : j) {
				list.push_back(T().fill_from_json(&curr_item));
			}
		}
		if (callback) {
			callback(confirmation_callback_t(c, list, http));
		}
	});
}


} // namespace dpp
