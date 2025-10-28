/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CrashHandler.h
 *  PURPOSE:     Header file for crash handling functions
 *
 *  THIS FILE CREDITS (IS BASED ON): "Debugging Applications" (Microsoft Press) by John Robbins
 *  Copyright (c) 1997-2000 John Robbins -- All rights reserved
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#if !defined(BUGSUTIL_DLLINTERFACE)
    #if defined(BUGSUTIL_EXPORTS)
        #define BUGSUTIL_DLLINTERFACE __declspec(dllexport)
    #else
        #define BUGSUTIL_DLLINTERFACE
    #endif
#endif

constexpr std::size_t DEBUG_BUFFER_SIZE = 256;
static_assert(DEBUG_BUFFER_SIZE > 1, "DEBUG_BUFFER_SIZE must allow for null termination");

constexpr DWORD CPP_EXCEPTION_CODE = 0xE06D7363;
constexpr DWORD STATUS_INVALID_CRUNTIME_PARAMETER_CODE = 0xC0000417;
constexpr DWORD STATUS_STACK_BUFFER_OVERRUN_CODE = 0xC0000409;
// STATUS_FATAL_USER_CALLBACK_EXCEPTION (0xC000041D):
// This exception type occurs when an exception happens inside a Windows system callback
// (e.g., window procedures, DLL callbacks, kernel-to-user transitions) and cannot be
// properly unwound. Special handling is required as:
// - The stack may be corrupted or incomplete
// - Context may not have full information
// - Stack walking may fail or cause secondary exceptions
// - Module resolution may point to trampolines instead of actual code
constexpr DWORD STATUS_FATAL_USER_CALLBACK_EXCEPTION = 0xC000041D;
constexpr DWORD CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT = 0xE0000001;

constexpr DWORD INIT_PHASE_MINIMAL = 0;
constexpr DWORD INIT_PHASE_D3D_INIT = 1;
constexpr DWORD INIT_PHASE_POST_D3D = 2;

constexpr int EXIT_CODE_CRASH = 3;

constexpr std::size_t DEBUG_BUFFER_SIZE_LARGE = 512;

constexpr DWORD EXCEPTION_BREAKPOINT_NONFATAL = 0x80000003;
constexpr DWORD EXCEPTION_SINGLE_STEP_NONFATAL = 0x80000004;
constexpr DWORD EXCEPTION_GUARD_PAGE_NONFATAL = 0x80000001;
constexpr DWORD EXCEPTION_INVALID_HANDLE_NONFATAL = 0xC0000008;
constexpr DWORD EXCEPTION_ARRAY_BOUNDS_EXCEEDED_NONFATAL = 0xC000008C;
constexpr DWORD EXCEPTION_FLT_DENORMAL_OPERAND_NONFATAL = 0xC000008D;
constexpr DWORD EXCEPTION_FLT_DIVIDE_BY_ZERO_NONFATAL = 0xC000008E;
constexpr DWORD EXCEPTION_FLT_INEXACT_RESULT_NONFATAL = 0xC000008F;
constexpr DWORD EXCEPTION_FLT_INVALID_OPERATION_NONFATAL = 0xC0000090;
constexpr DWORD EXCEPTION_FLT_OVERFLOW_NONFATAL = 0xC0000091;
constexpr DWORD EXCEPTION_FLT_STACK_CHECK_NONFATAL = 0xC0000092;
constexpr DWORD EXCEPTION_FLT_UNDERFLOW_NONFATAL = 0xC0000093;
constexpr DWORD EXCEPTION_INT_DIVIDE_BY_ZERO_NONFATAL = 0xC0000094;
constexpr DWORD EXCEPTION_INT_OVERFLOW_NONFATAL = 0xC0000095;
constexpr DWORD EXCEPTION_PRIV_INSTRUCTION_NONFATAL = 0xC0000096;
constexpr DWORD EXCEPTION_NONCONTINUABLE_EXCEPTION_NONFATAL = 0xC0000025;

inline constexpr std::string_view DEBUG_PREFIX_CRASH = "CrashHandler: ";
inline constexpr std::string_view DEBUG_PREFIX_SEH = "SEH: ";
inline constexpr std::string_view DEBUG_PREFIX_CPP = "C++ TERMINATE: ";
inline constexpr std::string_view DEBUG_PREFIX_ABORT = "ABORT: ";
inline constexpr std::string_view DEBUG_PREFIX_PURECALL = "PURECALL: ";
inline constexpr std::string_view DEBUG_PREFIX_EXCEPTION_INFO = "ExceptionInfo: ";
inline constexpr std::string_view DEBUG_PREFIX_WATCHDOG = "WATCHDOG: ";
inline constexpr std::string_view DEBUG_SEPARATOR = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";

inline void SafeDebugOutput(std::string_view message) noexcept
{
    const char* data = message.data();
    std::size_t remaining = message.size();

    if (data == nullptr || remaining == 0)
    {
        return;
    }

    char buffer[DEBUG_BUFFER_SIZE] = {};

    while (remaining > 0)
    {
        const std::size_t chunkLength = std::min<std::size_t>(remaining, DEBUG_BUFFER_SIZE - 1);
        std::memcpy(buffer, data, chunkLength);
        buffer[chunkLength] = '\0';
        OutputDebugStringA(buffer);

        data += chunkLength;
        remaining -= chunkLength;
    }
}

// Overload for C-string literals (backward compatibility)
inline void SafeDebugOutput(const char* message) noexcept
{
    if (message != nullptr)
    {
        OutputDebugStringA(message);
    }
}

// Overload for std::string (for string_view concatenation)
inline void SafeDebugOutput(const std::string& message) noexcept
{
    if (!message.empty())
    {
        OutputDebugStringA(message.c_str());
    }
}

template <typename Buffer>
inline void SafeDebugPrint(Buffer& buffer, const char* format, ...) noexcept
{
    if (format == nullptr)
        return;

    auto* data = buffer.data();
    const std::size_t size = buffer.size();

    if (data == nullptr || size == 0)
        return;

    std::memset(data, 0, size);

    va_list args;
    va_start(args, format);
    const int written = _vsnprintf_s(data, size, _TRUNCATE, format, args);
    va_end(args);

    if (written > 0)
    {
        OutputDebugStringA(data);
    }
}

inline void SafeDebugPrintC(char* buffer, std::size_t bufferSize, const char* format, ...) noexcept
{
    if (buffer == nullptr || bufferSize == 0 || format == nullptr)
        return;

    va_list args;
    va_start(args, format);
    const int written = _vsnprintf_s(buffer, bufferSize, _TRUNCATE, format, args);
    va_end(args);

    if (written > 0)
    {
        OutputDebugStringA(buffer);
    }
}

#define SAFE_DEBUG_PRINT(buffer, ...) SafeDebugPrint((buffer), __VA_ARGS__)
#define SAFE_DEBUG_PRINT_C(buffer, bufferSize, ...) SafeDebugPrintC((buffer), (bufferSize), __VA_ARGS__)

inline void SafeDebugPrintPrefixed(std::string_view prefix, const char* format, ...) noexcept
{
    if (format == nullptr)
        return;

    char buffer[DEBUG_BUFFER_SIZE] = {};

    std::size_t offset = 0;
    if (!prefix.empty())
    {
        offset = std::min<std::size_t>(prefix.size(), DEBUG_BUFFER_SIZE - 1);
        std::memcpy(buffer, prefix.data(), offset);
    }

    if (offset >= DEBUG_BUFFER_SIZE - 1)
    {
        buffer[DEBUG_BUFFER_SIZE - 1] = '\0';
        OutputDebugStringA(buffer);
        return;
    }

    va_list args;
    va_start(args, format);
    const int written = _vsnprintf_s(buffer + offset, DEBUG_BUFFER_SIZE - offset, _TRUNCATE, format, args);
    va_end(args);

    if (written > 0 || offset > 0)
    {
        OutputDebugStringA(buffer);
    }
}
#ifdef __cplusplus
extern "C"
{
#endif

    typedef LONG(__stdcall* PFNCHFILTFN)(EXCEPTION_POINTERS* pExPtrs);

    typedef struct _ENHANCED_EXCEPTION_INFO
    {
        DWORD                                   exceptionCode;
        void*                                   exceptionAddress;
        std::string                             moduleName;
        std::string                             modulePathName;
        std::string                             moduleBaseName;
        uint                                    moduleOffset;
        std::chrono::system_clock::time_point   timestamp;
        DWORD                                   threadId;
        DWORD                                   processId;
        std::optional<std::vector<std::string>> stackTrace;
        std::optional<std::exception_ptr>       capturedException;
        int                                     uncaughtExceptionCount;
        std::string                             exceptionDescription;
        bool                                    hasDetailedStackTrace;

        DWORD eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
        WORD  cs, ds, ss, es, fs, gs;

        std::string exceptionType;
        bool        isFatal;
        std::string additionalInfo;
    } ENHANCED_EXCEPTION_INFO, *PENHANCED_EXCEPTION_INFO;

    BOOL BUGSUTIL_DLLINTERFACE __stdcall SetCrashHandlerFilter(PFNCHFILTFN pFn) noexcept;

    BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableStackCookieFailureCapture(BOOL bEnable) noexcept;

    BOOL BUGSUTIL_DLLINTERFACE __stdcall GetEnhancedExceptionInfo(PENHANCED_EXCEPTION_INFO pExceptionInfo) noexcept;

    BOOL BUGSUTIL_DLLINTERFACE __stdcall CaptureCurrentException() noexcept;

    BOOL BUGSUTIL_DLLINTERFACE __stdcall LogExceptionDetails(EXCEPTION_POINTERS* pException) noexcept;

    BOOL BUGSUTIL_DLLINTERFACE __stdcall IsFatalException(DWORD exceptionCode) noexcept;

    typedef struct _CRASH_HANDLER_CONFIG
    {
        BOOL  disableSehDetour;
        BOOL  forceSehDetour;
        BOOL  enableStackCookieCapture;
        BOOL  enableEnhancedLogging;
        BOOL  enableSehHandler;
        BOOL  enableCppHandlers;
        DWORD memoryReservationKB;
        DWORD maxStackTraceFrames;
        DWORD debugBufferSize;
    } CRASH_HANDLER_CONFIG, *PCRASH_HANDLER_CONFIG;

    BOOL BUGSUTIL_DLLINTERFACE __stdcall GetCrashHandlerConfiguration(PCRASH_HANDLER_CONFIG pConfig) noexcept;

    BOOL BUGSUTIL_DLLINTERFACE __stdcall CaptureUnifiedStackTrace(_EXCEPTION_POINTERS* pException, DWORD maxFrames,
                                                                  std::vector<std::string>* pOutTrace) noexcept;

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableSehExceptionHandler() noexcept;

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall SetInitializationPhase(DWORD phase) noexcept;

    [[nodiscard]] DWORD BUGSUTIL_DLLINTERFACE __stdcall GetInitializationPhase() noexcept;

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableAllHandlersAfterInitialization() noexcept;

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall StartWatchdogThread(DWORD mainThreadId, DWORD timeoutSeconds = 20) noexcept;

    void BUGSUTIL_DLLINTERFACE __stdcall StopWatchdogThread() noexcept;

    void BUGSUTIL_DLLINTERFACE __stdcall UpdateWatchdogHeartbeat() noexcept;

#ifdef __cplusplus
}
#endif