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

/* OpenBSD errors when xopen_source is defined.
 * We want to make sure that OpenBSD does not define it.
 */
#if !defined(__OpenBSD__)
	#ifndef _XOPEN_SOURCE
		#define _XOPEN_SOURCE
	#endif
#endif

#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <dpp/discordevents.h>
#include <dpp/discordclient.h>
#include <dpp/event.h>
#include <dpp/cache.h>
#include <dpp/stringops.h>
#include <dpp/json.h>
#include <time.h>
#include <iomanip>
#include <sstream>

char* crossplatform_strptime(const char* s, const char* f, struct tm* tm) {
	std::istringstream input(s);
	input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
	input >> std::get_time(tm, f);
	if (input.fail()) {
		return nullptr;
	}
    return (char*)(s + static_cast<int>(input.tellg()));
}

namespace dpp {

std::string ts_to_string(time_t ts) {
	std::ostringstream ss;
	struct tm t;
	#ifdef _WIN32
		gmtime_s(&t, &ts);
	#else
		gmtime_r(&ts, &t);
	#endif
	ss << std::put_time(&t, "%FT%TZ");
	return ss.str();
}

uint64_t snowflake_not_null(const json* j, const char *keyname) {
	/* Snowflakes are a special case. Pun intended.
	 * Because discord drinks the javascript kool-aid, they have to send 64 bit integers as strings as js can't deal with them
	 * even though we can. So, all snowflakes are sent and received wrapped as string values and must be read by nlohmann::json
	 * as string types, then converted from string to uint64_t. Checks for existence of the value, and that it is a string containing
	 * a number. If not, then this function returns 0.
	 */
	auto k = j->find(keyname);
	if (k != j->end()) {
		return !k->is_null() && k->is_string() ? strtoull(k->get<std::string>().c_str(), nullptr, 10) : 0;
	} else {
		return 0;
	}
}

void set_snowflake_not_null(const json* j, const char *keyname, uint64_t &v) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() && k->is_string() ? strtoull(k->get<std::string>().c_str(), nullptr, 10) : 0;
	}
}

void set_snowflake_array_not_null(const json* j, const char *keyname, std::vector<class snowflake> &v) {
	v.clear();
	auto k = j->find(keyname);
	if (k != j->end() && !k->is_null()) {
		v.reserve(j->at(keyname).size());
		for (const auto &id : j->at(keyname)) {
			v.emplace_back(std::strtoull(id.get<std::string>().c_str(), nullptr, 10));
		}
	}
}

void for_each_json(nlohmann::json* parent, std::string_view key, const std::function<void(nlohmann::json*)> &fn) {
	auto it = parent->find(key);
	if (it == parent->end() || it->is_null()) {
		return;
	}
	for (nlohmann::json &elem : *it) {
		fn(&elem);
	}
}

std::string string_not_null(const json* j, const char *keyname) {
	/* Returns empty string if the value is not a string, or is null or not defined */
	auto k = j->find(keyname);
	if (k != j->end()) {
		return !k->is_null() && k->is_string() ? k->get<std::string>() : "";
	} else {
		return const_cast< char* >("");
	}
}

void set_string_not_null(const json* j, const char *keyname, std::string &v) {
	/* Returns empty string if the value is not a string, or is null or not defined */
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() && k->is_string() ? k->get<std::string>() : "";
	}
}

void set_iconhash_not_null(const json* j, const char *keyname, utility::iconhash &v) {
	/* Returns empty string if the value is not a string, or is null or not defined */
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() && k->is_string() ? k->get<std::string>() : "";
	}
}

double double_not_null(const json* j, const char *keyname) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		return !k->is_null() && !k->is_string() ? k->get<double>() : 0;
	} else {
		return 0;
	}
}

void set_double_not_null(const json* j, const char *keyname, double &v) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() && !k->is_string() ? k->get<double>() : 0;
	}
}

uint64_t int64_not_null(const json* j, const char *keyname) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		return !k->is_null() && !k->is_string() ? k->get<uint64_t>() : 0;
	} else {
		return 0;
	}
}

void set_int64_not_null(const json* j, const char *keyname, uint64_t &v) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() && !k->is_string() ? k->get<uint64_t>() : 0;
	}
}


uint32_t int32_not_null(const json* j, const char *keyname) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		return !k->is_null() && !k->is_string() ? k->get<uint32_t>() : 0;
	} else {
		return 0;
	}
}

void set_int32_not_null(const json* j, const char *keyname, uint32_t &v) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() && !k->is_string() ? k->get<uint32_t>() : 0;
	}
}

uint16_t int16_not_null(const json* j, const char *keyname) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		return !k->is_null() && !k->is_string() ? k->get<uint16_t>() : 0;
	} else {
		return 0;
	}
}

void set_int16_not_null(const json* j, const char *keyname, uint16_t &v) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() && !k->is_string() ? k->get<uint16_t>() : 0;
	}
}

uint8_t int8_not_null(const json* j, const char *keyname) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		return !k->is_null() && !k->is_string() ? k->get<uint8_t>() : 0;
	} else {
		return 0;
	}
}

void set_int8_not_null(const json* j, const char *keyname, uint8_t &v) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() && !k->is_string() ? k->get<uint8_t>() : 0;
	}
}

bool bool_not_null(const json* j, const char *keyname) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		return !k->is_null() ? (k->get<bool>() == true) : false;
	} else {
		return false;
	}
}

void set_bool_not_null(const json* j, const char *keyname, bool &v) {
	auto k = j->find(keyname);
	if (k != j->end()) {
		v = !k->is_null() ? (k->get<bool>() == true) : false;
	}
}

std::string base64_encode(unsigned char const* buf, unsigned int buffer_length) {
	/* Quick and dirty base64 encode */
	static constexpr std::string_view to_base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static constexpr auto push = [](std::string &dst, unsigned char b0, unsigned char b1, unsigned char b2) {
		dst.push_back(to_base64[ ((b0 & 0xfc) >> 2) ]);
		dst.push_back(to_base64[ ((b0 & 0x03) << 4) + ((b1 & 0xf0) >> 4) ]);
		dst.push_back(to_base64[ ((b1 & 0x0f) << 2) + ((b2 & 0xc0) >> 6) ]);
		dst.push_back(to_base64[ ((b2 & 0x3f)) ]);
	};
	size_t ret_size = 4 * ((buffer_length + 2) / 3); // ceil(4*size/3)
	size_t i = 0;
	std::string ret;

	ret.reserve(ret_size);

	if (buffer_length > 2) { //    vvvvv avoid unsigned overflow
		while (i < buffer_length - 2) {
			push(ret, buf[i], buf[i + 1], buf[i + 2]);
			i += 3;
		}
	}
	size_t left = buffer_length - i;
	if (left >= 1) { // handle non-multiple of 3s, pad the end with =
		ret.push_back(to_base64[ ((buf[i] & 0xfc) >> 2) ]);
		if (left >= 2) {
			ret.push_back(to_base64[ ((buf[i] & 0x03) << 4) + ((buf[i + 1] & 0xf0) >> 4) ]);
			ret.push_back(to_base64[ ((buf[i + 1] & 0x0f) << 2) ]);
			ret.push_back('=');
		}
		else {
			ret.push_back(to_base64[ ((buf[i] & 0x03) << 4) ]);
			ret += "==";
		}
	}
	return ret;
}

time_t ts_not_null(const json* j, const char* keyname)
{
	/* Parses discord ISO 8061 timestamps to time_t, accounting for local time adjustment.
	 * Note that discord timestamps contain a decimal seconds part, which time_t and struct tm
	 * can't handle. We strip these out.
	 */
	time_t retval = 0;
	if (j->contains(keyname) && !(*j)[keyname].is_null() && (*j)[keyname].is_string()) {
		tm timestamp = {};
		std::string timedate = (*j)[keyname].get<std::string>();
		if (timedate.find('+') != std::string::npos) {
			if (timedate.find('.') != std::string::npos) {
				timedate = timedate.substr(0, timedate.find('.'));
			}
			crossplatform_strptime(timedate.substr(0, 19).c_str(), "%Y-%m-%dT%T", &timestamp);
			timestamp.tm_isdst = 0;
			#ifndef _WIN32
				retval = timegm(&timestamp);
			#else
				retval = _mkgmtime(&timestamp);
			#endif
		} else {
			crossplatform_strptime(timedate.substr(0, 19).c_str(), "%Y-%m-%d %T", &timestamp);
			#ifndef _WIN32
				retval = timegm(&timestamp);
			#else
				retval = _mkgmtime(&timestamp);
			#endif
		}
	}
	return retval;
}

void set_ts_not_null(const json* j, const char* keyname, time_t &v)
{
	/* Parses discord ISO 8061 timestamps to time_t, accounting for local time adjustment.
	 * Note that discord timestamps contain a decimal seconds part, which time_t and struct tm
	 * can't handle. We strip these out.
	 */
	if (j->contains(keyname) && !(*j)[keyname].is_null() && (*j)[keyname].is_string()) {
		time_t retval = 0;
		tm timestamp = {};
		std::string timedate = (*j)[keyname].get<std::string>();
		if (timedate.find('+') != std::string::npos) {
			if (timedate.find('.') != std::string::npos) {
				timedate = timedate.substr(0, timedate.find('.'));
			}
			crossplatform_strptime(timedate.substr(0, 19).c_str(), "%Y-%m-%dT%T", &timestamp);
			timestamp.tm_isdst = 0;
			#ifndef _WIN32
				retval = timegm(&timestamp);
			#else
				retval = _mkgmtime(&timestamp);
			#endif
		} else {
			crossplatform_strptime(timedate.substr(0, 19).c_str(), "%Y-%m-%d %T", &timestamp);
			#ifndef _WIN32
				retval = timegm(&timestamp);
			#else
				retval = _mkgmtime(&timestamp);
			#endif
		}
		v = retval;
	}
}

template <typename EventType>
static dpp::events::event* make_static_event() noexcept {
	static EventType event;
	return &event;
}

static const std::map<std::string, dpp::events::event*> event_map = {
	{ "__LOG__", make_static_event<dpp::events::logger>() },
	{ "GUILD_CREATE", make_static_event<dpp::events::guild_create>() },
	{ "GUILD_UPDATE", make_static_event<dpp::events::guild_update>() },
	{ "GUILD_DELETE", make_static_event<dpp::events::guild_delete>() },
	{ "GUILD_MEMBER_UPDATE", make_static_event<dpp::events::guild_member_update>() },
	{ "RESUMED", make_static_event<dpp::events::resumed>() },
	{ "READY", make_static_event<dpp::events::ready>() },
	{ "CHANNEL_CREATE", make_static_event<dpp::events::channel_create>() },
	{ "CHANNEL_UPDATE", make_static_event<dpp::events::channel_update>() },
	{ "CHANNEL_DELETE", make_static_event<dpp::events::channel_delete>() },
	{ "PRESENCE_UPDATE", make_static_event<dpp::events::presence_update>() },
	{ "TYPING_START", make_static_event<dpp::events::typing_start>() },
	{ "MESSAGE_CREATE", make_static_event<dpp::events::message_create>() },
	{ "MESSAGE_UPDATE", make_static_event<dpp::events::message_update>() },
	{ "MESSAGE_DELETE", make_static_event<dpp::events::message_delete>() },
	{ "MESSAGE_DELETE_BULK", make_static_event<dpp::events::message_delete_bulk>() },
	{ "MESSAGE_REACTION_ADD", make_static_event<dpp::events::message_reaction_add>() },
	{ "MESSAGE_REACTION_REMOVE", make_static_event<dpp::events::message_reaction_remove>() },
	{ "MESSAGE_REACTION_REMOVE_ALL", make_static_event<dpp::events::message_reaction_remove_all>() },
	{ "MESSAGE_REACTION_REMOVE_EMOJI", make_static_event<dpp::events::message_reaction_remove_emoji>() },
	{ "MESSAGE_POLL_VOTE_ADD", make_static_event<dpp::events::message_poll_vote_add>() },
	{ "MESSAGE_POLL_VOTE_REMOVE", make_static_event<dpp::events::message_poll_vote_remove>() },
	{ "CHANNEL_PINS_UPDATE", make_static_event<dpp::events::channel_pins_update>() },
	{ "GUILD_BAN_ADD", make_static_event<dpp::events::guild_ban_add>() },
	{ "GUILD_BAN_REMOVE", make_static_event<dpp::events::guild_ban_remove>() },
	{ "GUILD_EMOJIS_UPDATE", make_static_event<dpp::events::guild_emojis_update>() },
	{ "GUILD_INTEGRATIONS_UPDATE", make_static_event<dpp::events::guild_integrations_update>() },
	{ "INTEGRATION_CREATE", make_static_event<dpp::events::integration_create>() },
	{ "INTEGRATION_UPDATE", make_static_event<dpp::events::integration_update>() },
	{ "INTEGRATION_DELETE", make_static_event<dpp::events::integration_delete>() },
	{ "GUILD_MEMBER_ADD", make_static_event<dpp::events::guild_member_add>() },
	{ "GUILD_MEMBER_REMOVE", make_static_event<dpp::events::guild_member_remove>() },
	{ "GUILD_MEMBERS_CHUNK", make_static_event<dpp::events::guild_members_chunk>() },
	{ "GUILD_ROLE_CREATE", make_static_event<dpp::events::guild_role_create>() },
	{ "GUILD_ROLE_UPDATE", make_static_event<dpp::events::guild_role_update>() },
	{ "GUILD_ROLE_DELETE", make_static_event<dpp::events::guild_role_delete>() },
	{ "VOICE_STATE_UPDATE", make_static_event<dpp::events::voice_state_update>() },
	{ "VOICE_SERVER_UPDATE", make_static_event<dpp::events::voice_server_update>() },
	{ "WEBHOOKS_UPDATE", make_static_event<dpp::events::webhooks_update>() },
	{ "INVITE_CREATE", make_static_event<dpp::events::invite_create>() },
	{ "INVITE_DELETE", make_static_event<dpp::events::invite_delete>() },
	{ "INTERACTION_CREATE", make_static_event<dpp::events::interaction_create>() },
	{ "USER_UPDATE", make_static_event<dpp::events::user_update>() },
	{ "GUILD_JOIN_REQUEST_DELETE", make_static_event<dpp::events::guild_join_request_delete>() },
	{ "GUILD_JOIN_REQUEST_UPDATE", nullptr },
	{ "STAGE_INSTANCE_CREATE", make_static_event<dpp::events::stage_instance_create>() },
	{ "STAGE_INSTANCE_UPDATE", make_static_event<dpp::events::stage_instance_update>() },
	{ "STAGE_INSTANCE_DELETE", make_static_event<dpp::events::stage_instance_delete>() },
	{ "THREAD_CREATE", make_static_event<dpp::events::thread_create>() },
	{ "THREAD_UPDATE", make_static_event<dpp::events::thread_update>() },
	{ "THREAD_DELETE", make_static_event<dpp::events::thread_delete>() },
	{ "THREAD_LIST_SYNC", make_static_event<dpp::events::thread_list_sync>() },
	{ "THREAD_MEMBER_UPDATE", make_static_event<dpp::events::thread_member_update>() },
	{ "THREAD_MEMBERS_UPDATE", make_static_event<dpp::events::thread_members_update>() },
	{ "GUILD_STICKERS_UPDATE", make_static_event<dpp::events::guild_stickers_update>() },
	{ "GUILD_APPLICATION_COMMAND_COUNTS_UPDATE", nullptr },
	{ "APPLICATION_COMMAND_PERMISSIONS_UPDATE", nullptr },
	{ "EMBEDDED_ACTIVITY_UPDATE", nullptr },
	{ "GUILD_APPLICATION_COMMAND_INDEX_UPDATE", nullptr },
	{ "CHANNEL_TOPIC_UPDATE", nullptr },
	{ "GUILD_SOUNDBOARD_SOUND_CREATE", nullptr },
	{ "GUILD_SOUNDBOARD_SOUND_DELETE", nullptr },
	{ "GUILD_SOUNDBOARD_SOUNDS_UPDATE", nullptr },
	{ "VOICE_CHANNEL_STATUS_UPDATE", nullptr },
	{ "GUILD_SCHEDULED_EVENT_CREATE", make_static_event<dpp::events::guild_scheduled_event_create>() },
	{ "GUILD_SCHEDULED_EVENT_UPDATE", make_static_event<dpp::events::guild_scheduled_event_update>() },
	{ "GUILD_SCHEDULED_EVENT_DELETE", make_static_event<dpp::events::guild_scheduled_event_delete>() },
	{ "GUILD_SCHEDULED_EVENT_USER_ADD", make_static_event<dpp::events::guild_scheduled_event_user_add>() },
	{ "GUILD_SCHEDULED_EVENT_USER_REMOVE", make_static_event<dpp::events::guild_scheduled_event_user_remove>() },
	{ "AUTO_MODERATION_RULE_CREATE", make_static_event<dpp::events::automod_rule_create>() },
	{ "AUTO_MODERATION_RULE_UPDATE", make_static_event<dpp::events::automod_rule_update>() },
	{ "AUTO_MODERATION_RULE_DELETE", make_static_event<dpp::events::automod_rule_delete>() },
	{ "AUTO_MODERATION_ACTION_EXECUTION", make_static_event<dpp::events::automod_rule_execute>() },
	{ "GUILD_AUDIT_LOG_ENTRY_CREATE", make_static_event<dpp::events::guild_audit_log_entry_create>() },
	{ "ENTITLEMENT_CREATE", make_static_event<dpp::events::entitlement_create>() },
	{ "ENTITLEMENT_UPDATE", make_static_event<dpp::events::entitlement_update>() },
	{ "ENTITLEMENT_DELETE", make_static_event<dpp::events::entitlement_delete>() },
};

void discord_client::handle_event(const std::string &event, json &j, const std::string &raw)
{
	auto ev_iter = event_map.find(event);
	if (ev_iter != event_map.end()) {
		/* A handler with nullptr is silently ignored. We don't plan to make a handler for it
		 * so this usually some user-only thing that's crept into the API and shown to bots
		 * that we dont care about.
		 */
		if (ev_iter->second != nullptr) {
			ev_iter->second->handle(this, j, raw);
		}
	} else {
		log(dpp::ll_debug, "Unhandled event: " + event + ", " + j.dump(-1, ' ', false, json::error_handler_t::replace));
	}
}

} // namespace dpp
