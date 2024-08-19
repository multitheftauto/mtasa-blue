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
#include <dpp/json.h>

double start = dpp::utility::time_f();
bool offline = false;
bool extended = false;

dpp::snowflake TEST_GUILD_ID = std::stoull(SAFE_GETENV("TEST_GUILD_ID"));
dpp::snowflake TEST_TEXT_CHANNEL_ID = std::stoull(SAFE_GETENV("TEST_TEXT_CHANNEL_ID"));
dpp::snowflake TEST_VC_ID = std::stoull(SAFE_GETENV("TEST_VC_ID"));
dpp::snowflake TEST_USER_ID = std::stoull(SAFE_GETENV("TEST_USER_ID"));
dpp::snowflake TEST_EVENT_ID = std::stoull(SAFE_GETENV("TEST_EVENT_ID"));

test_t::test_t(std::string_view testname, std::string_view testdesc, int testflags) : name{testname}, description{testdesc}, flags{static_cast<test_flags_t>(testflags)} {
	tests.push_back(this);
}

void set_status(test_t &test, test_status_t newstatus, std::string_view message) {
	static std::mutex m;
	std::scoped_lock lock{m};

	if (is_skipped(test) || newstatus == test.status) {
		return;
	}
	if (test.status != ts_failed) { // disallow changing the state of a failed test, but we still log
		test.status = newstatus;
	}
	switch (newstatus) {
		case ts_started:
			std::cout << "[" << std::fixed << std::setprecision(3) << get_time() << "]: " << "[\u001b[33mTESTING\u001b[0m] " << test.description << (message.empty() ? "" : " - " + std::string{message} ) << "\n";
			break;

		case ts_failed:
			std::cout << "[" << std::fixed << std::setprecision(3) << get_time() << "]: " << "[\u001b[31mFAILED\u001b[0m] " << test.description << (message.empty() ? "" : " - " + std::string{message} ) << "\n";
			break;

		case ts_success:
			std::cout << "[" << std::fixed << std::setprecision(3) << get_time() << "]: " << "[\u001b[32mSUCCESS\u001b[0m] " << test.description << (message.empty() ? "" : " - " + std::string{message} ) << "\n";
			break;

		case ts_skipped:
			std::cout << "[" << std::fixed << std::setprecision(3)  << get_time() << "]: " << "[\u001b[33mSKIPPED\u001b[0m] " << test.description << (message.empty() ? "" : " - " + std::string{message} ) << "\n";
			break;

		default:
			break;
	}
}

void start_test(test_t &test) {
	set_status(test, ts_started);
}

void skip_test(test_t &test) {
	set_status(test, ts_skipped);
}

void set_test(test_t &test, bool success) {
	if (test.status == ts_not_executed) {
		set_status(test, success ? ts_success : ts_started);
	} else {
		set_status(test, success ? ts_success : ts_failed);
	}
}

bool is_skipped(const test_t &test) {
	return (test.flags & tf_online && offline) || (test.flags & tf_extended && !extended) || (test.flags & tf_coro && !coro);
}

double get_start_time() {
	return start;
}

double get_time() {
	return dpp::utility::time_f() - get_start_time();
}

int test_summary() {
	/* Report on all test cases */
	int failed = 0, passed = 0, skipped = 0;
	std::cout << "\u001b[37;1m\n\nUNIT TEST SUMMARY\n==================\n\u001b[0m";
	for (auto & t : tests) {
		std::cout << std::left << std::setw(60) << t->description << " " << std::fixed << std::setw(6);
		if (t->status == ts_skipped || (t->flags & tf_online && offline) || (t->flags & tf_extended && !extended) || (t->flags & tf_coro && !coro)) {
			skipped++;
			std::cout << "\u001b[33mSKIPPED";
		} else {
			if (t->status == ts_success) {
				passed++;
				std::cout << "\u001b[32mPASS";
			} else {
				failed++;
				std::cout << (t->status == ts_not_executed ? "\u001b[31mNOT EXECUTED" : "\u001b[31mFAIL");
			}
		}
		std::cout << std::setw(0) << "\u001b[0m\n";
	}
	std::cout << "\u001b[37;1m\nExecution finished in " << std::fixed << std::setprecision(3) <<  get_time() << std::setprecision(0) << " seconds.\nFailed: " << failed << " Passed: " << passed << (skipped ? " Skipped: " : "") << (skipped ? std::to_string(skipped) : "") << " Percentage: " << std::setprecision(2) << ((float)(passed) / (float)(passed + failed) * 100.0f) << "%\u001b[0m\n";
	return failed;
}

namespace {
	std::string get_testdata_dir() {
		char *env_var = getenv("TEST_DATA_DIR");

		return (env_var ? env_var : "../../testdata/");
	}
} // namespace

std::vector<uint8_t> load_test_audio() {
	std::vector<uint8_t> testaudio;
	std::string dir = get_testdata_dir();
	std::ifstream input (dir + "Robot.pcm", std::ios::in|std::ios::binary|std::ios::ate);
	if (input.is_open()) {
		size_t testaudio_size = input.tellg();
		testaudio.resize(testaudio_size);
		input.seekg(0, std::ios::beg);
		input.read((char*)testaudio.data(), testaudio_size);
		input.close();
	}
	else {
		std::cout << "ERROR: Can't load " + dir + "Robot.pcm\n";
		exit(1);
	}
	return testaudio;
}

std::vector<std::byte> load_data(const std::string& file) {
	std::vector<std::byte> testimage;
	std::string dir = get_testdata_dir();
	std::ifstream input (dir + file, std::ios::in|std::ios::binary|std::ios::ate);
	if (input.is_open()) {
		size_t testimage_size = input.tellg();
		testimage.resize(testimage_size);
		input.seekg(0, std::ios::beg);
		input.read((char*)testimage.data(), testimage_size);
		input.close();
	}
	else {
		std::cout << "ERROR: Can't load " + dir + file + "\n";
		exit(1);
	}
	return testimage;
}

std::string get_token() {
	char* t = getenv("DPP_UNIT_TEST_TOKEN");
	std::string tok;
	if (!t) {
		offline = true;
	}  else {
		tok = std::string(t);
		if (tok.empty()) {
			offline = true;
		}
	}
	return tok;
}

void wait_for_tests() {
	uint16_t ticks = 0;
	while (ticks < TEST_TIMEOUT) {
		size_t finished = 0;
		for (auto t : tests) {
			if (t->status != ts_started) {
				finished++;
			} else if (is_skipped(*t)) {
				finished++;
				t->status = ts_skipped;
				std::cout << "[" << std::fixed << std::setprecision(3)  << get_time() << "]: " << "[\u001b[33mSKIPPED\u001b[0m] " << t->description << "\n";
			}
		}
		if (finished == tests.size()) {
			std::this_thread::sleep_for(std::chrono::seconds(10));
			return;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
		ticks++;
	}
	for (auto t : tests) {
		if (t->status == ts_started) {
			std::cout << "[" << std::fixed << std::setprecision(3)  << get_time() << "]: " << "[\u001b[31mTIMEOUT\u001b[0m] " << t->description << "\n";
		}
	}
}
