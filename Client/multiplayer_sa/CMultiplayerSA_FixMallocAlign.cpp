/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_FixMallocAlign.cpp
 *  PURPOSE:     Reimplement GTA:SA CMemoryMgr with performance and stability fixes
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <cerrno>

namespace mta
{
namespace memory
{
    constexpr std::size_t   VIRTUALALLOC_THRESHOLD = 1024 * 1024;
    constexpr std::uint32_t VIRTUALALLOC_PADDING = 64;
    constexpr std::size_t   MAX_ALIGNMENT = 8192;
    constexpr std::uint32_t NULL_PAGE_BOUNDARY = 0x10000;
    constexpr std::uint32_t MAX_ADDRESS_SPACE = 0xFFFFFFFF;
    constexpr std::uint32_t POINTER_SIZE = 4;

    constexpr bool is_valid_alignment(std::size_t alignment) noexcept
    {
        return alignment != 0 && (alignment & (alignment - 1)) == 0;
    }

    void* SafeMallocAlignVirtual(std::size_t size, std::size_t alignment) noexcept;

    // Aligned malloc - stores pointer at result-4
    void* SafeMallocAlign(std::size_t size, std::size_t alignment) noexcept
    {
        // Check alignment
        if (alignment == 0)
        {
            errno = EINVAL;
            return nullptr;
        }

        if (size == 0)
        {
            size = 1;
        }

        if (!is_valid_alignment(alignment) || alignment > MAX_ALIGNMENT)
        {
            errno = EINVAL;
            return nullptr;
        }

        // Fast path for small allocations
        if (alignment <= 16 && size <= 512)
        {
            const std::uint32_t size_u32 = static_cast<std::uint32_t>(size);
            const std::uint32_t align_u32 = static_cast<std::uint32_t>(alignment);

            // Prevent intermediate overflow
            if (size_u32 > UINT32_MAX - align_u32)
            {
                errno = ENOMEM;
                return nullptr;
            }
            // Now safe to add size_u32 + align_u32
            if (size_u32 + align_u32 > UINT32_MAX - POINTER_SIZE)
            {
                errno = ENOMEM;
                return nullptr;
            }
            const std::uint32_t total_size = size_u32 + align_u32 + POINTER_SIZE;

            void* raw_memory = malloc(total_size);
            if (!raw_memory)
            {
                errno = ENOMEM;
                return nullptr;
            }

            const std::uint32_t raw_addr = reinterpret_cast<std::uint32_t>(raw_memory);

            if (raw_addr > MAX_ADDRESS_SPACE - POINTER_SIZE - align_u32 + 1)
            {
                free(raw_memory);
                errno = ENOMEM;
                return nullptr;
            }

            const std::uint32_t aligned_addr = (raw_addr + POINTER_SIZE + align_u32 - 1) & ~(align_u32 - 1);

            if (aligned_addr < raw_addr + POINTER_SIZE || aligned_addr + size_u32 > raw_addr + total_size || aligned_addr + size_u32 < aligned_addr)
            {
                free(raw_memory);
                errno = EINVAL;
                return nullptr;
            }

            void* result = reinterpret_cast<void*>(aligned_addr);

            // Validate store location
            void** store_location = reinterpret_cast<void**>(aligned_addr - POINTER_SIZE);
            if (reinterpret_cast<std::uint32_t>(store_location) < raw_addr ||
                reinterpret_cast<std::uint32_t>(store_location) > raw_addr + total_size - POINTER_SIZE)
            {
                free(raw_memory);
                errno = EFAULT;
                return nullptr;
            }

            *store_location = raw_memory;

            return result;
        }

        // Use VirtualAlloc for large sizes
        if (size > VIRTUALALLOC_THRESHOLD)
        {
            return SafeMallocAlignVirtual(size, alignment);
        }

        if (size > 0xFFFFFFFF || alignment > 0xFFFFFFFF)
        {
            errno = ENOMEM;
            return nullptr;
        }

        const std::uint32_t size_u32 = static_cast<std::uint32_t>(size);
        const std::uint32_t align_u32 = static_cast<std::uint32_t>(alignment);

        // Prevent intermediate overflow
        if (size_u32 > UINT32_MAX - align_u32)
        {
            errno = ENOMEM;
            return nullptr;
        }
        // Now safe to add size_u32 + align_u32
        if (size_u32 + align_u32 > UINT32_MAX - POINTER_SIZE)
        {
            errno = ENOMEM;
            return nullptr;
        }
        const std::uint32_t total_size = size_u32 + align_u32 + POINTER_SIZE;

        void* raw_memory = malloc(total_size);
        if (!raw_memory)
        {
            errno = ENOMEM;
            return nullptr;
        }

        const std::uint32_t raw_addr = reinterpret_cast<std::uint32_t>(raw_memory);

        if (raw_addr > MAX_ADDRESS_SPACE - POINTER_SIZE - align_u32 + 1)
        {
            free(raw_memory);
            errno = ENOMEM;
            return nullptr;
        }

        const std::uint32_t aligned_addr = (raw_addr + POINTER_SIZE + align_u32 - 1) & ~(align_u32 - 1);

        if (aligned_addr < raw_addr + POINTER_SIZE || aligned_addr + size_u32 > raw_addr + total_size || aligned_addr + size_u32 < aligned_addr)
        {
            free(raw_memory);
            errno = EINVAL;
            return nullptr;
        }

        void* result = reinterpret_cast<void*>(aligned_addr);

        void** store_location = reinterpret_cast<void**>(aligned_addr - POINTER_SIZE);
        if (reinterpret_cast<std::uint32_t>(store_location) < raw_addr ||
            reinterpret_cast<std::uint32_t>(store_location) > raw_addr + total_size - POINTER_SIZE)
        {
            free(raw_memory);
            errno = EFAULT;
            return nullptr;
        }

        *store_location = raw_memory;

        return result;
    }

    // VirtualAlloc path
    void* SafeMallocAlignVirtual(std::size_t size, std::size_t alignment) noexcept
    {
        // Check params
        if (alignment == 0 || !is_valid_alignment(alignment) || alignment > MAX_ALIGNMENT)
        {
            errno = EINVAL;
            return nullptr;
        }

        if (size == 0)
        {
            size = 1;
        }

        if (size > 0xFFFFFFFF || alignment > 0xFFFFFFFF)
        {
            errno = ENOMEM;
            return nullptr;
        }

        const std::uint32_t size_u32 = static_cast<std::uint32_t>(size);
        const std::uint32_t align_u32 = static_cast<std::uint32_t>(alignment);
        const std::uint32_t padding = (align_u32 <= 64) ? 32 : VIRTUALALLOC_PADDING;

        if (align_u32 > UINT32_MAX - POINTER_SIZE)
        {
            errno = ENOMEM;
            return nullptr;
        }
        if (align_u32 + POINTER_SIZE > UINT32_MAX - padding)
        {
            errno = ENOMEM;
            return nullptr;
        }
        if (size_u32 > UINT32_MAX - align_u32 - POINTER_SIZE - padding)
        {
            errno = ENOMEM;
            return nullptr;
        }

        const DWORD total_size = size_u32 + align_u32 + POINTER_SIZE + padding;

        void* raw_ptr = VirtualAlloc(nullptr, static_cast<SIZE_T>(total_size), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!raw_ptr)
        {
            errno = ENOMEM;
            return nullptr;
        }

        const std::uint32_t raw_addr = reinterpret_cast<std::uint32_t>(raw_ptr);

        if (raw_addr > MAX_ADDRESS_SPACE - POINTER_SIZE - align_u32 + 1)
        {
            BOOL vfree_result = VirtualFree(raw_ptr, 0, MEM_RELEASE);
            (void)vfree_result;
            errno = ENOMEM;
            return nullptr;
        }

        const std::uint32_t aligned_addr = (raw_addr + POINTER_SIZE + align_u32 - 1) & ~(align_u32 - 1);

        if (aligned_addr < raw_addr + POINTER_SIZE || aligned_addr + size_u32 > raw_addr + total_size || aligned_addr + size_u32 < aligned_addr)
        {
            BOOL vfree_result = VirtualFree(raw_ptr, 0, MEM_RELEASE);
            (void)vfree_result;
            errno = EINVAL;
            return nullptr;
        }

        void* result = reinterpret_cast<void*>(aligned_addr);

        // Validate store location
        void** store_location = reinterpret_cast<void**>(aligned_addr - POINTER_SIZE);
        if (reinterpret_cast<std::uint32_t>(store_location) < raw_addr ||
            reinterpret_cast<std::uint32_t>(store_location) > raw_addr + total_size - POINTER_SIZE)
        {
            BOOL vfree_result = VirtualFree(raw_ptr, 0, MEM_RELEASE);
            (void)vfree_result;
            errno = EFAULT;
            return nullptr;
        }

        *store_location = raw_ptr;

        return result;
    }

    // Free aligned memory
    void SafeFreeAlign(void* ptr) noexcept
    {
        if (!ptr)
            return;

        const std::uint32_t ptr_addr = reinterpret_cast<std::uint32_t>(ptr);

        if (ptr_addr < NULL_PAGE_BOUNDARY)
        {
            return;
        }

        void** read_location = reinterpret_cast<void**>(ptr_addr - POINTER_SIZE);

        // Validate memory readable
        MEMORY_BASIC_INFORMATION mbi_read;
        SIZE_T                   mbi_read_result = VirtualQuery(read_location, &mbi_read, sizeof(mbi_read));

        if (mbi_read_result != sizeof(mbi_read) || mbi_read.State != MEM_COMMIT || mbi_read.Protect == PAGE_NOACCESS || mbi_read.Protect == PAGE_GUARD)
        {
            return;
        }

        void* original_ptr = *read_location;

        if (!original_ptr)
        {
            return;
        }

        const std::uint32_t original_addr = reinterpret_cast<std::uint32_t>(original_ptr);

        if (original_addr >= ptr_addr)
        {
            return;            // Impossible for our allocator
        }

        const std::uint32_t distance = ptr_addr - original_addr;
        if (distance > MAX_ALIGNMENT + POINTER_SIZE)
        {
            return;            // Beyond maximum possible alignment
        }

        if (ptr_addr < POINTER_SIZE || original_addr > ptr_addr - POINTER_SIZE)
        {
            return;            // Violates our storage pattern
        }

        if (original_addr >= 0xFFFF0000 || original_addr < NULL_PAGE_BOUNDARY)
        {
            return;
        }

        MEMORY_BASIC_INFORMATION mbi;
        SIZE_T                   mbi_result = VirtualQuery(original_ptr, &mbi, sizeof(mbi));

        if (mbi_result == sizeof(mbi))
        {
            const std::uint32_t base_addr = reinterpret_cast<std::uint32_t>(mbi.AllocationBase);

            // Validate region size
            if (mbi.RegionSize > 0 && mbi.RegionSize <= static_cast<SIZE_T>(MAX_ADDRESS_SPACE) &&
                base_addr <= MAX_ADDRESS_SPACE - static_cast<std::uint32_t>(mbi.RegionSize))
            {
                const std::uint32_t region_size_u32 = static_cast<std::uint32_t>(mbi.RegionSize);
                const std::uint32_t region_end = base_addr + region_size_u32;

                // Use VirtualFree if matches
                if (mbi.Type == MEM_PRIVATE && mbi.State == MEM_COMMIT && original_addr >= base_addr && original_addr < region_end)
                {
                    BOOL vfree_result = VirtualFree(mbi.AllocationBase, 0, MEM_RELEASE);
                    (void)vfree_result;
                    return;
                }
            }
        }

        // Use free for malloc
        free(original_ptr);
    }
}            // namespace memory
}            // namespace mta

// Hook constants
#define HOOKPOS_CMemoryMgr_MallocAlign  0x72F4C0
#define HOOKSIZE_CMemoryMgr_MallocAlign 5

#define HOOKPOS_CMemoryMgr_FreeAlign  0x72F4F0
#define HOOKSIZE_CMemoryMgr_FreeAlign 5

// Hook functions

void* __cdecl HOOK_CMemoryMgr_MallocAlign(int size, int alignment, int nHint)
{
    if (size <= 0 || alignment <= 0)
    {
        std::size_t safe_size = (size <= 0) ? 1 : static_cast<std::size_t>(size);
        std::size_t safe_align = (alignment <= 0) ? 4 : static_cast<std::size_t>(alignment);
        return mta::memory::SafeMallocAlign(safe_size, safe_align);
    }

    // Only check maximum (minimum handled above)
    if (alignment > static_cast<int>(mta::memory::MAX_ALIGNMENT))
    {
        errno = EINVAL;
        return nullptr;
    }

    if (size > static_cast<int>(0x7FFFFFFF) || alignment > static_cast<int>(0x7FFFFFFF))
    {
        errno = EINVAL;
        return nullptr;
    }

    (void)nHint;
    return mta::memory::SafeMallocAlign(static_cast<std::size_t>(size), static_cast<std::size_t>(alignment));
}

void __cdecl HOOK_CMemoryMgr_FreeAlign(void* ptr)
{
    mta::memory::SafeFreeAlign(ptr);
}

void CMultiplayerSA::InitHooks_FixMallocAlign()
{
    HookInstall(HOOKPOS_CMemoryMgr_MallocAlign, reinterpret_cast<DWORD>(HOOK_CMemoryMgr_MallocAlign), HOOKSIZE_CMemoryMgr_MallocAlign);
    HookInstall(HOOKPOS_CMemoryMgr_FreeAlign, reinterpret_cast<DWORD>(HOOK_CMemoryMgr_FreeAlign), HOOKSIZE_CMemoryMgr_FreeAlign);
}
