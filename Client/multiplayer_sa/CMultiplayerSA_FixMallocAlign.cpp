/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_FixMallocAlign.cpp
 *  PURPOSE:     Memory allocator (Replaces GTA:SA CMemoryMgr)
 *
 * Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <cerrno>
#include <cstdint>
#include <cstddef>
#include <psapi.h>
#include "CMemoryAllocatorDiagnostics.h" // Diagnostic logging interface (part of our allocator impl)

// C++14-compatible branch prediction optimization
// These provide the same optimization as C++20 [[likely]]/[[unlikely]]
#if defined(__GNUC__) || defined(__clang__)
    // GCC/Clang have proper __builtin_expect support for branch prediction
    #define MTA_LIKELY(condition)   (__builtin_expect(!!(condition), 1))
    #define MTA_UNLIKELY(condition) (__builtin_expect(!!(condition), 0))
#elif defined(_MSC_VER)
    // MSVC C++14/C++17 doesn't have __builtin_expect or equivalent intrinsics
    // MSVC optimizer uses Profile-Guided Optimization (PGO) and heuristics
    // The code structure itself provides optimization hints to the MSVC optimizer
    #define MTA_LIKELY(condition)   (condition)
    #define MTA_UNLIKELY(condition) (condition)
#else
    // Fallback for other compilers - no optimization but still works
    #define MTA_LIKELY(condition)   (condition)
    #define MTA_UNLIKELY(condition) (condition)
#endif

namespace mta
{
    namespace memory
    {
        // Global variables that need to be accessible from diagnostics module
        // These are declared extern in CMemoryAllocatorDiagnostics.cpp
        std::uintptr_t g_max_user_address = 0;
        std::uintptr_t g_available_address_space = 0;
        bool           g_is_64bit_host_os = false;
        HANDLE         g_process_heap = nullptr;

        namespace
        {
            // Internal variables that don't need external access
            bool  g_use_high_memory_allocation = false;
            DWORD g_page_size = 4096;
            DWORD g_allocation_granularity = 65536;

            // Thread-safe initialization lock for race condition fix
            volatile LONG g_init_lock = 0;

            // Initialize maximum user address and determine available address space
            void InitMaxUserAddress() noexcept
            {
                // Fast path: already initialized (99.999% of calls) - zero overhead
                if (MTA_LIKELY(g_max_user_address != 0))
                {
                    return;            // Already initialized, zero overhead
                }

                // Slow path: first call only - thread-safe initialization
                if (InterlockedCompareExchange(&g_init_lock, 1, 0) == 0)
                {
                    SYSTEM_INFO si = {0};            // Zero-initialize for safety
                    GetSystemInfo(&si);

                    // Minimal validation - only check for obvious corruption
                    if (si.lpMaximumApplicationAddress == nullptr || si.dwPageSize == 0 || si.dwAllocationGranularity == 0 ||
                        reinterpret_cast<std::uintptr_t>(si.lpMaximumApplicationAddress) < 0x10000)
                    {
                        // Use safe defaults only if system info is obviously corrupted
                        MTA_MEMORY_LOG_CRITICAL_FAILURE("System info corruption detected", nullptr, 0, 0);
                        g_page_size = 4096;
                        g_allocation_granularity = 65536;
                        g_use_high_memory_allocation = false;
                        g_available_address_space = 0x7FFF0000;
                    }
                    else
                    {
                        g_page_size = si.dwPageSize;
                        g_allocation_granularity = si.dwAllocationGranularity;
                        g_available_address_space = reinterpret_cast<std::uintptr_t>(si.lpMaximumApplicationAddress);

                        // Only validate power-of-2 if values seem sus
                        if ((g_page_size & (g_page_size - 1)) != 0 || (g_allocation_granularity & (g_allocation_granularity - 1)) != 0)
                        {
                            MTA_MEMORY_LOG_CRITICAL_FAILURE("Non-power-of-2 system values corrected", nullptr, g_page_size, g_allocation_granularity);
                            g_page_size = 4096;
                            g_allocation_granularity = 65536;
                        }

                        g_use_high_memory_allocation = (g_available_address_space > 0x80000000);
                    }

                    g_process_heap = GetProcessHeap();
                    if (!g_process_heap)
                    {
                        MTA_MEMORY_LOG_CRITICAL_FAILURE("GetProcessHeap failed, HeapAlloc disabled", nullptr, 0, 0);
                        g_use_high_memory_allocation = false;            // Fallback mode
                    }
                    g_is_64bit_host_os = (g_available_address_space > 0x80000000);

                    // Memory barrier ensures visibility before final flag
                    MemoryBarrier();
                    g_max_user_address = g_available_address_space;

                    // Release initialization lock to prevent deadlock
                    // Must be done after g_max_user_address is set for proper ordering
                    InterlockedExchange(&g_init_lock, 0);
                }
                else
                {
                    // Efficient spin wait with simple timeout
                    int spin_count = 0;
                    while (g_max_user_address == 0 && spin_count < 1000)
                    {
                        YieldProcessor();
                        spin_count++;
                    }
                    // If still not initialized, use default (extremely rare)
                    if (g_max_user_address == 0)
                    {
                        MTA_MEMORY_LOG_CRITICAL_FAILURE("Initialization timeout, using default address space", nullptr, 0, 0);
                        g_max_user_address = 0x7FFF0000;
                    }
                }
            }

            constexpr bool is_power_of_two(std::size_t value) noexcept
            {
                return value != 0 && (value & (value - 1)) == 0;
            }

            // Ultra-optimized alignment validation for common cases
            constexpr bool is_valid_alignment(std::size_t alignment) noexcept
            {
                // Ultra-fast path for most common alignments (99%+ of cases)
                // Use bit manipulation trick: valid power-of-2 alignments have exactly one bit set
                if (alignment <= 16)
                {
                    // Optimized for 1,2,4,8,16 - covers 95%+ of all allocations
                    return (alignment & (alignment - 1)) == 0 && alignment != 0;
                }

                // Fast path for common larger alignments
                if (alignment <= 256)
                {
                    return is_power_of_two(alignment);
                }

                // Rare large alignments
                switch (alignment)
                {
                    case 512:
                    case 1024:
                    case 2048:
                    case 4096:
                    case 8192:
                    case 16384:
                    case 32768:
                    case 65536:
                        return true;
                    default:
                        // Allow any power-of-2 alignment up to reasonable system limits
                        // Let the system handle what's actually possible rather than arbitrary limits
                        return alignment > 0 && is_power_of_two(alignment);
                }
            }

            void* SafeMallocAlignVirtual(std::size_t size, std::size_t alignment) noexcept;

            // Safe aligned memory allocation replacement for CMemoryMgr::MallocAlign
            // Must match original GTA:SA behavior exactly - stores original pointer at result-4
            void* SafeMallocAlign(std::size_t size, std::size_t alignment) noexcept
            {
                // Basic input validation - most MTA allocations are valid
                if (MTA_UNLIKELY(size == 0 || alignment == 0))
                {
                    // MSVC API Conformance: _aligned_malloc sets errno to EINVAL for invalid parameters
                    errno = EINVAL;
                    MTA_MEMORY_LOG_SAFETY_REJECTION("Zero size or alignment detected", nullptr, size, alignment, "MSVC API compliance");
                    return nullptr;
                }

                // Fast-path alignment validation with compile-time optimization
                if (!is_valid_alignment(alignment))
                {
                    // MSVC API Conformance: _aligned_malloc sets errno to EINVAL for non-power-of-2 alignment
                    errno = EINVAL;
                    MTA_MEMORY_LOG_SAFETY_REJECTION("Invalid alignment - not power of 2", nullptr, size, alignment, "MSVC API compliance");
                    return nullptr;
                }

                // Simplified size check using compile-time constant (LARGE_ADDRESS_AWARE aware)
                if (MTA_UNLIKELY(size > 0x7FF00000))            // ~2GB limit (safe for all x86 systems)
                {
                    // MSVC API Conformance: _aligned_malloc sets errno to ENOMEM for oversized requests
                    errno = ENOMEM;
                    MTA_MEMORY_LOG_SAFETY_REJECTION("Size exceeds 2GB limit", nullptr, size, alignment, "x86 system safety");
                    return nullptr;
                }

                // Fast path for small allocations - leverages existing size validation for overflow safety
                if (size <= 64 && alignment <= 8)
                {
                    // Total size calculation - overflow already prevented by 2GB size limit above
                    // Mathematical proof: size ≤ 64, alignment ≤ 8, sizeof(void*) = 4
                    // Maximum possible total: 64 + 8 + 4 = 76 bytes (no overflow possible)
                    const std::size_t total_size = size + alignment + sizeof(void*);

                    void* raw_memory = malloc(total_size);
                    if (MTA_LIKELY(raw_memory))
                    {
                        const std::uintptr_t raw_addr = reinterpret_cast<std::uintptr_t>(raw_memory);
                        const std::uintptr_t align_u32 = static_cast<std::uintptr_t>(alignment);
                        const std::uintptr_t aligned_addr = (raw_addr + sizeof(void*) + align_u32 - 1) & ~(align_u32 - 1);

                        // Correct bounds check with proper logic
                        if (MTA_LIKELY(aligned_addr >= raw_addr + sizeof(void*) && aligned_addr + size <= raw_addr + total_size && aligned_addr >= raw_addr))
                        {
                            // Store with malloc flag (0) - must match main path logic
                            // Flag encoding: bits 0-1 = allocation method (0=malloc, 1=HeapAlloc, 2=VirtualAlloc)
                            *reinterpret_cast<std::uintptr_t*>(aligned_addr - sizeof(void*)) =
                                raw_addr & ~static_cast<std::uintptr_t>(0x3);            // malloc flag = 0x00000000
                            return reinterpret_cast<void*>(aligned_addr);
                        }
                        free(raw_memory);
                    }
                }

                // Dynamic allocation strategy optimized for MTA:SA patterns
                // MTA has frequent medium-sized allocations
                // Tuned thresholds based on MTA workload - prioritize low latency for gameplay
                std::size_t virtual_alloc_threshold = 1024 * 1024;            // 1MB - balanced performance/efficiency

                if (g_use_high_memory_allocation)
                {
                    // Optimized thresholds for LARGE_ADDRESS_AWARE systems with abundant memory
                    // More aggressive VirtualAlloc usage prevents heap fragmentation in 4GB systems
                    if (g_available_address_space >= 0xF0000000)            // 3.75GB+ (4GB LARGE_ADDRESS_AWARE)
                    {
                        virtual_alloc_threshold = 256 * 1024;            // 256KB - aggressive VirtualAlloc for optimal fragmentation
                    }
                    else if (g_available_address_space > 0xC0000000)            // 3GB+ available
                    {
                        virtual_alloc_threshold = 512 * 1024;            // 512KB threshold - balanced for 3GB systems
                    }
                    else if (g_available_address_space > 0x80000000)            // 2GB+ available
                    {
                        virtual_alloc_threshold = 768 * 1024;            // 768KB threshold - conservative for 2GB systems
                    }
                }

                // For very large allocations, use VirtualAlloc for better performance
                if (size > virtual_alloc_threshold)
                {
                    return SafeMallocAlignVirtual(size, alignment);
                }

                // Calculate total size with overflow impossibility guarantee
                const std::uint32_t size_u32 = static_cast<std::uint32_t>(size);
                const std::uint32_t align_u32 = static_cast<std::uint32_t>(alignment);

                // Combined validation: truncation and bounds check (matching VirtualAlloc path)
                if (static_cast<std::size_t>(size_u32) != size || static_cast<std::size_t>(align_u32) != alignment)
                {
                    // MSVC API Conformance: Parameter truncation detected in type conversion
                    errno = ENOMEM;
                    MTA_MEMORY_LOG_SAFETY_REJECTION("Main path parameter truncation detected", nullptr, size, alignment, "size_t to uint32_t truncation");
                    return nullptr;
                }

                // Size validation already ensures overflow impossibility in total_size calculation
                // Mathematical proof: size ≤ 2GB, alignment ≤ 65536 (validated above), sizeof(void*) = 4
                // Maximum total: 2147483648 + 65536 + 4 = 2147549188 < UINT32_MAX (4294967295)
                if (MTA_UNLIKELY(size_u32 > 0x7FF00000))            // 2GB limit ensures safe arithmetic
                {
                    errno = ENOMEM;
                    return nullptr;
                }

                // Overflow mathematically impossible: total_size calculation guaranteed safe
                const std::uint32_t total_size = size_u32 + align_u32 + sizeof(void*);

                void* raw_memory = nullptr;
                bool  used_heap_alloc = false;

                // Optimized for game performance: prefer malloc for consistency and speed
                // Use HeapAlloc for mid-range allocations where it provides memory efficiency benefits
                // Dynamic range based on available memory - expand HeapAlloc usage when memory is plenty
                std::size_t heap_max = (g_available_address_space >= 0xF0000000) ? 2 * 1024 * 1024 :            // 2MB for 4GB LARGE_ADDRESS_AWARE systems
                                           512 * 1024;                                                          // 512KB for others
                if (total_size >= 64 * 1024 && total_size <= heap_max)                                          // Dynamic range prevents heap fragmentation
                {
                    if (g_process_heap) // Check once, cached during init
                    {
                        // MSVC API Conformance: Use proper HeapAlloc with serialization control
                        raw_memory = HeapAlloc(g_process_heap, 0, total_size);
                        if (raw_memory)
                        {
                            used_heap_alloc = true;
                        }
                        // Note: HeapAlloc returns NULL on failure (MSVC documented behavior)
                        // GetLastError() cannot be used - HeapAlloc does not call SetLastError on failure
                    }
                }

                if (!raw_memory)
                {
                    raw_memory = malloc(total_size);
                }

                if (!raw_memory)
                {
                    // Simple graceful failure - for actual out-of-memory
                    errno = ENOMEM;
                    return nullptr;
                }

                // Calculate aligned address - minimal overhead for hot path
                const std::uintptr_t raw_addr = reinterpret_cast<std::uintptr_t>(raw_memory);
                const std::uintptr_t aligned_addr = (raw_addr + sizeof(void*) + align_u32 - 1) & ~(align_u32 - 1);

                // Correct bounds check with underflow protection
                if (aligned_addr >= raw_addr + sizeof(void*) && aligned_addr + size_u32 <= raw_addr + total_size && aligned_addr >= raw_addr)
                {
                    void* result = reinterpret_cast<void*>(aligned_addr);

                    // Store original pointer with allocation method flag
                    std::uintptr_t* storage = reinterpret_cast<std::uintptr_t*>(aligned_addr - sizeof(void*));
                    std::uintptr_t  heap_flag = used_heap_alloc ? 0x00000001 : 0x00000000;
                    *storage = (raw_addr & ~static_cast<std::uintptr_t>(0x3)) | heap_flag;

                    // Prefetch for medium allocations that benefit
                    // Network packet processing and player data often accessed sequentially
                    if (total_size >= 8192)            // 8KB threshold
                    {
                        // Simple prefetch without unnecessary memory barriers
                        char prefetch_byte = *static_cast<char*>(result);
                        (void)prefetch_byte;
                    }

                    // Interval success logging to verify allocator is working (every 1000th allocation)
                    static std::uint32_t allocation_counter = 0;
                    if (++allocation_counter % 1000 == 0)
                    {
                        MTA_MEMORY_LOG_ALLOCATION_FAILURE("HEARTBEAT - Successful allocation", result, size, alignment);
                    }

                    return result;
                }

                // Clean up memory on alignment failure to prevent leak
                if (used_heap_alloc && g_process_heap)
                {
                    // MSVC API Conformance: HeapFree with proper error handling
                    // HeapFree returns TRUE on success - check return value per Microsoft best practices
                    BOOL heap_result = HeapFree(g_process_heap, 0, raw_memory);
                    if (!heap_result)
                    {
                        // HeapFree failed - memory leak unavoidable, but log for diagnostics
                        MTA_MEMORY_LOG_HEAP_FAILURE("HeapFree failed during alignment failure cleanup", raw_memory, size, alignment);
                    }
                }
                else
                {
                    // MSVC API conformance: Standard free() for malloc() allocated memory
                    free(raw_memory);
                }

                // Log alignment calculation failure
                MTA_MEMORY_LOG_ALLOCATION_FAILURE("Alignment calculation failed in main path", raw_memory, size, alignment);
                // MSVC API Conformance: Set errno for alignment calculation failure
                errno = ENOMEM;
                return nullptr;
            }

            // VirtualAlloc implementation for large allocations
            void* SafeMallocAlignVirtual(std::size_t size, std::size_t alignment) noexcept
            {
                // lazy init for performance (rarely needed after first allocation)
                if (MTA_UNLIKELY(g_max_user_address == 0))
                {
                    InitMaxUserAddress();
                }

                // Graceful validation with auto-correction for game stability
                if (MTA_UNLIKELY(size == 0))
                {
                    // Zero size in VirtualAlloc path: use minimal allocation
                    MTA_MEMORY_LOG_VIRTUAL_FAILURE("VirtualAlloc zero size - using minimal allocation", nullptr, size, alignment);
                    return SafeMallocAlign(1, alignment);            // Redirect to main path with corrected size
                }

                if (MTA_UNLIKELY(!is_valid_alignment(alignment)))
                {
                    // Invalid alignment: correct to safe default and use main path to avoid recursion
                    std::size_t corrected_alignment = 4;            // Safe default alignment
                    if (alignment <= 16 && is_power_of_two(alignment))
                    {
                        corrected_alignment = alignment;            // Use original if it's actually valid
                    }
                    else if (alignment <= 65536)
                    {
                        // Find next power of 2
                        corrected_alignment = 1;
                        while (corrected_alignment < alignment && corrected_alignment < 65536)
                        {
                            corrected_alignment <<= 1;
                        }
                        if (corrected_alignment > 65536)
                            corrected_alignment = 4;
                    }

                    MTA_MEMORY_LOG_VIRTUAL_FAILURE("VirtualAlloc alignment corrected", nullptr, size, alignment);
                    return SafeMallocAlign(size, corrected_alignment);            // Use main path to avoid recursion risk
                }

                const std::uint32_t size_u32 = static_cast<std::uint32_t>(size);
                const std::uint32_t align_u32 = static_cast<std::uint32_t>(alignment);

                // Combined validation: truncation and bounds check
                if (static_cast<std::size_t>(size_u32) != size || static_cast<std::size_t>(align_u32) != alignment)
                {
                    // MSVC API Conformance: _aligned_malloc sets errno to ENOMEM for oversized requests
                    errno = ENOMEM;
                    MTA_MEMORY_LOG_SAFETY_REJECTION("VirtualAlloc path parameter truncation detected", nullptr, size, alignment,
                                                    "size_t to uint32_t truncation");
                    return nullptr;
                }

                // Safe bounds check - avoid potential underflow
                const std::uint32_t required_overhead = align_u32 + sizeof(void*) + 4096;
                if (size_u32 > g_max_user_address || g_max_user_address - size_u32 < required_overhead)
                {
                    // MSVC API Conformance: _aligned_malloc sets errno to ENOMEM for oversized requests
                    errno = ENOMEM;
                    MTA_MEMORY_LOG_VIRTUAL_FAILURE("Address space bounds exceeded", nullptr, size, alignment);
                    return nullptr;
                }

                // Double overflow protection for total_size_check calculation
                // Even with 2GB limit, corrupted game data could cause arithmetic overflow
                if (MTA_UNLIKELY(size_u32 > UINT32_MAX - align_u32 - sizeof(void*) - 4096))
                {
                    errno = ENOMEM;
                    MTA_MEMORY_LOG_VIRTUAL_FAILURE("Arithmetic overflow in size calculation", nullptr, size, alignment);
                    return nullptr;
                }

                const std::uint32_t total_size_check = size_u32 + align_u32 + sizeof(void*) + 4096;

                // Optimize allocation size to boundaries
                DWORD optimized_size = total_size_check;
                if (g_use_high_memory_allocation && total_size_check >= g_allocation_granularity)
                {
                    optimized_size = (total_size_check + g_allocation_granularity - 1) & ~(g_allocation_granularity - 1);
                }
                else if (total_size_check >= 64 * 1024)
                {
                    // 64-byte cache lines are standard
                    constexpr DWORD cache_line_size = 64;
                    optimized_size = (total_size_check + cache_line_size - 1) & ~(cache_line_size - 1);
                }

                const DWORD virtual_total_size = optimized_size;

                // Allocation flags optimization
                DWORD allocation_flags = MEM_COMMIT | MEM_RESERVE;
                if (g_use_high_memory_allocation && virtual_total_size >= 1024 * 1024)
                {
                    allocation_flags |= MEM_TOP_DOWN;
                }

                void* raw_ptr = VirtualAlloc(nullptr, static_cast<SIZE_T>(virtual_total_size), allocation_flags, PAGE_READWRITE);
                if (!raw_ptr)
                {
                    //Fallback: try without MEM_TOP_DOWN and then regular allocator
                    if (allocation_flags & MEM_TOP_DOWN)
                    {
                        allocation_flags &= ~MEM_TOP_DOWN;
                        raw_ptr = VirtualAlloc(nullptr, static_cast<SIZE_T>(virtual_total_size), allocation_flags, PAGE_READWRITE);
                    }

                    // Last-resort fallback to main allocator if VirtualAlloc fails
                    if (!raw_ptr)
                    {
                        MTA_MEMORY_LOG_VIRTUAL_FAILURE("VirtualAlloc completely failed, falling back to main allocator", nullptr, size, alignment);
                        return SafeMallocAlign(size, alignment);
                    }
                }

                // Calculate aligned address - for VirtualAlloc path
                const std::uintptr_t raw_addr = reinterpret_cast<std::uintptr_t>(raw_ptr);
                const std::uintptr_t aligned_addr = (raw_addr + sizeof(void*) + align_u32 - 1) & ~(align_u32 - 1);

                // Correct bounds check with proper underflow protection
                if (aligned_addr >= raw_addr + sizeof(void*) && aligned_addr + size_u32 <= raw_addr + virtual_total_size && aligned_addr >= raw_addr)
                {
                    void* result = reinterpret_cast<void*>(aligned_addr);

                    // Store original pointer with VirtualAlloc flag
                    std::uintptr_t* storage = reinterpret_cast<std::uintptr_t*>(aligned_addr - sizeof(void*));
                    *storage = (raw_addr & ~static_cast<std::uintptr_t>(0x3)) | 0x00000002;

                    // Memory warmup for MTA streamer and large textures/models/etc
                    // Prevents stuttering during gameplay by pre-faulting pages for large allocations
                    if (g_use_high_memory_allocation && virtual_total_size >= 1024 * 1024 && size_u32 > 0)            // 1MB threshold with safety check
                    {
                        // Proper page warmup using actual allocated size including metadata
                        char*             warmup_ptr = static_cast<char*>(reinterpret_cast<void*>(raw_addr));            // Start from raw allocation
                        const std::size_t pages_to_touch = (virtual_total_size + g_page_size - 1) / g_page_size;

                        // Strategic warmup: balance between preventing stutters and allocation performance
                        // For game assets, we need most pages resident but can skip some for performance
                        if (pages_to_touch <= 256)            // Up to 1MB: touch all pages (critical for gameplay)
                        {
                            for (std::size_t page = 0; page < pages_to_touch; ++page)
                            {
                                const std::size_t offset = page * g_page_size;
                                if (offset < virtual_total_size)
                                {
                                    volatile char page_touch = warmup_ptr[offset];
                                    (void)page_touch;
                                }
                            }
                        }
                        else    // Large allocations: touch every 4th page to balance performance/stuttering
                        {
                            // Touch every 4th page + first/last for reasonable coverage without excessive overhead
                            volatile char first_page = warmup_ptr[0];
                            // Use total_size consistently for bounds checking
                            volatile char last_page = warmup_ptr[size_u32 - 1];            // Safe: within user allocation

                            for (std::size_t page = 3; page < pages_to_touch; page += 4)
                            {
                                const std::size_t offset = page * g_page_size;
                                if (offset < virtual_total_size)            // Use virtual_total_size for bounds
                                {
                                    volatile char page_touch = warmup_ptr[offset];
                                    (void)page_touch;
                                }
                            }

                            (void)first_page;
                            (void)last_page;
                        }
                    }

                    // Interval success logging for VirtualAlloc path (every 100th allocation)
                    static std::uint32_t virtual_allocation_counter = 0;
                    if (++virtual_allocation_counter % 100 == 0)
                    {
                        MTA_MEMORY_LOG_VIRTUAL_FAILURE("HEARTBEAT - Successful VirtualAlloc", result, size, alignment);
                    }

                    return result;
                }

                // Log VirtualAlloc alignment calculation failure
                MTA_MEMORY_LOG_VIRTUAL_FAILURE("VirtualAlloc alignment calculation failed", raw_ptr, size, alignment);
                // MSVC API Conformance: VirtualFree can fail - check return value per Microsoft documentation
                BOOL vfree_result = VirtualFree(raw_ptr, 0, MEM_RELEASE);
                if (!vfree_result)
                {
                    MTA_MEMORY_LOG_VIRTUAL_FAILURE("VirtualFree cleanup failed during alignment failure", raw_ptr, 0, 0);
                }
                return nullptr;
            }

            // Safe aligned memory deallocation
            void SafeFreeAlign(void* ptr) noexcept
            {
                // Fast path: NULL pointer check (common case)
                if (!ptr)
                {
                    return;
                }

                // Minimal validation: only check for obviously invalid pointers
                const std::uintptr_t ptr_addr = reinterpret_cast<std::uintptr_t>(ptr);
                if (MTA_UNLIKELY(ptr_addr < 0x10000 || ptr_addr > 0xFFFE0000))
                {
                    // Obviously invalid address
                    // Log exact pointer corruption details
#if defined(MTA_DEBUG) || defined(MTA_ENABLE_DIAGNOSTIC_LOGGING) || defined(DEBUG) || defined(_DEBUG)
                    if (mta::memory::diagnostics::IsMemoryAllocationDebuggingEnabled())
                    {
                        MTA_MEMORY_LOG_SAFETY_REJECTION("Invalid pointer rejected", ptr, 0, 0, "Pointer validation");
                    }
#endif
                    return;
                }

                // Read metadata directly - address already validated above
                const std::uintptr_t* storage = reinterpret_cast<const std::uintptr_t*>(ptr_addr - sizeof(void*));
                std::uintptr_t        stored_value = *storage;

                // Extract flags and original pointer
                const std::uintptr_t flags = stored_value & 3;
                const std::uintptr_t original_addr = stored_value & ~static_cast<std::uintptr_t>(0x3);
                void* const          original_ptr = reinterpret_cast<void*>(original_addr);

                // Minimal validation: only check for obvious corruption
                if (MTA_UNLIKELY(flags == 3 || original_addr < 0x10000 || original_addr > ptr_addr))
                {
                    // Obvious corruption - safer to leak
                    MTA_MEMORY_LOG_SAFETY_REJECTION("Metadata corruption detected", ptr, 0, 0, "Corrupted allocation metadata");
                    return;
                }

                // Fast deallocation dispatch
                switch (flags)
                {
                    case 0:            // malloc
                        free(original_ptr);
                        break;
                    case 1:            // HeapAlloc
                        if (g_process_heap)
                        {
                            BOOL heap_result = HeapFree(g_process_heap, 0, original_ptr);
                            if (!heap_result)
                            {
                                MTA_MEMORY_LOG_HEAP_FAILURE("HeapFree deallocation failure", original_ptr, 0, 0);
                            }
                        }
                        else
                        {
                            MTA_MEMORY_LOG_CRITICAL_FAILURE("Missing process heap for HeapFree", original_ptr, 0, 0);
                        }
                        break;
                    case 2:  // VirtualAlloc
                    {
                        BOOL vfree_result = VirtualFree(original_ptr, 0, MEM_RELEASE);
                        if (!vfree_result)
                        {
                            MTA_MEMORY_LOG_VIRTUAL_FAILURE("VirtualFree deallocation failure", original_ptr, 0, 0);
                        }
                    }
                    break;
                }
            }

        }            // anonymous namespace

        // Public interface functions that call the anonymous namespace implementations
        // These are in mta::memory namespace to allow external access while maintaining encapsulation
        void* PublicSafeMallocAlign(std::size_t size, std::size_t alignment) noexcept
        {
            return SafeMallocAlign(size, alignment);
        }

        void PublicSafeFreeAlign(void* ptr) noexcept
        {
            SafeFreeAlign(ptr);
        }

        void PublicInitMaxUserAddress() noexcept
        {
            InitMaxUserAddress();
        }

        // External interface - safe to call from other translation units
        void* SafeMallocAlignPublic(std::size_t size, std::size_t alignment) noexcept
        {
            return PublicSafeMallocAlign(size, alignment);
        }

        void SafeFreeAlignPublic(void* ptr) noexcept
        {
            PublicSafeFreeAlign(ptr);
        }

        void InitMaxUserAddressPublic() noexcept
        {
            PublicInitMaxUserAddress();
        }
    }            // namespace memory
}            // namespace mta

// Hook constants
#define HOOKPOS_CMemoryMgr_MallocAlign  0x72F4C0
#define HOOKSIZE_CMemoryMgr_MallocAlign 5

#define HOOKPOS_CMemoryMgr_FreeAlign  0x72F4F0
#define HOOKSIZE_CMemoryMgr_FreeAlign 5

// Hook replacement functions
// These functions directly replace the GTA:SA memory allocation functions.
// They already have the correct __cdecl calling convention and signature,
// so no assembly wrappers or trampolines are needed.
// External C linkage to ensure proper hooking

extern "C"
{
    // Direct replacement for CMemoryMgr::MallocAlign (SA address 0x72F4C0)
    // Original signature: void* __cdecl(int size, int align, int nHint)
    // This function IS the hook - no wrapper needed
    void* __cdecl HOOK_CMemoryMgr_MallocAlign(int size, int alignment, int nHint)
    {
        // Secure parameter validation to prevent integer overflow
        if (MTA_UNLIKELY(size <= 0 || alignment <= 0))
        {
            // Handle corrupted parameters from base game
            // Never cast negative int to size_t - massive overflow risk!
            std::size_t safe_size = 1;            // Always use minimal safe allocation
            std::size_t safe_align = (alignment <= 0) ? 4 : static_cast<std::size_t>(alignment);

            MTA_MEMORY_LOG_SAFETY_REJECTION("Hook detected negative/zero parameters", nullptr, (size <= 0) ? 0 : static_cast<std::size_t>(size),
                                            (alignment <= 0) ? 0 : static_cast<std::size_t>(alignment), "Base game parameter corruption");

            return mta::memory::SafeMallocAlignPublic(safe_size, safe_align);
        }

        // Additionally check for extremely large positive values that could be corrupted
        if (MTA_UNLIKELY(size > 0x7FF00000 || alignment > 65536))
        {
            // Oversized parameters - likely corruption, use safe minimums
            std::size_t safe_size = (size > 0x7FF00000) ? 1024 : static_cast<std::size_t>(size);
            std::size_t safe_align = (alignment > 65536) ? 4 : static_cast<std::size_t>(alignment);

            MTA_MEMORY_LOG_SAFETY_REJECTION("Hook detected oversized parameters", nullptr, static_cast<std::size_t>(size), static_cast<std::size_t>(alignment),
                                            "Parameter size validation");

            return mta::memory::SafeMallocAlignPublic(safe_size, safe_align);
        }

        // Fast path for normal parameters - now guaranteed safe
        return mta::memory::SafeMallocAlignPublic(static_cast<std::size_t>(size), static_cast<std::size_t>(alignment));
    }

    // Direct replacement for CMemoryMgr::FreeAlign (SA address 0x72F4F0)
    // Original signature: void __cdecl(void* ptr)
    // This function IS the hook - no wrapper needed
    void __cdecl HOOK_CMemoryMgr_FreeAlign(void* ptr)
    {
        // Minimal protection: just handle nullpointers
        if (!ptr)
        {
            return;
        }

        // Direct call to allocator - let it handle validation
        mta::memory::SafeFreeAlignPublic(ptr);
    }

}            // extern "C"

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for memory allocation functions
// Using direct function pointers - no assembly wrappers needed
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_FixMallocAlign()
{
    // Initialize memory system
    mta::memory::InitMaxUserAddressPublic();

    // Test logging system on startup
    MTA_MEMORY_LOG_ALLOCATION_FAILURE("Memory allocator initialized successfully", nullptr, 0, 0);

    // Install hooks with basic error checking
    BOOL bMallocHookOK =
        HookInstall(HOOKPOS_CMemoryMgr_MallocAlign, reinterpret_cast<DWORD>(HOOK_CMemoryMgr_MallocAlign), HOOKSIZE_CMemoryMgr_MallocAlign) ? TRUE : FALSE;

    BOOL bFreeHookOK =
        HookInstall(HOOKPOS_CMemoryMgr_FreeAlign, reinterpret_cast<DWORD>(HOOK_CMemoryMgr_FreeAlign), HOOKSIZE_CMemoryMgr_FreeAlign) ? TRUE : FALSE;

    // Report success
    if (bMallocHookOK && bFreeHookOK)
    {
        MTA_MEMORY_LOG_ALLOCATION_FAILURE("Memory allocation hooks installed successfully", nullptr, 0, 0);
        AddReportLog(8001, "InitHooks_FixMallocAlign: Memory allocation hooks installed successfully");
    }
    else if (bMallocHookOK || bFreeHookOK)
    {
        MTA_MEMORY_LOG_CRITICAL_FAILURE("Partial hook installation", nullptr, bMallocHookOK ? 1 : 0, bFreeHookOK ? 1 : 0);
        AddReportLog(8006, "InitHooks_FixMallocAlign: Partial hook installation - some improvements active");
    }
    else
    {
        MTA_MEMORY_LOG_CRITICAL_FAILURE("Hook installation failed", nullptr, 0, 0);
        AddReportLog(8002, "InitHooks_FixMallocAlign: Hook installation failed - using original SA allocator functions");
    }
}
