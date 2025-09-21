/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMemoryAllocatorDiagnostics.h
 *  PURPOSE:     Memory allocator diagnostic logging system
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstddef>    // For std::size_t
#include <cstdint>    // For std::uintptr_t

namespace mta
{
    namespace memory
    {
        namespace diagnostics
        {
            // Check if memory allocation debugging should be enabled
            // Returns true in debug builds OR when user has enabled EDiagnosticDebug::BAD_ALLOC (#0000 Memory allocation debug) in MTA advanced settings > "Debug setting:"
            bool IsMemoryAllocationDebuggingEnabled() noexcept;

            // Core diagnostic logging function
            void LogAllocationFailure(const char* reason, void* ptr = nullptr, std::size_t size = 0, std::size_t alignment = 0,
                                      void* caller = nullptr) noexcept;

            // Specialized logging functions for different failure scenarios
            void LogHeapAllocationFailure(const char* reason, void* ptr, std::size_t size, std::size_t alignment, void* caller) noexcept;
            void LogVirtualAllocFailure(const char* reason, void* ptr, std::size_t size, std::size_t alignment, void* caller) noexcept;
            void LogSafetyRejection(const char* reason, void* ptr, std::size_t size, std::size_t alignment, const char* safety_condition,
                                    void* caller) noexcept;
            void LogCriticalSystemFailure(const char* reason, void* ptr, std::size_t size, std::size_t alignment, void* caller) noexcept;

            // Memory pressure analysis
            void LogMemoryPressureDetails() noexcept;
        }
    }
}

// Conditional compilation macros for diagnostic logging
// These provide zero overhead when diagnostics are disabled
#if defined(MTA_DEBUG) || defined(MTA_ENABLE_DIAGNOSTIC_LOGGING) || defined(DEBUG) || defined(_DEBUG)
    #include <intrin.h>    // For _ReturnAddress() intrinsic

    // Debug builds: always enabled
    #if defined(MTA_DEBUG) || defined(DEBUG) || defined(_DEBUG)
        #define MTA_MEMORY_LOG_ALLOCATION_FAILURE(reason, ptr, size, alignment) \
            mta::memory::diagnostics::LogAllocationFailure(reason, ptr, size, alignment, _ReturnAddress())
        
        #define MTA_MEMORY_LOG_DEALLOCATION_FAILURE(reason, ptr) \
            mta::memory::diagnostics::LogAllocationFailure(reason, ptr, 0, 0, _ReturnAddress())
        
        #define MTA_MEMORY_LOG_HEAP_FAILURE(reason, ptr, size, alignment) \
            mta::memory::diagnostics::LogHeapAllocationFailure(reason, ptr, size, alignment, _ReturnAddress())
        
        #define MTA_MEMORY_LOG_VIRTUAL_FAILURE(reason, ptr, size, alignment) \
            mta::memory::diagnostics::LogVirtualAllocFailure(reason, ptr, size, alignment, _ReturnAddress())
        
        #define MTA_MEMORY_LOG_SAFETY_REJECTION(reason, ptr, size, alignment, condition) \
            mta::memory::diagnostics::LogSafetyRejection(reason, ptr, size, alignment, condition, _ReturnAddress())
        
        #define MTA_MEMORY_LOG_CRITICAL_FAILURE(reason, ptr, size, alignment) \
            mta::memory::diagnostics::LogCriticalSystemFailure(reason, ptr, size, alignment, _ReturnAddress())
    
    // Release builds: conditional on user setting
    #else
        #define MTA_MEMORY_LOG_ALLOCATION_FAILURE(reason, ptr, size, alignment) \
            do { if (mta::memory::diagnostics::IsMemoryAllocationDebuggingEnabled()) \
                mta::memory::diagnostics::LogAllocationFailure(reason, ptr, size, alignment, _ReturnAddress()); } while(0)
        
        #define MTA_MEMORY_LOG_DEALLOCATION_FAILURE(reason, ptr) \
            do { if (mta::memory::diagnostics::IsMemoryAllocationDebuggingEnabled()) \
                mta::memory::diagnostics::LogAllocationFailure(reason, ptr, 0, 0, _ReturnAddress()); } while(0)
        
        #define MTA_MEMORY_LOG_HEAP_FAILURE(reason, ptr, size, alignment) \
            do { if (mta::memory::diagnostics::IsMemoryAllocationDebuggingEnabled()) \
                mta::memory::diagnostics::LogHeapAllocationFailure(reason, ptr, size, alignment, _ReturnAddress()); } while(0)
        
        #define MTA_MEMORY_LOG_VIRTUAL_FAILURE(reason, ptr, size, alignment) \
            do { if (mta::memory::diagnostics::IsMemoryAllocationDebuggingEnabled()) \
                mta::memory::diagnostics::LogVirtualAllocFailure(reason, ptr, size, alignment, _ReturnAddress()); } while(0)
        
        #define MTA_MEMORY_LOG_SAFETY_REJECTION(reason, ptr, size, alignment, condition) \
            do { if (mta::memory::diagnostics::IsMemoryAllocationDebuggingEnabled()) \
                mta::memory::diagnostics::LogSafetyRejection(reason, ptr, size, alignment, condition, _ReturnAddress()); } while(0)
        
        #define MTA_MEMORY_LOG_CRITICAL_FAILURE(reason, ptr, size, alignment) \
            do { if (mta::memory::diagnostics::IsMemoryAllocationDebuggingEnabled()) \
                mta::memory::diagnostics::LogCriticalSystemFailure(reason, ptr, size, alignment, _ReturnAddress()); } while(0)
    #endif

#else
    // Release builds without diagnostic support: no-op macros for zero overhead
    #define MTA_MEMORY_LOG_ALLOCATION_FAILURE(reason, ptr, size, alignment)         ((void)0)
    #define MTA_MEMORY_LOG_DEALLOCATION_FAILURE(reason, ptr)                        ((void)0)
    #define MTA_MEMORY_LOG_HEAP_FAILURE(reason, ptr, size, alignment)               ((void)0)
    #define MTA_MEMORY_LOG_VIRTUAL_FAILURE(reason, ptr, size, alignment)            ((void)0)
    #define MTA_MEMORY_LOG_SAFETY_REJECTION(reason, ptr, size, alignment, condition) ((void)0)
    #define MTA_MEMORY_LOG_CRITICAL_FAILURE(reason, ptr, size, alignment)           ((void)0)

#endif // MTA_DEBUG || MTA_ENABLE_DIAGNOSTIC_LOGGING