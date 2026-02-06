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

class SString;

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
constexpr DWORD STATUS_HEAP_CORRUPTION_CODE = 0xC0000374;
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
// Helpers implemented in CrashHandler.cpp to avoid __try in inline functions
void                     OutputDebugStringSafeImpl(const char* message);
void                     SafeDebugPrintImpl(char* buffer, std::size_t bufferSize, const char* format, va_list args);
void                     SafeDebugPrintPrefixedImpl(const char* prefix, std::size_t prefixLen, const char* format, va_list args);
extern thread_local bool g_bCrashHandlerStackOverflow;

// Helper to safely call OutputDebugStringA with SEH guards
inline void OutputDebugStringSafe(const char* message)
{
    if (g_bCrashHandlerStackOverflow)
        return;
    OutputDebugStringSafeImpl(message);
}
inline void SafeDebugOutput(std::string_view message)
{
    if (g_bCrashHandlerStackOverflow)
        return;
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
        OutputDebugStringSafeImpl(buffer);

        data += chunkLength;
        remaining -= chunkLength;
    }
}

// Overload for C-string literals (backward compatibility)
inline void SafeDebugOutput(const char* message)
{
    if (g_bCrashHandlerStackOverflow)
        return;
    OutputDebugStringSafeImpl(message);
}

// Overload for std::string (for string_view concatenation)
inline void SafeDebugOutput(const std::string& message)
{
    if (g_bCrashHandlerStackOverflow)
        return;
    if (!message.empty())
    {
        OutputDebugStringSafeImpl(message.c_str());
    }
}

template <typename Buffer>
inline void SafeDebugPrint(Buffer& buffer, const char* format, ...)
{
    if (format == nullptr)
        return;

    auto*             data = buffer.data();
    const std::size_t size = buffer.size();

    if (data == nullptr || size == 0)
        return;

    va_list args;
    va_start(args, format);
    SafeDebugPrintImpl(data, size, format, args);
    va_end(args);
}

inline void SafeDebugPrintC(char* buffer, std::size_t bufferSize, const char* format, ...)
{
    if (buffer == nullptr || bufferSize == 0 || format == nullptr)
        return;

    va_list args;
    va_start(args, format);
    SafeDebugPrintImpl(buffer, bufferSize, format, args);
    va_end(args);
}

#define SAFE_DEBUG_PRINT(buffer, ...)               SafeDebugPrint((buffer), __VA_ARGS__)
#define SAFE_DEBUG_PRINT_C(buffer, bufferSize, ...) SafeDebugPrintC((buffer), (bufferSize), __VA_ARGS__)

inline void SafeDebugPrintPrefixed(std::string_view prefix, const char* format, ...)
{
    if (format == nullptr)
        return;

    va_list args;
    va_start(args, format);
    SafeDebugPrintPrefixedImpl(prefix.data(), prefix.size(), format, args);
    va_end(args);
}

#ifdef __cplusplus

using PFNCHFILTFN = LONG(__stdcall*)(EXCEPTION_POINTERS* pExPtrs);

struct ENHANCED_EXCEPTION_INFO
{
    DWORD                                   exceptionCode{};
    void*                                   exceptionAddress{};
    std::string                             moduleName{};
    std::string                             modulePathName{};
    std::string                             moduleBaseName{};
    uint                                    moduleOffset{};
    std::chrono::system_clock::time_point   timestamp{};
    DWORD                                   threadId{};
    DWORD                                   processId{};
    std::optional<std::vector<std::string>> stackTrace{};
    std::optional<std::exception_ptr>       capturedException{};
    int                                     uncaughtExceptionCount{};
    std::string                             exceptionDescription{};
    bool                                    hasDetailedStackTrace{};

    DWORD eax{}, ebx{}, ecx{}, edx{}, esi{}, edi{}, ebp{}, esp{}, eip{}, eflags{};
    WORD  cs{}, ds{}, ss{}, es{}, fs{}, gs{};

    std::string exceptionType{};
    bool        isFatal{};
    std::string additionalInfo{};

    [[nodiscard]] bool operator==(const ENHANCED_EXCEPTION_INFO& other) const
    {
        return exceptionCode == other.exceptionCode && exceptionAddress == other.exceptionAddress && moduleName == other.moduleName &&
               modulePathName == other.modulePathName && moduleBaseName == other.moduleBaseName && moduleOffset == other.moduleOffset &&
               timestamp == other.timestamp && threadId == other.threadId && processId == other.processId && stackTrace == other.stackTrace &&
               capturedException == other.capturedException && uncaughtExceptionCount == other.uncaughtExceptionCount &&
               exceptionDescription == other.exceptionDescription && hasDetailedStackTrace == other.hasDetailedStackTrace && eax == other.eax &&
               ebx == other.ebx && ecx == other.ecx && edx == other.edx && esi == other.esi && edi == other.edi && ebp == other.ebp && esp == other.esp &&
               eip == other.eip && eflags == other.eflags && cs == other.cs && ds == other.ds && ss == other.ss && es == other.es && fs == other.fs &&
               gs == other.gs && exceptionType == other.exceptionType && isFatal == other.isFatal && additionalInfo == other.additionalInfo;
    }

    [[nodiscard]] bool operator!=(const ENHANCED_EXCEPTION_INFO& other) const { return !(*this == other); }
};
using PENHANCED_EXCEPTION_INFO = ENHANCED_EXCEPTION_INFO*;

extern "C"
{
#endif

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall SetCrashHandlerFilter(PFNCHFILTFN pFn);

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableStackCookieFailureCapture(BOOL bEnable);

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall ConfigureWerForFailFast();

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall GetEnhancedExceptionInfo(PENHANCED_EXCEPTION_INFO pExceptionInfo);

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall CaptureCurrentException();

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall LogExceptionDetails(EXCEPTION_POINTERS* pException);

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall IsFatalException(DWORD exceptionCode);

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

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall GetCrashHandlerConfiguration(PCRASH_HANDLER_CONFIG pConfig);

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall CaptureUnifiedStackTrace(_EXCEPTION_POINTERS* pException, DWORD maxFrames,
                                                                                std::vector<std::string>* pOutTrace);

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableSehExceptionHandler();

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall SetInitializationPhase(DWORD phase);

    [[nodiscard]] DWORD BUGSUTIL_DLLINTERFACE __stdcall GetInitializationPhase();

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableAllHandlersAfterInitialization();

    [[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall StartWatchdogThread(DWORD mainThreadId, DWORD timeoutSeconds = 20);

    void BUGSUTIL_DLLINTERFACE __stdcall StopWatchdogThread();

    void BUGSUTIL_DLLINTERFACE __stdcall UpdateWatchdogHeartbeat();

#ifdef __cplusplus
}
#endif

namespace CrashHandler
{
    [[nodiscard]] bool ProcessHasLocalDebugSymbols();
}
