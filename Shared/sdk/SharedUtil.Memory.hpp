/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Memory.hpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "SharedUtil.Memory.h"

#ifdef _WIN32
    #include <Windows.h>
    #include <Psapi.h>
    #include <vector>
#elif defined(__APPLE__)
    #include <mach/mach.h>
    #include <unistd.h>
#else
    #include <fstream>
    #include <unistd.h>
    #include <sys/sysinfo.h>
#endif

namespace SharedUtil
{
    bool TryGetProcessMemoryStats(ProcessMemoryStats& out)
    {
        out = {};

#ifdef _WIN32
        MEMORYSTATUSEX memoryStatus{};
        memoryStatus.dwLength = sizeof(memoryStatus);

        if (!GlobalMemoryStatusEx(&memoryStatus))
            return false;

        out.virtualMemorySize = static_cast<size_t>(memoryStatus.ullTotalVirtual - memoryStatus.ullAvailVirtual);

        static std::vector<char> workingSetBuffer(2048 * sizeof(PSAPI_WORKING_SET_BLOCK) + sizeof(PSAPI_WORKING_SET_INFORMATION));
        auto                     workingSetInfo = reinterpret_cast<PSAPI_WORKING_SET_INFORMATION*>(workingSetBuffer.data());
        HANDLE                   process = GetCurrentProcess();
        BOOL                     success = QueryWorkingSet(process, workingSetBuffer.data(), workingSetBuffer.size());

        if (!success && GetLastError() == ERROR_BAD_LENGTH)
        {
            workingSetInfo->NumberOfEntries += 64;            // Insurance in case the number of entries changes.
            workingSetBuffer.resize(workingSetInfo->NumberOfEntries * sizeof(PSAPI_WORKING_SET_BLOCK) + sizeof(PSAPI_WORKING_SET_INFORMATION));
            workingSetInfo = reinterpret_cast<PSAPI_WORKING_SET_INFORMATION*>(workingSetBuffer.data());
            success = QueryWorkingSet(process, workingSetBuffer.data(), workingSetBuffer.size());
        }

        if (success)
        {
            for (ULONG_PTR i = 0; i < workingSetInfo->NumberOfEntries; ++i)
            {
                const PSAPI_WORKING_SET_BLOCK& block = workingSetInfo->WorkingSetInfo[i];

                if (block.Shared)
                {
                    out.sharedMemorySize++;
                }
                else
                {
                    out.privateMemorySize++;
                }
            }

            static size_t pageSize = ([]() {
                SYSTEM_INFO sysInfo{};
                GetSystemInfo(&sysInfo);
                return static_cast<size_t>(sysInfo.dwPageSize);
            })();

            out.sharedMemorySize *= pageSize;
            out.privateMemorySize *= pageSize;
            out.residentMemorySize = out.sharedMemorySize + out.privateMemorySize;
        }
        else
        {
            PROCESS_MEMORY_COUNTERS_EX memoryInfo{};

            if (!GetProcessMemoryInfo(process, reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&memoryInfo), sizeof(memoryInfo)))
                return false;

            out.residentMemorySize = memoryInfo.WorkingSetSize;
        }

        return true;
#elif defined(__APPLE__)
        struct task_basic_info info;
        mach_msg_type_number_t info_count = TASK_BASIC_INFO_COUNT;

        if (task_info(mach_task_self(), TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &info_count) != KERN_SUCCESS)
            return false;

        static const size_t pageSize = static_cast<size_t>(getpagesize());
        out.virtualMemorySize = info.virtual_size * pageSize;
        out.residentMemorySize = info.resident_size * pageSize;
        return true;
#else
        long rawPageSize = sysconf(_SC_PAGE_SIZE);

        if (rawPageSize <= 0)
            return false;
        
        auto pageSize = static_cast<size_t>(rawPageSize);

        std::ifstream stat("/proc/self/statm");

        if (!stat.is_open())
            return false;

        size_t size = 0, resident = 0, shared = 0;
        stat >> size >> resident >> shared;
        stat.close();
        
        out.virtualMemorySize = size * pageSize;
        out.residentMemorySize = resident * pageSize;
        out.sharedMemorySize = shared * pageSize;

        if (out.residentMemorySize > out.sharedMemorySize)
            out.privateMemorySize = out.residentMemorySize - out.sharedMemorySize;
        return true;
#endif
    }
}
