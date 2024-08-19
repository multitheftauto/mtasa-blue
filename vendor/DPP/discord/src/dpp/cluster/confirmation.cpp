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
#include <dpp/cluster.h>
#include <dpp/json.h>

namespace dpp {


confirmation_callback_t::confirmation_callback_t(cluster* creator, const confirmable_t& _value, const http_request_completion_t& _http)
	: http_info(_http), value(_value), bot(creator)
{
	if (std::holds_alternative<confirmation>(_value)) {
		confirmation newvalue = std::get<confirmation>(_value);
		newvalue.success = (http_info.status < 400);
		value = newvalue;
	}
}

confirmation_callback_t::confirmation_callback_t(const http_request_completion_t& _http)
	: http_info(_http),  value(), bot(nullptr)
{
}

confirmation_callback_t::confirmation_callback_t(cluster* creator) : bot(creator) {
	http_info = {};
	value = {};
}

bool confirmation_callback_t::is_error() const {
	if (http_info.status >= 400) {
		/* Invalid JSON or 4xx/5xx response */
		return true;
	}
	if (http_info.status == 204) {
		/* Body is empty so we can't parse it but interaction is not an error*/
		return false;
	}
	try {
		json j = json::parse(this->http_info.body);
		if (j.find("code") != j.end() && j.find("errors") != j.end() && j.find("message") != j.end()) {
			if (j["code"].is_number_unsigned() && j["errors"].is_object() && j["message"].is_string()) {
				return true;
			} else {
				return false;
			}
		}
		return false;
	}
	catch (const std::exception &) {
		/* JSON parse error indicates the content is not JSON.
		 * This means that its an empty body e.g. 204 response, and not an actual error.
		 */
		return false;
	}
}

namespace {

std::vector<error_detail> find_errors_in_object(const std::string& obj, const std::string& current_field, const json &j) {
	std::vector<error_detail> ret;

	if (auto errors = j.find("_errors"); errors != j.end()) {
		for (const json& errordetails : *errors) {
			error_detail detail;
			detail.code = errordetails["code"].get<std::string>();
			detail.reason = errordetails["message"].get<std::string>();
			detail.field = current_field;
			detail.object = obj;
			ret.emplace_back(detail);
		}
	} else {
		for (auto it = j.begin(); it != j.end(); ++it) {
			std::vector<error_detail> sub_errors;
			std::string               field;

			if (obj.empty()) {
				field = current_field;
			} else if (isdigit(*current_field.c_str())) {
				/* An element of an array, e.g. an element of a slash command vector for global_bulk_slash_command_create */
				field = obj;
				field += '[';
				field += current_field;
				field += ']';
			} else {
				/* A field of an object, e.g. message.content too long */
				field = obj;
				field += '.';
				field += current_field;
			}

			sub_errors = find_errors_in_object(field, it.key(), *it);

			if (!sub_errors.empty()) {
				ret.reserve(ret.capacity() + sub_errors.size());
				std::move(sub_errors.begin(), sub_errors.end(), std::back_inserter(ret));
			}
		}
	}
	return ret;
}

}

error_info confirmation_callback_t::get_error() const {
	if (is_error()) {
		try {
			json j = json::parse(this->http_info.body);
			error_info e;

			set_int32_not_null(&j, "code", e.code);
			set_string_not_null(&j, "message", e.message);
			json &errors = j["errors"];
			for (auto obj = errors.begin(); obj != errors.end(); ++obj) {
				std::vector<error_detail> sub_errors;
				std::string field = isdigit(*obj.key().c_str()) ? "<array>[" + obj.key() + "]" : obj.key();

				sub_errors = find_errors_in_object({}, field, *obj);

				if (!sub_errors.empty()) {
					e.errors.reserve(e.errors.capacity() + sub_errors.size());
					std::move(sub_errors.begin(), sub_errors.end(), std::back_inserter(e.errors));
				}
			}

			e.human_readable = std::to_string(e.code) + ": " + e.message;
			std::string prefix = e.errors.size() == 1 ? " " : "\n\t";
			for (const auto &error: e.errors) {
				if (error.object.empty()) {
					/* A singular field with an error in an unnamed object */
					e.human_readable += prefix + "- " + error.field + ": " + error.reason + " (" + error.code + ")";
				} else {
					/* An object field that caused an error */
					e.human_readable += prefix + "- " + error.object + '.' + error.field + ": " + error.reason + " (" + error.code + ")";
				}
			}

			return e;
		}
		catch (const std::exception &e) {
			return {};
		}
	}
	return {};
}

} // namespace dpp
