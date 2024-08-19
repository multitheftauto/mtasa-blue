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
#pragma once
#include <dpp/export.h>
#include <string>
#include <map>
#include <vector>
#include <variant>
#include <dpp/sslclient.h>

namespace dpp {

/**
 * @brief Websocket protocol types available on Discord
 */
enum websocket_protocol_t : uint8_t {
	/**
	 * @brief JSON data, text, UTF-8 character set
	 */
	ws_json = 0,
	
	/**
	 * @brief Erlang Term Format (ETF) binary protocol
	 */
	ws_etf = 1
};

/**
 * @brief Websocket connection status
 */
enum ws_state : uint8_t {
	/**
	 * @brief Sending/receiving HTTP headers, acting as a standard HTTP connection.
	 * This is the state prior to receiving "HTTP/1.1 101 Switching Protocols" from the
	 * server side.
	 */
	HTTP_HEADERS,

	/**
	 * @brief Connected as a websocket, and "upgraded". Now talking using binary frames.
	 */
	CONNECTED
};

/**
 * @brief Low-level websocket opcodes for frames
 */
enum ws_opcode : uint8_t {
	/**
	 * @brief Continuation.
	 */
        OP_CONTINUATION = 0x00,

	/**
	 * @brief Text frame.
	 */
        OP_TEXT = 0x01,

	/**
	 * @brief Binary frame.
	 */
        OP_BINARY = 0x02,

	/**
	 * @brief Close notification with close code.
	 */
        OP_CLOSE = 0x08,

	/**
	 * @brief Low level ping.
	 */
        OP_PING = 0x09,

	/**
	 * @brief Low level pong.
	 */
        OP_PONG = 0x0a
};

/**
 * @brief Implements a websocket client based on the SSL client
 */
class DPP_EXPORT websocket_client : public ssl_client {
	/**
	 * @brief Connection key used in the HTTP headers
	 */
	std::string key;

	/**
	 * @brief Current websocket state
	 */
	ws_state state;

	/**
	 * @brief Path part of URL for websocket
	 */
	std::string path;

	/**
	 * @brief Data opcode, represents the type of frames we send
	 */
	ws_opcode data_opcode;

	/**
	 * @brief HTTP headers received on connecting/upgrading
	 */
	std::map<std::string, std::string> http_headers;

	/**
	 * @brief Parse headers for a websocket frame from the buffer.
	 * @param buffer The buffer to operate on. Will modify the string removing completed items from the head of the queue
	 * @return true if a complete header has been received
	 */
	bool parseheader(std::string &buffer);

	/**
	 * @brief Unpack a frame and pass completed frames up the stack.
	 * @param buffer The buffer to operate on. Gets modified to remove completed frames on the head of the buffer
	 * @param offset The offset to start at (reserved for future use)
	 * @param first True if is the first element (reserved for future use)
	 * @return true if a complete frame has been received
	 */
	bool unpack(std::string &buffer, uint32_t offset, bool first = true);

	/**
	 * @brief Fill a header for outbound messages
	 * @param outbuf The raw frame to fill
	 * @param sendlength The size of the data to encapsulate
	 * @param opcode the ws_opcode to send in the header
	 * @return size of filled header
	 */
	size_t fill_header(unsigned char* outbuf, size_t sendlength, ws_opcode opcode);

	/**
	 * @brief Handle ping and pong requests.
	 * @param ping True if this is a ping, false if it is a pong 
	 * @param payload The ping payload, to be returned as-is for a ping
	 */
	void handle_ping_pong(bool ping, const std::string &payload);

protected:

	/**
	 * @brief (Re)connect
	 */
	virtual void connect();

	/**
	 * @brief Get websocket state
	 * @return websocket state
	 */
	ws_state get_state();

public:

	/**
	 * @brief Connect to a specific websocket server.
	 * @param hostname Hostname to connect to
	 * @param port Port to connect to
	 * @param urlpath The URL path components of the HTTP request to send
	 * @param opcode The encoding type to use, either OP_BINARY or OP_TEXT
	 * @note Voice websockets only support OP_TEXT, and other websockets must be
	 * OP_BINARY if you are going to send ETF.
	 */
        websocket_client(const std::string &hostname, const std::string &port = "443", const std::string &urlpath = "", ws_opcode opcode = OP_BINARY);

	/**
	 * @brief Destroy the websocket client object
	 */
        virtual ~websocket_client() = default;

	/**
	 * @brief Write to websocket. Encapsulates data in frames if the status is CONNECTED.
	 * @param data The data to send.
	 */
        virtual void write(const std::string &data);

	/**
	 * @brief Processes incoming frames from the SSL socket input buffer.
	 * @param buffer The buffer contents. Can modify this value removing the head elements when processed.
	 */
        virtual bool handle_buffer(std::string &buffer);

	/**
	 * @brief Close websocket
	 */
        virtual void close();

	/**
	 * @brief Receives raw frame content only without headers
	 * 
	 * @param buffer The buffer contents
	 * @return True if the frame was successfully handled. False if no valid frame is in the buffer.
	 */
	virtual bool handle_frame(const std::string &buffer);

	/**
	 * @brief Called upon error frame.
	 * 
	 * @param errorcode The error code from the websocket server
	 */
	virtual void error(uint32_t errorcode);

	/**
	 * @brief Fires every second from the underlying socket I/O loop, used for sending websocket pings
	 */
	virtual void one_second_timer();

	/**
	 * @brief Send OP_CLOSE error code 1000 to the other side of the connection.
	 * This indicates graceful close.
	 */
	void send_close_packet();
};

} // namespace dpp
