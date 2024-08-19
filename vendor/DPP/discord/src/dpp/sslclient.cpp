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
#include <cerrno>
#ifdef _WIN32
	/* Windows-specific sockets includes */
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <io.h>
	/* Windows doesn't have standard poll(), it has WSAPoll.
	 * It's the same thing with different symbol names.
	 * Microsoft gotta be different.
	 */
	#define poll(fds, nfds, timeout) WSAPoll(fds, nfds, timeout)
	#define pollfd WSAPOLLFD
	/* Windows sockets library */
	#pragma comment(lib, "ws2_32")
#else
	/* Anyting other than Windows (e.g. sane OSes) */
	#include <poll.h>
	#include <netinet/in.h>
	#include <resolv.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <netinet/tcp.h>
	#include <unistd.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
/* Windows specific OpenSSL symbol weirdness */
#ifdef OPENSSL_SYS_WIN32
	#undef X509_NAME
	#undef X509_EXTENSIONS
	#undef X509_CERT_PAIR
	#undef PKCS7_ISSUER_AND_SERIAL
	#undef OCSP_REQUEST
	#undef OCSP_RESPONSE
#endif
#include <exception>
#include <string>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <dpp/sslclient.h>
#include <dpp/exception.h>
#include <dpp/utility.h>
#include <dpp/dns.h>

/* Maximum allowed time in milliseconds for socket read/write timeouts and connect() */
#define SOCKET_OP_TIMEOUT 5000

namespace dpp {

/**
 * @brief This is an opaque class containing openssl library specific structures.
 * We define it this way so that the public facing D++ library doesn't require
 * the openssl headers be available to build against it.
 */
class openssl_connection {
public:
	/**
	 * @brief OpenSSL session
	 */
	SSL* ssl;
};

/**
 * @brief Keepalive cache record
 */
struct keepalive_cache_t {
	time_t created;
	openssl_connection* ssl;
	dpp::socket sfd;
};

/**
 * @brief Custom deleter for SSL_CTX
 */
class openssl_context_deleter {
public:
	void operator()(SSL_CTX* context) const noexcept {
		SSL_CTX_free(context);
	}
};

/**
 * @brief OpenSSL context
 */
thread_local std::unique_ptr<SSL_CTX, openssl_context_deleter> openssl_context;

/**
 * @brief Keepalive sessions, per-thread
 */
thread_local std::unordered_map<std::string, keepalive_cache_t> keepalives;

/* You'd think that we would get better performance with a bigger buffer, but SSL frames are 16k each.
 * SSL_read in non-blocking mode will only read 16k at a time. There's no point in a bigger buffer as
 * it'd go unused.
 */
#define DPP_BUFSIZE 16 * 1024

/* Represents a failed socket system call, e.g. connect() failure */
const int ERROR_STATUS = -1;

bool close_socket(dpp::socket sfd)
{
	/* close_socket on an error socket is a non-op */
	if (sfd != INVALID_SOCKET) {
		shutdown(sfd, 2);
#ifdef _WIN32
		return closesocket(sfd) == 0;
#else
		return ::close(sfd) == 0;
#endif
	}
	return false;
}

bool set_nonblocking(dpp::socket sockfd, bool non_blocking)
{
#ifdef _WIN32
	u_long mode = non_blocking ? 1 : 0;
	int result = ioctlsocket(sockfd, FIONBIO, &mode);
	if (result != NO_ERROR) {
		return false;
	}
#else
	int ofcmode = fcntl(sockfd, F_GETFL, 0);
	if (non_blocking) {
		ofcmode |= O_NDELAY;
	} else {
		ofcmode &= ~O_NDELAY;
	}
	if (fcntl(sockfd, F_SETFL, ofcmode)) {
		return false;
	}
#endif
	return true;
}

/**
 * @brief Connect to TCP socket with a poll() driven timeout
 * 
 * @param sockfd socket descriptor
 * @param addr address to connect to
 * @param addrlen address length
 * @param timeout_ms timeout in milliseconds
 * @return int -1 on error, 0 on succcess just like POSIX connect()
 * @throw dpp::connection_exception on failure
 */
int connect_with_timeout(dpp::socket sockfd, const struct sockaddr *addr, socklen_t addrlen, unsigned int timeout_ms) {
#ifdef __APPLE__
		/* Unreliable on OSX right now */
		return (::connect(sockfd, addr, addrlen));
#else
	if (!set_nonblocking(sockfd, true)) {
		throw dpp::connection_exception(err_nonblocking_failure, "Can't switch socket to non-blocking mode!");
	}
#ifdef _WIN32
	/* Windows connect returns -1 and sets its error value to 0 for successfull blocking connection -
	 * This is equivalent to EWOULDBLOCK on POSIX
	 */
	ULONG non_blocking = 1;
	ioctlsocket(sockfd, FIONBIO, &non_blocking);
	int rc = WSAConnect(sockfd, addr, addrlen, nullptr, nullptr, nullptr, nullptr);
	int err = EWOULDBLOCK;
#else
	/* Standard POSIX connection behaviour */
	int rc = (::connect(sockfd, addr, addrlen));
	int err = errno;
#endif
	if (rc == -1 && err != EWOULDBLOCK && err != EINPROGRESS) {
		throw connection_exception(err_connect_failure, strerror(errno));
	} else {
		/* Set a deadline timestamp 'timeout' ms from now */
		double deadline = utility::time_f() + (timeout_ms / 1000.0);
		do {
			rc = -1;
			if (utility::time_f() >= deadline) {
				throw connection_exception(err_connection_timed_out, "Connection timed out");
			}
			pollfd pfd = {};
			pfd.fd = sockfd;
			pfd.events = POLLOUT;
			int r = poll(&pfd, 1, 10);
			if (r > 0 && pfd.revents & POLLOUT) {
				rc = 0;
			} else if (r != 0 || pfd.revents & POLLERR) {
				throw connection_exception(err_connection_timed_out, strerror(errno));
			}
		} while (rc == -1);
	}
	if (!set_nonblocking(sockfd, false)) {
		throw connection_exception(err_nonblocking_failure, "Can't switch socket to blocking mode!");
	}
	return rc;
#endif
}

#ifndef _WIN32
void set_signal_handler(int signal)
{
	struct sigaction sa;
	sigaction(signal, nullptr, &sa);
	if (sa.sa_flags == 0 && sa.sa_handler == nullptr) {
		sa = {};
		sigaction(signal, &sa, nullptr);
	}
}
#endif

ssl_client::ssl_client(const std::string &_hostname, const std::string &_port, bool plaintext_downgrade, bool reuse) :
	nonblocking(false),
	sfd(INVALID_SOCKET),
	ssl(nullptr),
	last_tick(time(nullptr)),
	hostname(_hostname),
	port(_port),
	bytes_out(0),
	bytes_in(0),
	plaintext(plaintext_downgrade),
	make_new(true),
	keepalive(reuse)
{
#ifndef WIN32
	set_signal_handler(SIGALRM);
	set_signal_handler(SIGXFSZ);
	set_signal_handler(SIGCHLD);
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
#else
	// Set up winsock.
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		throw dpp::connection_exception(err_connect_failure, "WSAStartup failure");
	}
#endif
	if (keepalive) {
		std::string identifier((!plaintext ? "ssl://" : "tcp://") + hostname + ":" + port);
		auto iter = keepalives.find(identifier);
		if (iter != keepalives.end()) {
			/* Found a keepalive connection, check it is still connected/valid via poll() for error */
			pollfd pfd = {};
			pfd.fd = iter->second.sfd;
			pfd.events = POLLOUT;
			int r = poll(&pfd, 1, 1);
			if (time(nullptr) > (iter->second.created + 60) || r < 0 || pfd.revents & POLLERR) {
				make_new = true;
				/* This connection is dead, free its resources and make a new one */
				if (iter->second.ssl->ssl) {
					SSL_free(iter->second.ssl->ssl);
					iter->second.ssl->ssl = nullptr;
				}
				close_socket(iter->second.sfd);
				iter->second.sfd = INVALID_SOCKET;
				delete iter->second.ssl;
			} else {
				/* Connection is good, lets use it */
				this->sfd = iter->second.sfd;
				this->ssl = iter->second.ssl;
				make_new = false;
			}
			/* We don't keep in-flight connections in the keepalives list */
			keepalives.erase(iter);
		}

	}
	if (make_new) {
		if (plaintext) {
			ssl = nullptr;
		} else {
			ssl = new openssl_connection();
		}
	}
	try {
		this->connect();
	}
	catch (std::exception&) {
		cleanup();
		throw;
	}
}

/* SSL Client constructor throws std::runtime_error if it can't connect to the host */
void ssl_client::connect()
{
	/* Initial connection is done in blocking mode. There is a timeout on it. */
	nonblocking = false;

	if (make_new) {
		/* Resolve hostname to IP */
		int err = 0;
		const dns_cache_entry* addr = resolve_hostname(hostname, port);
		sfd = ::socket(addr->addr.ai_family, addr->addr.ai_socktype, addr->addr.ai_protocol);
		if (sfd == ERROR_STATUS) {
			err = errno;
		} else if (connect_with_timeout(sfd, (sockaddr*)&addr->ai_addr, (int)addr->addr.ai_addrlen, SOCKET_OP_TIMEOUT) != 0) {
			close_socket(sfd);
			sfd = ERROR_STATUS;
		}

		/* Check if none of the IPs yielded a valid connection */
		if (sfd == ERROR_STATUS) {
			throw dpp::connection_exception(err_connect_failure, strerror(err));
		}

		if (!plaintext) {
			/* Each thread needs a context, but we don't need to make a new one for each connection */
			if (!openssl_context) {
				/* We're good to go - hand the fd over to openssl */
				const SSL_METHOD *method = TLS_client_method(); /* Create new client-method instance */

				/* Create SSL context */
				openssl_context.reset(SSL_CTX_new(method));
				if (!openssl_context) {
					throw dpp::connection_exception(err_ssl_context, "Failed to create SSL client context!");
				}

				/* Do not allow SSL 3.0, TLS 1.0 or 1.1
				* https://www.packetlabs.net/posts/tls-1-1-no-longer-secure/
				*/
				if (!SSL_CTX_set_min_proto_version(openssl_context.get(), TLS1_2_VERSION)) {
					throw dpp::connection_exception(err_ssl_version, "Failed to set minimum SSL version!");
				}
			}

			/* Create SSL session */
			ssl->ssl = SSL_new(openssl_context.get());
			if (ssl->ssl == nullptr) {
				throw dpp::connection_exception(err_ssl_new, "SSL_new failed!");
			}

			SSL_set_fd(ssl->ssl, (int)sfd);

			/* Server name identification (SNI) */
			SSL_set_tlsext_host_name(ssl->ssl, hostname.c_str());

#ifndef _WIN32
			/* On Linux, we can set socket timeouts so that SSL_connect eventually gives up */
			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = SOCKET_OP_TIMEOUT * 1000;
			setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
			setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif
			if (SSL_connect(ssl->ssl) != 1) {
				throw dpp::connection_exception(err_ssl_connect, "SSL_connect error");
			}

			this->cipher = SSL_get_cipher(ssl->ssl);
		}
	}
}

void ssl_client::write(const std::string &data)
{
	/* If we are in nonblocking mode, append to the buffer,
	 * otherwise just use SSL_write directly. The only time we
	 * use SSL_write directly is during connection before the
	 * ReadLoop is called, which allows for guaranteed simple
	 * lock-step delivery e.g. for HTTP header negotiation
	 */
	if (nonblocking) {
		obuffer += data;
	} else {
		const int data_length = (int)data.length();
		if (plaintext) {
			if (sfd == INVALID_SOCKET || ::send(sfd, data.data(), data_length, 0) != data_length) {
				throw dpp::connection_exception(err_write, "write() failed");
			}
		} else {
			if (SSL_write(ssl->ssl, data.data(), data_length) != data_length) {
				throw dpp::connection_exception(err_ssl_write, "SSL_write() failed");
			}
		}
	}
}

void ssl_client::one_second_timer()
{
}

std::string ssl_client::get_cipher() {
	return cipher;
}

void ssl_client::log(dpp::loglevel severity, const std::string &msg) const
{
}

void ssl_client::read_loop()
{
	/* The read loop is non-blocking using poll(). This method
	 * cannot read while it is waiting for write, or write while it is
	 * waiting for read. This is a limitation of the openssl libraries,
	 * as SSL is sent and received in low level ~16k frames which must
	 * be synchronised and ordered correctly. Attempting to send while
	 * we need another frame or receive while we are due to send a frame
	 * would cause the protocol to break.
	 */
	int r = 0, sockets = 1;
	size_t client_to_server_length = 0, client_to_server_offset = 0;
	bool read_blocked_on_write =  false, write_blocked_on_read = false, read_blocked = false;
	pollfd pfd[2] = {};
	char client_to_server_buffer[DPP_BUFSIZE], server_to_client_buffer[DPP_BUFSIZE];

	try {

		if (sfd == INVALID_SOCKET)  {
			throw dpp::connection_exception(err_invalid_socket, "Invalid file descriptor in read_loop()");
		}
		
		/* Make the socket nonblocking */
		if (!set_nonblocking(sfd, true)) {
			throw dpp::connection_exception(err_nonblocking_failure, "Can't switch socket to non-blocking mode!");
		}
		nonblocking = true;

		/* Loop until there is a socket error */
		while(true) {

			if (last_tick != time(nullptr)) {
				this->one_second_timer();
				last_tick = time(nullptr);
			}

			sockets = 1;
			pfd[0].fd = sfd;
			pfd[0].events = POLLIN;
			pfd[1].events = 0;

			if (custom_readable_fd && custom_readable_fd() >= 0) {
				int cfd = (int)custom_readable_fd();
				pfd[1].fd = cfd;
				pfd[1].events = POLLIN;
				sockets = 2;
			}
			if (custom_writeable_fd && custom_writeable_fd() >= 0) {
				int cfd = (int)custom_writeable_fd();
				pfd[1].fd = cfd;
				pfd[1].events |= POLLOUT;
				sockets = 2;
			}

			if (sfd == -1) {
				throw dpp::connection_exception(err_invalid_socket, "File descriptor invalidated, connection died");
			}

			/* If we're waiting for a read on the socket don't try to write to the server */
			if (client_to_server_length || obuffer.length() || read_blocked_on_write) {
				pfd[0].events |= POLLOUT;
			}

			const int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			int poll_time = 1000 - (now % 1000);
			poll_time = poll_time > 400 ? 1000 : poll_time + poll_time / 3 + 1;
			r = poll(pfd, sockets, now / 1000 == (int64_t)last_tick ? poll_time : 0);

			if (r == 0) {
				continue;
			}

			if (custom_writeable_fd && custom_writeable_fd() >= 0 && pfd[1].revents & POLLOUT) {
				custom_writeable_ready();
			}
			if (custom_readable_fd && custom_readable_fd() >= 0 && pfd[1].revents & POLLIN) {
				custom_readable_ready();
			}
			if ((pfd[0].revents & POLLERR) || (pfd[0].revents & POLLNVAL) || sfd == INVALID_SOCKET) {
				throw dpp::connection_exception(err_socket_error, strerror(errno));
			}

			/* Now check if there's data to read */
			if (((pfd[0].revents & POLLIN) && !write_blocked_on_read) || (read_blocked_on_write && (pfd[0].revents & POLLOUT))) {
				if (plaintext) {
					read_blocked_on_write = false;
					read_blocked = false;
					r = (int) ::recv(sfd, server_to_client_buffer, DPP_BUFSIZE, 0);
					if (r <= 0) {
						/* error or EOF */
						return;
					} else {
						buffer.append(server_to_client_buffer, r);
						if (!this->handle_buffer(buffer)) {
							return;
						}
						bytes_in += r;
					}
				} else {
					do {
						read_blocked_on_write = false;
						read_blocked = false;
						
						r = SSL_read(ssl->ssl,server_to_client_buffer,DPP_BUFSIZE);
						int e = SSL_get_error(ssl->ssl,r);

						switch (e) {
							case SSL_ERROR_NONE:
								/* Data received, add it to the buffer */
								if (r > 0) {
									buffer.append(server_to_client_buffer, r);
									if (!this->handle_buffer(buffer)) {
										return;
									}
									bytes_in += r;
								}
							break;
							case SSL_ERROR_ZERO_RETURN:
								/* End of data */
								SSL_shutdown(ssl->ssl);
								return;
							break;
							case SSL_ERROR_WANT_READ:
								read_blocked = true;
							break;
									
							/* We get a WANT_WRITE if we're trying to rehandshake and we block on a write during that rehandshake.
							* We need to wait on the socket to be writeable but reinitiate the read when it is
							*/
							case SSL_ERROR_WANT_WRITE:
								read_blocked_on_write = true;
							break;
							default:
								return;
							break;
						}

						/* We need a check for read_blocked here because SSL_pending() doesn't work properly during the
						* handshake. This check prevents a busy-wait loop around SSL_read()
						*/
					} while (SSL_pending(ssl->ssl) && !read_blocked);
				}
			}

			/* Check for input on the sendq */
			if (obuffer.length() && client_to_server_length == 0) {
				memcpy(&client_to_server_buffer, obuffer.data(), obuffer.length() > DPP_BUFSIZE ? DPP_BUFSIZE : obuffer.length());
				client_to_server_length = obuffer.length() > DPP_BUFSIZE ? DPP_BUFSIZE : obuffer.length();
				obuffer = obuffer.substr(client_to_server_length, obuffer.length());
				client_to_server_offset = 0;
			}

			/* If the socket is writeable... */
			if (((pfd[0].revents & POLLOUT) && client_to_server_length) || (write_blocked_on_read && (pfd[0].revents & POLLIN))) {
				write_blocked_on_read = false;
				/* Try to write */

				if (plaintext) {
					r = (int) ::send(sfd, client_to_server_buffer + client_to_server_offset, (int)client_to_server_length, 0);

					if (r < 0) {
						/* Write error */
						return;
					} else {
						client_to_server_length -= r;
						client_to_server_offset += r;
						bytes_out += r;
					}
				} else {
					r = SSL_write(ssl->ssl, client_to_server_buffer + client_to_server_offset, (int)client_to_server_length);
					
					switch(SSL_get_error(ssl->ssl,r)) {
						/* We wrote something */
						case SSL_ERROR_NONE:
							client_to_server_length -= r;
							client_to_server_offset += r;
							bytes_out += r;
						break;
							
						/* We would have blocked */
						case SSL_ERROR_WANT_WRITE:
						break;
				
						/* We get a WANT_READ if we're trying to rehandshake and we block onwrite during the current connection.
						* We need to wait on the socket to be readable but reinitiate our write when it is
						*/
						case SSL_ERROR_WANT_READ:
							write_blocked_on_read = true;
						break;
								
						/* Some other error */
						default:
							return;
						break;
					}
				}
			}
		}
	}
	catch (const std::exception &e) {
		log(ll_warning, std::string("Read loop ended: ") + e.what());
	}
}

uint64_t ssl_client::get_bytes_out()
{
	return bytes_out;
}

uint64_t ssl_client::get_bytes_in()
{
	return bytes_in;
}

bool ssl_client::handle_buffer(std::string &buffer)
{
	return true;
}

void ssl_client::close()
{
	if (keepalive && this->sfd != INVALID_SOCKET) {
		std::string identifier((!plaintext ? "ssl://" : "tcp://") + hostname + ":" + port);
		auto iter = keepalives.find(identifier);
		if (iter == keepalives.end()) {
			keepalive_cache_t kc;
			kc.created = time(nullptr);
			kc.sfd = this->sfd;
			kc.ssl = this->ssl;
			keepalives.emplace(identifier, kc);
		}
		return;
	}

	if (!plaintext && ssl->ssl) {
		SSL_free(ssl->ssl);
		ssl->ssl = nullptr;
	}
	close_socket(sfd);
	sfd = INVALID_SOCKET;
	obuffer.clear();
	buffer.clear();
}

void ssl_client::cleanup()
{
	this->close();
	if (!keepalive) {
		delete ssl;
	}
}

ssl_client::~ssl_client()
{
	cleanup();
}

} // namespace dpp
