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
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <io.h>
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <resolv.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/tcp.h>
#endif
#include <string_view>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <dpp/exception.h>
#include <dpp/isa_detection.h>
#include <dpp/discordvoiceclient.h>
#include <dpp/cache.h>
#include <dpp/cluster.h>
#include <dpp/json.h>

#ifdef HAVE_VOICE
	#include <sodium.h>
	#include <opus/opus.h>
#else
	struct OpusDecoder {};
	struct OpusEncoder {};
	struct OpusRepacketizer {};
#endif

namespace dpp {
moving_averager::moving_averager(uint64_t collection_count_new) {
	collectionCount = collection_count_new;
}

moving_averager moving_averager::operator+=(int64_t value) {
	values.emplace_front(value);
	if (values.size() >= collectionCount) {
		values.pop_back();
	}
	return *this;
}

moving_averager::operator float() {
	float returnData{};
	if (values.size() > 0) {
		for (auto& value : values) {
			returnData += static_cast<float>(value);
		}
		return returnData / static_cast<float>(values.size());
	}
	else {
		return 0.0f;
	}
}

[[maybe_unused]]
constexpr int32_t opus_sample_rate_hz = 48000;
[[maybe_unused]]
constexpr int32_t opus_channel_count = 2;
std::string external_ip;

/**
 * @brief Represents an RTP packet. Size should always be exactly 12.
 */
struct rtp_header {
	uint16_t constant;
	uint16_t sequence;
	uint32_t timestamp;
	uint32_t ssrc;

	rtp_header(uint16_t _seq, uint32_t _ts, uint32_t _ssrc) : constant(htons(0x8078)), sequence(htons(_seq)), timestamp(htonl(_ts)), ssrc(htonl(_ssrc)) {
	}
};

bool discord_voice_client::sodium_initialised = false;

bool discord_voice_client::voice_payload::operator<(const voice_payload& other) const {
	if (timestamp != other.timestamp) {
		return timestamp > other.timestamp;
	}

	constexpr rtp_seq_t wrap_around_test_boundary = 5000;
	if ((seq < wrap_around_test_boundary && other.seq >= wrap_around_test_boundary)
    	    || (seq >= wrap_around_test_boundary && other.seq < wrap_around_test_boundary)) {
    		/* Match the cases where exactly one of the sequence numbers "may have"
		 * wrapped around.
		 *
		 * Examples:
		 * 1. this->seq = 65530, other.seq = 10  // Did wrap around
		 * 2. this->seq = 5002, other.seq = 4990 // Not wrapped around
		 *
		 * Add 5000 to both sequence numbers to force wrap around so they can be
		 * compared. This should be fine to do to case 2 as well, as long as the
		 * addend (5000) is not too large to cause one of them to wrap around.
		 *
		 * In practice, we should be unlikely to hit the case where
		 *
		 *           this->seq = 65530, other.seq = 5001
		 *
		 * because we shouldn't receive more than 5000 payloads in one batch, unless
		 * the voice courier thread is super slow. Also remember that the timestamp
		 * is compared first, and payloads this far apart shouldn't have the same
		 * timestamp.
		 */

		/* Casts here ensure the sum wraps around and not implicitly converted to
		 * wider types.
		 */
		return   static_cast<rtp_seq_t>(seq + wrap_around_test_boundary)
		       > static_cast<rtp_seq_t>(other.seq + wrap_around_test_boundary);
	} else {
		return seq > other.seq;
	}
}

#ifdef HAVE_VOICE
size_t audio_mix(discord_voice_client& client, audio_mixer& mixer, opus_int32* pcm_mix, const opus_int16* pcm, size_t park_count, int samples, int& max_samples) {
	/* Mix the combined stream if combined audio is bound */
	if (client.creator->on_voice_receive_combined.empty()) {
		return 0;
	}

	/* We must upsample the data to 32 bits wide, otherwise we could overflow */
	for (opus_int32 v = 0; v < (samples * opus_channel_count) / mixer.byte_blocks_per_register; ++v) {
		mixer.combine_samples(pcm_mix, pcm);
		pcm += mixer.byte_blocks_per_register;
		pcm_mix += mixer.byte_blocks_per_register;
	}
	client.moving_average += park_count;
	max_samples = (std::max)(samples, max_samples);
	return park_count + 1;
}
#endif

void discord_voice_client::voice_courier_loop(discord_voice_client& client, courier_shared_state_t& shared_state) {
#ifdef HAVE_VOICE
	utility::set_thread_name(std::string("vcourier/") + std::to_string(client.server_id));
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds{client.iteration_interval});
		
		struct flush_data_t {
			snowflake user_id;
			rtp_seq_t min_seq;
			std::priority_queue<voice_payload> parked_payloads;
			std::vector<std::function<void(OpusDecoder&)>> pending_decoder_ctls;
			std::shared_ptr<OpusDecoder> decoder;
		};
		std::vector<flush_data_t> flush_data;

		/*
		 * Transport the payloads onto this thread, and
		 * release the lock as soon as possible.
		 */
		{
			std::unique_lock lk(shared_state.mtx);

			/* mitigates vector resizing while holding the mutex */
			flush_data.reserve(shared_state.parked_voice_payloads.size());

			bool has_payload_to_deliver = false;
			for (auto& [user_id, parking_lot] : shared_state.parked_voice_payloads) {
				has_payload_to_deliver = has_payload_to_deliver || !parking_lot.parked_payloads.empty();
				flush_data.push_back(flush_data_t{user_id,
				                                  parking_lot.range.min_seq,
				                                  std::move(parking_lot.parked_payloads),
				                                  /* Quickly check if we already have a decoder and only take the pending ctls if so. */
				                                  parking_lot.decoder ? std::move(parking_lot.pending_decoder_ctls)
				                                                      : decltype(parking_lot.pending_decoder_ctls){},
				                                  parking_lot.decoder});
				parking_lot.range.min_seq = parking_lot.range.max_seq + 1;
				parking_lot.range.min_timestamp = parking_lot.range.max_timestamp + 1;
			}
            
			if (!has_payload_to_deliver) {
				if (shared_state.terminating) {
					/* We have delivered all data to handlers. Terminate now. */
					break;
				}

				shared_state.signal_iteration.wait(lk);
				/*
				 * More data came or about to terminate, or just a spurious wake.
				 * We need to collect the payloads again to determine what to do next.
				 */
				continue;
			}
		}

		if (client.creator->on_voice_receive.empty() && client.creator->on_voice_receive_combined.empty()) {
			/*
			 * We do this check late, to ensure this thread drains the data
			 * and prevents accumulating them even when there are no handlers.
			 */
			continue;
		}

		/* This 32 bit PCM audio buffer is an upmixed version of the streams
		 * combined for all users. This is a wider width audio buffer so that
	 	 * there is no clipping when there are many loud audio sources at once.
		 */
		opus_int32 pcm_mix[23040] = { 0 };
		size_t park_count = 0;
		int max_samples = 0;
		int samples = 0;

		for (auto& d : flush_data) {
			if (!d.decoder) {
				continue;
			}
			for (const auto& decoder_ctl : d.pending_decoder_ctls) {
				decoder_ctl(*d.decoder);
			}
			for (rtp_seq_t seq = d.min_seq; !d.parked_payloads.empty(); ++seq) {
				opus_int16 pcm[23040];
				if (d.parked_payloads.top().seq != seq) {
					/*
					 * Lost a packet with sequence number "seq",
					 * But Opus decoder might be able to guess something.
					 */
					if (int samples = opus_decode(d.decoder.get(), nullptr, 0, pcm, 5760, 0);
					    samples >= 0) {
						/*
						 * Since this sample comes from a lost packet,
						 * we can only pretend there is an event, without any raw payload byte.
						 */
						voice_receive_t vr(nullptr, "", &client, d.user_id, reinterpret_cast<uint8_t*>(pcm),
							samples * opus_channel_count * sizeof(opus_int16));

						park_count = audio_mix(client, *client.mixer, pcm_mix, pcm, park_count, samples, max_samples);
						client.creator->on_voice_receive.call(vr);
					}
				} else {
					voice_receive_t& vr = *d.parked_payloads.top().vr;
					if (vr.audio_data.length() > 0x7FFFFFFF) {
						throw dpp::length_exception(err_massive_audio, "audio_data > 2GB! This should never happen!");
					}
					if (samples = opus_decode(d.decoder.get(), vr.audio_data.data(),
						static_cast<opus_int32>(vr.audio_data.length() & 0x7FFFFFFF), pcm, 5760, 0);
					    samples >= 0) {
						vr.reassign(&client, d.user_id, reinterpret_cast<uint8_t*>(pcm),
							samples * opus_channel_count * sizeof(opus_int16));
						client.end_gain = 1.0f / client.moving_average;
						park_count = audio_mix(client, *client.mixer, pcm_mix, pcm, park_count, samples, max_samples);
						client.creator->on_voice_receive.call(vr);
					}

					d.parked_payloads.pop();
				}
			}
		}

		/* If combined receive is bound, dispatch it */
		if (park_count) {
			
			/* Downsample the 32 bit samples back to 16 bit */
			opus_int16 pcm_downsample[23040] = { 0 };
			opus_int16* pcm_downsample_ptr = pcm_downsample;
			opus_int32* pcm_mix_ptr = pcm_mix;
			client.increment = (client.end_gain - client.current_gain) / static_cast<float>(samples);
			for (int64_t x = 0; x < (samples * opus_channel_count) / client.mixer->byte_blocks_per_register; ++x) {
				client.mixer->collect_single_register(pcm_mix_ptr, pcm_downsample_ptr, client.current_gain, client.increment);
				client.current_gain += client.increment * static_cast<float>(client.mixer->byte_blocks_per_register);
				pcm_mix_ptr += client.mixer->byte_blocks_per_register;
				pcm_downsample_ptr += client.mixer->byte_blocks_per_register;
			}

			voice_receive_t vr(nullptr, "", &client, 0, reinterpret_cast<uint8_t*>(pcm_downsample),
				max_samples * opus_channel_count * sizeof(opus_int16));

			client.creator->on_voice_receive_combined.call(vr);
		}
	}
#endif
}

discord_voice_client::discord_voice_client(dpp::cluster* _cluster, snowflake _channel_id, snowflake _server_id, const std::string &_token, const std::string &_session_id, const std::string &_host)
	   : websocket_client(_host.substr(0, _host.find(":")), _host.substr(_host.find(":") + 1, _host.length()), "/?v=4", OP_TEXT),
	runner(nullptr),
	connect_time(0),
	mixer(std::make_unique<audio_mixer>()),
	port(0),
	ssrc(0),
	timescale(1000000),
	paused(false),
	encoder(nullptr),
	repacketizer(nullptr),
	fd(INVALID_SOCKET),
	secret_key(nullptr),
	sequence(0),
	timestamp(0),
	last_timestamp(std::chrono::high_resolution_clock::now()),
	sending(false),
	tracks(0),
	creator(_cluster),
	terminating(false),
	heartbeat_interval(0),
	last_heartbeat(time(nullptr)),
	token(_token),
	sessionid(_session_id),
	server_id(_server_id),
	channel_id(_channel_id)
{
#if HAVE_VOICE
	if (!discord_voice_client::sodium_initialised) {
		if (sodium_init() < 0) {
			throw dpp::voice_exception(err_sodium, "discord_voice_client::discord_voice_client; sodium_init() failed");
		}
		discord_voice_client::sodium_initialised = true;
	}
	int opusError = 0;
	encoder = opus_encoder_create(opus_sample_rate_hz, opus_channel_count, OPUS_APPLICATION_VOIP, &opusError);
	if (opusError) {
		throw dpp::voice_exception(err_opus, "discord_voice_client::discord_voice_client; opus_encoder_create() failed");
	}
	repacketizer = opus_repacketizer_create();
	if (!repacketizer) {
		throw dpp::voice_exception(err_opus, "discord_voice_client::discord_voice_client; opus_repacketizer_create() failed");
	}
	try {
		this->connect();
	}
	catch (std::exception&) {
		cleanup();
		throw;
	}
#else
	throw dpp::voice_exception(err_no_voice_support, "Voice support not enabled in this build of D++");
#endif
}

discord_voice_client::~discord_voice_client()
{
	cleanup();
}

void discord_voice_client::cleanup()
{
	if (runner) {
		this->terminating = true;
		runner->join();
		delete runner;
		runner = nullptr;
	}
#if HAVE_VOICE
	if (encoder) {
		opus_encoder_destroy(encoder);
		encoder = nullptr;
	}
	if (repacketizer) {
		opus_repacketizer_destroy(repacketizer);
		repacketizer = nullptr;
	}
	if (voice_courier.joinable()) {
		{
			std::lock_guard lk(voice_courier_shared_state.mtx);
			voice_courier_shared_state.terminating = true;
		}
		voice_courier_shared_state.signal_iteration.notify_one();
		voice_courier.join();
	}
#endif
	delete[] secret_key;
	secret_key = nullptr;
}

bool discord_voice_client::is_ready() {
	return secret_key != nullptr;
}

bool discord_voice_client::is_playing() {
	std::lock_guard<std::mutex> lock(this->stream_mutex);
	return (!this->outbuf.empty());
}

void discord_voice_client::thread_run()
{
	utility::set_thread_name(std::string("vc/") + std::to_string(server_id));

	size_t times_looped = 0;
	time_t last_loop_time = time(nullptr);

	do {
		bool error = false;
		ssl_client::read_loop();
		ssl_client::close();

		time_t current_time = time(nullptr);
		/* Here, we check if it's been longer than 3 seconds since the previous loop,
		 * this gives us time to see if it's an actual disconnect, or an error.
		 * This will prevent us from looping too much, meaning error codes do not cause an infinite loop.
		 */
		if (current_time - last_loop_time >= 3)
			times_looped = 0;

		/* This does mean we'll always have times_looped at a minimum of 1, this is intended. */
		times_looped++;
		/* If we've looped 5 or more times, abort the loop. */
		if (times_looped >= 5) {
			log(dpp::ll_warning, "Reached max loops whilst attempting to read from the websocket. Aborting websocket.");
			break;
		}

		last_loop_time = current_time;

		if (!terminating) {
			log(dpp::ll_debug, "Attempting to reconnect the websocket...");
			do {
				try {
					ssl_client::connect();
					websocket_client::connect();
				}
				catch (const std::exception &e) {
					log(dpp::ll_error, std::string("Error establishing voice websocket connection, retry in 5 seconds: ") + e.what());
					ssl_client::close();
					std::this_thread::sleep_for(std::chrono::seconds(5));
					error = true;
				}
			} while (error && !terminating);
		}
	} while(!terminating);
}

void discord_voice_client::run()
{
	this->runner = new std::thread(&discord_voice_client::thread_run, this);
	this->thread_id = runner->native_handle();
}

int discord_voice_client::udp_send(const char* data, size_t length)
{
	sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(this->port);
	servaddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
	return (int) sendto(this->fd, data, (int)length, 0, (const sockaddr*)&servaddr, (int)sizeof(sockaddr_in));
}

int discord_voice_client::udp_recv(char* data, size_t max_length)
{
	return (int) recv(this->fd, data, (int)max_length, 0);
}

bool discord_voice_client::handle_frame(const std::string &data)
{
	log(dpp::ll_trace, std::string("R: ") + data);
	json j;
	
	try {
		j = json::parse(data);
	}
	catch (const std::exception &e) {
		log(dpp::ll_error, std::string("discord_voice_client::handle_frame ") + e.what() + ": " + data);
		return true;
	}

	if (j.find("op") != j.end()) {
		uint32_t op = j["op"];

		switch (op) {
			/* Client Disconnect */
			case 13: {
				if (j.find("d") != j.end() && j["d"].find("user_id") != j["d"].end() && !j["d"]["user_id"].is_null()) {
					snowflake u_id = snowflake_not_null(&j["d"], "user_id");
					auto it = std::find_if(ssrc_map.begin(), ssrc_map.end(),
					   [&u_id](const auto & p) { return p.second == u_id; });

					if (it != ssrc_map.end()) {
						ssrc_map.erase(it);
					}

					if (!creator->on_voice_client_disconnect.empty()) {
						voice_client_disconnect_t vcd(nullptr, data);
						vcd.voice_client = this;
						vcd.user_id = u_id;
						creator->on_voice_client_disconnect.call(vcd);
					}
				}
			}
			break;
			/* Speaking */ 
			case 5:
			/* Client Connect (doesn't seem to work) */
			case 12: {
				if (j.find("d") != j.end() 
					&& j["d"].find("user_id") != j["d"].end() && !j["d"]["user_id"].is_null()
					&& j["d"].find("ssrc") != j["d"].end() && !j["d"]["ssrc"].is_null() && j["d"]["ssrc"].is_number_integer()) {
					uint32_t u_ssrc = j["d"]["ssrc"].get<uint32_t>();
					snowflake u_id = snowflake_not_null(&j["d"], "user_id");
					ssrc_map[u_ssrc] = u_id;

					if (!creator->on_voice_client_speaking.empty()) {
						voice_client_speaking_t vcs(nullptr, data);
						vcs.voice_client = this;
						vcs.user_id = u_id;
						vcs.ssrc = u_ssrc;
						creator->on_voice_client_speaking.call(vcs);
					}
				}
			}
			break;
			/* Voice resume */
			case 9:
				log(ll_debug, "Voice connection resumed");
			break;
			/* Voice HELLO */
			case 8: {
				if (j.find("d") != j.end() && j["d"].find("heartbeat_interval") != j["d"].end() && !j["d"]["heartbeat_interval"].is_null()) {
					this->heartbeat_interval = j["d"]["heartbeat_interval"].get<uint32_t>();
				}

				if (!modes.empty()) {
					log(dpp::ll_debug, "Resuming voice session...");
						json obj = {
						{ "op", 7 },
						{
							"d",
							{
								{ "server_id", std::to_string(this->server_id) },
								{ "session_id", this->sessionid },
								{ "token", this->token },
							}
						}
					};
					this->write(obj.dump(-1, ' ', false, json::error_handler_t::replace));
				} else {
					log(dpp::ll_debug, "Connecting new voice session...");
						json obj = {
						{ "op", 0 },
						{
							"d",
							{
								{ "user_id", creator->me.id },
								{ "server_id", std::to_string(this->server_id) },
								{ "session_id", this->sessionid },
								{ "token", this->token },
							}
						}
					};
					this->write(obj.dump(-1, ' ', false, json::error_handler_t::replace));
				}
				this->connect_time = time(nullptr);
			}
			break;
			/* Session description */
			case 4: {
				json &d = j["d"];
				secret_key = new uint8_t[32];
				size_t ofs = 0;
				for (auto & c : d["secret_key"]) {
					*(secret_key + ofs) = (uint8_t)c;
					ofs++;
					if (ofs > 31) {
						break;
					}
				}

				/* This is needed to start voice receiving and make sure that the start of sending isn't cut off */
				send_silence(20);

				/* Fire on_voice_ready */
				if (!creator->on_voice_ready.empty()) {
					voice_ready_t rdy(nullptr, data);
					rdy.voice_client = this;
					rdy.voice_channel_id = this->channel_id;
					creator->on_voice_ready.call(rdy);
				}
			}
			break;
			/* Voice ready */
			case 2: {
				/* Video stream stuff comes in this frame too, but we can't use it (YET!) */
				json &d = j["d"];
				this->ip = d["ip"].get<std::string>();
				this->port = d["port"].get<uint16_t>();
				this->ssrc = d["ssrc"].get<uint64_t>();
				// Modes
				for (auto & m : d["modes"]) {
					this->modes.push_back(m.get<std::string>());
				}
				log(ll_debug, "Voice websocket established; UDP endpoint: " + ip + ":" + std::to_string(port) + " [ssrc=" + std::to_string(ssrc) + "] with " + std::to_string(modes.size()) + " modes");

				external_ip = discover_ip();

				dpp::socket newfd;
				if ((newfd = ::socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {

					sockaddr_in servaddr{};
					memset(&servaddr, 0, sizeof(sockaddr_in));
					servaddr.sin_family = AF_INET;
					servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
					servaddr.sin_port = htons(0);

					if (bind(newfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
						throw dpp::connection_exception(err_bind_failure, "Can't bind() client UDP socket");
					}
					
					if (!set_nonblocking(newfd, true)) {
						throw dpp::connection_exception(err_nonblocking_failure, "Can't switch voice UDP socket to non-blocking mode!");
					}

					/* Hook poll() in the ssl_client to add a new file descriptor */
					this->fd = newfd;
					this->custom_writeable_fd = std::bind(&discord_voice_client::want_write, this);
					this->custom_readable_fd = std::bind(&discord_voice_client::want_read, this);
					this->custom_writeable_ready = std::bind(&discord_voice_client::write_ready, this);
					this->custom_readable_ready = std::bind(&discord_voice_client::read_ready, this);

					int bound_port = 0;
					sockaddr_in sin;
					socklen_t len = sizeof(sin);
					if (getsockname(this->fd, (sockaddr *)&sin, &len) > -1) {
						bound_port = ntohs(sin.sin_port);
					}

					log(ll_debug, "External IP address: " + external_ip);

					this->write(json({
						{ "op", 1 },
							{ "d", {
								{ "protocol", "udp" },
								{ "data", {
										{ "address", external_ip },
										{ "port", bound_port },
										{ "mode", "xsalsa20_poly1305" }
									}
								}
							}
						}
					}).dump(-1, ' ', false, json::error_handler_t::replace));
				}
			}
			break;
		}
	}
	return true;
}

discord_voice_client& discord_voice_client::pause_audio(bool pause) {
	this->paused = pause;
	return *this;
}

bool discord_voice_client::is_paused() {
	return this->paused;
}

float discord_voice_client::get_secs_remaining() {
	std::lock_guard<std::mutex> lock(this->stream_mutex);
	float ret = 0;

	for (const auto& packet : outbuf) {
		ret += packet.duration * (timescale / 1000000000.0f);
	}

	return ret;
}

dpp::utility::uptime discord_voice_client::get_remaining() {
	float fp_secs = get_secs_remaining();
	return dpp::utility::uptime((time_t)ceil(fp_secs));
}

discord_voice_client& discord_voice_client::stop_audio() {
	std::lock_guard<std::mutex> lock(this->stream_mutex);
	outbuf.clear();
	track_meta.clear();
	tracks = 0;
	return *this;
}

void discord_voice_client::send(const char* packet, size_t len, uint64_t duration) {
	std::lock_guard<std::mutex> lock(this->stream_mutex);
	voice_out_packet frame;
	frame.packet = std::string(packet, len);
	frame.duration = duration;
	outbuf.emplace_back(frame);
}

void discord_voice_client::read_ready()
{
#ifdef HAVE_VOICE
	uint8_t buffer[65535];
	int r = this->udp_recv((char*)buffer, sizeof(buffer));

	if (r > 0 && (!creator->on_voice_receive.empty() || !creator->on_voice_receive_combined.empty())) {
		const std::basic_string_view<uint8_t> packet{buffer, static_cast<size_t>(r)};
		constexpr size_t header_size = 12;
		if (static_cast<size_t>(r) < header_size) {
			/* Invalid RTP payload */
			return;
		}

		/* It's a "silence packet" - throw it away. */
		if (packet.size() < 44) {
			return;
		}

		if (uint8_t payload_type = packet[1] & 0b0111'1111;
		    72 <= payload_type && payload_type <= 76) {
			/*
			 * This is an RTCP payload. Discord is known to send
			 * RTCP Receiver Reports.
			 *
			 * See https://datatracker.ietf.org/doc/html/rfc3551#section-6
			 */
			return;
		}

		voice_payload vp{0, // seq, populate later
		                 0, // timestamp, populate later
		                 std::make_unique<voice_receive_t>(nullptr, std::string((char*)buffer, r))};

		vp.vr->voice_client = this;

		{	/* Get the User ID of the speaker */
			uint32_t speaker_ssrc;
			std::memcpy(&speaker_ssrc, &packet[8], sizeof(uint32_t));
			speaker_ssrc = ntohl(speaker_ssrc);
			vp.vr->user_id = ssrc_map[speaker_ssrc];
		}

		/* Get the sequence number of the voice UDP packet */
		std::memcpy(&vp.seq, &packet[2], sizeof(rtp_seq_t));
		vp.seq = ntohs(vp.seq);
		/* Get the timestamp of the voice UDP packet */
		std::memcpy(&vp.timestamp, &packet[4], sizeof(rtp_timestamp_t));
		vp.timestamp = ntohl(vp.timestamp);

		/* Nonce is the RTP Header with zero padding */
		uint8_t nonce[24] = { 0 };
		std::memcpy(nonce, &packet[0], header_size);

		/* Get the number of CSRC in header */
		const size_t csrc_count = packet[0] & 0b0000'1111;
		/* Skip to the encrypted voice data */
		const ptrdiff_t offset_to_data = header_size + sizeof(uint32_t) * csrc_count;
		uint8_t* encrypted_data = buffer + offset_to_data;
		const size_t encrypted_data_len = r - offset_to_data;

		if (crypto_secretbox_open_easy(encrypted_data, encrypted_data,
		                               encrypted_data_len, nonce, secret_key)) {
			/* Invalid Discord RTP payload. */
			return;
		}

		std::basic_string_view<uint8_t> decrypted_data{encrypted_data, encrypted_data_len - crypto_box_MACBYTES};
		if (const bool uses_extension [[maybe_unused]] = (packet[0] >> 4) & 0b0001) {
			/* Skip the RTP Extensions */
			size_t ext_len = 0;
			{
				uint16_t ext_len_in_words;
				memcpy(&ext_len_in_words, &decrypted_data[2], sizeof(uint16_t));
				ext_len_in_words = ntohs(ext_len_in_words);
				ext_len = sizeof(uint32_t) * ext_len_in_words;
			}
			constexpr size_t ext_header_len = sizeof(uint16_t) * 2;
			decrypted_data = decrypted_data.substr(ext_header_len + ext_len);
		}

		/*
		 * We're left with the decrypted, opus-encoded data.
		 * Park the payload and decode on the voice courier thread.
		 */
		vp.vr->audio_data.assign(decrypted_data);

		{
			std::lock_guard lk(voice_courier_shared_state.mtx);
			auto& [range, payload_queue, pending_decoder_ctls, decoder] = voice_courier_shared_state.parked_voice_payloads[vp.vr->user_id];

			if (!decoder) {
				/*
				 * Most likely this is the first time we encounter this speaker.
				 * Do some initialization for not only the decoder but also the range.
				 */
				range.min_seq = vp.seq;
				range.min_timestamp = vp.timestamp;

				int opus_error = 0;
				decoder.reset(opus_decoder_create(opus_sample_rate_hz, opus_channel_count, &opus_error),
				              &opus_decoder_destroy);
				if (opus_error) {
					/**
					 * NOTE: The -10 here makes the opus_error match up with values of exception_error_code,
					 * which would otherwise conflict as every C library loves to use values from -1 downwards.
					 */
					throw dpp::voice_exception((exception_error_code)(opus_error - 10), "discord_voice_client::discord_voice_client; opus_decoder_create() failed");
				}
			}

			if (vp.seq < range.min_seq && vp.timestamp < range.min_timestamp) {
				/* This packet arrived too late. We can only discard it. */
				return;
			}
			range.max_seq = vp.seq;
			range.max_timestamp = vp.timestamp;
			payload_queue.push(std::move(vp));
		}

		voice_courier_shared_state.signal_iteration.notify_one();

		if (!voice_courier.joinable()) {
			/* Courier thread is not running, start it */
			voice_courier = std::thread(&voice_courier_loop,
			                            std::ref(*this),
			                            std::ref(voice_courier_shared_state));
		}
	}
#else
	throw dpp::voice_exception(err_no_voice_support, "Voice support not enabled in this build of D++");
#endif
}

void discord_voice_client::write_ready()
{
	uint64_t duration = 0;
	bool track_marker_found = false;
	uint64_t bufsize = 0;
	send_audio_type_t type = satype_recorded_audio; 
	{
		std::lock_guard<std::mutex> lock(this->stream_mutex);
		if (!this->paused && outbuf.size()) {
			type = send_audio_type;
			if (outbuf[0].packet.size() == sizeof(uint16_t) && (*((uint16_t*)(outbuf[0].packet.data()))) == AUDIO_TRACK_MARKER) {
				outbuf.erase(outbuf.begin());
				track_marker_found = true;
				if (tracks > 0) {
					tracks--;
				}
			}
			if (outbuf.size()) {
				if (this->udp_send(outbuf[0].packet.data(), outbuf[0].packet.length()) == (int)outbuf[0].packet.length()) {
					duration = outbuf[0].duration * timescale;
					bufsize = outbuf[0].packet.length();
					outbuf.erase(outbuf.begin());
				}
			}
		}
	}
	if (duration) {
		if (type == satype_recorded_audio) {
			std::chrono::nanoseconds latency = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - last_timestamp);
			std::chrono::nanoseconds sleep_time = std::chrono::nanoseconds(duration) - latency;
			if (sleep_time.count() > 0) {
				std::this_thread::sleep_for(sleep_time);
			}
		}
		else if (type == satype_overlap_audio) {
			std::chrono::nanoseconds latency = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - last_timestamp);
			std::chrono::nanoseconds sleep_time = std::chrono::nanoseconds(duration) + last_sleep_remainder - latency;
			std::chrono::nanoseconds sleep_increment = (std::chrono::nanoseconds(duration) - latency) / AUDIO_OVERLAP_SLEEP_SAMPLES;
			if (sleep_time.count() > 0) {
				uint16_t samples_count = 0;
				std::chrono::nanoseconds overshoot_accumulator{};

				do {
					std::chrono::high_resolution_clock::time_point start_sleep = std::chrono::high_resolution_clock::now();
					std::this_thread::sleep_for(sleep_increment);
					std::chrono::high_resolution_clock::time_point end_sleep = std::chrono::high_resolution_clock::now();

					samples_count++;
					overshoot_accumulator += std::chrono::duration_cast<std::chrono::nanoseconds>(end_sleep - start_sleep) - sleep_increment;
					sleep_time -= std::chrono::duration_cast<std::chrono::nanoseconds>(end_sleep - start_sleep);
				} while (std::chrono::nanoseconds(overshoot_accumulator.count() / samples_count) + sleep_increment < sleep_time);
				last_sleep_remainder = sleep_time;
			} else {
				last_sleep_remainder = std::chrono::nanoseconds(0);
			}
		}

		last_timestamp = std::chrono::high_resolution_clock::now();
		if (!creator->on_voice_buffer_send.empty()) {
			voice_buffer_send_t snd(nullptr, "");
			snd.buffer_size = (int)bufsize;
			snd.voice_client = this;
			creator->on_voice_buffer_send.call(snd);
		}
	}
	if (track_marker_found) {
		if (!creator->on_voice_track_marker.empty()) {
			voice_track_marker_t vtm(nullptr, "");
			vtm.voice_client = this;
			{
				std::lock_guard<std::mutex> lock(this->stream_mutex);
				if (!track_meta.empty()) {
					vtm.track_meta = track_meta[0];
					track_meta.erase(track_meta.begin());
				}
			}
			creator->on_voice_track_marker.call(vtm);
		}
	}
}

dpp::utility::uptime discord_voice_client::get_uptime()
{
	return dpp::utility::uptime(time(nullptr) - connect_time);
}

bool discord_voice_client::is_connected()
{
	return (this->get_state() == CONNECTED);
}

dpp::socket discord_voice_client::want_write() {
	std::lock_guard<std::mutex> lock(this->stream_mutex);
	if (!this->paused && !outbuf.empty()) {
		return fd;
	} else {
		return INVALID_SOCKET;
	}
}

dpp::socket discord_voice_client::want_read() {
	return fd;
}

void discord_voice_client::error(uint32_t errorcode)
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
		{ 4001, "Unknown opcode" },
		{ 4002, "Failed to decode payload" },
		{ 4003, "Not authenticated" },
		{ 4004, "Authentication failed" },
		{ 4005, "Already authenticated" },
		{ 4006, "Session no longer valid" },
		{ 4009, "Session timeout" },
		{ 4011, "Server not found" },
		{ 4012, "Unknown protocol" },
		{ 4014, "Disconnected" },
		{ 4015, "Voice server crashed" },
		{ 4016, "Unknown encryption mode" }
	};
	std::string error = "Unknown error";
	auto i = errortext.find(errorcode);
	if (i != errortext.end()) {
		error = i->second;
	}
	log(dpp::ll_warning, "Voice session error: " + std::to_string(errorcode) + " on channel " + std::to_string(channel_id) + ": " + error);

	/* Errors 4004...4016 except 4014 are fatal and cause termination of the voice session */
	if (errorcode >= 4003) {
		stop_audio();
		this->terminating = true;
		log(dpp::ll_error, "This is a non-recoverable error, giving up on voice connection");
	}
}

void discord_voice_client::set_user_gain(snowflake user_id, float factor)
{
#ifdef HAVE_VOICE
	int16_t gain;

	if (factor < 0.0f) {
		/* Invalid factor; must be nonnegative. */
		return;
	} else if (factor == 0.0f) {
		/*
		 * Client probably wants to mute the user,
		 * but log10(0) is undefined, so let's
		 * hardcode the gain to the Opus minimum
		 * for clients.
		 */
		gain = -32768;
	} else {
		/*
		 * OPUS_SET_GAIN takes a value (x) in Q8 dB units.
		 * factor = 10^(x / (20 * 256))
		 * x = log_10(factor) * 20 * 256
		 */
		gain = static_cast<int16_t>(std::log10(factor) * 20.0f * 256.0f);
	}

	std::lock_guard lk(voice_courier_shared_state.mtx);

	voice_courier_shared_state
		/*
		 * Use of the modifying operator[] is intentional;
		 * this is so that we can set ctls on the decoder
		 * even before the user speaks. The decoder doesn't
		 * even have to be ready now, and the setting doesn't
		 * actually take place until we receive some voice
		 * from that speaker.
		 */
		.parked_voice_payloads[user_id]
		.pending_decoder_ctls.push_back([gain](OpusDecoder& decoder) {
			opus_decoder_ctl(&decoder, OPUS_SET_GAIN(gain));
		});
#endif
}

void discord_voice_client::log(dpp::loglevel severity, const std::string &msg) const
{
	creator->log(severity, msg);
}

void discord_voice_client::queue_message(const std::string &j, bool to_front)
{
	std::unique_lock locker(queue_mutex);
	if (to_front) {
		message_queue.emplace_front(j);
	} else {
		message_queue.emplace_back(j);
	}
}

void discord_voice_client::clear_queue()
{
	std::unique_lock locker(queue_mutex);
	message_queue.clear();
}

size_t discord_voice_client::get_queue_size()
{
	std::shared_lock locker(queue_mutex);
	return message_queue.size();
}

const std::vector<std::string> discord_voice_client::get_marker_metadata() {
	std::shared_lock locker(queue_mutex);
	return track_meta;
}

void discord_voice_client::one_second_timer()
{
	if (terminating) {
		throw dpp::connection_exception(err_voice_terminating, "Terminating voice connection");
	}
	/* Rate limit outbound messages, 1 every odd second, 2 every even second */
	if (this->get_state() == CONNECTED) {
		for (int x = 0; x < (time(nullptr) % 2) + 1; ++x) {
			std::unique_lock locker(queue_mutex);
			if (!message_queue.empty()) {
				std::string message = message_queue.front();
				message_queue.pop_front();
				this->write(message);
			}
		}

		if (this->heartbeat_interval) {
			/* Check if we're due to emit a heartbeat */
			if (time(nullptr) > last_heartbeat + ((heartbeat_interval / 1000.0) * 0.75)) {
				queue_message(json({{"op", 3}, {"d", rand()}}).dump(-1, ' ', false, json::error_handler_t::replace), true);
				last_heartbeat = time(nullptr);
			}
		}
	}
}

size_t discord_voice_client::encode(uint8_t *input, size_t inDataSize, uint8_t *output, size_t &outDataSize)
{
#if HAVE_VOICE
	outDataSize = 0;
	int mEncFrameBytes = 11520;
	int mEncFrameSize = 2880;
	if (0 == (inDataSize % mEncFrameBytes)) {
		bool isOk = true;
		uint8_t *out = encode_buffer;

		memset(out, 0, sizeof(encode_buffer));
		repacketizer = opus_repacketizer_init(repacketizer);
		if (!repacketizer) {
			log(ll_warning, "opus_repacketizer_init(): failure");
			return outDataSize;
		}
		for (size_t i = 0; i < (inDataSize / mEncFrameBytes); ++ i) {
			const opus_int16* pcm = (opus_int16*)(input + i * mEncFrameBytes);
			int ret = opus_encode(encoder, pcm, mEncFrameSize, out, 65536);
			if (ret > 0) {
				int retval = opus_repacketizer_cat(repacketizer, out, ret);
				if (retval != OPUS_OK) {
					isOk = false;
					log(ll_warning, "opus_repacketizer_cat(): " + std::string(opus_strerror(retval)));
					break;
				}
				out += ret;
			} else {
				isOk = false;
					log(ll_warning, "opus_encode(): " + std::string(opus_strerror(ret)));
				break;
			}
		}
		if (isOk) {
			int ret = opus_repacketizer_out(repacketizer, output, 65536);
			if (ret > 0) {
				outDataSize = ret;
			} else {
				log(ll_warning, "opus_repacketizer_out(): " + std::string(opus_strerror(ret)));
			}
		}
	} else {
		throw dpp::voice_exception(err_invalid_voice_packet_length, "Invalid input data length: " + std::to_string(inDataSize) + ", must be n times of " + std::to_string(mEncFrameBytes));
	}
#else
	throw dpp::voice_exception(err_no_voice_support, "Voice support not enabled in this build of D++");
#endif
	return outDataSize;
}

discord_voice_client& discord_voice_client::insert_marker(const std::string& metadata) {
	/* Insert a track marker. A track marker is a single 16 bit value of 0xFFFF.
	 * This is too small to be a valid RTP packet so the send function knows not
	 * to actually send it, and instead to skip it
	 */
	uint16_t tm = AUDIO_TRACK_MARKER;
	this->send((const char*)&tm, sizeof(uint16_t), 0);
	{
		std::lock_guard<std::mutex> lock(this->stream_mutex);
		track_meta.push_back(metadata);
		tracks++;
	}
	return *this;
}

uint32_t discord_voice_client::get_tracks_remaining() {
	std::lock_guard<std::mutex> lock(this->stream_mutex);
	if (outbuf.empty()) {
		return 0;
	} else {
		return tracks + 1;
	}
}

discord_voice_client& discord_voice_client::skip_to_next_marker() {
	std::lock_guard<std::mutex> lock(this->stream_mutex);
	if (!outbuf.empty()) {
		/* Find the first marker to skip to */
		auto i = std::find_if(outbuf.begin(), outbuf.end(), [](const voice_out_packet &v){
			return v.packet.size() == sizeof(uint16_t) && (*((uint16_t*)(v.packet.data()))) == AUDIO_TRACK_MARKER;
		});

		if (i != outbuf.end()) {
			/* Skip queued packets until including found marker */
			outbuf.erase(outbuf.begin(), i+1);
		} else {
			/* No market found, skip the whole queue */
			outbuf.clear();
		}
	}

	if (tracks > 0) {
		tracks--;
	}

	if (!track_meta.empty()) {
		track_meta.erase(track_meta.begin());
	}

	return *this;
}

discord_voice_client& discord_voice_client::send_silence(const uint64_t duration) {
	uint8_t silence_packet[3] = { 0xf8, 0xff, 0xfe };
	send_audio_opus(silence_packet, 3, duration);
	return *this;
}

discord_voice_client& discord_voice_client::set_send_audio_type(send_audio_type_t type)
{
	{
		std::lock_guard<std::mutex> lock(this->stream_mutex);
		send_audio_type = type;
	}
	return *this;
}

discord_voice_client& discord_voice_client::send_audio_raw(uint16_t* audio_data, const size_t length)  {
#if HAVE_VOICE
	if (length < 4) {
		throw dpp::voice_exception(err_invalid_voice_packet_length, "Raw audio packet size can't be less than 4");
	}

	if ((length % 4) != 0) {
		throw dpp::voice_exception(err_invalid_voice_packet_length, "Raw audio packet size should be divisible by 4");
	}

	if (length > send_audio_raw_max_length) {
		std::string s_audio_data((const char*)audio_data, length);

		while (s_audio_data.length() > send_audio_raw_max_length) {
			std::string packet(s_audio_data.substr(0, send_audio_raw_max_length));
			const auto packet_size = static_cast<ptrdiff_t>(packet.size());

			s_audio_data.erase(s_audio_data.begin(), s_audio_data.begin() + packet_size);

			send_audio_raw((uint16_t*)packet.data(), packet_size);
		}

		return *this;
	}

	if (length < send_audio_raw_max_length) {
		std::string packet((const char*)audio_data, length);
		packet.resize(send_audio_raw_max_length, 0);

		return send_audio_raw((uint16_t*)packet.data(), packet.size());
	}

	opus_int32 encodedAudioMaxLength = (opus_int32)length;
	std::vector<uint8_t> encodedAudioData(encodedAudioMaxLength);
	size_t encodedAudioLength = encodedAudioMaxLength;

	encodedAudioLength = this->encode((uint8_t*)audio_data, length, encodedAudioData.data(), encodedAudioLength);

	send_audio_opus(encodedAudioData.data(), encodedAudioLength);
#else
	throw dpp::voice_exception(err_no_voice_support, "Voice support not enabled in this build of D++");
#endif
	return *this;
}

discord_voice_client& discord_voice_client::send_audio_opus(uint8_t* opus_packet, const size_t length) {
#if HAVE_VOICE
	int samples = opus_packet_get_nb_samples(opus_packet, (opus_int32)length, opus_sample_rate_hz);
	uint64_t duration = (samples / 48) / (timescale / 1000000);
	send_audio_opus(opus_packet, length, duration);
#else
	throw dpp::voice_exception(err_no_voice_support, "Voice support not enabled in this build of D++");
#endif
	return *this;
}

discord_voice_client& discord_voice_client::send_audio_opus(uint8_t* opus_packet, const size_t length, uint64_t duration) {
#if HAVE_VOICE
	int frameSize = (int)(48 * duration * (timescale / 1000000));
	opus_int32 encodedAudioMaxLength = (opus_int32)length;
	std::vector<uint8_t> encodedAudioData(encodedAudioMaxLength);
	size_t encodedAudioLength = encodedAudioMaxLength;

	encodedAudioLength = length;
	encodedAudioData.reserve(length);
	memcpy(encodedAudioData.data(), opus_packet, length);

	++sequence;
	const int nonceSize = 24;
	rtp_header header(sequence, timestamp, (uint32_t)ssrc);

	int8_t nonce[nonceSize];
	std::memcpy(nonce, &header, sizeof(header));
	std::memset(nonce + sizeof(header), 0, sizeof(nonce) - sizeof(header));

	std::vector<uint8_t> audioDataPacket(sizeof(header) + encodedAudioLength + crypto_secretbox_MACBYTES);
	std::memcpy(audioDataPacket.data(), &header, sizeof(header));

	crypto_secretbox_easy(audioDataPacket.data() + sizeof(header), encodedAudioData.data(), encodedAudioLength, (const unsigned char*)nonce, secret_key);

	this->send((const char*)audioDataPacket.data(), audioDataPacket.size(), duration);
	timestamp += frameSize;

	speak();
#else
	throw dpp::voice_exception(err_no_voice_support, "Voice support not enabled in this build of D++");
#endif
	return *this;
}

discord_voice_client& discord_voice_client::speak() {
	if (!this->sending) {
		this->queue_message(json({
		{"op", 5},
		{"d", {
			{"speaking", 1},
			{"delay", 0},
			{"ssrc", ssrc}
		}}
		}).dump(-1, ' ', false, json::error_handler_t::replace), true);
		sending = true;
	}
	return *this;
}

discord_voice_client& discord_voice_client::set_timescale(uint64_t new_timescale) {
	timescale = new_timescale;
	return *this;
}

uint64_t discord_voice_client::get_timescale() {
	return timescale;
}

std::string discord_voice_client::discover_ip() {
	dpp::socket newfd = SOCKET_ERROR;
	unsigned char packet[74] = { 0 };
	(*(uint16_t*)(packet)) = htons(0x01);
	(*(uint16_t*)(packet + 2)) = htons(70);
	(*(uint32_t*)(packet + 4)) = htonl((uint32_t)this->ssrc);
	if ((newfd = ::socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
		sockaddr_in servaddr{};
		memset(&servaddr, 0, sizeof(sockaddr_in));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(0);
		if (bind(newfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
			log(ll_warning, "Could not bind socket for IP discovery");
			return "";
		}
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(this->port);
		servaddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
		if (::connect(newfd, (const sockaddr*)&servaddr, sizeof(sockaddr_in)) < 0) {
			log(ll_warning, "Could not connect socket for IP discovery");
			return "";
		}
		if (::send(newfd, (const char*)packet, 74, 0) == -1) {
			log(ll_warning, "Could not send packet for IP discovery");
			return "";
		}
		if (recv(newfd, (char*)packet, 74, 0) == -1) {
			log(ll_warning, "Could not receive packet for IP discovery");
			return "";
		}

		close_socket(newfd);

		//utility::debug_dump(packet, 74);
		return std::string((const char*)(packet + 8));
	}
	return "";
}

discord_voice_client& discord_voice_client::set_iteration_interval(uint16_t interval) {
	this->iteration_interval = interval;
	return *this;
}

uint16_t discord_voice_client::get_iteration_interval() {
	return this->iteration_interval;
}

} // namespace dpp
