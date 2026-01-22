/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/FastFailCrashHandler/WerCrashHandler.h
 *  PURPOSE:     WER crash handler declarations.
 *               Capable of dump, register, and stacktrace collection fo 0xC0000409 / 0xC0000374 crashes.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <windows.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <vector>

#include "SString.h"

namespace WerCrash
{
    using FileTimeDuration = std::chrono::duration<std::int64_t, std::ratio<1, 10000000>>;

    struct MinidumpRegisters
    {
        bool valid = false;
        DWORD eax = 0, ebx = 0, ecx = 0, edx = 0;
        DWORD esi = 0, edi = 0, ebp = 0, esp = 0, eip = 0;
        DWORD eflags = 0;
        WORD cs = 0, ds = 0, es = 0, fs = 0, gs = 0, ss = 0;
        DWORD exceptionCode = 0;
        DWORD64 exceptionAddress = 0;
    };

    struct WerDumpResult
    {
        SString path;
        SString sourceFilename;
        MinidumpRegisters regs;
        SString stackTrace;
    };

    struct ModuleEntry
    {
        SString name;
        DWORD   base = 0;
        DWORD   size = 0;
    };

    struct ResolvedModule
    {
        bool    resolved = false;
        SString moduleName;
        DWORD   moduleBase = 0;
        DWORD   crashAddress = 0;
        DWORD   rva = 0;
        DWORD   idaAddress = 0;
        int     debugChunkCount = 0;
        int     debugModuleCount = 0;
        int     debugFailReason = 0;
    };

    [[nodiscard]] MinidumpRegisters ExtractRegistersFromMinidump(const SString& dumpPath);
    [[nodiscard]] SString ExtractStackTraceFromMinidump(const SString& dumpPath, const MinidumpRegisters& regs);

    void AppendWerInfoToDump(const SString& dumpPath, const SString& moduleName, DWORD offset, DWORD exceptionCode,
                            const MinidumpRegisters& regs);

    [[nodiscard]] SString RenameWerDumpToMtaFormat(const SString& sourcePath, const SString& dumpDir,
                                                  const SString& moduleName, DWORD offset, DWORD exceptionCode,
                                                  const MinidumpRegisters& regs);

    [[nodiscard]] WerDumpResult FindAndRenameWerDump(const SString& dumpDir, const SString& moduleName, DWORD offset,
                                                     DWORD exceptionCode, const std::optional<FileTimeDuration>& processCreationTime);

    [[nodiscard]] bool IsFileRecentEnough(HANDLE hFile, std::uint64_t maxAgeMinutes = 15) noexcept;

    void UpdateModuleBases();

    [[nodiscard]] std::vector<ModuleEntry> GetModuleBasesFromRegistry();

    [[nodiscard]] ResolvedModule ResolveAddressToModule(DWORD crashAddress);

    [[nodiscard]] ResolvedModule ResolveAddressToModule(DWORD crashAddress, const std::vector<ModuleEntry>& modules);

    [[nodiscard]] ResolvedModule ResolveAddressFromMinidump(const SString& dumpPath, DWORD crashAddress);
}
