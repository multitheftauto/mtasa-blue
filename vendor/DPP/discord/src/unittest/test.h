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
#undef DPP_BUILD
#ifdef _WIN32
_Pragma("warning( disable : 4251 )"); // 4251 warns when we export classes or structures with stl member variables
_Pragma("warning( disable : 5105 )"); // 4251 warns when we export classes or structures with stl member variables
#endif
#include <dpp/dpp.h>
#include <dpp/json_fwd.h>
#include <iomanip>
#include <type_traits>

#ifdef _WIN32
#define SHARED_OBJECT "dpp.dll"
#elif __APPLE__
#define SHARED_OBJECT "libdpp.dylib"
#else
#define SHARED_OBJECT "libdpp.so"
#endif

using json = nlohmann::json;

enum test_flags_t {
	tf_offline = 0,
	/* A test that requires discord connectivity */
	tf_online = 1,
	/* A test that requires full tests to be enabled */
	tf_extended = 1 << 1,
	/* A test that requires coro to be enabled */
	tf_coro = 1 << 2
};

enum test_status_t {
	/* Test was not executed */
	ts_not_executed = 0,
	/* Test was started */
	ts_started,
	/* Test was skipped */
	ts_skipped,
	/* Test succeeded */
	ts_success,
	/* Test failed */
	ts_failed
};

struct test_t;

inline std::vector<test_t *> tests = {};

/* Represents a test case */
struct test_t {
	std::string_view name;
	/* Description of test */
	std::string_view description;
	/* Test type */
	test_flags_t flags;
	/* Test status */
	test_status_t status = ts_not_executed;

	test_t(std::string_view testname, std::string_view testdesc, int testflags);
};

#define DPP_TEST(name, desc, flags) inline test_t name = {#name, desc, flags}

/* Current list of unit tests */
DPP_TEST(SNOWFLAKE, "dpp::snowflake class", tf_offline);
DPP_TEST(JSON_INTERFACE, "dpp::json_interface class", tf_offline);
DPP_TEST(CLUSTER, "Instantiate DPP cluster", tf_offline);
DPP_TEST(BOTSTART, "cluster::start method", tf_online);
DPP_TEST(CONNECTION, "Connection to client websocket", tf_online);
DPP_TEST(APPCOMMAND, "Creation of application command", tf_online);
DPP_TEST(DELCOMMAND, "Deletion of application command", tf_online);
DPP_TEST(LOGGER, "Log events", tf_online);
DPP_TEST(MESSAGECREATE, "Creation of a channel message", tf_online);
DPP_TEST(MESSAGEEDIT, "Editing a channel message", tf_online);
DPP_TEST(EDITEVENT, "Message edit event", tf_online);
DPP_TEST(MESSAGEDELETE, "Deletion of a channel message", tf_online);
DPP_TEST(MESSAGERECEIVE, "Receipt of a created message", tf_online);
DPP_TEST(MESSAGEFILE, "Message attachment send and check", tf_online);
DPP_TEST(CACHE, "Test guild cache", tf_online);
DPP_TEST(USERCACHE, "Test user cache", tf_online);
DPP_TEST(REACT, "React to a message", tf_online);
DPP_TEST(REACTEVENT, "Reaction event", tf_online);
DPP_TEST(GUILDCREATE, "Receive guild create event", tf_online);
DPP_TEST(MESSAGESGET, "Get messages", tf_online);
DPP_TEST(TIMESTAMP, "crossplatform_strptime()", tf_online);
DPP_TEST(ICONHASH, "utility::iconhash", tf_offline);
DPP_TEST(CURRENTUSER, "cluster::current_user_get()", tf_online);
DPP_TEST(GETGUILD, "cluster::guild_get()", tf_online);
DPP_TEST(GETCHAN, "cluster::channel_get()", tf_online);
DPP_TEST(GETCHANS, "cluster::channels_get()", tf_online);
DPP_TEST(GETROLES, "cluster::roles_get()", tf_online);
DPP_TEST(GETINVS, "cluster::guild_get_invites()", tf_online);
DPP_TEST(GETBANS, "cluster::guild_get_bans()", tf_online);
DPP_TEST(GETPINS, "cluster::channel_pins_get()", tf_online);
DPP_TEST(GETEVENTS, "cluster::guild_events_get()", tf_online);
DPP_TEST(GETEVENT, "cluster::guild_event_get()", tf_online);
DPP_TEST(MSGCREATESEND, "message_create_t::send()", tf_online);
DPP_TEST(GETEVENTUSERS, "cluster::guild_event_users_get()", tf_online);
DPP_TEST(TIMERSTART, "start timer", tf_online);
DPP_TEST(TIMERSTOP, "stop timer", tf_online);
DPP_TEST(ONESHOT, "one-shot timer", tf_online);
DPP_TEST(TIMEDLISTENER, "timed listener", tf_online);
DPP_TEST(PRESENCE, "Presence intent", tf_online);
DPP_TEST(CUSTOMCACHE, "Instantiate a cache", tf_offline);
DPP_TEST(MSGCOLLECT, "message_collector", tf_online);
DPP_TEST(TS, "managed::get_creation_date()", tf_online);
DPP_TEST(READFILE, "utility::read_file()", tf_offline);
DPP_TEST(TIMESTAMPTOSTRING, "ts_to_string()", tf_offline);
DPP_TEST(TIMESTRINGTOTIMESTAMP, "ts_not_null()", tf_offline);
DPP_TEST(OPTCHOICE_DOUBLE, "command_option_choice::fill_from_json: double", tf_offline);
DPP_TEST(OPTCHOICE_INT, "command_option_choice::fill_from_json: int64_t", tf_offline);
DPP_TEST(OPTCHOICE_BOOL, "command_option_choice::fill_from_json: bool", tf_offline);
DPP_TEST(OPTCHOICE_SNOWFLAKE, "command_option_choice::fill_from_json: snowflake", tf_offline);
DPP_TEST(OPTCHOICE_STRING, "command_option_choice::fill_from_json: string", tf_offline);
DPP_TEST(HOSTINFO, "https_client::get_host_info()", tf_offline);
DPP_TEST(HTTPS, "https_client HTTPS request", tf_online);
DPP_TEST(HTTP, "https_client HTTP request", tf_offline);
DPP_TEST(RUNONCE, "run_once<T>", tf_offline);
DPP_TEST(WEBHOOK, "webhook construct from URL", tf_offline);
DPP_TEST(MD_ESC_1, "Markdown escaping (ignore code block contents)", tf_offline);
DPP_TEST(MD_ESC_2, "Markdown escaping (escape code block contents)", tf_offline);
DPP_TEST(URLENC, "URL encoding", tf_offline);
DPP_TEST(BASE64ENC, "Base 64 encoding", tf_offline);
DPP_TEST(SYNC, "sync<T>()", tf_online);
DPP_TEST(COMPARISON, "manged object comparison", tf_offline);
DPP_TEST(CHANNELCACHE, "find_channel()", tf_online);
DPP_TEST(CHANNELTYPES, "channel type flags", tf_online);
DPP_TEST(FORUM_CREATION, "create a forum channel", tf_online);
DPP_TEST(FORUM_CHANNEL_GET, "retrieve the created forum channel", tf_online);
DPP_TEST(FORUM_CHANNEL_DELETE, "delete the created forum channel", tf_online);
DPP_TEST(ERRORS, "Human readable error translation", tf_offline);
DPP_TEST(INVALIDUTF8, "Invalid UTF-8 handling", tf_online);

DPP_TEST(GUILD_EDIT, "cluster::guild_edit", tf_online);
DPP_TEST(GUILD_BAN_CREATE, "cluster::guild_ban_add ban three deleted discord accounts", tf_online);
DPP_TEST(GUILD_BAN_GET, "cluster::guild_get_ban getting one of the banned accounts", tf_online);
DPP_TEST(GUILD_BANS_GET, "cluster::guild_get_bans get bans using the after-parameter", tf_online);
DPP_TEST(GUILD_BAN_DELETE, "cluster::guild_ban_delete unban the banned discord accounts", tf_online);

DPP_TEST(THREAD_CREATE, "cluster::thread_create", tf_online);
DPP_TEST(THREAD_CREATE_EVENT, "cluster::on_thread_create event", tf_online);
DPP_TEST(THREAD_DELETE, "cluster::channel_delete with thread", tf_online);
DPP_TEST(THREAD_DELETE_EVENT, "cluster::on_thread_delete event", tf_online);
DPP_TEST(THREAD_EDIT, "cluster::thread_edit", tf_online);
DPP_TEST(THREAD_UPDATE_EVENT, "cluster::on_thread_update event", tf_online);
DPP_TEST(THREAD_GET_ACTIVE, "cluster::threads_get_active", tf_online);

DPP_TEST(VOICE_CHANNEL_CREATE, "creating a voice channel", tf_online);
DPP_TEST(VOICE_CHANNEL_EDIT, "editing the created voice channel", tf_online);
DPP_TEST(VOICE_CHANNEL_DELETE, "deleting the created voice channel", tf_online);

DPP_TEST(PERMISSION_CLASS, "permission", tf_offline);
DPP_TEST(EVENT_CLASS, "event class", tf_offline);
DPP_TEST(USER_GET, "cluster::user_get", tf_online);
DPP_TEST(USER_GET_FLAGS, "cluster::user_get flag parsing", tf_online);
DPP_TEST(MEMBER_GET, "cluster::guild_get_member", tf_online);
DPP_TEST(USER_GET_MENTION, "user::get_mention", tf_offline);
DPP_TEST(USER_GET_URL, "user::get_url", tf_offline);
DPP_TEST(USER_FORMAT_USERNAME, "user::format_username", tf_offline);
DPP_TEST(USER_GET_CREATION_TIME, "user::get_creation_time", tf_offline);
DPP_TEST(USER_GET_AVATAR_URL, "user::get_avatar_url", tf_offline);
DPP_TEST(CHANNEL_SET_TYPE, "channel::set_type", tf_offline);
DPP_TEST(CHANNEL_GET_MENTION, "channel::get_mention", tf_offline);
DPP_TEST(CHANNEL_GET_URL, "channel::get_url", tf_offline);
DPP_TEST(MESSAGE_GET_URL,"message::get_url",tf_offline);
DPP_TEST(UTILITY_GUILD_NAVIGATION, "utility::guild_navigation", tf_offline);
DPP_TEST(UTILITY_ICONHASH, "utility::iconhash", tf_offline);
DPP_TEST(UTILITY_MAKE_URL_PARAMETERS, "utility::make_url_parameters", tf_offline);
DPP_TEST(UTILITY_MARKDOWN_ESCAPE, "utility::markdown_escape", tf_offline);
DPP_TEST(UTILITY_TOKENIZE, "utility::tokenize", tf_offline);
DPP_TEST(UTILITY_URL_ENCODE, "utility::url_encode", tf_offline);
DPP_TEST(UTILITY_SLASHCOMMAND_MENTION, "utility::slashcommand_mention", tf_offline);
DPP_TEST(UTILITY_CHANNEL_MENTION, "utility::channel_mention", tf_offline);
DPP_TEST(UTILITY_USER_MENTION, "utility::user_mention", tf_offline);
DPP_TEST(UTILITY_ROLE_MENTION, "utility::role_mention", tf_offline);
DPP_TEST(UTILITY_EMOJI_MENTION, "utility::emoji_mention", tf_offline);
DPP_TEST(UTILITY_USER_URL, "utility::user_url", tf_offline);
DPP_TEST(UTILITY_MESSAGE_URL, "utility::message_url", tf_offline);
DPP_TEST(UTILITY_CHANNEL_URL, "utility::channel_url", tf_offline);
DPP_TEST(UTILITY_THREAD_URL, "utility::thread_url", tf_offline);
DPP_TEST(UTILITY_AVATAR_SIZE, "utility::avatar_size", tf_offline);
DPP_TEST(UTILITY_CDN_ENDPOINT_URL_HASH, "utility::cdn_endpoint_url_hash", tf_offline);
DPP_TEST(STICKER_GET_URL, "sticker::get_url aka utility::cdn_endpoint_url_sticker", tf_offline);
DPP_TEST(EMOJI_GET_URL, "emoji::get_url", tf_offline);
DPP_TEST(ROLE_COMPARE, "role::operator<", tf_offline);
DPP_TEST(ROLE_CREATE, "cluster::role_create", tf_online);
DPP_TEST(ROLE_EDIT, "cluster::role_edit", tf_online);
DPP_TEST(ROLE_DELETE, "cluster::role_delete", tf_online);
DPP_TEST(JSON_PARSE_ERROR, "JSON parse error for post_rest", tf_online);
DPP_TEST(USER_GET_CACHED_PRESENT, "cluster::user_get_cached_sync() with present member", tf_online);
DPP_TEST(USER_GET_CACHED_ABSENT, "cluster::user_get_cached_sync() with not present member", tf_online);
DPP_TEST(GET_PARAMETER_WITH_SUBCOMMANDS, "interaction_create_t::get_parameter() with subcommands", tf_offline);
DPP_TEST(GET_PARAMETER_WITHOUT_SUBCOMMANDS, "interaction_create_t::get_parameter() without subcommands", tf_offline);
DPP_TEST(AUTOMOD_RULE_CREATE, "cluster::automod_rule_create", tf_online);
DPP_TEST(AUTOMOD_RULE_GET, "cluster::automod_rule_get", tf_online);
DPP_TEST(AUTOMOD_RULE_GET_ALL, "cluster::automod_rules_get", tf_online);
DPP_TEST(AUTOMOD_RULE_DELETE, "cluster::automod_rule_delete", tf_online);
DPP_TEST(REQUEST_GET_IMAGE, "using the cluster::request method to fetch an image", tf_online);
DPP_TEST(EMOJI_CREATE, "cluster::guild_emoji_create", tf_online);
DPP_TEST(EMOJI_GET, "cluster::guild_emoji_get", tf_online);
DPP_TEST(EMOJI_DELETE, "cluster::guild_emoji_delete", tf_online);
DPP_TEST(INVITE_CREATE_EVENT, "cluster::on_invite_create", tf_online);
DPP_TEST(INVITE_DELETE_EVENT, "cluster::on_invite_delete", tf_online);
DPP_TEST(INVITE_CREATE, "cluster::channel_invite_create", tf_online);
DPP_TEST(INVITE_GET, "cluster::invite_get", tf_online);
DPP_TEST(INVITE_DELETE, "cluster::invite_delete", tf_online);

/* Extended set -- Less important, skipped on the master branch due to rate limits and GitHub actions limitations*/
/* To execute, run unittests with "full" command line argument */
DPP_TEST(MULTIHEADER, "multiheader cookie test", tf_offline | tf_extended); // Fails in the EU as cookies are not sent without acceptance

DPP_TEST(VOICECONN, "Connect to voice channel", tf_online | tf_extended);
DPP_TEST(VOICESEND, "Send audio to voice channel", tf_online | tf_extended); // udp unreliable on gitbub
DPP_TEST(MESSAGEPIN, "Pinning a channel message", tf_online | tf_extended);
DPP_TEST(MESSAGEUNPIN, "Unpinning a channel message", tf_online | tf_extended);

DPP_TEST(POLL_CREATE, "Creating a poll", tf_online);
DPP_TEST(POLL_END, "Ending a poll", tf_online);

DPP_TEST(THREAD_MEMBER_ADD, "cluster::thread_member_add", tf_online | tf_extended);
DPP_TEST(THREAD_MEMBER_GET, "cluster::thread_member_get", tf_online | tf_extended);
DPP_TEST(THREAD_MEMBERS_GET, "cluster::thread_members_get", tf_online | tf_extended);
DPP_TEST(THREAD_MEMBER_REMOVE, "cluster::thread_member_remove", tf_online | tf_extended);
DPP_TEST(THREAD_MEMBERS_ADD_EVENT, "cluster::on_thread_members_update event with member addition", tf_online | tf_extended);
DPP_TEST(THREAD_MEMBERS_REMOVE_EVENT, "cluster::on_thread_members_update event with member removal", tf_online | tf_extended);
DPP_TEST(THREAD_CREATE_MESSAGE, "cluster::thread_create_with_message", tf_online | tf_extended);

DPP_TEST(THREAD_MESSAGE, "message manipulation in thread", tf_online | tf_extended);
DPP_TEST(THREAD_MESSAGE_CREATE_EVENT, "cluster::on_message_create in thread", tf_online | tf_extended);
DPP_TEST(THREAD_MESSAGE_EDIT_EVENT, "cluster::on_message_edit in thread", tf_online | tf_extended);
DPP_TEST(THREAD_MESSAGE_DELETE_EVENT, "cluster::on_message_delete in thread", tf_online | tf_extended);
DPP_TEST(THREAD_MESSAGE_REACT_ADD_EVENT, "cluster::on_reaction_add in thread", tf_online | tf_extended);
DPP_TEST(THREAD_MESSAGE_REACT_REMOVE_EVENT, "cluster::on_reaction_remove in thread", tf_online | tf_extended);

DPP_TEST(CORO_JOB_OFFLINE, "coro: offline job", tf_offline | tf_coro);
DPP_TEST(CORO_COROUTINE_OFFLINE, "coro: offline coroutine", tf_offline | tf_coro);
DPP_TEST(CORO_TASK_OFFLINE, "coro: offline task", tf_offline | tf_coro);
DPP_TEST(CORO_ASYNC_OFFLINE, "coro: offline async", tf_offline | tf_coro);
DPP_TEST(CORO_EVENT_HANDLER, "coro: online event handler", tf_online | tf_coro);
DPP_TEST(CORO_API_CALLS, "coro: online api calls", tf_online | tf_coro);
DPP_TEST(CORO_MUMBO_JUMBO, "coro: online mumbo jumbo in event handler", tf_online | tf_coro | tf_extended);

void coro_offline_tests();
void coro_online_tests(dpp::cluster *bot);

class test_cached_object_t : public dpp::managed {
public:
	test_cached_object_t(dpp::snowflake _id) : dpp::managed(_id) { };
	virtual ~test_cached_object_t() = default;
	std::string foo;
};

/* How long the unit tests can run for */
const int64_t TEST_TIMEOUT = 60;

#define SAFE_GETENV(var) (getenv(var) && *(getenv(var)) ? getenv(var) : "0")

/* IDs of various channels and guilds used to test */
extern dpp::snowflake TEST_GUILD_ID;
extern dpp::snowflake TEST_TEXT_CHANNEL_ID;
extern dpp::snowflake TEST_VC_ID;
extern dpp::snowflake TEST_USER_ID;
extern dpp::snowflake TEST_EVENT_ID;

/* True if we skip tt_online tests */
extern bool offline;
/* True if we skip tt_extended tests*/
extern bool extended;
#ifdef DPP_CORO
inline constexpr bool coro = true;
#else
inline constexpr bool coro = false;
#endif

/**
 * @brief Perform a test of a REST base API call with one parameter
 */
#define singleparam_api_test(func_name, param, return_type, testname) \
	set_test(testname, false); \
	if (!offline) { \
		bot.func_name (param, [&](const dpp::confirmation_callback_t &cc) { \
			if (!cc.is_error()) { \
				return_type g = std::get<return_type>(cc.value); \
				if (g.id == param) { \
					set_test(testname, true); \
				} else { \
					bot.log(dpp::ll_debug, cc.http_info.body); \
					set_test(testname, false); \
				} \
			} else { \
				bot.log(dpp::ll_debug, cc.http_info.body); \
				set_test(testname, false); \
			} \
		}); \
	}

/**
 * @brief Perform a test of a REST base API call with one parameter
 */
#define twoparam_api_test(func_name, param1, param2, return_type, testname) \
	set_test(testname, false); \
	if (!offline) { \
		bot.func_name (param1, param2, [&](const dpp::confirmation_callback_t &cc) { \
			if (!cc.is_error()) { \
				return_type g = std::get<return_type>(cc.value); \
				if (g.id > 0) { \
					set_test(testname, true); \
				} else { \
					bot.log(dpp::ll_debug, cc.http_info.body); \
					set_test(testname, false); \
				} \
			} else { \
				bot.log(dpp::ll_debug, cc.http_info.body); \
				set_test(testname, false); \
			} \
		}); \
	}

/**
 * @brief Perform a test of a REST base API call with one parameter that returns a list
 */
#define singleparam_api_test_list(func_name, param, return_type, testname) \
	set_test(testname, false); \
	if (!offline) { \
		bot.func_name (param, [&](const dpp::confirmation_callback_t &cc) { \
			if (!cc.is_error()) { \
				return_type g = std::get<return_type>(cc.value); \
				if (g.size() > 0) { \
					set_test(testname, true); \
				} else { \
					set_test(testname, false); \
					bot.log(dpp::ll_debug, cc.http_info.body); \
				} \
			} else { \
				set_test(testname, false); \
				bot.log(dpp::ll_debug, cc.http_info.body); \
			} \
		}); \
	}

/**
 * @brief Perform a test of a REST base API call with one parameter that returns a list
 */
#define multiparam_api_test_list(func_name, param, return_type, testname) \
	set_test(testname, false); \
	if (!offline) { \
		bot.func_name (param, 0, 0, 1000, [&](const dpp::confirmation_callback_t &cc) { \
			if (!cc.is_error()) { \
				return_type g = std::get<return_type>(cc.value); \
				if (g.size() > 0) { \
					set_test(testname, true); \
				} else { \
					set_test(testname, false); \
					bot.log(dpp::ll_debug, cc.http_info.body); \
				} \
			} else { \
				set_test(testname, false); \
				bot.log(dpp::ll_debug, cc.http_info.body); \
			} \
		}); \
	}

/**
 * @brief Perform a test of a REST base API call with two parameters
 */
#define twoparam_api_test_list(func_name, param1, param2, return_type, testname) \
	set_test(testname, false); \
	if (!offline) { \
		bot.func_name (param1, param2, [&](const dpp::confirmation_callback_t &cc) { \
			if (!cc.is_error()) { \
				return_type g = std::get<return_type>(cc.value); \
				if (g.size() > 0) { \
					set_test(testname, true); \
				} else { \
					bot.log(dpp::ll_debug, cc.http_info.body); \
					set_test(testname, false); \
				} \
			} else { \
				bot.log(dpp::ll_debug, cc.http_info.body); \
				set_test(testname, false); \
			} \
		}); \
	}


/**
 * @brief Perform a test of a REST base API call with no parameters
 */
#define noparam_api_test(func_name, return_type, testname) \
	set_test(testname, false); \
	if (!offline) { \
		bot.func_name ([&](const dpp::confirmation_callback_t &cc) { \
			if (!cc.is_error()) { \
				return_type g = std::get<return_type>(cc.value); \
				set_test(testname, true); \
			} else { \
				bot.log(dpp::ll_debug, cc.http_info.body); \
				set_test(testname, false); \
			} \
		}); \
	}

/**
 * @brief Sets a test's status (legacy)
 *
 * @param test The test to set the status of
 * @param success If set to true, sets success to true, if set to false and called
 * once, sets executed to true, if called twice, also sets success to false.
 * This means that before you run the test you should call this function once
 * with success set to false, then if/wen the test completes call it again with true.
 * If the test fails, call it a second time with false, or not at all.
 */
void set_test(test_t &test, bool success = false);

/**
 * @brief Sets a test's status
 *
 * @param test The test to set the status of
 * @param status Status to set the test to
 */
void set_status(test_t &test, test_status_t status, std::string_view message = {});

/**
 * @brief Sets a test's status to ts_skipped
 *
 * @param test The test to set the status of
 */
void skip_test(test_t &test);

/**
 * @brief Sets a test's status to ts_started
 *
 * @param test The test to set the status of
 */
void start_test(test_t &test);

/**
 * @brief Check if a test is/should be skipped
 *
 * @return bool Whether the test is/should be skipped
 */
bool is_skipped(const test_t &test);

/**
 * @brief Prints a summary of all tests executed
 * @param tests List of tests executed
 * 
 * @return int Returns number of failed tests, for use as a return value from the main() function
 */
int test_summary();


/**
 * @brief Load test audio for the voice channel tests
 * 
 * @return std::vector<uint8_t> data and size for test audio
 */
std::vector<uint8_t> load_test_audio();

/**
 * @brief Load bytes from file
 * 
 * @return std::vector<std::byte> File data
 */
std::vector<std::byte> load_data(const std::string& file);

/**
 * @brief Get the token from the environment variable DPP_UNIT_TEST_TOKEN
 * 
 * @return std::string token
 * @note If the environment variable does not exist, this will exit the program.
 */
std::string get_token();

/**
 * @brief Wait for all tests to be completed or test program to time out
 */
void wait_for_tests();

/**
 * @brief Get the start time of tests
 * 
 * @return double start time in fractional seconds
 */
double get_start_time();

/**
 * @brief Get the current execution time in seconds
 * 
 * @return double fractional seconds
 */
double get_time();

/**
 * @brief A test version of the message collector for use in unit tests
 */
class message_collector : public dpp::message_collector {
public:
	message_collector(dpp::cluster* cl, uint64_t duration) : dpp::message_collector(cl, duration) { }

	virtual void completed(const std::vector<dpp::message>& list) {
		set_test(MSGCOLLECT, list.size() > 0);
	}
};

/**
 * @brief Convenience functor to get the snowflake of a certain type
 */
struct user_project_id_t {
	dpp::snowflake operator()(const dpp::user &user) const noexcept {
		return user.id;
	}

	dpp::snowflake operator()(const dpp::guild_member &user) const noexcept {
		return user.user_id;
	}

	dpp::snowflake operator()(dpp::snowflake user) const noexcept {
		return user;
	}

	dpp::snowflake operator()(const dpp::thread_member &user) const noexcept {
		return user.user_id;
	}
};

/**
 * @brief Convenience lambda to get the user snowflake of a certain user type
 * @see user_project_id_t
 */
inline constexpr user_project_id_t get_user_snowflake;

/**
 * @brief Convenience lambda to check if a certain user is the owner of the test bot, mostly meant to be passed to standard algorithms
 * @see get_user_snowflake
 *
 * @return bool whether the user is the test bot owner
 */
inline constexpr auto is_owner = [](auto &&user) noexcept {
	return get_user_snowflake(user) == TEST_USER_ID;
};

#define DPP_RUNTIME_CHECK(test, check, var) if (!check) { var = false; set_status(test, ts_failed, "check failed: " #check); }
#define DPP_COMPILETIME_CHECK(test, check, var) static_assert(check, #test ": " #check)

#ifndef DPP_STATIC_TEST
#define DPP_CHECK(test, check, var) DPP_RUNTIME_CHECK(test, check, var)
#else
#define DPP_CHECK(test, check, var) DPP_COMPILETIME_CHECK(test, check, var)
#endif

#define DPP_CHECK_CONSTRUCT_ASSIGN(test, type, var) do { \
  DPP_CHECK(test, std::is_default_constructible_v<type>, var); \
  DPP_CHECK(test, std::is_copy_constructible_v<type>, var); \
  DPP_CHECK(test, std::is_move_constructible_v<type>, var); \
  DPP_CHECK(test, std::is_copy_assignable_v<type>, var); \
	DPP_CHECK(test, std::is_move_assignable_v<type>, var); \
  } while(0)
