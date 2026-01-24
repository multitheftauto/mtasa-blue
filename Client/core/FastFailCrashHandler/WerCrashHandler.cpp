/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/FastFailCrashHandler/WerCrashHandler.cpp
 *  PURPOSE:     WER crash handler implementation.
 *               Capable of dump, register, and stacktrace collection fo 0xC0000409 / 0xC0000374 crashes.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// Self-contained module - compiled by both Loader and Core projects
// Do not use precompiled headers here

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <DbgHelp.h>
#include <Psapi.h>
#include <time.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#define MTA_CLIENT
#include <SharedUtil.h>
#include <version.h>

#include "WerCrashHandler.h"

namespace
{
    constexpr DWORD kWerInfoTagStart = 'WERs';
    constexpr DWORD kWerInfoTagEnd = 'WERe';
    constexpr DWORD kDefaultDllBase = 0x10000000;
    constexpr DWORD kDefaultExeBase = 0x00400000;

    struct MinidumpModuleInfo
    {
        SString name;
        DWORD64 baseAddress = 0;
        DWORD32 size = 0;
    };

    struct FormattedStackFrame
    {
        SString text;
        bool    isDll = false;
        DWORD   idaOffset = 0;
    };

    [[nodiscard]] FormattedStackFrame FormatAddressWithModules(DWORD64 address, const std::vector<MinidumpModuleInfo>& modules)
    {
        FormattedStackFrame result;
        for (const auto& mod : modules)
        {
            if (mod.size <= (0xFFFFFFFFFFFFFFFF - mod.baseAddress) && address >= mod.baseAddress && address < mod.baseAddress + mod.size)
            {
                const DWORD64 offset = address - mod.baseAddress;
                result.text = SString("%s+0x%llX", mod.name.c_str(), static_cast<unsigned long long>(offset));
                result.isDll = mod.name.length() > 4 && mod.name.EndsWithI(".dll");
                if (result.isDll && offset <= (0xFFFFFFFF - kDefaultDllBase))
                    result.idaOffset = kDefaultDllBase + static_cast<DWORD>(offset);
                return result;
            }
        }
        result.text = SString("0x%08X", static_cast<unsigned int>(address));
        return result;
    }

    [[nodiscard]] WerCrash::FileTimeDuration FileTimeToDuration(const FILETIME& value) noexcept
    {
        const auto combined = (static_cast<unsigned long long>(value.dwHighDateTime) << 32) | value.dwLowDateTime;
        return WerCrash::FileTimeDuration{static_cast<WerCrash::FileTimeDuration::rep>(combined)};
    }

    [[nodiscard]] bool IsWriteTimeRecentEnough(const FILETIME& ftWrite, std::uint64_t maxAgeMinutes) noexcept
    {
        FILETIME ftNow{};
        GetSystemTimeAsFileTime(&ftNow);

        const ULARGE_INTEGER uliNow{ftNow.dwLowDateTime, ftNow.dwHighDateTime};
        const ULARGE_INTEGER uliWrite{ftWrite.dwLowDateTime, ftWrite.dwHighDateTime};

        constexpr ULONGLONG kFileTimeUnitsPerSecond = 10000000ULL;
        const ULONGLONG     maxAge = maxAgeMinutes * 60 * kFileTimeUnitsPerSecond;

        return uliNow.QuadPart >= uliWrite.QuadPart && (uliNow.QuadPart - uliWrite.QuadPart) < maxAge;
    }

    [[nodiscard]] bool IsValidRva(SIZE_T mappedSize, RVA rva, SIZE_T dataSize) noexcept
    {
        return rva != 0 && dataSize != 0 && rva < mappedSize && dataSize <= mappedSize - rva;
    }
}

namespace WerCrash
{
    bool IsFileRecentEnough(HANDLE hFile, std::uint64_t maxAgeMinutes) noexcept
    {
        FILETIME ftCreate{}, ftAccess{}, ftWrite{};
        if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
            return false;

        return IsWriteTimeRecentEnough(ftWrite, maxAgeMinutes);
    }

    MinidumpRegisters ExtractRegistersFromMinidump(const SString& dumpPath)
    {
        MinidumpRegisters regs{};

        const HANDLE hFile = CreateFileA(dumpPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE)
            return regs;

        LARGE_INTEGER fileSize{};
        if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart < 1024)
        {
            CloseHandle(hFile);
            return regs;
        }
        const auto mappedSize = static_cast<SIZE_T>(fileSize.QuadPart);

        const HANDLE hMapping = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (!hMapping)
        {
            CloseHandle(hFile);
            return regs;
        }

        const LPVOID pBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
        if (!pBase)
        {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            return regs;
        }

        PMINIDUMP_DIRECTORY pExceptionDir = nullptr;
        PVOID               pExceptionStream = nullptr;
        ULONG               streamSize = 0;

        if (MiniDumpReadDumpStream(pBase, ExceptionStream, &pExceptionDir, &pExceptionStream, &streamSize))
        {
            auto* pExInfo = static_cast<MINIDUMP_EXCEPTION_STREAM*>(pExceptionStream);
            if (pExInfo && streamSize >= sizeof(MINIDUMP_EXCEPTION_STREAM))
            {
                regs.exceptionCode = pExInfo->ExceptionRecord.ExceptionCode;
                regs.exceptionAddress = pExInfo->ExceptionRecord.ExceptionAddress;

                PMINIDUMP_DIRECTORY pThreadListDir = nullptr;
                PVOID               pThreadListStream = nullptr;
                ULONG               threadListSize = 0;

                if (MiniDumpReadDumpStream(pBase, ThreadListStream, &pThreadListDir, &pThreadListStream, &threadListSize))
                {
                    auto* pThreadList = static_cast<MINIDUMP_THREAD_LIST*>(pThreadListStream);
                    if (pThreadList && pThreadList->NumberOfThreads > 0)
                    {
                        for (ULONG i = 0; i < pThreadList->NumberOfThreads; i++)
                        {
                            const auto& thread = pThreadList->Threads[i];
                            if (thread.ThreadId != pExInfo->ThreadId)
                                continue;

                            const MINIDUMP_LOCATION_DESCRIPTOR ctxLoc = thread.ThreadContext;
                            if (ctxLoc.DataSize < sizeof(CONTEXT) || !IsValidRva(mappedSize, ctxLoc.Rva, ctxLoc.DataSize))
                                break;

                            auto* pCtx = reinterpret_cast<CONTEXT*>(static_cast<BYTE*>(pBase) + ctxLoc.Rva);

                            regs.eax = pCtx->Eax;
                            regs.ebx = pCtx->Ebx;
                            regs.ecx = pCtx->Ecx;
                            regs.edx = pCtx->Edx;
                            regs.esi = pCtx->Esi;
                            regs.edi = pCtx->Edi;
                            regs.ebp = pCtx->Ebp;
                            regs.esp = pCtx->Esp;
                            regs.eip = pCtx->Eip;
                            regs.eflags = pCtx->EFlags;
                            regs.cs = static_cast<WORD>(pCtx->SegCs);
                            regs.ds = static_cast<WORD>(pCtx->SegDs);
                            regs.es = static_cast<WORD>(pCtx->SegEs);
                            regs.fs = static_cast<WORD>(pCtx->SegFs);
                            regs.gs = static_cast<WORD>(pCtx->SegGs);
                            regs.ss = static_cast<WORD>(pCtx->SegSs);
                            regs.valid = true;
                            break;
                        }
                    }
                }
            }
        }

        UnmapViewOfFile(pBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return regs;
    }

    SString ExtractStackTraceFromMinidump(const SString& dumpPath, const MinidumpRegisters& regs)
    {
        if (!regs.valid || regs.esp == 0)
            return "";

        constexpr size_t kMaxFrames = 64;
        constexpr size_t kMinFramesBeforeRawScan = 3;
        constexpr size_t kRawStackScanBytes = 8192;
        constexpr DWORD  kMinCodeAddress = 0x00400000;
        constexpr DWORD  kMaxCodeAddress = 0x7FFFFFFF;

        const HANDLE hFile = CreateFileA(dumpPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE)
            return "";

        LARGE_INTEGER fileSize{};
        if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart < 1024)
        {
            CloseHandle(hFile);
            return "";
        }
        const auto mappedSize = static_cast<SIZE_T>(fileSize.QuadPart);

        const HANDLE hMapping = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (!hMapping)
        {
            CloseHandle(hFile);
            return "";
        }

        const LPVOID pBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
        if (!pBase)
        {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            return "";
        }

        std::vector<MinidumpModuleInfo> modules;
        PMINIDUMP_DIRECTORY             pModuleListDir = nullptr;
        PVOID                           pModuleListStream = nullptr;
        ULONG                           moduleListSize = 0;

        if (MiniDumpReadDumpStream(pBase, ModuleListStream, &pModuleListDir, &pModuleListStream, &moduleListSize))
        {
            auto* pModuleList = static_cast<MINIDUMP_MODULE_LIST*>(pModuleListStream);
            if (pModuleList && pModuleList->NumberOfModules > 0)
            {
                modules.reserve(pModuleList->NumberOfModules);
                for (ULONG i = 0; i < pModuleList->NumberOfModules; i++)
                {
                    const auto&        mod = pModuleList->Modules[i];
                    MinidumpModuleInfo info;
                    info.baseAddress = mod.BaseOfImage;
                    info.size = mod.SizeOfImage;

                    if (mod.ModuleNameRva != 0 && mod.ModuleNameRva <= mappedSize - sizeof(ULONG32))
                    {
                        auto* pModName = reinterpret_cast<MINIDUMP_STRING*>(static_cast<BYTE*>(pBase) + mod.ModuleNameRva);

                        if (pModName->Length > 0 && pModName->Length < 1024)
                        {
                            const SIZE_T availableForString = mappedSize - mod.ModuleNameRva - sizeof(ULONG32);
                            if (pModName->Length <= availableForString)
                            {
                                std::wstring wname(pModName->Buffer, pModName->Length / sizeof(wchar_t));
                                auto         lastSlash = wname.find_last_of(L"\\/");
                                if (lastSlash != std::wstring::npos)
                                    wname = wname.substr(lastSlash + 1);

                                std::string name(wname.begin(), wname.end());
                                info.name = name.c_str();
                            }
                        }
                    }

                    if (!info.name.empty())
                        modules.push_back(std::move(info));
                }
            }
        }

        std::map<DWORD64, std::vector<BYTE>> memoryRegions;
        PMINIDUMP_DIRECTORY                  pMemoryListDir = nullptr;
        PVOID                                pMemoryListStream = nullptr;
        ULONG                                memoryListSize = 0;

        if (MiniDumpReadDumpStream(pBase, MemoryListStream, &pMemoryListDir, &pMemoryListStream, &memoryListSize))
        {
            auto* pMemoryList = static_cast<MINIDUMP_MEMORY_LIST*>(pMemoryListStream);
            if (pMemoryList && pMemoryList->NumberOfMemoryRanges > 0)
            {
                for (ULONG i = 0; i < pMemoryList->NumberOfMemoryRanges; i++)
                {
                    const auto& range = pMemoryList->MemoryRanges[i];
                    if (IsValidRva(mappedSize, range.Memory.Rva, range.Memory.DataSize))
                    {
                        auto* pData = static_cast<BYTE*>(pBase) + range.Memory.Rva;
                        memoryRegions[range.StartOfMemoryRange] = std::vector<BYTE>(pData, pData + range.Memory.DataSize);
                    }
                }
            }
        }

        const auto readDword = [&memoryRegions](DWORD64 address) noexcept -> std::optional<DWORD>
        {
            for (const auto& [baseAddr, data] : memoryRegions)
            {
                if (address >= baseAddr && address + sizeof(DWORD) <= baseAddr + data.size())
                {
                    const auto offset = static_cast<size_t>(address - baseAddr);
                    DWORD      value = 0;
                    std::memcpy(&value, data.data() + offset, sizeof(DWORD));
                    return value;
                }
            }
            return std::nullopt;
        };

        const auto isAddressInModule = [&modules](DWORD64 addr) noexcept -> bool
        {
            for (const auto& mod : modules)
            {
                if (mod.size <= (0xFFFFFFFFFFFFFFFF - mod.baseAddress) && addr >= mod.baseAddress && addr < mod.baseAddress + mod.size)
                    return true;
            }
            return false;
        };

        const auto isLikelyCodeAddress = [&](DWORD addr) noexcept -> bool
        {
            if (addr < kMinCodeAddress || addr > kMaxCodeAddress)
                return false;
            return isAddressInModule(addr);
        };

        std::vector<DWORD64> frames;
        frames.reserve(kMaxFrames);

        std::set<DWORD> seenAddresses;
        frames.push_back(regs.eip);
        seenAddresses.insert(regs.eip);

        DWORD           currentEbp = regs.ebp;
        std::set<DWORD> visitedFrames;

        for (size_t i = 0; i < kMaxFrames && currentEbp != 0; i++)
        {
            if (visitedFrames.count(currentEbp))
                break;
            visitedFrames.insert(currentEbp);

            auto returnAddr = readDword(static_cast<DWORD64>(currentEbp) + sizeof(DWORD));
            if (!returnAddr)
                break;

            if (*returnAddr != 0 && *returnAddr <= kMaxCodeAddress && !seenAddresses.count(*returnAddr))
            {
                frames.push_back(*returnAddr);
                seenAddresses.insert(*returnAddr);
            }

            auto nextEbp = readDword(currentEbp);
            if (!nextEbp)
                break;

            if (*nextEbp <= currentEbp)
                break;

            currentEbp = *nextEbp;
        }

        if (frames.size() < kMinFramesBeforeRawScan)
        {
            DWORD64 stackPtr = regs.esp;
            size_t  bytesScanned = 0;

            while (bytesScanned < kRawStackScanBytes && frames.size() < kMaxFrames)
            {
                auto stackValue = readDword(stackPtr);
                if (!stackValue)
                {
                    stackPtr += sizeof(DWORD);
                    bytesScanned += sizeof(DWORD);

                    if (bytesScanned % 256 == 0)
                    {
                        bool foundNextRegion = false;
                        for (const auto& [baseAddr, data] : memoryRegions)
                        {
                            if (baseAddr > stackPtr && baseAddr < stackPtr + 4096)
                            {
                                stackPtr = baseAddr;
                                foundNextRegion = true;
                                break;
                            }
                        }
                        if (!foundNextRegion)
                            break;
                    }
                    continue;
                }

                const DWORD potentialAddr = *stackValue;
                if (isLikelyCodeAddress(potentialAddr) && !seenAddresses.count(potentialAddr))
                {
                    frames.push_back(potentialAddr);
                    seenAddresses.insert(potentialAddr);
                }

                stackPtr += sizeof(DWORD);
                bytesScanned += sizeof(DWORD);
            }
        }

        if (frames.size() <= 1)
        {
            UnmapViewOfFile(pBase);
            CloseHandle(hMapping);
            CloseHandle(hFile);
            return "";
        }

        SString stackTrace;
        for (size_t i = 0; i < frames.size(); i++)
        {
            const auto addr = frames[i];
            const auto formatted = FormatAddressWithModules(addr, modules);
            if (formatted.isDll)
                stackTrace += SString("#%02u %s [0x%08X] - IDA Address: 0x%08X\n", static_cast<unsigned int>(i), formatted.text.c_str(),
                                      static_cast<unsigned int>(addr), formatted.idaOffset);
            else
                stackTrace += SString("#%02u %s [0x%08X]\n", static_cast<unsigned int>(i), formatted.text.c_str(), static_cast<unsigned int>(addr));
        }

        UnmapViewOfFile(pBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return stackTrace;
    }

    void AppendWerInfoToDump(const SString& dumpPath, const SString& moduleName, DWORD offset, DWORD exceptionCode, const MinidumpRegisters& regs)
    {
        if (dumpPath.empty())
            return;

        const char* exceptionName = (exceptionCode == 0xC0000409)   ? "Stack Buffer Overrun"
                                    : (exceptionCode == 0xC0000374) ? "Heap Corruption"
                                                                    : "Security Exception";
        const DWORD idaAddress = (offset <= (0xFFFFFFFF - kDefaultDllBase)) ? (kDefaultDllBase + offset) : 0;

        SYSTEMTIME st{};
        GetLocalTime(&st);

        SString werInfo;
        werInfo += "WER Crash Info (Fail-Fast Exception)\n";
        werInfo += "=====================================\n";
        werInfo += SString("Exception: 0x%08X (%s)\n", exceptionCode, exceptionName);
        werInfo += SString("Module: %s\n", moduleName.empty() ? "unknown" : moduleName.c_str());
        werInfo += SString("Offset: 0x%08X\n", offset);
        werInfo += SString("IDA Address: 0x%08X (default base 0x10000000)\n", idaAddress);
        werInfo += "Resolved Module Base: 0x10000000\n";
        werInfo += SString("Detected: %04d-%02d-%02d %02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        werInfo += "Source: Windows Error Reporting (WER)\n";

        if (regs.valid)
        {
            werInfo += "\nRegisters:\n";
            werInfo += SString("EAX=%08X EBX=%08X ECX=%08X EDX=%08X ESI=%08X EDI=%08X\n", regs.eax, regs.ebx, regs.ecx, regs.edx, regs.esi, regs.edi);
            werInfo += SString("EBP=%08X ESP=%08X EIP=%08X FLG=%08X\n", regs.ebp, regs.esp, regs.eip, regs.eflags);
            werInfo += SString("CS=%04X DS=%04X SS=%04X ES=%04X FS=%04X GS=%04X\n", regs.cs, regs.ds, regs.ss, regs.es, regs.fs, regs.gs);
        }

        const DWORD zero = 0;
        const DWORD dataSize = static_cast<DWORD>(werInfo.length() + 1);

        std::vector<BYTE> buffer;
        buffer.reserve(sizeof(DWORD) * 6 + dataSize);

        const auto appendDword = [&buffer](DWORD value)
        {
            const auto* bytes = reinterpret_cast<const BYTE*>(&value);
            buffer.insert(buffer.end(), bytes, bytes + sizeof(DWORD));
        };

        appendDword(zero);
        appendDword(kWerInfoTagStart);
        appendDword(dataSize);
        buffer.insert(buffer.end(), werInfo.c_str(), werInfo.c_str() + dataSize);
        appendDword(dataSize);
        appendDword(kWerInfoTagEnd);
        appendDword(zero);

        FileAppend(dumpPath, buffer.data(), static_cast<unsigned long>(buffer.size()));
    }

    SString RenameWerDumpToMtaFormat(const SString& sourcePath, const SString& dumpDir, const SString& moduleName, DWORD offset, DWORD exceptionCode,
                                     const MinidumpRegisters& regs)
    {
        SYSTEMTIME st{};
        GetLocalTime(&st);

        SString cleanedModuleName = moduleName;
        cleanedModuleName = cleanedModuleName.ReplaceI(".dll", "").Replace(".exe", "").Replace("_", "").Replace(".", "").Replace("-", "");
        if (cleanedModuleName.empty())
            cleanedModuleName = "unknown";

        const auto strMTAVersionFull = SString("%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting("mta-version-ext").SplitRight(".", nullptr, -2));
        const auto strSerialPart = GetApplicationSetting("serial").substr(0, 5);
        const auto uiServerIP = static_cast<DWORD>(GetApplicationSettingInt("last-server-ip"));
        const auto uiServerPort = static_cast<DWORD>(GetApplicationSettingInt("last-server-port"));
        const auto uiServerTime = static_cast<DWORD>(GetApplicationSettingInt("last-server-time"));
        const auto currentTime = _time64(nullptr);

        std::int64_t rawDuration = 0;
        if (currentTime != -1)
            rawDuration = currentTime - static_cast<std::int64_t>(uiServerTime);
        rawDuration = std::clamp(rawDuration, std::int64_t{0}, std::numeric_limits<std::int64_t>::max() - 1);
        const int uiServerDuration = static_cast<int>(std::clamp(rawDuration + 1, std::int64_t{1}, std::int64_t{0x0fff}));

        SString strPathCode;
        {
            std::vector<SString> parts;
            PathConform(CalcMTASAPath("")).Split(PATH_SEPERATOR, parts);
            for (const auto& part : parts)
            {
                if (!part.empty())
                    strPathCode += part.Left(1).ToUpper();
            }
        }

        SString newFilename = SString("client_%s_%s_%08x_%x_%s_%08X_%04X_%03X_%s_%04d%02d%02d_%02d%02d.dmp", strMTAVersionFull.c_str(),
                                      cleanedModuleName.c_str(), offset, exceptionCode & 0xffff, strPathCode.c_str(), uiServerIP, uiServerPort,
                                      uiServerDuration, strSerialPart.c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

        SString newPath = PathJoin(dumpDir, newFilename);

        if (!MoveFileA(sourcePath, newPath))
            return {};

        AppendWerInfoToDump(newPath, moduleName, offset, exceptionCode, regs);

        const SString coreDmpPath = CalcMTASAPath("mta\\core.dmp");
        CopyFileA(newPath, coreDmpPath, FALSE);

        return newPath;
    }

    WerDumpResult FindAndRenameWerDump(const SString& dumpDir, const SString& moduleName, DWORD offset, DWORD exceptionCode,
                                       const std::optional<FileTimeDuration>& processCreationTime)
    {
        OutputDebugStringA(SString("FindAndRenameWerDump: dumpDir=%s module=%s\n", dumpDir.c_str(), moduleName.c_str()));

        auto werDumpFiles = FindFiles(PathJoin(dumpDir, "gta_sa.exe.*.dmp"), true, false, true);
        OutputDebugStringA(SString("FindAndRenameWerDump: Found %zu WER dump files\n", werDumpFiles.size()));

        if (werDumpFiles.empty())
            return {};

        SString  selectedPath;
        FILETIME selectedWrite{};
        bool     hasSelected = false;

        for (const auto& dumpFile : werDumpFiles)
        {
            const SString fullPath = PathJoin(dumpDir, dumpFile);
            OutputDebugStringA(SString("FindAndRenameWerDump: Checking %s\n", dumpFile.c_str()));

            const HANDLE hFile = CreateFileA(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile == INVALID_HANDLE_VALUE)
            {
                OutputDebugStringA(SString("FindAndRenameWerDump: Failed to open file (error %lu)\n", GetLastError()));
                continue;
            }

            FILETIME   ftWrite{};
            const BOOL gotTime = GetFileTime(hFile, nullptr, nullptr, &ftWrite);
            CloseHandle(hFile);

            if (!gotTime)
            {
                OutputDebugStringA("FindAndRenameWerDump: Failed to get file time\n");
                continue;
            }

            if (!IsWriteTimeRecentEnough(ftWrite, 15))
            {
                OutputDebugStringA("FindAndRenameWerDump: File too old (>15 min)\n");
                continue;
            }

            if (processCreationTime.has_value())
            {
                const auto lastWrite = FileTimeToDuration(ftWrite);
                if (lastWrite < *processCreationTime)
                {
                    OutputDebugStringA("FindAndRenameWerDump: File older than process creation\n");
                    continue;
                }
            }

            if (!hasSelected || CompareFileTime(&ftWrite, &selectedWrite) > 0)
            {
                selectedPath = fullPath;
                selectedWrite = ftWrite;
                hasSelected = true;
                OutputDebugStringA(SString("FindAndRenameWerDump: Selected %s\n", dumpFile.c_str()));
            }
        }

        if (!hasSelected)
        {
            OutputDebugStringA("FindAndRenameWerDump: No suitable file found\n");
            return {};
        }

        const SString sourceFilename = ExtractFilename(selectedPath);
        OutputDebugStringA(SString("FindAndRenameWerDump: Renaming %s\n", selectedPath.c_str()));
        const MinidumpRegisters regs = ExtractRegistersFromMinidump(selectedPath);
        const SString           newPath = RenameWerDumpToMtaFormat(selectedPath, dumpDir, moduleName, offset, exceptionCode, regs);
        if (newPath.empty())
        {
            OutputDebugStringA(SString("FindAndRenameWerDump: Rename failed (error %lu)\n", GetLastError()));
            WerDumpResult failResult;
            failResult.sourceFilename = sourceFilename;
            failResult.regs = regs;
            if (regs.valid)
                failResult.stackTrace = ExtractStackTraceFromMinidump(selectedPath, regs);
            return failResult;
        }

        OutputDebugStringA(SString("FindAndRenameWerDump: SUCCESS - renamed to %s\n", newPath.c_str()));
        WerDumpResult result;
        result.path = newPath;
        result.sourceFilename = sourceFilename;
        result.regs = regs;
        if (result.regs.valid)
        {
            result.stackTrace = ExtractStackTraceFromMinidump(newPath, result.regs);
        }
        return result;
    }

    void UpdateModuleBases()
    {
        HANDLE hProcess = GetCurrentProcess();

        int prevChunkCount = GetApplicationSettingInt("diagnostics", "module-bases-chunks");
        if (prevChunkCount < 1)
            prevChunkCount = 6;
        if (prevChunkCount > 256)
            prevChunkCount = 256;

        struct InternalModuleEntry
        {
            SString entry;
            bool    isMta;
        };

        auto storeCurrentOnly = [&, prevChunkCount](bool truncatedFlag)
        {
            HMODULE hCurrent = nullptr;
            if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   reinterpret_cast<LPCWSTR>(&UpdateModuleBases), &hCurrent))
            {
                wchar_t wszModName[MAX_PATH];
                if (GetModuleFileNameExW(hProcess, hCurrent, wszModName, MAX_PATH))
                {
                    const wchar_t* wfilename = wcsrchr(wszModName, L'\\');
                    wfilename = wfilename ? wfilename + 1 : wszModName;
                    SString filename = UTF16ToMbUTF8(wfilename);

                    MODULEINFO modInfo = {};
                    if (GetModuleInformation(hProcess, hCurrent, &modInfo, sizeof(modInfo)))
                    {
                        SString strEntry = SString("%s=%08X,%08X", filename.c_str(), static_cast<DWORD>(reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll)),
                                                   modInfo.SizeOfImage);
                        SetApplicationSetting("diagnostics", "module-bases", strEntry);
                        for (int i = 1; i < prevChunkCount; ++i)
                            SetApplicationSetting("diagnostics", SString("module-bases-%d", i), "");
                        SetApplicationSettingInt("diagnostics", "module-bases-chunks", 1);
                        SetApplicationSettingInt("diagnostics", "module-bases-truncated", truncatedFlag ? 1 : 0);
                    }
                }
            }
        };

        DWORD cbNeeded = 0;
        if (!EnumProcessModules(hProcess, nullptr, 0, &cbNeeded) || cbNeeded == 0)
        {
            storeCurrentOnly(false);
            return;
        }

        size_t moduleCount = cbNeeded / sizeof(HMODULE);
        if (moduleCount == 0)
        {
            storeCurrentOnly(false);
            return;
        }

        std::vector<HMODULE> hMods(moduleCount);
        if (!EnumProcessModules(hProcess, hMods.data(), static_cast<DWORD>(hMods.size() * sizeof(HMODULE)), &cbNeeded))
        {
            storeCurrentOnly(false);
            return;
        }

        if (cbNeeded > hMods.size() * sizeof(HMODULE))
        {
            moduleCount = cbNeeded / sizeof(HMODULE);
            hMods.resize(moduleCount);
            if (!EnumProcessModules(hProcess, hMods.data(), static_cast<DWORD>(hMods.size() * sizeof(HMODULE)), &cbNeeded))
            {
                storeCurrentOnly(false);
                return;
            }
        }

        moduleCount = std::min(moduleCount, static_cast<size_t>(cbNeeded / sizeof(HMODULE)));

        std::vector<InternalModuleEntry> entries;
        entries.reserve(moduleCount);

        for (size_t i = 0; i < moduleCount; ++i)
        {
            if (!hMods[i])
                continue;

            wchar_t wszModName[MAX_PATH];
            if (!GetModuleFileNameExW(hProcess, hMods[i], wszModName, MAX_PATH))
                continue;

            const wchar_t* wfilename = wcsrchr(wszModName, L'\\');
            wfilename = wfilename ? wfilename + 1 : wszModName;
            SString filename = UTF16ToMbUTF8(wfilename);

            MODULEINFO modInfo = {};
            if (!GetModuleInformation(hProcess, hMods[i], &modInfo, sizeof(modInfo)))
                continue;

            SString lower = filename.ToLower();
            bool    isMta = lower.Contains("core") || lower.Contains("client") || lower.Contains("game_sa") || lower.Contains("multiplayer") ||
                         lower.Contains("netc") || lower.Contains("gta_sa") || lower.Contains("proxy_sa") || lower.Contains("cef") || lower.Contains("mta") ||
                         lower.Contains("deathmatch") || lower.Contains("gui") || lower.Contains("xmll") || lower.Contains("loader");

            InternalModuleEntry m;
            m.entry = SString("%s=%08X,%08X", filename.c_str(), static_cast<DWORD>(reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll)), modInfo.SizeOfImage);
            m.isMta = isMta;
            entries.push_back(m);
        }

        const size_t maxValueLen = 60000;
        const size_t maxChunks = 256;
        bool         truncated = false;

        std::vector<SString> chunks;
        chunks.reserve(prevChunkCount > 0 ? static_cast<size_t>(prevChunkCount) : 6);

        auto tryAppend = [&](const SString& entry) -> bool
        {
            if (entry.length() > maxValueLen)
            {
                truncated = true;
                return false;
            }
            for (auto& buffer : chunks)
            {
                size_t extra = entry.length();
                if (!buffer.empty())
                    extra += 1;
                if (buffer.length() + extra > maxValueLen)
                    continue;
                if (!buffer.empty())
                    buffer += ";";
                buffer += entry;
                return true;
            }

            if (chunks.size() >= maxChunks)
            {
                truncated = true;
                return false;
            }

            chunks.emplace_back(entry);
            return true;
        };

        auto appendEntries = [&](bool mtaFirst)
        {
            for (const auto& e : entries)
            {
                if (e.isMta != mtaFirst)
                    continue;

                if (tryAppend(e.entry))
                    continue;

                truncated = true;
                return;
            }
        };

        appendEntries(true);
        if (!truncated)
            appendEntries(false);

        bool anyData = false;
        for (const auto& c : chunks)
        {
            if (!c.empty())
            {
                anyData = true;
                break;
            }
        }

        if (!anyData)
        {
            storeCurrentOnly(truncated);
            return;
        }

        auto setChunk = [&](size_t idx, const SString& value)
        {
            if (idx == 0)
                SetApplicationSetting("diagnostics", "module-bases", value);
            else
                SetApplicationSetting("diagnostics", SString("module-bases-%d", static_cast<int>(idx)), value);
        };

        const size_t usedChunks = chunks.size();
        for (size_t i = 0; i < usedChunks; ++i)
            setChunk(i, chunks[i]);

        const size_t clearFrom = usedChunks;
        const size_t clearTo = static_cast<size_t>(std::max(prevChunkCount, static_cast<int>(usedChunks)));
        for (size_t i = clearFrom; i < clearTo; ++i)
            setChunk(i, "");

        SetApplicationSettingInt("diagnostics", "module-bases-chunks", static_cast<int>(usedChunks > 0 ? usedChunks : 1));
        SetApplicationSettingInt("diagnostics", "module-bases-truncated", truncated ? 1 : 0);
    }

    std::vector<ModuleEntry> GetModuleBasesFromRegistry()
    {
        std::vector<ModuleEntry> modules;

        int chunkCount = GetApplicationSettingInt("diagnostics", "module-bases-chunks");
        if (chunkCount <= 0)
            chunkCount = 6;
        if (chunkCount > 256)
            chunkCount = 256;

        SString strModuleBases;
        for (int i = 0; i < chunkCount; ++i)
        {
            SString key;
            if (i == 0)
                key = "module-bases";
            else
                key.Format("module-bases-%d", i);

            SString chunk = GetApplicationSetting("diagnostics", key);
            if (chunk.empty())
                continue;

            if (!strModuleBases.empty())
                strModuleBases += ";";
            strModuleBases += chunk;
        }

        if (strModuleBases.empty())
            return modules;

        std::vector<SString> entries;
        entries.reserve(128);
        strModuleBases.Split(";", entries, true);

        for (const SString& entry : entries)
        {
            std::vector<SString> parts;
            entry.Split("=", parts);
            if (parts.size() != 2)
                continue;

            ModuleEntry mod;
            mod.name = parts[0];

            std::vector<SString> addrParts;
            parts[1].Split(",", addrParts);

            if (addrParts.empty())
                continue;

            SString addrStr = addrParts[0];
            addrStr.Replace("0x", "");
            addrStr.Replace("0X", "");
            mod.base = static_cast<DWORD>(strtoull(addrStr.c_str(), nullptr, 16));

            if (addrParts.size() >= 2)
            {
                SString sizeStr = addrParts[1];
                sizeStr.Replace("0x", "");
                sizeStr.Replace("0X", "");
                mod.size = static_cast<DWORD>(strtoull(sizeStr.c_str(), nullptr, 16));
            }
            else
            {
                mod.size = 0x400000;
            }

            if (!mod.name.empty() && mod.base != 0 && mod.size != 0)
                modules.push_back(mod);
        }

        return modules;
    }

    ResolvedModule ResolveAddressToModule(DWORD crashAddress)
    {
        int chunkCount = GetApplicationSettingInt("diagnostics", "module-bases-chunks");
        if (chunkCount <= 0)
            chunkCount = 6;
        if (chunkCount > 256)
            chunkCount = 256;

        ResolvedModule result = ResolveAddressToModule(crashAddress, GetModuleBasesFromRegistry());
        result.debugChunkCount = chunkCount;
        return result;
    }

    ResolvedModule ResolveAddressToModule(DWORD crashAddress, const std::vector<ModuleEntry>& modules)
    {
        ResolvedModule result{};
        result.crashAddress = crashAddress;
        result.resolved = false;
        result.debugModuleCount = static_cast<int>(modules.size());

        if (modules.empty())
        {
            result.debugFailReason = 2;
            return result;
        }

        for (const ModuleEntry& mod : modules)
        {
            if (mod.size > 0xFFFFFFFF - mod.base)
                continue;

            const DWORD moduleEnd = mod.base + mod.size;

            if (crashAddress >= mod.base && crashAddress < moduleEnd)
            {
                result.moduleName = mod.name;
                result.moduleBase = mod.base;
                result.rva = crashAddress - mod.base;

                const bool  isExe = result.moduleName.EndsWithI(".exe");
                const DWORD idaBase = isExe ? kDefaultExeBase : kDefaultDllBase;
                if (result.rva <= (0xFFFFFFFF - idaBase))
                    result.idaAddress = idaBase + result.rva;
                result.resolved = true;
                return result;
            }
        }

        result.debugFailReason = 3;
        return result;
    }

    ResolvedModule ResolveAddressFromMinidump(const SString& dumpPath, DWORD crashAddress)
    {
        ResolvedModule result{};
        result.crashAddress = crashAddress;
        result.resolved = false;

        if (dumpPath.empty())
        {
            result.debugFailReason = 1;
            return result;
        }

        const HANDLE hFile = CreateFileA(dumpPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            result.debugFailReason = 1;
            return result;
        }

        LARGE_INTEGER fileSize{};
        if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart < 1024 || fileSize.QuadPart > 512 * 1024 * 1024)
        {
            CloseHandle(hFile);
            result.debugFailReason = 1;
            return result;
        }
        const auto mappedSize = static_cast<SIZE_T>(fileSize.QuadPart);

        const HANDLE hMapping = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (!hMapping)
        {
            CloseHandle(hFile);
            result.debugFailReason = 1;
            return result;
        }

        const LPVOID pBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
        if (!pBase)
        {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            result.debugFailReason = 1;
            return result;
        }

        PMINIDUMP_DIRECTORY pModuleListDir = nullptr;
        PVOID               pModuleListStream = nullptr;
        ULONG               moduleListSize = 0;

        if (MiniDumpReadDumpStream(pBase, ModuleListStream, &pModuleListDir, &pModuleListStream, &moduleListSize))
        {
            if (moduleListSize < sizeof(ULONG32))
            {
                result.debugFailReason = 2;
                UnmapViewOfFile(pBase);
                CloseHandle(hMapping);
                CloseHandle(hFile);
                return result;
            }

            auto* pModuleList = static_cast<MINIDUMP_MODULE_LIST*>(pModuleListStream);
            if (pModuleList && pModuleList->NumberOfModules > 0 && pModuleList->NumberOfModules < 10000)
            {
                const SIZE_T requiredSize = sizeof(MINIDUMP_MODULE_LIST) + (static_cast<SIZE_T>(pModuleList->NumberOfModules) - 1) * sizeof(MINIDUMP_MODULE);
                if (moduleListSize < requiredSize)
                {
                    result.debugFailReason = 2;
                    UnmapViewOfFile(pBase);
                    CloseHandle(hMapping);
                    CloseHandle(hFile);
                    return result;
                }

                result.debugModuleCount = static_cast<int>(pModuleList->NumberOfModules);

                for (ULONG i = 0; i < pModuleList->NumberOfModules; i++)
                {
                    const auto&   mod = pModuleList->Modules[i];
                    const DWORD64 modBase = mod.BaseOfImage;
                    const DWORD32 modSize = mod.SizeOfImage;

                    if (modSize == 0 || modSize > 0xFFFFFFFF - modBase)
                        continue;

                    if (crashAddress >= modBase && crashAddress < modBase + modSize)
                    {
                        SString modName;
                        if (mod.ModuleNameRva != 0 && mod.ModuleNameRva < mappedSize && mod.ModuleNameRva <= mappedSize - sizeof(ULONG32))
                        {
                            auto* pModName = reinterpret_cast<MINIDUMP_STRING*>(static_cast<BYTE*>(pBase) + mod.ModuleNameRva);
                            if (pModName->Length > 0 && pModName->Length < 1024)
                            {
                                const SIZE_T stringOffset = mod.ModuleNameRva + sizeof(ULONG32);
                                if (stringOffset < mappedSize)
                                {
                                    const SIZE_T availableForString = mappedSize - stringOffset;
                                    if (pModName->Length <= availableForString)
                                    {
                                        try
                                        {
                                            std::wstring wname(pModName->Buffer, pModName->Length / sizeof(wchar_t));
                                            auto         lastSlash = wname.find_last_of(L"\\/");
                                            if (lastSlash != std::wstring::npos)
                                                wname = wname.substr(lastSlash + 1);
                                            std::string name(wname.begin(), wname.end());
                                            modName = name.c_str();
                                        }
                                        catch (...)
                                        {
                                        }
                                    }
                                }
                            }
                        }

                        if (!modName.empty())
                        {
                            result.moduleName = modName;
                            result.moduleBase = static_cast<DWORD>(modBase);
                            result.rva = crashAddress - static_cast<DWORD>(modBase);

                            const bool  isExe = result.moduleName.EndsWithI(".exe");
                            const DWORD idaBase = isExe ? kDefaultExeBase : kDefaultDllBase;
                            if (result.rva <= (0xFFFFFFFF - idaBase))
                                result.idaAddress = idaBase + result.rva;
                            result.resolved = true;
                            break;
                        }
                    }
                }

                if (!result.resolved)
                    result.debugFailReason = 3;
            }
            else
            {
                result.debugFailReason = 2;
            }
        }
        else
        {
            result.debugFailReason = 2;
        }

        UnmapViewOfFile(pBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return result;
    }
}
