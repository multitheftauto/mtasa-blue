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
#include <dpp/export.h>
#ifdef _WIN32
/* Central point for forcing inclusion of winsock library for all socket code */
#include <io.h>
#pragma comment(lib,"ws2_32")
#endif
#include <dpp/queues.h>
#include <dpp/cluster.h>
#include <dpp/httpsclient.h>
#include <dpp/stringops.h>

namespace dpp {

http_request::http_request(const std::string &_endpoint, const std::string &_parameters, http_completion_event completion, const std::string &_postdata, http_method _method, const std::string &audit_reason, const std::string &filename, const std::string &filecontent, const std::string &filemimetype, const std::string &http_protocol)
 : complete_handler(completion), completed(false), non_discord(false), endpoint(_endpoint), parameters(_parameters), postdata(_postdata),  method(_method), reason(audit_reason), mimetype("application/json"), waiting(false), protocol(http_protocol)
{
	if (!filename.empty()) {
		file_name.push_back(filename);
	}
	if (!filecontent.empty()) {
		file_content.push_back(filecontent);
	}
	if (!filemimetype.empty()) {
		file_mimetypes.push_back(filemimetype);
	}
}

http_request::http_request(const std::string &_endpoint, const std::string &_parameters, http_completion_event completion, const std::string &_postdata, http_method method, const std::string &audit_reason, const std::vector<std::string> &filename, const std::vector<std::string> &filecontent, const std::vector<std::string> &filemimetypes, const std::string &http_protocol)
 : complete_handler(completion), completed(false), non_discord(false), endpoint(_endpoint), parameters(_parameters), postdata(_postdata),  method(method), reason(audit_reason), file_name(filename), file_content(filecontent), file_mimetypes(filemimetypes), mimetype("application/json"), waiting(false), protocol(http_protocol)
{
}


http_request::http_request(const std::string &_url, http_completion_event completion, http_method _method, const std::string &_postdata, const std::string &_mimetype, const std::multimap<std::string, std::string> &_headers, const std::string &http_protocol)
 : complete_handler(completion), completed(false), non_discord(true), endpoint(_url), postdata(_postdata), method(_method), mimetype(_mimetype), req_headers(_headers), waiting(false), protocol(http_protocol)
{
}

http_request::~http_request() = default;

void http_request::complete(const http_request_completion_t &c) {
	/* Call completion handler only if the request has been completed */
	if (is_completed() && complete_handler) {
		complete_handler(c);
	}
}

/* Fill a http_request_completion_t from a HTTP result */
void populate_result(const std::string &url, cluster* owner, http_request_completion_t& rv, const https_client &res) {
	rv.status = res.get_status();
	rv.body = res.get_content();
	for (auto &v : res.get_headers()) {
		rv.headers.emplace(v.first, v.second);
	}

	/* This will be ignored for non-discord requests without rate limit headers */

	rv.ratelimit_limit = from_string<uint64_t>(res.get_header("x-ratelimit-limit"));
	rv.ratelimit_remaining = from_string<uint64_t>(res.get_header("x-ratelimit-remaining"));
	rv.ratelimit_reset_after = from_string<uint64_t>(res.get_header("x-ratelimit-reset-after"));
	rv.ratelimit_bucket = res.get_header("x-ratelimit-bucket");
	rv.ratelimit_global = (res.get_header("x-ratelimit-global") == "true");
	owner->rest_ping = rv.latency;
	if (res.get_header("x-ratelimit-retry-after") != "") {
		rv.ratelimit_retry_after = from_string<uint64_t>(res.get_header("x-ratelimit-retry-after"));
	}
	uint64_t rl_timer = rv.ratelimit_retry_after ? rv.ratelimit_retry_after : rv.ratelimit_reset_after;
	if (rv.status == 429) {
		owner->log(ll_warning, "Rate limited on endpoint " + url + ", reset after " + std::to_string(rl_timer) + "s!");
	}
	if (url != "/api/v" DISCORD_API_VERSION "/gateway/bot") {	// Squelch this particular api endpoint or it generates a warning the minute we boot a cluster
		if (rv.ratelimit_global) {
			owner->log(ll_warning, "At global rate limit on endpoint " + url + ", reset after " + std::to_string(rl_timer) + "s!");
		} else if (rv.ratelimit_remaining == 0 && rl_timer > 0) {
			owner->log(ll_debug, "Waiting for endpoint " + url + " rate limit, next request in " + std::to_string(rl_timer) + "s");
		}
	}
}

/* Returns true if the request has been made */
bool http_request::is_completed()
{
	return completed;
}

/* Execute a HTTP request */
http_request_completion_t http_request::run(cluster* owner) {

	http_request_completion_t rv;
	double start = dpp::utility::time_f();
	std::string _host = DISCORD_HOST;
	std::string _url = endpoint;

	if (non_discord) {
		std::size_t s_start = endpoint.find("://", 0);
		if (s_start != std::string::npos) {
			s_start += 3; /* "://" */
			/**
			 * NOTE: "#" is in this list, really # is client side only.
			 * This won't stop some moron from using it as part of an
			 * API endpoint...
			 */
			std::size_t s_end = endpoint.find_first_of("/?#", s_start + 1);
			if (s_end != std::string::npos) {
				_host = endpoint.substr(0, s_end);
				_url = endpoint.substr(s_end);
			} else {
				_host = endpoint;
				_url.clear();
			}
		} else {
			owner->log(ll_error, "Request to '" + endpoint + "' missing protocol scheme. This is not supported. Please specify http or https.");
		}
	}

	rv.ratelimit_limit = rv.ratelimit_remaining = rv.ratelimit_reset_after = rv.ratelimit_retry_after = 0;
	rv.status = 0;
	rv.latency = 0;
	rv.ratelimit_global = false;

	dpp::http_headers headers;
	if (non_discord) {
		/* Requests outside of Discord have their own headers an NEVER EVER send a bot token! */
		for (auto& r : req_headers) {
			headers.emplace(r.first, r.second);
		};
	} else {
		/* Always attach token and correct user agent when sending REST to Discord */
		headers.emplace("Authorization", "Bot " + owner->token);
		headers.emplace("User-Agent", http_version);
		if (!reason.empty()) {
			headers.emplace("X-Audit-Log-Reason", reason);
		}
		if (!empty(parameters)) {
			_url = endpoint + "/" +parameters;
		}
	}

	constexpr std::array request_verb {
		"GET",
		"POST",
		"PUT",
		"PATCH",
		"DELETE"
	};

	multipart_content multipart;
	if (non_discord) {
		multipart = { postdata, mimetype };
	} else {
		multipart = https_client::build_multipart(postdata, file_name, file_content, file_mimetypes);
	}
	if (!multipart.mimetype.empty()) {
		headers.emplace("Content-Type", multipart.mimetype);
	}
	http_connect_info hci = https_client::get_host_info(_host);
	try {
		https_client cli(hci.hostname, hci.port, _url, request_verb[method], multipart.body, headers, !hci.is_ssl, 5, protocol);
		rv.latency = dpp::utility::time_f() - start;
		if (cli.get_status() < 100) {
			rv.error = h_connection;
			owner->log(ll_error, "HTTP(S) error on " + hci.scheme + " connection to " + hci.hostname + ":" + std::to_string(hci.port) + ": Malformed HTTP response");
		} else {
			populate_result(_url, owner, rv, cli);
		}
	}
	catch (const std::exception& e) {
		owner->log(ll_error, "HTTP(S) error on " + hci.scheme + " connection to " + hci.hostname + ":" + std::to_string(hci.port) + ": " + std::string(e.what()));
		rv.error = h_connection;
	}

	/* Set completion flag */
	completed = true;
	return rv;
}

request_queue::request_queue(class cluster* owner, uint32_t request_threads) : creator(owner), terminating(false), globally_ratelimited(false), globally_limited_for(0), in_thread_pool_size(request_threads)
{
	for (uint32_t in_alloc = 0; in_alloc < in_thread_pool_size; ++in_alloc) {
		requests_in.push_back(new in_thread(owner, this, in_alloc));
	}
	out_thread = new std::thread(&request_queue::out_loop, this);
}

request_queue& request_queue::add_request_threads(uint32_t request_threads)
{
	for (uint32_t in_alloc_ex = 0; in_alloc_ex < request_threads; ++in_alloc_ex) {
		requests_in.push_back(new in_thread(creator, this, in_alloc_ex + in_thread_pool_size));
	}
	in_thread_pool_size += request_threads;
	return *this;
}

uint32_t request_queue::get_request_thread_count() const
{
	return in_thread_pool_size;
}

in_thread::in_thread(class cluster* owner, class request_queue* req_q, uint32_t index) : terminating(false), requests(req_q), creator(owner)
{
	this->in_thr = new std::thread(&in_thread::in_loop, this, index);
}

in_thread::~in_thread()
{
	terminating = true;
	in_ready.notify_one();
	in_thr->join();
	delete in_thr;
}

request_queue::~request_queue()
{
	terminating = true;
	out_ready.notify_one();
	out_thread->join();
	delete out_thread;
}

namespace
{

/**
 * @brief Comparator for sorting a request container
 */
struct compare_request {
	/**
	 * @brief Less_than comparator for sorting
	 * @param lhs Left-hand side
	 * @param rhs Right-hand side
	 * @return Whether lhs comes before rhs in strict ordering
	 */
	bool operator()(const std::unique_ptr<http_request>& lhs, const std::unique_ptr<http_request>& rhs) const noexcept {
		return std::less{}(lhs->endpoint, rhs->endpoint);
	};

	/**
	 * @brief Less_than comparator for sorting
	 * @param lhs Left-hand side
	 * @param rhs Right-hand side
	 * @return Whether lhs comes before rhs in strict ordering
	 */
	bool operator()(const std::unique_ptr<http_request>& lhs, std::string_view rhs) const noexcept {
		return std::less{}(lhs->endpoint, rhs);
	};

	/**
	 * @brief Less_than comparator for sorting
	 * @param lhs Left-hand side
	 * @param rhs Right-hand side
	 * @return Whether lhs comes before rhs in strict ordering
	 */
	bool operator()(std::string_view lhs, const std::unique_ptr<http_request>& rhs) const noexcept {
		return std::less{}(lhs, rhs->endpoint);
	};
};

}

void in_thread::in_loop(uint32_t index)
{
	utility::set_thread_name(std::string("http_req/") + std::to_string(index));
	while (!terminating) {
		std::mutex mtx;
		std::unique_lock<std::mutex> lock{ mtx };
		in_ready.wait_for(lock, std::chrono::seconds(1));
		/* New request to be sent! */

		if (!requests->globally_ratelimited) {

			std::vector<http_request*> requests_view;
			{
				/* Gather all the requests first within a mutex */
				std::shared_lock lock(in_mutex);
				if (requests_in.empty()) {
					/* Nothing to copy, wait again */
					continue;
				}
				requests_view.reserve(requests_in.size());
				std::transform(requests_in.begin(), requests_in.end(), std::back_inserter(requests_view), [](const std::unique_ptr<http_request> &r) {
					return r.get();
				});
			}

			for (auto& request_view : requests_view) {
				const std::string &key = request_view->endpoint;
				http_request_completion_t rv;
				auto                      currbucket = buckets.find(key);

				if (currbucket != buckets.end()) {
					/* There's a bucket for this request. Check its status. If the bucket says to wait,
					* skip all requests in this bucket till its ok.
					*/
					if (currbucket->second.remaining < 1) {
						uint64_t wait = (currbucket->second.retry_after ? currbucket->second.retry_after : currbucket->second.reset_after);
						if ((uint64_t)time(nullptr) > currbucket->second.timestamp + wait) {
							/* Time has passed, we can process this bucket again. send its request. */
							rv = request_view->run(creator);
						} else {
							if (!request_view->waiting) {
								request_view->waiting = true;
							}
							/* Time not up yet, wait more */
							break;
						}
					} else {
						/* There's limit remaining, we can just run the request */
						rv = request_view->run(creator);
					}
				} else {
					/* No bucket for this endpoint yet. Just send it, and make one from its reply */
					rv = request_view->run(creator);
				}

				bucket_t newbucket;
				newbucket.limit = rv.ratelimit_limit;
				newbucket.remaining = rv.ratelimit_remaining;
				newbucket.reset_after = rv.ratelimit_reset_after;
				newbucket.retry_after = rv.ratelimit_retry_after;
				newbucket.timestamp = time(nullptr);
				requests->globally_ratelimited = rv.ratelimit_global;
				if (requests->globally_ratelimited) {
					requests->globally_limited_for = (newbucket.retry_after ? newbucket.retry_after : newbucket.reset_after);
				}
				buckets[request_view->endpoint] = newbucket;

				/* Remove the request from the incoming requests to transfer it to completed requests */
				std::unique_ptr<http_request> request;
				{
					/* Find the owned pointer in requests_in */
					std::scoped_lock lock1{in_mutex};

					auto [begin, end] = std::equal_range(requests_in.begin(), requests_in.end(), key, compare_request{});
					for (auto it = begin; it != end; ++it) {
						if (it->get() == request_view) {
							/* Grab and remove */
							request = std::move(*it);
							requests_in.erase(it);
							break;
						}
					}
				}
				/* Make a new entry in the completion list and notify */
				auto hrc = std::make_unique<http_request_completion_t>();
				*hrc = rv;
				{
					std::scoped_lock lock1(requests->out_mutex);
					requests->responses_out.push({std::move(request), std::move(hrc)});
				}
				requests->out_ready.notify_one();
			}

		} else {
			if (requests->globally_limited_for > 0) {
				std::this_thread::sleep_for(std::chrono::seconds(requests->globally_limited_for));
				requests->globally_limited_for = 0;
			}
			requests->globally_ratelimited = false;
			in_ready.notify_one();
		}
	}
}

bool request_queue::queued_deleting_request::operator<(const queued_deleting_request& other) const noexcept {
	return time_to_delete < other.time_to_delete;
}

bool request_queue::queued_deleting_request::operator<(time_t time) const noexcept {
	return time_to_delete < time;
}


void request_queue::out_loop()
{
	utility::set_thread_name("req_callback");
	while (!terminating) {

		std::mutex mtx;
		std::unique_lock lock{ mtx };
		out_ready.wait_for(lock, std::chrono::seconds(1));
		time_t now = time(nullptr);

		/* A request has been completed! */
		completed_request queue_head = {};
		{
			std::scoped_lock lock1(out_mutex);
			if (responses_out.size()) {
				queue_head = std::move(responses_out.front());
				responses_out.pop();
			}
		}

		if (queue_head.request && queue_head.response) {
			queue_head.request->complete(*queue_head.response);
			/* Queue deletions for 60 seconds from now */
			auto when = now + 60;
			auto where = std::lower_bound(responses_to_delete.begin(), responses_to_delete.end(), when);
			responses_to_delete.insert(where, {when, std::move(queue_head)});
		}

		/* Check for deletable items every second regardless of select status */
		auto end = std::lower_bound(responses_to_delete.begin(), responses_to_delete.end(), now);
		if (end != responses_to_delete.begin()) {
			responses_to_delete.erase(responses_to_delete.begin(), end);
		}
	}
}

/* Post a http_request into the queue */
void in_thread::post_request(std::unique_ptr<http_request> req)
{
	{
		std::scoped_lock lock(in_mutex);

		auto where = std::lower_bound(requests_in.begin(), requests_in.end(), req->endpoint, compare_request{});
		requests_in.emplace(where, std::move(req));
	}
	in_ready.notify_one();
}

/* Simple hash function for hashing urls into thread pool values,
 * ensuring that the same url always ends up on the same thread,
 * which means that it will be part of the same ratelimit bucket.
 * I did consider std::hash for this, but std::hash returned even
 * numbers for absolutely every string i passed it on g++ 10.0,
 * so this was a no-no. There are also much bigger more complex
 * hash functions that claim to be really fast, but this is
 * readable and small and fits the requirement exactly.
 */
inline uint32_t hash(const char *s)
{
	uint32_t hashval;
	for (hashval = 17; *s != 0; s++) {
		hashval = *s + 31 * hashval;
	}
	return hashval;
}

/* Post a http_request into a request queue */
request_queue& request_queue::post_request(std::unique_ptr<http_request> req)
{
	requests_in[hash(req->endpoint.c_str()) % in_thread_pool_size]->post_request(std::move(req));
	return *this;
}

bool request_queue::is_globally_ratelimited() const
{
	return this->globally_ratelimited;
}

} // namespace dpp
