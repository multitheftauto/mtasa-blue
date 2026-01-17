#pragma once

#include "StdInc.h"

#include <atomic>
#include <chrono>
#include <limits>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace ComPtrValidation
{
    constexpr uint64_t kValidationThrottleMs = 200;
    constexpr size_t   kValidationCacheReserve = 256;

    inline uint64_t GetMonotonicMilliseconds()
    {
        using namespace std::chrono;
        return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
    }

    struct CacheEntry
    {
        uint64_t lastCheckMs{0};
        bool     valid{false};
    };

    struct CacheStorage
    {
        CacheStorage()
        {
            cache.reserve(kValidationCacheReserve);
            cache.max_load_factor(0.5f);
        }

        std::unordered_map<const void*, CacheEntry> cache;
    };

    inline std::atomic<uint64_t>& InvalidationEpoch()
    {
        static std::atomic<uint64_t> s_epoch{0};
        return s_epoch;
    }

    inline std::shared_mutex& CacheMutex()
    {
        static std::shared_mutex s_mutex;
        return s_mutex;
    }

    inline std::unordered_map<const void*, CacheEntry>& Cache()
    {
        static CacheStorage s_storage;
        return s_storage.cache;
    }

    inline void Invalidate(const void* pointer)
    {
        if (!pointer)
            return;

        {
            std::unique_lock lock(CacheMutex());
            Cache().erase(pointer);
        }
        InvalidationEpoch().fetch_add(1, std::memory_order_acq_rel);
    }

    enum class ValidationMode
    {
        Default,
        ForceRefresh,
    };

    template <typename T>
    bool ValidateSlow(T* pointer)
    {
        if (!SharedUtil::IsReadablePointer(pointer, sizeof(void*)))
            return false;

        void* const* vtablePtr = reinterpret_cast<void* const*>(pointer);
        if (!vtablePtr)
            return false;

        void* const vtable = *vtablePtr;
        if (!vtable)
            return false;

        constexpr size_t requiredBytes = sizeof(void*) * 3;
        return SharedUtil::IsReadablePointer(vtable, requiredBytes);
    }

    struct ThreadCache
    {
        const void* pointer{nullptr};
        CacheEntry  entry{};
        uint64_t    epoch{std::numeric_limits<uint64_t>::max()};
    };

    template <typename T>
    bool Validate(T* pointer, ValidationMode mode = ValidationMode::Default)
    {
        if (!pointer)
            return true;

        thread_local ThreadCache s_threadCache;

        for (;;)
        {
            const uint64_t epoch = InvalidationEpoch().load(std::memory_order_acquire);
            const uint64_t now = GetMonotonicMilliseconds();

            if (mode == ValidationMode::Default && s_threadCache.pointer == pointer && s_threadCache.epoch == epoch)
            {
                const uint64_t last = s_threadCache.entry.lastCheckMs;
                const uint64_t elapsed = (now >= last) ? now - last : 0;
                if (elapsed < kValidationThrottleMs)
                {
                    if (InvalidationEpoch().load(std::memory_order_acquire) == epoch)
                        return s_threadCache.entry.valid;
                    continue;
                }
            }

            if (mode == ValidationMode::Default)
            {
                std::shared_lock lock(CacheMutex());
                auto&            cache = Cache();
                const auto       iter = cache.find(pointer);
                if (iter != cache.end())
                {
                    const uint64_t last = iter->second.lastCheckMs;
                    const uint64_t elapsed = (now >= last) ? now - last : 0;
                    if (elapsed < kValidationThrottleMs)
                    {
                        if (InvalidationEpoch().load(std::memory_order_acquire) == epoch)
                        {
                            s_threadCache.pointer = pointer;
                            s_threadCache.entry = iter->second;
                            s_threadCache.epoch = epoch;
                            return iter->second.valid;
                        }
                        continue;
                    }
                }
            }

            const bool       valid = ValidateSlow(pointer);
            const CacheEntry newEntry{now, valid};
            {
                std::unique_lock lock(CacheMutex());
                Cache()[pointer] = newEntry;
            }

            if (InvalidationEpoch().load(std::memory_order_acquire) != epoch)
                continue;

            s_threadCache.pointer = pointer;
            s_threadCache.entry = newEntry;
            s_threadCache.epoch = epoch;
            return valid;
        }
    }

}  // namespace ComPtrValidation

template <typename T>
bool IsValidComInterfacePointer(T* pointer, ComPtrValidation::ValidationMode mode = ComPtrValidation::ValidationMode::Default)
{
    return ComPtrValidation::Validate(pointer, mode);
}
