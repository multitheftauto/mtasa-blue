/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMemoryAllocatorDiagnostics.cpp
 *  PURPOSE:     Memory allocator diagnostic logging system implementation
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMemoryAllocatorDiagnostics.h"
#include "SharedUtil.h"
#include "SharedUtil.Game.h"
#include <errno.h>
#include <cstdint>
#include <cstring>
#include <psapi.h>

// Forward declarations
class CCore;

namespace mta
{
    namespace memory
    {
        // Forward declarations to access global variables from the allocator
        // These are defined in CMultiplayerSA_FixMallocAlign.cpp
        extern std::uintptr_t g_max_user_address;
        extern std::uintptr_t g_available_address_space;
        extern bool           g_is_64bit_host_os;
        extern HANDLE         g_process_heap;

        namespace diagnostics
        {
            // Forward declaration for g_pCore (defined in core)
            extern CCore* g_pCore;

            // Check if memory allocation debugging should be enabled
            bool IsMemoryAllocationDebuggingEnabled() noexcept
            {
#if defined(MTA_DEBUG) || defined(DEBUG) || defined(_DEBUG)
                // Always enabled in debug build
                return true;
#else
                // Check if user selected "#0000 Memory allocation debug" (EDiagnosticDebug::BAD_ALLOC) in Advanced settings menu
                try {
                    if (g_pCore != nullptr)
                    {
                        return g_pCore->GetDiagnosticDebug() == EDiagnosticDebug::BAD_ALLOC;
                    }
                } catch (...) {
                    // Silently handle any exceptions when checking diagnostic debug state
                }
                return false;
#endif
            }
            
            // Get the path to memdebug.txt in MTA root directory (next to Multi Theft Auto.exe)
            static SString GetMemDebugFilePath() noexcept
            {
                static SString strDebugFilePath;
                if (strDebugFilePath.empty())
                {
                    try {
                        // Use the existing GetMTASABaseDir() function to get root path
                        SString mtaRootPath = GetMTASABaseDir();
                        strDebugFilePath = PathJoin(mtaRootPath, "memdebug.txt");
                        
                        // Test if path determination is working
#if defined(MTA_DEBUG) || defined(DEBUG) || defined(_DEBUG)
                        try {
                            FILE* pathTest = fopen("memdebug_path_test.txt", "w");
                            if (pathTest)
                            {
                                fprintf(pathTest, "Debug file path: %s\n", strDebugFilePath.c_str());
                                fprintf(pathTest, "MTA_DEBUG defined: YES\n");
                                fclose(pathTest);
                            }
                        } catch (...) {}
#endif
                    } catch (...) {
                        strDebugFilePath = "memdebug.txt";
                    }
                }
                return strDebugFilePath;
            }
            
            
            void WriteToMemDebugFile(const char* message) noexcept
            {
#if defined(MTA_DEBUG) || defined(DEBUG) || defined(_DEBUG)
                // Debug builds: always log regardless of diagnostic settings
                if (!message)
                    return;
#else
                // Release builds: check diagnostic setting
                if (!IsMemoryAllocationDebuggingEnabled() || !message)
                    return;
#endif

                // Initialize debug file with header on first use
                static bool bFileInitialized = false;
                if (!bFileInitialized)
                {
                    try {
                        SString strFilePath = GetMemDebugFilePath();
                        SString strHeader = 
                            "=== MTA:SA Allocator Debug Log ===\n"
                            "Generated: " + GetLocalTimeString(false, true) + "\n"
#ifdef MTA_DEBUG
                            "Build: DEBUG\n"
#else
                            "Build: RELEASE\n"
#endif
                            "\nNote: HEARTBEAT entries are intentional tests, not real failures.\n\n";
                        
                        bool saveResult = FileSave(strFilePath, strHeader, false);
                        bFileInitialized = true;
                        
                        // Add logging for file creation issues
                        if (!saveResult)
                        {
                            // Fallback: try to write to a simple file for diagnostics
                            try {
                                FILE* debugFile = fopen("memdebug_fallback.txt", "w");
                                if (debugFile)
                                {
                                    fprintf(debugFile, "FileSave failed for path: %s\n", strFilePath.c_str());
                                    fclose(debugFile);
                                }
                            } catch (...) {}
                        }
                    } catch (...) {
                        bFileInitialized = true;
                        // Fallback diagnostic
                        try {
                            FILE* debugFile = fopen("memdebug_exception.txt", "w");
                            if (debugFile)
                            {
                                fprintf(debugFile, "Exception in WriteToMemDebugFile initialization\n");
                                fclose(debugFile);
                            }
                        } catch (...) {}
                    }
                }

                try {
                    SString strMessage = SString("[%s] %s\n", GetTimeString(true, true).c_str(), message);
                    bool appendResult = FileAppend(GetMemDebugFilePath(), strMessage, true);
                    
                    // Fallback if FileAppend fails
                    if (!appendResult)
                    {
                        try {
                            FILE* debugFile = fopen("memdebug_fallback.txt", "a");
                            if (debugFile)
                            {
                                fprintf(debugFile, "[FALLBACK] %s\n", message);
                                fclose(debugFile);
                            }
                        } catch (...) {}
                    }
                } catch (...) {
                    // Last-resort fallback using standard C file operations
                    try {
                        FILE* debugFile = fopen("memdebug_emergency.txt", "a");
                        if (debugFile)
                        {
                            fprintf(debugFile, "[EMERGENCY] %s\n", message);
                            fclose(debugFile);
                        }
                    } catch (...) {}
                }
                
                // Always try to write using raw C file operations for debug builds
#ifdef MTA_DEBUG
                try {
                    FILE* rawDebugFile = fopen("memdebug_raw.txt", "a");
                    if (rawDebugFile)
                    {
                        fprintf(rawDebugFile, "[RAW] %s\n", message);
                        fclose(rawDebugFile);
                    }
                } catch (...) {}
#endif
            }


            void LogAllocationFailure(const char* reason, void* ptr, std::size_t size, std::size_t alignment, void* caller) noexcept
            {
                if (!reason)
                {
                    reason = "[NULL REASON]";
                }

                // Capture system error state immediately to avoid being overwritten
                DWORD win32_error = GetLastError();
                int   errno_value = errno;

                // Get memory status information
                MEMORYSTATUSEX memStatus = {sizeof(MEMORYSTATUSEX)};
                BOOL           mem_status_valid = GlobalMemoryStatusEx(&memStatus);

                // Calculate available address space in game process
                std::uintptr_t           used_address_space = 0;
                std::uintptr_t           largest_free_block = 0;
                MEMORY_BASIC_INFORMATION mbi;
                std::uintptr_t           current_addr = 0x10000;            // Start after null pointer guard

                // Thread-safe copy of global max address to avoid races
                std::uintptr_t max_addr_copy = g_max_user_address;

                // Quick scan for largest free block (limit scan to avoid performance hit)
                int scan_count = 0;
                // only scan if g_max_user_address is valid and reasonable
                if (max_addr_copy > 0x10000 && max_addr_copy <= 0xFFFFFFFF)
                {
                    while (current_addr < max_addr_copy && scan_count < 100)
                    {
                        if (VirtualQuery(reinterpret_cast<LPCVOID>(current_addr), &mbi, sizeof(mbi)) == sizeof(mbi))
                        {
                            if (mbi.State == MEM_FREE && mbi.RegionSize > largest_free_block)
                            {
                                largest_free_block = mbi.RegionSize;
                            }
                            if (mbi.State != MEM_FREE)
                            {
                                used_address_space += mbi.RegionSize;
                            }

                            // Against integer overflow when calculating next address
                            std::uintptr_t base_addr = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
                            if (base_addr > max_addr_copy - mbi.RegionSize)
                            {
                                // Would overflow, stop scanning
                                break;
                            }
                            current_addr = base_addr + mbi.RegionSize;

                            // Additional safety: ensure we're making progress
                            if (current_addr <= base_addr)
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                        scan_count++;
                    }
                }

                char debug_buffer[512];
                _snprintf(debug_buffer, sizeof(debug_buffer),
                              "[MTA Memory] FAILURE: %s\n"
                              "  Request: Ptr=0x%p, Size=%zu, Align=%zu, Caller=0x%p\n"
                              "  System: Win32Err=%lu, Errno=%d, MemLoad=%lu%%\n"
                              "  Memory: PhysFree=%lluMB, VirtFree=%lluMB, LargestBlock=%zuMB\n"
                              "  Address: MaxUser=0x%p, UsedSpace=%zuMB, x64Host=%s\n",
                              reason, ptr, size, alignment, caller, win32_error, errno_value, mem_status_valid ? memStatus.dwMemoryLoad : 999,
                              mem_status_valid ? (memStatus.ullAvailPhys / (1024 * 1024)) : 0, mem_status_valid ? (memStatus.ullAvailVirtual / (1024 * 1024)) : 0,
                              largest_free_block / (1024 * 1024), reinterpret_cast<void*>(g_max_user_address), used_address_space / (1024 * 1024),
                              g_is_64bit_host_os ? "Yes" : "No");
                WriteToMemDebugFile(debug_buffer);

                // Additional NT status information if available
                if (win32_error != ERROR_SUCCESS)
                {
                    char error_buffer[128];
                    _snprintf(error_buffer, sizeof(error_buffer), "  ErrorDetail: ");

                    size_t current_len = strlen(error_buffer);
                    size_t remaining = sizeof(error_buffer) - current_len - 1;

                    // Decode common memory allocation errors
                    switch (win32_error)
                    {
                        case ERROR_NOT_ENOUGH_MEMORY:
                            std::strncat(error_buffer, "Insufficient memory (ERROR_NOT_ENOUGH_MEMORY)", remaining);
                            break;
                        case ERROR_OUTOFMEMORY:
                            std::strncat(error_buffer, "Out of memory (ERROR_OUTOFMEMORY)", remaining);
                            break;
                        case ERROR_INVALID_PARAMETER:
                            std::strncat(error_buffer, "Invalid parameter (ERROR_INVALID_PARAMETER)", remaining);
                            break;
                        case ERROR_INVALID_ADDRESS:
                            std::strncat(error_buffer, "Invalid address (ERROR_INVALID_ADDRESS)", remaining);
                            break;
                        case ERROR_COMMITMENT_LIMIT:
                            std::strncat(error_buffer, "Commitment limit exceeded (ERROR_COMMITMENT_LIMIT)", remaining);
                            break;
                        default:
                        {
                            char temp[64];
                            _snprintf(temp, sizeof(temp), "Win32 error %lu", win32_error);
                            current_len = strlen(error_buffer);
                            remaining = sizeof(error_buffer) - current_len - 1;
                            std::strncat(error_buffer, temp, remaining);
                            break;
                        }
                    }
                    current_len = strlen(error_buffer);
                    remaining = sizeof(error_buffer) - current_len - 1;
                    std::strncat(error_buffer, "\n", remaining);
                    WriteToMemDebugFile(error_buffer);
                }
            }

            // Specialized logging functions for different allocation failure scenarios
            void LogHeapAllocationFailure(const char* reason, void* ptr, std::size_t size, std::size_t alignment, void* caller) noexcept
            {
                DWORD  heap_flags = 0;
                BOOL   heap_valid = HeapValidate(g_process_heap, 0, nullptr);
                SIZE_T heap_size = 0;
                BOOL   heap_size_valid = FALSE;

                // HeapSize can fail and return (SIZE_T)-1, so check validity
                if (ptr && g_process_heap)
                {
                    heap_size = HeapSize(g_process_heap, 0, ptr);
                    heap_size_valid = (heap_size != static_cast<SIZE_T>(-1));
                }

                char heap_buffer[256];
                char size_buffer[32] = "Unknown";
                if (heap_size_valid)
                {
                    // Use safe snprintf instead of std::to_string to avoid potential exceptions in noexcept function
                    _snprintf(size_buffer, sizeof(size_buffer), "%zu", static_cast<size_t>(heap_size));
                }

                _snprintf(heap_buffer, sizeof(heap_buffer), "  HeapInfo: Valid=%s, Flags=0x%lx, Size=%s, Handle=0x%p\n", heap_valid ? "Yes" : "No", heap_flags,
                              size_buffer, g_process_heap);
                WriteToMemDebugFile(heap_buffer);

                // Add memory pressure analysis for heap failures
                LogMemoryPressureDetails();

                LogAllocationFailure(reason, ptr, size, alignment, caller);
            }

            void LogVirtualAllocFailure(const char* reason, void* ptr, std::size_t size, std::size_t alignment, void* caller) noexcept
            {
                // Get detailed VirtualAlloc-specific information
                SYSTEM_INFO sysInfo;
                GetSystemInfo(&sysInfo);

                // Check if this is a VirtualAlloc size/alignment issue (with zero division protection)
                bool size_aligned = (sysInfo.dwPageSize != 0) ? ((size % sysInfo.dwPageSize) == 0) : false;
                bool addr_aligned =
                    ptr ? ((sysInfo.dwAllocationGranularity != 0) ? ((reinterpret_cast<std::uintptr_t>(ptr) % sysInfo.dwAllocationGranularity) == 0) : false)
                        : true;

                char virtual_buffer[256];
                _snprintf(virtual_buffer, sizeof(virtual_buffer), "  VirtualInfo: PageSize=%lu, AllocGran=%lu, SizeAlign=%s, AddrAlign=%s\n", sysInfo.dwPageSize,
                          sysInfo.dwAllocationGranularity, size_aligned ? "Yes" : "No", addr_aligned ? "Yes" : "No");
                WriteToMemDebugFile(virtual_buffer);

                // Add memory pressure analysis for VirtualAlloc failures
                LogMemoryPressureDetails();

                LogAllocationFailure(reason, ptr, size, alignment, caller);
            }

            void LogSafetyRejection(const char* reason, void* ptr, std::size_t size, std::size_t alignment, const char* safety_condition,
                                           void* caller) noexcept
            {
                char safety_buffer[256];
                _snprintf(safety_buffer, sizeof(safety_buffer), "  SafetyReject: Condition='%s', Policy=GracefulFail\n", 
                         safety_condition ? safety_condition : "[NULL]");
                WriteToMemDebugFile(safety_buffer);

                LogAllocationFailure(reason, ptr, size, alignment, caller);
            }

            // Enhanced memory pressure analysis for critical failures
            void LogMemoryPressureDetails() noexcept
            {
                // Get process memory information
                PROCESS_MEMORY_COUNTERS_EX pmc = {sizeof(PROCESS_MEMORY_COUNTERS_EX)};
                BOOL                       pmc_valid = GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc));

                // Get system performance information
                PERFORMANCE_INFORMATION perf = {sizeof(PERFORMANCE_INFORMATION)};
                BOOL                    perf_valid = GetPerformanceInfo(&perf, sizeof(perf));

                char pressure_buffer[512];
                _snprintf(pressure_buffer, sizeof(pressure_buffer),
                          "  MemoryPressure: ProcessWorkingSet=%lluMB, PageFaults=%lu, PeakWorkingSet=%lluMB\n"
                          "                  CommitTotal=%lluMB, CommitLimit=%lluMB, SystemCache=%lluMB\n",
                          pmc_valid ? (static_cast<unsigned long long>(pmc.WorkingSetSize) / (1024 * 1024)) : 0, pmc_valid ? pmc.PageFaultCount : 0,
                          pmc_valid ? (static_cast<unsigned long long>(pmc.PeakWorkingSetSize) / (1024 * 1024)) : 0,
                          perf_valid ? ((static_cast<ULONGLONG>(perf.CommitTotal) * perf.PageSize) / (1024 * 1024)) : 0,
                          perf_valid ? ((static_cast<ULONGLONG>(perf.CommitLimit) * perf.PageSize) / (1024 * 1024)) : 0,
                          perf_valid ? ((static_cast<ULONGLONG>(perf.SystemCache) * perf.PageSize) / (1024 * 1024)) : 0);
                WriteToMemDebugFile(pressure_buffer);
            }

            // Critical system failure logging with NT status information
            void LogCriticalSystemFailure(const char* reason, void* ptr, std::size_t size, std::size_t alignment, void* caller) noexcept
            {
                DWORD win32_error = GetLastError();

                // Try to get NT status (for system-level failures)
                HMODULE ntdll = GetModuleHandleA("ntdll.dll");
                if (ntdll != nullptr)
                {
                    typedef ULONG(WINAPI * RtlGetLastNtStatus_t)(void);
                    RtlGetLastNtStatus_t RtlGetLastNtStatus = reinterpret_cast<RtlGetLastNtStatus_t>(GetProcAddress(ntdll, "RtlGetLastNtStatus"));
                    if (RtlGetLastNtStatus != nullptr)
                    {
                        ULONG nt_status = RtlGetLastNtStatus();
                        char  nt_buffer[128];
                        _snprintf(nt_buffer, sizeof(nt_buffer), "  NTStatus: 0x%08lx, LastWin32Error: %lu\n", nt_status, win32_error);
                        WriteToMemDebugFile(nt_buffer);
                    }
                    else
                    {
                        char nt_buffer[128];
                        _snprintf(nt_buffer, sizeof(nt_buffer), "  NTStatus: RtlGetLastNtStatus not found, LastWin32Error: %lu\n", win32_error);
                        WriteToMemDebugFile(nt_buffer);
                    }
                }
                else
                {
                    char nt_buffer[128];
                    _snprintf(nt_buffer, sizeof(nt_buffer), "  NTStatus: ntdll.dll not available, LastWin32Error: %lu\n", win32_error);
                    WriteToMemDebugFile(nt_buffer);
                }

                LogMemoryPressureDetails();
                LogAllocationFailure(reason, ptr, size, alignment, caller);
            }

        } // namespace diagnostics
    } // namespace memory
} // namespace mta