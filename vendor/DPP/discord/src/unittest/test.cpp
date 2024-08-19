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
#include "test.h"

#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>
#include <dpp/restrequest.h>
#include <dpp/json.h>

/**
 * @brief Type trait to check if a certain type has a build_json method
 *
 * @tparam T type to check for
 */
template <typename T, typename = std::void_t<>>
struct has_build_json : std::false_type {};

template <typename T>
struct has_build_json<T, std::void_t<decltype(std::declval<T&>().build_json())>> : std::true_type {};

/**
 * @brief Type trait to check if a certain type has a build_json method
 *
 * @tparam T type to check for
 */
template <typename T>
constexpr bool has_build_json_v = has_build_json<T>::value;

/**
 * @brief Type trait to check if a certain type has a fill_from_json method
 *
 * @tparam T type to check for
 */
template <typename T, typename = void>
struct has_fill_from_json : std::false_type {};

template <typename T>
struct has_fill_from_json<T, std::void_t<decltype(std::declval<T&>().fill_from_json(std::declval<dpp::json*>()))>> : std::true_type {};

/**
 * @brief Type trait to check if a certain type has a fill_from_json method
 *
 * @tparam T type to check for
 */
template <typename T>
constexpr bool has_fill_from_json_v = has_fill_from_json<T>::value;

/* Unit tests go here */
int main(int argc, char *argv[])
{
	std::string token(get_token());

	std::cout << "[" << std::fixed << std::setprecision(3) << (dpp::utility::time_f() - get_start_time()) << "]: [\u001b[36mSTART\u001b[0m] ";
	if (offline) {
		std::cout << "Running offline unit tests only.\n";
	} else {
		if (argc > 1 && std::find_if(argv + 1, argv + argc, [](const char *a){ return (std::strcmp(a, "full") == 0); }) != argv + argc) {
			extended = true;
		}
		std::cout << "Running offline and " << (extended ? "extended" : "limited") << " online unit tests. Guild ID: " << TEST_GUILD_ID << " Text Channel ID: " << TEST_TEXT_CHANNEL_ID << " VC ID: " << TEST_VC_ID << " User ID: " << TEST_USER_ID << " Event ID: " << TEST_EVENT_ID << "\n";
	}

	std::string test_to_escape = "*** _This is a test_ ***\n```cpp\n\
int main() {\n\
    /* Comment */\n\
    int answer = 42;\n\
    return answer; // ___\n\
};\n\
```\n\
Markdown lol ||spoiler|| ~~strikethrough~~ `small *code* block`\n";

	set_test(COMPARISON, false);
	dpp::user u1;
	dpp::user u2;
	dpp::user u3;
	u1.id = u2.id = 666;
	u3.id = 777;
	set_test(COMPARISON, u1 == u2 && u1 != u3);


	set_test(ERRORS, false);

	/* Prepare a confirmation_callback_t in error state (400) */
	dpp::confirmation_callback_t error_test;
	bool error_message_success = false;
	error_test.http_info.status = 400;

	error_test.http_info.body = "{\
		\"message\": \"Invalid Form Body\",\
		\"code\": 50035,\
		\"errors\": {\
			\"options\": {\
				\"0\": {\
					\"name\": {\
						\"_errors\": [\
							{\
								\"code\": \"STRING_TYPE_REGEX\",\
								\"message\": \"String value did not match validation regex.\"\
							},\
							{\
								\"code\": \"APPLICATION_COMMAND_INVALID_NAME\",\
								\"message\": \"Command name is invalid\"\
							}\
						]\
					}\
				}\
			}\
		}\
	}";
	error_message_success = (error_test.get_error().human_readable == "50035: Invalid Form Body\n\t- options[0].name: String value did not match validation regex. (STRING_TYPE_REGEX)\n\t- options[0].name: Command name is invalid (APPLICATION_COMMAND_INVALID_NAME)");

	error_test.http_info.body = "{\
		\"message\": \"Invalid Form Body\",\
		\"code\": 50035,\
		\"errors\": {\
			\"type\": {\
				\"_errors\": [\
					{\
						\"code\": \"BASE_TYPE_CHOICES\",\
						\"message\": \"Value must be one of {4, 5, 9, 10, 11}.\"\
					}\
				]\
			}\
		}\
	}";
	error_message_success = (error_message_success && error_test.get_error().human_readable == "50035: Invalid Form Body - type: Value must be one of {4, 5, 9, 10, 11}. (BASE_TYPE_CHOICES)");

	error_test.http_info.body = "{\
		\"message\": \"Unknown Guild\",\
		\"code\": 10004\
	}";
	error_message_success = (error_message_success && error_test.get_error().human_readable == "10004: Unknown Guild");

	error_test.http_info.body = "{\
		\"message\": \"Invalid Form Body\",\
		\"code\": 50035,\
		\"errors\": {\
			\"allowed_mentions\": {\
				\"_errors\": [\
					{\
						\"code\": \"MESSAGE_ALLOWED_MENTIONS_PARSE_EXCLUSIVE\",\
						\"message\": \"parse:[\\\"users\\\"] and users: [ids...] are mutually exclusive.\"\
					}\
				]\
			}\
		}\
	}";
	error_message_success = (error_message_success && error_test.get_error().human_readable == "50035: Invalid Form Body - allowed_mentions: parse:[\"users\"] and users: [ids...] are mutually exclusive. (MESSAGE_ALLOWED_MENTIONS_PARSE_EXCLUSIVE)");

	error_test.http_info.body = "{\
		\"message\": \"Invalid Form Body\",\
		\"code\": 50035,\
		\"errors\": {\
			\"1\": {\
				\"options\": {\
					\"1\": {\
						\"description\": {\
							\"_errors\": [\
								{\
									\"code\": \"BASE_TYPE_BAD_LENGTH\",\
									\"message\": \"Must be between 1 and 100 in length.\"\
								}\
							]\
						}\
					}\
				}\
			}\
		}\
	}";
	error_message_success = (error_message_success && error_test.get_error().human_readable == "50035: Invalid Form Body - <array>[1].options[1].description: Must be between 1 and 100 in length. (BASE_TYPE_BAD_LENGTH)");

	error_test.http_info.body = "{\
  	\"message\": \"Invalid Form Body\",\
  	\"code\": 50035,\
  	\"errors\": {\
  	  \"data\": {\
  	    \"poll\": {\
  	      \"_errors\": [\
  	        {\
							\"code\": \"POLL_TYPE_QUESTION_ALLOWS_TEXT_ONLY\",\
 							\"message\": \"This poll type cannot include attachments, emoji or stickers with the question\"}\
  	      ]\
  	    }\
  	  }\
  	}\
	}";
	error_message_success = (error_message_success && error_test.get_error().human_readable == "50035: Invalid Form Body - data.poll: This poll type cannot include attachments, emoji or stickers with the question (POLL_TYPE_QUESTION_ALLOWS_TEXT_ONLY)");

	set_test(ERRORS, error_message_success);

	set_test(MD_ESC_1, false);
	set_test(MD_ESC_2, false);
	std::string escaped1 = dpp::utility::markdown_escape(test_to_escape);
	std::string escaped2 = dpp::utility::markdown_escape(test_to_escape, true);
	set_test(MD_ESC_1, escaped1 == "\\*\\*\\* \\_This is a test\\_ \\*\\*\\*\n\
```cpp\n\
int main() {\n\
    /* Comment */\n\
    int answer = 42;\n\
    return answer; // ___\n\
};\n\
```\n\
Markdown lol \\|\\|spoiler\\|\\| \\~\\~strikethrough\\~\\~ `small *code* block`\n");
	set_test(MD_ESC_2, escaped2 == "\\*\\*\\* \\_This is a test\\_ \\*\\*\\*\n\
\\`\\`\\`cpp\n\
int main\\(\\) {\n\
    /\\* Comment \\*/\n\
    int answer = 42;\n\
    return answer; // \\_\\_\\_\n\
};\n\
\\`\\`\\`\n\
Markdown lol \\|\\|spoiler\\|\\| \\~\\~strikethrough\\~\\~ \\`small \\*code\\* block\\`\n");

	set_test(URLENC, false);
	set_test(URLENC, dpp::utility::url_encode("ABC123_+\\|$*/AAA[]😄") == "ABC123_%2B%5C%7C%24%2A%2FAAA%5B%5D%F0%9F%98%84");

	set_test(BASE64ENC, false);
	set_test(BASE64ENC,
		dpp::base64_encode(reinterpret_cast<unsigned char const*>("a"), 1) == "YQ==" &&
		dpp::base64_encode(reinterpret_cast<unsigned char const*>("bc"), 2) == "YmM=" &&
		dpp::base64_encode(reinterpret_cast<unsigned char const*>("def"), 3) == "ZGVm" &&
		dpp::base64_encode(reinterpret_cast<unsigned char const*>("ghij"), 4) == "Z2hpag==" &&
		dpp::base64_encode(reinterpret_cast<unsigned char const*>("klmno"), 5) == "a2xtbm8=" &&
		dpp::base64_encode(reinterpret_cast<unsigned char const*>("pqrstu"), 6) == "cHFyc3R1" &&
		dpp::base64_encode(reinterpret_cast<unsigned char const*>("vwxyz12"), 7) == "dnd4eXoxMg=="
	);

	dpp::http_connect_info hci;
	set_test(HOSTINFO, false);

	hci = dpp::https_client::get_host_info("https://test.com:444");
	bool hci_test = (hci.scheme == "https" && hci.hostname == "test.com" && hci.port == 444 && hci.is_ssl == true);

	hci = dpp::https_client::get_host_info("https://test.com");
	hci_test = hci_test && (hci.scheme == "https" && hci.hostname == "test.com" && hci.port == 443 && hci.is_ssl == true);

	hci = dpp::https_client::get_host_info("http://test.com");
	hci_test = hci_test && (hci.scheme == "http" && hci.hostname == "test.com" && hci.port == 80 && hci.is_ssl == false);

	hci = dpp::https_client::get_host_info("http://test.com:90");
	hci_test = hci_test && (hci.scheme == "http" && hci.hostname == "test.com" && hci.port == 90 && hci.is_ssl == false);

	hci = dpp::https_client::get_host_info("test.com:97");
	hci_test = hci_test && (hci.scheme == "http" && hci.hostname == "test.com" && hci.port == 97 && hci.is_ssl == false);

	hci = dpp::https_client::get_host_info("test.com");
	hci_test = hci_test && (hci.scheme == "http" && hci.hostname == "test.com" && hci.port == 80 && hci.is_ssl == false);

	set_test(HOSTINFO, hci_test);

	set_test(HTTPS, false);
	if (!offline) {
		dpp::multipart_content multipart = dpp::https_client::build_multipart(
			"{\"content\":\"test\"}", {"test.txt", "blob.blob"}, {"ABCDEFGHI", "BLOB!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"}, {"text/plain", "application/octet-stream"}
		);
		try {
			dpp::https_client c("discord.com", 443, "/api/channels/" + std::to_string(TEST_TEXT_CHANNEL_ID) + "/messages", "POST", multipart.body,
				{
					{"Content-Type", multipart.mimetype},
					{"Authorization", "Bot " + token}
				}
			);
			std::string hdr1 = c.get_header("server");
			std::string content1 = c.get_content();
			set_test(HTTPS, hdr1 == "cloudflare" && c.get_status() == 200);
		}
		catch (const dpp::exception& e) {
			std::cout << e.what() << "\n";
			set_test(HTTPS, false);
		}
	}

	set_test(HTTP, false);
	try {
		dpp::https_client c2("github.com", 80, "/", "GET", "", {}, true);
		std::string hdr2 = c2.get_header("location");
		std::string content2 = c2.get_content();
		set_test(HTTP, hdr2 == "https://github.com/" && c2.get_status() == 301);
	}
	catch (const dpp::exception& e) {
		std::cout << e.what() << "\n";
		set_test(HTTP, false);
	}

	set_test(MULTIHEADER, false);
	try {
		dpp::https_client c2("dl.dpp.dev", 443, "/cookietest.php", "GET", "", {});
		size_t count = c2.get_header_count("set-cookie");
		size_t count_list = c2.get_header_list("set-cookie").size();
		// Google sets a bunch of cookies when we start accessing it.
		set_test(MULTIHEADER, c2.get_status() == 200 && count > 1 && count == count_list);
	}
	catch (const dpp::exception& e) {
		std::cout << e.what() << "\n";
		set_test(MULTIHEADER, false);
	}

	std::vector<uint8_t> testaudio = load_test_audio();

	set_test(READFILE, false);
	std::string rf_test = dpp::utility::read_file(SHARED_OBJECT);
	FILE* fp = fopen(SHARED_OBJECT, "rb");
	fseek(fp, 0, SEEK_END);
	size_t off = (size_t)ftell(fp);
	fclose(fp);
	set_test(READFILE, off == rf_test.length());

	set_test(TIMESTAMPTOSTRING, false);
	set_test(TIMESTAMPTOSTRING, dpp::ts_to_string(1642611864) == "2022-01-19T17:04:24Z");

	set_test(ROLE_COMPARE, false);
	dpp::role role_1, role_2;
	role_1.position = 1;
	role_2.position = 2;
	set_test(ROLE_COMPARE, role_1 < role_2 && role_1 != role_2);

	set_test(WEBHOOK, false);
	try {
		dpp::webhook test_wh("https://discord.com/api/webhooks/833047646548133537/ntCHEYYIoHSLy_GOxPx6pmM0sUoLbP101ct-WI6F-S4beAV2vaIcl_Id5loAMyQwxqhE");
		set_test(WEBHOOK, (test_wh.token == "ntCHEYYIoHSLy_GOxPx6pmM0sUoLbP101ct-WI6F-S4beAV2vaIcl_Id5loAMyQwxqhE") && (test_wh.id == dpp::snowflake(833047646548133537)));
	}
	catch (const dpp::exception&) {
		set_test(WEBHOOK, false);
	}

	{ // test dpp::snowflake
		start_test(SNOWFLAKE);
		bool success = true;
		dpp::snowflake s = 69420;
		json j;
		j["value"] = s;
		success = dpp::snowflake_not_null(&j, "value") == 69420 && success;
		DPP_CHECK_CONSTRUCT_ASSIGN(SNOWFLAKE, dpp::snowflake, success);
		s = 42069;
		success = success && (s == 42069 && s == dpp::snowflake{42069} && s == "42069");
		success = success && (dpp::snowflake{69} < dpp::snowflake{420} && (dpp::snowflake{69} < 420));
		s = "69420";
		success = success && s == 69420;
		auto conversion_test = [](dpp::snowflake sl) {
			return sl.str();
		};
		s = conversion_test(std::string{"1337"});
		success = success && s == 1337; /* THIS BREAKS (and i do not care very much): && s == conversion_test(dpp::snowflake{"1337"}); */
		success = success && dpp::snowflake{0} == 0;
		set_test(SNOWFLAKE, success);
	}

	{ // test dpp::json_interface
		start_test(JSON_INTERFACE);
		struct fillable : dpp::json_interface<fillable> {
			fillable &fill_from_json_impl(dpp::json *) {
				return *this;
			}
		};
		struct buildable : dpp::json_interface<buildable> {
			json to_json_impl(bool = false) const {
				return {};
			}
		};
		struct fillable_and_buildable : dpp::json_interface<fillable_and_buildable> {
			fillable_and_buildable &fill_from_json_impl(dpp::json *) {
				return *this;
			}

			json to_json_impl(bool = false) const {
				return {};
			}
		};
		bool success = true;

		DPP_CHECK(JSON_INTERFACE, has_build_json_v<dpp::json_interface<buildable>>, success);
		DPP_CHECK(JSON_INTERFACE, !has_fill_from_json_v<dpp::json_interface<buildable>>, success);
		DPP_CHECK(JSON_INTERFACE, has_build_json_v<buildable>, success);
		DPP_CHECK(JSON_INTERFACE, !has_fill_from_json_v<buildable>, success);

		DPP_CHECK(JSON_INTERFACE, !has_build_json_v<dpp::json_interface<fillable>>, success);
		DPP_CHECK(JSON_INTERFACE, has_fill_from_json_v<dpp::json_interface<fillable>>, success);
		DPP_CHECK(JSON_INTERFACE, !has_build_json_v<fillable>, success);
		DPP_CHECK(JSON_INTERFACE, has_fill_from_json_v<fillable>, success);

		DPP_CHECK(JSON_INTERFACE, has_build_json_v<dpp::json_interface<fillable_and_buildable>>, success);
		DPP_CHECK(JSON_INTERFACE, has_fill_from_json_v<dpp::json_interface<fillable_and_buildable>>, success);
		DPP_CHECK(JSON_INTERFACE, has_build_json_v<fillable_and_buildable>, success);
		DPP_CHECK(JSON_INTERFACE, has_fill_from_json_v<fillable_and_buildable>, success);
		set_test(JSON_INTERFACE, success);
	}

	{ // test interaction_create_t::get_parameter
		// create a fake interaction
		dpp::cluster cluster("");
		dpp::discord_client client(&cluster, 1, 1, "");
		dpp::interaction_create_t interaction(&client, "");

		/* Check the method with subcommands */
		set_test(GET_PARAMETER_WITH_SUBCOMMANDS, false);

		dpp::command_interaction cmd_data; // command
		cmd_data.type = dpp::ctxm_chat_input;
		cmd_data.name = "command";

		dpp::command_data_option subcommandgroup; // subcommand group
		subcommandgroup.name = "group";
		subcommandgroup.type = dpp::co_sub_command_group;

		dpp::command_data_option subcommand; // subcommand
		subcommand.name = "add";
		subcommand.type = dpp::co_sub_command;

		dpp::command_data_option option1; // slashcommand option
		option1.name = "user";
		option1.type = dpp::co_user;
		option1.value = dpp::snowflake(189759562910400512);

		dpp::command_data_option option2; // slashcommand option
		option2.name = "checked";
		option2.type = dpp::co_boolean;
		option2.value = true;

		// add them
		subcommand.options.push_back(option1);
		subcommand.options.push_back(option2);
		subcommandgroup.options.push_back(subcommand);
		cmd_data.options.push_back(subcommandgroup);
		interaction.command.data = cmd_data;

		dpp::snowflake value1 = std::get<dpp::snowflake>(interaction.get_parameter("user"));
		set_test(GET_PARAMETER_WITH_SUBCOMMANDS, value1 == dpp::snowflake(189759562910400512));

		/* Check the method without subcommands */
		set_test(GET_PARAMETER_WITHOUT_SUBCOMMANDS, false);

		dpp::command_interaction cmd_data2; // command
		cmd_data2.type = dpp::ctxm_chat_input;
		cmd_data2.name = "command";

		dpp::command_data_option option3; // slashcommand option
		option3.name = "number";
		option3.type = dpp::co_integer;
		option3.value = int64_t(123456);

		cmd_data2.options.push_back(option3);
		interaction.command.data = cmd_data2;

		int64_t value2 = std::get<int64_t>(interaction.get_parameter("number"));
		set_test(GET_PARAMETER_WITHOUT_SUBCOMMANDS, value2 == 123456);
	}

	{ // test dpp::command_option_choice::fill_from_json
		set_test(OPTCHOICE_DOUBLE, false);
		set_test(OPTCHOICE_INT, false);
		set_test(OPTCHOICE_BOOL, false);
		set_test(OPTCHOICE_SNOWFLAKE, false);
		set_test(OPTCHOICE_STRING, false);
		json j;
		dpp::command_option_choice choice;
		j["value"] = 54.321;
		choice.fill_from_json(&j);
		bool success_double = std::holds_alternative<double>(choice.value);
		j["value"] = 8223372036854775807;
		choice.fill_from_json(&j);
		bool success_int = std::holds_alternative<int64_t>(choice.value);
		j["value"] = -8223372036854775807;
		choice.fill_from_json(&j);
		bool success_int2 = std::holds_alternative<int64_t>(choice.value);
		j["value"] = true;
		choice.fill_from_json(&j);
		bool success_bool = std::holds_alternative<bool>(choice.value);
		dpp::snowflake s(845266178036516757); // example snowflake
		j["value"] = s;
		choice.fill_from_json(&j);
		bool success_snowflake = std::holds_alternative<dpp::snowflake>(choice.value) && std::get<dpp::snowflake>(choice.value) == s;
		j["value"] = "foobar";
		choice.fill_from_json(&j);
		bool success_string = std::holds_alternative<std::string>(choice.value);
		set_test(OPTCHOICE_DOUBLE, success_double);
		set_test(OPTCHOICE_INT, success_int && success_int2);
		set_test(OPTCHOICE_BOOL, success_bool);
		set_test(OPTCHOICE_SNOWFLAKE, success_snowflake);
		set_test(OPTCHOICE_STRING, success_string);
	}

	{
		set_test(PERMISSION_CLASS, false);
		bool success = false;
		auto p = dpp::permission();
		p = 16;
		success = p == 16;
		p |= 4;
		success = p == 20 && success;
		p <<= 8; // left shift
		success = p == 5120 && success;
		auto s = std::to_string(p);
		success = s == "5120" && success;
		p.set(0).add(~uint64_t{0}).remove(dpp::p_speak).set(dpp::p_administrator);
		success = !p.has(dpp::p_administrator, dpp::p_ban_members) && success; // must return false because they're not both set
		success = !p.has(dpp::p_administrator | dpp::p_ban_members) && success;
		success = p.can(dpp::p_ban_members) && success;
		success = p.can(dpp::p_speak) && success;

		constexpr auto permission_test = [](dpp::permission p) constexpr noexcept {
			bool success{true};

			p.set(0).add(~uint64_t{0}).remove(dpp::p_speak).set(dpp::p_connect);
			p.set(dpp::p_administrator, dpp::p_ban_members);
			success = p.has(dpp::p_administrator) && success;
			success = p.has(dpp::p_administrator) && p.has(dpp::p_ban_members) && success;
			success = p.has(dpp::p_administrator, dpp::p_ban_members) && success;
			success = p.has(dpp::p_administrator | dpp::p_ban_members) && success;
			success = p.add(dpp::p_speak).has(dpp::p_administrator, dpp::p_speak) && success;
			success = !p.remove(dpp::p_speak).has(dpp::p_administrator, dpp::p_speak) && success;
			p.remove(dpp::p_administrator);
			success = p.can(dpp::p_ban_members) && success;
			success = !p.can(dpp::p_speak, dpp::p_ban_members) && success;
			success = p.can_any(dpp::p_speak, dpp::p_ban_members) && success;
			return success;
		};
		constexpr auto constexpr_success = permission_test({~uint64_t{0}}); // test in constant evaluated
		success = permission_test({~uint64_t{0}}) && constexpr_success && success; // test at runtime
		set_test(PERMISSION_CLASS, success);
	}

	{ // dpp event classes
		start_test(EVENT_CLASS);
		bool success = true;
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::log_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_scheduled_event_user_add_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_scheduled_event_user_remove_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_scheduled_event_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_scheduled_event_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_scheduled_event_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::automod_rule_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::automod_rule_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::automod_rule_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::automod_rule_execute_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::stage_instance_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::stage_instance_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::stage_instance_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_state_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::interaction_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::slashcommand_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::button_click_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::form_submit_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::autocomplete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::context_menu_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_context_menu_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::user_context_menu_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::select_click_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_stickers_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_join_request_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::channel_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::channel_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::ready_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_member_remove_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::resumed_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_role_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::typing_start_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_track_marker_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_reaction_add_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_members_chunk_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_reaction_remove_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::channel_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_reaction_remove_emoji_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_delete_bulk_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_role_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_role_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::channel_pins_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_reaction_remove_all_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_server_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_emojis_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::presence_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::webhooks_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_member_add_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::invite_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_integrations_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_member_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::invite_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::user_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::message_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_audit_log_entry_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_ban_add_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::guild_ban_remove_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::integration_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::integration_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::integration_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::thread_create_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::thread_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::thread_delete_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::thread_list_sync_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::thread_member_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::thread_members_update_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_buffer_send_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_user_talking_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_ready_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_receive_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_client_speaking_t, success);
		DPP_CHECK_CONSTRUCT_ASSIGN(EVENT_CLASS, dpp::voice_client_disconnect_t, success);
		set_test(EVENT_CLASS, success);
	}


	{ // some dpp::user methods
		dpp::user user1;
		user1.id = 189759562910400512;
		user1.discriminator = 0001;
		user1.username = "brain";

		set_test(USER_GET_MENTION, false);
		set_test(USER_GET_MENTION, user1.get_mention() == "<@189759562910400512>");

		set_test(USER_FORMAT_USERNAME, false);
		set_test(USER_FORMAT_USERNAME, user1.format_username() == "brain#0001");

		set_test(USER_GET_CREATION_TIME, false);
		set_test(USER_GET_CREATION_TIME, (uint64_t)user1.get_creation_time() == 1465312605);

		set_test(USER_GET_URL, false);

		dpp::user user2;
		set_test(USER_GET_URL,
				 user1.get_url() == dpp::utility::url_host + "/users/189759562910400512" &&
				 user2.get_url() == ""
		);
	}

	{ // avatar size function
		set_test(UTILITY_AVATAR_SIZE, false);
		bool success = false;
		success = dpp::utility::avatar_size(0).empty();
		success = dpp::utility::avatar_size(16) == "?size=16" && success;
		success = dpp::utility::avatar_size(256) == "?size=256" && success;
		success = dpp::utility::avatar_size(4096) == "?size=4096" && success;
		success = dpp::utility::avatar_size(8192).empty() && success;
		success = dpp::utility::avatar_size(3000).empty() && success;
		set_test(UTILITY_AVATAR_SIZE, success);
	}

	{ // cdn endpoint url getter
		set_test(UTILITY_CDN_ENDPOINT_URL_HASH, false);
		bool success = false;
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png }, "foobar/test", "", dpp::i_jpg, 0).empty();
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png }, "foobar/test", "", dpp::i_png, 0) == "https://cdn.discordapp.com/foobar/test.png" && success;
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png }, "foobar/test", "", dpp::i_png, 128) == "https://cdn.discordapp.com/foobar/test.png?size=128" && success;
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png, dpp::i_gif }, "foobar/test", "12345", dpp::i_gif, 0, false, true) == "https://cdn.discordapp.com/foobar/test/a_12345.gif" && success;
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png, dpp::i_gif }, "foobar/test", "12345", dpp::i_png, 0, false, true) == "https://cdn.discordapp.com/foobar/test/a_12345.png" && success;
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png, dpp::i_gif }, "foobar/test", "12345", dpp::i_png, 0, false, false) == "https://cdn.discordapp.com/foobar/test/12345.png" && success;
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png, dpp::i_gif }, "foobar/test", "12345", dpp::i_png, 0, true, true) == "https://cdn.discordapp.com/foobar/test/a_12345.gif" && success;
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png, dpp::i_gif }, "foobar/test", "", dpp::i_png, 0, true, true) == "https://cdn.discordapp.com/foobar/test.gif" && success;
		success = dpp::utility::cdn_endpoint_url_hash({ dpp::i_png, dpp::i_gif }, "foobar/test", "", dpp::i_gif, 0, false, false).empty() && success;
		set_test(UTILITY_CDN_ENDPOINT_URL_HASH, success);
	}

	{ // sticker url getter
		set_test(STICKER_GET_URL, false);
		dpp::sticker s;
		s.format_type = dpp::sf_png;
		bool success = s.get_url().empty();
		s.id = 12345;
		success = s.get_url() == "https://cdn.discordapp.com/stickers/12345.png" && success;
		s.format_type = dpp::sf_gif;
		success = s.get_url() == "https://cdn.discordapp.com/stickers/12345.gif" && success;
		s.format_type = dpp::sf_lottie;
		success = s.get_url() == "https://cdn.discordapp.com/stickers/12345.json" && success;
		set_test(STICKER_GET_URL, success);
	}

	{ // user url getter
		dpp::user user1;
		user1.id = 189759562910400512;
		user1.username = "Brain";
		user1.discriminator = 0001;

		auto user2 = user1;
		user2.avatar = "5532c6414c70765a28cf9448c117205f";

		auto user3 = user2;
		user3.flags |= dpp::u_animated_icon;

		set_test(USER_GET_AVATAR_URL, false);
		set_test(USER_GET_AVATAR_URL,
				 dpp::user().get_avatar_url().empty() &&
				 user1.get_avatar_url() == dpp::utility::cdn_host + "/embed/avatars/1.png" &&
				 user2.get_avatar_url() == dpp::utility::cdn_host + "/avatars/189759562910400512/5532c6414c70765a28cf9448c117205f.png" &&
				 user2.get_avatar_url(0, dpp::i_webp) == dpp::utility::cdn_host + "/avatars/189759562910400512/5532c6414c70765a28cf9448c117205f.webp" &&
				 user2.get_avatar_url(0, dpp::i_jpg) == dpp::utility::cdn_host + "/avatars/189759562910400512/5532c6414c70765a28cf9448c117205f.jpg" &&
				 user3.get_avatar_url() == dpp::utility::cdn_host + "/avatars/189759562910400512/a_5532c6414c70765a28cf9448c117205f.gif" &&
				 user3.get_avatar_url(4096, dpp::i_gif) == dpp::utility::cdn_host + "/avatars/189759562910400512/a_5532c6414c70765a28cf9448c117205f.gif?size=4096" &&
				 user3.get_avatar_url(512, dpp::i_webp) == dpp::utility::cdn_host + "/avatars/189759562910400512/a_5532c6414c70765a28cf9448c117205f.gif?size=512" &&
				 user3.get_avatar_url(512, dpp::i_jpg) == dpp::utility::cdn_host + "/avatars/189759562910400512/a_5532c6414c70765a28cf9448c117205f.gif?size=512" &&
				 user3.get_avatar_url(16, dpp::i_jpg, false) == dpp::utility::cdn_host + "/avatars/189759562910400512/a_5532c6414c70765a28cf9448c117205f.jpg?size=16" &&
				 user3.get_avatar_url(5000) == dpp::utility::cdn_host + "/avatars/189759562910400512/a_5532c6414c70765a28cf9448c117205f.gif"
		);
	}

	{ // emoji url getter
		dpp::emoji emoji;
		emoji.id = 825407338755653641;

		set_test(EMOJI_GET_URL, false);
		set_test(EMOJI_GET_URL, emoji.get_url() == dpp::utility::cdn_host + "/emojis/825407338755653641.png");
	}

	{ // message methods
		dpp::message m;
		m.guild_id = 825407338755653642;
		m.channel_id = 956230231277072415;
		m.id = 1151617986541666386;

		dpp::message m2;
		m2.guild_id = 825407338755653642;
		m2.channel_id = 956230231277072415;

		dpp::message m3;
		m3.guild_id = 825407338755653642;
		m3.id = 1151617986541666386;

		dpp::message m4;
		m4.channel_id = 956230231277072415;
		m4.id = 1151617986541666386;

		dpp::message m5;
		m5.guild_id = 825407338755653642;

		dpp::message m6;
		m6.channel_id = 956230231277072415;

		dpp::message m7;
		m7.id = 1151617986541666386;

		dpp::message m8;

		set_test(MESSAGE_GET_URL, false);
		set_test(MESSAGE_GET_URL,
				 m.get_url() == dpp::utility::url_host + "/channels/825407338755653642/956230231277072415/1151617986541666386" &&
				 m2.get_url() == "" &&
				 m3.get_url() == "" &&
				 m4.get_url() == "" &&
				 m5.get_url() == "" &&
				 m6.get_url() == "" &&
				 m7.get_url() == "" &&
				 m8.get_url() == "" 
		);
	}

	{ // channel methods
		set_test(CHANNEL_SET_TYPE, false);
		dpp::channel c;
		c.set_flags(dpp::c_nsfw | dpp::c_video_quality_720p);
		c.set_type(dpp::CHANNEL_CATEGORY);
		bool before = c.is_category() && !c.is_forum();
		c.set_type(dpp::CHANNEL_FORUM);
		bool after = !c.is_category() && c.is_forum();
		set_test(CHANNEL_SET_TYPE, before && after);

		set_test(CHANNEL_GET_MENTION, false);
		c.id = 825411707521728511;
		set_test(CHANNEL_GET_MENTION, c.get_mention() == "<#825411707521728511>");

		set_test(CHANNEL_GET_URL, false);
		c.guild_id = 825407338755653642;

		dpp::channel c2;
		c2.id = 825411707521728511;

		dpp::channel c3;
		c3.guild_id = 825407338755653642;

		dpp::channel c4;

		set_test(CHANNEL_GET_URL,
				 c.get_url() == dpp::utility::url_host + "/channels/825407338755653642/825411707521728511" &&
				 c2.get_url() == "" &&
				 c3.get_url() == "" &&
				 c4.get_url() == ""
		);
	}

	{ // utility methods
		set_test(UTILITY_GUILD_NAVIGATION, false);
		auto gn1 = dpp::utility::guild_navigation(123, dpp::utility::gnt_customize);
		auto gn2 = dpp::utility::guild_navigation(1234, dpp::utility::gnt_browse);
		auto gn3 = dpp::utility::guild_navigation(12345, dpp::utility::gnt_guide);
		set_test(UTILITY_GUILD_NAVIGATION, gn1 == "<123:customize>" && gn2 == "<1234:browse>" && gn3 == "<12345:guide>");

		set_test(UTILITY_ICONHASH, false);
		auto iconhash1 = dpp::utility::iconhash("a_5532c6414c70765a28cf9448c117205f");
		set_test(UTILITY_ICONHASH, iconhash1.first == 6139187225817019994 &&
									 iconhash1.second == 2940732121894297695 &&
									 iconhash1.to_string() == "5532c6414c70765a28cf9448c117205f"
		);

		set_test(UTILITY_MAKE_URL_PARAMETERS, false);
		auto url_params1 = dpp::utility::make_url_parameters({
			{"foo", 15},
			{"bar", 7}
		});
		auto url_params2 = dpp::utility::make_url_parameters({
			{"foo", "hello"},
			{"bar", "two words"}
		});
		set_test(UTILITY_MAKE_URL_PARAMETERS, url_params1 == "?bar=7&foo=15" && url_params2 == "?bar=two%20words&foo=hello");

		set_test(UTILITY_MARKDOWN_ESCAPE, false);
		auto escaped = dpp::utility::markdown_escape(
				"> this is a quote\n"
				"**some bold text**");
		set_test(UTILITY_MARKDOWN_ESCAPE, "\\>this is a quote\\n\\*\\*some bold text\\*\\*");

		set_test(UTILITY_TOKENIZE, false);
		auto tokens = dpp::utility::tokenize("some Whitespace seperated Text to Tokenize", " ");
		std::vector<std::string> expected_tokens = {"some", "Whitespace", "seperated", "Text", "to", "Tokenize"};
		set_test(UTILITY_TOKENIZE, tokens == expected_tokens);

		set_test(UTILITY_URL_ENCODE, false);
		auto url_encoded = dpp::utility::url_encode("S2-^$1Nd+U!g'8+_??o?p-bla bla");
		set_test(UTILITY_URL_ENCODE, url_encoded == "S2-%5E%241Nd%2BU%21g%278%2B_%3F%3Fo%3Fp-bla%20bla");

		set_test(UTILITY_SLASHCOMMAND_MENTION, false);
		auto mention1 = dpp::utility::slashcommand_mention(123, "name");
		auto mention2 = dpp::utility::slashcommand_mention(123, "name", "sub");
		auto mention3 = dpp::utility::slashcommand_mention(123, "name", "group", "sub");
		bool success = mention1 == "</name:123>" && mention2 == "</name sub:123>" && mention3 == "</name group sub:123>";
		set_test(UTILITY_SLASHCOMMAND_MENTION, success);

		set_test(UTILITY_CHANNEL_MENTION, false);
		auto channel_mention = dpp::utility::channel_mention(123);
		set_test(UTILITY_CHANNEL_MENTION, channel_mention == "<#123>");

		set_test(UTILITY_USER_MENTION, false);
		auto user_mention = dpp::utility::user_mention(123);
		set_test(UTILITY_USER_MENTION, user_mention == "<@123>");

		set_test(UTILITY_ROLE_MENTION, false);
		auto role_mention = dpp::utility::role_mention(123);
		set_test(UTILITY_ROLE_MENTION, role_mention == "<@&123>");

		set_test(UTILITY_EMOJI_MENTION, false);
		auto emoji_mention1 = dpp::utility::emoji_mention("role1", 123, false);
		auto emoji_mention2 = dpp::utility::emoji_mention("role2", 234, true);
		auto emoji_mention3 = dpp::utility::emoji_mention("white_check_mark", 0, false);
		auto emoji_mention4 = dpp::utility::emoji_mention("white_check_mark", 0, true);
		set_test(UTILITY_EMOJI_MENTION,
				 emoji_mention1 == "<:role1:123>" &&
				 emoji_mention2 == "<a:role2:234>" &&
				 emoji_mention3 == ":white_check_mark:" &&
				 emoji_mention4 == ":white_check_mark:"
		);

		set_test(UTILITY_USER_URL, false);
		auto user_url = dpp::utility::user_url(123);
		set_test(UTILITY_USER_URL, 
				 user_url == dpp::utility::url_host + "/users/123" && 
				 dpp::utility::user_url(0) == ""
		);

		set_test(UTILITY_MESSAGE_URL, false);
		auto message_url = dpp::utility::message_url(1,2,3);
		set_test(UTILITY_MESSAGE_URL,
				 message_url == dpp::utility::url_host+ "/channels/1/2/3" && 
				 dpp::utility::message_url(0,2,3) == "" &&
				 dpp::utility::message_url(1,0,3) == "" && 
				 dpp::utility::message_url(1,2,0) == "" &&
				 dpp::utility::message_url(0,0,3) == "" &&
				 dpp::utility::message_url(0,2,0) == "" &&
				 dpp::utility::message_url(1,0,0) == "" &&
				 dpp::utility::message_url(0,0,0) == "" 
		);

		set_test(UTILITY_CHANNEL_URL, false);
		auto channel_url = dpp::utility::channel_url(1,2);
		set_test(UTILITY_CHANNEL_URL, 
				 channel_url == dpp::utility::url_host+ "/channels/1/2" &&
				 dpp::utility::channel_url(0,2) == "" &&
				 dpp::utility::channel_url(1,0) == "" &&
				 dpp::utility::channel_url(0,0) == "" 
		);

		set_test(UTILITY_THREAD_URL, false);
		auto thread_url = dpp::utility::thread_url(1,2);
		set_test(UTILITY_THREAD_URL,
				 thread_url == dpp::utility::url_host+ "/channels/1/2" &&
				 dpp::utility::thread_url(0,2) == "" &&
				 dpp::utility::thread_url(1,0) == "" &&
				 dpp::utility::thread_url(0,0) == "" 
		);
	}

#ifndef _WIN32
	set_test(TIMESTRINGTOTIMESTAMP, false);
	json tj;
	tj["t1"] = "2022-01-19T17:18:14.506000+00:00";
	tj["t2"] = "2022-01-19T17:18:14+00:00";
	uint32_t inTimestamp = 1642612694;
	set_test(TIMESTRINGTOTIMESTAMP, (uint64_t)dpp::ts_not_null(&tj, "t1") == inTimestamp && (uint64_t)dpp::ts_not_null(&tj, "t2") == inTimestamp);
#else
	set_test(TIMESTRINGTOTIMESTAMP, true);
#endif

	{
		set_test(TS, false);
		dpp::managed m(189759562910400512);
		set_test(TS, ((uint64_t) m.get_creation_time()) == 1465312605);
	}

	{
		coro_offline_tests();
	}

	std::vector<std::byte> dpp_logo = load_data("DPP-Logo.png");

	set_test(PRESENCE, false);
	set_test(CLUSTER, false);
	try {
		dpp::cluster bot(token, dpp::i_all_intents);
		bot.set_websocket_protocol(dpp::ws_etf);
		set_test(CLUSTER, true);
		set_test(CONNECTION, false);
		set_test(GUILDCREATE, false);
		set_test(ICONHASH, false);

		set_test(MSGCOLLECT, false);
		if (!offline) {
			/* Intentional leak: freed on unit test end */
			[[maybe_unused]]
			message_collector* collect_messages = new message_collector(&bot, 25);
		}

		set_test(JSON_PARSE_ERROR, false);
		dpp::rest_request<dpp::confirmation>(&bot, "/nonexistent", "address", "", dpp::m_get, "", [](const dpp::confirmation_callback_t& e) {
			if (e.is_error() && e.get_error().code == 404) {
				set_test(JSON_PARSE_ERROR, true);
			} else {
				set_test(JSON_PARSE_ERROR, false);
			}
		});

		if (!offline) {
			start_test(INVALIDUTF8);
			bot.message_create(dpp::message(TEST_TEXT_CHANNEL_ID, "ä\xA9ü"), [](const auto &cc) {
				set_status(INVALIDUTF8, ts_success);
			});
		} else {
			set_status(INVALIDUTF8, ts_skipped);
		}

		dpp::utility::iconhash i;
		std::string dummyval("fcffffffffffff55acaaaaaaaaaaaa66");
		i = dummyval;
		set_test(ICONHASH, (i.to_string() == dummyval));

		/* This ensures we test both protocols, as voice is json and shard is etf */
		bot.set_websocket_protocol(dpp::ws_etf);

		bot.on_form_submit([&](const dpp::form_submit_t & event) {
		});

		/* This is near impossible to test without a 'clean room' voice channel.
		 * We attach this event just so that the decoder events are fired while we
		 * are sending audio later, this way if the audio receive code is plain unstable
		 * the test suite will crash and fail.
		 */
		bot.on_voice_receive_combined([&](const auto& event) {
		});

		bot.on_guild_create([&](const dpp::guild_create_t& event) {
			dpp::guild *g = event.created;

			if (g->id == TEST_GUILD_ID) {
				start_test(GUILD_EDIT);
				g->set_icon(dpp::i_png, dpp_logo.data(), static_cast<uint32_t>(dpp_logo.size()));
				bot.guild_edit(*g, [&bot](const dpp::confirmation_callback_t &result) {
					if (result.is_error()) {
						set_status(GUILD_EDIT, ts_failed, "guild_edit 1 errored:\n" + result.get_error().human_readable);
						return;
					}
					dpp::guild g = result.get<dpp::guild>();

					if (g.get_icon_url().empty()) {
						set_status(GUILD_EDIT, ts_failed, "icon not set or not retrieved");
						return;
					}
					g.remove_icon();
					bot.guild_edit(g, [](const dpp::confirmation_callback_t &result) {
						if (result.is_error()) {
							set_status(GUILD_EDIT, ts_failed, "guild_edit 2 errored:\n" + result.get_error().human_readable);
							return;
						}
						const dpp::guild &g = result.get<dpp::guild>();
						if (!g.get_icon_url().empty()) {
							set_status(GUILD_EDIT, ts_failed, "icon not removed");
							return;
						}
						set_status(GUILD_EDIT, ts_success);
					});
				});
			}
		});

		std::promise<void> ready_promise;
		std::future ready_future = ready_promise.get_future();
		bot.on_ready([&](const dpp::ready_t & event) {
			set_test(CONNECTION, true);
			ready_promise.set_value();

			set_test(APPCOMMAND, false);
			set_test(LOGGER, false);
			bot.log(dpp::ll_info, "Test log message");

			bot.guild_command_create(dpp::slashcommand().set_name("testcommand")
				.set_description("Test command for DPP unit test")
				.add_option(dpp::command_option(dpp::co_attachment, "file", "a file"))
				.set_application_id(bot.me.id)
				.add_localization("fr", "zut", "Ou est la salor dans Discord?"),
				TEST_GUILD_ID, [&](const dpp::confirmation_callback_t &callback) {
					if (!callback.is_error()) {
						set_test(APPCOMMAND, true);
						set_test(DELCOMMAND, false);
						dpp::slashcommand s = std::get<dpp::slashcommand>(callback.value);
						bot.guild_command_delete(s.id, TEST_GUILD_ID, [&](const dpp::confirmation_callback_t &callback) {
							if (!callback.is_error()) {
								dpp::message test_message(TEST_TEXT_CHANNEL_ID, "test message");

								set_test(DELCOMMAND, true);
								set_test(MESSAGECREATE, false);
								set_test(MESSAGEEDIT, false);
								set_test(MESSAGERECEIVE, false);
								test_message.add_file("no-mime", "test");
								test_message.add_file("test.txt", "test", "text/plain");
								test_message.add_file("test.png", std::string{reinterpret_cast<const char*>(dpp_logo.data()), dpp_logo.size()}, "image/png");
								bot.message_create(test_message, [&bot](const dpp::confirmation_callback_t &callback) {
									if (!callback.is_error()) {
										set_test(MESSAGECREATE, true);
										set_test(REACT, false);
										dpp::message m = std::get<dpp::message>(callback.value);
										set_test(REACTEVENT, false);
										bot.message_add_reaction(m.id, TEST_TEXT_CHANNEL_ID, "😄", [](const dpp::confirmation_callback_t &callback) {
											if (!callback.is_error()) {
												set_test(REACT, true);
											} else {
												set_test(REACT, false);
											}
										});
										set_test(EDITEVENT, false);
										bot.message_edit(dpp::message(m).set_content("test edit"), [](const dpp::confirmation_callback_t &callback) {
											if (!callback.is_error()) {
												set_test(MESSAGEEDIT, true);
											}
										});
									}
								});
							} else {
								set_test(DELCOMMAND, false);
							}
						});
					}
				});
		});

		std::mutex loglock;
		bot.on_log([&](const dpp::log_t & event) {
			std::lock_guard<std::mutex> locker(loglock);
			if (event.severity > dpp::ll_trace) {
				std::cout << "[" << std::fixed << std::setprecision(3) << (dpp::utility::time_f() - get_start_time()) << "]: [\u001b[36m" << dpp::utility::loglevel(event.severity) << "\u001b[0m] " << event.message << "\n";
			}
			if (event.message == "Test log message") {
				set_test(LOGGER, true);
			}
		});

		set_test(RUNONCE, false);
		uint8_t runs = 0;
		for (int x = 0; x < 10; ++x) {
			if (dpp::run_once<struct test_run>()) {
				runs++;
			}
		}
		set_test(RUNONCE, (runs == 1));

		bot.on_voice_ready([&](const dpp::voice_ready_t & event) {
			set_test(VOICECONN, true);
			dpp::discord_voice_client* v = event.voice_client;
			set_test(VOICESEND, false);
			if (v && v->is_ready()) {
				v->send_audio_raw((uint16_t*)testaudio.data(), testaudio.size());
			} else {
				set_test(VOICESEND, false);
			}
		});

		bot.on_invite_create([](const dpp::invite_create_t &event) {
			auto &inv = event.created_invite;
			if (!inv.code.empty() && inv.channel_id == TEST_TEXT_CHANNEL_ID && inv.guild_id == TEST_GUILD_ID && inv.created_at != 0 && inv.max_uses == 100) {
				set_test(INVITE_CREATE_EVENT, true);
			}
		});

		bot.on_invite_delete([](const dpp::invite_delete_t &event) {
			auto &inv = event.deleted_invite;
			if (!inv.code.empty() && inv.channel_id == TEST_TEXT_CHANNEL_ID && inv.guild_id == TEST_GUILD_ID) {
				set_test(INVITE_DELETE_EVENT, true);
			}
		});

		bot.on_voice_buffer_send([&](const dpp::voice_buffer_send_t & event) {
			if (event.buffer_size == 0) {
				set_test(VOICESEND, true);
			}
		});

		set_test(SYNC, false);
		if (!offline) {
			dpp::message m = dpp::sync<dpp::message>(&bot, &dpp::cluster::message_create, dpp::message(TEST_TEXT_CHANNEL_ID, "TEST"));
			set_test(SYNC, m.content == "TEST");
		}

		bot.on_guild_create([&](const dpp::guild_create_t & event) {
			if (event.created->id == TEST_GUILD_ID) {
				set_test(GUILDCREATE, true);
				if (event.presences.size() && event.presences.begin()->second.user_id > 0) {
					set_test(PRESENCE, true);
				}
				dpp::guild* g = dpp::find_guild(TEST_GUILD_ID);
				set_test(CACHE, false);
				if (g) {
					set_test(CACHE, true);
					set_test(VOICECONN, false);
					dpp::discord_client* s = bot.get_shard(0);
					s->connect_voice(g->id, TEST_VC_ID, false, false);
				}
				else {
					set_test(CACHE, false);
				}
			}
		});

		// this helper class contains logic for the message tests, deletes the message when all tests are done
		class message_test_helper
		{
		private:
			std::mutex mutex;
			bool pin_tested;
			bool thread_tested;
			std::array<bool, 3> files_tested;
			std::array<bool, 3> files_success;
			dpp::snowflake channel_id;
			dpp::snowflake message_id;
			dpp::cluster &bot;

			void delete_message_if_done() {
				if (files_tested == std::array{true, true, true} && pin_tested && thread_tested) {
					set_test(MESSAGEDELETE, false);
					bot.message_delete(message_id, channel_id, [](const dpp::confirmation_callback_t &callback) {
						if (!callback.is_error()) {
							set_test(MESSAGEDELETE, true);
						}
					});
				}
			}

			void set_pin_tested() {
				assert(!pin_tested);
				pin_tested = true;
				delete_message_if_done();
			}

			void set_thread_tested() {
				assert(!thread_tested);
				thread_tested = true;
				delete_message_if_done();
			}

			void set_file_tested(size_t index) {
				assert(!files_tested[index]);
				files_tested[index] = true;
				if (files_tested == std::array{true, true, true}) {
					set_test(MESSAGEFILE, files_success == std::array{true, true, true});
				}
				delete_message_if_done();
			}

			void test_threads(const dpp::message &message)
			{
				set_test(THREAD_CREATE_MESSAGE, false);
				set_test(THREAD_DELETE, false);
				set_test(THREAD_DELETE_EVENT, false);
				bot.thread_create_with_message("test", message.channel_id, message.id, 60, 60, [this](const dpp::confirmation_callback_t &callback) {
					std::lock_guard lock(mutex);
					if (callback.is_error()) {
						set_thread_tested();
					}
					else {
						auto thread = callback.get<dpp::thread>();
						thread_id = thread.id;
						set_test(THREAD_CREATE_MESSAGE, true);
						bot.channel_delete(thread.id, [this](const dpp::confirmation_callback_t &callback) {
							set_test(THREAD_DELETE, !callback.is_error());
							set_thread_tested();
						});
					}
				});
			}

			void test_files(const dpp::message &message) {
				set_test(MESSAGEFILE, false);
				if (message.attachments.size() == 3) {
					static constexpr auto check_mimetype = [](const auto &headers, std::string mimetype) {
						if (auto it = headers.find("content-type"); it != headers.end()) {
							// check that the mime type starts with what we gave : for example discord will change "text/plain" to "text/plain; charset=UTF-8"
							return it->second.size() >= mimetype.size() && std::equal(it->second.begin(), it->second.begin() + mimetype.size(), mimetype.begin());
						}
						else {
							return false;
						}
					};
					message.attachments[0].download([&](const dpp::http_request_completion_t &callback) {
						std::lock_guard lock(mutex);
						if (callback.status == 200 && callback.body == "test") {
							files_success[0] = true;
						}
						set_file_tested(0);
					});
					message.attachments[1].download([&](const dpp::http_request_completion_t &callback) {
						std::lock_guard lock(mutex);
						if (callback.status == 200 && check_mimetype(callback.headers, "text/plain") && callback.body == "test") {
							files_success[1] = true;
						}
						set_file_tested(1);
					});
					message.attachments[2].download([&](const dpp::http_request_completion_t &callback) {
						std::lock_guard lock(mutex);
						// do not check the contents here because discord can change compression
						if (callback.status == 200 && check_mimetype(callback.headers, "image/png")) {
							files_success[2] = true;
						}
						set_file_tested(2);
					});
				}
				else {
					set_file_tested(0);
					set_file_tested(1);
					set_file_tested(2);
				}
			}

			void test_pin() {
				if (!extended) {
					set_pin_tested();
					return;
				}
				set_test(MESSAGEPIN, false);
				set_test(MESSAGEUNPIN, false);
				bot.message_pin(channel_id, message_id, [this](const dpp::confirmation_callback_t &callback) {
					std::lock_guard lock(mutex);
					if (!callback.is_error()) {
						set_test(MESSAGEPIN, true);
						bot.message_unpin(TEST_TEXT_CHANNEL_ID, message_id, [this](const dpp::confirmation_callback_t &callback) {
							std::lock_guard lock(mutex);
							if (!callback.is_error()) {
								set_test(MESSAGEUNPIN, true);
							}
							set_pin_tested();
						});
					}
					else {
						set_pin_tested();
					}
				});
			}

		public:
			dpp::snowflake thread_id;

			message_test_helper(dpp::cluster &_bot) : bot(_bot) {}

			void run(const dpp::message &message) {
				pin_tested = false;
				thread_tested = false;
				files_tested = {false, false, false};
				files_success = {false, false, false};
				channel_id = message.channel_id;
				message_id = message.id;
				test_pin();
				test_files(message);
				test_threads(message);
			}
		};

		message_test_helper message_helper(bot);

		class thread_test_helper
		{
		public:
			enum event_flag
			{
				MESSAGE_CREATE = 1 << 0,
				MESSAGE_EDIT = 1 << 1,
				MESSAGE_REACT = 1 << 2,
				MESSAGE_REMOVE_REACT = 1 << 3,
				MESSAGE_DELETE = 1 << 4,
				EVENT_END = 1 << 5
			};
		private:
			std::mutex mutex;
			dpp::cluster &bot;
			bool edit_tested = false;
			bool members_tested = false;
			bool messages_tested = false;
			bool events_tested = false;
			bool get_active_tested = false;
			uint32_t events_tested_mask = 0;
			uint32_t events_to_test_mask = 0;

			void delete_if_done()
			{
				if (edit_tested && members_tested && messages_tested && events_tested && get_active_tested) {
					bot.channel_delete(thread_id);
				}
			}

			void set_events_tested()
			{
				if (events_tested) {
					return;
				}
				events_tested = true;
				delete_if_done();
			}

			void set_edit_tested()
			{
				if (edit_tested) {
					return;
				}
				edit_tested = true;
				delete_if_done();
			}

			void set_members_tested()
			{
				if (members_tested) {
					return;
				}
				members_tested = true;
				delete_if_done();
			}

			void set_get_active_tested()
			{
				if (get_active_tested) {
					return;
				}
				get_active_tested = true;
				delete_if_done();
			}

			void set_messages_tested()
			{
				if (messages_tested) {
					return;
				}
				messages_tested = true;
				delete_if_done();
			}

			void set_event_tested(event_flag flag)
			{
				if (events_tested_mask & flag) {
					return;
				}
				events_tested_mask |= flag;
				for (uint32_t i = 1; i < EVENT_END; i <<= 1) {
					if ((events_to_test_mask & i) && (events_tested_mask & i) != i) {
						return;
					}
				}
				set_events_tested();
			}

			void events_abort()
			{
				events_tested_mask |= ~events_to_test_mask;
				for (uint32_t i = 1; i < EVENT_END; i <<= 1) {
					if ((events_tested_mask & i) != i) {
						return;
					}
				}
				set_events_tested();
			}

		public:
			/**
			 * @Brief wrapper for set_event_tested, locking the mutex. Meant to be used from outside the class
			 */
			void notify_event_tested(event_flag flag)
			{
				std::lock_guard lock{mutex};

				set_event_tested(flag);
			}

			dpp::snowflake thread_id;

			void test_edit(const dpp::thread &thread)
			{
				std::lock_guard lock{mutex};

				if (!edit_tested) {
					dpp::thread edit = thread;
					set_test(THREAD_EDIT, false);
					set_test(THREAD_UPDATE_EVENT, false);
					edit.name = "edited";
					edit.metadata.locked = true;
					bot.thread_edit(edit, [this](const dpp::confirmation_callback_t &callback) {
						std::lock_guard lock(mutex);
						if (!callback.is_error()) {
							set_test(THREAD_EDIT, true);
						}
						set_edit_tested();
					});
				}
			}

			void test_get_active(const dpp::thread &thread)
			{
				std::lock_guard lock{mutex};

				set_test(THREAD_GET_ACTIVE, false);
				bot.threads_get_active(TEST_GUILD_ID, [this](const dpp::confirmation_callback_t &callback) {
					std::lock_guard lock{mutex};
					if (!callback.is_error()) {
						const auto &threads = callback.get<dpp::active_threads>();
						if (auto thread_it = threads.find(thread_id); thread_it != threads.end()) {
							const auto &thread = thread_it->second.active_thread;
							const auto &member = thread_it->second.bot_member;
							if (thread.id == thread_id && member.has_value() && member->user_id == bot.me.id) {
								set_test(THREAD_GET_ACTIVE, true);
							}
						}
					}
					set_get_active_tested();
				});
			}

			void test_members(const dpp::thread &thread)
			{
				std::lock_guard lock{mutex};

				if (!members_tested) {
					if (!extended) {
						set_members_tested();
						return;
					}
					set_test(THREAD_MEMBER_ADD, false);
					set_test(THREAD_MEMBER_GET, false);
					set_test(THREAD_MEMBERS_GET, false);
					set_test(THREAD_MEMBER_REMOVE, false);
					set_test(THREAD_MEMBERS_ADD_EVENT, false);
					set_test(THREAD_MEMBERS_REMOVE_EVENT, false);
					bot.thread_member_add(thread_id, TEST_USER_ID, [this](const dpp::confirmation_callback_t &callback) {
						std::lock_guard lock{mutex};
						if (callback.is_error()) {
							set_members_tested();
							return;
						}
						set_test(THREAD_MEMBER_ADD, true);
						bot.thread_member_get(thread_id, TEST_USER_ID, [this](const dpp::confirmation_callback_t &callback) {
							std::lock_guard lock{mutex};
							if (callback.is_error()) {
								set_members_tested();
								return;
							}
							set_test(THREAD_MEMBER_GET, true);
							bot.thread_members_get(thread_id, [this](const dpp::confirmation_callback_t &callback) {
								std::lock_guard lock{mutex};
								if (callback.is_error()) {
									set_members_tested();
									return;
								}
								const auto &members = callback.get<dpp::thread_member_map>();
								if (members.find(TEST_USER_ID) == members.end() || members.find(bot.me.id) == members.end()) {
									set_members_tested();
									return;
								}
								set_test(THREAD_MEMBERS_GET, true);
								bot.thread_member_remove(thread_id, TEST_USER_ID, [this](const dpp::confirmation_callback_t &callback) {
									std::lock_guard lock{mutex};
									if (!callback.is_error()) {
										set_test(THREAD_MEMBER_REMOVE, true);
									}
									set_members_tested();
								});
							});
						});
					});
				}
			}

			void test_messages(const dpp::thread &thread)
			{
				if (!extended) {
					set_messages_tested();
					set_events_tested();
					return;
				}

				std::lock_guard lock{mutex};
				set_test(THREAD_MESSAGE, false);
				set_test(THREAD_MESSAGE_CREATE_EVENT, false);
				set_test(THREAD_MESSAGE_EDIT_EVENT, false);
				set_test(THREAD_MESSAGE_REACT_ADD_EVENT, false);
				set_test(THREAD_MESSAGE_REACT_REMOVE_EVENT, false);
				set_test(THREAD_MESSAGE_DELETE_EVENT, false);
				events_to_test_mask |= MESSAGE_CREATE;
				bot.message_create(dpp::message{"hello thread"}.set_channel_id(thread.id), [this](const dpp::confirmation_callback_t &callback) {
					std::lock_guard lock{mutex};
					if (callback.is_error()) {
						events_abort();
						set_messages_tested();
						return;
					}
					auto m = callback.get<dpp::message>();
					m.content = "hello thread?";
					events_to_test_mask |= MESSAGE_EDIT;
					bot.message_edit(m, [this, message_id = m.id](const dpp::confirmation_callback_t &callback) {
						std::lock_guard lock{mutex};
						if (callback.is_error()) {
							events_abort();
							set_messages_tested();
							return;
						}
						events_to_test_mask |= MESSAGE_REACT;
						bot.message_add_reaction(message_id, thread_id, dpp::unicode_emoji::thread, [this, message_id](const dpp::confirmation_callback_t &callback) {
							std::lock_guard lock{mutex};
							if (callback.is_error()) {
								events_abort();
								set_messages_tested();
								return;
							}
							events_to_test_mask |= MESSAGE_REMOVE_REACT;
							bot.message_delete_reaction(message_id, thread_id, bot.me.id, dpp::unicode_emoji::thread, [this, message_id](const dpp::confirmation_callback_t &callback) {
								std::lock_guard lock{mutex};
								if (callback.is_error()) {
									events_abort();
									set_messages_tested();
									return;
								}
								events_to_test_mask |= MESSAGE_DELETE;
								bot.message_delete(message_id, thread_id, [this] (const dpp::confirmation_callback_t &callback) {
									std::lock_guard lock{mutex};
									set_messages_tested();
									if (callback.is_error()) {
										events_abort();
										return;
									}
									set_test(THREAD_MESSAGE, true);
								});
							});
						});
					});
				});
			}

			void run(const dpp::thread &thread)
			{
				thread_id = thread.id;
				test_get_active(thread);
				test_edit(thread);
				test_members(thread);
				test_messages(thread);
			}

			thread_test_helper(dpp::cluster &bot_) : bot{bot_}
			{
			}
		};

		thread_test_helper thread_helper(bot);

		bot.on_thread_create([&](const dpp::thread_create_t &event) {
			if (event.created.name == "thread test") {
				set_test(THREAD_CREATE_EVENT, true);
				thread_helper.run(event.created);
			}
		});

		bool message_tested = false;
		bot.on_message_create([&](const dpp::message_create_t & event) {
			if (event.msg.author.id == bot.me.id) {
				if (event.msg.content == "test message" && !message_tested) {
					message_tested = true;
					set_test(MESSAGERECEIVE, true);
					message_helper.run(event.msg);
					set_test(MESSAGESGET, false);
					bot.messages_get(event.msg.channel_id, 0, event.msg.id, 0, 5, [](const dpp::confirmation_callback_t &cc){
						if (!cc.is_error()) {
							dpp::message_map mm = std::get<dpp::message_map>(cc.value);
							if (mm.size()) {
								set_test(MESSAGESGET, true);
								set_test(TIMESTAMP, false);
								dpp::message m = mm.begin()->second;
								if (m.sent > 0) {
									set_test(TIMESTAMP, true);
								} else {
									set_test(TIMESTAMP, false);
								}
							} else {
								set_test(MESSAGESGET, false);	
							}
						}  else {
							set_test(MESSAGESGET, false);
						}
					});
					set_test(MSGCREATESEND, false);
					event.send("MSGCREATESEND", [&bot, ch_id = event.msg.channel_id] (const auto& cc) {
						if (!cc.is_error()) {
							dpp::message m = std::get<dpp::message>(cc.value);
							if (m.channel_id == ch_id) {
								set_test(MSGCREATESEND, true);
							} else {
								bot.log(dpp::ll_debug, cc.http_info.body);
								set_test(MSGCREATESEND, false);
							}
							bot.message_delete(m.id, m.channel_id);
						} else {
							bot.log(dpp::ll_debug, cc.http_info.body);
							set_test(MSGCREATESEND, false);
						}
					});
				}
				if (event.msg.channel_id == thread_helper.thread_id && event.msg.content == "hello thread") {
					set_test(THREAD_MESSAGE_CREATE_EVENT, true);
					thread_helper.notify_event_tested(thread_test_helper::MESSAGE_CREATE);
				}
			}
		});

		bot.on_message_reaction_add([&](const dpp::message_reaction_add_t & event) {
			if (event.reacting_user.id == bot.me.id) {
				if (event.reacting_emoji.name == "😄") {
					set_test(REACTEVENT, true);
				}
				if (event.channel_id == thread_helper.thread_id && event.reacting_emoji.name == dpp::unicode_emoji::thread) {
					set_test(THREAD_MESSAGE_REACT_ADD_EVENT, true);
					thread_helper.notify_event_tested(thread_test_helper::MESSAGE_REACT);
				}
			}
		});

		bot.on_message_reaction_remove([&](const dpp::message_reaction_remove_t & event) {
			if (event.reacting_user_id == bot.me.id) {
				if (event.channel_id == thread_helper.thread_id && event.reacting_emoji.name == dpp::unicode_emoji::thread) {
					set_test(THREAD_MESSAGE_REACT_REMOVE_EVENT, true);
					thread_helper.notify_event_tested(thread_test_helper::MESSAGE_REMOVE_REACT);
				}
			}
		});

		bot.on_message_delete([&](const dpp::message_delete_t & event) {
			if (event.channel_id == thread_helper.thread_id) {
				set_test(THREAD_MESSAGE_DELETE_EVENT, true);
				thread_helper.notify_event_tested(thread_test_helper::MESSAGE_DELETE);
			}
		});

		bool message_edit_tested = false;
		bot.on_message_update([&](const dpp::message_update_t &event) {
			if (event.msg.author == bot.me.id) {
				if (event.msg.content == "test edit" && !message_edit_tested) {
					message_edit_tested = true;
					set_test(EDITEVENT, true);
				}
				if (event.msg.channel_id == thread_helper.thread_id && event.msg.content == "hello thread?") {
					set_test(THREAD_MESSAGE_EDIT_EVENT, true);
					thread_helper.notify_event_tested(thread_test_helper::MESSAGE_EDIT);
				}
			}
		});

		bot.on_thread_update([&](const dpp::thread_update_t &event) {
			if (event.updating_guild->id == TEST_GUILD_ID && event.updated.id == thread_helper.thread_id && event.updated.name == "edited") {
				set_test(THREAD_UPDATE_EVENT, true);
			}
		});

		bot.on_thread_members_update([&](const dpp::thread_members_update_t &event) {
			if (event.updating_guild->id == TEST_GUILD_ID && event.thread_id == thread_helper.thread_id) {
				if (std::find_if(std::begin(event.added), std::end(event.added), is_owner) != std::end(event.added)) {
					set_test(THREAD_MEMBERS_ADD_EVENT, true);
				}
				if (std::find_if(std::begin(event.removed_ids), std::end(event.removed_ids), is_owner) != std::end(event.removed_ids)) {
					set_test(THREAD_MEMBERS_REMOVE_EVENT, true);
				}
			}
		});

		bot.on_thread_delete([&](const dpp::thread_delete_t &event) {
			if (event.deleting_guild->id == TEST_GUILD_ID && event.deleted.id == message_helper.thread_id) {
				set_test(THREAD_DELETE_EVENT, true);
			}
		});

		// set to execute from this thread (main thread) after on_ready is fired
		auto do_online_tests = [&] {
			coro_online_tests(&bot);
			set_test(GUILD_BAN_CREATE, false);
			set_test(GUILD_BAN_GET, false);
			set_test(GUILD_BANS_GET, false);
			set_test(GUILD_BAN_DELETE, false);
			if (!offline) {
				// some deleted discord accounts to test the ban stuff with...
				dpp::snowflake deadUser1(802670069523415057);
				dpp::snowflake deadUser2(875302419335094292);
				dpp::snowflake deadUser3(1048247361903792198);

				bot.set_audit_reason("ban reason one").guild_ban_add(TEST_GUILD_ID, deadUser1, 0, [deadUser1, deadUser2, deadUser3, &bot](const dpp::confirmation_callback_t &event) {
					if (!event.is_error()) bot.guild_ban_add(TEST_GUILD_ID, deadUser2, 0, [deadUser1, deadUser2, deadUser3, &bot](const dpp::confirmation_callback_t &event) {
						if (!event.is_error()) bot.set_audit_reason("ban reason three").guild_ban_add(TEST_GUILD_ID, deadUser3, 0, [deadUser1, deadUser2, deadUser3, &bot](const dpp::confirmation_callback_t &event) {
							if (event.is_error()) {
								return;
							}
							set_test(GUILD_BAN_CREATE, true);
							// when created, continue with getting and deleting

							// get ban
							bot.guild_get_ban(TEST_GUILD_ID, deadUser1, [deadUser1](const dpp::confirmation_callback_t &event) {
								if (!event.is_error()) {
									dpp::ban ban = event.get<dpp::ban>();
									if (ban.user_id == deadUser1 && ban.reason == "ban reason one") {
										set_test(GUILD_BAN_GET, true);
									}
								}
							});

							// get multiple bans
							bot.guild_get_bans(TEST_GUILD_ID, 0, deadUser1, 3, [deadUser2, deadUser3](const dpp::confirmation_callback_t &event) {
								if (!event.is_error()) {
									dpp::ban_map bans = event.get<dpp::ban_map>();
									int successCount = 0;
									for (auto &ban: bans) {
										if (ban.first == ban.second.user_id) { // the key should match the ban's user_id
											if (ban.first == deadUser2 && ban.second.reason.empty()) {
												successCount++;
											} else if (ban.first == deadUser3 && ban.second.reason == "ban reason three") {
												successCount++;
											}
										}
									}
									if (successCount == 2) {
										set_test(GUILD_BANS_GET, true);
									}
								}
							});

							// unban them
							bot.guild_ban_delete(TEST_GUILD_ID, deadUser1, [&bot, deadUser2, deadUser3](const dpp::confirmation_callback_t &event) {
								if (!event.is_error()) {
									bot.guild_ban_delete(TEST_GUILD_ID, deadUser2, [&bot, deadUser3](const dpp::confirmation_callback_t &event) {
										if (!event.is_error()) {
											bot.guild_ban_delete(TEST_GUILD_ID, deadUser3, [](const dpp::confirmation_callback_t &event) {
												if (!event.is_error()) {
													set_test(GUILD_BAN_DELETE, true);
												}
											});
										}
									});
								}
							});
						});
					});
				});
			}

			set_test(REQUEST_GET_IMAGE, false);
			if (!offline) {
				bot.request("https://dpp.dev/DPP-Logo.png", dpp::m_get, [&bot](const dpp::http_request_completion_t &callback) {
					if (callback.status != 200) {
						return;
					}
					set_test(REQUEST_GET_IMAGE, true);

					dpp::emoji emoji;
					emoji.load_image(callback.body, dpp::i_png);
					emoji.name = "dpp";

					// emoji unit test with the requested image
					set_test(EMOJI_CREATE, false);
					set_test(EMOJI_GET, false);
					set_test(EMOJI_DELETE, false);
					bot.guild_emoji_create(TEST_GUILD_ID, emoji, [&bot](const dpp::confirmation_callback_t &event) {
						if (event.is_error()) {
							return;
						}
						set_test(EMOJI_CREATE, true);

						auto created = event.get<dpp::emoji>();
						bot.guild_emoji_get(TEST_GUILD_ID, created.id, [&bot, created](const dpp::confirmation_callback_t &event) {
							if (event.is_error()) {
								return;
							}
							auto fetched = event.get<dpp::emoji>();
							if (created.id == fetched.id && created.name == fetched.name && created.flags == fetched.flags) {
								set_test(EMOJI_GET, true);
							}

							bot.guild_emoji_delete(TEST_GUILD_ID, fetched.id, [](const dpp::confirmation_callback_t &event) {
								if (!event.is_error()) {
									set_test(EMOJI_DELETE, true);
								}
							});
						});
					});
				});
			}

			set_test(INVITE_CREATE, false);
			set_test(INVITE_GET, false);
			set_test(INVITE_DELETE, false);
			if (!offline) {
				dpp::channel channel;
				channel.id = TEST_TEXT_CHANNEL_ID;
				dpp::invite invite;
				invite.max_age = 0;
				invite.max_uses = 100;
				set_test(INVITE_CREATE_EVENT, false);
				bot.channel_invite_create(channel, invite, [&bot, invite](const dpp::confirmation_callback_t &event) {
					if (event.is_error()) {
						return;
					}

					auto created = event.get<dpp::invite>();
					if (!created.code.empty() && created.channel_id == TEST_TEXT_CHANNEL_ID && created.guild_id == TEST_GUILD_ID && created.inviter.id == bot.me.id) {
						set_test(INVITE_CREATE, true);
					}

					bot.invite_get(created.code, [&bot, created](const dpp::confirmation_callback_t &event) {
						if (!event.is_error()) {
							auto retrieved = event.get<dpp::invite>();
							if (retrieved.code == created.code && retrieved.guild_id == created.guild_id && retrieved.channel_id == created.channel_id && retrieved.inviter.id == created.inviter.id) {
								if (retrieved.destination_guild.flags & dpp::g_community) {
									set_test(INVITE_GET, retrieved.expires_at == 0);
								} else {
									set_test(INVITE_GET, true);
								}

							} else {
								set_test(INVITE_GET, false);
							}
						} else {
							set_test(INVITE_GET, false);
						}

						set_test(INVITE_DELETE_EVENT, false);
						bot.invite_delete(created.code, [](const dpp::confirmation_callback_t &event) {
							set_test(INVITE_DELETE, !event.is_error());
						});
					});
				});
			}

			set_test(AUTOMOD_RULE_CREATE, false);
			set_test(AUTOMOD_RULE_GET, false);
			set_test(AUTOMOD_RULE_GET_ALL, false);
			set_test(AUTOMOD_RULE_DELETE, false);
			if (!offline) {
				dpp::automod_rule automodRule;
				automodRule.name = "automod rule (keyword type)";
				automodRule.trigger_type = dpp::amod_type_keyword;
				dpp::automod_metadata metadata1;
				metadata1.keywords.emplace_back("*cat*");
				metadata1.keywords.emplace_back("train");
				metadata1.keywords.emplace_back("*.exe");
				metadata1.regex_patterns.emplace_back("^[^a-z]$");
				metadata1.allow_list.emplace_back("@silent*");
				automodRule.trigger_metadata = metadata1;
				dpp::automod_action automodAction;
				automodAction.type = dpp::amod_action_timeout;
				automodAction.duration_seconds = 6000;
				automodRule.actions.emplace_back(automodAction);

				bot.automod_rules_get(TEST_GUILD_ID, [&bot, automodRule](const dpp::confirmation_callback_t &event) {
					if (event.is_error()) {
						return;
					}
					auto rules = event.get<dpp::automod_rule_map>();
					set_test(AUTOMOD_RULE_GET_ALL, true);
					for (const auto &rule: rules) {
						if (rule.second.trigger_type == dpp::amod_type_keyword) {
							// delete one automod rule of type KEYWORD before creating one to make space...
							bot.automod_rule_delete(TEST_GUILD_ID, rule.first);
						}
					}

					// start creating the automod rules
					bot.automod_rule_create(TEST_GUILD_ID, automodRule, [&bot, automodRule](const dpp::confirmation_callback_t &event) {
						if (event.is_error()) {
							return;
						}
						auto created = event.get<dpp::automod_rule>();
						if (created.name == automodRule.name) {
							set_test(AUTOMOD_RULE_CREATE, true);
						}

						// get automod rule
						bot.automod_rule_get(TEST_GUILD_ID, created.id, [automodRule, &bot, created](const dpp::confirmation_callback_t &event) {
							if (event.is_error()) {
								return;
							}
							auto retrieved = event.get<dpp::automod_rule>();
							if (retrieved.name == automodRule.name &&
								retrieved.trigger_type == automodRule.trigger_type &&
								retrieved.trigger_metadata.keywords == automodRule.trigger_metadata.keywords &&
								retrieved.trigger_metadata.regex_patterns == automodRule.trigger_metadata.regex_patterns &&
								retrieved.trigger_metadata.allow_list == automodRule.trigger_metadata.allow_list && retrieved.actions.size() == automodRule.actions.size()) {
								set_test(AUTOMOD_RULE_GET, true);
							}

							// delete the automod rule
							bot.automod_rule_delete(TEST_GUILD_ID, retrieved.id, [](const dpp::confirmation_callback_t &event) {
								if (!event.is_error()) {
									set_test(AUTOMOD_RULE_DELETE, true);
								}
							});
						});
					});
				});
			}

			set_test(USER_GET, false);
			set_test(USER_GET_FLAGS, false);
			if (!offline) {
				bot.user_get(TEST_USER_ID, [](const dpp::confirmation_callback_t &event) {
					if (!event.is_error()) {
						auto u = std::get<dpp::user_identified>(event.value);
						if (u.id == TEST_USER_ID) {
							set_test(USER_GET, true);
						} else {
							set_test(USER_GET, false);
						}
						json j = json::parse(event.http_info.body);
						uint64_t raw_flags = j["public_flags"];
						if (j.contains("flags")) {
							uint64_t flags = j["flags"];
							raw_flags |= flags;
						}
						// testing all user flags from https://discord.com/developers/docs/resources/user#user-object-user-flags
						// they're manually set here because the dpp::user_flags don't match to the discord API, so we can't use them to compare with the raw flags!
						if (
								u.is_discord_employee() == 			((raw_flags & (1 << 0)) != 0) &&
								u.is_partnered_owner() == 			((raw_flags & (1 << 1)) != 0) &&
								u.has_hypesquad_events() == 		((raw_flags & (1 << 2)) != 0) &&
								u.is_bughunter_1() == 				((raw_flags & (1 << 3)) != 0) &&
								u.is_house_bravery() == 			((raw_flags & (1 << 6)) != 0) &&
								u.is_house_brilliance() == 			((raw_flags & (1 << 7)) != 0) &&
								u.is_house_balance() == 			((raw_flags & (1 << 8)) != 0) &&
								u.is_early_supporter() == 			((raw_flags & (1 << 9)) != 0) &&
								u.is_team_user() == 				((raw_flags & (1 << 10)) != 0) &&
								u.is_bughunter_2() == 				((raw_flags & (1 << 14)) != 0) &&
								u.is_verified_bot() == 				((raw_flags & (1 << 16)) != 0) &&
								u.is_verified_bot_dev() == 			((raw_flags & (1 << 17)) != 0) &&
								u.is_certified_moderator() == 		((raw_flags & (1 << 18)) != 0) &&
								u.is_bot_http_interactions() == 	((raw_flags & (1 << 19)) != 0) &&
								u.is_active_developer() == 			((raw_flags & (1 << 22)) != 0)
								) {
							set_test(USER_GET_FLAGS, true);
						} else {
							set_test(USER_GET_FLAGS, false);
						}
					} else {
						set_test(USER_GET, false);
						set_test(USER_GET_FLAGS, false);
					}
				});
			}

			set_test(VOICE_CHANNEL_CREATE, false);
			set_test(VOICE_CHANNEL_EDIT, false);
			set_test(VOICE_CHANNEL_DELETE, false);
			if (!offline) {
				dpp::channel channel1;
				channel1.set_type(dpp::CHANNEL_VOICE)
					.set_guild_id(TEST_GUILD_ID)
					.set_name("voice1")
					.add_permission_overwrite(TEST_GUILD_ID, dpp::ot_role, 0, dpp::p_view_channel)
					.set_user_limit(99);
				dpp::channel createdChannel;
				try {
					createdChannel = bot.channel_create_sync(channel1);
				} catch (dpp::rest_exception &exception) {
					set_test(VOICE_CHANNEL_CREATE, false);
				}
				if (createdChannel.name == channel1.name &&
						createdChannel.user_limit == 99 &&
						createdChannel.name == "voice1") {
					for (auto overwrite: createdChannel.permission_overwrites) {
						if (overwrite.id == TEST_GUILD_ID && overwrite.type == dpp::ot_role && overwrite.deny == dpp::p_view_channel) {
							set_test(VOICE_CHANNEL_CREATE, true);
						}
					}

					// edit the voice channel
					createdChannel.set_name("foobar2");
					createdChannel.set_user_limit(2);
					for (auto overwrite: createdChannel.permission_overwrites) {
						if (overwrite.id == TEST_GUILD_ID) {
							overwrite.deny.set(0);
							overwrite.allow.set(dpp::p_view_channel);
						}
					}
					try {
						dpp::channel edited = bot.channel_edit_sync(createdChannel);
						if (edited.name == "foobar2" && edited.user_limit == 2) {
							set_test(VOICE_CHANNEL_EDIT, true);
						}
					} catch (dpp::rest_exception &exception) {
						set_test(VOICE_CHANNEL_EDIT, false);
					}

					// delete the voice channel
					try {
						bot.channel_delete_sync(createdChannel.id);
						set_test(VOICE_CHANNEL_DELETE, true);
					} catch (dpp::rest_exception &exception) {
						set_test(VOICE_CHANNEL_DELETE, false);
					}
				}
			}

			set_test(FORUM_CREATION, false);
			set_test(FORUM_CHANNEL_GET, false);
			set_test(FORUM_CHANNEL_DELETE, false);
			if (!offline) {
				dpp::channel c;
				c.name = "test-forum-channel";
				c.guild_id = TEST_GUILD_ID;
				c.set_topic("This is a forum channel");
				c.set_flags(dpp::CHANNEL_FORUM);
				c.default_sort_order = dpp::so_creation_date;
				dpp::forum_tag t;
				t.name = "Alpha";
				t.emoji = "❌";
				c.available_tags = {t};
				c.default_auto_archive_duration = dpp::arc_1_day;
				c.default_reaction = "✅";
				c.default_thread_rate_limit_per_user = 10;
				bot.channel_create(c, [&bot](const dpp::confirmation_callback_t &event) {
					if (!event.is_error()) {
						set_test(FORUM_CREATION, true);
						auto channel = std::get<dpp::channel>(event.value);
						// retrieve the forum channel and check the values
						bot.channel_get(channel.id, [forum_id = channel.id, &bot](const dpp::confirmation_callback_t &event) {
							if (!event.is_error()) {
								auto channel = std::get<dpp::channel>(event.value);
								bot.log(dpp::ll_debug, event.http_info.body);
								bool tag = false;
								for (auto &t : channel.available_tags) {
									if (t.name == "Alpha" && std::holds_alternative<std::string>(t.emoji) && std::get<std::string>(t.emoji) == "❌") {
										tag = true;
									}
								}
								bool name = channel.name == "test-forum-channel";
								bool sort = channel.default_sort_order == dpp::so_creation_date;
								bool rateLimit = channel.default_thread_rate_limit_per_user == 10;
								set_test(FORUM_CHANNEL_GET, tag && name && sort && rateLimit);
							} else {
								set_test(FORUM_CHANNEL_GET, false);
							}
							// delete the forum channel
							bot.channel_delete(forum_id, [](const dpp::confirmation_callback_t &event) {
								if (!event.is_error()) {
									set_test(FORUM_CHANNEL_DELETE, true);
								} else {
									set_test(FORUM_CHANNEL_DELETE, false);
								}
							});
						});
					} else {
						set_test(FORUM_CREATION, false);
						set_test(FORUM_CHANNEL_GET, false);
					}
				});
			}

			set_test(THREAD_CREATE, false);
			if (!offline) {
				bot.thread_create("thread test", TEST_TEXT_CHANNEL_ID, 60, dpp::channel_type::CHANNEL_PUBLIC_THREAD, true, 60, [&](const dpp::confirmation_callback_t &event) {
					if (!event.is_error()) {
						[[maybe_unused]] const auto &thread = event.get<dpp::thread>();
						set_test(THREAD_CREATE, true);
					}
					// the thread tests are in the on_thread_create event handler
				});
			}

			start_test(POLL_CREATE);
			if (!offline) {
				dpp::message poll_msg{};

				poll_msg.set_poll(dpp::poll{}
					.set_question("hello!")
					.add_answer("one", dpp::unicode_emoji::one)
					.add_answer("two", dpp::unicode_emoji::two)
					.add_answer("three", dpp::unicode_emoji::three)
					.add_answer("four")
					.set_duration(48)
					.set_allow_multiselect(true)
				).set_channel_id(TEST_TEXT_CHANNEL_ID);

				bot.message_create(poll_msg, [&bot, poll_msg](const dpp::confirmation_callback_t& result) {
					if (result.is_error()) {
						set_status(POLL_CREATE, ts_failed, result.get_error().human_readable);
						return;
					}

					const dpp::message& m = std::get<dpp::message>(result.value);

					if (!m.attached_poll.has_value()) {
						set_status(POLL_CREATE, ts_failed, "poll missing in received message");
						return;
					}

					if (m.attached_poll->find_answer(std::numeric_limits<uint32_t>::max()) != nullptr) {
						set_status(POLL_CREATE, ts_failed, "poll::find_answer failed to return nullptr");
						return;
					}

					std::array<bool, 4> correct = {false, false, false, false};
					int i = 0;
					for (const auto& [_, answer] : m.attached_poll->answers) {
						if (m.attached_poll->find_answer(answer.id) != &answer.media) {
							set_status(POLL_CREATE, ts_failed, "poll::find_answer failed to return valid answer");
							return;
						}
						if (answer.media.text == "one" && answer.media.emoji.name == dpp::unicode_emoji::one) {
							if (correct[i]) {
								set_status(POLL_CREATE, ts_failed, "poll answer found twice");
								return;
							}
							correct[i] = true;
						}
						if (answer.media.text == "two" && answer.media.emoji.name == dpp::unicode_emoji::two) {
							if (correct[i]) {
								set_status(POLL_CREATE, ts_failed, "poll answer found twice");
								return;
							}
							correct[i] = true;
						}
						if (answer.media.text == "three" && answer.media.emoji.name == dpp::unicode_emoji::three) {
							if (correct[i]) {
								set_status(POLL_CREATE, ts_failed, "poll answer found twice");
								return;
							}
							correct[i] = true;
						}
						if (answer.media.text == "four" && answer.media.emoji.name.empty()) {
							if (correct[i]) {
								set_status(POLL_CREATE, ts_failed, "poll answer found twice");
								return;
							}
							correct[i] = true;
							bot.poll_get_answer_voters(m, answer.id, 0, 100, [m, &bot](const dpp::confirmation_callback_t& result) {
								if (result.is_error()) {
									set_status(POLL_CREATE, ts_failed, "poll_get_answer_voters: " + result.get_error().human_readable);
									return;
								}

								start_test(POLL_END);
								bot.poll_end(m, [message_id = m.id, channel_id = m.channel_id, &bot](const dpp::confirmation_callback_t& result) {
									if (result.is_error()) {
										set_status(POLL_END, ts_failed, result.get_error().human_readable);
										return;
									}
									set_status(POLL_END, ts_success);
									bot.message_delete(message_id, channel_id);
								});
							});
						}
						++i;
					}
					if (correct == std::array<bool, 4>{true, true, true, true}) {
						set_status(POLL_CREATE, ts_success);
					} else {
						set_status(POLL_CREATE, ts_failed, "failed to find the submitted answers");
					}
				});
			}

			set_test(MEMBER_GET, false);
			if (!offline) {
				bot.guild_get_member(TEST_GUILD_ID, TEST_USER_ID, [](const dpp::confirmation_callback_t &event){
					if (!event.is_error()) {
						dpp::guild_member m = std::get<dpp::guild_member>(event.value);
						if (m.guild_id == TEST_GUILD_ID && m.user_id == TEST_USER_ID) {
							set_test(MEMBER_GET, true);
						} else {
							set_test(MEMBER_GET, false);
						}
					} else {
						set_test(MEMBER_GET, false);
					}
				});
			}

			set_test(ROLE_CREATE, false);
			set_test(ROLE_EDIT, false);
			set_test(ROLE_DELETE, false);
			if (!offline) {
				dpp::role r;
				r.guild_id = TEST_GUILD_ID;
				r.name = "Test-Role";
				r.permissions.add(dpp::p_move_members);
				r.set_flags(dpp::r_mentionable);
				r.colour = dpp::colors::moon_yellow;
				dpp::role createdRole;
				try {
					createdRole = bot.role_create_sync(r);
					if (createdRole.name == r.name &&
						createdRole.has_move_members() &&
						createdRole.flags & dpp::r_mentionable &&
						createdRole.colour == r.colour) {
						set_test(ROLE_CREATE, true);
					}
				} catch (dpp::rest_exception &exception) {
					set_test(ROLE_CREATE, false);
				}
				createdRole.guild_id = TEST_GUILD_ID;
				createdRole.name = "Test-Role-Edited";
				createdRole.colour = dpp::colors::light_sea_green;
				try {
					dpp::role edited = bot.role_edit_sync(createdRole);
					if (createdRole.id == edited.id && edited.name == "Test-Role-Edited") {
						set_test(ROLE_EDIT, true);
					}
				} catch (dpp::rest_exception &exception) {
					set_test(ROLE_EDIT, false);
				}
				try {
					bot.role_delete_sync(TEST_GUILD_ID, createdRole.id);
					set_test(ROLE_DELETE, true);
				} catch (dpp::rest_exception &exception) {
					set_test(ROLE_DELETE, false);
				}
			}
		};

		set_test(BOTSTART, false);
		try {
			if (!offline) {
				bot.start(true);
				set_test(BOTSTART, true);
			}
		}
		catch (const std::exception &) {
			set_test(BOTSTART, false);
		}

		set_test(TIMERSTART, false);
		uint32_t ticks = 0;
		dpp::timer th = bot.start_timer([&](dpp::timer timer_handle) {
			if (ticks == 5) {
				/* The simple test timer ticks every second.
				 * If we get to 5 seconds, we know the timer is working.
				 */
				set_test(TIMERSTART, true);
			}
			ticks++;
		}, 1);

		set_test(USER_GET_CACHED_PRESENT, false);
		try {
			dpp::user_identified u = bot.user_get_cached_sync(TEST_USER_ID);
			set_test(USER_GET_CACHED_PRESENT, (u.id == TEST_USER_ID));
		}
		catch (const std::exception&) {
			set_test(USER_GET_CACHED_PRESENT, false);
		}

		set_test(USER_GET_CACHED_ABSENT, false);
		try {
			/* This is the snowflake ID of a discord staff member.
			 * We assume here that staffer's discord IDs will remain constant
			 * for long periods of time and they won't lurk in the unit test server.
			 * If this becomes not true any more, we'll pick another well known
			 * user ID.
			 */
			dpp::user_identified u = bot.user_get_cached_sync(90339695967350784);
			set_test(USER_GET_CACHED_ABSENT, (u.id == dpp::snowflake(90339695967350784)));
		}
		catch (const std::exception&) {
			set_test(USER_GET_CACHED_ABSENT, false);
		}

		set_test(TIMEDLISTENER, false);
		dpp::timed_listener tl(&bot, 10, bot.on_log, [&](const dpp::log_t & event) {
			set_test(TIMEDLISTENER, true);
		});

		set_test(ONESHOT, false);
		bool once = false;
		dpp::oneshot_timer ost(&bot, 5, [&](dpp::timer timer_handle) {
			if (!once) {
				set_test(ONESHOT, true);
			} else {
				set_test(ONESHOT, false);
			}
			once = true;
		});

		set_test(CUSTOMCACHE, false);
		dpp::cache<test_cached_object_t> testcache;
		test_cached_object_t* tco = new test_cached_object_t(666);
		tco->foo = "bar";
		testcache.store(tco);
		test_cached_object_t* found_tco = testcache.find(666);
		if (found_tco && found_tco->id == dpp::snowflake(666) && found_tco->foo == "bar") {
			set_test(CUSTOMCACHE, true);
		} else {
			set_test(CUSTOMCACHE, false);
		}
		testcache.remove(found_tco);

		if (!offline) {
			if (std::future_status status = ready_future.wait_for(std::chrono::seconds(20)); status != std::future_status::timeout) {
				do_online_tests();
			}
		}

		noparam_api_test(current_user_get, dpp::user_identified, CURRENTUSER);
		singleparam_api_test(channel_get, TEST_TEXT_CHANNEL_ID, dpp::channel, GETCHAN);
		singleparam_api_test(guild_get, TEST_GUILD_ID, dpp::guild, GETGUILD);
		singleparam_api_test_list(roles_get, TEST_GUILD_ID, dpp::role_map, GETROLES);
		singleparam_api_test_list(channels_get, TEST_GUILD_ID, dpp::channel_map, GETCHANS);
		singleparam_api_test_list(guild_get_invites, TEST_GUILD_ID, dpp::invite_map, GETINVS);
		multiparam_api_test_list(guild_get_bans, TEST_GUILD_ID, dpp::ban_map, GETBANS);
		singleparam_api_test_list(channel_pins_get, TEST_TEXT_CHANNEL_ID, dpp::message_map, GETPINS);
		singleparam_api_test_list(guild_events_get, TEST_GUILD_ID, dpp::scheduled_event_map, GETEVENTS);
		twoparam_api_test(guild_event_get, TEST_GUILD_ID, TEST_EVENT_ID, dpp::scheduled_event, GETEVENT);
		twoparam_api_test_list(guild_event_users_get, TEST_GUILD_ID, TEST_EVENT_ID, dpp::event_member_map, GETEVENTUSERS);

		std::this_thread::sleep_for(std::chrono::seconds(20));

		/* Test stopping timer */
		set_test(TIMERSTOP, false);
		set_test(TIMERSTOP, bot.stop_timer(th));

		set_test(USERCACHE, false);
		if (!offline) {
			dpp::user* u = dpp::find_user(TEST_USER_ID);
			set_test(USERCACHE, u);
		}
		set_test(CHANNELCACHE, false);
		set_test(CHANNELTYPES, false);
		if (!offline) {
			dpp::channel* c = dpp::find_channel(TEST_TEXT_CHANNEL_ID);
			dpp::channel* c2 = dpp::find_channel(TEST_VC_ID);
			set_test(CHANNELCACHE, c && c2);
			set_test(CHANNELTYPES, c && c->is_text_channel() && !c->is_voice_channel() && c2 && c2->is_voice_channel() && !c2->is_text_channel());
		}

		wait_for_tests();

	}
	catch (const std::exception &e) {
		std::cout << e.what() << "\n";
		set_test(CLUSTER, false);
	}

	/* Return value = number of failed tests, exit code 0 = success */
	return test_summary();
}
