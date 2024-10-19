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
#include <map>
#include <dpp/exception.h>
#include <dpp/cluster.h>
#include <dpp/discordclient.h>
#include <dpp/discordevents.h>
#include <dpp/message.h>
#include <dpp/cache.h>
#include <dpp/once.h>
#include <dpp/sync.h>
#include <chrono>
#include <iostream>
#include <dpp/json.h>
#include <utility>
#include <algorithm>

namespace dpp {

/**
 * @brief An audit reason for each thread. These are per-thread to make the cluster
 * methods like cluster::get_audit_reason and cluster::set_audit_reason thread safe across
 * multiple threads. You must ensure you set the audit reason on the same thread that makes
 * the request associated with it.
 */
thread_local std::string audit_reason;

/**
 * @brief Make a warning lambda for missing message intents
 *
 * @tparam T type of parameter for the event in the router 
 * @param cl Creating cluster
 * @param required_intent Intent which is required
 * @param message Message to display
 * @return std::function<void(const T&)> Returned lambda
 */
template<typename T> std::function<void(const T&)> make_intent_warning(cluster* cl, const intents required_intent, const std::string& message) {
	return [cl, required_intent, message](const T& event) {
		if (!(cl->intents & required_intent) && event.msg.guild_id) {
			cl->log(ll_warning, message);
		}
	};
}

template <build_type BuildType>
bool validate_configuration() {
#ifdef _DEBUG
	[[maybe_unused]] constexpr build_type expected = build_type::debug;
#else
	[[maybe_unused]] constexpr build_type expected = build_type::release;
#endif
#ifdef _WIN32
	if constexpr (BuildType != build_type::universal && BuildType != expected) {
		MessageBox(
			nullptr,
			"Mismatched Debug/Release configurations between project and dpp.dll.\n"
			"Please ensure both your program and the D++ DLL file are both using the same configuration.\n"
			"The program will now terminate.",
			"D++ Debug/Release mismatch",
			MB_OK | MB_ICONERROR
		);
		/* Use std::runtime_rror here because dpp exceptions use std::string and that would crash when catching, because of ABI */
		throw std::runtime_error("Mismatched Debug/Release configurations between project and dpp.dll");
	}
	return true;
#else
	return true;
#endif
}

template bool DPP_EXPORT validate_configuration<build_type::debug>();

template bool DPP_EXPORT validate_configuration<build_type::release>();

template bool DPP_EXPORT validate_configuration<build_type::universal>();

cluster::cluster(const std::string &_token, uint32_t _intents, uint32_t _shards, uint32_t _cluster_id, uint32_t _maxclusters, bool comp, cache_policy_t policy, uint32_t request_threads, uint32_t request_threads_raw)
	: default_gateway("gateway.discord.gg"), rest(nullptr), raw_rest(nullptr), compressed(comp), start_time(0), token(_token), last_identify(time(nullptr) - 5), intents(_intents),
	numshards(_shards), cluster_id(_cluster_id), maxclusters(_maxclusters), rest_ping(0.0), cache_policy(policy), ws_mode(ws_json)
{
	/* Instantiate REST request queues */
	try {
		rest = new request_queue(this, request_threads);
		raw_rest = new request_queue(this, request_threads_raw);
	}
	catch (std::bad_alloc&) {
		delete rest;
		delete raw_rest;
		throw;
	}

	/* Add checks for missing intents, these emit a one-off warning to the log if bound without the right intents */
	on_message_create.set_warning_callback(
		make_intent_warning<message_create_t>(
			this,
			i_message_content,
			"You have attached an event to cluster::on_message_create() but have not specified the privileged intent dpp::i_message_content. Message content, embeds, attachments, and components on received guild messages will be empty.")
	);
	on_message_update.set_warning_callback(
		make_intent_warning<message_update_t>(
			this,
			i_message_content,
			"You have attached an event to cluster::on_message_update() but have not specified the privileged intent dpp::i_message_content. Message content, embeds, attachments, and components on received guild messages will be empty.")
	);
}

cluster::~cluster()
{
	this->shutdown();
	delete rest;
	delete raw_rest;
#ifdef _WIN32
	WSACleanup();
#endif
}

request_queue* cluster::get_rest() {
	return rest;
}

request_queue* cluster::get_raw_rest() {
	return raw_rest;
}

cluster& cluster::set_websocket_protocol(websocket_protocol_t mode) {
	if (start_time > 0) {
		throw dpp::logic_exception(err_websocket_proto_already_set, "Cannot change websocket protocol on a started cluster!");
	}
	ws_mode = mode;
	return *this;
}

void cluster::log(dpp::loglevel severity, const std::string &msg) const {
	if (!on_log.empty()) {
		/* Pass to user if they've hooked the event */
		dpp::log_t logmsg(nullptr, msg);
		logmsg.severity = severity;
		logmsg.message = msg;
		on_log.call(logmsg);
	}
}

dpp::utility::uptime cluster::uptime()
{
	return dpp::utility::uptime(time(nullptr) - start_time);
}

void cluster::start(bool return_after) {

	auto block_calling_thread = [this]() {
		std::mutex thread_mutex;
		std::unique_lock thread_lock(thread_mutex);
		this->terminating.wait(thread_lock);
	};

	/* Start up all shards */
	gateway g;
	try {
		g = dpp::sync<gateway>(this, &cluster::get_gateway_bot);
		log(ll_debug, "Cluster: " + std::to_string(g.session_start_remaining) + " of " + std::to_string(g.session_start_total) + " session starts remaining");
		if (g.session_start_remaining < g.shards) {
			throw dpp::connection_exception(err_no_sessions_left, "Discord indicates you cannot start enough sessions to boot this cluster! Cluster startup aborted. Try again later.");
		}
		if (g.session_start_max_concurrency > 1) {
			log(ll_debug, "Cluster: Large bot sharding; Using session concurrency: " + std::to_string(g.session_start_max_concurrency));
		}
		if (numshards == 0) {
			if (g.shards) {
				log(ll_info, "Auto Shard: Bot requires " + std::to_string(g.shards) + std::string(" shard") + ((g.shards > 1) ? "s" : ""));
			} else {
				throw dpp::connection_exception(err_auto_shard, "Auto Shard: Cannot determine number of shards. Cluster startup aborted. Check your connection.");
			}
			numshards = g.shards;
		}
	}
	catch (const dpp::rest_exception& e) {
		if (std::string(e.what()) == "401: Unauthorized") {
			/* Throw special form of exception for invalid token */
			throw dpp::invalid_token_exception(err_unauthorized, "Invalid bot token (401: Unauthorized when getting gateway shard count)");
		} else {
			/* Rethrow */
			throw e;
		}
	}

	start_time = time(nullptr);

	log(ll_debug, "Starting with " + std::to_string(numshards) + " shards...");

	for (uint32_t s = 0; s < numshards; ++s) {
		/* Filter out shards that aren't part of the current cluster, if the bot is clustered */
		if (s % maxclusters == cluster_id) {
			/* Each discord_client spawns its own thread in its run() */
			try {
				this->shards[s] = new discord_client(this, s, numshards, token, intents, compressed, ws_mode);
				this->shards[s]->run();
			}
			catch (const std::exception &e) {
				log(dpp::ll_critical, "Could not start shard " + std::to_string(s) + ": " + std::string(e.what()));
			}
			/* Stagger the shard startups, pausing every 'session_start_max_concurrency' shards for 5 seconds.
			 * This means that for bots that don't have large bot sharding, any number % 1 is always 0,
			 * so it will pause after every shard. For any with non-zero concurrency it'll pause 5 seconds
			 * after every batch.
			 */
			if (((s + 1) % g.session_start_max_concurrency) == 0) {
				size_t wait_time = 5;
				if (g.session_start_max_concurrency > 1) {
					/* If large bot sharding, be sure to give the batch of shards time to settle */
					bool all_connected = true;
					do {
						all_connected = true;
						for (auto& shard : this->shards) {
							if (!shard.second->ready) {
								all_connected = false;
								std::this_thread::sleep_for(std::chrono::milliseconds(100));
								break;
							}
						}
					} while (all_connected);
				}
				std::this_thread::sleep_for(std::chrono::seconds(wait_time));
			}
		}
	}

	/* Get all active DM channels and map them to user id -> dm id */
	this->current_user_get_dms([this](const dpp::confirmation_callback_t& completion) {
		dpp::channel_map dmchannels = std::get<channel_map>(completion.value);
		for (auto & c : dmchannels) {
			for (auto & u : c.second.recipients) {
				this->set_dm_channel(u, c.second.id);
			}
		}
	});

	log(ll_debug, "Shards started.");
	
	if (!return_after) {
		block_calling_thread();
	}
}

void cluster::shutdown() {
	/* Signal condition variable to terminate */
	terminating.notify_all();
	/* Free memory for active timers */
	for (auto & t : timer_list) {
		delete t.second;
	}
	timer_list.clear();
	/* Terminate shards */
	for (const auto& sh : shards) {
		log(ll_info, "Terminating shard id " + std::to_string(sh.second->shard_id));
		delete sh.second;
	}
	shards.clear();
}

snowflake cluster::get_dm_channel(snowflake user_id) {
	std::lock_guard<std::mutex> lock(dm_list_lock);
	auto i = dm_channels.find(user_id);
	if (i != dm_channels.end()) {
		return i->second;
	} else {
		return 0;
	}
}

void cluster::set_dm_channel(snowflake user_id, snowflake channel_id) {
	std::lock_guard<std::mutex> lock(dm_list_lock);
	dm_channels[user_id] = channel_id;
}

/**
 * @brief Given an error exception from nlohmann::json, turn it into a discord-style error object
 * that can be parsed by the get_error() function. Modifies the http request body, moving it to
 * `.get_error().errors[0].reason` in the callback.
 * 
 * @param message Exception message
 * @param rv Request completion data
 * @return json
 */
json error_response(const std::string& message, http_request_completion_t& rv)
{
	json j({
		{"code", rv.status},
		{"errors", {
			{"json", {
				{"0", {
					{"body", {
						{"_errors", {{
							{"code", "JSON_PARSE"},
							{"message", rv.body},
						}}}
					}}
				}}
			}}
		}},
		{"message", message}
	});
	rv.body = j.dump(-1, ' ', false, json::error_handler_t::replace);
	return j;
}

void cluster::post_rest(const std::string &endpoint, const std::string &major_parameters, const std::string &parameters, http_method method, const std::string &postdata, json_encode_t callback, const std::string &filename, const std::string &filecontent, const std::string &filemimetype, const std::string &protocol) {
	rest->post_request(std::make_unique<http_request>(endpoint + (!major_parameters.empty() ? "/" : "") + major_parameters, parameters, [endpoint, callback](http_request_completion_t rv) {
		json j;
		if (rv.error == h_success && !rv.body.empty()) {
			try {
				j = json::parse(rv.body);
			}
			catch (const std::exception &e) {
				j = error_response(e.what(), rv);
			}
		}
		if (callback) {
			callback(j, rv);
		}
	}, postdata, method, get_audit_reason(), filename, filecontent, filemimetype, protocol));
}

void cluster::post_rest_multipart(const std::string &endpoint, const std::string &major_parameters, const std::string &parameters, http_method method, const std::string &postdata, json_encode_t callback, const std::vector<message_file_data> &file_data) {
	std::vector<std::string> file_names{};
	std::vector<std::string> file_contents{};
	std::vector<std::string> file_mimetypes{};

	for(const message_file_data& data : file_data) {
		file_names.push_back(data.name);
		file_contents.push_back(data.content);
		file_mimetypes.push_back(data.mimetype);
	}

	rest->post_request(std::make_unique<http_request>(endpoint + (!major_parameters.empty() ? "/" : "") + major_parameters, parameters, [endpoint, callback](http_request_completion_t rv) {
		json j;
		if (rv.error == h_success && !rv.body.empty()) {
			try {
				j = json::parse(rv.body);
			}
			catch (const std::exception &e) {
				j = error_response(e.what(), rv);
			}
		}
		if (callback) {
			callback(j, rv);
		}
	}, postdata, method, get_audit_reason(), file_names, file_contents, file_mimetypes));
}


void cluster::request(const std::string &url, http_method method, http_completion_event callback, const std::string &postdata, const std::string &mimetype, const std::multimap<std::string, std::string> &headers, const std::string &protocol) {
	raw_rest->post_request(std::make_unique<http_request>(url, callback, method, postdata, mimetype, headers, protocol));
}

gateway::gateway() : shards(0), session_start_total(0), session_start_remaining(0), session_start_reset_after(0), session_start_max_concurrency(0) {
}

gateway& gateway::fill_from_json_impl(nlohmann::json* j) {
	url = string_not_null(j, "url");
	shards = int32_not_null(j, "shards");
	session_start_total = int32_not_null(&((*j)["session_start_limit"]), "total");
	session_start_remaining  = int32_not_null(&((*j)["session_start_limit"]), "remaining");
	session_start_reset_after = int32_not_null(&((*j)["session_start_limit"]), "reset_after");
	session_start_max_concurrency = int32_not_null(&((*j)["session_start_limit"]), "max_concurrency");
	return *this;
}

gateway::gateway(nlohmann::json* j) {
	fill_from_json(j);
}

void cluster::set_presence(const dpp::presence &p) {
	if(p.activities.empty()) {
		log(ll_warning, "An empty presence was passed to set_presence.");
		return;
	}

	json pres = p.to_json();
	for (auto& s : shards) {
		if (s.second->is_connected()) {
			s.second->queue_message(s.second->jsonobj_to_string(pres));
		}
	}
}

cluster& cluster::set_audit_reason(const std::string &reason) {
	audit_reason = reason;
	return *this;
}

cluster& cluster::clear_audit_reason() {
	audit_reason.clear();
	return *this;
}

cluster& cluster::set_default_gateway(std::string &default_gateway_new) {
	default_gateway = default_gateway_new;
	return *this;
}

std::string cluster::get_audit_reason() {
	std::string r = audit_reason;
	audit_reason.clear();
	return r;
}

discord_client* cluster::get_shard(uint32_t id) {
	auto i = shards.find(id);
	if (i != shards.end()) {
		return i->second;
	} else {
		return nullptr;
	}
}

const shard_list& cluster::get_shards() {
	return shards;
}

};
