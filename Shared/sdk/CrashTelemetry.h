/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

#ifdef __cplusplus
namespace CrashTelemetry
{
    struct Context
    {
        std::size_t requestedSize{0};
        const void* resourcePointer{nullptr};
        std::array<char, 64> resourceTag{};
        std::array<char, 128> detail{};
        std::chrono::system_clock::time_point timestamp{};
        std::uint32_t threadId{0};
        bool hasData{false};
    };

#if defined(MTA_CLIENT)
#    include <algorithm>
#    include <windows.h>
#    include <cstring>
#    include <cstdio>

namespace detail
{
    constexpr std::size_t kHistoryCapacity = 6;

    template <std::size_t N>
    inline void CopyInto(std::array<char, N>& destination, const char* source) noexcept
    {
        if (destination.empty())
            return;

        const std::size_t maxCopy = N - 1;
        const std::size_t copyLength = (source != nullptr) ? std::min<std::size_t>(std::strlen(source), maxCopy) : 0;

        if (copyLength != 0)
        {
            std::memcpy(destination.data(), source, copyLength);
            destination[copyLength] = '\0';
        }
        else
        {
            destination[0] = '\0';
        }

        if (copyLength + 1 < N)
        {
            std::fill(destination.begin() + static_cast<std::ptrdiff_t>(copyLength + 1), destination.end(), '\0');
        }
    }

    struct ThreadState
    {
        Context current{};
        std::array<Context, kHistoryCapacity> history{};
        std::size_t nextSlot = 0;
        std::size_t size = 0;
        std::size_t lastFailureSize = 0;
        Context lastManualScope{};
        bool     lastManualScopeValid = false;

        void PushSnapshot() noexcept
        {
            history[nextSlot] = current;
            history[nextSlot].hasData = true;
            nextSlot = (nextSlot + 1) % kHistoryCapacity;
            if (size < kHistoryCapacity)
                ++size;
        }

        template <typename Callback>
        void VisitNewestFirst(Callback&& callback) const noexcept
        {
            for (std::size_t i = 0; i < size; ++i)
            {
                const auto slotIndex = (nextSlot + kHistoryCapacity - 1 - i) % kHistoryCapacity;
                const Context& entry = history[slotIndex];
                if (!entry.hasData)
                    continue;

                callback(entry);
            }
        }
    };

    inline ThreadState& GetThreadState() noexcept
    {
        thread_local ThreadState state{};
        return state;
    }

    inline bool ContextEquals(const Context& lhs, const Context& rhs) noexcept
    {
        if (!lhs.hasData || !rhs.hasData)
            return false;

        if (lhs.requestedSize != rhs.requestedSize || lhs.resourcePointer != rhs.resourcePointer || lhs.threadId != rhs.threadId
            || lhs.timestamp != rhs.timestamp)
            return false;

        if (std::memcmp(lhs.resourceTag.data(), rhs.resourceTag.data(), lhs.resourceTag.size()) != 0)
            return false;

        if (std::memcmp(lhs.detail.data(), rhs.detail.data(), lhs.detail.size()) != 0)
            return false;

        return true;
    }

    inline void RecordManualScopeSnapshot() noexcept
    {
        ThreadState& state = GetThreadState();
        state.lastManualScope = state.current;
        state.lastManualScope.hasData = true;
        state.lastManualScopeValid = true;
    }
}            // namespace detail

    inline void SetAllocationContext(std::size_t sizeBytes,
                                     const void* resourcePtr,
                                     const char* resourceTag,
                                     const char* detail) noexcept
    {
        Context& current = detail::GetThreadState().current;
        current.requestedSize = sizeBytes;
        current.resourcePointer = resourcePtr;
        current.timestamp = std::chrono::system_clock::now();
        current.threadId = GetCurrentThreadId();
        detail::CopyInto(current.resourceTag, resourceTag);
        detail::CopyInto(current.detail, detail);
        current.hasData = true;

        detail::GetThreadState().PushSnapshot();
    }

    inline void ClearAllocationContext() noexcept
    {
        detail::GetThreadState().current = Context{};
    }

    inline Context CaptureContext() noexcept
    {
        return detail::GetThreadState().current;
    }

    inline void RestoreContext(const Context& context) noexcept
    {
        detail::GetThreadState().current = context;
    }

    inline void RecordAllocationFailureSize(std::size_t sizeBytes) noexcept
    {
        detail::GetThreadState().lastFailureSize = sizeBytes;
    }

    inline std::size_t GetRecordedAllocationSize() noexcept
    {
        const auto& state = detail::GetThreadState();
        if (state.lastFailureSize != 0)
            return state.lastFailureSize;

        return state.current.requestedSize;
    }

    inline std::string BuildAllocationTelemetryNote() noexcept
    {
        std::array<Context, detail::kHistoryCapacity> snapshot{};
        std::size_t snapshotCount = 0;
        const auto  now = std::chrono::system_clock::now();

        const auto& threadState = detail::GetThreadState();
        threadState.VisitNewestFirst([&](const Context& entry) {
            if (snapshotCount < snapshot.size())
                snapshot[snapshotCount++] = entry;
        });

        const std::size_t recordedSize = GetRecordedAllocationSize();

        std::string note;
        note.reserve(640);

        auto appendEntryLine = [&](const Context& entry, std::size_t index, bool includeIndexLabel) {
            std::size_t sizeToReport = entry.requestedSize;
            if (includeIndexLabel && index == 0 && recordedSize != 0)
            {
                sizeToReport = recordedSize;
            }

            const char* resourceTag = (entry.resourceTag[0] != '\0') ? entry.resourceTag.data() : "<unset>";
            const char* detailText = (entry.detail[0] != '\0') ? entry.detail.data() : "<none>";
            const auto  threadId = entry.threadId != 0 ? entry.threadId : GetCurrentThreadId();

            long long ageMs = -1;
            if (entry.timestamp.time_since_epoch().count() != 0)
            {
                ageMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - entry.timestamp).count();
                if (ageMs < 0)
                    ageMs = 0;
            }

            char line[256]{};
            if (ageMs >= 0)
            {
                if (includeIndexLabel)
                {
                    _snprintf_s(line,
                                sizeof(line),
                                _TRUNCATE,
                                "  #%zu requested=%zu bytes, tag=%s, detail=%s, object=0x%p, thread=%lu, age=%llims\n",
                                index,
                                sizeToReport,
                                resourceTag,
                                detailText,
                                entry.resourcePointer,
                                static_cast<unsigned long>(threadId),
                                ageMs);
                }
                else
                {
                    _snprintf_s(line,
                                sizeof(line),
                                _TRUNCATE,
                                "  requested=%zu bytes, tag=%s, detail=%s, object=0x%p, thread=%lu, age=%llims\n",
                                sizeToReport,
                                resourceTag,
                                detailText,
                                entry.resourcePointer,
                                static_cast<unsigned long>(threadId),
                                ageMs);
                }
            }
            else
            {
                if (includeIndexLabel)
                {
                    _snprintf_s(line,
                                sizeof(line),
                                _TRUNCATE,
                                "  #%zu requested=%zu bytes, tag=%s, detail=%s, object=0x%p, thread=%lu\n",
                                index,
                                sizeToReport,
                                resourceTag,
                                detailText,
                                entry.resourcePointer,
                                static_cast<unsigned long>(threadId));
                }
                else
                {
                    _snprintf_s(line,
                                sizeof(line),
                                _TRUNCATE,
                                "  requested=%zu bytes, tag=%s, detail=%s, object=0x%p, thread=%lu\n",
                                sizeToReport,
                                resourceTag,
                                detailText,
                                entry.resourcePointer,
                                static_cast<unsigned long>(threadId));
                }
            }

            note += line;
        };

        if (snapshotCount == 0)
        {
            char buffer[160]{};
            _snprintf_s(buffer,
                        sizeof(buffer),
                        _TRUNCATE,
                        "Allocation telemetry: requested=%zu bytes (no tagged context available)\n",
                        recordedSize);
            note += buffer;
        }
        else
        {
            note += "Allocation telemetry (most recent first):\n";
            for (std::size_t i = 0; i < snapshotCount; ++i)
            {
                appendEntryLine(snapshot[i], i, true);
            }
        }

        const bool hasManualScope = threadState.lastManualScopeValid && threadState.lastManualScope.hasData;
        bool manualScopeCapturedInRing = false;
        if (hasManualScope)
        {
            for (std::size_t i = 0; i < snapshotCount; ++i)
            {
                if (detail::ContextEquals(snapshot[i], threadState.lastManualScope))
                {
                    manualScopeCapturedInRing = true;
                    break;
                }
            }
        }

        if (!hasManualScope)
        {
            note +=
                "CrashTelemetry::Scope annotations: none executed on this thread before crash; instrument entry points to capture context.\n";
        }
        else if (!manualScopeCapturedInRing)
        {
            // Helps to see the last annotated scope even if the allocator ring threw it out; use this to decide
            // which additional paths (such as todos: CMapEventManager dispatch, CClientEntity::CallEvent, resource download callbacks, etc.)
            // should receive CrashTelemetry::Scope guards next.
            note += "Last CrashTelemetry::Scope (not present in allocation ring):\n";
            appendEntryLine(threadState.lastManualScope, 0, false);
        }

        return note;
    }

    class Scope
    {
    public:
        Scope(std::size_t sizeBytes,
              const void* resourcePtr,
              const char* resourceTag,
              const char* detail) noexcept
            : m_previous(CaptureContext()), m_restore(true)
        {
            SetAllocationContext(sizeBytes, resourcePtr, resourceTag, detail);
            // Every CrashTelemetry::Scope marks a meaningful execution point. Add these to crash-prone hot paths
            // (Lua event dispatch, CMapEventManager, CClientEntity::CallEvent, resource downloads, etc.) so any crash,
            // including nullptr dereferences, retains the last context in telemetry dumps.
            detail::RecordManualScopeSnapshot();
        }
        ~Scope()
        {
            if (m_restore)
            {
                RestoreContext(m_previous);
            }
        }

        Scope(const Scope&) = delete;
        Scope& operator=(const Scope&) = delete;
        Scope(Scope&&) = delete;
        Scope& operator=(Scope&&) = delete;

    private:
        Context m_previous{};
        bool    m_restore{false};
    };
#else
    inline void RecordAllocationFailureSize(std::size_t) noexcept {}
    inline std::size_t GetRecordedAllocationSize() noexcept { return 0; }
    inline void SetAllocationContext(std::size_t,
                                     const void*,
                                     const char*,
                                     const char*) noexcept
    {
    }

    inline void ClearAllocationContext() noexcept {}

    inline Context CaptureContext() noexcept { return Context{}; }
    inline void RestoreContext(const Context&) noexcept {}
    inline std::string BuildAllocationTelemetryNote() noexcept { return {}; }

    class Scope
    {
    public:
        Scope(std::size_t, const void*, const char*, const char*) noexcept {}
        ~Scope() = default;

        Scope(const Scope&) = delete;
        Scope& operator=(const Scope&) = delete;
        Scope(Scope&&) = delete;
        Scope& operator=(Scope&&) = delete;
    };
#endif
}            // namespace CrashTelemetry
#endif
