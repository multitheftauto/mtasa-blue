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
#pragma once
#include <dpp/export.h>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <sys/types.h>
#include <string>
#include <unordered_map>

namespace dpp {

	/**
	 * @brief Represents a cached DNS result.
	 * Used by the ssl_client class to store cached copies of dns lookups.
	 */
	struct dns_cache_entry {
		/**
		 * @brief Resolved address information
		 */
		addrinfo addr;

		/**
		 * @brief Socket address.
		 * Discord only supports ipv4, but sockaddr_in6 is larger
		 * than sockaddr_in, sockaddr_storage will hold either. This
		 * means that if discord ever do support ipv6 we just flip
		 * one value in dns.cpp and that should be all that is needed.
		 */
		sockaddr_storage ai_addr;

		/**
		 * @brief Time at which this cache entry is invalidated
		 */
		time_t expire_timestamp;
	};

	/**
	 * @brief Cache container type
	 */
	using dns_cache_t = std::unordered_map<std::string, dns_cache_entry*>;

	/**
	 * @brief Resolve a hostname to an addrinfo
	 * 
	 * @param hostname Hostname to resolve
	 * @param port A port number or named service, e.g. "80"
	 * @return dns_cache_entry* First IP address associated with the hostname DNS record
	 * @throw dpp::connection_exception On failure to resolve hostname
	 */
	const dns_cache_entry* resolve_hostname(const std::string& hostname, const std::string& port);
} // namespace dpp
