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

#include <dpp/dns.h>
#include <cerrno>
#include <exception>
#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <dpp/exception.h>

namespace dpp
{
	/* One hour in seconds */
	constexpr time_t one_hour = 60 * 60;

	/* Thread safety mutex for dns cache */
	std::shared_mutex dns_cache_mutex;

	/* Cache container */
	dns_cache_t dns_cache;

	const dns_cache_entry* resolve_hostname(const std::string& hostname, const std::string& port)
	{
		addrinfo hints, *addrs;
		dns_cache_t::const_iterator iter;
		time_t now = time(nullptr);
		int error;
		bool exists = false;

		/* Thread safety scope */
		{
			/* Check cache for existing DNS record. This can use a shared lock. */
			std::shared_lock dns_cache_lock(dns_cache_mutex);
			iter = dns_cache.find(hostname);
			if (iter != dns_cache.end()) {
				exists = true;
				if (now < iter->second->expire_timestamp) {
					/* there is a cached entry that is still valid, return it */
					return iter->second;
				}
			}
		}
		if (exists) {
			/* there is a cached entry, but it has expired,
			 * delete and free it, and fall through to a new lookup.
			 * We must use a unique lock here as we modify the cache.
			 */
			std::unique_lock dns_cache_lock(dns_cache_mutex);
			iter = dns_cache.find(hostname);
			if (iter != dns_cache.end()) { /* re-validate iter */
				delete iter->second;
				dns_cache.erase(iter);
			}
		}
		
		/* The hints indicate what sort of DNS results we are interested in.
		 * To change this to support IPv6, one change we need to make here is
		 * to change AF_INET to AF_UNSPEC. Everything else should just work fine.
		 */
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = AF_INET; // IPv6 explicitly unsupported by Discord
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if ((error = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &addrs))) {
			/**
			 * The -20 makes sure the error codes dont conflict with codes given in the rest of the list
			 * Because C libraries love to use -1 and below directly as conflicting error codes.
			 */
			throw dpp::connection_exception((exception_error_code)(error - 20), std::string("getaddrinfo error: ") + gai_strerror(error));
		}

		/* Thread safety scope */
		{
			/* Update cache, requires unique lock */
			std::unique_lock dns_cache_lock(dns_cache_mutex);
			dns_cache_entry* cache_entry = new dns_cache_entry();

			/* The sockaddr struct contains a bunch of raw pointers that we
			 * must copy to the cache, before freeing it with freeaddrinfo().
			 * Icky icky C APIs.
			 */
			memcpy(&cache_entry->addr, addrs, sizeof(addrinfo));
			memcpy(&cache_entry->ai_addr, addrs->ai_addr, addrs->ai_addrlen);
			cache_entry->expire_timestamp = now + one_hour;
			dns_cache[hostname] = cache_entry;

			/* Now we're done with this horrible struct, free it and return */
			freeaddrinfo(addrs);
			return cache_entry;
		}
	}
} // namespace dpp
