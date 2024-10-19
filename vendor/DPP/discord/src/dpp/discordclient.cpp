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
#include <string>
#include <iostream>
#include <fstream>
#include <dpp/exception.h>
#include <dpp/discordclient.h>
#include <dpp/cache.h>
#include <dpp/cluster.h>
#include <thread>
#include <dpp/json.h>
#include <dpp/etf.h>
#include <zlib.h>
#ifdef _WIN32
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <io.h>
#else
	#include <unistd.h>
	#include <netinet/in.h>
	#include <resolv.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/tcp.h>
#endif

#define PATH_UNCOMPRESSED_JSON	"/?v=" DISCORD_API_VERSION "&encoding=json"
#define PATH_COMPRESSED_JSON	"/?v=" DISCORD_API_VERSION "&encoding=json&compress=zlib-stream"
#define PATH_UNCOMPRESSED_ETF	"/?v=" DISCORD_API_VERSION "&encoding=etf"
#define PATH_COMPRESSED_ETF	"/?v=" DISCORD_API_VERSION "&encoding=etf&compress=zlib-stream"
#define DECOMP_BUFFER_SIZE	512 * 1024

#define STRINGIFY(a) STRINGIFY_(a)
#define STRINGIFY_(a) #a

#ifndef DPP_OS
#define DPP_OS unknown
#endif

namespace dpp {

/**
 * @brief This is an opaque class containing zlib library specific structures.
 * We define it this way so that the public facing D++ library doesn't require
 * the zlib headers be available to build against it.
 */
class zlibcontext {
public:
	/**
	 * @brief Zlib stream struct
	 */
	z_stream d_stream;
};

/**
 * @brief Stores the most recent ping message on this shard, which we check for to monitor latency
 */
thread_local static std::string last_ping_message;

discord_client::discord_client(dpp::cluster* _cluster, uint32_t _shard_id, uint32_t _max_shards, const std::string &_token, uint32_t _intents, bool comp, websocket_protocol_t ws_proto)
       : websocket_client(_cluster->default_gateway, "443", comp ? (ws_proto == ws_json ? PATH_COMPRESSED_JSON : PATH_COMPRESSED_ETF) : (ws_proto == ws_json ? PATH_UNCOMPRESSED_JSON : PATH_UNCOMPRESSED_ETF)),
        terminating(false),
        runner(nullptr),
	compressed(comp),
	decomp_buffer(nullptr),
	zlib(nullptr),
	decompressed_total(0),
	connect_time(0),
	ping_start(0.0),
	etf(nullptr),
	creator(_cluster),
	heartbeat_interval(0),
	last_heartbeat(time(nullptr)),
	shard_id(_shard_id),
	max_shards(_max_shards),
	last_seq(0),
	token(_token),
	intents(_intents),
	resumes(0),
	reconnects(0),
	websocket_ping(0.0),
	ready(false),
	last_heartbeat_ack(time(nullptr)),
	protocol(ws_proto),
	resume_gateway_url(_cluster->default_gateway)	
{
	try {
		zlib = new zlibcontext();
		etf = new etf_parser();
	}
	catch (std::bad_alloc&) {
		delete zlib;
		delete etf;
		/* Clean up and rethrow to caller */
		throw std::bad_alloc();
	}
	try {
		this->connect();
	}
	catch (std::exception&) {
		cleanup();
		throw;
	}
}

void discord_client::cleanup()
{
	terminating = true;
	if (runner) {
		runner->join();
		delete runner;
	}
	delete etf;
	delete zlib;
}

discord_client::~discord_client()
{
	cleanup();
}

uint64_t discord_client::get_decompressed_bytes_in()
{
	return decompressed_total;
}

void discord_client::setup_zlib()
{
	if (compressed) {
		zlib->d_stream.zalloc = (alloc_func)0;
		zlib->d_stream.zfree = (free_func)0;
		zlib->d_stream.opaque = (voidpf)0;
		int error = inflateInit(&(zlib->d_stream));
		if (error != Z_OK) {
			throw dpp::connection_exception((exception_error_code)error, "Can't initialise stream compression!");
		}
		this->decomp_buffer = new unsigned char[DECOMP_BUFFER_SIZE];
	}

}

void discord_client::end_zlib()
{
	if (compressed) {
		inflateEnd(&(zlib->d_stream));
		delete[] this->decomp_buffer;
		this->decomp_buffer = nullptr;
	}
}

void discord_client::set_resume_hostname()
{
	hostname = resume_gateway_url;
}

void discord_client::thread_run()
{
	utility::set_thread_name(std::string("shard/") + std::to_string(shard_id));
	setup_zlib();
	do {
		bool error = false;
		ready = false;
		message_queue.clear();
		ssl_client::read_loop();
		if (!terminating) {
			ssl_client::close();
			end_zlib();
			setup_zlib();
			do {
				this->log(ll_debug, "Attempting reconnection of shard " + std::to_string(this->shard_id) + " to wss://" + resume_gateway_url);
				error = false;
				try {
					set_resume_hostname();
					ssl_client::connect();
					websocket_client::connect();
				}
				catch (const std::exception &e) {
					log(dpp::ll_error, std::string("Error establishing connection, retry in 5 seconds: ") + e.what());
					ssl_client::close();
					std::this_thread::sleep_for(std::chrono::seconds(5));
					error = true;
				}
			} while (error);
		}
	} while(!terminating);
	if (this->sfd != INVALID_SOCKET) {
		/* Send a graceful termination */
		this->log(ll_debug, "Graceful shutdown of shard " + std::to_string(this->shard_id) + " succeeded.");
		this->nonblocking = false;
		this->send_close_packet();
		ssl_client::close();
	} else {
		this->log(ll_debug, "Graceful shutdown of shard " + std::to_string(this->shard_id) + " not possible, socket already closed.");
	}
	end_zlib();
}

void discord_client::run()
{
	this->runner = new std::thread(&discord_client::thread_run, this);
	this->thread_id = runner->native_handle();
}

bool discord_client::handle_frame(const std::string &buffer)
{
	std::string& data = (std::string&)buffer;

	/* gzip compression is a special case */
	if (compressed) {
		/* Check that we have a complete compressed frame */
		if ((uint8_t)buffer[buffer.size() - 4] == 0x00 && (uint8_t)buffer[buffer.size() - 3] == 0x00 && (uint8_t)buffer[buffer.size() - 2] == 0xFF
		&& (uint8_t)buffer[buffer.size() - 1] == 0xFF) {
			/* Decompress buffer */
			decompressed.clear();
			zlib->d_stream.next_in = (Bytef *)buffer.c_str();
			zlib->d_stream.avail_in = (uInt)buffer.size();
			do {
				int have = 0;
				zlib->d_stream.next_out = (Bytef*)decomp_buffer;
				zlib->d_stream.avail_out = DECOMP_BUFFER_SIZE;
				int ret = inflate(&(zlib->d_stream), Z_NO_FLUSH);
				have = DECOMP_BUFFER_SIZE - zlib->d_stream.avail_out;
				switch (ret)
				{
					case Z_NEED_DICT:
					case Z_STREAM_ERROR:
						this->error(err_compression_stream);
						this->close();
						return true;
					break;
					case Z_DATA_ERROR:
						this->error(err_compression_data);
						this->close();
						return true;
					break;
					case Z_MEM_ERROR:
						this->error(err_compression_memory);
						this->close();
						return true;
					break;
					case Z_OK:
						this->decompressed.append((const char*)decomp_buffer, have);
						this->decompressed_total += have;
					break;
					default:
						/* Stub */
					break;
				}
			} while (zlib->d_stream.avail_out == 0);
			data = decompressed;
		} else {
			/* No complete compressed frame yet */
			return false;
		}
	}


	json j;
	
	/**
	 * This section parses the input frames from the websocket after they're decompressed.
	 * Note that both ETF and JSON parsers return an nlohmann::json object, so that the rest
	 * of the library or any user code does not need to be concerned with protocol differences.
	 * Generally, ETF is faster and consumes much less memory, but provides less opportunities
	 * to diagnose if it goes wrong.
	 */
	switch (protocol) {
		case ws_json:
			try {
				j = json::parse(data);
			}
			catch (const std::exception &e) {
				log(dpp::ll_error, "discord_client::handle_frame(JSON): " + std::string(e.what()) + " [" + data + "]");
				return true;
			}
		break;
		case ws_etf:
			try {
				j = etf->parse(data);
			}
			catch (const std::exception &e) {
				log(dpp::ll_error, "discord_client::handle_frame(ETF): " + std::string(e.what()) + " len=" + std::to_string(data.size()) + "\n" + dpp::utility::debug_dump((uint8_t*)data.data(), data.size()));
				return true;
			}
		break;
	}

	auto seq = j.find("s");
	if (seq != j.end() && !seq->is_null()) {
		last_seq = seq->get<uint64_t>();
	}

	auto o = j.find("op");
	if (o != j.end() && !o->is_null()) {
		uint32_t op = o->get<uint32_t>();

		switch (op) {
			case 9:
				/* Reset session state and fall through to 10 */
				op = 10;
				log(dpp::ll_debug, "Failed to resume session " + sessionid + ", will reidentify");
				this->sessionid.clear();
				this->last_seq = 0;
				/* No break here, falls through to state 10 to cause a reidentify */
				[[fallthrough]];
			case 10:
				/* Need to check carefully for the existence of this before we try to access it! */
				if (j.find("d") != j.end() && j["d"].find("heartbeat_interval") != j["d"].end() && !j["d"]["heartbeat_interval"].is_null()) {
					this->heartbeat_interval = j["d"]["heartbeat_interval"].get<uint32_t>();
				}

				if (last_seq && !sessionid.empty()) {
					/* Resume */
					log(dpp::ll_debug, "Resuming session " + sessionid + " with seq=" + std::to_string(last_seq));
					json obj = {
						{ "op", 6 },
						{ "d", {
								{"token", this->token },
								{"session_id", this->sessionid },
								{"seq", this->last_seq }
							}
						}
					};
					this->write(jsonobj_to_string(obj));
					resumes++;
				} else {
					/* Full connect */
					while (time(nullptr) < creator->last_identify + 5) {
						time_t wait = (creator->last_identify + 5) - time(nullptr);
						std::this_thread::sleep_for(std::chrono::seconds(wait));
					}
					log(dpp::ll_debug, "Connecting new session...");
					json obj = {
						{ "op", 2 },
						{
							"d",
							{
								{ "token", this->token },
								{ "properties",
									{
										{ "os", STRINGIFY(DPP_OS) },
										{ "browser", "D++" },
										{ "device", "D++" }
									}
								},
								{ "shard", json::array({ shard_id, max_shards }) },
								{ "compress", false },
								{ "large_threshold", 250 },
								{ "intents", this->intents }
							}
						}
					};
					this->write(jsonobj_to_string(obj));
					this->connect_time = creator->last_identify = time(nullptr);
					reconnects++;
				}
				this->last_heartbeat_ack = time(nullptr);
				websocket_ping = 0;
			break;
			case 0: {
				std::string event = j["t"];
				handle_event(event, j, data);
			}
			break;
			case 7:
				log(dpp::ll_debug, "Reconnection requested, closing socket " + sessionid);
				message_queue.clear();
				throw dpp::connection_exception(err_reconnection, "Remote site requested reconnection");
			break;
			/* Heartbeat ack */
			case 11:
				this->last_heartbeat_ack = time(nullptr);
				websocket_ping = utility::time_f() - ping_start;
			break;
		}
	}
	return true;
}

dpp::utility::uptime discord_client::get_uptime()
{
	return dpp::utility::uptime(time(nullptr) - connect_time);
}

bool discord_client::is_connected()
{
	return (this->get_state() == CONNECTED) && (this->ready);
}

void discord_client::error(uint32_t errorcode)
{
	const static std::map<uint32_t, std::string> errortext = {
		{ 1000, "Socket shutdown" },
		{ 1001, "Client is leaving" },
		{ 1002, "Endpoint received a malformed frame" },
		{ 1003, "Endpoint received an unsupported frame" },
		{ 1004, "Reserved code" },
		{ 1005, "Expected close status, received none" },
		{ 1006, "No close code frame has been received" },
		{ 1007, "Endpoint received inconsistent message (e.g. malformed UTF-8)" },
		{ 1008, "Generic error" },
		{ 1009, "Endpoint won't process large frame" },
		{ 1010, "Client wanted an extension which server did not negotiate" },
		{ 1011, "Internal server error while operating" },
		{ 1012, "Server/service is restarting" },
		{ 1013, "Temporary server condition forced blocking client's request" },
		{ 1014, "Server acting as gateway received an invalid response" },
		{ 1015, "Transport Layer Security handshake failure" },
		{ 4000, "Unknown error" },
		{ 4001, "Unknown opcode" },
		{ 4002, "Decode error" },
		{ 4003, "Not authenticated" },
		{ 4004, "Authentication failed" },
		{ 4005, "Already authenticated" },
		{ 4007, "Invalid seq" },
		{ 4008, "Rate limited" },
		{ 4009, "Session timed out" },
		{ 4010, "Invalid shard" },
		{ 4011, "Sharding required" },
		{ 4012, "Invalid API version" },
		{ 4013, "Invalid intent(s)" },
		{ 4014, "Disallowed intent(s)" },
		{ 6000, "ZLib Stream Error" },
		{ 6001, "ZLib Data Error" },
		{ 6002, "ZLib Memory Error" },
		{ 6666, "Hell freezing over" }
	};
	std::string error = "Unknown error";
	auto i = errortext.find(errorcode);
	if (i != errortext.end()) {
		error = i->second;
	}
	log(dpp::ll_warning, "OOF! Error from underlying websocket: " + std::to_string(errorcode) + ": " + error);
}

void discord_client::log(dpp::loglevel severity, const std::string &msg) const
{
	if (!creator->on_log.empty()) {
		/* Pass to user if they've hooked the event */
		dpp::log_t logmsg(nullptr, msg);
		logmsg.severity = severity;
		logmsg.message = msg;
		creator->on_log.call(logmsg);
	}
}

void discord_client::queue_message(const std::string &j, bool to_front)
{
	std::unique_lock locker(queue_mutex);
	if (to_front) {
		message_queue.emplace_front(j);
	} else {
		message_queue.emplace_back(j);
	}
}

discord_client& discord_client::clear_queue()
{
	std::unique_lock locker(queue_mutex);
	message_queue.clear();
	return *this;
}

size_t discord_client::get_queue_size()
{
	std::shared_lock locker(queue_mutex);
	return message_queue.size();
}

void discord_client::one_second_timer()
{
	if (terminating) {
		throw dpp::exception("Shard terminating due to cluster shutdown");
	}

	websocket_client::one_second_timer();

	/* Every minute, rehash all containers from first shard.
	 * We can't just get shard with the id 0 because this won't
	 * work on a clustered environment
	 */
	auto shards = creator->get_shards();
	auto first_iter = shards.begin();
	if (first_iter != shards.end()) {
		dpp::discord_client* first_shard = first_iter->second;
		if (first_shard == this) {
			creator->tick_timers();

			if ((time(nullptr) % 60) == 0) {
				dpp::garbage_collection();
			}
		}
	}

	/* This all only triggers if we are connected (have completed websocket, and received READY or RESUMED) */
	if (this->is_connected()) {

		/* If we stopped getting heartbeat acknowledgements, this means the connections is dead.
		 * This can happen to TCP connections which is why we have heartbeats in the first place.
		 * Miss two ACKS, forces a reconnection.
		 */
		if ((time(nullptr) - this->last_heartbeat_ack) > heartbeat_interval * 2) {
			log(dpp::ll_warning, "Missed heartbeat ACK, forcing reconnection to session " + sessionid);
			message_queue.clear();
			close_socket(sfd);
			return;
		}

		/* Rate limit outbound messages, 1 every odd second, 2 every even second */
		for (int x = 0; x < (time(nullptr) % 2) + 1; ++x) {
			std::unique_lock locker(queue_mutex);
			if (message_queue.size()) {
				std::string message = message_queue.front();
				message_queue.pop_front();
				/* Checking here with .find() saves us having to deserialise the json
				 * to find pings in our queue. The assumption is that the format of the
				 * ping isn't going to change.
				 */
				if (!last_ping_message.empty() && message == last_ping_message) {
					ping_start = utility::time_f();
					last_ping_message.clear();
				}
				this->write(message);
			}
		}

		/* Send pings (heartbeat opcodes) before each interval. We send them slightly more regular than expected,
		 * just to be safe.
		 */
		if (this->heartbeat_interval && this->last_seq) {
			/* Check if we're due to emit a heartbeat */
			if (time(nullptr) > last_heartbeat + ((heartbeat_interval / 1000.0) * 0.75)) {
				last_ping_message = jsonobj_to_string(json({{"op", 1}, {"d", last_seq}}));
				queue_message(last_ping_message, true);
				last_heartbeat = time(nullptr);
			}
		}
	}
}

uint64_t discord_client::get_guild_count() {
	uint64_t total = 0;
	dpp::cache<guild>* c = dpp::get_guild_cache();
	/* IMPORTANT: We must lock the container to iterate it */
	std::shared_lock l(c->get_mutex());
	std::unordered_map<snowflake, guild*>& gc = c->get_container();
	for (auto g = gc.begin(); g != gc.end(); ++g) {
		dpp::guild* gp = (dpp::guild*)g->second;
		if (gp->shard_id == this->shard_id) {
			total++;
		}
	}
	return total;
}

uint64_t discord_client::get_member_count() {
	uint64_t total = 0;
	dpp::cache<guild>* c = dpp::get_guild_cache();
	/* IMPORTANT: We must lock the container to iterate it */
	std::shared_lock l(c->get_mutex());
	std::unordered_map<snowflake, guild*>& gc = c->get_container();
	for (auto g = gc.begin(); g != gc.end(); ++g) {
		dpp::guild* gp = (dpp::guild*)g->second;
		if (gp->shard_id == this->shard_id) {
			if (creator->cache_policy.user_policy == dpp::cp_aggressive) {
				/* We can use actual member count if we are using full user caching */
				total += gp->members.size();
			} else {
				/* Otherwise we use approximate guild member counts from guild_create */
				total += gp->member_count;
			}
		}
	}
	return total;
}

uint64_t discord_client::get_channel_count() {
	uint64_t total = 0;
	dpp::cache<guild>* c = dpp::get_guild_cache();
	/* IMPORTANT: We must lock the container to iterate it */
	std::shared_lock l(c->get_mutex());
	std::unordered_map<snowflake, guild*>& gc = c->get_container();
	for (auto g = gc.begin(); g != gc.end(); ++g) {
		dpp::guild* gp = (dpp::guild*)g->second;
		if (gp->shard_id == this->shard_id) {
			total += gp->channels.size();
		}
	}
	return total;
}

discord_client& discord_client::connect_voice(snowflake guild_id, snowflake channel_id, bool self_mute, bool self_deaf) {
#ifdef HAVE_VOICE
	std::unique_lock lock(voice_mutex);
	if (connecting_voice_channels.find(guild_id) != connecting_voice_channels.end()) {
		if (connecting_voice_channels[guild_id]->channel_id == channel_id) {
			log(ll_debug, "Requested the bot connect to voice channel " + std::to_string(channel_id) + " on guild " + std::to_string(guild_id) + ", but it seems we are already on this VC");
			return *this;
		}
	}
	connecting_voice_channels[guild_id] = std::make_unique<voiceconn>(this, channel_id);
	/* Once sent, this expects two events (in any order) on the websocket:
	* VOICE_SERVER_UPDATE and VOICE_STATUS_UPDATE
	*/
	log(ll_debug, "Sending op 4 to join VC, guild " + std::to_string(guild_id) + " channel " + std::to_string(channel_id));
	queue_message(jsonobj_to_string(json({
		{ "op", 4 },
		{ "d", {
				{ "guild_id", std::to_string(guild_id) },
				{ "channel_id", std::to_string(channel_id) },
				{ "self_mute", self_mute },
				{ "self_deaf", self_deaf },
			}
		}
	})), false);
#endif
	return *this;
}

std::string discord_client::jsonobj_to_string(const nlohmann::json& json) {
	if (protocol == ws_json) {
		return json.dump(-1, ' ', false, json::error_handler_t::replace);
	} else {
		return etf->build(json);
	}
}

void discord_client::disconnect_voice_internal(snowflake guild_id, bool emit_json) {
#ifdef HAVE_VOICE
	std::unique_lock lock(voice_mutex);
	auto v = connecting_voice_channels.find(guild_id);
	if (v != connecting_voice_channels.end()) {
		log(ll_debug, "Disconnecting voice, guild: " + std::to_string(guild_id));
		if (emit_json) {
			queue_message(jsonobj_to_string(json({
				{ "op", 4 },
				{ "d", {
						{ "guild_id", std::to_string(guild_id) },
						{ "channel_id", json::value_t::null },
						{ "self_mute", false },
						{ "self_deaf", false },
					}
				}
			})), false);
		}
		connecting_voice_channels.erase(v);
	}
#endif
}

discord_client& discord_client::disconnect_voice(snowflake guild_id) {
	disconnect_voice_internal(guild_id, true);
	return *this;
}

voiceconn* discord_client::get_voice(snowflake guild_id) {
#ifdef HAVE_VOICE
	std::shared_lock lock(voice_mutex);
	auto v = connecting_voice_channels.find(guild_id);
	if (v != connecting_voice_channels.end()) {
		return v->second.get();
	}
#endif
	return nullptr;
}


voiceconn::voiceconn(discord_client* o, snowflake _channel_id) : creator(o), channel_id(_channel_id), voiceclient(nullptr) {
}

bool voiceconn::is_ready() {
	return (!websocket_hostname.empty() && !session_id.empty() && !token.empty());
}

bool voiceconn::is_active() {
	return voiceclient != nullptr;
}

voiceconn& voiceconn::disconnect() {
	if (this->is_active()) {
		delete voiceclient;
		voiceclient = nullptr;
	}
	return *this;
}

voiceconn::~voiceconn() {
	this->disconnect();
}

voiceconn& voiceconn::connect(snowflake guild_id) {
	if (this->is_ready() && !this->is_active()) {
		/* This is wrapped in a thread because instantiating discord_voice_client can initiate a blocking SSL_connect() */
		auto t = std::thread([guild_id, this]() {
			try {
				this->creator->log(ll_debug, "Connecting voice for guild " + std::to_string(guild_id) + " channel " + std::to_string(this->channel_id));
				this->voiceclient = new discord_voice_client(creator->creator, this->channel_id, guild_id, this->token, this->session_id, this->websocket_hostname);
				/* Note: Spawns thread! */
				this->voiceclient->run();
			}
			catch (std::exception &e) {
				this->creator->log(ll_debug, "Can't connect to voice websocket (guild_id: " + std::to_string(guild_id) + ", channel_id: " + std::to_string(this->channel_id) + ": " + std::string(e.what()));
			}
		});
		t.detach();
	}
	return *this;
}


} // namespace dpp
