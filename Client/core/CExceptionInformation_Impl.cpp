/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CExceptionInformation_Impl.cpp
 *  PURPOSE:     Exception information parser
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <SharedUtil.Misc.h>
#include <SharedUtil.Detours.h>
#include <SharedUtil.Memory.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <exception>
#include <errno.h>
#include <filesystem>
#include <intrin.h>
#include <memory>
#include <new>
#include <optional>
#include <string_view>
#include <vector>

#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

static void DebugPrintExceptionInfo([[maybe_unused]] const char* /*message*/)
{
}

[[nodiscard]] static bool ValidateExceptionContext(_EXCEPTION_POINTERS* pException)
{
    if (pException == nullptr)
        return false;

    if (pException->ExceptionRecord == nullptr)
        return false;

    const EXCEPTION_RECORD* pRecord = pException->ExceptionRecord;
    if (pRecord->ExceptionCode == 0)
        return false;

    // CUSTOM_EXCEPTION_CODE_OOM is a custom exception raised manually via RaiseException
    // Always accept it for crash dump processing regardless of context state
    if (pRecord->ExceptionCode == CUSTOM_EXCEPTION_CODE_OOM)
        return true;

    // CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT is a custom exception for frozen threads
    // Always accept it for crash dump processing
    if (pRecord->ExceptionCode == CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT)
        return true;

    // STATUS_FATAL_USER_CALLBACK_EXCEPTION (0xC000041D) - Windows often provides
    // null or incomplete context for callback exceptions. Accept them anyway to generate dumps.
    if (pRecord->ExceptionCode == 0xC000041D)
    {
        // Accept callback exceptions even with null context - we'll work with what we have
        return true;
    }

    // For other exceptions, require valid context to ensure we can extract meaningful information
    if (pException->ContextRecord == nullptr)
        return false;

    const CONTEXT* pContext = pException->ContextRecord;

    if (pContext->ContextFlags == 0)
        return false;

    if (pContext->ContextFlags & CONTEXT_CONTROL)
    {
        if (pContext->Esp == 0)
        {
            return false;
        }
    }

    return true;
}

constexpr std::size_t MAX_MODULE_PATH = 512;
constexpr std::size_t MAX_STACK_WALK_DEPTH = 16;
constexpr std::size_t MAX_STACK_FRAMES = 32;
constexpr std::size_t MAX_SYMBOL_NAME = 256;

[[nodiscard]] static bool CopyModulePathToBuffer(const char* source, char* destination, std::size_t destinationSize, const char* context)
{
    if (source == nullptr || destination == nullptr || destinationSize == 0U)
        return false;

    const errno_t copyResult = strncpy_s(destination, destinationSize, source, _TRUNCATE);
    if (copyResult == STRUNCATE)
    {
        char buffer[DEBUG_BUFFER_SIZE] = {};
        SAFE_DEBUG_PRINT_C(buffer, DEBUG_BUFFER_SIZE, "%.*s%s - Module path truncated to %zu bytes\n", 
            static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(), context, destinationSize - 1U);
        return true;
    }

    if (copyResult != 0)
    {
        char buffer[DEBUG_BUFFER_SIZE] = {};
        SAFE_DEBUG_PRINT_C(buffer, DEBUG_BUFFER_SIZE, "%.*s%s - strncpy_s failed (error=%d)\n", static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(), context, copyResult);
        if (destinationSize > 0U)
            destination[0] = '\0';
        return false;
    }

    return true;
}

struct StackFrameInfo
{
    std::string symbolName;
    std::string fileName;
    DWORD       lineNumber;
    DWORD64     address;
    bool        isValid;
};
constexpr std::size_t MAX_FILE_NAME = 260;

class SymbolHandlerGuard
{
public:
    explicit SymbolHandlerGuard(HANDLE process) : m_process(process), m_initialized(false), m_uncaughtExceptions(std::uncaught_exceptions())
    {
        if (m_process != nullptr)
        {
            if (SymInitialize(m_process, nullptr, TRUE) != FALSE)
            {
                m_initialized = true;
            }
            else
            {
                const DWORD                         dwError = GetLastError();
                char debugBuffer[DEBUG_BUFFER_SIZE] = {};
                SAFE_DEBUG_PRINT_C(debugBuffer, DEBUG_BUFFER_SIZE, "%.*sSymbolHandlerGuard: SymInitialize failed, error: %u\n", 
                    static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(), dwError);

            }
        }
    }

    ~SymbolHandlerGuard()
    {
        if (m_initialized)
        {
            if (std::uncaught_exceptions() <= m_uncaughtExceptions)
            {
                SymCleanup(m_process);
            }
            m_initialized = false;
        }
    }

    SymbolHandlerGuard(const SymbolHandlerGuard&) = delete;
    SymbolHandlerGuard& operator=(const SymbolHandlerGuard&) = delete;
    SymbolHandlerGuard(SymbolHandlerGuard&&) = delete;
    SymbolHandlerGuard& operator=(SymbolHandlerGuard&&) = delete;

    bool IsInitialized() const { return m_initialized; }

private:
    HANDLE m_process;
    bool   m_initialized;
    int    m_uncaughtExceptions;
};

[[nodiscard]] static std::optional<std::vector<std::string>> CaptureEnhancedStackTrace(_EXCEPTION_POINTERS* pException)
{
    if (pException == nullptr || pException->ContextRecord == nullptr)
        return std::nullopt;

    std::vector<StackFrameInfo> frames;
    frames.reserve(MAX_STACK_FRAMES);

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    CONTEXT      context = *pException->ContextRecord;
    STACKFRAME64 frame;
    memset(&frame, 0, sizeof(frame));

    frame.AddrPC.Offset = context.Eip;
    frame.AddrFrame.Offset = context.Ebp;
    frame.AddrStack.Offset = context.Esp;

    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;

    SymbolHandlerGuard symbolGuard(hProcess);
    if (!symbolGuard.IsInitialized())
        return std::nullopt;

    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_FAIL_CRITICAL_ERRORS);
    alignas(SYMBOL_INFO) std::uint8_t symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYMBOL_NAME];
    memset(symbolBuffer, 0, sizeof(symbolBuffer));
    PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYMBOL_NAME;

    for (std::size_t frameIndex = 0; frameIndex < MAX_STACK_FRAMES; ++frameIndex)
    {
        BOOL bWalked =
            StackWalk64(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &frame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr);
        if (bWalked == FALSE)
            break;

        if (frame.AddrPC.Offset == 0)
            break;

        StackFrameInfo frameInfo;
        frameInfo.address = frame.AddrPC.Offset;
        frameInfo.isValid = true;

        DWORD64 address = frame.AddrPC.Offset;
        frameInfo.symbolName = std::to_string(address);

        DWORD64 displacement = 0;
        if (SymFromAddr(hProcess, address, &displacement, pSymbol) != FALSE)
        {
            frameInfo.symbolName = pSymbol->Name[0] != '\0' ? pSymbol->Name : "unknown";
        }

        IMAGEHLP_LINE64 lineInfo;
        memset(&lineInfo, 0, sizeof(lineInfo));
        lineInfo.SizeOfStruct = sizeof(lineInfo);
        DWORD lineDisplacement = 0;

        if (SymGetLineFromAddr64(hProcess, address, &lineDisplacement, &lineInfo) != FALSE)
        {
            frameInfo.fileName = lineInfo.FileName ? lineInfo.FileName : "unknown";
            frameInfo.lineNumber = lineInfo.LineNumber;
        }
        else
        {
            frameInfo.fileName = "unknown";
            frameInfo.lineNumber = 0;
        }

        frames.push_back(std::move(frameInfo));
    }

    if (frames.empty())
        return std::nullopt;

    std::vector<std::string> result;
    result.reserve(frames.size());
    for (const auto& frame : frames)
    {
        if (frame.isValid)
        {
            std::string frameStr = frame.symbolName;
            if (!frame.fileName.empty())
            {
                frameStr += " (" + frame.fileName + ":" + std::to_string(frame.lineNumber) + ")";
            }
            result.push_back(frameStr);
        }
    }
    return result;
}

CExceptionInformation_Impl::CExceptionInformation_Impl()
    : m_uiCode(0),
      m_pAddress(nullptr),
      m_szModulePathName(nullptr),
      m_szModuleBaseName(nullptr),
      m_moduleBaseNameStorage(),
      m_uiAddressModuleOffset(0),
      m_ulEAX(0),
      m_ulEBX(0),
      m_ulECX(0),
      m_ulEDX(0),
      m_ulESI(0),
      m_ulEDI(0),
      m_ulEBP(0),
      m_ulESP(0),
      m_ulEIP(0),
      m_ulCS(0),
      m_ulDS(0),
      m_ulES(0),
      m_ulFS(0),
      m_ulGS(0),
      m_ulSS(0),
      m_ulEFlags(0),
      m_stackTrace(),
      m_timestamp(std::chrono::system_clock::now()),
      m_threadId(GetCurrentThreadId()),
      m_processId(GetCurrentProcessId()),
      m_hasDetailedStackTrace(false),
      m_capturedException(nullptr),
      m_uncaughtExceptionCount(std::uncaught_exceptions())
{
}

CExceptionInformation_Impl::~CExceptionInformation_Impl()
{
}

void CExceptionInformation_Impl::Set(std::uint32_t iCode, _EXCEPTION_POINTERS* pException)
{
    if (!ValidateExceptionContext(pException))
    {
        DebugPrintExceptionInfo("Set - Invalid exception context\n");
        return;
    }

    // Allow null context for callback exceptions and other special cases (validated above)
    const bool isCallbackException = (iCode == 0xC000041D);
    if (pException->ContextRecord == nullptr && !isCallbackException)
    {
        DebugPrintExceptionInfo("Set - Null context record (exception type requires context)\n");
        return;
    }
    
    if (pException->ContextRecord == nullptr && isCallbackException)
    {
        DebugPrintExceptionInfo("Set - Null context for callback exception - proceeding with limited info\n");
    }

    if (iCode == 0 || iCode > 0xFFFFFFFF)
    {
        DebugPrintExceptionInfo("Set - Invalid exception code\n");
        return;
    }

    m_uiCode = iCode;
    m_pAddress = pException->ExceptionRecord->ExceptionAddress;
    ClearModulePathState();

    ENHANCED_EXCEPTION_INFO enhancedInfo = {};
    bool                    hasEnhancedInfo = (GetEnhancedExceptionInfo(&enhancedInfo) != FALSE);

    if (hasEnhancedInfo && enhancedInfo.exceptionCode == iCode)
    {
        DebugPrintExceptionInfo("Set - Using enhanced exception info from CrashHandler (FRESH)\n");
        
        // Additional note for callback exceptions even when we have enhanced info
        if (iCode == 0xC000041D)
        {
            DebugPrintExceptionInfo("Set - Callback exception: enhanced info available but stack/module may be incomplete\n");
        }
    }
    else if (hasEnhancedInfo && enhancedInfo.exceptionCode != iCode)
    {
        char mismatchBuffer[DEBUG_BUFFER_SIZE] = {};
        SAFE_DEBUG_PRINT_C(mismatchBuffer, DEBUG_BUFFER_SIZE,
                         "%.*sSet - Exception code mismatch (stored: 0x%08X, current: 0x%08X) - STALE DATA, extracting fresh\n",
                         static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(),
                         enhancedInfo.exceptionCode, iCode);
        hasEnhancedInfo = false;
    }
    else if (!hasEnhancedInfo)
    {
        DebugPrintExceptionInfo("Set - No enhanced info available, extracting manually\n");
    }

    if (hasEnhancedInfo)
    {
        m_uiAddressModuleOffset = enhancedInfo.moduleOffset;
        m_timestamp = enhancedInfo.timestamp;
        m_threadId = enhancedInfo.threadId;
        m_processId = enhancedInfo.processId;
        m_uncaughtExceptionCount = enhancedInfo.uncaughtExceptionCount;
        m_capturedException = enhancedInfo.capturedException.value_or(nullptr);

        if (!enhancedInfo.modulePathName.empty())
        {
            const std::size_t       pathLen = enhancedInfo.modulePathName.length() + 1U;
            std::unique_ptr<char[]> enhancedPathBuffer(new (std::nothrow) char[pathLen]);
            if (!enhancedPathBuffer)
            {
                DebugPrintExceptionInfo("Set - Failed to allocate enhanced module path buffer\n");
            }
            else
            {
                memcpy(enhancedPathBuffer.get(), enhancedInfo.modulePathName.c_str(), pathLen);
                m_szModulePathName = std::move(enhancedPathBuffer);
                UpdateModuleBaseNameFromCurrentPath();
            }
        }

        m_ulEAX = enhancedInfo.eax;
        m_ulEBX = enhancedInfo.ebx;
        m_ulECX = enhancedInfo.ecx;
        m_ulEDX = enhancedInfo.edx;
        m_ulESI = enhancedInfo.esi;
        m_ulEDI = enhancedInfo.edi;
        m_ulEBP = enhancedInfo.ebp;
        m_ulESP = enhancedInfo.esp;
        m_ulEIP = enhancedInfo.eip;
        m_ulCS = enhancedInfo.cs;
        m_ulDS = enhancedInfo.ds;
        m_ulES = enhancedInfo.es;
        m_ulFS = enhancedInfo.fs;
        m_ulGS = enhancedInfo.gs;
        m_ulSS = enhancedInfo.ss;
        m_ulEFlags = enhancedInfo.eflags;

        if (auto trace = enhancedInfo.stackTrace)
        {
            m_stackTrace = *trace;
            m_hasDetailedStackTrace = true;
        }

        return;
    }

    try
    {
        m_timestamp = std::chrono::system_clock::now();
        m_threadId = GetCurrentThreadId();
        m_processId = GetCurrentProcessId();
        m_uncaughtExceptionCount = std::uncaught_exceptions();

        if (std::uncaught_exceptions() > 0)
        {
            try
            {
                m_capturedException = std::current_exception();
            }
            catch (...)
            {
                m_capturedException = nullptr;
            }
        }
        else
        {
            m_capturedException = nullptr;
        }
    }
    catch (...)
    {
        m_threadId = GetCurrentThreadId();
        m_processId = GetCurrentProcessId();
        m_uncaughtExceptionCount = 0;
        m_capturedException = nullptr;
    }

    // Only access ContextRecord if it's not null (can be null for certain exception types)
    if (pException->ContextRecord != nullptr)
    {
        m_ulEAX = pException->ContextRecord->Eax;
        m_ulEBX = pException->ContextRecord->Ebx;
        m_ulECX = pException->ContextRecord->Ecx;
        m_ulEDX = pException->ContextRecord->Edx;
        m_ulESI = pException->ContextRecord->Esi;
        m_ulEDI = pException->ContextRecord->Edi;
        m_ulEBP = pException->ContextRecord->Ebp;
        m_ulESP = pException->ContextRecord->Esp;
        m_ulEIP = pException->ContextRecord->Eip;
        m_ulCS = pException->ContextRecord->SegCs;
        m_ulDS = pException->ContextRecord->SegDs;
        m_ulES = pException->ContextRecord->SegEs;
        m_ulFS = pException->ContextRecord->SegFs;
        m_ulGS = pException->ContextRecord->SegGs;
        m_ulSS = pException->ContextRecord->SegSs;
        m_ulEFlags = pException->ContextRecord->EFlags;
    }
    else
    {
        // No context available - zero out all registers
        DebugPrintExceptionInfo("Set - Null context, zeroing register values\n");
        m_ulEAX = 0;
        m_ulEBX = 0;
        m_ulECX = 0;
        m_ulEDX = 0;
        m_ulESI = 0;
        m_ulEDI = 0;
        m_ulEBP = 0;
        m_ulESP = 0;
        m_ulEIP = 0;
        m_ulCS = 0;
        m_ulDS = 0;
        m_ulES = 0;
        m_ulFS = 0;
        m_ulGS = 0;
        m_ulSS = 0;
        m_ulEFlags = 0;
    }

    std::unique_ptr<char[]> modulePathNameBuffer(new (std::nothrow) char[MAX_MODULE_PATH]);
    std::unique_ptr<char[]> tempModulePathBuffer(new (std::nothrow) char[MAX_MODULE_PATH]);

    if (!modulePathNameBuffer || !tempModulePathBuffer)
    {
        DebugPrintExceptionInfo("Set - Failed to allocate buffers\n");
        return;
    }

    modulePathNameBuffer[0] = '\0';
    tempModulePathBuffer[0] = '\0';

    void* pModuleBaseAddress = nullptr;
    void* pExceptionAddress = m_pAddress;
    void* pQueryAddress = m_pAddress;

    // Special handling for EIP=0: Start from return address at [ESP] instead
    constexpr auto kNullAddress = uintptr_t{0};
    static_assert(kNullAddress == 0, "Null address must be zero");
    
    const auto queryAddressValue = reinterpret_cast<uintptr_t>(pQueryAddress);
    
    if (queryAddressValue == kNullAddress && m_ulEIP == kNullAddress)
    {
        const auto espAddr = static_cast<uintptr_t>(m_ulESP);
        const auto pReturnAddress = reinterpret_cast<void* const*>(espAddr);
        
        char debugBuffer[DEBUG_BUFFER_SIZE] = {};
        SAFE_DEBUG_PRINT_C(debugBuffer, DEBUG_BUFFER_SIZE,
                           "%.*sSet - EIP=0 detected (ESP=0x%08X), attempting to read return address...\n",
                           static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(),
                           static_cast<unsigned int>(espAddr));
        
        if (SharedUtil::IsReadablePointer(pReturnAddress, sizeof(void*)))
        {
            pQueryAddress = *pReturnAddress;
            pExceptionAddress = pQueryAddress;
            
            const auto returnAddressValue = reinterpret_cast<uintptr_t>(pQueryAddress);
            SAFE_DEBUG_PRINT_C(debugBuffer, DEBUG_BUFFER_SIZE,
                               "%.*sSet - Successfully read return address: 0x%08X\n",
                               static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(),
                               static_cast<unsigned int>(returnAddressValue));
        }
        else
        {
            SAFE_DEBUG_PRINT_C(debugBuffer, DEBUG_BUFFER_SIZE,
                               "%.*sSet - Failed to read return address at ESP=0x%08X (not readable)\n",
                               static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(),
                               static_cast<unsigned int>(espAddr));
        }
    }

    for (std::size_t i = 0; i < MAX_STACK_WALK_DEPTH; ++i)
    {
        void* pModuleBaseAddressTemp = nullptr;

        if (pQueryAddress == nullptr || !SharedUtil::IsReadablePointer(pQueryAddress, sizeof(void*)))
        {
            DebugPrintExceptionInfo("Set - Invalid stack address during walk\n");
            break;
        }

        if (!GetModule(pQueryAddress, tempModulePathBuffer.get(), static_cast<int>(MAX_MODULE_PATH), &pModuleBaseAddressTemp))
        {
            DebugPrintExceptionInfo("Set - Failed to get module info\n");
            break;
        }

        const std::string_view tempPathView(tempModulePathBuffer.get());
        auto             tempBaseNamePos = tempPathView.find_last_of('\\');
        const char*      szModuleBaseNameTemp =
            tempBaseNamePos != std::string_view::npos ? tempModulePathBuffer.get() + tempBaseNamePos + 1 : tempModulePathBuffer.get();

        if (szModuleBaseNameTemp == nullptr)
        {
            szModuleBaseNameTemp = tempModulePathBuffer.get();
        }

        if (i == 0)
        {
            const bool copied = CopyModulePathToBuffer(tempModulePathBuffer.get(), modulePathNameBuffer.get(), MAX_MODULE_PATH,
                                                       "Set - Initial module path copy");
            if (!copied)
            {
                DebugPrintExceptionInfo("Set - Initial module path copy failed\n");
            }
            pModuleBaseAddress = pModuleBaseAddressTemp;
            pExceptionAddress = pQueryAddress;
        }

        if (::_strnicmp(szModuleBaseNameTemp, "ntdll", 5) != 0 && ::_strnicmp(szModuleBaseNameTemp, "kernel", 6) != 0 &&
            ::_strnicmp(szModuleBaseNameTemp, "msvc", 4) != 0 && ::_stricmp(szModuleBaseNameTemp, "") != 0)
        {
            const bool copied = CopyModulePathToBuffer(tempModulePathBuffer.get(), modulePathNameBuffer.get(), MAX_MODULE_PATH,
                                                       "Set - Selected module path copy");
            if (!copied)
            {
                DebugPrintExceptionInfo("Set - Selected module path copy failed\n");
            }
            pModuleBaseAddress = pModuleBaseAddressTemp;
            pExceptionAddress = pQueryAddress;
            break;
        }

        const uintptr_t espAddr = static_cast<uintptr_t>(m_ulESP);
        const uintptr_t offset = i * sizeof(void*);

        if (offset > UINTPTR_MAX - espAddr)
        {
            DebugPrintExceptionInfo("Set - Stack address calculation overflow\n");
            break;
        }

        const auto stackAddress = espAddr + offset;
        void**     stackEntry = reinterpret_cast<void**>(stackAddress);

        if (!SharedUtil::IsReadablePointer(stackEntry, sizeof(*stackEntry)))
        {
            DebugPrintExceptionInfo("Set - Stack walk encountered unreadable memory\n");
            break;
        }

        void* stackValue = nullptr;
        if (SharedUtil::IsReadablePointer(stackEntry, sizeof(*stackEntry)))
        {
            stackValue = *stackEntry;
        }

        if (stackValue == nullptr)
        {
            DebugPrintExceptionInfo("Set - Stack walk encountered null pointer\n");
            break;
        }

        pQueryAddress = stackValue;
    }

    m_szModulePathName = std::move(modulePathNameBuffer);
    UpdateModuleBaseNameFromCurrentPath();

    if (pModuleBaseAddress != nullptr && pExceptionAddress != nullptr)
    {
        const uintptr_t baseAddr = reinterpret_cast<uintptr_t>(pModuleBaseAddress);
        const uintptr_t exceptionAddr = reinterpret_cast<uintptr_t>(pExceptionAddress);

        if (exceptionAddr >= baseAddr)
        {
            const uintptr_t offset = exceptionAddr - baseAddr;

            if (offset <= UINT_MAX)
            {
                m_uiAddressModuleOffset = static_cast<uint>(offset);
            }
            else
            {
                m_uiAddressModuleOffset = 0;
                DebugPrintExceptionInfo("Set - Module offset too large\n");
            }
        }
        else
        {
            m_uiAddressModuleOffset = 0;
            DebugPrintExceptionInfo("Set - Exception address before module base\n");
        }
    }
    else
    {
        m_uiAddressModuleOffset = 0;
    }

    try
    {
        if (CaptureUnifiedStackTrace(pException, 0, &m_stackTrace) != FALSE)
        {
            m_hasDetailedStackTrace = true;
            DebugPrintExceptionInfo("Set - Unified stack trace captured successfully\n");
        }
        else
        {
            m_hasDetailedStackTrace = false;
            DebugPrintExceptionInfo("Set - Unified stack trace capture failed\n");
        }
    }
    catch (...)
    {
        m_hasDetailedStackTrace = false;
        DebugPrintExceptionInfo("Set - Stack trace capture failed with exception\n");
    }
}

bool CExceptionInformation_Impl::GetModule(void* pQueryAddress, char* szOutputBuffer, int nOutputNameLength, void** ppModuleBaseAddress)
{
    if (pQueryAddress == nullptr || szOutputBuffer == nullptr || ppModuleBaseAddress == nullptr)
    {
        DebugPrintExceptionInfo("GetModule - Invalid parameters\n");
        return false;
    }

    if (nOutputNameLength <= 0)
    {
        DebugPrintExceptionInfo("GetModule - Invalid buffer length\n");
        return false;
    }

    szOutputBuffer[0] = '\0';
    *ppModuleBaseAddress = nullptr;

    using GetModuleHandleExA_t = BOOL(WINAPI*)(DWORD, LPCSTR, HMODULE*);

    HMODULE hKern32 = GetModuleHandleA("kernel32.dll");
    if (hKern32 == nullptr)
    {
        DebugPrintExceptionInfo("GetModule - Failed to get kernel32 handle\n");
        return false;
    }

    GetModuleHandleExA_t pfnGetModuleHandleExA = nullptr;
    if (!SharedUtil::TryGetProcAddress(hKern32, "GetModuleHandleExA", pfnGetModuleHandleExA))
    {
        DebugPrintExceptionInfo("GetModule - GetModuleHandleExA not available\n");
        return false;
    }

    HMODULE hModule = nullptr;
    
    // Wrap in __try for exception addresses that may be invalid/in guard pages/trampolines
    // (callbacks, corrupted stacks, etc. can point to invalid memory)
    __try
    {
        if (pfnGetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, static_cast<LPCSTR>(pQueryAddress),
                                  &hModule) == 0)
        {
            const DWORD                         dwErrorHandle = GetLastError();
            char debugBuffer[DEBUG_BUFFER_SIZE] = {};
            SAFE_DEBUG_PRINT_C(debugBuffer, DEBUG_BUFFER_SIZE, "%.*sGetModule - GetModuleHandleExA failed (0x%08X)\n",
                               static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(), dwErrorHandle);
            
            // Address may be a system trampoline or invalid memory - don't proceed
            return false;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        char debugBuffer[DEBUG_BUFFER_SIZE] = {};
        SAFE_DEBUG_PRINT_C(debugBuffer, DEBUG_BUFFER_SIZE, "%.*sGetModule - Exception accessing address 0x%p (invalid/trampoline address)\n",
                           static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(), pQueryAddress);
        // Don't proceed with invalid module info
        return false;
    }

    *ppModuleBaseAddress = hModule;

    const DWORD dwResult = GetModuleFileNameA(hModule, szOutputBuffer, nOutputNameLength);
    if (dwResult == 0)
    {
        const DWORD                         dwErrorFileName = GetLastError();
        char debugBuffer[DEBUG_BUFFER_SIZE] = {};
        SAFE_DEBUG_PRINT_C(debugBuffer, DEBUG_BUFFER_SIZE, "%.*sGetModule - GetModuleFileNameA failed (0x%08X)\n",
                           static_cast<int>(DEBUG_PREFIX_EXCEPTION_INFO.size()), DEBUG_PREFIX_EXCEPTION_INFO.data(), dwErrorFileName);
        szOutputBuffer[0] = '\0';
        return false;
    }

    if (static_cast<int>(dwResult) >= nOutputNameLength)
    {
        szOutputBuffer[nOutputNameLength - 1] = '\0';
        DebugPrintExceptionInfo("GetModule - Module path truncated\n");
    }

    if (szOutputBuffer != nullptr && strlen(szOutputBuffer) == 0)
    {
        DebugPrintExceptionInfo("GetModule - Empty module path\n");
        return false;
    }

    return true;
}

struct ExceptionDetails
{
    DWORD            code;
    void*            address;
    std::string_view description;
    bool             isValid;
};

[[nodiscard]] static ExceptionDetails GetExceptionDetails(_EXCEPTION_POINTERS* pException)
{
    if (pException == nullptr || pException->ExceptionRecord == nullptr)
    {
        return ExceptionDetails{0, nullptr, "Invalid exception", false};
    }

    const auto& record = *pException->ExceptionRecord;
    return ExceptionDetails{record.ExceptionCode, record.ExceptionAddress, "Valid exception", true};
}

std::optional<std::vector<std::string>> CExceptionInformation_Impl::GetStackTrace() const
{
    if (!m_hasDetailedStackTrace || m_stackTrace.empty())
        return std::nullopt;

    return m_stackTrace;
}

std::optional<std::chrono::system_clock::time_point> CExceptionInformation_Impl::GetTimestamp() const
{
    return m_timestamp;
}

DWORD CExceptionInformation_Impl::GetThreadId() const
{
    return m_threadId;
}

DWORD CExceptionInformation_Impl::GetProcessId() const
{
    return m_processId;
}

std::string CExceptionInformation_Impl::GetExceptionDescription() const
{
    std::string description;
    description.reserve(256);

    char buffer[DEBUG_BUFFER_SIZE] = {};
    SAFE_DEBUG_PRINT_C(buffer, DEBUG_BUFFER_SIZE, "Exception 0x%08X", m_uiCode);
    description = buffer;

    if (m_szModuleBaseName != nullptr)
    {
        description += " in ";
        description += m_szModuleBaseName;
        if (m_uiAddressModuleOffset > 0)
        {
            SAFE_DEBUG_PRINT_C(buffer, DEBUG_BUFFER_SIZE, "+0x%08X", m_uiAddressModuleOffset);
            description += buffer;
        }
    }

    SAFE_DEBUG_PRINT_C(buffer, DEBUG_BUFFER_SIZE, " (Thread: %u)", m_threadId);
    description += buffer;

    return description;
}

bool CExceptionInformation_Impl::HasDetailedStackTrace() const
{
    return m_hasDetailedStackTrace;
}

std::optional<std::exception_ptr> CExceptionInformation_Impl::GetCapturedException() const
{
    return m_capturedException ? std::optional{m_capturedException} : std::nullopt;
}

int CExceptionInformation_Impl::GetUncaughtExceptionCount() const
{
    return m_uncaughtExceptionCount;
}

void CExceptionInformation_Impl::CaptureCurrentException()
{
    const int uncaught = std::uncaught_exceptions();
    if (uncaught <= 0)
    {
        m_capturedException = nullptr;
        m_uncaughtExceptionCount = 0;
        return;
    }

    try
    {
        m_capturedException = std::current_exception();
    }
    catch (...)
    {
        m_capturedException = nullptr;
    }

    m_uncaughtExceptionCount = uncaught;
}

void CExceptionInformation_Impl::UpdateModuleBaseNameFromCurrentPath()
{
    m_moduleBaseNameStorage.clear();
    m_szModuleBaseName = nullptr;

    if (!m_szModulePathName)
        return;

    const char* path = m_szModulePathName.get();
    if (path == nullptr || path[0] == '\0')
        return;

    std::string_view pathView(path);
    const auto       separatorPos = pathView.find_last_of("\\/");
    std::string_view baseView = separatorPos != std::string_view::npos ? pathView.substr(separatorPos + 1) : pathView;
    if (baseView.empty())
        return;

    m_moduleBaseNameStorage.assign(baseView.begin(), baseView.end());
    m_szModuleBaseName = m_moduleBaseNameStorage.c_str();
}

void CExceptionInformation_Impl::ClearModulePathState()
{
    m_szModulePathName.reset();
    m_moduleBaseNameStorage.clear();
    m_szModuleBaseName = nullptr;
}

[[nodiscard]] static std::optional<std::string> GetSafeModulePath(std::string_view modulePath)
{
    try
    {
        std::filesystem::path path(modulePath);

        if (!std::filesystem::exists(path))
        {
            return std::nullopt;
        }

        auto absolutePath = std::filesystem::absolute(path);

        return absolutePath.string();
    }
    catch (...)
    {
        return std::nullopt;
    }
}

[[nodiscard]] static std::optional<std::vector<uint8_t>> CaptureMemoryDump(void* address, std::size_t size)
{
    if (address == nullptr || size == 0)
        return std::nullopt;

    if (!SharedUtil::IsReadablePointer(address, size))
    {
        DebugPrintExceptionInfo("CaptureMemoryDump - Memory region not readable\n");
        return std::nullopt;
    }

    try
    {
        std::vector<uint8_t> memoryDump(size);
        const auto*          source = static_cast<const uint8_t*>(address);
        std::memcpy(memoryDump.data(), source, size);
        return memoryDump;
    }
    catch (...)
    {
        DebugPrintExceptionInfo("CaptureMemoryDump - Failed to allocate buffer\n");
        return std::nullopt;
    }
}

