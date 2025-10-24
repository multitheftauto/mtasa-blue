/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CrashHandler.cpp
 *  PURPOSE:     Crash handling functions
 *
 *  THIS FILE CREDITS (IS BASED ON): "Debugging Applications" (Microsoft Press) by John Robbins
 *  Copyright (c) 1997-2000 John Robbins -- All rights reserved
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#define BUGSUTIL_EXPORTS
#include "StdInc.h"
#include "CrashHandler.h"
#include "CExceptionInformation_Impl.h"
#include <SharedUtil.Detours.h>
#include <SharedUtil.Misc.h>
#include <SharedUtil.Memory.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <exception>
#include <errno.h>
#include <filesystem>
#include <intrin.h>
#include <mutex>
#include <new>
#include <optional>
#include <process.h>
#include <signal.h>
#include <stdio.h>
#include <string_view>
#include <variant>
#include <vector>

#if defined(_MSC_VER)
    #include <corecrt.h>
#endif

using namespace std::string_literals;

[[nodiscard]] constexpr bool IsMemoryException(DWORD code) noexcept
{
    return code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_IN_PAGE_ERROR;
}

[[nodiscard]] constexpr bool IsFloatingPointException(DWORD code) noexcept
{
    return code >= EXCEPTION_FLT_DENORMAL_OPERAND && code <= EXCEPTION_FLT_UNDERFLOW;
}

[[nodiscard]] constexpr bool IsIntegerException(DWORD code) noexcept
{
    return code == EXCEPTION_INT_DIVIDE_BY_ZERO || code == EXCEPTION_INT_OVERFLOW;
}

#ifndef STATUS_INVALID_PARAMETER
inline constexpr DWORD STATUS_INVALID_PARAMETER = 0xC000000DL;
#endif

inline std::optional<ENHANCED_EXCEPTION_INFO> g_lastExceptionInfo;
inline std::mutex                             g_exceptionInfoMutex;

inline std::atomic<DWORD> g_initializationPhase{INIT_PHASE_MINIMAL};

using CrashHandlerResult = std::variant<std::monostate, std::string, DWORD, std::exception_ptr>;

[[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall IsFatalException(DWORD exceptionCode) noexcept
{
    switch (exceptionCode)
    {
        case EXCEPTION_BREAKPOINT:
        case EXCEPTION_SINGLE_STEP:
        case EXCEPTION_GUARD_PAGE:
        case EXCEPTION_INVALID_HANDLE:
        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_UNDERFLOW:
            return FALSE;
    }

    switch (exceptionCode)
    {
        case EXCEPTION_ILLEGAL_INSTRUCTION:
        case EXCEPTION_STACK_OVERFLOW:
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        case STATUS_STACK_BUFFER_OVERRUN_CODE:
        case STATUS_INVALID_CRUNTIME_PARAMETER_CODE:
        case CPP_EXCEPTION_CODE:
        case CUSTOM_EXCEPTION_CODE_OOM:
        case CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT:
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_OVERFLOW:
        case EXCEPTION_PRIV_INSTRUCTION:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
            return TRUE;
    }

    if (IsMemoryException(exceptionCode))
    {
        constexpr auto bufferSize = DEBUG_BUFFER_SIZE;
        char           debugBuffer[bufferSize];
        SAFE_DEBUG_PRINT_C(debugBuffer, bufferSize, "IsFatalException: Access violation/page fault 0x%08X - treating as fatal for dump generation\n",
                           exceptionCode);
        return TRUE;
    }

    constexpr auto bufferSize = DEBUG_BUFFER_SIZE;
    char           debugBuffer[bufferSize];
    SAFE_DEBUG_PRINT_C(debugBuffer, bufferSize, "IsFatalException: Unknown exception code 0x%08X, treating as non-fatal\n", exceptionCode);
    return FALSE;
}

static void LogBasicExceptionInfo(_EXCEPTION_POINTERS* exception) noexcept
{
    if (exception == nullptr || exception->ExceptionRecord == nullptr)
        return;

    SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "Exception 0x%08X at 0x%p (TID: %lu)\n",
                           exception->ExceptionRecord->ExceptionCode,
                           exception->ExceptionRecord->ExceptionAddress,
                           GetCurrentThreadId());
}

static void StoreBasicExceptionInfo(_EXCEPTION_POINTERS* pException) noexcept
{
    if (pException == nullptr || pException->ExceptionRecord == nullptr)
        return;

    try
    {
        std::unique_lock<std::mutex> lock(g_exceptionInfoMutex, std::try_to_lock);
        if (!lock.owns_lock())
        {
            LogBasicExceptionInfo(pException);
            return;
        }

        ENHANCED_EXCEPTION_INFO info = {};
        info.exceptionCode = pException->ExceptionRecord->ExceptionCode;
        info.exceptionAddress = pException->ExceptionRecord->ExceptionAddress;
        info.threadId = GetCurrentThreadId();
        info.processId = GetCurrentProcessId();
        info.timestamp = std::chrono::system_clock::now();

        g_lastExceptionInfo = info;
    }
    catch (...)
    {
        LogBasicExceptionInfo(pException);
    }
}

[[nodiscard]] static std::string_view GetExceptionCodeDescription(DWORD exceptionCode) noexcept
{
    switch (exceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            return "Access Violation - Invalid memory access";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            return "Array Bounds Exceeded";
        case EXCEPTION_BREAKPOINT:
            return "Breakpoint Hit";
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            return "Data Type Misalignment";
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            return "Floating Point - Denormal Operand";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            return "Floating Point - Divide by Zero";
        case EXCEPTION_FLT_INEXACT_RESULT:
            return "Floating Point - Inexact Result";
        case EXCEPTION_FLT_INVALID_OPERATION:
            return "Floating Point - Invalid Operation";
        case EXCEPTION_FLT_OVERFLOW:
            return "Floating Point - Overflow";
        case EXCEPTION_FLT_STACK_CHECK:
            return "Floating Point - Stack Check";
        case EXCEPTION_FLT_UNDERFLOW:
            return "Floating Point - Underflow";
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            return "Illegal Instruction";
        case EXCEPTION_IN_PAGE_ERROR:
            return "In Page Error - Page fault";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            return "Integer Divide by Zero";
        case EXCEPTION_INT_OVERFLOW:
            return "Integer Overflow";
        case EXCEPTION_INVALID_DISPOSITION:
            return "Invalid Disposition";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            return "Non-Continuable Exception";
        case EXCEPTION_PRIV_INSTRUCTION:
            return "Privileged Instruction";
        case EXCEPTION_SINGLE_STEP:
            return "Single Step (Debugging)";
        case EXCEPTION_STACK_OVERFLOW:
            return "Stack Overflow";
        case EXCEPTION_GUARD_PAGE:
            return "Guard Page Violation";
        case EXCEPTION_INVALID_HANDLE:
            return "Invalid Handle";
        case CPP_EXCEPTION_CODE:
            return "C++ Exception";
        case STATUS_STACK_BUFFER_OVERRUN_CODE:
            return "Stack Buffer Overrun (Security Check)";
        case STATUS_INVALID_CRUNTIME_PARAMETER_CODE:
            return "Invalid C Runtime Parameter";
        case CUSTOM_EXCEPTION_CODE_OOM:
            return "Out of Memory - Allocation Failure";
        case CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT:
            return "Watchdog Timeout - Application Freeze Detected";
        case 0xC0000374:
            return "Heap Corruption Detected";
        case 0xC0000420:
            return "Assertion Failure";
        case 0x40000015:
            return "Fatal App Exit";
        case 0xC00002C9:
            return "Register NaT Consumption";
        case 0xC0000194:
            return "Possible Deadlock Condition";
        case 0x80000029:
            return "Unwind Consolidate (Frame consolidation)";
        default:
            return "Unknown Exception";
    }
}

static void LogEnhancedExceptionInfo(_EXCEPTION_POINTERS* pException) noexcept
{
    if (pException == nullptr || pException->ExceptionRecord == nullptr)
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogEnhancedExceptionInfo - NULL exception pointers\n");
        return;
    }

    if (pException->ContextRecord == nullptr)
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogEnhancedExceptionInfo - NULL context record\n");
        LogBasicExceptionInfo(pException);
        return;
    }

    try
    {
        std::scoped_lock lock{g_exceptionInfoMutex};

        ENHANCED_EXCEPTION_INFO info{};

        info.exceptionCode = pException->ExceptionRecord->ExceptionCode;
        info.exceptionAddress = pException->ExceptionRecord->ExceptionAddress;
        info.timestamp = std::chrono::system_clock::now();
        info.threadId = GetCurrentThreadId();
        info.processId = GetCurrentProcessId();

        info.eax = pException->ContextRecord->Eax;
        info.ebx = pException->ContextRecord->Ebx;
        info.ecx = pException->ContextRecord->Ecx;
        info.edx = pException->ContextRecord->Edx;
        info.esi = pException->ContextRecord->Esi;
        info.edi = pException->ContextRecord->Edi;
        info.ebp = pException->ContextRecord->Ebp;
        info.esp = pException->ContextRecord->Esp;
        info.eip = pException->ContextRecord->Eip;
        info.cs = pException->ContextRecord->SegCs;
        info.ds = pException->ContextRecord->SegDs;
        info.es = pException->ContextRecord->SegEs;
        info.fs = pException->ContextRecord->SegFs;
        info.gs = pException->ContextRecord->SegGs;
        info.ss = pException->ContextRecord->SegSs;
        info.eflags = pException->ContextRecord->EFlags;

        CExceptionInformation_Impl moduleHelper;
        constexpr std::size_t      moduleBufferSize = 512;
        char                       moduleBuffer[moduleBufferSize]{};
        void*                      moduleBase = nullptr;

        // For EIP=0 crashes, resolve module from return address at [ESP] instead of EIP
        void* addressToResolve = info.exceptionAddress;
        constexpr auto kNullAddress = uintptr_t{0};
        
        const auto exceptionAddressValue = reinterpret_cast<uintptr_t>(info.exceptionAddress);
        
        if (exceptionAddressValue == kNullAddress && info.eip == kNullAddress)
        {
            const auto espAddr = static_cast<uintptr_t>(info.esp);
            const auto pReturnAddress = reinterpret_cast<void* const*>(espAddr);

            SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH,
                                   "LogEnhancedExceptionInfo - EIP=0 detected (ESP=0x%08X), reading return address...\n",
                                   static_cast<unsigned int>(espAddr));

            if (SharedUtil::IsReadablePointer(pReturnAddress, sizeof(void*)))
            {
                addressToResolve = *pReturnAddress;

                const auto returnAddressValue = reinterpret_cast<uintptr_t>(addressToResolve);
                SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH,
                                       "LogEnhancedExceptionInfo - Return address: 0x%08X (resolving module from this)\n",
                                       static_cast<unsigned int>(returnAddressValue));
            }
            else
            {
                SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH,
                                       "LogEnhancedExceptionInfo - ESP not readable (0x%08X), cannot read return address\n",
                                       static_cast<unsigned int>(espAddr));
            }
        }

        if (moduleHelper.GetModule(addressToResolve, moduleBuffer, sizeof(moduleBuffer), &moduleBase))
        {
            info.modulePathName = std::string(moduleBuffer);

            const std::string_view moduleView(moduleBuffer);
            if (const auto pos = moduleView.rfind('\\'); pos != std::string_view::npos)
            {
                info.moduleName = std::string(moduleView.substr(pos + 1));
                info.moduleBaseName = info.moduleName;
            }
            else
            {
                info.moduleName = std::string(moduleView);
                info.moduleBaseName = info.moduleName;
            }

            if (moduleBase != nullptr)
            {
                // Use addressToResolve for offset calculation (handles EIP=0 return address case)
                const uintptr_t resolvedAddr = reinterpret_cast<uintptr_t>(addressToResolve);
                const uintptr_t baseAddr = reinterpret_cast<uintptr_t>(moduleBase);

                if (resolvedAddr >= baseAddr)
                {
                    const uintptr_t offset = resolvedAddr - baseAddr;

                    if (offset <= UINT_MAX)
                    {
                        info.moduleOffset = static_cast<uint>(offset);
                    }
                    else
                    {
                        info.moduleOffset = 0;
                        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogEnhancedExceptionInfo - Module offset exceeds UINT_MAX\n");
                    }
                }
                else
                {
                    info.moduleOffset = 0;
                    SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogEnhancedExceptionInfo - Exception address before module base\n");
                }
            }
            else
            {
                info.moduleOffset = 0;
            }
        }
        else
        {
            SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogEnhancedExceptionInfo - Failed to get module info\n");
            info.moduleName = "Unknown";
            info.moduleBaseName = "Unknown";
            info.modulePathName = "Unknown";
            info.moduleOffset = 0;
        }

        std::vector<std::string> stackTraceVec;
        if (CaptureUnifiedStackTrace(pException, 32, &stackTraceVec))
        {
            info.stackTrace = stackTraceVec;
            info.hasDetailedStackTrace = !stackTraceVec.empty();
        }
        else
        {
            info.hasDetailedStackTrace = false;
        }

        info.uncaughtExceptionCount = std::uncaught_exceptions();
        if (info.uncaughtExceptionCount > 0)
        {
            try
            {
                info.capturedException = std::current_exception();
            }
            catch (...)
            {
                info.capturedException = std::nullopt;
            }
        }
        else
        {
            info.capturedException = std::nullopt;
        }

        info.isFatal = IsFatalException(info.exceptionCode) != FALSE;

        switch (info.exceptionCode)
        {
            case EXCEPTION_ACCESS_VIOLATION:
                info.exceptionType = "SEH:AccessViolation";
                break;
            case EXCEPTION_STACK_OVERFLOW:
                info.exceptionType = "SEH:StackOverflow";
                break;
            case CPP_EXCEPTION_CODE:
                info.exceptionType = "C++";
                break;
            case STATUS_STACK_BUFFER_OVERRUN_CODE:
                info.exceptionType = "SEH:BufferOverrun";
                break;
            case STATUS_INVALID_CRUNTIME_PARAMETER_CODE:
                info.exceptionType = "SEH:InvalidCRTParameter";
                break;
            case EXCEPTION_BREAKPOINT:
                info.exceptionType = "SEH:Breakpoint";
                break;
            case EXCEPTION_SINGLE_STEP:
                info.exceptionType = "SEH:SingleStep";
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                info.exceptionType = "SEH:ArrayBounds";
                break;
            case EXCEPTION_FLT_DENORMAL_OPERAND:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_FLT_INEXACT_RESULT:
            case EXCEPTION_FLT_INVALID_OPERATION:
            case EXCEPTION_FLT_OVERFLOW:
            case EXCEPTION_FLT_STACK_CHECK:
            case EXCEPTION_FLT_UNDERFLOW:
                info.exceptionType = "SEH:FloatingPoint";
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_OVERFLOW:
                info.exceptionType = "SEH:IntegerMath";
                break;
            case EXCEPTION_PRIV_INSTRUCTION:
                info.exceptionType = "SEH:PrivilegedInstruction";
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                info.exceptionType = "SEH:IllegalInstruction";
                break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
                info.exceptionType = "SEH:DataMisalignment";
                break;
            case EXCEPTION_IN_PAGE_ERROR:
                info.exceptionType = "SEH:PageError";
                break;
            case EXCEPTION_GUARD_PAGE:
                info.exceptionType = "SEH:GuardPage";
                break;
            default:
                if (info.exceptionCode >= 0xC0000000 && info.exceptionCode <= 0xCFFFFFFF)
                {
                    info.exceptionType = "SEH:Unknown";
                }
                else if (info.exceptionCode >= 0xE0000000)
                {
                    info.exceptionType = "User";
                }
                else
                {
                    info.exceptionType = "Unknown";
                }
                break;
        }

        info.exceptionDescription = GetExceptionCodeDescription(info.exceptionCode);

        g_lastExceptionInfo = info;
    }
    catch (...)
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogEnhancedExceptionInfo - Exception during processing, using basic logging\n");
        LogBasicExceptionInfo(pException);
    }
}

static std::variant<DWORD, std::string> HandleExceptionModern(_EXCEPTION_POINTERS* pException) noexcept
{
    if (pException == nullptr || pException->ExceptionRecord == nullptr)
    {
        return std::string("Invalid exception pointers");
    }

    const EXCEPTION_RECORD* pRecord = pException->ExceptionRecord;
    const DWORD             exceptionCode = pRecord->ExceptionCode;

    LogEnhancedExceptionInfo(pException);

    CExceptionInformation_Impl exceptionInfo;
    exceptionInfo.Set(exceptionCode, pException);

    std::string_view exceptionType = "Unknown";

    switch (exceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            exceptionType = "Access Violation";
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            exceptionType = "Array Bounds Exceeded";
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            exceptionType = "Data Type Misalignment";
            break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            exceptionType = "Floating Point Denormal Operand";
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            exceptionType = "Floating Point Divide by Zero";
            break;
        case EXCEPTION_FLT_INEXACT_RESULT:
            exceptionType = "Floating Point Inexact Result";
            break;
        case EXCEPTION_FLT_INVALID_OPERATION:
            exceptionType = "Floating Point Invalid Operation";
            break;
        case EXCEPTION_FLT_OVERFLOW:
            exceptionType = "Floating Point Overflow";
            break;
        case EXCEPTION_FLT_STACK_CHECK:
            exceptionType = "Floating Point Stack Check";
            break;
        case EXCEPTION_FLT_UNDERFLOW:
            exceptionType = "Floating Point Underflow";
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            exceptionType = "Illegal Instruction";
            break;
        case EXCEPTION_IN_PAGE_ERROR:
            exceptionType = "In Page Error";
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            exceptionType = "Integer Divide by Zero";
            break;
        case EXCEPTION_INT_OVERFLOW:
            exceptionType = "Integer Overflow";
            break;
        case EXCEPTION_INVALID_DISPOSITION:
            exceptionType = "Invalid Disposition";
            break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            exceptionType = "Noncontinuable Exception";
            break;
        case EXCEPTION_PRIV_INSTRUCTION:
            exceptionType = "Privileged Instruction";
            break;
        case EXCEPTION_SINGLE_STEP:
            exceptionType = "Single Step";
            break;
        case EXCEPTION_STACK_OVERFLOW:
            exceptionType = "Stack Overflow";
            break;
        case CPP_EXCEPTION_CODE:
            exceptionType = "C++ Exception";
            break;
        case STATUS_INVALID_CRUNTIME_PARAMETER_CODE:
            exceptionType = "Invalid CRT Parameter";
            break;
        case STATUS_STACK_BUFFER_OVERRUN_CODE:
            exceptionType = "Stack Buffer Overrun";
            break;
    }

    SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "%.*s (0x%08X) at 0x%p\n",
                           static_cast<int>(exceptionType.size()),
                           exceptionType.data(),
                           exceptionCode,
                           pRecord->ExceptionAddress);

    return exceptionCode;
}

static std::mutex                                          g_handlerStateMutex;
static std::atomic<PFNCHFILTFN>                            g_pfnCrashCallback{nullptr};
static std::atomic<LPTOP_LEVEL_EXCEPTION_FILTER>           g_pfnOrigFilt{nullptr};
static std::atomic<std::terminate_handler>                 g_pfnOrigTerminate{nullptr};
static std::atomic<std::new_handler>                       g_pfnOrigNewHandler{nullptr};
static std::atomic<decltype(&SetUnhandledExceptionFilter)> g_pfnKernelSetUnhandledExceptionFilter{nullptr};
static decltype(&SetUnhandledExceptionFilter)              g_kernelSetUnhandledExceptionFilterTrampoline = nullptr;

static std::atomic<bool>                g_bStackCookieCaptureEnabled{false};
static std::atomic<void(__cdecl*)(int)> g_pfnOrigAbortHandler{nullptr};
static std::atomic<_purecall_handler>   g_pfnOrigPureCall{nullptr};
static std::atomic<bool>                g_bInAbortHandler{false};
static std::atomic<bool>                g_bInPureCallHandler{false};
static std::atomic<bool>                g_bInTerminateHandler{false};
static std::atomic<bool>                g_bInNewHandler{false};

LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs);

[[noreturn]] void __cdecl CppTerminateHandler() noexcept;
[[noreturn]] void __cdecl CppNewHandler() noexcept;
void __cdecl              AbortSignalHandler(int signal) noexcept;
[[noreturn]] void __cdecl PureCallHandler() noexcept;

static void InstallCppHandlers() noexcept;
static bool InstallSehHandler() noexcept;
static void InstallAbortHandlers() noexcept;
static void UninstallCrashHandlers() noexcept;
static void ReportCurrentCppException() noexcept;

[[nodiscard]] static bool BuildExceptionContext(EXCEPTION_POINTERS& outExPtrs, EXCEPTION_RECORD*& outExRecord, CONTEXT*& outCtx,
                                                DWORD dwExceptionCode) noexcept;

inline void InitializeExceptionRecord(EXCEPTION_RECORD* const pRecord, const DWORD code, const void* const address) noexcept;

static void LogHandlerEvent(const char* prefix, const char* event) noexcept;

// Single overload using string_view handles all string types efficiently
static void SignalSafeOutput(std::string_view message) noexcept
{
    SafeDebugOutput(message);
}

static void SignalSafePrintPrefixed(std::string_view prefix, std::string_view message) noexcept
{
    if (!prefix.empty())
    {
        SignalSafeOutput(prefix);
    }

    if (!message.empty())
    {
        SignalSafeOutput(message);
    }
}

static void LogHandlerEvent(const char* prefix, const char* event) noexcept
{
    if (prefix != nullptr && event != nullptr)
    {
        SafeDebugPrintPrefixed(prefix, "%s\n", event);
    }
}

[[noreturn]] static void TerminateSelfWithExitCode(DWORD exitCode) noexcept
{
    if (exitCode == 0)
    {
        exitCode = EXIT_CODE_CRASH;
    }

    TerminateProcess(GetCurrentProcess(), exitCode);
    _exit(static_cast<int>(exitCode));
}

inline void InitializeExceptionRecord(EXCEPTION_RECORD* const pRecord, const DWORD code, const void* const address) noexcept
{
    if (pRecord == nullptr)
    {
        return;
    }

    pRecord->ExceptionCode = code;
    pRecord->ExceptionFlags = EXCEPTION_NONCONTINUABLE;
    pRecord->ExceptionRecord = nullptr;
    pRecord->ExceptionAddress = const_cast<void*>(address);
    pRecord->NumberParameters = 0;

    for (auto& param : pRecord->ExceptionInformation)
        param = 0;
}

static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI RedirectedSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) noexcept
{
    std::array szLog{std::array<char, DEBUG_BUFFER_SIZE>{}};
    SAFE_DEBUG_PRINT(szLog, "%.*sIntercepted SetUnhandledExceptionFilter call with arg 0x%p\n", static_cast<int>(DEBUG_PREFIX_CRASH.size()), DEBUG_PREFIX_CRASH.data(),
                     static_cast<const void*>(lpTopLevelExceptionFilter));

    if (lpTopLevelExceptionFilter == nullptr)
    {
        SafeDebugOutput("CrashHandler: Blocked attempt to remove exception filter\n");
        return CrashHandlerExceptionFilter;
    }

    SafeDebugOutput("CrashHandler: Allowing non-nullptr exception filter change\n");
    auto kernelSet = g_pfnKernelSetUnhandledExceptionFilter.load(std::memory_order_acquire);
    if (kernelSet == nullptr)
    {
        SafeDebugOutput("CrashHandler: WARNING - Original SetUnhandledExceptionFilter pointer missing\n");
        return lpTopLevelExceptionFilter;
    }

    return kernelSet(lpTopLevelExceptionFilter);
}

void __cdecl AbortSignalHandler([[maybe_unused]] int signal) noexcept
{
    bool expected = false;
    if (!g_bInAbortHandler.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        SignalSafePrintPrefixed(DEBUG_PREFIX_ABORT, "Recursive\n");
        TerminateSelfWithExitCode(STATUS_STACK_BUFFER_OVERRUN_CODE);
    }

    SignalSafeOutput(DEBUG_SEPARATOR);
    SignalSafePrintPrefixed(DEBUG_PREFIX_ABORT, "SIGABRT (STATUS_STACK_BUFFER_OVERRUN)\n");
    SignalSafeOutput(DEBUG_SEPARATOR);

    alignas(16) EXCEPTION_RECORD exRecord{};
    alignas(16) CONTEXT          ctx{};

    void* returnAddr = _ReturnAddress();

    InitializeExceptionRecord(&exRecord, STATUS_STACK_BUFFER_OVERRUN_CODE, returnAddr);

    ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
    RtlCaptureContext(&ctx);

    EXCEPTION_POINTERS exPtrs = {};
    exPtrs.ExceptionRecord = &exRecord;
    exPtrs.ContextRecord = &ctx;

    PFNCHFILTFN callback = g_pfnCrashCallback.load(std::memory_order_acquire);

    if (callback != nullptr)
    {
        SignalSafePrintPrefixed(DEBUG_PREFIX_ABORT, "Calling handler\n");

        try
        {
            callback(&exPtrs);
        }
        catch (...)
        {
            SignalSafePrintPrefixed(DEBUG_PREFIX_ABORT, "Handler exception\n");
        }
    }
    else
    {
        SignalSafePrintPrefixed(DEBUG_PREFIX_ABORT, "No handler\n");
    }

    SignalSafePrintPrefixed(DEBUG_PREFIX_ABORT, "Terminating\n");

    TerminateSelfWithExitCode(STATUS_STACK_BUFFER_OVERRUN_CODE);
}

[[noreturn]] void __cdecl PureCallHandler() noexcept
{
    bool expected = false;
    if (!g_bInPureCallHandler.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        SignalSafePrintPrefixed(DEBUG_PREFIX_PURECALL, "Recursive\n");
        TerminateSelfWithExitCode(EXCEPTION_NONCONTINUABLE_EXCEPTION);
    }

    SafeDebugOutput(DEBUG_SEPARATOR);
    LogHandlerEvent(DEBUG_PREFIX_PURECALL.data(), "Pure virtual function call detected");
    SafeDebugOutput(DEBUG_SEPARATOR);

    EXCEPTION_RECORD*  pExRecord = nullptr;
    CONTEXT*           pCtx = nullptr;
    EXCEPTION_POINTERS exPtrs{};

    PFNCHFILTFN callback = g_pfnCrashCallback.load(std::memory_order_acquire);

    if (callback != nullptr && BuildExceptionContext(exPtrs, pExRecord, pCtx, EXCEPTION_NONCONTINUABLE_EXCEPTION))
    {
        LogHandlerEvent(DEBUG_PREFIX_PURECALL.data(), "Calling crash handler callback");

        try
        {
            callback(&exPtrs);
        }
        catch (...)
        {
            LogHandlerEvent(DEBUG_PREFIX_PURECALL.data(), "Exception in crash handler callback");
        }
    }

    if (pCtx != nullptr)
    {
        LocalFree(pCtx);
    }

    if (pExRecord != nullptr)
    {
        LocalFree(pExRecord);
    }

    LogHandlerEvent(DEBUG_PREFIX_PURECALL.data(), "Terminating process");
    TerminateSelfWithExitCode(EXCEPTION_NONCONTINUABLE_EXCEPTION);
}

#pragma warning(disable : 4073)
#pragma init_seg(lib)
class CleanUpCrashHandler
{
public:
    CleanUpCrashHandler() = default;
    ~CleanUpCrashHandler() noexcept
    {
        std::scoped_lock lock{g_handlerStateMutex};

        g_bStackCookieCaptureEnabled.store(false, std::memory_order_seq_cst);
        g_pfnCrashCallback.store(nullptr, std::memory_order_seq_cst);

        if (auto terminateHandler = g_pfnOrigTerminate.exchange(nullptr, std::memory_order_seq_cst); terminateHandler != nullptr)
        {
            std::set_terminate(terminateHandler);
        }

        if (auto newHandler = g_pfnOrigNewHandler.exchange(nullptr, std::memory_order_seq_cst); newHandler != nullptr)
        {
            std::set_new_handler(newHandler);
        }

        if (auto abortHandler = g_pfnOrigAbortHandler.exchange(nullptr, std::memory_order_seq_cst); abortHandler != nullptr)
        {
            signal(SIGABRT, abortHandler);
        }

        if (auto pureCallHandler = g_pfnOrigPureCall.exchange(nullptr, std::memory_order_seq_cst); pureCallHandler != nullptr)
        {
            _set_purecall_handler(pureCallHandler);
        }

        if (auto previousFilter = g_pfnOrigFilt.exchange(nullptr, std::memory_order_seq_cst); previousFilter != nullptr)
        {
            SetUnhandledExceptionFilter(previousFilter);
        }

        if (auto kernelSetUnhandled = g_pfnKernelSetUnhandledExceptionFilter.exchange(nullptr, std::memory_order_seq_cst); kernelSetUnhandled != nullptr)
        {
            g_kernelSetUnhandledExceptionFilterTrampoline = kernelSetUnhandled;
            if (!SharedUtil::UndoFunctionDetour(g_kernelSetUnhandledExceptionFilterTrampoline, reinterpret_cast<void*>(RedirectedSetUnhandledExceptionFilter)))
            {
                SafeDebugOutput("CrashHandler: WARNING - Failed to undo SEH detour during cleanup\n");
            }
            g_kernelSetUnhandledExceptionFilterTrampoline = nullptr;
        }
    }
};

static CleanUpCrashHandler g_cBeforeAndAfter;

[[nodiscard]] BOOL __stdcall SetCrashHandlerFilter(PFNCHFILTFN pFn) noexcept
{
    if (pFn == nullptr)
    {
        UninstallCrashHandlers();
        return TRUE;
    }

    g_pfnCrashCallback.store(pFn, std::memory_order_seq_cst);

    std::array<char, DEBUG_BUFFER_SIZE> debugBuffer{};
    SAFE_DEBUG_PRINT(debugBuffer, "%.*sInstalling handlers with callback at 0x%p\n", static_cast<int>(DEBUG_PREFIX_CRASH.size()), DEBUG_PREFIX_CRASH.data(), reinterpret_cast<const void*>(pFn));

    if (const BOOL phaseResult = SetInitializationPhase(INIT_PHASE_MINIMAL); phaseResult == FALSE)
    {
        SafeDebugOutput("CrashHandler: WARNING - Failed to set initialization phase to minimal\n");
    }

    SafeDebugOutput("CrashHandler: Phase 0 - Minimal setup\n");
    SafeDebugOutput("CrashHandler: Vectored exception handling disabled (SEH-only mode)\n");

    if (g_bStackCookieCaptureEnabled.load(std::memory_order_acquire))
    {
        InstallAbortHandlers();
        SafeDebugOutput("CrashHandler: Abort and purecall handlers armed for stack cookie capture\n");
    }

    InstallCppHandlers();

    SafeDebugOutput("CrashHandler: SEH handler deferred until post-initialization phase\n");
    SafeDebugOutput("CrashHandler: Phase 0 setup completed successfully\n");
    SafeDebugOutput("===========================================\n");

    return TRUE;
}

[[nodiscard]] BOOL __stdcall EnableStackCookieFailureCapture(BOOL bEnable) noexcept
{
    const bool bEnableBool = (bEnable != FALSE);
    g_bStackCookieCaptureEnabled.store(bEnableBool, std::memory_order_release);

    std::array<char, DEBUG_BUFFER_SIZE> szDebug{};
    SAFE_DEBUG_PRINT(szDebug, "%.*sStack cookie failure capture %s\n", static_cast<int>(DEBUG_PREFIX_CRASH.size()), DEBUG_PREFIX_CRASH.data(), bEnableBool ? "ENABLED" : "DISABLED");

    if (bEnableBool)
    {
        SafeDebugOutput("CrashHandler: Stack cookie capture enabled - call SetCrashHandlerFilter to activate\n");
        if (g_pfnCrashCallback.load(std::memory_order_acquire) != nullptr)
        {
            InstallAbortHandlers();
        }
    }
    else
    {
        std::scoped_lock lock{g_handlerStateMutex};

        if (auto abortHandler = g_pfnOrigAbortHandler.exchange(nullptr, std::memory_order_acq_rel); abortHandler != nullptr)
        {
            if (void(__cdecl * previousHandler)(int) = signal(SIGABRT, abortHandler); previousHandler == SIG_ERR)
            {
                const int                    errnoValue = errno;
                constexpr auto               bufferSize = DEBUG_BUFFER_SIZE;
                std::array<char, bufferSize> szDebug{};
                SAFE_DEBUG_PRINT_C(szDebug.data(), bufferSize,
                                   "CrashHandler: WARNING - Failed to restore SIGABRT handler, errno: %d\n", errnoValue);
            }
            else
            {
                SafeDebugOutput("CrashHandler: SIGABRT handler restored\n");
            }
        }

        if (auto pureCallHandler = g_pfnOrigPureCall.exchange(nullptr, std::memory_order_acq_rel); pureCallHandler != nullptr)
        {
            if (_purecall_handler previous = _set_purecall_handler(pureCallHandler); previous == nullptr)
            {
                SafeDebugOutput("CrashHandler: WARNING - Failed to restore pure call handler\n");
            }
            else
            {
                SafeDebugOutput("CrashHandler: Pure call handler restored\n");
            }
        }

        g_bInAbortHandler.store(false, std::memory_order_release);
    }

    return TRUE;
}

static void InstallCppHandlers() noexcept
{
    std::scoped_lock lock{g_handlerStateMutex};

    if (g_pfnOrigTerminate.load(std::memory_order_acquire) == nullptr)
    {
        std::terminate_handler previous = std::set_terminate(CppTerminateHandler);
        g_pfnOrigTerminate.store(previous, std::memory_order_release);
        SafeDebugOutput("CrashHandler: C++ terminate handler installed\n");
    }

    if (g_pfnOrigNewHandler.load(std::memory_order_acquire) == nullptr)
    {
        std::new_handler previous = std::set_new_handler(CppNewHandler);
        g_pfnOrigNewHandler.store(previous, std::memory_order_release);
        SafeDebugOutput("CrashHandler: C++ new handler installed\n");
    }
}

static void InstallAbortHandlers() noexcept
{
    std::scoped_lock lock{g_handlerStateMutex};

    if (!g_bStackCookieCaptureEnabled.load(std::memory_order_acquire))
    {
        SafeDebugOutput("CrashHandler: Stack cookie capture disabled, skipping abort handlers\n");
        return;
    }

    if (g_pfnCrashCallback.load(std::memory_order_acquire) == nullptr)
    {
        SafeDebugOutput("CrashHandler: WARNING - No callback set, skipping abort handlers\n");
        return;
    }

    if (g_pfnOrigAbortHandler.load(std::memory_order_acquire) == nullptr)
    {
        if (void(__cdecl * prevHandler)(int) = signal(SIGABRT, AbortSignalHandler); prevHandler != SIG_ERR)
        {
            g_pfnOrigAbortHandler.store(prevHandler, std::memory_order_release);
            SafeDebugOutput("CrashHandler: SIGABRT handler installed\n");
        }
        else
        {
            const int                    errnoValue = errno;
            constexpr auto               bufferSize = DEBUG_BUFFER_SIZE;
            std::array<char, bufferSize> szDebug{};
            SAFE_DEBUG_PRINT_C(szDebug.data(), bufferSize, "CrashHandler: WARNING - Failed to install SIGABRT handler, errno: %d\n", errnoValue);
        }
    }

    if (g_pfnOrigPureCall.load(std::memory_order_acquire) == nullptr)
    {
        _purecall_handler previous = _set_purecall_handler(PureCallHandler);
        g_pfnOrigPureCall.store(previous, std::memory_order_release);
        SafeDebugOutput("CrashHandler: Pure virtual call handler installed\n");
    }
}

[[nodiscard]] static bool TryReadEnvBool(const char* name, bool& outValue) noexcept
{
    std::array<char, 8> buffer{};
    DWORD               length = GetEnvironmentVariableA(name, buffer.data(), static_cast<DWORD>(buffer.size()));

    if (length == 0 || length >= buffer.size())
        return false;

    const std::uint8_t rawChar = static_cast<std::uint8_t>(buffer[0]);
    const char         indicator = static_cast<char>(std::tolower(rawChar));
    outValue = !(indicator == '0' || indicator == 'f' || indicator == 'n');
    return true;
}

[[nodiscard]] BOOL __stdcall GetCrashHandlerConfiguration(PCRASH_HANDLER_CONFIG pConfig) noexcept
{
    if (pConfig == nullptr)
        return FALSE;

    memset(pConfig, 0, sizeof(CRASH_HANDLER_CONFIG));

    bool disableSehDetourBool = false;
    bool forceSehDetourBool = false;
    const bool hasDisableSehDetourEnv = TryReadEnvBool("MTA_DISABLE_SEH_DETOUR", disableSehDetourBool);
    const bool hasForceSehDetourEnv = TryReadEnvBool("MTA_FORCE_SEH_DETOUR", forceSehDetourBool);

    if (!hasDisableSehDetourEnv)
    {
        disableSehDetourBool = false;
    }

    if (!hasForceSehDetourEnv)
    {
        forceSehDetourBool = false;
    }
    pConfig->disableSehDetour = disableSehDetourBool ? TRUE : FALSE;
    pConfig->forceSehDetour = forceSehDetourBool ? TRUE : FALSE;

    constexpr auto initDefaults = []() constexpr noexcept { return true; };
    static_assert(initDefaults(), "Configuration defaults validation");
    if (initDefaults())
    {
        pConfig->enableStackCookieCapture = TRUE;
        pConfig->enableEnhancedLogging = TRUE;
        pConfig->enableSehHandler = TRUE;
        pConfig->enableCppHandlers = TRUE;
        pConfig->memoryReservationKB = 512;
        pConfig->maxStackTraceFrames = 100;
        pConfig->debugBufferSize = DEBUG_BUFFER_SIZE;
    }

    return TRUE;
}

[[nodiscard]] BOOL __stdcall CaptureUnifiedStackTrace(_EXCEPTION_POINTERS* pException, DWORD maxFrames, std::vector<std::string>* pOutTrace) noexcept
{
    if (pException == nullptr || pException->ContextRecord == nullptr)
        return FALSE;

    if (pOutTrace == nullptr)
        return FALSE;

    pOutTrace->clear();

    constexpr DWORD defaultMaxFrames = 32;
    DWORD           configMaxFrames = defaultMaxFrames;
    if (CRASH_HANDLER_CONFIG config{}; GetCrashHandlerConfiguration(&config))
    {
        configMaxFrames = config.maxStackTraceFrames;
    }

    constexpr DWORD minFrames = 1;
    constexpr DWORD maxAllowedFrames = 256;
    const DWORD     requestedFrames = maxFrames != 0 ? maxFrames : configMaxFrames;
    const DWORD     actualMaxFrames = std::clamp(requestedFrames, minFrames, maxAllowedFrames);
    try
    {
        pOutTrace->reserve(actualMaxFrames);
    }
    catch (...)
    {
    }

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    CONTEXT      context = *pException->ContextRecord;
    STACKFRAME64 frame{};

    // For EIP=0 crashes, start stack walk from return address at [ESP]
    constexpr auto kNullAddress = uintptr_t{0};
    static_assert(kNullAddress == 0, "Null address constant must be zero");
    
    const auto isNullJump = (context.Eip == kNullAddress);
    
    if (isNullJump)
    {
        // Try to read return address from [ESP]
        const auto espAddr = static_cast<uintptr_t>(context.Esp);
        const auto pReturnAddress = reinterpret_cast<void* const*>(espAddr);

        if (SharedUtil::IsReadablePointer(pReturnAddress, sizeof(void*)))
        {
            const auto returnAddr = reinterpret_cast<uintptr_t>(*pReturnAddress);

            SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH,
                                   "CaptureUnifiedStackTrace - EIP=0, starting stack walk from return address: 0x%08X\n",
                                   static_cast<unsigned int>(returnAddr));

            // Start stack walk from return address instead of null EIP
            frame.AddrPC.Offset = returnAddr;
            frame.AddrFrame.Offset = context.Ebp;
            frame.AddrStack.Offset = context.Esp;
        }
        else
        {
            SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH,
                                   "CaptureUnifiedStackTrace - EIP=0, ESP not readable (0x%08X), cannot capture stack\n",
                                   static_cast<unsigned int>(espAddr));
            return FALSE;
        }
    }
    else
    {
        frame.AddrPC.Offset = context.Eip;
        frame.AddrFrame.Offset = context.Ebp;
        frame.AddrStack.Offset = context.Esp;
    }

    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;

    struct SymbolHandlerGuard
    {
        HANDLE process;
        bool   initialized;

        SymbolHandlerGuard(HANDLE hProcess) : process(hProcess), initialized(false)
        {
            if (process != nullptr && SymInitialize(process, nullptr, TRUE) != FALSE)
            {
                initialized = true;
                constexpr auto symOptions = SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_FAIL_CRITICAL_ERRORS;
                SymSetOptions(symOptions);
            }
        }

        ~SymbolHandlerGuard()
        {
            if (initialized)
            {
                SymCleanup(process);
            }
        }

        SymbolHandlerGuard(const SymbolHandlerGuard&) = delete;
        SymbolHandlerGuard& operator=(const SymbolHandlerGuard&) = delete;
    };

    SymbolHandlerGuard symbolGuard(hProcess);
    if (!symbolGuard.initialized)
        return FALSE;

    constexpr auto                    kMaxSymbolNameLength = 256;
    constexpr auto                    symbolBufferSize = sizeof(SYMBOL_INFO) + kMaxSymbolNameLength;
    alignas(SYMBOL_INFO) std::uint8_t symbolBuffer[symbolBufferSize]{};
    PSYMBOL_INFO                      pSymbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = kMaxSymbolNameLength;

    for (DWORD frameIndex = 0; frameIndex < actualMaxFrames; ++frameIndex)
    {
        constexpr auto machineType = IMAGE_FILE_MACHINE_I386;
        if (const BOOL bWalked = StackWalk64(machineType, hProcess, hThread, &frame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr);
            bWalked == FALSE)
            break;

        if (frame.AddrPC.Offset == 0)
            break;

        const DWORD64 address = frame.AddrPC.Offset;

        auto symbolName = "0x"s + std::to_string(address);
        if (const BOOL symResult = SymFromAddr(hProcess, address, nullptr, pSymbol); symResult != FALSE && pSymbol->Name != nullptr)
        {
            symbolName = pSymbol->Name;
        }

        IMAGEHLP_LINE64 lineInfo{};
        lineInfo.SizeOfStruct = sizeof(lineInfo);
        DWORD lineDisplacement = 0;

        std::string frameInfo = symbolName;
        if (const BOOL lineResult = SymGetLineFromAddr64(hProcess, address, &lineDisplacement, &lineInfo); lineResult != FALSE)
        {
            if (lineInfo.FileName != nullptr)
            {
                frameInfo += " ("s + lineInfo.FileName + ":"s + std::to_string(lineInfo.LineNumber) + ")"s;
            }
        }

        frameInfo += " [0x"s + std::to_string(address) + "]"s;
        pOutTrace->push_back(frameInfo);
    }

    return !pOutTrace->empty();
}

static bool InstallSehHandler() noexcept
{
    std::scoped_lock lock{g_handlerStateMutex};

    if (g_pfnOrigFilt.load(std::memory_order_acquire) != nullptr)
    {
        SafeDebugOutput("CrashHandler: SEH already installed\n");
        return true;
    }

    LPTOP_LEVEL_EXCEPTION_FILTER previousFilter = SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
    g_pfnOrigFilt.store(previousFilter, std::memory_order_release);

    bool success = true;

    std::array<char, DEBUG_BUFFER_SIZE> szDebug{};
    if (previousFilter != nullptr)
    {
        SAFE_DEBUG_PRINT(szDebug, "%.*sSEH installed, previous filter at 0x%p\n", static_cast<int>(DEBUG_PREFIX_CRASH.size()), DEBUG_PREFIX_CRASH.data(), static_cast<const void*>(previousFilter));
    }
    else
    {
        SafeDebugOutput("CrashHandler: SEH installed - no previous filter present\n");
    }

    bool detourEnabled = true;
    bool detourForced = false;

    if (CRASH_HANDLER_CONFIG config{}; GetCrashHandlerConfiguration(&config))
    {
        const bool disableDetour = config.disableSehDetour != FALSE;
        const bool forceDetour = config.forceSehDetour != FALSE;

        SAFE_DEBUG_PRINT(szDebug, "CrashHandler: SEH detour env check - MTA_DISABLE_SEH_DETOUR: %s, MTA_FORCE_SEH_DETOUR: %s\n",
                         disableDetour ? "true" : "false", forceDetour ? "true" : "false");

        detourForced = forceDetour;

        if (disableDetour && !detourForced)
        {
            detourEnabled = false;
        }
    }

    const bool debuggerPresent = IsDebuggerPresent() != FALSE;
    SAFE_DEBUG_PRINT(szDebug, "CrashHandler: Debugger present: %s\n", debuggerPresent ? "true" : "false");

    if (!detourEnabled)
    {
        SafeDebugOutput("CrashHandler: SEH detour protection disabled by configuration\n");
        return success;
    }

    if (detourForced)
    {
        SafeDebugOutput("CrashHandler: SEH detour protection forced by configuration\n");
    }
    else if (debuggerPresent)
    {
        SafeDebugOutput("CrashHandler: SEH detour protection enabled (debugger present)\n");
    }
    else
    {
        SafeDebugOutput("CrashHandler: SEH detour protection enabled for production runtime\n");
    }

    if (g_pfnKernelSetUnhandledExceptionFilter.load(std::memory_order_acquire) != nullptr)
    {
        SafeDebugOutput("CrashHandler: SEH detour protection already installed\n");
        return success;
    }

    if (HMODULE kernelModule = GetModuleHandleW(L"KERNEL32.DLL"); kernelModule == nullptr)
    {
        SafeDebugOutput("CrashHandler: WARNING - Failed to resolve KERNEL32.DLL handle\n");
        return success;
    }
    else
    {
        decltype(&SetUnhandledExceptionFilter) setUnhandled = nullptr;
        if (SharedUtil::TryGetProcAddress(kernelModule, "SetUnhandledExceptionFilter", setUnhandled))
        {
            g_kernelSetUnhandledExceptionFilterTrampoline = setUnhandled;

            if (SharedUtil::DetourFunction(g_kernelSetUnhandledExceptionFilterTrampoline, reinterpret_cast<void*>(RedirectedSetUnhandledExceptionFilter)))
            {
                g_pfnKernelSetUnhandledExceptionFilter.store(g_kernelSetUnhandledExceptionFilterTrampoline, std::memory_order_release);
                SafeDebugOutput("CrashHandler: SEH detour protection installed\n");
            }
            else
            {
                SafeDebugOutput("CrashHandler: WARNING - SEH detour protection FAILED (non-critical)\n");
                g_pfnKernelSetUnhandledExceptionFilter.store(nullptr, std::memory_order_release);
                g_kernelSetUnhandledExceptionFilterTrampoline = nullptr;
                success = false;
            }
        }
        else
        {
            SafeDebugOutput("CrashHandler: WARNING - Failed to resolve SetUnhandledExceptionFilter address\n");
            success = false;
        }
    }

    return success;
}

static void UninstallCrashHandlers() noexcept
{
    std::scoped_lock lock{g_handlerStateMutex};

    SafeDebugOutput("CrashHandler: Unhooking crash handlers\n");

    g_pfnCrashCallback.store(nullptr, std::memory_order_release);
    g_bStackCookieCaptureEnabled.store(false, std::memory_order_release);

    if (auto terminateHandler = g_pfnOrigTerminate.exchange(nullptr, std::memory_order_acq_rel); terminateHandler != nullptr)
    {
        std::set_terminate(terminateHandler);
    }

    if (auto newHandler = g_pfnOrigNewHandler.exchange(nullptr, std::memory_order_acq_rel); newHandler != nullptr)
    {
        std::set_new_handler(newHandler);
    }

    if (auto abortHandler = g_pfnOrigAbortHandler.exchange(nullptr, std::memory_order_acq_rel); abortHandler != nullptr)
    {
        signal(SIGABRT, reinterpret_cast<void(__cdecl*)(int)>(abortHandler));
    }

    if (auto pureCallHandler = g_pfnOrigPureCall.exchange(nullptr, std::memory_order_acq_rel); pureCallHandler != nullptr)
    {
        _set_purecall_handler(pureCallHandler);
    }

    if (auto previousFilt = g_pfnOrigFilt.exchange(nullptr, std::memory_order_acq_rel); previousFilt != nullptr)
    {
        SetUnhandledExceptionFilter(previousFilt);
    }

    if (auto kernelSetUnhandled = g_pfnKernelSetUnhandledExceptionFilter.exchange(nullptr, std::memory_order_acq_rel); kernelSetUnhandled != nullptr)
    {
        g_kernelSetUnhandledExceptionFilterTrampoline = kernelSetUnhandled;
        if (!SharedUtil::UndoFunctionDetour(g_kernelSetUnhandledExceptionFilterTrampoline, reinterpret_cast<void*>(RedirectedSetUnhandledExceptionFilter)))
        {
            SafeDebugOutput("CrashHandler: WARNING - Failed to undo SEH detour\n");
        }
        g_kernelSetUnhandledExceptionFilterTrampoline = nullptr;
    }
}

static bool BuildExceptionContext(EXCEPTION_POINTERS& outExPtrs, EXCEPTION_RECORD*& outExRecord, CONTEXT*& outCtx, DWORD dwExceptionCode) noexcept
{
    struct LocalAllocGuard
    {
        void* ptr;
        explicit LocalAllocGuard(std::size_t size) : ptr(LocalAlloc(LPTR, size)) {}
        ~LocalAllocGuard()
        {
            if (ptr)
                LocalFree(ptr);
        }
        LocalAllocGuard(const LocalAllocGuard&) = delete;
        LocalAllocGuard& operator=(const LocalAllocGuard&) = delete;
    };

    LocalAllocGuard exRecordGuard(sizeof(EXCEPTION_RECORD));
    LocalAllocGuard ctxGuard(sizeof(CONTEXT));

    if (exRecordGuard.ptr == nullptr || ctxGuard.ptr == nullptr)
    {
        return false;
    }

    outExRecord = static_cast<EXCEPTION_RECORD*>(exRecordGuard.ptr);
    outCtx = static_cast<CONTEXT*>(ctxGuard.ptr);

    exRecordGuard.ptr = nullptr;
    ctxGuard.ptr = nullptr;

    void* returnAddr = _ReturnAddress();
    InitializeExceptionRecord(outExRecord, dwExceptionCode, returnAddr);

    outCtx->ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;

    try
    {
        RtlCaptureContext(outCtx);
    }
    catch (...)
    {
        SafeDebugOutput("BuildExceptionContext: RtlCaptureContext failed\n");

        LocalFree(outExRecord);
        LocalFree(outCtx);
        outExRecord = nullptr;
        outCtx = nullptr;

        return false;
    }

    outExPtrs.ExceptionRecord = outExRecord;
    outExPtrs.ContextRecord = outCtx;

    return true;
}

[[noreturn]] void __cdecl CppTerminateHandler() noexcept
{
    bool expected = false;
    if (!g_bInTerminateHandler.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        SignalSafePrintPrefixed(DEBUG_PREFIX_CPP, "Recursive\n");
        TerminateSelfWithExitCode(CPP_EXCEPTION_CODE);
    }

    SafeDebugOutput(DEBUG_SEPARATOR);
    SafeDebugPrintPrefixed(DEBUG_PREFIX_CPP, "Unhandled C++ exception detected\n");

    ReportCurrentCppException();

    SafeDebugOutput(DEBUG_SEPARATOR);

    EXCEPTION_RECORD*  pExRecord = nullptr;
    CONTEXT*           pCtx = nullptr;
    EXCEPTION_POINTERS exPtrs{};

    PFNCHFILTFN callback = g_pfnCrashCallback.load(std::memory_order_acquire);

    if (callback != nullptr && BuildExceptionContext(exPtrs, pExRecord, pCtx, CPP_EXCEPTION_CODE))
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CPP, "Calling crash handler callback\n");

        try
        {
            callback(&exPtrs);
        }
        catch (...)
        {
            SafeDebugPrintPrefixed(DEBUG_PREFIX_CPP, "Exception in crash handler callback\n");
        }
    }

    if (pCtx != nullptr)
    {
        LocalFree(pCtx);
    }

    if (pExRecord != nullptr)
    {
        LocalFree(pExRecord);
    }

    SafeDebugPrintPrefixed(DEBUG_PREFIX_CPP, "Terminating process\n");
    TerminateSelfWithExitCode(CPP_EXCEPTION_CODE);
}

static void ReportCurrentCppException() noexcept
{
    try
    {
        std::exception_ptr eptr = std::current_exception();
        if (!eptr)
        {
            return;
        }

        std::rethrow_exception(eptr);
    }
    catch (const std::bad_array_new_length&)
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CPP, "std::bad_array_new_length caught\n");
    }
    catch (const std::bad_alloc&)
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CPP, "std::bad_alloc caught - OUT OF MEMORY!\n");
    }
    catch (const std::exception& e)
    {
        std::array<char, DEBUG_BUFFER_SIZE_LARGE> szDebug{};
        SAFE_DEBUG_PRINT(szDebug, "%.*sstd::exception caught: %s\n", static_cast<int>(DEBUG_PREFIX_CPP.size()), DEBUG_PREFIX_CPP.data(), e.what());
    }
    catch (...)
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CPP, "Unknown exception type\n");
    }
}

[[noreturn]] void __cdecl CppNewHandler() noexcept
{
    bool expected = false;
    if (!g_bInNewHandler.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        SignalSafeOutput("C++ NEW HANDLER: Recursive\n");
        TerminateSelfWithExitCode(STATUS_NO_MEMORY);
    }

    SafeDebugOutput(DEBUG_SEPARATOR);
    SafeDebugOutput("C++ NEW HANDLER: Memory allocation failed\n");
    SafeDebugOutput(DEBUG_SEPARATOR);

    std::terminate();
}

[[nodiscard]] BOOL __stdcall GetEnhancedExceptionInfo(PENHANCED_EXCEPTION_INFO pExceptionInfo) noexcept
{
    if (pExceptionInfo == nullptr)
        return FALSE;

    std::scoped_lock lock{g_exceptionInfoMutex};

    if (!g_lastExceptionInfo)
        return FALSE;

    *pExceptionInfo = *g_lastExceptionInfo;
    return TRUE;
}

[[nodiscard]] BOOL __stdcall CaptureCurrentException() noexcept
{
    try
    {
        std::scoped_lock lock{g_exceptionInfoMutex};

        ENHANCED_EXCEPTION_INFO info = {};
        info.capturedException = std::current_exception();
        info.uncaughtExceptionCount = std::uncaught_exceptions();
        info.timestamp = std::chrono::system_clock::now();
        info.threadId = GetCurrentThreadId();
        info.processId = GetCurrentProcessId();

        g_lastExceptionInfo = info;
        return TRUE;
    }
    catch (...)
    {
        return FALSE;
    }
}

[[nodiscard]] BOOL __stdcall LogExceptionDetails(EXCEPTION_POINTERS* pException) noexcept
{
    SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "========================================\n");
    SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - ENTRY\n");
    SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "========================================\n");

    if (pException == nullptr)
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - pException is NULL\n");
        return FALSE;
    }

    SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - pException OK\n");
    
    if (pException->ExceptionRecord != nullptr)
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - ExceptionRecord OK\n");
    else
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - ExceptionRecord is NULL\n");
    
    if (pException->ContextRecord != nullptr)
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - ContextRecord OK\n");
    else
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - ContextRecord is NULL (THIS WILL PREVENT ENHANCED INFO)\n");

    LogBasicExceptionInfo(pException);
    StoreBasicExceptionInfo(pException);

    try
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - Calling LogEnhancedExceptionInfo...\n");
        LogEnhancedExceptionInfo(pException);
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - LogEnhancedExceptionInfo completed\n");
        return TRUE;
    }
    catch (...)
    {
        SafeDebugPrintPrefixed(DEBUG_PREFIX_CRASH, "LogExceptionDetails - Exception caught in LogEnhancedExceptionInfo\n");
        return TRUE;
    }
}

LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
{
    SafeDebugOutput("SEH: CrashHandlerExceptionFilter - ENTRY\n");

    if (pExPtrs == nullptr || pExPtrs->ExceptionRecord == nullptr)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    const DWORD dwExceptionCode = pExPtrs->ExceptionRecord->ExceptionCode;

    if (IsFatalException(dwExceptionCode) == FALSE)
    {
        std::array<char, DEBUG_BUFFER_SIZE> szDebug{};
        SAFE_DEBUG_PRINT_C(szDebug.data(), szDebug.size(), "SEH: Non-fatal exception 0x%08X - ignoring\n", dwExceptionCode);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    LONG lRet = EXCEPTION_CONTINUE_SEARCH;

    static std::atomic<bool> bHandlerActive{false};
    bool                     expected = false;

    if (!bHandlerActive.compare_exchange_strong(expected, true, std::memory_order_acq_rel, std::memory_order_relaxed))
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    SafeDebugOutput(DEBUG_SEPARATOR);
    SafeDebugOutput("SEH: FATAL - Unhandled exception filter triggered\n");

    if (const BOOL logResult = LogExceptionDetails(pExPtrs); logResult == FALSE)
    {
        SafeDebugOutput("SEH: WARNING - Failed to log exception details\n");
    }

    if (STATUS_INVALID_CRUNTIME_PARAMETER_CODE == dwExceptionCode || STATUS_STACK_BUFFER_OVERRUN_CODE == dwExceptionCode)
    {
        SafeDebugOutput("SEH: FATAL - STACK BUFFER OVERRUN (STATUS_STACK_BUFFER_OVERRUN)\n");
    }
    else if (EXCEPTION_STACK_OVERFLOW == dwExceptionCode)
    {
        SafeDebugOutput("SEH: FATAL - EXCEPTION_STACK_OVERFLOW occurred\n");
        if (_resetstkoflw() == 0)
        {
            SafeDebugOutput("SEH: WARNING - _resetstkoflw failed to reset the stack overflow state\n");
        }
    }
    else
    {
        std::array<char, DEBUG_BUFFER_SIZE> szDebug{};
        SAFE_DEBUG_PRINT_C(szDebug.data(), szDebug.size(), "%.*sFATAL - Exception 0x%08X at 0x%p\n", static_cast<int>(DEBUG_PREFIX_SEH.size()), DEBUG_PREFIX_SEH.data(), dwExceptionCode,
                           static_cast<const void*>(pExPtrs->ExceptionRecord->ExceptionAddress));
    }

    SafeDebugOutput(DEBUG_SEPARATOR);

    try
    {
        if (PFNCHFILTFN callback = g_pfnCrashCallback.load(std::memory_order_acquire); callback != nullptr)
        {
            lRet = callback(pExPtrs);
        }
    }
    catch (...)
    {
        SafeDebugOutput("SEH: CRITICAL - Exception in crash handler callback!\n");
        lRet = EXCEPTION_CONTINUE_SEARCH;
    }

    bHandlerActive.store(false, std::memory_order_release);

    return lRet;
}

[[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableSehExceptionHandler() noexcept
{
    return InstallSehHandler() ? TRUE : FALSE;
}

[[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall SetInitializationPhase(DWORD phase) noexcept
{
    constexpr DWORD maxPhase = INIT_PHASE_POST_D3D;
    if (phase > maxPhase)
    {
        SafeDebugOutput("CrashHandler: Invalid initialization phase\n");
        return FALSE;
    }

    DWORD oldPhase = g_initializationPhase.exchange(phase, std::memory_order_seq_cst);

    std::array<char, DEBUG_BUFFER_SIZE> debugBuffer{};
    SAFE_DEBUG_PRINT(debugBuffer, "CrashHandler: Initialization phase changed from %u to %u\n", oldPhase, phase);

    return TRUE;
}

[[nodiscard]] DWORD BUGSUTIL_DLLINTERFACE __stdcall GetInitializationPhase() noexcept
{
    return g_initializationPhase.load(std::memory_order_acquire);
}

[[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableAllHandlersAfterInitialization() noexcept
{
    if (const BOOL phaseResult = SetInitializationPhase(INIT_PHASE_POST_D3D); phaseResult == FALSE)
    {
        SafeDebugOutput("CrashHandler: WARNING - Failed to set initialization phase to post D3D\n");
    }

    bool allHandlersEnabled = true;
    SafeDebugOutput("CrashHandler: Vectored exception handling not available - proceeding with SEH-only pipeline\n");

    if (const BOOL sehResult = EnableSehExceptionHandler(); sehResult == FALSE)
    {
        SafeDebugOutput("CrashHandler: WARNING - Failed to enable SEH exception handler\n");
        allHandlersEnabled = false;
    }
    else
    {
        SafeDebugOutput("CrashHandler: SEH exception handler enabled successfully\n");
    }

    InstallCppHandlers();
    SafeDebugOutput("CrashHandler: C++ handlers installed after D3D initialization\n");

    if (g_bStackCookieCaptureEnabled.load(std::memory_order_seq_cst))
    {
        InstallAbortHandlers();
        SafeDebugOutput("CrashHandler: Abort handlers installed after D3D initialization\n");
    }

    SafeDebugOutput("CrashHandler: Phase 2 - All handlers enabled after D3D initialization\n");

    return allHandlersEnabled ? TRUE : FALSE;
}

//
// Watchdog Thread Implementation
// Detects MTA freezes and triggers crash dumps with stack traces to find what froze
//

namespace
{
#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_destructive_interference_size;
#else
    inline constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

    [[nodiscard]] constexpr bool IsHandleValid(HANDLE handle) noexcept
    {
        return handle != nullptr && handle != INVALID_HANDLE_VALUE;
    }

    class HandleGuard
    {
    public:
        HandleGuard() noexcept = default;
        explicit HandleGuard(HANDLE handle) noexcept : m_handle(handle) {}
        ~HandleGuard() noexcept { reset(); }

        HandleGuard(const HandleGuard&) = delete;
        HandleGuard& operator=(const HandleGuard&) = delete;

        HandleGuard(HandleGuard&& other) noexcept : m_handle(other.release()) {}
        HandleGuard& operator=(HandleGuard&& other) noexcept
        {
            if (this != &other)
            {
                reset();
                m_handle = other.release();
            }
            return *this;
        }

        void reset(HANDLE handle = nullptr) noexcept
        {
            if (IsHandleValid(m_handle))
                CloseHandle(m_handle);
            m_handle = handle;
        }

        [[nodiscard]] HANDLE get() const noexcept { return m_handle; }
        [[nodiscard]] HANDLE release() noexcept
        {
            HANDLE handle = m_handle;
            m_handle = nullptr;
            return handle;
        }
        [[nodiscard]] explicit operator bool() const noexcept { return IsHandleValid(m_handle); }

    private:
        HANDLE m_handle = nullptr;
    };

    struct WatchdogState
    {
        // Cache-line aligned atomics to prevent false sharing between cores
        alignas(hardware_destructive_interference_size) std::atomic<bool> running{false};
        alignas(hardware_destructive_interference_size) std::atomic<bool> shouldStop{false};
        alignas(hardware_destructive_interference_size) std::atomic<std::chrono::steady_clock::time_point> lastHeartbeat{std::chrono::steady_clock::now()};
        alignas(hardware_destructive_interference_size) std::atomic<DWORD> targetThreadId{0};
        alignas(hardware_destructive_interference_size) std::atomic<DWORD> timeoutSeconds{20};
        
        // Non-atomic handle doesn't need cache-line alignment
        HANDLE watchdogThreadHandle{nullptr};
        
        ~WatchdogState() noexcept
        {
            // Note: watchdogThreadHandle should be closed by StopWatchdogThread,
            // but we close it here as a safety measure in case of abnormal termination
            if (watchdogThreadHandle != nullptr && watchdogThreadHandle != INVALID_HANDLE_VALUE)
            {
                CloseHandle(watchdogThreadHandle);
                watchdogThreadHandle = nullptr;
            }
        }
    };

    static_assert(std::is_trivially_copyable_v<std::chrono::steady_clock::time_point>,
                  "time_point must be trivially copyable for use with std::atomic");
    static_assert(std::has_unique_object_representations_v<std::chrono::steady_clock::time_point>,
                  "time_point must have unique object representations for atomic operations");

    static_assert(std::atomic<bool>::is_always_lock_free,
                  "bool atomics must be lock-free on this platform");
    static_assert(std::atomic<DWORD>::is_always_lock_free,
                  "DWORD atomics must be lock-free on this platform");
    
    // Validate timeout constraints at compile-time using std::conjunction
    template<auto Min, auto Max>
    using valid_range = std::conjunction<
        std::bool_constant<(Min > 0)>,
        std::bool_constant<(Max >= Min)>
    >;
    
    inline WatchdogState g_watchdogState{};
    
    [[nodiscard]] static bool TriggerWatchdogException(HANDLE targetThread, DWORD targetThreadId) noexcept
    {
        AddReportLog(9300, SString("Watchdog freeze detected after %u seconds (thread %u)",
                                   g_watchdogState.timeoutSeconds.load(std::memory_order_relaxed),
                                   targetThreadId));
        
        // Suspend the thread once and capture everything while suspended
        if (SuspendThread(targetThread) == static_cast<DWORD>(-1))
        {
            AddReportLog(9301, SString("Watchdog failed to suspend thread %u", targetThreadId));
            return false;
        }
        
        // Thread is now suspended - capture context
        CONTEXT context{};
        context.ContextFlags = CONTEXT_FULL;
        
        if (GetThreadContext(targetThread, &context) == FALSE)
        {
            AddReportLog(9302, SString("Watchdog failed to capture thread context for %u", targetThreadId));
            
            // Must resume thread before returning!
            if (ResumeThread(targetThread) == static_cast<DWORD>(-1))
            {
                AddReportLog(9303, SString("Watchdog failed to resume thread %u after context error", targetThreadId));
            }
            return false;
        }
        
        // Create exception record and pointers for stack walking (thread still suspended)
        EXCEPTION_RECORD exceptionRecord{};
        exceptionRecord.ExceptionCode = CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT;
        exceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
        exceptionRecord.ExceptionAddress = reinterpret_cast<PVOID>(context.Eip);
        exceptionRecord.NumberParameters = 2;
        exceptionRecord.ExceptionInformation[0] = targetThreadId;
        exceptionRecord.ExceptionInformation[1] = g_watchdogState.timeoutSeconds.load(std::memory_order_relaxed);
        
        // Use static storage to keep exception pointers alive
        static thread_local CONTEXT s_capturedContext{};
        s_capturedContext = context;
        
        EXCEPTION_POINTERS exceptionPointers{};
        exceptionPointers.ExceptionRecord = &exceptionRecord;
        exceptionPointers.ContextRecord = &s_capturedContext;
        
        // Capture stack trace NOW while thread is still suspended - prevents race conditions
        std::vector<std::string> stackTraceVec{};
        const auto stackCaptured = CaptureUnifiedStackTrace(&exceptionPointers, 32, &stackTraceVec);
        
        // Now we can safely resume the thread - we have everything we need
        if (ResumeThread(targetThread) == static_cast<DWORD>(-1))
        {
            AddReportLog(9304, SString("Watchdog failed to resume thread %u after stack capture", targetThreadId));
            // Continue anyway - we still want the crash dump even if resume failed
        }

        AddReportLog(9305, SString("Watchdog captured context for thread %u", targetThreadId));
        
        // Store enhanced exception info
        try
        {
            // Use try_lock to avoid deadlock if main thread was suspended while holding the mutex
            if (std::unique_lock<std::mutex> lock{g_exceptionInfoMutex, std::try_to_lock}; lock.owns_lock())
            {
                // For cleaner initialization
                g_lastExceptionInfo = [&]() {
                    ENHANCED_EXCEPTION_INFO info{};
                    info.exceptionCode = CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT;
                    info.exceptionAddress = exceptionRecord.ExceptionAddress;
                    info.timestamp = std::chrono::system_clock::now();
                    info.threadId = targetThreadId;
                    info.processId = GetCurrentProcessId();
                    info.exceptionType = "Watchdog Timeout (Application Freeze)";
                    info.isFatal = true;
                    
                    const auto timeoutSecs = g_watchdogState.timeoutSeconds.load(std::memory_order_relaxed);
                    info.additionalInfo = "Application became unresponsive for " + std::to_string(timeoutSecs) + " seconds";
                    
                    info.eax = s_capturedContext.Eax;
                    info.ebx = s_capturedContext.Ebx;
                    info.ecx = s_capturedContext.Ecx;
                    info.edx = s_capturedContext.Edx;
                    info.esi = s_capturedContext.Esi;
                    info.edi = s_capturedContext.Edi;
                    info.ebp = s_capturedContext.Ebp;
                    info.esp = s_capturedContext.Esp;
                    info.eip = s_capturedContext.Eip;
                    info.cs = s_capturedContext.SegCs;
                    info.ds = s_capturedContext.SegDs;
                    info.ss = s_capturedContext.SegSs;
                    info.es = s_capturedContext.SegEs;
                    info.fs = s_capturedContext.SegFs;
                    info.gs = s_capturedContext.SegGs;
                    info.eflags = s_capturedContext.EFlags;
                    
                    // Use the stack trace we captured while thread was suspended
                    if (stackCaptured != FALSE && !stackTraceVec.empty())
                    {
                        info.stackTrace = std::move(stackTraceVec);
                        info.hasDetailedStackTrace = true;
                    }
                    
                    return info;  // Guaranteed copy - no move, no copy
                }();
            }
            else
            {
                AddReportLog(9306, "Watchdog skipped enhanced info capture (mutex busy)");
            }
        }
        catch (...)
        {
            // To detect exception-during-exception scenarios
            if (std::uncaught_exceptions() > 1)
            {
                AddReportLog(9307, "Watchdog exception while storing enhanced info during stack unwinding");
            }
            else
            {
                AddReportLog(9308, "Watchdog exception while storing enhanced info");
            }
        }
        
        // Trigger the global exception handler
        CCrashDumpWriter::HandleExceptionGlobal(&exceptionPointers);
        
        return true;
    }
    
    [[nodiscard]] static unsigned int __stdcall WatchdogThreadProc(void* /*pParameter*/) noexcept
    {
        AddReportLog(9309, "Watchdog thread started");
        
        constexpr auto kCheckInterval = std::chrono::milliseconds{500};
        static_assert(kCheckInterval.count() > 0, "Check interval must be positive");
        
        const auto targetThreadId = g_watchdogState.targetThreadId.load(std::memory_order_acquire);
        
        // Open handle to target thread
        constexpr DWORD kThreadAccess = THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION;
        HandleGuard targetThread{OpenThread(kThreadAccess, FALSE, targetThreadId)};

        if (!targetThread)
        {
            AddReportLog(9310, SString("Watchdog failed to open thread %lu (error %lu)", targetThreadId, GetLastError()));

            g_watchdogState.running.store(false, std::memory_order_release);
            return 1;
        }

        // Note: targetThread handle is local to this thread - no global state needed

        while (!g_watchdogState.shouldStop.load(std::memory_order_acquire))
        {
            const auto now = std::chrono::steady_clock::now();
            const auto lastBeat = g_watchdogState.lastHeartbeat.load(std::memory_order_acquire);
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeat);
            const auto timeoutSecs = g_watchdogState.timeoutSeconds.load(std::memory_order_relaxed);

            if (elapsed.count() >= static_cast<std::chrono::seconds::rep>(timeoutSecs))
            {
                AddReportLog(9311, SString("Watchdog detected freeze after %lld seconds (threshold %u)",
                                           static_cast<long long>(elapsed.count()),
                                           timeoutSecs));

                const bool triggered = TriggerWatchdogException(targetThread.get(), targetThreadId);
                if (!triggered)
                {
                    AddReportLog(9312, SString("Watchdog trigger failed for thread %u", targetThreadId));
                }

                // Exit after triggering - crash dump handler will terminate process
                break;
            }

            std::this_thread::sleep_for(kCheckInterval);
        }

        AddReportLog(9313, "Watchdog thread stopping");

        g_watchdogState.running.store(false, std::memory_order_release);

        return 0;
    }
}

[[nodiscard]] BOOL BUGSUTIL_DLLINTERFACE __stdcall StartWatchdogThread(DWORD mainThreadId, DWORD timeoutSeconds) noexcept
{
    // Pprevent race conditions on double-start
    auto expected = false;
    if (!g_watchdogState.running.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        AddReportLog(9314, "Watchdog thread already running");
        return FALSE;
    }
    
    constexpr auto kMinTimeout = DWORD{5};
    constexpr auto kMaxTimeout = DWORD{300};
    static_assert(kMinTimeout > 0, "Minimum timeout must be positive");
    static_assert(kMaxTimeout >= kMinTimeout, "Maximum timeout must be >= minimum");
    
    if (timeoutSeconds < kMinTimeout || timeoutSeconds > kMaxTimeout)
    {
        AddReportLog(9315, SString("Watchdog invalid timeout %u (allowed %u-%u)", timeoutSeconds, kMinTimeout, kMaxTimeout));

        g_watchdogState.running.store(false, std::memory_order_release);
        return FALSE;
    }
    
    g_watchdogState.targetThreadId.store(mainThreadId, std::memory_order_release);
    g_watchdogState.timeoutSeconds.store(timeoutSeconds, std::memory_order_release);
    g_watchdogState.shouldStop.store(false, std::memory_order_release);
    g_watchdogState.lastHeartbeat.store(std::chrono::steady_clock::now(), std::memory_order_release);
    
    constexpr unsigned int kStackSize = 0;
    const auto threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(
        nullptr,
        kStackSize,
        WatchdogThreadProc,
        nullptr,
        0,
        nullptr
    ));
    
    HandleGuard watchdogHandle{threadHandle};

    if (!watchdogHandle)
    {
        AddReportLog(9316, SString("Watchdog failed to create thread (error %u)", GetLastError()));

        g_watchdogState.running.store(false, std::memory_order_release);
        return FALSE;
    }

    g_watchdogState.watchdogThreadHandle = watchdogHandle.release();

    AddReportLog(9317, SString("Watchdog monitoring thread %u with %u second timeout", mainThreadId, timeoutSeconds));
    
    return TRUE;
}

void BUGSUTIL_DLLINTERFACE __stdcall StopWatchdogThread() noexcept
{
    if (!g_watchdogState.running.load(std::memory_order_acquire))
    {
        return;
    }

    AddReportLog(9318, "Stopping watchdog thread");
    g_watchdogState.shouldStop.store(true, std::memory_order_release);
    
    if (g_watchdogState.watchdogThreadHandle != nullptr && 
        g_watchdogState.watchdogThreadHandle != INVALID_HANDLE_VALUE)
    {
        constexpr DWORD kWaitTimeout = 5000;
        if (const auto waitResult = WaitForSingleObject(g_watchdogState.watchdogThreadHandle, kWaitTimeout);
            waitResult != WAIT_OBJECT_0)
        {
            AddReportLog(9319, "Watchdog thread forced termination");
            TerminateThread(g_watchdogState.watchdogThreadHandle, 1);
            WaitForSingleObject(g_watchdogState.watchdogThreadHandle, INFINITE);
        }
        
        CloseHandle(g_watchdogState.watchdogThreadHandle);
        g_watchdogState.watchdogThreadHandle = nullptr;
    }

    AddReportLog(9320, "Watchdog thread stopped");
}

void BUGSUTIL_DLLINTERFACE __stdcall UpdateWatchdogHeartbeat() noexcept
{
    if (g_watchdogState.running.load(std::memory_order_acquire))
    {
        g_watchdogState.lastHeartbeat.store(std::chrono::steady_clock::now(), std::memory_order_release);
    }
}
