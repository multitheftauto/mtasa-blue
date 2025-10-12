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

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <exception>
#include <errno.h>
#include <filesystem>
#include <intrin.h>
#include <memory>
#include <new>
#include <mutex>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>

#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

[[nodiscard]] static bool ValidateExceptionContext(_EXCEPTION_POINTERS* pException) noexcept
{
    if (pException == nullptr)
        return false;

    if (pException->ExceptionRecord == nullptr)
        return false;

    if (pException->ContextRecord == nullptr)
        return false;

    const EXCEPTION_RECORD* pRecord = pException->ExceptionRecord;
    if (pRecord->ExceptionCode == 0)
        return false;

    const CONTEXT* pContext = pException->ContextRecord;
    if (pContext->ContextFlags == 0)
        return false;

    if (pContext->ContextFlags & CONTEXT_CONTROL)
    {
        if (pContext->Eip == 0 || pContext->Esp == 0)
        {
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "ValidateExceptionContext - Invalid control registers\n");
            return false;
        }
    }

    if (pRecord->ExceptionAddress == nullptr)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "ValidateExceptionContext - Null exception address\n");
        return false;
    }

    return true;
}

constexpr std::size_t MAX_MODULE_PATH = 512;
constexpr std::size_t MAX_STACK_WALK_DEPTH = 16;
constexpr std::size_t MAX_STACK_FRAMES = 32;
constexpr std::size_t MAX_SYMBOL_NAME = 256;

[[nodiscard]] static bool CopyModulePathToBuffer(const char* source, char* destination, std::size_t destinationSize, const char* context) noexcept
{
    if (source == nullptr || destination == nullptr || destinationSize == 0U)
        return false;

    const errno_t copyResult = strncpy_s(destination, destinationSize, source, _TRUNCATE);
    if (copyResult == STRUNCATE)
    {
        std::array<char, DEBUG_BUFFER_SIZE> buffer{};
        SAFE_DEBUG_PRINT(buffer, DEBUG_PREFIX_EXCEPTION_INFO "%s - Module path truncated to %zu bytes\n", context, destinationSize - 1U);
        return true;
    }

    if (copyResult != 0)
    {
        std::array<char, DEBUG_BUFFER_SIZE> buffer{};
        SAFE_DEBUG_PRINT(buffer, DEBUG_PREFIX_EXCEPTION_INFO "%s - strncpy_s failed (error=%d)\n", context, copyResult);
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
    explicit SymbolHandlerGuard(HANDLE process) noexcept : m_process(process), m_initialized(false), m_uncaughtExceptions(std::uncaught_exceptions())
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
                std::array<char, DEBUG_BUFFER_SIZE> debugBuffer{};
                SAFE_DEBUG_PRINT(debugBuffer, DEBUG_PREFIX_EXCEPTION_INFO "SymbolHandlerGuard: SymInitialize failed, error: %u\n", dwError);

                if (dwError == ERROR_NOT_ENOUGH_MEMORY)
                {
                    SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "SymbolHandlerGuard: Insufficient memory for symbol initialization\n");
                }
                else if (dwError == ERROR_ACCESS_DENIED)
                {
                    SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "SymbolHandlerGuard: Access denied for symbol initialization\n");
                }
            }
        }
    }

    ~SymbolHandlerGuard() noexcept
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

    bool IsInitialized() const noexcept { return m_initialized; }

private:
    HANDLE m_process;
    bool   m_initialized;
    int    m_uncaughtExceptions;
};

[[nodiscard]] static std::optional<std::vector<std::string>> CaptureEnhancedStackTrace(_EXCEPTION_POINTERS* pException) noexcept
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

CExceptionInformation_Impl::CExceptionInformation_Impl() noexcept
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

CExceptionInformation_Impl::~CExceptionInformation_Impl() noexcept
{
}

void CExceptionInformation_Impl::Set(std::uint32_t iCode, _EXCEPTION_POINTERS* pException) noexcept
{
    if (!ValidateExceptionContext(pException))
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Invalid exception context\n");
        return;
    }

    if (pException->ContextRecord == nullptr)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Null context record\n");
        return;
    }

    if (iCode == 0 || iCode > 0xFFFFFFFF)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Invalid exception code\n");
        return;
    }

    m_uiCode = iCode;
    m_pAddress = pException->ExceptionRecord->ExceptionAddress;
    ClearModulePathState();

    ENHANCED_EXCEPTION_INFO enhancedInfo = {};
    bool                    hasEnhancedInfo = (GetEnhancedExceptionInfo(&enhancedInfo) != FALSE);

    if (hasEnhancedInfo && enhancedInfo.exceptionCode == iCode)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Using enhanced exception info from CrashHandler (FRESH)\n");
    }
    else if (hasEnhancedInfo && enhancedInfo.exceptionCode != iCode)
    {
        std::array<char, DEBUG_BUFFER_SIZE> mismatchBuffer{};
        SAFE_DEBUG_PRINT(mismatchBuffer,
                         DEBUG_PREFIX_EXCEPTION_INFO "Set - Exception code mismatch (stored: 0x%08X, current: 0x%08X) - STALE DATA, extracting fresh\n",
                         enhancedInfo.exceptionCode, iCode);
        hasEnhancedInfo = false;
    }
    else if (!hasEnhancedInfo)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - No enhanced info available, extracting manually\n");
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
                SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Failed to allocate enhanced module path buffer\n");
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

    std::unique_ptr<char[]> modulePathNameBuffer(new (std::nothrow) char[MAX_MODULE_PATH]);
    std::unique_ptr<char[]> tempModulePathBuffer(new (std::nothrow) char[MAX_MODULE_PATH]);

    if (!modulePathNameBuffer || !tempModulePathBuffer)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Failed to allocate buffers\n");
        return;
    }

    modulePathNameBuffer[0] = '\0';
    tempModulePathBuffer[0] = '\0';

    void* pModuleBaseAddress = nullptr;
    void* pExceptionAddress = m_pAddress;
    void* pQueryAddress = m_pAddress;

    for (std::size_t i = 0; i < MAX_STACK_WALK_DEPTH; ++i)
    {
        void* pModuleBaseAddressTemp = nullptr;

        if (pQueryAddress == nullptr || !SharedUtil::IsReadablePointer(pQueryAddress, sizeof(void*)))
        {
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Invalid stack address during walk\n");
            break;
        }

        if (!GetModule(pQueryAddress, tempModulePathBuffer.get(), static_cast<int>(MAX_MODULE_PATH), &pModuleBaseAddressTemp))
        {
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Failed to get module info\n");
            break;
        }

        std::string_view tempPathView(tempModulePathBuffer.get());
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
                SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Initial module path copy failed\n");
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
                SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Selected module path copy failed\n");
            }
            pModuleBaseAddress = pModuleBaseAddressTemp;
            pExceptionAddress = pQueryAddress;
            break;
        }

        const uintptr_t espAddr = static_cast<uintptr_t>(m_ulESP);
        const uintptr_t offset = i * sizeof(void*);

        if (offset > UINTPTR_MAX - espAddr)
        {
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Stack address calculation overflow\n");
            break;
        }

        const auto stackAddress = espAddr + offset;
        void**     stackEntry = reinterpret_cast<void**>(stackAddress);

        if (!SharedUtil::IsReadablePointer(stackEntry, sizeof(*stackEntry)))
        {
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Stack walk encountered unreadable memory\n");
            break;
        }

        void* stackValue = nullptr;
        if (SharedUtil::IsReadablePointer(stackEntry, sizeof(*stackEntry)))
        {
            stackValue = *stackEntry;
        }

        if (stackValue == nullptr)
        {
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Stack walk encountered null pointer\n");
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
                SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Module offset too large\n");
            }
        }
        else
        {
            m_uiAddressModuleOffset = 0;
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Exception address before module base\n");
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
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Unified stack trace captured successfully\n");
        }
        else
        {
            m_hasDetailedStackTrace = false;
            SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Unified stack trace capture failed\n");
        }
    }
    catch (...)
    {
        m_hasDetailedStackTrace = false;
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "Set - Stack trace capture failed with exception\n");
    }
}

bool CExceptionInformation_Impl::GetModule(void* pQueryAddress, char* szOutputBuffer, int nOutputNameLength, void** ppModuleBaseAddress) noexcept
{
    if (pQueryAddress == nullptr || szOutputBuffer == nullptr || ppModuleBaseAddress == nullptr)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "GetModule - Invalid parameters\n");
        return false;
    }

    if (nOutputNameLength <= 0)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "GetModule - Invalid buffer length\n");
        return false;
    }

    szOutputBuffer[0] = '\0';
    *ppModuleBaseAddress = nullptr;

    using GetModuleHandleExA_t = BOOL(WINAPI*)(DWORD, LPCSTR, HMODULE*);

    HMODULE hKern32 = GetModuleHandleA("kernel32.dll");
    if (hKern32 == nullptr)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "GetModule - Failed to get kernel32 handle\n");
        return false;
    }

    GetModuleHandleExA_t pfnGetModuleHandleExA = nullptr;
    if (!SharedUtil::TryGetProcAddress(hKern32, "GetModuleHandleExA", pfnGetModuleHandleExA))
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "GetModule - GetModuleHandleExA not available\n");
        return false;
    }

    HMODULE hModule = nullptr;
    if (pfnGetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, static_cast<LPCSTR>(pQueryAddress),
                              &hModule) == 0)
    {
        const DWORD                         dwErrorHandle = GetLastError();
        std::array<char, DEBUG_BUFFER_SIZE> debugBuffer{};
        SAFE_DEBUG_PRINT(debugBuffer, DEBUG_PREFIX_EXCEPTION_INFO "GetModule - GetModuleHandleExA failed (0x%08X)\n", dwErrorHandle);
        return false;
    }

    *ppModuleBaseAddress = hModule;

    const DWORD dwResult = GetModuleFileNameA(hModule, szOutputBuffer, nOutputNameLength);
    if (dwResult == 0)
    {
        const DWORD                         dwErrorFileName = GetLastError();
        std::array<char, DEBUG_BUFFER_SIZE> debugBuffer{};
        SAFE_DEBUG_PRINT(debugBuffer, DEBUG_PREFIX_EXCEPTION_INFO "GetModule - GetModuleFileNameA failed (0x%08X)\n", dwErrorFileName);
        szOutputBuffer[0] = '\0';
        return false;
    }

    if (static_cast<int>(dwResult) >= nOutputNameLength)
    {
        szOutputBuffer[nOutputNameLength - 1] = '\0';
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "GetModule - Module path truncated\n");
    }

    if (szOutputBuffer != nullptr && strlen(szOutputBuffer) == 0)
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "GetModule - Empty module path\n");
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

[[nodiscard]] static ExceptionDetails GetExceptionDetails(_EXCEPTION_POINTERS* pException) noexcept
{
    if (pException == nullptr || pException->ExceptionRecord == nullptr)
    {
        return ExceptionDetails{0, nullptr, "Invalid exception", false};
    }

    const auto& record = *pException->ExceptionRecord;
    return ExceptionDetails{record.ExceptionCode, record.ExceptionAddress, "Valid exception", true};
}

std::optional<std::vector<std::string>> CExceptionInformation_Impl::GetStackTrace() const noexcept
{
    if (!m_hasDetailedStackTrace || m_stackTrace.empty())
        return std::nullopt;

    return m_stackTrace;
}

std::optional<std::chrono::system_clock::time_point> CExceptionInformation_Impl::GetTimestamp() const noexcept
{
    return m_timestamp;
}

DWORD CExceptionInformation_Impl::GetThreadId() const noexcept
{
    return m_threadId;
}

DWORD CExceptionInformation_Impl::GetProcessId() const noexcept
{
    return m_processId;
}

std::string CExceptionInformation_Impl::GetExceptionDescription() const noexcept
{
    std::string description;
    description.reserve(256);

    std::array<char, DEBUG_BUFFER_SIZE> buffer{};
    SAFE_DEBUG_PRINT(buffer, "Exception 0x%08X", m_uiCode);
    description = buffer.data();

    if (m_szModuleBaseName != nullptr)
    {
        description += " in ";
        description += m_szModuleBaseName;
        if (m_uiAddressModuleOffset > 0)
        {
            SAFE_DEBUG_PRINT(buffer, "+0x%08X", m_uiAddressModuleOffset);
            description += buffer.data();
        }
    }

    SAFE_DEBUG_PRINT(buffer, " (Thread: %u)", m_threadId);
    description += buffer.data();

    return description;
}

bool CExceptionInformation_Impl::HasDetailedStackTrace() const noexcept
{
    return m_hasDetailedStackTrace;
}

std::optional<std::exception_ptr> CExceptionInformation_Impl::GetCapturedException() const noexcept
{
    return m_capturedException ? std::optional{m_capturedException} : std::nullopt;
}

int CExceptionInformation_Impl::GetUncaughtExceptionCount() const noexcept
{
    return m_uncaughtExceptionCount;
}

void CExceptionInformation_Impl::CaptureCurrentException() noexcept
{
    if (std::uncaught_exceptions() > 0)
    {
        try
        {
            m_capturedException = std::current_exception();
            m_uncaughtExceptionCount = std::uncaught_exceptions();
        }
        catch (...)
        {
            m_capturedException = nullptr;
            try
            {
                m_uncaughtExceptionCount = std::uncaught_exceptions();
            }
            catch (...)
            {
                m_uncaughtExceptionCount = 0;
            }
        }
    }
    else
    {
        m_capturedException = nullptr;
        m_uncaughtExceptionCount = 0;
    }
}

void CExceptionInformation_Impl::UpdateModuleBaseNameFromCurrentPath() noexcept
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

void CExceptionInformation_Impl::ClearModulePathState() noexcept
{
    m_szModulePathName.reset();
    m_moduleBaseNameStorage.clear();
    m_szModuleBaseName = nullptr;
}

[[nodiscard]] static std::optional<std::string> GetSafeModulePath(std::string_view modulePath) noexcept
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

[[nodiscard]] static std::optional<std::vector<uint8_t>> CaptureMemoryDump(void* address, std::size_t size) noexcept
{
    if (address == nullptr || size == 0)
        return std::nullopt;

    if (!SharedUtil::IsReadablePointer(address, size))
    {
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "CaptureMemoryDump - Memory region not readable\n");
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
        SafeDebugOutput(DEBUG_PREFIX_EXCEPTION_INFO "CaptureMemoryDump - Failed to allocate buffer\n");
        return std::nullopt;
    }
}
