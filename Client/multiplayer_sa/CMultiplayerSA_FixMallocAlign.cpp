/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_FixMallocAlign.cpp
 *  PURPOSE:     Fixed version of GTA:SA CMemoryMgr
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

namespace mta::memory
{
    constexpr bool is_power_of_two(std::size_t value) noexcept
    {
        return value != 0 && (value & (value - 1)) == 0;
    }

    void* SafeMallocAlignVirtual(std::size_t size, std::size_t alignment) noexcept;

    // Safe aligned memory allocation replacement for CMemoryMgr::MallocAlign
    // Must match original GTA:SA behavior exactly - stores original pointer at result-4
    [[nodiscard]] void* SafeMallocAlign(std::size_t size, std::size_t alignment) noexcept
    {
        // Validate alignment FIRST to prevent undefined behavior in calculations
        if (alignment == 0 || !is_power_of_two(alignment) || alignment > 8192)
        {
            return nullptr;
        }

        // Safety checks - these are fast and prevent crashes from direct calls
        if (size == 0)
        {
            return nullptr;
        }

        // Check for x86 size limits (2GB max for user-mode allocations)
        // Reserve space for actual alignment overhead
        if (size > 0x7FFFFFFF - alignment - sizeof(void*))
        {
            return nullptr;
        }

        // For very large allocations, use VirtualAlloc
        if (size > 1024 * 1024)            // 1MB threshold
        {
            return SafeMallocAlignVirtual(size, alignment);
        }

        // Calculate total size needed with x86-specific overflow protection
        const std::uint32_t size_u32 = static_cast<std::uint32_t>(size);
        const std::uint32_t align_u32 = static_cast<std::uint32_t>(alignment);

        // Validate that casting to uint32 didn't truncate the values (x86 safety)
        if (static_cast<std::size_t>(size_u32) != size || static_cast<std::size_t>(align_u32) != alignment)
        {
            return nullptr;
        }

        const std::uint32_t total_size = size_u32 + align_u32 + sizeof(void*);

        // Check for overflow and validate total size
        if (total_size < size_u32 || total_size > 0x7FFFFFFF)
        {
            return nullptr;
        }

        void* raw_memory = nullptr;
        try
        {
            raw_memory = malloc(total_size);
            if (!raw_memory)
            {
                return nullptr;
            }

            // Calculate aligned address like original GTA:SA with safer arithmetic
            std::uint32_t raw_addr = reinterpret_cast<std::uint32_t>(raw_memory);

            // Check for arithmetic overflow in alignment calculation
            // We need to check if (raw_addr + align_u32 + sizeof(void*)) would overflow uint32_t
            if (raw_addr > 0xFFFFFFFFU - align_u32 - sizeof(void*))
            {
                free(raw_memory);
                return nullptr;
            }

            std::uint32_t aligned_addr = (raw_addr + align_u32 + sizeof(void*)) & ~(align_u32 - 1);

            // Enhanced bounds checking for pointer storage
            if (aligned_addr < raw_addr + sizeof(void*) ||            // Minimum offset check (includes wraparound protection)
                aligned_addr > raw_addr + total_size)                 // Upper bound check
            {
                free(raw_memory);
                return nullptr;
            }

            void* result = reinterpret_cast<void*>(aligned_addr);

            // Store original pointer exactly like GTA:SA
            *reinterpret_cast<void**>(aligned_addr - sizeof(void*)) = raw_memory;

            return result;
        }
        catch (...)
        {
            // Clean up allocated memory if an exception occurred
            if (raw_memory)
            {
                free(raw_memory);
            }
            return nullptr;
        }
    }

    // VirtualAlloc implementation for large allocations
    [[nodiscard]] void* SafeMallocAlignVirtual(std::size_t size, std::size_t alignment) noexcept
    {
        // Validate parameters first to prevent undefined behavior
        if (alignment == 0 || !is_power_of_two(alignment) || alignment > 8192)
        {
            return nullptr;
        }

        if (size == 0)
        {
            return nullptr;
        }

        const std::uint32_t size_u32 = static_cast<std::uint32_t>(size);
        const std::uint32_t align_u32 = static_cast<std::uint32_t>(alignment);

        // Validate that casting to uint32 didn't truncate the values
        if (static_cast<std::size_t>(size_u32) != size || static_cast<std::size_t>(align_u32) != alignment)
        {
            return nullptr;
        }

        // Check for overflow before allocation
        if (size_u32 > 0x7FFFFFFF - align_u32 - sizeof(void*) - 4096)
        {
            return nullptr;
        }

        // Calculate total size with overflow protection
        std::uint32_t total_size_check = size_u32 + align_u32 + sizeof(void*) + 4096;
        if (total_size_check < size_u32)            // Overflow occurred
        {
            return nullptr;
        }

        const DWORD total_size = total_size_check;

        void* raw_ptr = VirtualAlloc(nullptr, static_cast<SIZE_T>(total_size), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!raw_ptr)
        {
            return nullptr;
        }

        // Calculate aligned address with overflow protection
        std::uint32_t raw_addr = reinterpret_cast<std::uint32_t>(raw_ptr);

        // Check for arithmetic overflow in alignment calculation
        // We need to check if (raw_addr + align_u32 + sizeof(void*)) would overflow uint32_t
        if (raw_addr > 0xFFFFFFFFU - align_u32 - sizeof(void*))
        {
            VirtualFree(raw_ptr, 0, MEM_RELEASE);
            return nullptr;
        }

        std::uint32_t aligned_addr = (raw_addr + align_u32 + sizeof(void*)) & ~(align_u32 - 1);

        // Enhanced bounds checking with wraparound detection
        if (aligned_addr < raw_addr + sizeof(void*) ||            // Minimum offset check (includes wraparound protection)
            aligned_addr > raw_addr + total_size)                 // Upper bound check
        {
            VirtualFree(raw_ptr, 0, MEM_RELEASE);
            return nullptr;
        }

        void* result = reinterpret_cast<void*>(aligned_addr);

        // Store original pointer for SafeFreeAlign compatibility
        *reinterpret_cast<void**>(aligned_addr - sizeof(void*)) = raw_ptr;

        return result;
    }

    // Safe aligned memory deallocation
    void SafeFreeAlign(void* ptr) noexcept
    {
        if (!ptr)
            return;

        try
        {
            std::uint32_t ptr_addr = reinterpret_cast<std::uint32_t>(ptr);

            // Enhanced pointer validation for x86 user-mode address space
            // x86 user-mode: 0x00010000 - 0x7FFFFFFF (avoid null page and kernel space)
            if (ptr_addr < 0x10000 || ptr_addr >= 0x80000000)
            {
                return;
            }

            // Check pointer alignment - must be at least 4-byte aligned for x86
            if ((ptr_addr & 3) != 0)
            {
                return;
            }

            // Validate that we can safely read the stored pointer
            void* stored_ptr_location = reinterpret_cast<void*>(ptr_addr - sizeof(void*));

            // Check if the memory location is readable using VirtualQuery
            MEMORY_BASIC_INFORMATION mbi_check;
            SIZE_T                   query_result = VirtualQuery(stored_ptr_location, &mbi_check, sizeof(mbi_check));
            if (query_result == 0 || mbi_check.State != MEM_COMMIT)
            {
                return;
            }

            // Check if memory is readable (any readable protection flag)
            const DWORD readable_flags = PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_WRITECOPY;
            if (!(mbi_check.Protect & readable_flags))
            {
                return;
            }

            void*         original_ptr = *reinterpret_cast<void**>(ptr_addr - sizeof(void*));
            std::uint32_t original_addr = reinterpret_cast<std::uint32_t>(original_ptr);

            // Validation for original pointer
            // Must be in valid user-mode range (alignment checked separately)
            if (original_addr < 0x10000 || original_addr >= 0x7FFFFFFF)
            {
                return;
            }

            // Validate alignment is reasonable (4-byte minimum for x86)
            if ((original_addr & 3) != 0)
            {
                return;
            }

            // Determine allocation method using VirtualQuery
            MEMORY_BASIC_INFORMATION mbi;
            SIZE_T                   mbi_result = VirtualQuery(original_ptr, &mbi, sizeof(mbi));
            if (mbi_result != 0)
            {
                // Check if this is VirtualAlloc memory
                const std::uint32_t base_addr = reinterpret_cast<std::uint32_t>(mbi.AllocationBase);

                // Safe overflow-protected calculation for region end
                std::uint32_t region_end;
                if (base_addr > 0xFFFFFFFFU - static_cast<std::uint32_t>(mbi.RegionSize))
                {
                    // Overflow would occur - this indicates corrupted memory info
                    return;
                }
                region_end = base_addr + static_cast<std::uint32_t>(mbi.RegionSize);

                if (mbi.Type == MEM_PRIVATE && mbi.State == MEM_COMMIT && original_addr >= base_addr && original_addr < region_end)
                {
                    VirtualFree(original_ptr, 0, MEM_RELEASE);
                }
                else
                {
                    free(original_ptr);
                }
            }
            else
            {
                // VirtualQuery failed - assume malloc
                free(original_ptr);
            }
        }
        catch (...)
        {
            // Ignore exceptions in free - standard behavior
        }
    }
}            // namespace mta::memory

// Hook constants
#define HOOKPOS_CMemoryMgr_MallocAlign  0x72F4C0
#define HOOKSIZE_CMemoryMgr_MallocAlign 5

#define HOOKPOS_CMemoryMgr_FreeAlign  0x72F4F0
#define HOOKSIZE_CMemoryMgr_FreeAlign 5

// Hook replacement functions
// These functions directly replace the GTA:SA memory allocation functions.
// They already have the correct __cdecl calling convention and signature,
// so no assembly wrappers or trampolines are needed.
// No extern "C" to match the rest of MTA's hook style

// Direct replacement for CMemoryMgr::MallocAlign (SA address 0x72F4C0)
// Original signature: void* __cdecl(int size, int align, int nHint)
// This function IS the hook - no wrapper needed
void* __cdecl HOOK_CMemoryMgr_MallocAlign(int size, int alignment, int nHint)
{
    if (size <= 0 || size > 0x7FFFFFFF)
    {
        return nullptr;
    }

    if (alignment <= 0 || alignment > 8192 || !mta::memory::is_power_of_two(static_cast<std::size_t>(alignment)))
    {
        return nullptr;
    }

    if (static_cast<std::uint32_t>(size) > 0x7FFFFFFF - static_cast<std::uint32_t>(alignment) - sizeof(void*))
    {
        return nullptr;
    }

    // Ignore nHint parameter (not used in our implementation)
    return mta::memory::SafeMallocAlign(static_cast<std::uint32_t>(size), static_cast<std::uint32_t>(alignment));
}

// Direct replacement for CMemoryMgr::FreeAlign (SA address 0x72F4F0)
// Original signature: void __cdecl(void* ptr)
// This function IS the hook - no wrapper needed
void __cdecl HOOK_CMemoryMgr_FreeAlign(void* ptr)
{
    mta::memory::SafeFreeAlign(ptr);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for memory allocation functions
// Using direct function pointers - no assembly wrappers needed
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_FixMallocAlign()
{
    try
    {
        // Install hooks using the direct C functions
        // The functions already have the correct __cdecl calling convention and matching signatures, so they work as direct replacements
        HookInstall(HOOKPOS_CMemoryMgr_MallocAlign, reinterpret_cast<DWORD>(HOOK_CMemoryMgr_MallocAlign), HOOKSIZE_CMemoryMgr_MallocAlign);
        HookInstall(HOOKPOS_CMemoryMgr_FreeAlign, reinterpret_cast<DWORD>(HOOK_CMemoryMgr_FreeAlign), HOOKSIZE_CMemoryMgr_FreeAlign);
    }
    catch (...)
    {
        // Silent failure - use original functions
    }
}
