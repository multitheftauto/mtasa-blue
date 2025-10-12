/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CrashHandler.h
 *  PURPOSE:     Header file for crash handling functions
 *
 *  THIS FILE CREDITS: "Debugging Applications" (Microsoft Press) by John Robbins
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <chrono>
#include <exception>
#include <optional>
#include <string>
#include <vector>

#if !defined(BUGSUTIL_DLLINTERFACE)
    #if defined(BUGSUTIL_EXPORTS)
        #define BUGSUTIL_DLLINTERFACE __declspec(dllexport)
    #else
        #define BUGSUTIL_DLLINTERFACE
    #endif
#endif

constexpr std::size_t DEBUG_BUFFER_SIZE = 256;

constexpr DWORD CPP_EXCEPTION_CODE = 0xE06D7363;
constexpr DWORD STATUS_INVALID_CRUNTIME_PARAMETER_CODE = 0xC0000417;
constexpr DWORD STATUS_STACK_BUFFER_OVERRUN_CODE = 0xC0000409;

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

#define DEBUG_PREFIX_CRASH          "CrashHandler: "
#define DEBUG_PREFIX_SEH            "SEH: "
#define DEBUG_PREFIX_CPP            "C++ TERMINATE: "
#define DEBUG_PREFIX_ABORT          "ABORT: "
#define DEBUG_PREFIX_PURECALL       "PURECALL: "
#define DEBUG_PREFIX_EXCEPTION_INFO "ExceptionInfo: "
#define DEBUG_SEPARATOR             "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"

inline void SafeDebugOutput(const char* message) noexcept
{
    if (message != nullptr)
    {
        OutputDebugStringA(message);
    }
}

#define SAFE_DEBUG_PRINT(buffer, format, ...) \
    do \
    { \
        if ((buffer).data() != nullptr && (buffer).size() > 0) \
        { \
            memset((buffer).data(), 0, (buffer).size()); \
            int result = _snprintf_s((buffer).data(), (buffer).size(), _TRUNCATE, format, __VA_ARGS__); \
            if (result > 0) \
            { \
                OutputDebugStringA((buffer).data()); \
            } \
        } \
    } while (false)

#define SAFE_DEBUG_PRINT_C(buffer, bufferSize, format, ...) \
    do \
    { \
        if (buffer != nullptr && bufferSize > 0) \
        { \
            int result = _snprintf_s(buffer, bufferSize, _TRUNCATE, format, __VA_ARGS__); \
            if (result > 0) \
            { \
                OutputDebugStringA(buffer); \
            } \
        } \
    } while (false)

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

#ifdef __cplusplus
}
#endif