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
#include <dpp/snowflake.h>
#include <dpp/managed.h>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

namespace dpp {

extern DPP_EXPORT std::unordered_map<managed*, time_t> deletion_queue;
extern DPP_EXPORT std::mutex deletion_mutex;

/** forward declaration */
class guild_member;

/**
 * @brief A cache object maintains a cache of dpp::managed objects.
 * 
 * This is for example users, channels or guilds. You may instantiate
 * your own caches, to contain any type derived from dpp::managed including
 * your own types.
 * 
 * @note This class is critical to the operation of the library and therefore
 * designed with thread safety in mind.
 * @tparam T class type to store, which should be derived from dpp::managed.
 */
template<class T> class cache {
private:
	/**
	 * @brief Mutex to protect the cache
	 * 
	 * This is a shared mutex so reading is cheap.
	 */
	std::shared_mutex cache_mutex;

	/**
	 * @brief Container of pointers to cached items
	 */
	std::unordered_map<snowflake, T*>* cache_map;
public:

	/**
	 * @brief Construct a new cache object.
	 * 
	 * @note Caches must contain classes derived from dpp::managed.
	 */
	cache() {
		cache_map = new std::unordered_map<snowflake, T*>;
	}

	/**
	 * @brief Destroy the cache object
	 * 
	 * @note This does not delete objects stored in the cache.
	 */
	~cache() {
		std::unique_lock l(cache_mutex);
		delete cache_map;			
	}

	/**
	 * @brief Store an object in the cache. Passing a nullptr will have no effect.
	 * 
	 * The object must be derived from dpp::managed and should be allocated on the heap.
	 * Generally this is done via `new`. Once stored in the cache the lifetime of the stored
	 * object is managed by the cache class unless the cache is deleted (at which point responsibility
	 * for deleting the object returns to its allocator). Objects stored are removed when the
	 * cache::remove() method is called by placing them into a garbage collection queue for deletion
	 * within the next 60 seconds, which are then deleted in bulk for efficiency and to aid thread
	 * safety.
	 * 
	 * @note Adding an object to the cache with an ID which already exists replaces that entry.
	 * The previously entered cache item is inserted into the garbage collection queue for deletion
	 * similarly to if cache::remove() was called first.
	 * 
	 * @param object object to store. Storing a pointer to the cache relinquishes ownership to the cache object.
	 */
	void store(T* object) {
		if (!object) {
			return;
		}
		std::unique_lock l(cache_mutex);
		auto existing = cache_map->find(object->id);
		if (existing == cache_map->end()) {
			(*cache_map)[object->id] = object;
		} else if (object != existing->second) {
			/* Flag old pointer for deletion and replace */
			std::lock_guard<std::mutex> delete_lock(deletion_mutex);
			deletion_queue[existing->second] = time(nullptr);
			(*cache_map)[object->id] = object;
		}
	}

	/**
	 * @brief Remove an object from the cache.
	 * 
	 * @note The cache class takes ownership of the pointer, and calling this method will
	 * cause deletion of the object within the next 60 seconds by means of a garbage
	 * collection queue. This queue aids in efficiency by freeing memory in bulk, and
	 * assists in thread safety by ensuring that all deletions can be locked and freed
	 * at the same time.
	 * 
	 * @param object object to remove. Passing a nullptr will have no effect.
	 */
	void remove(T* object) {
		if (!object) {
			return;
		}
		std::unique_lock l(cache_mutex);
		std::lock_guard<std::mutex> delete_lock(deletion_mutex);
		auto existing = cache_map->find(object->id);
		if (existing != cache_map->end()) {
			cache_map->erase(existing);
			deletion_queue[object] = time(nullptr);
		}
	}

	/**
	 * @brief Find an object in the cache by id.
	 * 
	 * The cache is searched for the object. All dpp::managed objects have a snowflake id
	 * (this is the only field dpp::managed actually has).
	 * 
	 * @warning Do not hang onto objects returned by cache::find() indefinitely. They may be
	 * deleted at a later date if cache::remove() is called. If persistence is required,
	 * take a copy of the object after checking its pointer is non-null.
	 * 
	 * @param id Object snowflake id to find
	 * @return Found object or nullptr if the object with this id does not exist.
	 */
	T* find(snowflake id) {
		std::shared_lock l(cache_mutex);
		auto r = cache_map->find(id);
		if (r != cache_map->end()) {
			return r->second;
		}
		return nullptr;
	}

	/**
	 * @brief Return a count of the number of items in the cache.
	 * 
	 * This is used by the library e.g. to count guilds, users, and roles
	 * stored within caches.
	 * get
	 * @return uint64_t count of items in the cache
	 */
	uint64_t count() {
		std::shared_lock l(cache_mutex);
		return cache_map->size();
	}

	/** 
	 * @brief Return the cache's locking mutex.
	 * 
	 * Use this whenever you manipulate or iterate raw elements in the cache!
	 * 
	 * @note If you are only reading from the cache's container, wrap this
	 * mutex in `std::shared_lock`, else wrap it in a `std::unique_lock`.
	 * Shared locks will allow for multiple readers whilst blocking writers,
	 * and unique locks will allow only one writer whilst blocking readers
	 * and writers.
	 * 
	 * **Example:**
	 * 
	 * ```cpp
	 * dpp::cache<guild>* c = dpp::get_guild_cache();
	 * std::unordered_map<snowflake, guild*>& gc = c->get_container();
	 * std::shared_lock l(c->get_mutex()); // MUST LOCK HERE
	 * for (auto g = gc.begin(); g != gc.end(); ++g) {
	 *     dpp::guild* gp = (dpp::guild*)g->second;
	 *     // Do something here with the guild* in 'gp'
	 * }
	 * ``` 
	 * 
	 * @return The mutex used to protect the container
	 */
	std::shared_mutex& get_mutex() {
		return this->cache_mutex;			
	}

	/**
	 * @brief Get the container unordered map
	 * 
	 * @warning Be sure to use cache::get_mutex() correctly if you
	 * manipulate or iterate the map returned by this method! If you do
	 * not, this is not thread safe and will cause crashes!
	 * 
	 * @see cache::get_mutex
	 * 
	 * @return A reference to the cache's container map
	 */
	auto & get_container() {
		return *(this->cache_map);		
	}

	/**
	 * @brief "Rehash" a cache by reallocating the map and copying
	 * all elements into the new one.
	 * 
	 * Over a long running timeframe, unordered maps can grow in size
	 * due to bucket allocation, this function frees that unused memory
	 * to keep the maps in control over time. If this is an issue which
	 * is apparent with your use of dpp::cache objects, you should periodically
	 * call this method.
	 * 
	 * @warning May be time consuming! This function is O(n) in relation to the
	 * number of cached entries.
	 */
	void rehash() {
		std::unique_lock l(cache_mutex);
		std::unordered_map<snowflake, T*>* n = new std::unordered_map<snowflake, T*>;
		n->reserve(cache_map->size());
		for (auto t = cache_map->begin(); t != cache_map->end(); ++t) {
			n->insert(*t);
		}
		delete cache_map;
		cache_map = n;
	}

	/**
	 * @brief Get "real" size in RAM of the cached objects
	 * 
	 * This does not include metadata used to maintain the unordered map itself.
	 * 
	 * @return size_t size of cache in bytes
	 */
	size_t bytes() {
		std::shared_lock l(cache_mutex);
		return sizeof(*this) + (cache_map->bucket_count() * sizeof(size_t));
	}

};

/**
 * Run garbage collection across all caches removing deleted items
 * that have been deleted over 60 seconds ago.
 */
void DPP_EXPORT garbage_collection();

#define cache_decl(type, setter, getter, counter) /** Find an object in the cache by id. @return type* Pointer to the object or nullptr when it's not found */ DPP_EXPORT class type * setter (snowflake id); DPP_EXPORT cache<class type> * getter (); /** Get the amount of cached type objects. */ DPP_EXPORT uint64_t counter ();

/* Declare major caches */
cache_decl(user, find_user, get_user_cache, get_user_count);
cache_decl(guild, find_guild, get_guild_cache, get_guild_count);
cache_decl(role, find_role, get_role_cache, get_role_count);
cache_decl(channel, find_channel, get_channel_cache, get_channel_count);
cache_decl(emoji, find_emoji, get_emoji_cache, get_emoji_count);

} // namespace dpp

