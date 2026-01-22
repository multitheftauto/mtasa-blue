/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/StackTraceHelpers.h
 *  PURPOSE:     Crash handler component
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>

#include <windows.h>
#include <DbgHelp.h>

namespace StackTraceHelpers
{
    struct StackWalkRoutines
    {
        PREAD_PROCESS_MEMORY_ROUTINE64   readMemory = nullptr;
        PFUNCTION_TABLE_ACCESS_ROUTINE64 functionTableAccess = nullptr;
        PGET_MODULE_BASE_ROUTINE64       moduleBase = nullptr;

        bool operator==(const StackWalkRoutines& other) const noexcept
        {
            return readMemory == other.readMemory &&
                   functionTableAccess == other.functionTableAccess &&
                   moduleBase == other.moduleBase;
        }

        bool operator!=(const StackWalkRoutines& other) const noexcept
        {
            return !(*this == other);
        }
    };

    inline BOOL __stdcall LocalReadProcessMemory(HANDLE /*process*/, DWORD64 baseAddress, PVOID buffer, DWORD size, LPDWORD bytesRead) noexcept
    {
        if (buffer == nullptr || size == 0) [[unlikely]]
        {
            if (bytesRead)
                *bytesRead = 0;
            return FALSE;
        }

        if (baseAddress > static_cast<DWORD64>(UINT32_MAX)) [[unlikely]]
        {
            if (bytesRead)
                *bytesRead = 0;
            return FALSE;
        }

        __try
        {
            const auto srcAddr = static_cast<std::uint32_t>(baseAddress);
            const auto srcPtr = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(srcAddr));
            std::memcpy(buffer, srcPtr, size);
            if (bytesRead)
                *bytesRead = size;
            return TRUE;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            if (bytesRead)
                *bytesRead = 0;
            return FALSE;
        }
    }

    inline DWORD64 __stdcall LocalGetModuleBase(HANDLE /*process*/, DWORD64 address) noexcept
    {
        constexpr DWORD64 max32BitAddress = static_cast<DWORD64>(UINT32_MAX);
        if (address > max32BitAddress) [[unlikely]]
            return 0;

        MEMORY_BASIC_INFORMATION mbi{};
        const auto addr32 = static_cast<std::uint32_t>(address);
        if (VirtualQuery(reinterpret_cast<LPCVOID>(static_cast<std::uintptr_t>(addr32)), &mbi, sizeof(mbi)) == 0) [[unlikely]]
            return 0;

        if (mbi.AllocationBase == nullptr) [[unlikely]]
            return 0;

        return static_cast<DWORD64>(reinterpret_cast<std::uintptr_t>(mbi.AllocationBase));
    }

    [[nodiscard]] inline StackWalkRoutines MakeStackWalkRoutines(bool useDbgHelp) noexcept
    {
        StackWalkRoutines routines{};
        routines.readMemory = &LocalReadProcessMemory;

        if (useDbgHelp)
        {
            routines.functionTableAccess = SymFunctionTableAccess64;
            routines.moduleBase = SymGetModuleBase64;
        }
        else
        {
            routines.functionTableAccess = nullptr;
            routines.moduleBase = &LocalGetModuleBase;
        }

        return routines;
    }

    struct ModuleAddressInfo
    {
        std::string name;
        DWORD64     base;

        bool operator==(const ModuleAddressInfo& other) const noexcept
        {
            return name == other.name && base == other.base;
        }

        bool operator!=(const ModuleAddressInfo& other) const noexcept
        {
            return !(*this == other);
        }
    };

    [[nodiscard]] inline std::optional<ModuleAddressInfo> DescribeModule(DWORD64 address)
    {
        if (address == 0) [[unlikely]]
            return std::nullopt;

        constexpr DWORD64 max32BitAddress = static_cast<DWORD64>(UINT32_MAX);
        if (address > max32BitAddress) [[unlikely]]
            return std::nullopt;

        MEMORY_BASIC_INFORMATION mbi{};
        if (const auto addr32 = static_cast<std::uint32_t>(address);
            VirtualQuery(reinterpret_cast<LPCVOID>(static_cast<std::uintptr_t>(addr32)), &mbi, sizeof(mbi)) == 0) [[unlikely]]
            return std::nullopt;

        if (mbi.AllocationBase == nullptr) [[unlikely]]
            return std::nullopt;

        std::array<char, MAX_PATH> modulePath{};
        const DWORD pathLen = GetModuleFileNameA(reinterpret_cast<HMODULE>(mbi.AllocationBase), modulePath.data(), static_cast<DWORD>(modulePath.size()));

        if (pathLen == 0) [[unlikely]]
            return std::nullopt;

        if (pathLen >= modulePath.size()) [[unlikely]]
        {
            modulePath.back() = '\0';
            return std::nullopt;
        }

        const std::string_view modulePathView{modulePath.data(), pathLen};
        const auto lastSlash = modulePathView.find_last_of("\\/");
        const std::string_view moduleName = (lastSlash != std::string_view::npos) 
            ? modulePathView.substr(lastSlash + 1) 
            : modulePathView;

        ModuleAddressInfo info;
        info.name = std::string{moduleName};
        info.base = static_cast<DWORD64>(reinterpret_cast<std::uintptr_t>(mbi.AllocationBase));
        return info;
    }

    [[nodiscard]] inline std::string FormatAddressWithModule(DWORD64 address)
    {
        std::array<char, 512> buffer{};

        if (const auto moduleInfo = DescribeModule(address))
        {
            const auto& name = moduleInfo->name;
            const auto base = moduleInfo->base;

            if (address < base)
            {
                _snprintf_s(buffer.data(), buffer.size(), _TRUNCATE, "0x%llX", address);
                return std::string{buffer.data()};
            }

            const DWORD64 offset = address - base;
            _snprintf_s(buffer.data(), buffer.size(), _TRUNCATE, "%s+0x%llX", name.c_str(), offset);
            return std::string{buffer.data()};
        }

        _snprintf_s(buffer.data(), buffer.size(), _TRUNCATE, "0x%llX", address);
        return std::string{buffer.data()};
    }

    [[nodiscard]] inline std::string FormatAddressWithModuleAndAbsolute(DWORD64 address)
    {
        std::array<char, 512> buffer{};
        const auto moduleStr = FormatAddressWithModule(address);
        _snprintf_s(buffer.data(), buffer.size(), _TRUNCATE, "%s [0x%llX]", moduleStr.c_str(), address);
        return std::string{buffer.data()};
    }
} // namespace StackTraceHelpers
