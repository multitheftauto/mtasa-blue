#include <thread>
#include <atomic>
#include <algorithm>

#include "test.h"

#ifdef DPP_CORO

namespace {

namespace std_coroutine = dpp::detail::std_coroutine;

template <size_t N>
struct test_exception : std::exception {
};

std::atomic<int> exceptions_caught = 0;

std::array<std::atomic<int>, 10> job_data;
std::array<std::atomic<int>, 10> task_data;


struct simple_awaitable {
	test_t &test_for;
	volatile int value{};
	int result{};

	bool await_ready() const noexcept {
		return false;
	}

	template <typename T>
	void await_suspend(std_coroutine::coroutine_handle<T> handle) {
		std::thread th([this, handle]() mutable {
			auto *test = &test_for;
			std::this_thread::sleep_for(std::chrono::seconds(5));
			result = value;
			try {
				handle.resume();
			} catch (const std::exception &) {
				/* no exception should be caught */
				set_status(*test, ts_failed, "unexpected exception during resume");
			}
		});
		th.detach();
	}

	int await_resume() const noexcept {
		return result;
	}
};

void sync_awaitable_fun(std::function<void(int)> callback) {
	callback(42);
}

void sync_awaitable_throw(std::function<void(int)> callback) {
	throw test_exception<0>{};
}

void async_awaitable_wait5(std::function<void(int)> callback) {
	std::thread th([cc = std::move(callback)]() noexcept {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		cc(69);
	});
	th.detach();
}

struct job_awaitable {
	volatile int value{};
	int result{};

	bool await_ready() const noexcept {
		return false;
	}

	template <typename T>
	void await_suspend(std_coroutine::coroutine_handle<T> handle) {
		std::thread th([this, handle]() mutable {
			std::this_thread::sleep_for(std::chrono::seconds(5));
			result = value;
			try {
				handle.resume();
			} catch (const test_exception<42> &) {
				/* intended exception to catch. we should have 2, one for the job_data test, one for the top-level job test */
				int exceptions = ++exceptions_caught;
				if (exceptions == 2) {
					set_status(CORO_JOB_OFFLINE, ts_success);
				} else if (exceptions > 2) {
					set_status(CORO_JOB_OFFLINE, ts_failed, "resume() threw more than expected");
				}
			} catch (const std::exception &) {
				/* anything else should not be caught */
				set_status(CORO_JOB_OFFLINE, ts_failed, "resume() threw an exception it shouldn't have");
			}
		});
		th.detach();
	}

	int await_resume() const noexcept {
		return result;
	}
};

dpp::job job_offline_test() {
	if (int ret = co_await job_awaitable{42}; ret != 42) {
		set_status(CORO_JOB_OFFLINE, ts_failed, "failed simple awaitable");
	}
	std::array<dpp::job, 10> jobs;
	for (int i = 0; i < 10; ++i) {
		jobs[i] = [](int i) -> dpp::job {
			static std::atomic<bool> passed = false;
			if (int ret = co_await job_awaitable{i}; ret != i) {
				set_status(CORO_JOB_OFFLINE, ts_failed, "failed in-loop awaitable");
			}
			job_data[i] = i;
			for (int j = 0; j < 10; ++j) {
				if (job_data[j] != j) {
					co_return;
				}
			}
			if (passed.exchange(true) == true) { // another thread came here and already passed this check, skip the exception
				co_return;
			}
			throw test_exception<42>(); // should be caught by simple_awaitable (since this is after resume)
		}(i);
	}
	if (std::find_if(job_data.begin(), job_data.end(), [](int i){ return i > 0; }) != job_data.end()) {
		set_status(CORO_JOB_OFFLINE, ts_failed, "job should not have modified the data at this point");
	}

	// verify that exceptions work as expected (dpp::job throws uncaught exceptions immediately to the caller / resumer)
	try {
		[]() -> dpp::job {
			throw test_exception<1>{};
		}();
	} catch (const test_exception<1> &) {
		throw test_exception<42>{}; // should be caught by simple_awaitable (since this is after resume)
	}
}

dpp::task<void> task_offline_test() {
	namespace time = std::chrono;
	using clock = time::system_clock;
	static auto &test = CORO_TASK_OFFLINE;

	if (int ret = co_await simple_awaitable{test, 42}; ret != 42) {
		set_status(test, ts_failed, "failed simple awaitable");
	}
	std::array<dpp::task<int>, 10> tasks;
	auto start = clock::now();
	for (int i = 0; i < 10; ++i) {
		tasks[i] = [](int i) -> dpp::task<int> {
			if (int ret = co_await simple_awaitable{test, i}; ret != i) {
				set_status(test, ts_failed, "failed in-loop awaitable");
			}
			job_data[i] = i;
			co_return i;
		}(i);
	}
	for (int i = 0; i < 10; ++i) {
		if (co_await tasks[i] != i) {
			set_status(test, ts_failed, "failed to co_await expected value");
		}
	}
	auto diff = clock::now() - start;
	if (diff > time::seconds(10)) { // task is async so 10 parallel tasks should all take 5 seconds + some overhead
		set_status(test, ts_failed, "parallel tasks took longer than expected");
	}

	// verify that exceptions work as expected (dpp::task throws uncaught exceptions to co_await-er)
	dpp::task task1;
	dpp::task task2;
	try {
		task1 = []() -> dpp::task<void> { // throws after co_await
			if (int ret = co_await simple_awaitable{test, 69}; ret != 69) {
				set_status(test, ts_failed, "nested failed simple awaitable");
			}
			dpp::task task2 = []() -> dpp::task<void> {
				co_await std_coroutine::suspend_never{}; // needed to make sure the task is initialized properly
				throw test_exception<1>{};
			}();
			co_await task2;
		}();
		task2 = []() -> dpp::task<void> { // throws immediately
			co_await std_coroutine::suspend_never{}; // needed to make sure the task is initialized properly
			throw test_exception<2>{};
		}();
	} catch (const std::exception &e) {
		/* SHOULD NOT throw. exceptions are sent when resuming from co_await */
		set_status(test, ts_failed, "task threw in constructor");
	}

	bool success = false;
	// test throw after co_await
	try {
		co_await task1;
	} catch (const test_exception<1> &) {
		success = true;
	}
	if (!success) {
		set_status(CORO_TASK_OFFLINE, ts_failed, "task failed to throw expected test_exception<1>");
	}
	success = false;
	// test throw without suspending
	try {
		co_await task2;
	}
	catch (const test_exception<2> &) {
		success = true;
	}
	if (!success) {
		set_status(CORO_TASK_OFFLINE, ts_failed, "task failed to throw expected test_exception<2>");
	}
	// test cancel (throws dpp::task_cancelled_exception)
	auto task3 = []() -> dpp::task<void> {
		auto make_task = []() -> dpp::task<void> {
			for (int i = 0; i < 5; ++i) {
				co_await dpp::async<int>(&async_awaitable_wait5);
			}
			set_status(CORO_TASK_OFFLINE, ts_failed, "failed to cancel nested task");
		};
		std::array<dpp::task<void>, 3> tasks;

		// test cancel and catch {
		auto start_time = clock::now();
		int exceptions = 0;
		std::generate(tasks.begin(), tasks.end(), make_task);
		for (auto &task : tasks) {
			task.cancel();
		}
		for (auto &task : tasks) {
			try {
				co_await task;
			}
			catch (const dpp::task_cancelled_exception &) {
				++exceptions;
			}
		}
		if (exceptions < 3 || !(clock::now() - start_time < std::chrono::seconds(10))) {
			set_status(CORO_TASK_OFFLINE, ts_failed, "failed to cancel 3 nested tasks in time");
		}

		// test cancel and propagate {
		std::generate(tasks.begin(), tasks.end(), make_task);
		tasks[0].cancel();
		for (auto &task : tasks) {
			co_await task; // tasks[0] should throw
		}
		// unreachable normally
		set_status(CORO_TASK_OFFLINE, ts_failed, "failed to exit the scope after expected dpp::task_cancelled_exception");
		// }
	}();
	try {
		co_await task3;
	}
	catch (const dpp::task_cancelled_exception &) {
		success = true;
	}
	if (!success) {
		set_status(CORO_TASK_OFFLINE, ts_failed, "failed to propagate dpp::task_cancelled_exception to caller");
	}
	throw test_exception<0>{};
}

dpp::coroutine<void> coroutine_offline_test() {
	static auto &test = CORO_COROUTINE_OFFLINE;
	std::array<int, 3> data;
	int num = 0;

	auto factory = [&data](int &i) -> dpp::coroutine<int> {
		if (int ret = co_await simple_awaitable{test, 42}; ret != 42) {
			set_status(test, ts_failed, "failed simple awaitable");
		}
		data[i] = i;
		co_return i++;
	};
	if (int ret = co_await factory(num); ret != 0) {
		set_status(test, ts_failed, "coroutine 1 to set expected values");
	}
	if (int ret = co_await factory(num); ret != 1) {
		set_status(test, ts_failed, "coroutine 2 to set expected values");
	}
	if (int ret = co_await factory(num); ret != 2) {
		set_status(test, ts_failed, "coroutine 3 to set expected values");
	}
	if (data != std::to_array<int>({0, 1, 2})) {
		set_status(test, ts_failed, "unexpected test data");
	}

	// verify that exceptions work as expected (dpp::coroutine throws uncaught exceptions to co_await-er)
	co_await []() -> dpp::coroutine<void> {
		dpp::coroutine<void> nested1;
		dpp::coroutine<void> nested2;
		try {
			nested1 = []() -> dpp::coroutine<void> {
				int n = rand();
				if (int ret = co_await simple_awaitable{test, n}; ret != n) {
					set_status(test, ts_failed, "nested failed simple awaitable");
				}
				throw test_exception<1>{};
			}();
			nested2 = []() -> dpp::coroutine<void> {
				co_await std_coroutine::suspend_never{};
				throw test_exception<2>{};
			}();
		} catch (const std::exception &e) {
			/* SHOULD NOT throw. exceptions are sent when resuming from co_await */
			set_status(test, ts_failed, "threw before co_await");
		}
		bool success1 = false;
		bool success2 = false;
		try {
			co_await nested1;
		} catch (const test_exception<1> &) {
			success1 = true;
		}
		try {
			co_await nested2;
		} catch (const test_exception<2> &) {
			success2 = true;
		}
		if (success1 && success2) {
			throw test_exception<0>{};
		} else {
			set_status(test, ts_failed, "failed to throw at co_await");
		}
	}(); // test_exception<0> escapes
}

dpp::job async_test() {
	namespace time = std::chrono;
	using clock = time::system_clock;
	test_t &test = CORO_ASYNC_OFFLINE;
	try {
		std::array<dpp::async<int>, 10> arr;

		std::generate(arr.begin(), arr.end(), [] { return dpp::async<int>{&sync_awaitable_fun}; });
		for (auto &async : arr) {
			if (int ret = co_await async; ret != 42) {
				set_status(test, ts_failed, "failed to sync resume with expected value");
			}
		}
		bool success = false;
		try {
			dpp::async<int> throws{&sync_awaitable_throw};
		} catch (const test_exception<0> &) {
			success = true;
		}
		if (!success) {
			set_status(test, ts_failed, "failed to propagate exception");
		}
		auto now = clock::now();
		std::generate(arr.begin(), arr.end(), [] { return dpp::async<int>{&async_awaitable_wait5}; });
		for (auto &async : arr) {
			if (int ret = co_await async; ret != 69) {
				set_status(test, ts_failed, "failed to async resume with expected value");
			}
		}
		auto diff = clock::now() - now;
		if (diff > time::seconds(10)) {
			// async executes asynchronously so we should be waiting 5 seconds + some overhead
			set_status(test, ts_failed, "parallel asyncs took more time than expected");
		}
		set_status(test, ts_success);
	} catch (const std::exception &e) {
		/* no exception should be caught here */
		set_status(test, ts_failed, "unknown exception thrown");
	}
}

}

void coro_offline_tests()
{
	start_test(CORO_JOB_OFFLINE);
	// Initialize all job data to -1
	std::fill(job_data.begin(), job_data.end(), -1);
	job_offline_test();

	start_test(CORO_TASK_OFFLINE);
	std::fill(task_data.begin(), task_data.end(), -1);
	[]() -> dpp::job {
		dpp::task task = task_offline_test();

		try {
			co_await dpp::task{std::move(task)};
		} catch (const test_exception<0> &) { // exception thrown at the end of the task test
			set_status(CORO_TASK_OFFLINE, ts_success);
		} catch (const std::exception &e) { // anything else should not escape
			set_status(CORO_TASK_OFFLINE, ts_failed, "unknown exception thrown");
		}
	}();

	start_test(CORO_COROUTINE_OFFLINE);
	[]() -> dpp::job {
		dpp::coroutine task = coroutine_offline_test();

		try {
			co_await dpp::coroutine{std::move(task)};
		} catch (const test_exception<0> &) {
			set_status(CORO_COROUTINE_OFFLINE, ts_success);
		} catch (const std::exception &e) { // anything else should not escape
			set_status(CORO_COROUTINE_OFFLINE, ts_failed, "unknown exception thrown");
		}
	}();

	start_test(CORO_ASYNC_OFFLINE);
	async_test();
}

void event_handler_test(dpp::cluster *bot) {
	bot->on_message_create([](dpp::message_create_t event) -> dpp::task<void> {
		if (event.msg.content == "coro test") {
			dpp::cluster *bot = event.from->creator;

			set_status(CORO_EVENT_HANDLER, ts_success);
			start_test(CORO_API_CALLS);
			dpp::confirmation_callback_t result = co_await bot->co_message_edit(dpp::message{event.msg}.set_content("coro 👍"));
			dpp::message *confirm = std::get_if<dpp::message>(&(result.value));
			set_status(CORO_API_CALLS, (confirm == nullptr || confirm->content != "coro 👍") ? ts_failed : ts_success);

			if (extended) {
				start_test(CORO_MUMBO_JUMBO);
				std::array<dpp::task<dpp::snowflake>, 3> tasks;
				for (int i = 0; i < 3; ++i) {
					tasks[i] = [](dpp::cluster *bot, dpp::snowflake channel, int i) -> dpp::task<dpp::snowflake> {
						using user_member = std::pair<std::optional<dpp::user>, std::optional<dpp::guild_member>>;
						constexpr auto get_member_user = [](dpp::cluster *bot) -> dpp::task<user_member> {
							std::pair<std::optional<dpp::user>, std::optional<dpp::guild_member>> ret{};
							dpp::confirmation_callback_t result;

							try {
								ret.second = dpp::find_guild_member(TEST_GUILD_ID, TEST_USER_ID);
							} catch (const dpp::cache_exception &e) {}
							if (!ret.second.has_value()) {
								result = co_await bot->co_guild_get_member(TEST_GUILD_ID, TEST_USER_ID);
								if (!result.is_error()) {
									ret.second = std::get<dpp::guild_member>(std::move(result).value);
								}
							}
							result = co_await bot->co_user_get_cached(TEST_USER_ID);
							if (!result.is_error()) {
								ret.first = std::get<dpp::user_identified>(std::move(result).value);
							}
							co_return ret;
						};
						auto get_member_task = get_member_user(bot);
						std::string content = "coro " + std::to_string(i);
						dpp::confirmation_callback_t result = co_await bot->co_message_create(dpp::message{channel, content});

						if (result.is_error()) {
							co_return {};
						}
						dpp::message msg = std::get<dpp::message>(std::move(result).value);
						if (msg.content != content) {
							co_return {};
						}
						user_member pair = co_await get_member_task;
						if (!pair.first.has_value()) {
							co_return {};
						}
						const std::string& member_nick = pair.second.has_value() ? pair.second->get_nickname() : "";
						const std::string& user_nick = pair.first->username;
						result = co_await bot->co_message_edit(msg.set_content("coro " + (member_nick.empty() ? user_nick : member_nick) + " " + std::to_string(i)));
						co_return result.is_error() ? dpp::snowflake{} : std::get<dpp::message>(result.value).id;
					}(bot, event.msg.channel_id, i);
				}
				std::array<dpp::snowflake, 3> msg_ids;
				std::array<dpp::async<dpp::confirmation_callback_t>, 3> reacts;
				for (int i = 0; i < 3; ++i) {
					try {
						msg_ids[i] = co_await tasks[i];

						if (msg_ids[i] == dpp::snowflake{}) {
							set_status(CORO_MUMBO_JUMBO, ts_failed, "failed to post message");
							reacts[i] = dpp::async{[](auto &&cc) { cc(dpp::confirmation_callback_t{}); }};
						} else {
							reacts[i] = bot->co_message_add_reaction(msg_ids[i], event.msg.channel_id, "✅");
						}
					} catch (const std::exception &e) {
						set_status(CORO_MUMBO_JUMBO, ts_failed, "message task threw " + std::string{e.what()});
						reacts[i] = dpp::async{[](auto &&cc) { cc(dpp::confirmation_callback_t{}); }};
					}
				}
				for (int i = 0; i < 3; ++i) {
					try {
						dpp::confirmation_callback_t result = co_await reacts[i];
					} catch (const std::exception &e) {
						set_status(CORO_MUMBO_JUMBO, ts_failed, "react task threw " + std::string{e.what()});
					}
				}
				for (int i = 0; i < 3; ++i) {
					bot->message_delete(msg_ids[i], event.msg.channel_id);
				}
				set_status(CORO_MUMBO_JUMBO, ts_success);
			}
		}
		co_return;
	});
	bot->message_create(dpp::message{"coro test"}.set_channel_id(TEST_TEXT_CHANNEL_ID));
}

void coro_online_tests(dpp::cluster *bot) {
	start_test(CORO_EVENT_HANDLER);
	event_handler_test(bot);
}

#else

void coro_offline_tests() {}
void coro_online_tests(dpp::cluster *bot) {}

#endif
