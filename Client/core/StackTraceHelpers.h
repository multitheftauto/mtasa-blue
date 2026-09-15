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
#include <format>
#include <optional>
#include <ranges>
#include <span>
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

        bool operator==(const StackWalkRoutines&) const noexcept = default;
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
            const auto srcPtr = reinterpret_cast<const std::byte*>(static_cast<std::uintptr_t>(srcAddr));
            const auto dest = std::span{reinterpret_cast<std::byte*>(buffer), size};
            const auto srcSpan = std::span{srcPtr, size};
            std::ranges::copy(srcSpan, dest.begin());
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
        const auto               addr32 = static_cast<std::uint32_t>(address);
        if (VirtualQuery(reinterpret_cast<LPCVOID>(static_cast<std::uintptr_t>(addr32)), &mbi, sizeof(mbi)) == 0) [[unlikely]]
            return 0;

        if (mbi.AllocationBase == nullptr) [[unlikely]]
            return 0;

        return static_cast<DWORD64>(reinterpret_cast<std::uintptr_t>(mbi.AllocationBase));
    }

    [[nodiscard]] inline StackWalkRoutines MakeStackWalkRoutines(bool useDbgHelp) noexcept
    {
        if (useDbgHelp)
        {
            return StackWalkRoutines{.readMemory = &LocalReadProcessMemory, .functionTableAccess = SymFunctionTableAccess64, .moduleBase = SymGetModuleBase64};
        }

        return StackWalkRoutines{.readMemory = &LocalReadProcessMemory, .functionTableAccess = nullptr, .moduleBase = &LocalGetModuleBase};
    }

    struct ModuleAddressInfo
    {
        std::string name;
        DWORD64     base;

        bool operator==(const ModuleAddressInfo&) const noexcept = default;
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
        const auto             lastSlash = modulePathView.find_last_of("\\/");
        const std::string_view moduleName = (lastSlash != std::string_view::npos) ? modulePathView.substr(lastSlash + 1) : modulePathView;

        ModuleAddressInfo info{.name = std::string{moduleName}, .base = static_cast<DWORD64>(reinterpret_cast<std::uintptr_t>(mbi.AllocationBase))};
        return info;
    }

    [[nodiscard]] inline std::string FormatAddressWithModule(DWORD64 address)
    {
        try
        {
            if (const auto moduleInfo = DescribeModule(address)) [[likely]]
            {
                const auto& [name, base] = *moduleInfo;

                if (address < base) [[unlikely]]
                    return std::format("0x{:X}", address);

                const DWORD64 offset = address - base;
                return std::format("{}+0x{:X}", name, offset);
            }

            return std::format("0x{:X}", address);
        }
        catch (const std::format_error&)
        {
            return "<format_error>";
        }
    }

    [[nodiscard]] inline std::string FormatAddressWithModuleAndAbsolute(DWORD64 address)
    {
        try
        {
            return std::format("{} [0x{:X}]", FormatAddressWithModule(address), address);
        }
        catch (const std::format_error&)
        {
            return "<format_error>";
        }
    }
}  // namespace StackTraceHelpers
