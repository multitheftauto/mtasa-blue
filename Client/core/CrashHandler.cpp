/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CrashHandler.cpp
 *  PURPOSE:     Crash handling functions
 *
 *  THIS FILE CREDITS:
 *  "Debugging Applications" (Microsoft Press)
 *  Copyright (c) 1997-2000 John Robbins -- All rights reserved.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CrashHandler.h"
#include <SharedUtil.Detours.h>
#include <exception>
#include <new>
#include <process.h>
#include <stdio.h>
#include <atomic>
#include <array>
#include <algorithm>
#include <mutex>
#include <signal.h>
#include <intrin.h>  // For _ReturnAddress()


// C++ exception code
constexpr DWORD CPP_EXCEPTION_CODE = 0xE06D7363;

// STATUS_INVALID_CRUNTIME_PARAMETER - Invalid CRT parameter
constexpr DWORD STATUS_INVALID_CRUNTIME_PARAMETER_CODE = 0xC0000417;

// STATUS_STACK_BUFFER_OVERRUN - Fast-fail exception
constexpr DWORD STATUS_STACK_BUFFER_OVERRUN_CODE = 0xC0000409;

// Buffer sizes for debug strings
constexpr size_t DEBUG_BUFFER_SIZE = 256;
constexpr size_t DEBUG_BUFFER_SIZE_LARGE = 512;

// Debug string prefixes
#define DEBUG_PREFIX_CRASH     "CrashHandler: "
#define DEBUG_PREFIX_VEH       "VEH: "
#define DEBUG_PREFIX_SEH       "SEH: "
#define DEBUG_PREFIX_CPP       "C++ TERMINATE: "
#define DEBUG_PREFIX_ABORT     "ABORT: "
#define DEBUG_PREFIX_PURECALL  "PURECALL: "
#define DEBUG_SEPARATOR        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"

// Magic numbers as named constants
constexpr UINT      VEH_HANDLER_PRIORITY_FIRST = 1;
constexpr int       EXIT_CODE_CRASH = 3;

// Sync
static std::mutex g_handlerStateMutex;
//
static std::atomic<PFNCHFILTFN>               g_pfnCrashCallback{nullptr};
static std::atomic<LPTOP_LEVEL_EXCEPTION_FILTER>           g_pfnOrigFilt{nullptr};
static std::atomic<PVOID>                                  g_pVectoredHandler{nullptr};
static std::atomic<std::terminate_handler>                 g_pfnOrigTerminate{nullptr};
static std::atomic<std::new_handler>                       g_pfnOrigNewHandler{nullptr};
static std::atomic<decltype(&SetUnhandledExceptionFilter)> g_pfnKernelSetUnhandledExceptionFilter{nullptr};
static decltype(&SetUnhandledExceptionFilter)              g_kernelSetUnhandledExceptionFilterTrampoline = nullptr;

// Stack cookie failure detection
static std::atomic<bool>                      g_bStackCookieCaptureEnabled{false};
static std::atomic<void (__cdecl *)(int)>     g_pfnOrigAbortHandler{nullptr};
static std::atomic<_purecall_handler>         g_pfnOrigPureCall{nullptr};
static std::atomic<bool>                      g_bInAbortHandler{false};

/*//////////////////////////////////////////////////////////////////////
                    File Scope Function Declarations
//////////////////////////////////////////////////////////////////////*/
LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs);
LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExPtrs);

[[noreturn]] void __cdecl CppTerminateHandler() noexcept;
[[noreturn]] void __cdecl CppNewHandler() noexcept;
void __cdecl AbortSignalHandler(int signal) noexcept;
[[noreturn]] void __cdecl PureCallHandler() noexcept;

// Helper functions
static void InstallCppHandlers() noexcept;
static void InstallVectoredHandler() noexcept;
static void InstallSehHandler() noexcept;
static void InstallAbortHandlers() noexcept;
static void UninstallCrashHandlers() noexcept;
static void ReportCurrentCppException() noexcept;

// Exception context creation
static bool BuildExceptionContext(EXCEPTION_POINTERS& outExPtrs, EXCEPTION_RECORD*& outExRecord,
                                  CONTEXT*& outCtx, DWORD dwExceptionCode) noexcept;

// Safe initialization helper
inline void InitializeExceptionRecord(EXCEPTION_RECORD* const pRecord, const DWORD code, const void* const address) noexcept;

// Minimal signal-safe output
static void SignalSafeOutput(const char* message) noexcept;


#define SAFE_DEBUG_PRINT(buffer, format, ...) \
    do \
    { \
        _snprintf_s((buffer).data(), (buffer).size(), _TRUNCATE, format, __VA_ARGS__); \
        OutputDebugStringA((buffer).data()); \
    } while (false)

#define SAFE_DEBUG_PRINT_C(buffer, bufferSize, format, ...) \
    do \
    { \
        _snprintf_s(buffer, bufferSize, _TRUNCATE, format, __VA_ARGS__); \
        OutputDebugStringA(buffer); \
    } while (false)

inline void SafeDebugOutput(const char* message) noexcept
{
    if (message != nullptr)
    {
        OutputDebugStringA(message);
    }
}

static void SignalSafeOutput(const char* message) noexcept
{
    if (message != nullptr)
    {
        OutputDebugStringA(message);
    }
}

// Helpers
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

    std::fill(std::begin(pRecord->ExceptionInformation), std::end(pRecord->ExceptionInformation), static_cast<ULONG_PTR>(0));
}

static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI RedirectedSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) noexcept
{
    std::array<char, DEBUG_BUFFER_SIZE> szLog{};
    SAFE_DEBUG_PRINT(szLog, "%sIntercepted SetUnhandledExceptionFilter call with arg 0x%p\n", DEBUG_PREFIX_CRASH,
                     static_cast<const void*>(lpTopLevelExceptionFilter));

    if (lpTopLevelExceptionFilter == nullptr)
    {
        SafeDebugOutput("CrashHandler: Blocked attempt to remove exception filter\n");
        return CrashHandlerExceptionFilter;
    }

    SafeDebugOutput("CrashHandler: Allowing non-NULL exception filter change\n");
    auto kernelSet = g_pfnKernelSetUnhandledExceptionFilter.load(std::memory_order_acquire);
    if (kernelSet == nullptr)
    {
        SafeDebugOutput("CrashHandler: WARNING - Original SetUnhandledExceptionFilter pointer missing\n");
        return lpTopLevelExceptionFilter;
    }

    return kernelSet(lpTopLevelExceptionFilter);
}

// Abort and Pure Call handlurs
void __cdecl AbortSignalHandler(int /* signal */) noexcept
{
    // Prevent recursive abort handling
    bool expected = false;
    if (!g_bInAbortHandler.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        SignalSafeOutput(DEBUG_PREFIX_ABORT "Recursive\n");
        _exit(EXIT_CODE_CRASH);
    }

    SignalSafeOutput(DEBUG_SEPARATOR);
    SignalSafeOutput(DEBUG_PREFIX_ABORT "SIGABRT (0xC0000409)\n");
    SignalSafeOutput(DEBUG_SEPARATOR);

    // Use properly aligned stack buffers (C++ alignment, safe in C++ function)
    // Lifetime is tied to this noreturn handler; any early exit would invalidate pointers passed below.
    alignas(16) EXCEPTION_RECORD exRecord = {};
    alignas(16) CONTEXT ctx = {};

    // Get return address for exception record
    void* returnAddr = _ReturnAddress();

    // Initialize structures
    InitializeExceptionRecord(&exRecord, STATUS_STACK_BUFFER_OVERRUN_CODE, returnAddr);
    
    ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
    RtlCaptureContext(&ctx);

    EXCEPTION_POINTERS exPtrs = {};
    exPtrs.ExceptionRecord = &exRecord;
    exPtrs.ContextRecord = &ctx;

    // Atomically load callback
    PFNCHFILTFN callback = g_pfnCrashCallback.load(std::memory_order_acquire);
    
    if (callback != nullptr)
    {
        SignalSafeOutput(DEBUG_PREFIX_ABORT "Calling handler\n");

        __try
        {
            callback(&exPtrs);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            SignalSafeOutput(DEBUG_PREFIX_ABORT "Handler exception\n");
        }
    }
    else
    {
        SignalSafeOutput(DEBUG_PREFIX_ABORT "No handler\n");
    }

    SignalSafeOutput(DEBUG_PREFIX_ABORT "Terminating\n");

    g_bInAbortHandler.store(false, std::memory_order_release);

    TerminateProcess(GetCurrentProcess(), EXIT_CODE_CRASH);
    _exit(EXIT_CODE_CRASH);
}

[[noreturn]] void __cdecl PureCallHandler() noexcept
{
    SafeDebugOutput(DEBUG_SEPARATOR);
    SafeDebugOutput(DEBUG_PREFIX_PURECALL "Pure virtual function call detected\n");
    SafeDebugOutput(DEBUG_SEPARATOR);

    EXCEPTION_RECORD*   pExRecord = nullptr;
    CONTEXT*            pCtx = nullptr;
    EXCEPTION_POINTERS  exPtrs{};

    if (BuildExceptionContext(exPtrs, pExRecord, pCtx, 0xC0000025))
    {
        PFNCHFILTFN callback = g_pfnCrashCallback.load(std::memory_order_acquire);
        if (callback != nullptr)
        {
            SafeDebugOutput(DEBUG_PREFIX_PURECALL "Calling crash handler callback\n");

            __try
            {
                callback(&exPtrs);
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                SafeDebugOutput(DEBUG_PREFIX_PURECALL "Exception in crash handler callback\n");
            }
        }
        else
        {
            SafeDebugOutput(DEBUG_PREFIX_PURECALL "No crash handler callback available\n");
        }
    }
    else
    {
        SafeDebugOutput(DEBUG_PREFIX_PURECALL "Failed to allocate exception structures\n");
    }

    if (pCtx != nullptr)
    {
        LocalFree(pCtx);
    }

    if (pExRecord != nullptr)
    {
        LocalFree(pExRecord);
    }

    SafeDebugOutput(DEBUG_PREFIX_PURECALL "Terminating process\n");
    TerminateProcess(GetCurrentProcess(), EXIT_CODE_CRASH);
    _exit(EXIT_CODE_CRASH);
}

/*//////////////////////////////////////////////////////////////////////
                    Destructor Class
//////////////////////////////////////////////////////////////////////*/
#pragma warning(disable : 4073)
#pragma init_seg(lib)
class CleanUpCrashHandler
{
public:
    CleanUpCrashHandler() = default;
    ~CleanUpCrashHandler() noexcept
    {
        std::lock_guard<std::mutex> lock(g_handlerStateMutex);

        g_bStackCookieCaptureEnabled.store(false, std::memory_order_release);
        g_pfnCrashCallback.store(nullptr, std::memory_order_release);

        PVOID vectoredHandler = g_pVectoredHandler.exchange(nullptr, std::memory_order_acq_rel);
        if (vectoredHandler != nullptr)
        {
            RemoveVectoredExceptionHandler(vectoredHandler);
        }

        std::terminate_handler terminateHandler = g_pfnOrigTerminate.exchange(nullptr, std::memory_order_acq_rel);
        if (terminateHandler != nullptr)
        {
            std::set_terminate(terminateHandler);
        }

        std::new_handler newHandler = g_pfnOrigNewHandler.exchange(nullptr, std::memory_order_acq_rel);
        if (newHandler != nullptr)
        {
            std::set_new_handler(newHandler);
        }

        auto abortHandler = g_pfnOrigAbortHandler.exchange(nullptr, std::memory_order_acq_rel);
        if (abortHandler != nullptr)
        {
            signal(SIGABRT, abortHandler);
        }

        _purecall_handler pureCallHandler = g_pfnOrigPureCall.exchange(nullptr, std::memory_order_acq_rel);
        if (pureCallHandler != nullptr)
        {
            _set_purecall_handler(pureCallHandler);
        }

        auto previousFilter = g_pfnOrigFilt.exchange(nullptr, std::memory_order_acq_rel);
        if (previousFilter != nullptr)
        {
            SetUnhandledExceptionFilter(previousFilter);
        }

        auto kernelSetUnhandled = g_pfnKernelSetUnhandledExceptionFilter.exchange(nullptr, std::memory_order_acq_rel);
        if (kernelSetUnhandled != nullptr)
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

/*//////////////////////////////////////////////////////////////////////
                 Crash Handler Installation Functions
//////////////////////////////////////////////////////////////////////*/

BOOL __stdcall SetCrashHandlerFilter(PFNCHFILTFN pFn)
{
    if (pFn == nullptr)
    {
        UninstallCrashHandlers();
    }
    else
    {
        g_pfnCrashCallback.store(pFn, std::memory_order_release);

        std::array<char, DEBUG_BUFFER_SIZE> szDebug{};
        SAFE_DEBUG_PRINT(szDebug, "%sInstalling handlers with callback at 0x%p\n", DEBUG_PREFIX_CRASH,
                         reinterpret_cast<const void*>(pFn));

        if (g_bStackCookieCaptureEnabled.load(std::memory_order_acquire))
        {
            InstallAbortHandlers();
        }
        
        InstallCppHandlers();
        InstallVectoredHandler();
        InstallSehHandler();

        SafeDebugOutput("CrashHandler: All handlers installed successfully\n");
        SafeDebugOutput("===========================================\n");
    }
    return TRUE;
}

BOOL __stdcall EnableStackCookieFailureCapture(BOOL bEnable)
{
    const bool bEnableBool = (bEnable != FALSE);
    g_bStackCookieCaptureEnabled.store(bEnableBool, std::memory_order_release);
    
    std::array<char, DEBUG_BUFFER_SIZE> szDebug{};
    SAFE_DEBUG_PRINT(szDebug, "%sStack cookie failure capture %s\n", DEBUG_PREFIX_CRASH, 
                     bEnableBool ? "ENABLED" : "DISABLED");
    
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
        std::lock_guard<std::mutex> lock(g_handlerStateMutex);

        auto abortHandler = g_pfnOrigAbortHandler.exchange(nullptr, std::memory_order_acq_rel);
        if (abortHandler != nullptr)
        {
            signal(SIGABRT, abortHandler);
            SafeDebugOutput("CrashHandler: SIGABRT handler restored\n");
        }

        _purecall_handler pureCallHandler = g_pfnOrigPureCall.exchange(nullptr, std::memory_order_acq_rel);
        if (pureCallHandler != nullptr)
        {
            _set_purecall_handler(pureCallHandler);
            SafeDebugOutput("CrashHandler: Pure call handler restored\n");
        }

        g_bInAbortHandler.store(false, std::memory_order_release);
    }
    
    return TRUE;
}

static void InstallCppHandlers() noexcept
{
    std::lock_guard<std::mutex> lock(g_handlerStateMutex);

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
    std::lock_guard<std::mutex> lock(g_handlerStateMutex);

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
        void (__cdecl *prevHandler)(int) = signal(SIGABRT, AbortSignalHandler);
        
        if (prevHandler != SIG_ERR)
        {
            g_pfnOrigAbortHandler.store(prevHandler, std::memory_order_release);
            SafeDebugOutput("CrashHandler: SIGABRT handler installed\n");
        }
        else
        {
            SafeDebugOutput("CrashHandler: WARNING - Failed to install SIGABRT handler\n");
        }
    }

    if (g_pfnOrigPureCall.load(std::memory_order_acquire) == nullptr)
    {
        _purecall_handler previous = _set_purecall_handler(PureCallHandler);
        g_pfnOrigPureCall.store(previous, std::memory_order_release);
        SafeDebugOutput("CrashHandler: Pure virtual call handler installed\n");
    }
}

static void InstallVectoredHandler() noexcept
{
    std::lock_guard<std::mutex> lock(g_handlerStateMutex);

    if (g_pVectoredHandler.load(std::memory_order_acquire) != nullptr)
    {
        SafeDebugOutput("CrashHandler: VEH already installed\n");
        return;
    }

    PVOID installedHandler = AddVectoredExceptionHandler(VEH_HANDLER_PRIORITY_FIRST, VectoredExceptionHandler);
    g_pVectoredHandler.store(installedHandler, std::memory_order_release);
    std::array<char, DEBUG_BUFFER_SIZE> szDebug{};
    if (installedHandler != nullptr)
    {
        SAFE_DEBUG_PRINT(szDebug, "%sVEH installed successfully at 0x%p\n", DEBUG_PREFIX_CRASH, installedHandler);
    }
    else
    {
        const DWORD dwError = GetLastError();
        SAFE_DEBUG_PRINT(szDebug, "%sVEH installation FAILED! Error: %u\n", DEBUG_PREFIX_CRASH, dwError);
    }
}

static void InstallSehHandler() noexcept
{
    std::lock_guard<std::mutex> lock(g_handlerStateMutex);

    if (g_pfnOrigFilt.load(std::memory_order_acquire) != nullptr)
    {
        SafeDebugOutput("CrashHandler: SEH already installed\n");
        return;
    }

    LPTOP_LEVEL_EXCEPTION_FILTER previousFilter = SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
    g_pfnOrigFilt.store(previousFilter, std::memory_order_release);

    std::array<char, DEBUG_BUFFER_SIZE> szDebug{};
    SAFE_DEBUG_PRINT(szDebug, "%sSEH installed, previous filter at 0x%p\n", DEBUG_PREFIX_CRASH, static_cast<const void*>(previousFilter));

    if (g_pfnKernelSetUnhandledExceptionFilter.load(std::memory_order_acquire) != nullptr)
    {
        SafeDebugOutput("CrashHandler: SEH detour protection already installed\n");
        return;
    }

    HMODULE kernelModule = GetModuleHandleW(L"KERNEL32.DLL");
    if (kernelModule == nullptr)
    {
        SafeDebugOutput("CrashHandler: WARNING - Failed to resolve KERNEL32.DLL handle\n");
        return;
    }

    auto setUnhandled = reinterpret_cast<decltype(&SetUnhandledExceptionFilter)>(GetProcAddress(kernelModule, "SetUnhandledExceptionFilter"));
    if (setUnhandled == nullptr)
    {
        SafeDebugOutput("CrashHandler: WARNING - Failed to resolve SetUnhandledExceptionFilter address\n");
        return;
    }

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
    }
}

static void UninstallCrashHandlers() noexcept
{
    std::lock_guard<std::mutex> lock(g_handlerStateMutex);

    SafeDebugOutput("CrashHandler: Unhooking crash handlers\n");

    g_pfnCrashCallback.store(nullptr, std::memory_order_release);
    g_bStackCookieCaptureEnabled.store(false, std::memory_order_release);

    PVOID vectoredHandler = g_pVectoredHandler.exchange(nullptr, std::memory_order_acq_rel);
    if (vectoredHandler != nullptr)
    {
        RemoveVectoredExceptionHandler(vectoredHandler);
    }

    std::terminate_handler terminateHandler = g_pfnOrigTerminate.exchange(nullptr, std::memory_order_acq_rel);
    if (terminateHandler != nullptr)
    {
        std::set_terminate(terminateHandler);
    }

    std::new_handler newHandler = g_pfnOrigNewHandler.exchange(nullptr, std::memory_order_acq_rel);
    if (newHandler != nullptr)
    {
        std::set_new_handler(newHandler);
    }

    auto abortHandler = g_pfnOrigAbortHandler.exchange(nullptr, std::memory_order_acq_rel);
    if (abortHandler != nullptr)
    {
        signal(SIGABRT, abortHandler);
    }

    _purecall_handler pureCallHandler = g_pfnOrigPureCall.exchange(nullptr, std::memory_order_acq_rel);
    if (pureCallHandler != nullptr)
    {
        _set_purecall_handler(pureCallHandler);
    }

    auto previousFilt = g_pfnOrigFilt.exchange(nullptr, std::memory_order_acq_rel);
    if (previousFilt != nullptr)
    {
        SetUnhandledExceptionFilter(previousFilt);
    }

    auto kernelSetUnhandled = g_pfnKernelSetUnhandledExceptionFilter.exchange(nullptr, std::memory_order_acq_rel);
    if (kernelSetUnhandled != nullptr)
    {
        g_kernelSetUnhandledExceptionFilterTrampoline = kernelSetUnhandled;
        if (!SharedUtil::UndoFunctionDetour(g_kernelSetUnhandledExceptionFilterTrampoline, reinterpret_cast<void*>(RedirectedSetUnhandledExceptionFilter)))
        {
            SafeDebugOutput("CrashHandler: WARNING - Failed to undo SEH detour\n");
        }
        g_kernelSetUnhandledExceptionFilterTrampoline = nullptr;
    }
}

static bool BuildExceptionContext(EXCEPTION_POINTERS& outExPtrs, EXCEPTION_RECORD*& outExRecord,
                                  CONTEXT*& outCtx, DWORD dwExceptionCode) noexcept
{
    outExRecord = static_cast<EXCEPTION_RECORD*>(LocalAlloc(LPTR, sizeof(EXCEPTION_RECORD)));
    outCtx = static_cast<CONTEXT*>(LocalAlloc(LPTR, sizeof(CONTEXT)));

    if (outExRecord == nullptr || outCtx == nullptr)
    {
        if (outExRecord != nullptr)
        {
            LocalFree(outExRecord);
            outExRecord = nullptr;
        }

        if (outCtx != nullptr)
        {
            LocalFree(outCtx);
            outCtx = nullptr;
        }

        return false;
    }

    void* returnAddr = _ReturnAddress();
    InitializeExceptionRecord(outExRecord, dwExceptionCode, returnAddr);

    outCtx->ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;

    __try
    {
        RtlCaptureContext(outCtx);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
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

// C++ exception handlers
[[noreturn]] void __cdecl CppTerminateHandler() noexcept
{
    SafeDebugOutput(DEBUG_SEPARATOR);
    SafeDebugOutput(DEBUG_PREFIX_CPP "Unhandled C++ exception detected\n");

    ReportCurrentCppException();

    SafeDebugOutput(DEBUG_SEPARATOR);

    EXCEPTION_RECORD*   pExRecord = nullptr;
    CONTEXT*            pCtx = nullptr;
    EXCEPTION_POINTERS  exPtrs{};

    PFNCHFILTFN callback = g_pfnCrashCallback.load(std::memory_order_acquire);

    if (BuildExceptionContext(exPtrs, pExRecord, pCtx, CPP_EXCEPTION_CODE) && callback != nullptr)
    {
        SafeDebugOutput(DEBUG_PREFIX_CPP "Calling crash handler callback\n");

        __try
        {
            callback(&exPtrs);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            SafeDebugOutput(DEBUG_PREFIX_CPP "Exception in crash handler callback\n");
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

    SafeDebugOutput(DEBUG_PREFIX_CPP "Terminating process\n");
    TerminateProcess(GetCurrentProcess(), EXIT_CODE_CRASH);
    _exit(EXIT_CODE_CRASH);
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
    catch (const std::bad_alloc&)
    {
        SafeDebugOutput(DEBUG_PREFIX_CPP "std::bad_alloc caught - OUT OF MEMORY!\n");
    }
    catch (const std::bad_array_new_length&)
    {
        SafeDebugOutput(DEBUG_PREFIX_CPP "std::bad_array_new_length caught\n");
    }
    catch (const std::exception& e)
    {
        std::array<char, DEBUG_BUFFER_SIZE_LARGE> szDebug{};
        SAFE_DEBUG_PRINT(szDebug, DEBUG_PREFIX_CPP "std::exception caught: %s\n", e.what());
    }
    catch (...)
    {
        SafeDebugOutput(DEBUG_PREFIX_CPP "Unknown exception type\n");
    }
}

[[noreturn]] void __cdecl CppNewHandler() noexcept
{
    SafeDebugOutput(DEBUG_SEPARATOR);
    SafeDebugOutput("C++ NEW HANDLER: Memory allocation failed\n");
    SafeDebugOutput(DEBUG_SEPARATOR);

    std::terminate();
}

// Exception handlrs
LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExPtrs)
{
    if (pExPtrs == nullptr || pExPtrs->ExceptionRecord == nullptr)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    const DWORD dwExceptionCode = pExPtrs->ExceptionRecord->ExceptionCode;

    if (STATUS_INVALID_CRUNTIME_PARAMETER_CODE == dwExceptionCode || STATUS_STACK_BUFFER_OVERRUN_CODE == dwExceptionCode)
    {
        SafeDebugOutput(DEBUG_SEPARATOR);
        SafeDebugOutput("VEH: STACK BUFFER OVERRUN DETECTED (0xC0000409)\n");
        SafeDebugOutput(DEBUG_SEPARATOR);
    }
    else if (EXCEPTION_STACK_OVERFLOW == dwExceptionCode)
    {
        SafeDebugOutput(DEBUG_SEPARATOR);
        SafeDebugOutput("VEH: EXCEPTION_STACK_OVERFLOW occurred\n");
        SafeDebugOutput(DEBUG_SEPARATOR);

        _resetstkoflw();
    }
    else if (CPP_EXCEPTION_CODE == dwExceptionCode)
    {
        SafeDebugOutput("VEH: C++ exception detected (0xE06D7363)\n");
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
{
    if (pExPtrs == nullptr || pExPtrs->ExceptionRecord == nullptr)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    LONG lRet = EXCEPTION_CONTINUE_SEARCH;

    static std::atomic<bool> bHandlerActive{false};
    bool                     expected = false;

    if (!bHandlerActive.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed))
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    SafeDebugOutput(DEBUG_SEPARATOR);
    SafeDebugOutput("SEH: Unhandled exception filter triggered\n");

    const DWORD dwExceptionCode = pExPtrs->ExceptionRecord->ExceptionCode;

    if (STATUS_INVALID_CRUNTIME_PARAMETER_CODE == dwExceptionCode || STATUS_STACK_BUFFER_OVERRUN_CODE == dwExceptionCode)
    {
        SafeDebugOutput("SEH: STACK BUFFER OVERRUN (0xC0000409)\n");
    }
    else if (EXCEPTION_STACK_OVERFLOW == dwExceptionCode)
    {
        SafeDebugOutput("SEH: EXCEPTION_STACK_OVERFLOW occurred\n");
        _resetstkoflw();
    }
    else
    {
        char szDebug[DEBUG_BUFFER_SIZE];
        SAFE_DEBUG_PRINT_C(szDebug, DEBUG_BUFFER_SIZE, "%sException 0x%08X at 0x%p\n", DEBUG_PREFIX_SEH, dwExceptionCode,
                           static_cast<const void*>(pExPtrs->ExceptionRecord->ExceptionAddress));
    }

    SafeDebugOutput(DEBUG_SEPARATOR);

    __try
    {
        PFNCHFILTFN callback = g_pfnCrashCallback.load(std::memory_order_acquire);
        if (callback != nullptr)
        {
            lRet = callback(pExPtrs);
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        SafeDebugOutput("SEH: CRITICAL - Exception in crash handler callback!\n");
        lRet = EXCEPTION_CONTINUE_SEARCH;
    }

    bHandlerActive.store(false, std::memory_order_release);

    return lRet;
}

