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
#include <dpp/misc-enum.h>
#include <string>
#include <functional>
#include <dpp/socket.h>
#include <cstdint>

namespace dpp {

/**
 * @brief This is an opaque class containing openssl library specific structures.
 * We define it this way so that the public facing D++ library doesn't require
 * the openssl headers be available to build against it.
 */
class openssl_connection;

/**
 * @brief A callback for socket status
 */
typedef std::function<dpp::socket()> socket_callback_t;

/**
 * @brief A socket notification callback
 */
typedef std::function<void()> socket_notification_t;

/**
 * @brief Close a socket 
 * 
 * @param sfd Socket to close
 * @return false on error, true on success
 */
bool close_socket(dpp::socket sfd);

/**
 * @brief Set a socket to blocking or non-blocking IO
 *
 * @param sockfd socket to act upon
 * @param non_blocking should socket be non-blocking?
 * @return false on error, true on success
 */
bool set_nonblocking(dpp::socket sockfd, bool non_blocking);

/**
 * @brief Implements a simple non-blocking SSL stream client.
 * 
 * @note although the design is non-blocking the run() method will
 * execute in an infinite loop until the socket disconnects. This is intended
 * to be run within a std::thread.
 */
class DPP_EXPORT ssl_client
{
private:
	/**
	 * @brief Clean up resources
	 */
	void cleanup();
protected:
	/**
	 * @brief Input buffer received from socket
	 */
	std::string buffer;

	/**
	 * @brief Output buffer for sending to socket
	 */
	std::string obuffer;

	/**
	 * @brief True if in nonblocking mode. The socket switches to nonblocking mode
	 * once ReadLoop is called.
	 */
	bool nonblocking;

	/**
	 * @brief Raw file descriptor of connection
	 */
	dpp::socket sfd;

	/**
	 * @brief Openssl opaque contexts
	 */
	openssl_connection* ssl;

	/**
	 * @brief SSL cipher in use
	 */
	std::string cipher;

	/**
	 * @brief For timers
	 */
	time_t last_tick;

	/**
	 * @brief Hostname connected to
	 */
	std::string hostname;

	/**
	 * @brief Port connected to
	 */
	std::string port;

	/**
	 * @brief Bytes out
	 */
	uint64_t bytes_out;

	/**
	 * @brief Bytes in
	 */
	uint64_t bytes_in;

	/**
	 * @brief True for a plain text connection
	 */
	bool plaintext;

	/**
	 * @brief True if we are establishing a new connection, false if otherwise.
	 */
	bool make_new;


	/**
	 * @brief Called every second
	 */
	virtual void one_second_timer();

	/**
	 * @brief Start SSL connection and connect to TCP endpoint
	 * @throw dpp::exception Failed to initialise connection
	 */
	virtual void connect();
public:
	/**
	 * @brief Get the bytes out objectGet total bytes sent
	 * @return uint64_t bytes sent
	 */
	uint64_t get_bytes_out();
	
	/**
	 * @brief Get total bytes received
	 * @return uint64_t bytes received
	 */
	uint64_t get_bytes_in();

	/**
	 * @brief Get SSL cipher name
	 * @return std::string ssl cipher name
	 */
	std::string get_cipher();

	/**
	 * @brief Attaching an additional file descriptor to this function will send notifications when there is data to read.
	 * 
	 * NOTE: Only hook this if you NEED it as it can increase CPU usage of the thread!
	 * Returning -1 means that you don't want to be notified.
	 */
	socket_callback_t custom_readable_fd;

	/**
	 * @brief Attaching an additional file descriptor to this function will send notifications when you are able to write
	 * to the socket.
	 * 
	 * NOTE: Only hook this if you NEED it as it can increase CPU usage of the thread! You should toggle this
	 * to -1 when you do not have anything to write otherwise it'll keep triggering repeatedly (it is level triggered).
	 */
	socket_callback_t custom_writeable_fd;

	/**
	 * @brief This event will be called when you can read from the custom fd
	 */
	socket_notification_t custom_readable_ready;

	/**
	 * @brief This event will be called when you can write to a custom fd
	 */
	socket_notification_t custom_writeable_ready;

	/**
	 * @brief True if we are keeping the connection alive after it has finished
	 */
	bool keepalive;

	/**
	 * @brief Connect to a specified host and port. Throws std::runtime_error on fatal error.
	 * @param _hostname The hostname to connect to
	 * @param _port the Port number to connect to
	 * @param plaintext_downgrade Set to true to connect using plaintext only, without initialising SSL.
	 * @param reuse Attempt to reuse previous connections for this hostname and port, if available
	 * Note that no Discord endpoints will function when downgraded. This option is provided only for
	 * connection to non-Discord addresses such as within dpp::cluster::request().
	 * @throw dpp::exception Failed to initialise connection
	 */
	ssl_client(const std::string &_hostname, const std::string &_port = "443", bool plaintext_downgrade = false, bool reuse = false);

	/**
	 * @brief Nonblocking I/O loop
	 * @throw std::exception Any std::exception (or derivative) thrown from read_loop() causes reconnection of the shard
	 */
	void read_loop();

	/**
	 * @brief Destroy the ssl_client object
	 */
	virtual ~ssl_client();

	/**
	 * @brief Handle input from the input buffer. This function will be called until
	 * all data in the buffer has been processed and the buffer is empty.
	 * @param buffer the buffer content. Will be modified removing any processed front elements
	 * @return bool True if the socket should remain connected
	 */
	virtual bool handle_buffer(std::string &buffer);

	/**
	 * @brief Write to the output buffer.
	 * @param data Data to be written to the buffer
	 * @note The data may not be written immediately and may be written at a later time to the socket.
	 */
	virtual void write(const std::string &data);

	/**
	 * @brief Close socket connection
	 */
	virtual void close();

	/**
	 * @brief Log a message
	 * @param severity severity of log message
	 * @param msg Log message to send
	 */
	virtual void log(dpp::loglevel severity, const std::string &msg) const;
};

} // namespace dpp
