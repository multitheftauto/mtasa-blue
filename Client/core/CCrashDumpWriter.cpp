/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCrashDumpWriter.cpp
 *  PURPOSE:     Crash dump generation and crash dialog
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <SharedUtil.Misc.h>
#include <game/CGame.h>
#include <game/CPools.h>
#include <game/CRenderWare.h>
#include <multiplayer/CMultiplayer.h>
#include <core/CClientBase.h>
#include "CrashHandler.h"

#include <process.h>
#include <DbgHelp.h>

#include <atomic>
#include <array>
#include <vector>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <limits>
#include <ctime>
#include <optional>
#include <utility>

static constexpr DWORD       CRASH_EXIT_CODE = 3;
static constexpr std::size_t LOG_EVENT_SIZE = 200;
static constexpr uint        INVALID_PARAMETER_WARNING_THRESHOLD = 5;
static constexpr std::size_t INVALID_PARAMETER_SAMPLE_LIMIT = 8;
static constexpr std::size_t MAX_FALLBACK_STACK_FRAMES = 32;
static constexpr int         MAX_WIDE_TO_UTF8_BYTES = 1 * 1024 * 1024;

static constexpr DWORD       PROCESS_WAIT_TIMEOUT_MS = 500;
static constexpr DWORD       WINDOW_POLL_TIMEOUT_MS = 100;
static constexpr std::size_t MAX_WINDOW_POLL_ATTEMPTS = 30;
static constexpr std::size_t SHELL_EXEC_POLL_ATTEMPTS = 20;
static constexpr DWORD       ALT_KEY_DURATION_MS = 50;
static constexpr int         SCREEN_MARGIN_PIXELS = 50;
static constexpr int         EMERGENCY_MSGBOX_WIDTH = 600;
static constexpr int         EMERGENCY_MSGBOX_HEIGHT = 200;

#define SAFE_DEBUG_OUTPUT(msg) SafeDebugOutput(msg)

class CClientBase;

static bool SafeReadGameByte(uintptr_t address, unsigned char& outValue) noexcept
{
    __try
    {
        outValue = *reinterpret_cast<unsigned char*>(address);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

static bool InvokeClientHandleExceptionSafe(CClientBase* pClient, CExceptionInformation_Impl* pExceptionInformation, bool& outHandled) noexcept
{
    outHandled = false;

    if (pClient == nullptr || pExceptionInformation == nullptr)
        return false;

    __try
    {
        outHandled = pClient->HandleException(pExceptionInformation);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: DOUBLE-FAULT in exception handler\n");
        return false;
    }
    return true;
}

namespace
{
    void ConfigureDbgHelpOptions() noexcept
    {
        static std::atomic_flag configured = ATOMIC_FLAG_INIT;
        if (!configured.test_and_set(std::memory_order_acq_rel))
        {
            SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_FAIL_CRITICAL_ERRORS);
        }
    }
}            // namespace

class SymbolHandlerGuard
{
public:
    explicit SymbolHandlerGuard(HANDLE process) noexcept : m_process(process), m_initialized(false)
    {
        if (m_process != nullptr)
        {
            ConfigureDbgHelpOptions();

            if (SymInitialize(m_process, nullptr, TRUE) != FALSE)
                m_initialized = true;
        }
    }

    ~SymbolHandlerGuard() noexcept
    {
        if (m_initialized)
            SymCleanup(m_process);
    }

    SymbolHandlerGuard(const SymbolHandlerGuard&) = delete;
    SymbolHandlerGuard& operator=(const SymbolHandlerGuard&) = delete;
    SymbolHandlerGuard(SymbolHandlerGuard&&) = delete;
    SymbolHandlerGuard& operator=(SymbolHandlerGuard&&) = delete;

    bool IsInitialized() const noexcept { return m_initialized; }

private:
    HANDLE m_process;
    bool   m_initialized;
};

[[nodiscard]] static bool NormalizePathForValidation(const SString& inputPath, SString& outputPath)
{
    if (inputPath.empty())
        return false;

    const auto required = GetFullPathNameA(inputPath.c_str(), 0, nullptr, nullptr);
    if (required == 0)
        return false;

    std::vector<char> buffer(static_cast<std::size_t>(required) + 1u, '\0');
    const auto        written = GetFullPathNameA(inputPath.c_str(), required + 1u, buffer.data(), nullptr);
    if (written == 0 || written > required)
        return false;

    outputPath = buffer.data();
    for (std::size_t i = 0; i < outputPath.length(); ++i)
    {
        if (outputPath[i] == '/')
            outputPath[i] = '\\';
    }

    return true;
}

static void EnsureTrailingBackslash(SString& path)
{
    if (path.empty())
        return;

    if (!path.EndsWith("\\"))
        path += "\\";
}

[[nodiscard]] static bool ValidatePathWithinBase(const SString& baseDir, const SString& candidatePath, DWORD logId)
{
    SString normalizedBase;
    if (!NormalizePathForValidation(baseDir, normalizedBase))
    {
        AddReportLog(logId, SString("CCrashDumpWriter::ValidatePathWithinBase - failed to normalize base (%s)", baseDir.c_str()));
        return false;
    }

    EnsureTrailingBackslash(normalizedBase);

    SString normalizedCandidate;
    if (!NormalizePathForValidation(candidatePath, normalizedCandidate))
    {
        AddReportLog(logId, SString("CCrashDumpWriter::ValidatePathWithinBase - failed to normalize candidate (%s)", candidatePath.c_str()));
        return false;
    }

    const std::size_t baseLength = normalizedBase.length();
    if (baseLength == 0 || normalizedCandidate.length() < baseLength)
        return false;

    if (_strnicmp(normalizedCandidate.c_str(), normalizedBase.c_str(), baseLength) != 0)
        return false;

    return true;
}

struct SLogEventInfo
{
    uint    uiTickCount;
    SString strType;
    SString strContext;
    SString strBody;
};

struct SLogEventLine
{
    SString strBody;
    SString strType;
    SString strContext;
    void    operator+=(const char* szAppend) { strBody += szAppend; }
    bool    operator==(const SLogEventLine& other) const { return strBody == other.strBody && strType == other.strType && strContext == other.strContext; }
};

struct SCrashAvertedInfo
{
    uint uiTickCount;
    int  uiUsageCount;
};

static std::list<SLogEventInfo>            ms_LogEventList;
static CDuplicateLineFilter<SLogEventLine> ms_LogEventFilter;
static std::map<int, SCrashAvertedInfo>    ms_CrashAvertedMap;
static uint                                ms_uiTickCountBase = 0;
static void*                               ms_pReservedMemory = nullptr;
static uint                                ms_uiInCrashZone = 0;

static std::atomic<uint>                                   ms_uiInvalidParameterCount{0};
static std::atomic<uint>                                   ms_uiInvalidParameterCountLogged{0};
static std::atomic<bool>                                   ms_bInCrashHandler{false};
static std::array<SString, INVALID_PARAMETER_SAMPLE_LIMIT> ms_InvalidParameterSamples;
static std::atomic<uint>                                   ms_uiInvalidParameterSampleCount{0};
static std::atomic<bool>                                   ms_bInvalidParameterWarningStored{false};
static std::atomic<bool>                                   ms_bFallbackStackLogged{false};
static HANDLE                                              ms_hCrashDialogProcess = nullptr;

[[nodiscard]] static std::array<SString, 2> BuildCrashDialogCandidates()
{
    const SString          basePath = GetMTASABaseDir();
    std::array<SString, 2> candidates = {SharedUtil::PathJoin(basePath, "Multi Theft Auto.exe"), SharedUtil::PathJoin(basePath, "Multi Theft Auto_d.exe")};

    return candidates;
}

[[nodiscard]] static inline constexpr bool IsValidHandle(HANDLE handle) noexcept
{
    return handle != nullptr && handle != INVALID_HANDLE_VALUE;
}

static void EnsureCrashReasonForDialog(CExceptionInformation* pExceptionInformation) noexcept
{
    if (pExceptionInformation == nullptr)
    {
        SAFE_DEBUG_OUTPUT("EnsureCrashReasonForDialog: NULL exception information\n");
        SetApplicationSetting("diagnostics", "last-crash-reason", "Unknown crash - no exception information");
        return;
    }

    SString strReason = GetApplicationSetting("diagnostics", "last-crash-reason");

    if (strReason.empty())
    {
        const unsigned int code = pExceptionInformation->GetCode();

        SString exceptionType;
        switch (code)
        {
            case EXCEPTION_ACCESS_VIOLATION:
                exceptionType = "Access Violation - Invalid memory access";
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                exceptionType = "Array Bounds Exceeded";
                break;
            case EXCEPTION_BREAKPOINT:
                exceptionType = "Breakpoint Hit";
                break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
                exceptionType = "Data Type Misalignment";
                break;
            case EXCEPTION_FLT_DENORMAL_OPERAND:
                exceptionType = "Floating Point - Denormal Operand";
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                exceptionType = "Floating Point - Divide by Zero";
                break;
            case EXCEPTION_FLT_INEXACT_RESULT:
                exceptionType = "Floating Point - Inexact Result";
                break;
            case EXCEPTION_FLT_INVALID_OPERATION:
                exceptionType = "Floating Point - Invalid Operation";
                break;
            case EXCEPTION_FLT_OVERFLOW:
                exceptionType = "Floating Point - Overflow";
                break;
            case EXCEPTION_FLT_STACK_CHECK:
                exceptionType = "Floating Point - Stack Check";
                break;
            case EXCEPTION_FLT_UNDERFLOW:
                exceptionType = "Floating Point - Underflow";
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                exceptionType = "Illegal Instruction";
                break;
            case EXCEPTION_IN_PAGE_ERROR:
                exceptionType = "In Page Error - Page fault";
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
                exceptionType = "Non-Continuable Exception";
                break;
            case EXCEPTION_PRIV_INSTRUCTION:
                exceptionType = "Privileged Instruction";
                break;
            case EXCEPTION_SINGLE_STEP:
                exceptionType = "Single Step (Debugging)";
                break;
            case EXCEPTION_STACK_OVERFLOW:
                exceptionType = "Stack Overflow";
                break;
            case EXCEPTION_GUARD_PAGE:
                exceptionType = "Guard Page Violation";
                break;
            case EXCEPTION_INVALID_HANDLE:
                exceptionType = "Invalid Handle";
                break;
            case CPP_EXCEPTION_CODE:
                exceptionType = "C++ Exception";
                break;
            case STATUS_STACK_BUFFER_OVERRUN_CODE:
                exceptionType = "Stack Buffer Overrun (Security Check)";
                break;
            case STATUS_INVALID_CRUNTIME_PARAMETER_CODE:
                exceptionType = "Invalid C Runtime Parameter";
                break;
            case CUSTOM_EXCEPTION_CODE_OOM:
                exceptionType = "Out of Memory - Allocation Failure";
                break;
            case CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT:
                exceptionType = "Watchdog Timeout - Thread Freeze Detected";
                break;
            case 0xC0000374:
                exceptionType = "Heap Corruption Detected";
                break;
            case 0xC0000420:
                exceptionType = "Assertion Failure";
                break;
            case 0x40000015:
                exceptionType = "Fatal App Exit";
                break;
            case 0xC00002C9:
                exceptionType = "Register NaT Consumption";
                break;
            case 0xC0000194:
                exceptionType = "Possible Deadlock Condition";
                break;
            case 0x80000029:
                exceptionType = "Unwind Consolidate (Frame consolidation)";
                break;
            default:
                exceptionType = SString("Exception 0x%08X", code);
                break;
        }

        const char* moduleBaseName = pExceptionInformation->GetModuleBaseName();
        if (moduleBaseName == nullptr || moduleBaseName[0] == '\0')
            moduleBaseName = pExceptionInformation->GetModulePathName();
        if (moduleBaseName == nullptr || moduleBaseName[0] == '\0')
            moduleBaseName = "unknown";

        strReason = SString("\n\nReason: %s at %s+0x%08X", exceptionType.c_str(), moduleBaseName, pExceptionInformation->GetAddressModuleOffset());

        if (ms_uiInCrashZone != 0)
            strReason += SString(" (CrashZone=%u)", ms_uiInCrashZone);

        const uint invalidParameterCount = ms_uiInvalidParameterCount.load(std::memory_order_relaxed);
        if (invalidParameterCount > 0)
            strReason += SString(" (InvalidParameterCount=%u)", invalidParameterCount);

        strReason += SString(" (ThreadId=%u)", GetCurrentThreadId());

        SAFE_DEBUG_OUTPUT(SString("EnsureCrashReasonForDialog: %s\n", strReason.c_str()).c_str());
    }
    else
    {
        SAFE_DEBUG_OUTPUT(SString("EnsureCrashReasonForDialog: Using existing reason: %s\n", strReason.c_str()).c_str());
    }

    SString pendingWarning = GetApplicationSetting("diagnostics", "pending-invalid-parameter-warning");
    if (!pendingWarning.empty())
        strReason += pendingWarning;

    SetApplicationSetting("diagnostics", "last-crash-reason", strReason);
}

[[nodiscard]] static SString WideToUtf8(const wchar_t* input)
{
    if (input == nullptr || input[0] == L'\0')
        return SString();

    const auto required = WideCharToMultiByte(CP_UTF8, 0, input, -1, nullptr, 0, nullptr, nullptr);
    if (required <= 0)
        return SString();

    if (required > MAX_WIDE_TO_UTF8_BYTES)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::WideToUtf8 - input too large for UTF-8 conversion\n");
        AddReportLog(9212, SString("CCrashDumpWriter::WideToUtf8 - conversion request exceeds %d bytes", MAX_WIDE_TO_UTF8_BYTES));
        return SString();
    }

    std::vector<char> buffer(static_cast<std::size_t>(required));
    const auto        converted = WideCharToMultiByte(CP_UTF8, 0, input, -1, buffer.data(), required, nullptr, nullptr);
    if (converted <= 0)
        return SString();

    const char* convertedText = buffer.data();
    return SString(std::string(convertedText));
}

static void AppendCrashDiagnostics(const SString& text)
{
    if (text.empty())
        return;

    FILE* pFile = File::Fopen(CalcMTASAPath("mta\\core.log"), "a+");
    if (pFile != nullptr)
    {
        fprintf(pFile, "%s", text.c_str());
        fclose(pFile);
    }

    SString strInfo = GetApplicationSetting("diagnostics", "last-crash-info");
    strInfo += text;
    SetApplicationSetting("diagnostics", "last-crash-info", strInfo);

    WriteDebugEvent(text.Replace("\n", " "));
}

[[nodiscard]] static bool CaptureStackTraceText(_EXCEPTION_POINTERS* pException, SString& outText) noexcept
{
    if (pException == nullptr || pException->ContextRecord == nullptr)
        return false;

    const DWORD contextFlags = pException->ContextRecord->ContextFlags;
    if ((contextFlags & CONTEXT_CONTROL) == 0)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::CaptureStackTraceText - CONTEXT_CONTROL flag missing; stack walk aborted\n");
        return false;
    }

    outText = "";
    outText.reserve(MAX_FALLBACK_STACK_FRAMES * 128u);

    const auto hProcess = GetCurrentProcess();
    const auto hThread = GetCurrentThread();

    CONTEXT      context = *pException->ContextRecord;
    STACKFRAME64 frame{};

    frame.AddrPC.Offset = context.Eip;
    frame.AddrFrame.Offset = context.Ebp;
    frame.AddrStack.Offset = context.Esp;

    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;

    SymbolHandlerGuard symbolGuard(hProcess);
    if (!symbolGuard.IsInitialized())
        return false;

    static_assert(MAX_SYM_NAME > 1, "MAX_SYM_NAME must include room for a terminator");
    constexpr DWORD                    kSymbolNameCapacity = MAX_SYM_NAME - 1;
    alignas(SYMBOL_INFO) unsigned char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
    PSYMBOL_INFO                       pSymbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
    *pSymbol = SYMBOL_INFO{.SizeOfStruct = sizeof(SYMBOL_INFO), .MaxNameLen = kSymbolNameCapacity};

    std::array<DWORD64, MAX_FALLBACK_STACK_FRAMES> visitedAddresses{};
    std::size_t                                    visitedCount = 0;

    for (std::size_t frameIndex = 0; frameIndex < MAX_FALLBACK_STACK_FRAMES; ++frameIndex)
    {
        BOOL bWalked =
            StackWalk64(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &frame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr);
        if (bWalked == FALSE)
            break;

        if (frame.AddrPC.Offset == 0)
            break;

        DWORD64 address = frame.AddrPC.Offset;

        const bool alreadyVisited =
            std::any_of(visitedAddresses.begin(), visitedAddresses.begin() + visitedCount, [address](DWORD64 seen) { return seen == address; });
        if (alreadyVisited)
            break;

        if (visitedCount < visitedAddresses.size())
            visitedAddresses[visitedCount++] = address;

        SString symbolName = SString("0x%llX", static_cast<unsigned long long>(address));
        if (SymFromAddr(hProcess, address, nullptr, pSymbol) != FALSE)
        {
            const auto terminatorIndex = static_cast<std::size_t>(pSymbol->MaxNameLen);
            if (terminatorIndex < MAX_SYM_NAME)
                pSymbol->Name[terminatorIndex] = '\0';
            symbolName = pSymbol->Name;
        }

        IMAGEHLP_LINE64 lineInfo{.SizeOfStruct = sizeof(IMAGEHLP_LINE64)};
        DWORD           lineDisplacement = 0;
        SString         lineDetail = "unknown";
        if (SymGetLineFromAddr64(hProcess, address, &lineDisplacement, &lineInfo) != FALSE)
        {
            const char* fileName = lineInfo.FileName != nullptr ? lineInfo.FileName : "unknown";
            lineDetail = SString("%s:%lu", fileName, static_cast<unsigned long>(lineInfo.LineNumber));
        }

        outText += SString("#%02u %s [0x%llX] (%s)\n", static_cast<unsigned int>(frameIndex), symbolName.c_str(), static_cast<unsigned long long>(address),
                           lineDetail.c_str());
    }

    return !outText.empty();
}

static void AppendFallbackStackTrace(_EXCEPTION_POINTERS* pException) noexcept
{
    bool expected = false;
    if (!ms_bFallbackStackLogged.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed))
        return;

    SString stackText;
    if (CaptureStackTraceText(pException, stackText))
    {
        AppendCrashDiagnostics(SString("\n[Fallback Stack Trace]\n%s\n", stackText.c_str()));
    }
    else
    {
        AppendCrashDiagnostics("\n[Fallback Stack Trace] unavailable\n");
    }
}

template <typename SectionGenerator>
void AppendDumpSection(const SString& targetPath, const char* tryLabel, const char* successLabel, DWORD tagBegin, DWORD tagEnd, SectionGenerator&& generator)
{
    SetApplicationSetting("diagnostics", "last-dump-extra", tryLabel);
    CBuffer buffer;
    std::forward<SectionGenerator>(generator)(buffer);
    CCrashDumpWriter::AppendToDumpFile(targetPath, buffer, tagBegin, tagEnd);
    SetApplicationSetting("diagnostics", "last-dump-extra", successLabel);
}

using MINIDUMPWRITEDUMP = BOOL(WINAPI*)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

void CCrashDumpWriter::OnCrashAverted(uint uiId)
{
    SCrashAvertedInfo* pInfo = MapFind(ms_CrashAvertedMap, uiId);
    if (pInfo == nullptr)
    {
        MapSet(ms_CrashAvertedMap, uiId, SCrashAvertedInfo());
        pInfo = MapFind(ms_CrashAvertedMap, uiId);
        pInfo->uiUsageCount = 0;
    }
    pInfo->uiTickCount = GetTickCount32();
    pInfo->uiUsageCount++;
}

void CCrashDumpWriter::OnEnterCrashZone(uint uiId)
{
    ms_uiInCrashZone = uiId;
}

void CCrashDumpWriter::LogEvent(const char* szType, const char* szContext, const char* szBody)
{
    ms_LogEventFilter.AddLine({szBody, szType, szContext});

    SLogEventLine line;
    while (ms_LogEventFilter.PopOutputLine(line))
    {
        ms_LogEventList.emplace_front(
            SLogEventInfo{.uiTickCount = GetTickCount32(), .strType = line.strType, .strContext = line.strContext, .strBody = line.strBody});

        while (ms_LogEventList.size() > LOG_EVENT_SIZE)
            ms_LogEventList.pop_back();
    }
}

void CCrashDumpWriter::SetHandlers()
{
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Setting up crash handlers\n");

    ms_uiInvalidParameterSampleCount.store(0, std::memory_order_relaxed);
    ms_bInvalidParameterWarningStored.store(false, std::memory_order_relaxed);
    std::fill(ms_InvalidParameterSamples.begin(), ms_InvalidParameterSamples.end(), "");
    SetApplicationSetting("diagnostics", "pending-invalid-parameter-warning", "");

    _set_invalid_parameter_handler(CCrashDumpWriter::HandleInvalidParameter);
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Invalid parameter handler installed\n");

    if (!SetCrashHandlerFilter(CCrashDumpWriter::HandleExceptionGlobal))
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: WARNING - Failed to install crash handler filter\n");
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Crash handler filter installed successfully\n");
    }

    CCrashDumpWriter::ReserveMemoryKBForCrashDumpProcessing(3000);
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Reserved 3000KB for crash dump processing\n");
}

void CCrashDumpWriter::UpdateCounters()
{
    const auto currentCount = ms_uiInvalidParameterCount.load(std::memory_order_relaxed);
    const auto loggedCount = ms_uiInvalidParameterCountLogged.load(std::memory_order_relaxed);

    if (currentCount > loggedCount && loggedCount < 10)
    {
        AddReportLog(9206, SString("InvalidParameterCount changed from %d to %d", loggedCount, currentCount));
        ms_uiInvalidParameterCountLogged.store(currentCount, std::memory_order_relaxed);
    }

    if (currentCount >= INVALID_PARAMETER_WARNING_THRESHOLD)
    {
        bool expected = false;
        if (ms_bInvalidParameterWarningStored.compare_exchange_strong(expected, true, std::memory_order_acq_rel, std::memory_order_relaxed))
        {
            const auto sampleCount =
                std::min(ms_uiInvalidParameterSampleCount.load(std::memory_order_relaxed), static_cast<uint>(INVALID_PARAMETER_SAMPLE_LIMIT));

            SString warning = SString("\n\nWarning: Detected %u invalid CRT parameter calls prior to crash.\n", currentCount);
            if (sampleCount > 0)
            {
                warning += "Sample invalid parameter inputs:\n";
                for (uint i = 0; i < sampleCount && i < ms_InvalidParameterSamples.size(); ++i)
                {
                    warning += SString("  %u) %s\n", i + 1, ms_InvalidParameterSamples[i].c_str());
                }
            }

            warning += "\n";

            SetApplicationSetting("diagnostics", "pending-invalid-parameter-warning", warning);
            WriteDebugEvent(warning.Replace("\n", " "));
            AddReportLog(9210, warning);
        }
    }
}

void CCrashDumpWriter::HandleInvalidParameter(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line,
                                              [[maybe_unused]] uintptr_t pReserved)
{
    const uint sampleIndex = ms_uiInvalidParameterSampleCount.fetch_add(1, std::memory_order_relaxed);
    if (sampleIndex < INVALID_PARAMETER_SAMPLE_LIMIT)
    {
        SString exprStr = WideToUtf8(expression);
        SString funcStr = WideToUtf8(function);
        SString fileStr = WideToUtf8(file);

        if (exprStr.empty())
            exprStr = "<null>";
        if (funcStr.empty())
            funcStr = "<unknown>";
        if (fileStr.empty())
            fileStr = "<unknown>";

        ms_InvalidParameterSamples[static_cast<std::size_t>(sampleIndex)] =
            SString("expr=\"%s\" func=\"%s\" file=\"%s\" line=%u reserved=0x%p", exprStr.c_str(), funcStr.c_str(), fileStr.c_str(), line,
                    reinterpret_cast<void*>(pReserved));
    }

    ms_uiInvalidParameterCount.fetch_add(1, std::memory_order_relaxed);
}

void CCrashDumpWriter::ReserveMemoryKBForCrashDumpProcessing(uint uiMemoryKB)
{
    FreeMemoryForCrashDumpProcessing();

    constexpr std::size_t kMaxReserveMemoryKB = 64u * 1024u;

    uiMemoryKB = std::clamp(static_cast<std::size_t>(uiMemoryKB), std::size_t{0}, kMaxReserveMemoryKB);

    if (uiMemoryKB > SIZE_MAX / 1024)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Memory reservation size too large\n");
        return;
    }

    ms_pReservedMemory = malloc(static_cast<std::size_t>(uiMemoryKB) * 1024);
    if (ms_pReservedMemory == nullptr)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Failed to reserve memory for crash dump processing\n");
    }
}

void CCrashDumpWriter::FreeMemoryForCrashDumpProcessing()
{
    if (ms_pReservedMemory != nullptr)
    {
        free(ms_pReservedMemory);
        ms_pReservedMemory = nullptr;
    }
}

long WINAPI CCrashDumpWriter::HandleExceptionGlobal(_EXCEPTION_POINTERS* pException)
{
    SAFE_DEBUG_OUTPUT("========================================\n");
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter::HandleExceptionGlobal - ENTRY\n");
    SAFE_DEBUG_OUTPUT("========================================\n");

    bool expected = false;
    if (!ms_bInCrashHandler.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed))
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: RECURSIVE CRASH - Already in crash handler\n");
        TerminateProcess(GetCurrentProcess(), CRASH_EXIT_CODE);
        _exit(CRASH_EXIT_CODE);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    ms_bFallbackStackLogged.store(false, std::memory_order_relaxed);

    FreeMemoryForCrashDumpProcessing();

    if (pException == nullptr || pException->ExceptionRecord == nullptr)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::HandleExceptionGlobal - NULL exception pointers\n");
        TerminateProcess(GetCurrentProcess(), CRASH_EXIT_CODE);
        _exit(CRASH_EXIT_CODE);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    const auto exceptionCode = pException->ExceptionRecord->ExceptionCode;

    if (IsFatalException(exceptionCode) == FALSE)
    {
        std::array<char, DEBUG_BUFFER_SIZE> szDebug;
        SAFE_DEBUG_PRINT_C(szDebug.data(), szDebug.size(), "CCrashDumpWriter: Non-fatal exception 0x%08X - ignoring\n", exceptionCode);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    LogExceptionDetails(pException);

    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: ======================================\n");
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: FATAL EXCEPTION - Begin crash processing\n");
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: ======================================\n");

    CExceptionInformation_Impl* pExceptionInformation = nullptr;

    pExceptionInformation = new (std::nothrow) CExceptionInformation_Impl;
    if (pExceptionInformation == nullptr)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::HandleExceptionGlobal - Failed to allocate exception information\n");
        TerminateProcess(GetCurrentProcess(), CRASH_EXIT_CODE);
        _exit(CRASH_EXIT_CODE);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    pExceptionInformation->Set(exceptionCode, pException);

    if (pExceptionInformation->GetCode() != exceptionCode)
    {
        std::array<char, DEBUG_BUFFER_SIZE> szDebug;
        SAFE_DEBUG_PRINT_C(szDebug.data(), szDebug.size(), "CCrashDumpWriter: WARNING - Exception code mismatch after Set() (expected 0x%08X, got 0x%08X)\n",
                           exceptionCode, pExceptionInformation->GetCode());
    }

    WriteDebugEvent("CCrashDumpWriter::HandleExceptionGlobal");

    bool bCrashArtifactsGenerated = false;
    bool bClientHandled = false;

    CModManager* pModManager = CModManager::GetSingletonPtr();
    if (pModManager != nullptr && pModManager->IsLoaded())
    {
        CClientBase* pClient = pModManager->GetClient();
        bool         bHandled = false;
        const bool   bHandledSafely = InvokeClientHandleExceptionSafe(pClient, pExceptionInformation, bHandled);

        if (bHandledSafely && bHandled)
        {
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Client handled exception - continuing execution\n");
            delete pExceptionInformation;
            return EXCEPTION_CONTINUE_SEARCH;
        }

        if (!bHandledSafely)
        {
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Client exception handler faulted - forcing crash dump\n");
        }

        bClientHandled = true;
    }

    ms_uiTickCountBase = GetTickCount32();

    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Generating crash artifacts...\n");

    try
    {
        DumpCoreLog(pException, pExceptionInformation);
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Core log dumped successfully\n");
    }
    catch (...)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: ERROR - Failed to dump core log\n");
    }

    try
    {
        DumpMiniDump(pException, pExceptionInformation);
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Mini dump created successfully\n");
    }
    catch (...)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: ERROR - Failed to create mini dump\n");
    }

    try
    {
        RunErrorTool(pExceptionInformation);
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Error dialog launched successfully\n");
    }
    catch (...)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: ERROR - Failed to launch error dialog\n");
    }

    bCrashArtifactsGenerated = true;

    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: ======================================\n");
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Crash processing complete - terminating\n");
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: ======================================\n");

    delete pExceptionInformation;
    pExceptionInformation = nullptr;

    if (ms_hCrashDialogProcess != nullptr)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Waiting for crash dialog to close before terminating...\n");
        const DWORD waitResult = WaitForSingleObject(ms_hCrashDialogProcess, INFINITE);
        if (waitResult == WAIT_OBJECT_0)
        {
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Crash dialog closed by user\n");
        }
        else
        {
            SAFE_DEBUG_OUTPUT(SString("CCrashDumpWriter: Wait for dialog returned 0x%08X\n", waitResult).c_str());
        }
        CloseHandle(ms_hCrashDialogProcess);
        ms_hCrashDialogProcess = nullptr;
    }
    else if (bCrashArtifactsGenerated)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: No process handle (likely ShellExecute path), polling for dialog window...\n");
        
        HWND hDialogWindow = nullptr;
        for (int attempts = 0; attempts < 30 && hDialogWindow == nullptr; attempts++)
        {
            hDialogWindow = FindWindowW(nullptr, L"MTA: San Andreas has encountered a problem");
            if (hDialogWindow == nullptr)
                Sleep(100);
        }
        
        if (hDialogWindow != nullptr && IsWindow(hDialogWindow))
        {
            SAFE_DEBUG_OUTPUT(SString("CCrashDumpWriter: Found dialog window %p, waiting for it to close...\n", hDialogWindow).c_str());
            
            while (IsWindow(hDialogWindow))
            {
                Sleep(100);
            }
            
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Dialog window closed\n");
        }
        else
        {
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Could not find dialog window, waiting 500ms as fallback\n");
            Sleep(500);
        }
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: No dialog process and no artifacts generated (early termination)\n");
        Sleep(500);
    }

    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Force terminating crashed process NOW\n");
    TerminateProcess(GetCurrentProcess(), CRASH_EXIT_CODE);

    _exit(CRASH_EXIT_CODE);

    return EXCEPTION_EXECUTE_HANDLER;
}

void CCrashDumpWriter::DumpCoreLog(_EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation)
{
    if (pExceptionInformation == nullptr)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - nullptr exception information\n");
        return;
    }

    if (!SharedUtil::IsReadablePointer(pExceptionInformation, sizeof(CExceptionInformation)))
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - Invalid exception information pointer\n");
        return;
    }

    FILE* pFlagFile = File::Fopen(CalcMTASAPath("mta\\core.log.flag"), "w");
    if (pFlagFile != nullptr)
    {
        fclose(pFlagFile);
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - Failed to create crash flag file\n");
    }

    time_t timeTemp;
    time(&timeTemp);

    const auto strMTAVersionFull = SString("%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting("mta-version-ext").SplitRight(".", nullptr, -2));

    constexpr auto kNullInstructionPointer = 0u;
    static_assert(kNullInstructionPointer == 0u, "Null instruction pointer must be zero");
    
    const auto eip = pExceptionInformation->GetEIP();
    const auto isNullJump = (eip == kNullInstructionPointer);
    
    constexpr std::string_view kNullJumpAnnotation = " (attempted jump to null)";
    constexpr std::string_view kEmptyAnnotation = "";
    const auto eipAnnotation = isNullJump ? kNullJumpAnnotation : kEmptyAnnotation;

    SString strInfo{};
    strInfo += SString("Version = %s\n", strMTAVersionFull.c_str());
    strInfo += SString("Time = %s", ctime(&timeTemp));
    strInfo += SString("Module = %s\n", pExceptionInformation->GetModulePathName());
    strInfo += SString("Code = 0x%08X\n", pExceptionInformation->GetCode());
    strInfo += SString("Offset = 0x%08X\n\n", pExceptionInformation->GetAddressModuleOffset());

    // Add special notice for watchdog timeout exceptions
    if (const auto exceptionCode = pExceptionInformation->GetCode(); exceptionCode == CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT)
    {
        constexpr std::string_view kWatchdogBanner =
            "========================================\n"
            "GAME FREEZE: FORCED CRASH GENERATION FOR INVESTIGATION\n"
            "Stack Arrow marks frozen function/loop\n"
            "========================================\n\n";
        strInfo += SString{kWatchdogBanner.data(), kWatchdogBanner.size()};
    }

    strInfo += SString(
        "EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\n"
        "EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X%.*s  FLG=%08X\n"
        "CS=%04X   DS=%04X  SS=%04X  ES=%04X   FS=%04X  GS=%04X\n\n",
        pExceptionInformation->GetEAX(), pExceptionInformation->GetEBX(), pExceptionInformation->GetECX(), pExceptionInformation->GetEDX(),
        pExceptionInformation->GetESI(), pExceptionInformation->GetEDI(), pExceptionInformation->GetEBP(), pExceptionInformation->GetESP(),
        pExceptionInformation->GetEIP(), static_cast<int>(eipAnnotation.size()), eipAnnotation.data(), pExceptionInformation->GetEFlags(), 
        pExceptionInformation->GetCS(), pExceptionInformation->GetDS(),
        pExceptionInformation->GetSS(), pExceptionInformation->GetES(), pExceptionInformation->GetFS(), pExceptionInformation->GetGS());

    const auto invalidParameterCount = ms_uiInvalidParameterCount.load(std::memory_order_relaxed);
    strInfo += SString("InvalidParameterCount = %u\n", invalidParameterCount);
    strInfo += SString("CrashZone = %u\n", ms_uiInCrashZone);
    strInfo += SString("ThreadId = %u\n\n", GetCurrentThreadId());

    if (auto pendingWarning = GetApplicationSetting("diagnostics", "pending-invalid-parameter-warning"); !pendingWarning.empty())
    {
        strInfo += pendingWarning;
    }

    EnsureCrashReasonForDialog(pExceptionInformation);

    SString strDetailed;
    SString strDetailedForDialog;
    bool    hasDetailedSection = false;
    bool    hasDetailedSectionForDialog = false;

    struct DetailedAppender
    {
        SString& buffer;
        SString& bufferForDialog;
        bool&    hasSection;
        bool&    hasSectionForDialog;
        bool     skipFromDialog;

        void EnsureSectionHeader() const
        {
            if (!hasSection)
            {
                buffer += "\n";
                hasSection = true;
            }
            if (!skipFromDialog && !hasSectionForDialog)
            {
                bufferForDialog += "\n";
                hasSectionForDialog = true;
            }
        }

        void operator()(const SString& line, bool excludeFromDialog = false) const
        {
            if (line.empty())
                return;

            EnsureSectionHeader();
            buffer += line;
            if (!excludeFromDialog)
                bufferForDialog += line;
        }

        void operator()(const char* text, bool excludeFromDialog = false) const
        {
            if (text == nullptr || *text == '\0')
                return;

            EnsureSectionHeader();
            buffer += text;
            if (!excludeFromDialog)
                bufferForDialog += text;
        }
    } appendDetailedLine{strDetailed, strDetailedForDialog, hasDetailedSection, hasDetailedSectionForDialog, false};

    ENHANCED_EXCEPTION_INFO enhancedInfo = {};
    bool                    hasEnhancedInfo = false;

    const std::vector<std::string>*         stackFrames = nullptr;
    std::optional<std::vector<std::string>> fallbackStackStorage;
    const char*                             stackHeader = nullptr;

    try
    {
        hasEnhancedInfo = GetEnhancedExceptionInfo(&enhancedInfo) != FALSE;
        if (hasEnhancedInfo)
        {
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - Using enhanced exception info from CrashHandler\n");
        }
        else
        {
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - No enhanced exception info available, using local data\n");
        }
    }
    catch (...)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - Exception while querying enhanced info\n");
        hasEnhancedInfo = false;
    }

    CExceptionInformation_Impl* pImpl = dynamic_cast<CExceptionInformation_Impl*>(pExceptionInformation);

    if (hasEnhancedInfo)
    {
        const auto timePoint = enhancedInfo.timestamp;
        const auto timeT = std::chrono::system_clock::to_time_t(timePoint);
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch()) % 1000;

        if (std::tm* tmInfo = std::localtime(&timeT))
        {
            const int year = std::clamp(tmInfo->tm_year + 1900, 1900, 9999);
            const int month = std::clamp(tmInfo->tm_mon + 1, 1, 12);
            const int day = std::clamp(tmInfo->tm_mday, 1, 31);
            const int hour = std::clamp(tmInfo->tm_hour, 0, 23);
            const int minute = std::clamp(tmInfo->tm_min, 0, 59);
            const int second = std::clamp(tmInfo->tm_sec, 0, 59);

            const long long msValue = std::clamp(milliseconds.count(), 0LL, 999LL);

            appendDetailedLine(SString("Precise crash time: %04d-%02d-%02d %02d:%02d:%02d.%03lld\n", year, month, day, hour, minute, second, msValue));
        }

        appendDetailedLine(SString("Thread ID: %lu\n", enhancedInfo.threadId), true);
        appendDetailedLine(SString("Process ID: %lu\n", enhancedInfo.processId), true);

        if (!enhancedInfo.exceptionType.empty())
            appendDetailedLine(SString("Exception Type: %s\n", enhancedInfo.exceptionType.c_str()));

        if (!enhancedInfo.exceptionDescription.empty())
            appendDetailedLine(SString("Exception: %s\n", enhancedInfo.exceptionDescription.c_str()));

        if (enhancedInfo.stackTrace.has_value() && !enhancedInfo.stackTrace->empty())
        {
            stackFrames = &enhancedInfo.stackTrace.value();
            stackHeader = "Stack trace:\n";
        }

        if (!enhancedInfo.modulePathName.empty())
            appendDetailedLine(SString("Resolved Module Path: %s\n", enhancedInfo.modulePathName.c_str()));

        if (!enhancedInfo.moduleBaseName.empty())
            appendDetailedLine(SString("Resolved Module Base: %s\n", enhancedInfo.moduleBaseName.c_str()));

        appendDetailedLine(SString("Resolved Module Offset: 0x%08X\n", enhancedInfo.moduleOffset));

        if (!enhancedInfo.additionalInfo.empty())
            appendDetailedLine(SString("Additional Info: %s\n", enhancedInfo.additionalInfo.c_str()));

        if (enhancedInfo.capturedException.has_value())
        {
            appendDetailedLine("Captured C++ exception: Yes\n");
            appendDetailedLine(SString("Uncaught exception count: %d\n", enhancedInfo.uncaughtExceptionCount));
        }

        appendDetailedLine(SString("Fatal Exception: %s\n", enhancedInfo.isFatal ? "Yes" : "No"));
    }
    else if (pImpl != nullptr)
    {
        auto timestamp = pImpl->GetTimestamp();
        if (timestamp.has_value())
        {
            const auto timePoint = timestamp.value();
            const auto timeT = std::chrono::system_clock::to_time_t(timePoint);
            const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch()) % 1000;

            if (std::tm* tmInfo = std::localtime(&timeT))
            {
                long long msValue = milliseconds.count();
                if (msValue < 0)
                    msValue = 0;
                else if (msValue > 999)
                    msValue = 999;

                appendDetailedLine(SString("Precise crash time: %04d-%02d-%02d %02d:%02d:%02d.%03lld\n", tmInfo->tm_year + 1900, tmInfo->tm_mon + 1,
                                           tmInfo->tm_mday, tmInfo->tm_hour, tmInfo->tm_min, tmInfo->tm_sec, msValue));
            }
        }

        appendDetailedLine(SString("Thread ID: %lu\n", pImpl->GetThreadId()), true);
        appendDetailedLine(SString("Process ID: %lu\n", pImpl->GetProcessId()), true);

        std::string description = pImpl->GetExceptionDescription();
        if (!description.empty())
            appendDetailedLine(SString("Exception: %s\n", description.c_str()));

        auto capturedException = pImpl->GetCapturedException();
        if (capturedException.has_value())
        {
            appendDetailedLine("Captured C++ exception: Yes\n");
            appendDetailedLine(SString("Uncaught exception count: %d\n", pImpl->GetUncaughtExceptionCount()));
        }
    }
    else
    {
        appendDetailedLine("Enhanced exception diagnostics unavailable.\n");
    }

    // Try to get stack trace from detailed exception information
    if (stackFrames == nullptr && pImpl != nullptr && pImpl->HasDetailedStackTrace())
    {
        if (auto stackTrace = pImpl->GetStackTrace(); stackTrace.has_value() && !stackTrace->empty())
        {
            fallbackStackStorage = std::move(*stackTrace);
            stackFrames = std::addressof(*fallbackStackStorage);
            stackHeader = hasEnhancedInfo ? "Stack trace (local capture):\n" : "Stack trace:\n";
        }
    }

    // If still no stack trace, try to capture one directly from exception pointers
    if (stackFrames == nullptr && pException != nullptr)
    {
        if (SString stackText; CaptureStackTraceText(pException, stackText) && !stackText.empty())
        {
            constexpr std::size_t kTypicalStackDepth = 50;
            static_assert(kTypicalStackDepth > 0, "Stack depth must be positive");
            
            std::vector<SString> lines{};
            lines.reserve(kTypicalStackDepth);
            stackText.Split("\n", lines);
            
            if (const auto lineCount = lines.size(); lineCount > 0)
            {
                std::vector<std::string> capturedFrames{};
                capturedFrames.reserve(lineCount);
                
                std::transform(lines.cbegin(), lines.cend(), std::back_inserter(capturedFrames),
                    [](const auto& line) noexcept -> std::string {
                        return std::string{line.c_str()};
                    });

                const auto newEnd = std::remove_if(capturedFrames.begin(), capturedFrames.end(),
                    [](const auto& frame) noexcept -> bool {
                        return frame.empty();
                    });
                capturedFrames.erase(newEnd, capturedFrames.end());
                
                if (const auto capturedCount = capturedFrames.size(); capturedCount > 0)
                {
                    fallbackStackStorage = std::move(capturedFrames);
                    stackFrames = std::addressof(*fallbackStackStorage);
                    stackHeader = "Stack trace (captured):\n";
                }
            }
        }
        else if (pException->ContextRecord != nullptr && pException->ContextRecord->Eip == 0)
        {
            // Special case: EIP=0 means we cannot walk the stack
            // Provide register-based context instead
            constexpr std::size_t kRegisterFrameCount = 3;
            static_assert(kRegisterFrameCount > 0, "Register frame count must be positive");
            
            std::vector<std::string> registerFrames{};
            registerFrames.reserve(kRegisterFrameCount);
            
            const auto* ctx = pException->ContextRecord;
            registerFrames.emplace_back(SString("EIP=0x%08X (null instruction pointer - cannot walk stack)", ctx->Eip).c_str());
            registerFrames.emplace_back(SString("ESP=0x%08X EBP=0x%08X (stack pointers)", ctx->Esp, ctx->Ebp).c_str());
            registerFrames.emplace_back(SString("Last known address: check return address at [ESP] or recent call history", 0).c_str());
            
            fallbackStackStorage = std::move(registerFrames);
            stackFrames = std::addressof(*fallbackStackStorage);
            stackHeader = "Stack trace (EIP=0 - register context only):\n";
        }
    }

    if (stackFrames != nullptr)
    {
        constexpr std::size_t kMaxDisplayFrames = 100;
        constexpr std::size_t kMaxFrameLength = 512;
        static_assert(kMaxDisplayFrames > 0 && kMaxFrameLength > 0, "Display limits must be positive");
        
        const auto& frames = *stackFrames;
        const auto  frameCount = frames.size();
        const auto  maxFrames = std::clamp(frameCount, std::size_t{0}, 
                                           std::min(kMaxDisplayFrames, std::numeric_limits<std::size_t>::max()));

        if (stackHeader == nullptr)
            stackHeader = "Stack trace:\n";

        // For watchdog timeouts, add explanatory note about freeze point
        const bool isWatchdogTimeout = pExceptionInformation != nullptr && 
                                       pExceptionInformation->GetCode() == CUSTOM_EXCEPTION_CODE_WATCHDOG_TIMEOUT;
        
        if (isWatchdogTimeout && maxFrames > 0)
        {
            appendDetailedLine("Stack trace (captured from frozen thread - first frame shows freeze location):\n");
        }
        else
        {
            appendDetailedLine(stackHeader);
        }
        
        [[maybe_unused]] const auto allFramesValid = 
            std::all_of(frames.cbegin(), std::next(frames.cbegin(), static_cast<std::ptrdiff_t>(maxFrames)), 
                       [](const auto& frame) constexpr noexcept -> bool { return !frame.empty(); });
        
        for (std::size_t i{}; i < maxFrames; ++i)
        {
            const auto&       frame = frames[i];
            const std::size_t frameLength = std::clamp(frame.length(), std::size_t{0}, kMaxFrameLength);
            
            // First frame in watchdog timeout = exact freeze location
            if (isWatchdogTimeout && i == 0)
            {
                appendDetailedLine(SString("--> %.*s  <-- FREEZE POINT\n", static_cast<int>(frameLength), frame.c_str()));
            }
            else
            {
                appendDetailedLine(SString("  %.*s\n", static_cast<int>(frameLength), frame.c_str()));
            }
        }
    }
    else
    {
        appendDetailedLine("Stack trace: unavailable\n");
    }

    // Get crash dump file path if available
    const auto dumpFileInfo = [dumpFilePath = GetApplicationSetting("diagnostics", "last-dump-save")]() -> SString {
        if (dumpFilePath.empty())
            return {};
        
        constexpr std::string_view kDumpFileHeader = "\nCrash dump file: ";
        return SString{kDumpFileHeader.data(), kDumpFileHeader.size()} + dumpFilePath + "\n";
    }();

    FILE* pFile = File::Fopen(CalcMTASAPath("mta\\core.log"), "a+");
    if (pFile != nullptr)
    {
        fprintf(pFile, "%s", "** -- Unhandled exception -- **\n\n");
        fprintf(pFile, "%s", strInfo.c_str());

        if (!strDetailed.empty())
            fprintf(pFile, "%s", strDetailed.c_str());

        if (!dumpFileInfo.empty())
            fprintf(pFile, "%s", dumpFileInfo.c_str());

        fprintf(pFile, "%s", "** -- End of unhandled exception -- **\n\n\n");
        fclose(pFile);
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - Failed to open core.log file\n");
    }

    SString strInfoForSettings = strInfo;
    if (!strDetailedForDialog.empty())
    {
        strInfoForSettings += strDetailedForDialog;
    }

    if (!dumpFileInfo.empty())
    {
        strInfoForSettings += dumpFileInfo;
    }

    if (strInfoForSettings.empty())
        strInfoForSettings = "** Crash info unavailable **\n";

    SetApplicationSetting("diagnostics", "last-crash-info", strInfoForSettings);
    SetApplicationSetting("diagnostics", "last-crash-module", pExceptionInformation->GetModulePathName());
    SetApplicationSettingInt("diagnostics", "last-crash-code", pExceptionInformation->GetCode());
    SetApplicationSetting("diagnostics", "last-crash-extra", "");
    SetApplicationSetting("diagnostics", "pending-invalid-parameter-warning", "");

    SString strInfoForDebug = strInfoForSettings;
    strInfoForDebug.Replace("\n", " ");
    WriteDebugEvent(strInfoForDebug);
}

void CCrashDumpWriter::DumpMiniDump(_EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation)
{
    WriteDebugEvent("CCrashDumpWriter::DumpMiniDump");
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - Starting minidump creation...\n");

    bool bMiniDumpSucceeded = false;

    constexpr const char* kDbgHelpDllName = "DBGHELP.DLL";

    HMODULE                    hDll = nullptr;
    std::array<char, MAX_PATH> modulePath{};
    DWORD                      moduleLen = GetModuleFileNameA(nullptr, modulePath.data(), static_cast<DWORD>(modulePath.size()));

    if (moduleLen == 0)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - GetModuleFileNameA failed\n");
    }
    else if (moduleLen >= modulePath.size())
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - Module path truncated\n");
    }
    else
    {
        char* pSlash = strrchr(modulePath.data(), '\\');
        if (pSlash != nullptr)
        {
            const std::size_t dirLength = static_cast<std::size_t>(pSlash - modulePath.data()) + 1;
            const std::size_t remainingSpace = modulePath.size() - dirLength;
            const std::size_t requiredSpace = std::strlen(kDbgHelpDllName) + 1;

            if (remainingSpace >= requiredSpace)
            {
                std::memcpy(pSlash + 1, kDbgHelpDllName, requiredSpace);
                hDll = LoadLibraryA(modulePath.data());
            }
            else
            {
                SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - Not enough space to append DBGHELP.DLL\n");
            }
        }
    }

    if (hDll == nullptr)
    {
        std::array<char, MAX_PATH> systemPath{};
        UINT                       systemLen = GetSystemDirectoryA(systemPath.data(), static_cast<UINT>(systemPath.size()));

        if (systemLen == 0)
        {
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - GetSystemDirectoryA failed\n");
        }
        else if (systemLen >= systemPath.size())
        {
            SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - System directory path truncated\n");
        }
        else
        {
            const std::size_t dllNameLen = std::strlen(kDbgHelpDllName);
            std::size_t       appendIndex = static_cast<std::size_t>(systemLen);

            if (appendIndex > 0 && systemPath[appendIndex - 1] != '\\')
            {
                if (appendIndex + 1 < systemPath.size())
                {
                    systemPath[appendIndex++] = '\\';
                }
                else
                {
                    SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - No space to append backslash to system path\n");
                    appendIndex = std::numeric_limits<size_t>::max();
                }
            }

            if (appendIndex != std::numeric_limits<std::size_t>::max())
            {
                const std::size_t required = appendIndex + dllNameLen + 1;
                if (required <= systemPath.size())
                {
                    std::memcpy(systemPath.data() + appendIndex, kDbgHelpDllName, dllNameLen + 1);
                    hDll = LoadLibraryA(systemPath.data());
                }
                else
                {
                    SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - System path too long for DBGHELP.DLL\n");
                }
            }
        }
    }

    if (hDll == nullptr)
        AddReportLog(9201, "CCrashDumpWriter::DumpMiniDump - Could not load DBGHELP.DLL");

    if (hDll != nullptr)
    {
        if (MINIDUMPWRITEDUMP pDump = nullptr; SharedUtil::TryGetProcAddress(hDll, "MiniDumpWriteDump", pDump))
        {
            const auto hFile =
                CreateFileA(CalcMTASAPath("mta\\core.dmp"), GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile == INVALID_HANDLE_VALUE)
                AddReportLog(9203, SString("CCrashDumpWriter::DumpMiniDump - Could not create '%s'", *CalcMTASAPath("mta\\core.dmp")));

            if (hFile != INVALID_HANDLE_VALUE)
            {
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo{.ThreadId = GetCurrentThreadId(), .ExceptionPointers = pException, .ClientPointers = FALSE};

                BOOL bResult = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
                                     static_cast<MINIDUMP_TYPE>(MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory), &ExInfo, nullptr, nullptr);

                if (!bResult)
                {
                    AddReportLog(9204, SString("CCrashDumpWriter::DumpMiniDump - MiniDumpWriteDump failed (%08x)", GetLastError()));
                    SAFE_DEBUG_OUTPUT(SString("CCrashDumpWriter::DumpMiniDump - MiniDumpWriteDump FAILED with error 0x%08X\n", GetLastError()).c_str());
                }
                else
                {
                    WriteDebugEvent("CCrashDumpWriter::DumpMiniDump - MiniDumpWriteDump succeeded");
                    SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - MiniDumpWriteDump succeeded\n");
                    bMiniDumpSucceeded = true;
                }

                CloseHandle(hFile);

                SYSTEMTIME SystemTime;
                GetLocalTime(&SystemTime);

                if (!CreateDirectoryA(CalcMTASAPath("mta\\dumps"), nullptr))
                {
                    if (const auto dwError = GetLastError(); dwError != ERROR_ALREADY_EXISTS)
                    {
                        AddReportLog(9207, SString("CCrashDumpWriter::DumpMiniDump - Failed to create dumps directory (%08x)", dwError));
                    }
                }

                if (!CreateDirectoryA(CalcMTASAPath("mta\\dumps\\private"), nullptr))
                {
                    if (const auto dwError = GetLastError(); dwError != ERROR_ALREADY_EXISTS)
                    {
                        AddReportLog(9208, SString("CCrashDumpWriter::DumpMiniDump - Failed to create private dumps directory (%08x)", dwError));
                    }
                }

                SString strModuleName = pExceptionInformation->GetModuleBaseName();
                strModuleName = strModuleName.ReplaceI(".dll", "").Replace(".exe", "").Replace("_", "").Replace(".", "").Replace("-", "");
                if (strModuleName.length() == 0)
                    strModuleName = "unknown";

                const auto strMTAVersionFull = SString("%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting("mta-version-ext").SplitRight(".", nullptr, -2));
                const auto strSerialPart = GetApplicationSetting("serial").substr(0, 5);
                const auto uiServerIP = GetApplicationSettingInt("last-server-ip");
                const auto uiServerPort = GetApplicationSettingInt("last-server-port");
                const auto uiServerTime = GetApplicationSettingInt("last-server-time");
                const __time64_t currentTime = _time64(nullptr);

                __int64 rawDuration = 0;
                if (currentTime != static_cast<__time64_t>(-1))
                {
                    const __time64_t lastServerTime = static_cast<__time64_t>(uiServerTime);
                    rawDuration = currentTime - lastServerTime;
                }

                rawDuration = std::clamp(rawDuration, static_cast<__int64>(0), std::numeric_limits<__int64>::max() - 1);

                const __int64 adjustedDuration = std::clamp(rawDuration + 1, static_cast<__int64>(1), static_cast<__int64>(0x0fff));

                const int uiServerDuration = static_cast<int>(adjustedDuration);

                SString strPathCode;
                {
                    std::vector<SString> parts;
                    PathConform(CalcMTASAPath("")).Split(PATH_SEPERATOR, parts);
                    for (uint i = 0; i < parts.size(); i++)
                    {
                        if (parts[i].CompareI("Program Files"))
                            strPathCode += "Pr";
                        else if (parts[i].CompareI("Program Files (x86)"))
                            strPathCode += "Px";
                        else if (parts[i].CompareI("MTA San Andreas"))
                            strPathCode += "Mt";
                        else if (parts[i].BeginsWithI("MTA San Andreas"))
                            strPathCode += "Mb";
                        else
                            strPathCode += parts[i].Left(1).ToUpper();
                    }
                }

                SString strFilename("mta\\dumps\\private\\client_%s_%s_%08x_%x_%s_%08X_%04X_%03X_%s_%04d%02d%02d_%02d%02d.dmp", strMTAVersionFull.c_str(),
                                    strModuleName.c_str(), pExceptionInformation->GetAddressModuleOffset(), pExceptionInformation->GetCode() & 0xffff,
                                    strPathCode.c_str(), uiServerIP, uiServerPort, uiServerDuration, strSerialPart.c_str(), SystemTime.wYear, SystemTime.wMonth,
                                    SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute);

                const SString privateDumpDir = CalcMTASAPath("mta\\dumps\\private");
                const SString strPathFilename = CalcMTASAPath(strFilename);

                if (!ValidatePathWithinBase(privateDumpDir, strPathFilename, 9213))
                {
                    AddReportLog(9213, SString("CCrashDumpWriter::DumpMiniDump - rejected dump path (%s)", strPathFilename.c_str()));
                    SetApplicationSetting("diagnostics", "last-dump-extra", "invalid-path");
                    SetApplicationSetting("diagnostics", "last-dump-save", "");
                    SetApplicationSetting("diagnostics", "last-dump-complete", "");
                }
                else
                {
                    const BOOL bCopied = CopyFileA(CalcMTASAPath("mta\\core.dmp"), strPathFilename, FALSE);
                    if (!bCopied)
                    {
                        const DWORD copyError = GetLastError();
                        AddReportLog(9209, SString("CCrashDumpWriter::DumpMiniDump - Failed to persist crash dump copy (%08x)", copyError));
                        SetApplicationSetting("diagnostics", "last-dump-extra", "copy-failed");
                        SetApplicationSetting("diagnostics", "last-dump-save", "");
                        SetApplicationSetting("diagnostics", "last-dump-complete", "");
                    }
                    else
                    {
                        SetApplicationSetting("diagnostics", "last-dump-extra", "none");
                        SetApplicationSetting("diagnostics", "last-dump-save", strPathFilename);
                        SetApplicationSetting("diagnostics", "last-dump-complete", "");

                        AppendDumpSection(strPathFilename, "try-pools", "added-pools", 'POLs', 'POLe',
                                          [](CBuffer& buffer) { CCrashDumpWriter::GetPoolInfo(buffer); });

                        AppendDumpSection(strPathFilename, "try-d3d", "added-d3d", 'D3Ds', 'D3De',
                                          [](CBuffer& buffer) { CCrashDumpWriter::GetD3DInfo(buffer); });

                        AppendDumpSection(strPathFilename, "try-crash-averted", "added-crash-averted", 'CASs', 'CASe',
                                          [](CBuffer& buffer) { CCrashDumpWriter::GetCrashAvertedStats(buffer); });

                        AppendDumpSection(strPathFilename, "try-log", "added-log", 'LOGs', 'LOGe',
                                          [](CBuffer& buffer) { CCrashDumpWriter::GetLogInfo(buffer); });

                        AppendDumpSection(strPathFilename, "try-misc", "added-misc", 'DXIs', 'DXIe',
                                          [](CBuffer& buffer) { CCrashDumpWriter::GetDxInfo(buffer); });

                        AppendDumpSection(strPathFilename, "try-misc", "added-misc", 'MSCs', 'MSCe',
                                          [](CBuffer& buffer) { CCrashDumpWriter::GetMiscInfo(buffer); });

                        AppendDumpSection(strPathFilename, "try-mem", "added-mem", 'MEMs', 'MEMe',
                                          [](CBuffer& buffer) { CCrashDumpWriter::GetMemoryInfo(buffer); });

                        AppendDumpSection(strPathFilename, "try-logfile", "added-logfile", 'LOGs', 'LOGe',
                                          [](CBuffer& buffer) { buffer.LoadFromFile(CalcMTASAPath(PathJoin("mta", "logs", "logfile.txt"))); });

                        AppendDumpSection(strPathFilename, "try-report", "added-report", 'REPs', 'REPe',
                                          [](CBuffer& buffer) { buffer.LoadFromFile(PathJoin(GetMTADataPath(), "report.log")); });

                        AppendDumpSection(strPathFilename, "try-anim-task", "added-anim-task", 'CATs', 'CATe',
                                          [](CBuffer& buffer) { CCrashDumpWriter::GetCurrentAnimTaskInfo(buffer); });

                        // Mark dump file as complete (all sections appended)
                        SetApplicationSetting("diagnostics", "last-dump-complete", "1");
                        AddReportLog(5201, SString("CCrashDumpWriter::DumpMiniDump - Marked dump file as complete: %s", *strPathFilename));
                    }
                }
            }
        }
        else
        {
            AddReportLog(9202, "CCrashDumpWriter::DumpMiniDump - Could not find MiniDumpWriteDump");
        }

        FreeLibrary(hDll);
    }

    if (bMiniDumpSucceeded)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - Minidump creation COMPLETED successfully\n");
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpMiniDump - Minidump creation FAILED\n");
        AppendFallbackStackTrace(pException);
    }

    if (ms_uiInCrashZone == 1 || ms_uiInCrashZone == 2)
    {
        CVARS_SET("volumetric_shadows", false);
        CCore::GetSingleton().SaveConfig();
        AddReportLog(9205, "Disabled volumetric shadows");
    }

    CNet* pNet = CCore::GetSingleton().GetNetwork();
    if (pNet != nullptr)
        pNet->PostCrash();
}

void CCrashDumpWriter::RunErrorTool(CExceptionInformation* pExceptionInformation)
{
    SAFE_DEBUG_OUTPUT("========================================\n");
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - ENTRY\n");
    SAFE_DEBUG_OUTPUT("========================================\n");

    static std::atomic<bool> bDoneReport{false};
    bool                     expected = false;
    if (!bDoneReport.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed))
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - Already called, returning\n");
        return;
    }

    SString strMessage(
        "Crash 0x%08X 0x%08X %s"
        " EAX=%08X EBX=%08X ECX=%08X EDX=%08X ESI=%08X"
        " EDI=%08X EBP=%08X ESP=%08X EIP=%08X FLG=%08X"
        " CS=%04X DS=%04X SS=%04X ES=%04X"
        " FS=%04X GS=%04X",
        pExceptionInformation->GetCode(), pExceptionInformation->GetAddressModuleOffset(), pExceptionInformation->GetModulePathName(),
        pExceptionInformation->GetEAX(), pExceptionInformation->GetEBX(), pExceptionInformation->GetECX(), pExceptionInformation->GetEDX(),
        pExceptionInformation->GetESI(), pExceptionInformation->GetEDI(), pExceptionInformation->GetEBP(), pExceptionInformation->GetESP(),
        pExceptionInformation->GetEIP(), pExceptionInformation->GetEFlags(), pExceptionInformation->GetCS(), pExceptionInformation->GetDS(),
        pExceptionInformation->GetSS(), pExceptionInformation->GetES(), pExceptionInformation->GetFS(), pExceptionInformation->GetGS());

    AddReportLog(3120, strMessage);

    EnsureCrashReasonForDialog(pExceptionInformation);

    SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - *** FORCING DISPLAY TO DESKTOP MODE (CRITICAL) ***\n");

    for (int retryCount = 0; retryCount < 3; retryCount++)
    {
        LONG                  result = ChangeDisplaySettings(nullptr, 0);
        std::array<char, 128> debugMsg;
        sprintf(debugMsg.data(), "CCrashDumpWriter::RunErrorTool - ChangeDisplaySettings attempt %d, result=%ld\n", retryCount + 1, result);
        SAFE_DEBUG_OUTPUT(debugMsg.data());
        Sleep(300);
    }

    SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - Display mode restoration complete\n");

    HWND hGTAWindow = FindWindowW(L"Grand theft auto San Andreas", nullptr);
    if (hGTAWindow == nullptr)
        hGTAWindow = FindWindowW(nullptr, L"MTA: San Andreas");
    if (hGTAWindow != nullptr && IsWindow(hGTAWindow))
    {
        ShowWindow(hGTAWindow, SW_HIDE);
    }

    const auto basePath = GetMTASABaseDir();
    const auto basePathWide = basePath.empty() ? WString() : FromUTF8(basePath);

    auto RestoreBaseDirectories = [&]()
    {
        if (basePath.empty())
            return;

        if (basePathWide.empty())
        {
            if (!SetCurrentDirectoryA(basePath.c_str()))
            {
                const DWORD restoreError = GetLastError();
                AddReportLog(3125, SString("RunErrorTool failed to restore current directory (%s) error %u", basePath.c_str(), restoreError));
            }

            if (!SetDllDirectoryA(basePath.c_str()))
            {
                const DWORD restoreDllError = GetLastError();
                AddReportLog(3126, SString("RunErrorTool failed to restore DLL directory (%s) error %u", basePath.c_str(), restoreDllError));
            }

            return;
        }

        if (!SetCurrentDirectoryW(basePathWide.c_str()))
        {
            const DWORD restoreError = GetLastError();
            AddReportLog(3125, SString("RunErrorTool failed to restore current directory (%s) error %u", basePath.c_str(), restoreError));
        }

        if (!SetDllDirectoryW(basePathWide.c_str()))
        {
            const DWORD restoreDllError = GetLastError();
            AddReportLog(3126, SString("RunErrorTool failed to restore DLL directory (%s) error %u", basePath.c_str(), restoreDllError));
        }
    };

    if (basePath.empty())
    {
        AddReportLog(3125, "RunErrorTool base path empty; skipping initial directory setup");
    }
    else
    {
        if (basePathWide.empty())
        {
            AddReportLog(3125, SString("RunErrorTool failed to convert base directory to wide characters (%s)", basePath.c_str()));
            if (!SetCurrentDirectoryA(basePath.c_str()))
            {
                const DWORD baseDirError = GetLastError();
                AddReportLog(3125, SString("RunErrorTool failed to set base current directory (%s) error %u", basePath.c_str(), baseDirError));
            }

            if (!SetDllDirectoryA(basePath.c_str()))
            {
                const DWORD baseDllError = GetLastError();
                AddReportLog(3126, SString("RunErrorTool failed to set base DLL directory (%s) error %u", basePath.c_str(), baseDllError));
            }
        }
        else
        {
            if (!SetCurrentDirectoryW(basePathWide.c_str()))
            {
                const DWORD baseDirError = GetLastError();
                AddReportLog(3125, SString("RunErrorTool failed to set base current directory (%s) error %u", basePath.c_str(), baseDirError));
            }

            if (!SetDllDirectoryW(basePathWide.c_str()))
            {
                const DWORD baseDllError = GetLastError();
                AddReportLog(3126, SString("RunErrorTool failed to set base DLL directory (%s) error %u", basePath.c_str(), baseDllError));
            }
        }
    }

    constexpr const char* crashArgs = "install_stage=crashed";
    const WString         crashArgsWide = FromUTF8(crashArgs);
    const size_t          crashArgsLength = std::strlen(crashArgs);
    if (crashArgsLength > std::numeric_limits<size_t>::max() - 1)
    {
        AddReportLog(3129, "RunErrorTool crash arguments unexpectedly large; aborting relaunch");
        return;
    }

    bool       dialogLaunched = false;
    const auto candidates = BuildCrashDialogCandidates();

    for (const SString& candidate : candidates)
    {
        if (candidate.empty())
            continue;

        const std::string candidateStd(candidate.c_str());
        bool              candidateExists = SharedUtil::FileExists(candidateStd);

        const WString candidateWide = candidate.empty() ? WString() : FromUTF8(candidate);
        if (!candidateExists && !candidateWide.empty())
        {
            if (const auto attrs = GetFileAttributesW(candidateWide.c_str()); attrs != INVALID_FILE_ATTRIBUTES)
            {
                candidateExists = (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
            }
        }

        if (!candidateExists)
        {
            AddReportLog(3123, SString("RunErrorTool candidate missing (%s)", candidate.c_str()));
            continue;
        }

        SString candidateDir = SharedUtil::ExtractPath(candidate);
        if (candidateDir.empty())
            candidateDir = basePath;

        const WString candidateDirWide = candidateDir.empty() ? basePathWide : FromUTF8(candidateDir);
        if (candidateDirWide.empty() && !candidateDir.empty())
        {
            AddReportLog(3127, SString("RunErrorTool failed to convert candidate directory to wide characters (%s)", candidateDir.c_str()));
            RestoreBaseDirectories();
            continue;
        }

        const wchar_t* candidateDirWidePtr = candidateDirWide.empty() ? basePathWide.c_str() : candidateDirWide.c_str();

        if ((candidateDirWidePtr == nullptr || candidateDirWidePtr[0] == L'\0') && !candidateDir.empty())
        {
            AddReportLog(3127, SString("RunErrorTool candidate directory unavailable (%s)", candidateDir.c_str()));
            RestoreBaseDirectories();
            continue;
        }

        if (!SetCurrentDirectoryW(candidateDirWidePtr))
        {
            if (const auto candidateDirError = GetLastError())
            {
                AddReportLog(3127, SString("RunErrorTool failed to set candidate current directory (%s) error %u", candidateDir.c_str(), candidateDirError));
            }
            RestoreBaseDirectories();
            continue;
        }

        if (!SetDllDirectoryW(candidateDirWidePtr))
        {
            if (const auto candidateDllError = GetLastError())
            {
                AddReportLog(3128, SString("RunErrorTool failed to set candidate DLL directory (%s) error %u", candidateDir.c_str(), candidateDllError));
            }
            RestoreBaseDirectories();
            continue;
        }

        auto commandLine = SString("\"%s\" %s", candidate.c_str(), crashArgs);
        SAFE_DEBUG_OUTPUT(SString("CCrashDumpWriter::RunErrorTool - Full command line: %s\n", commandLine.c_str()).c_str());
        AddReportLog(3125, SString("RunErrorTool attempting launch with command: %s", commandLine.c_str()));

        const size_t commandLength = commandLine.length();
        if (commandLength == 0 || commandLength > std::numeric_limits<size_t>::max() - 1)
        {
            AddReportLog(3130, SString("RunErrorTool produced invalid command line for %s", candidate.c_str()));
            RestoreBaseDirectories();
            continue;
        }

        const WString commandLineWide = FromUTF8(commandLine);
        if (commandLineWide.empty())
        {
            AddReportLog(3130, SString("RunErrorTool failed to convert command line to wide characters for %s", candidate.c_str()));
            RestoreBaseDirectories();
            continue;
        }

        std::vector<wchar_t> commandBuffer(commandLineWide.length() + 1u, L'\0');
        std::copy(commandLineWide.begin(), commandLineWide.end(), commandBuffer.begin());

        STARTUPINFOW        startupInfo{.cb = sizeof(STARTUPINFOW), .dwFlags = STARTF_USESHOWWINDOW, .wShowWindow = SW_SHOWNORMAL};
        PROCESS_INFORMATION processInfo{};

        constexpr DWORD kProcessCreationFlags = DETACHED_PROCESS | CREATE_BREAKAWAY_FROM_JOB | CREATE_NEW_PROCESS_GROUP;
        const auto bProcessCreated =
            CreateProcessW(nullptr, commandBuffer.data(), nullptr, nullptr, FALSE, kProcessCreationFlags, nullptr, candidateDirWidePtr, &startupInfo, &processInfo);
        if (bProcessCreated)
        {
            SString debugMsg = SString("CCrashDumpWriter::RunErrorTool - Relaunched crash dialog via CreateProcess (%s)\n", candidate.c_str());
            SAFE_DEBUG_OUTPUT(debugMsg.c_str());
            AddReportLog(3124, SString("RunErrorTool launched crash dialog via CreateProcess (%s)", candidate.c_str()));

            if (IsValidHandle(processInfo.hProcess))
            {
                SetPriorityClass(processInfo.hProcess, HIGH_PRIORITY_CLASS);
            }

            if (processInfo.dwProcessId != 0)
            {
                AllowSetForegroundWindow(processInfo.dwProcessId);
            }

            if (IsValidHandle(processInfo.hProcess))
            {
                WaitForInputIdle(processInfo.hProcess, 3000);
            }

            Sleep(500);

            HWND hDialogWindow = nullptr;
            for (std::size_t attempts = 0; attempts < MAX_WINDOW_POLL_ATTEMPTS && hDialogWindow == nullptr; attempts++)
            {
                hDialogWindow = FindWindowW(nullptr, L"MTA: San Andreas has encountered a problem");
                if (hDialogWindow == nullptr)
                    Sleep(WINDOW_POLL_TIMEOUT_MS);
            }

            if (hDialogWindow != nullptr && IsWindow(hDialogWindow))
            {
                if (const auto isIconic = IsIconic(hDialogWindow); isIconic)
                {
                    ShowWindow(hDialogWindow, SW_RESTORE);
                }

                if (IsWindow(hDialogWindow))
                {
                    const DWORD dialogThreadId = GetWindowThreadProcessId(hDialogWindow, nullptr);
                    const DWORD currentThreadId = GetCurrentThreadId();
                    
                    BOOL attached = FALSE;
                    if (dialogThreadId != 0 && dialogThreadId != currentThreadId)
                    {
                        attached = AttachThreadInput(currentThreadId, dialogThreadId, TRUE);
                    }
                        
                    LockSetForegroundWindow(LSFW_UNLOCK);
                    SetWindowPos(hDialogWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                    
                    if (!SetForegroundWindow(hDialogWindow))
                    {
                        keybd_event(VK_MENU, 0, 0, 0);
                        SetForegroundWindow(hDialogWindow);
                        Sleep(ALT_KEY_DURATION_MS);
                        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
                    }
                    
                    BringWindowToTop(hDialogWindow);
                    SetActiveWindow(hDialogWindow);
                    SetFocus(hDialogWindow);
                        
                    if (attached && dialogThreadId != 0)
                    {
                        AttachThreadInput(currentThreadId, dialogThreadId, FALSE);
                    }
                        
                    RECT windowRect{};
                    if (GetWindowRect(hDialogWindow, &windowRect))
                    {
                        const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
                        const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
                        
                        if (windowRect.left < -100 || windowRect.top < -100 || 
                            windowRect.left > screenWidth || windowRect.top > screenHeight)
                        {
                            const int windowWidth = windowRect.right - windowRect.left;
                            const int windowHeight = windowRect.bottom - windowRect.top;
                            const int centerX = (screenWidth - windowWidth) / 2;
                            const int centerY = (screenHeight - windowHeight) / 2;
                            SetWindowPos(hDialogWindow, HWND_TOPMOST, centerX, centerY, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
                        }
                    }

                    if (!IsWindowVisible(hDialogWindow))
                    {
                        ShowWindow(hDialogWindow, SW_RESTORE);
                    }
                }
            }
            else
            {
                AddReportLog(3132, "RunErrorTool could not find crash dialog window by title");
            }

            if (IsValidHandle(processInfo.hThread))
            {
                CloseHandle(processInfo.hThread);
            }
            ms_hCrashDialogProcess = processInfo.hProcess;
            dialogLaunched = true;
            break;
        }

        if (const auto dwError = GetLastError())
        {
            AddReportLog(3121, SString("RunErrorTool CreateProcess failed with error %u for %s", dwError, candidate.c_str()));
        }

        if (crashArgsWide.empty())
        {
            AddReportLog(3131, "RunErrorTool failed to convert crash arguments to wide characters");
            RestoreBaseDirectories();
            continue;
        }

        const auto shellExecuteResult = ShellExecuteW(nullptr, L"open", candidateWide.c_str(), crashArgsWide.c_str(), candidateDirWidePtr, SW_SHOW);
        const auto shellExecuteCode = reinterpret_cast<ULONG_PTR>(shellExecuteResult);
        if (shellExecuteCode > 32u)
        {
            SString debugMsg = SString("CCrashDumpWriter::RunErrorTool - Relaunched crash dialog via ShellExecute (%s)\n", candidate.c_str());
            SAFE_DEBUG_OUTPUT(debugMsg.c_str());
            AddReportLog(3124, SString("RunErrorTool launched crash dialog via ShellExecute (%s)", candidate.c_str()));
            
            Sleep(1000);
            
            HWND hDialogWindow = nullptr;
            for (std::size_t attempts = 0; attempts < SHELL_EXEC_POLL_ATTEMPTS && hDialogWindow == nullptr; attempts++)
            {
                hDialogWindow = FindWindowW(nullptr, L"MTA: San Andreas has encountered a problem");
                if (hDialogWindow == nullptr)
                    Sleep(WINDOW_POLL_TIMEOUT_MS);
            }
            
            if (hDialogWindow != nullptr && IsWindow(hDialogWindow))
            {
                SetWindowPos(hDialogWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                SetForegroundWindow(hDialogWindow);
            }
            
            dialogLaunched = true;
            break;
        }

        AddReportLog(3122,
                     SString("RunErrorTool ShellExecute fallback failed with code %u for %s", static_cast<unsigned int>(shellExecuteCode), candidate.c_str()));
        RestoreBaseDirectories();
    }

    RestoreBaseDirectories();

    if (!dialogLaunched)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - FAILED to launch crash dialog with any candidate!\n");
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - User will not see crash report dialog!\n");
        AddReportLog(3129, "RunErrorTool completely failed to launch crash dialog");
        
        const wchar_t* emergencyMessage = 
            L"MTA: San Andreas has crashed.\n\n"
            L"The usual crash dialog has also failed, with this as fallback.\n\n"
            L"Crash information has been saved to:\n"
            L"MTA San Andreas\\mta\\core.log\n\n"
            L"Contact support on the MTA discord: https://discord.gg/RygaCSD.\n\n"
            L"The game will now close.";
        
        MessageBoxW(nullptr, emergencyMessage, L"MTA: San Andreas - Fatal Error", 
                   MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND | MB_TOPMOST);
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - Crash dialog launched successfully\n");
    }
}

void CCrashDumpWriter::AppendToDumpFile(const SString& strPathFilename, const CBuffer& dataBuffer, DWORD dwMagicStart, DWORD dwMagicEnd)
{
    CBuffer            output;
    CBufferWriteStream stream(output);
    stream.Write(static_cast<DWORD>(0));
    stream.Write(dwMagicStart);
    stream.Write(dataBuffer.GetSize());
    stream.WriteBytes(dataBuffer.GetData(), dataBuffer.GetSize());
    stream.Write(dataBuffer.GetSize());
    stream.Write(dwMagicEnd);
    stream.Write(static_cast<DWORD>(0));
    FileAppend(strPathFilename, output.GetData(), output.GetSize());
}

namespace
{
    constexpr int POOL_INFO_VERSION = 1;
    constexpr int D3D_INFO_VERSION = 2;
    constexpr int CRASH_AVERTED_STATS_VERSION = 1;
    constexpr int LOG_INFO_VERSION = 1;

    constexpr DWORD CLASS_CBuildingPool = 0xb74498;
    constexpr DWORD CLASS_CPedPool = 0xb74490;
    constexpr DWORD CLASS_CObjectPool = 0xb7449c;
    constexpr DWORD CLASS_CDummyPool = 0xb744a0;
    constexpr DWORD CLASS_CVehiclePool = 0xb74494;
    constexpr DWORD CLASS_CColModelPool = 0xb744a4;
    constexpr DWORD CLASS_CTaskPool = 0xb744a8;
    constexpr DWORD CLASS_CEventPool = 0xb744ac;
    constexpr DWORD CLASS_CTaskAllocatorPool = 0xb744bc;
    constexpr DWORD CLASS_CPedIntelligencePool = 0xb744c0;
    constexpr DWORD CLASS_CPedAttractorPool = 0xb744c4;
    constexpr DWORD CLASS_CEntryInfoNodePool = 0xb7448c;
    constexpr DWORD CLASS_CNodeRoutePool = 0xb744b8;
    constexpr DWORD CLASS_CPatrolRoutePool = 0xb744b4;
    constexpr DWORD CLASS_CPointRoutePool = 0xb744b0;
    constexpr DWORD CLASS_CPtrNodeDoubleLinkPool = 0xB74488;
    constexpr DWORD CLASS_CPtrNodeSingleLinkPool = 0xB74484;

    constexpr DWORD FUNC_CBuildingPool_GetNoOfUsedSpaces = 0x550620;
    constexpr DWORD FUNC_CPedPool_GetNoOfUsedSpaces = 0x5504A0;
    constexpr DWORD FUNC_CObjectPool_GetNoOfUsedSpaces = 0x54F6B0;
    constexpr DWORD FUNC_CDummyPool_GetNoOfUsedSpaces = 0x5507A0;
    constexpr DWORD FUNC_CVehiclePool_GetNoOfUsedSpaces = 0x42CCF0;
    constexpr DWORD FUNC_CColModelPool_GetNoOfUsedSpaces = 0x550870;
    constexpr DWORD FUNC_CTaskPool_GetNoOfUsedSpaces = 0x550940;
    constexpr DWORD FUNC_CEventPool_GetNoOfUsedSpaces = 0x550A10;
    constexpr DWORD FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces = 0x550d50;
    constexpr DWORD FUNC_CPedIntelligencePool_GetNoOfUsedSpaces = 0x550E20;
    constexpr DWORD FUNC_CPedAttractorPool_GetNoOfUsedSpaces = 0x550ef0;
    constexpr DWORD FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces = 0x5503d0;
    constexpr DWORD FUNC_CNodeRoutePool_GetNoOfUsedSpaces = 0x550c80;
    constexpr DWORD FUNC_CPatrolRoutePool_GetNoOfUsedSpaces = 0x550bb0;
    constexpr DWORD FUNC_CPointRoutePool_GetNoOfUsedSpaces = 0x550ae0;
    constexpr DWORD FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces = 0x550230;
    constexpr DWORD FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces = 0x550300;

    [[nodiscard]] int GetPoolCapacity(ePools pool)
    {
        DWORD iPtr = 0;
        DWORD cPtr = 0;
        switch (pool)
        {
            case BUILDING_POOL:
                iPtr = 0x55105F;
                break;
            case PED_POOL:
                iPtr = 0x550FF2;
                break;
            case OBJECT_POOL:
                iPtr = 0x551097;
                break;
            case DUMMY_POOL:
                iPtr = 0x5510CF;
                break;
            case VEHICLE_POOL:
                cPtr = 0x55102A;
                break;
            case COL_MODEL_POOL:
                iPtr = 0x551107;
                break;
            case TASK_POOL:
                iPtr = 0x55113F;
                break;
            case EVENT_POOL:
                iPtr = 0x551177;
                break;
            case TASK_ALLOCATOR_POOL:
                cPtr = 0x55124E;
                break;
            case PED_INTELLIGENCE_POOL:
                iPtr = 0x551283;
                break;
            case PED_ATTRACTOR_POOL:
                cPtr = 0x5512BC;
                break;
            case ENTRY_INFO_NODE_POOL:
                iPtr = 0x550FBA;
                break;
            case NODE_ROUTE_POOL:
                cPtr = 0x551219;
                break;
            case PATROL_ROUTE_POOL:
                cPtr = 0x5511E4;
                break;
            case POINT_ROUTE_POOL:
                cPtr = 0x5511AF;
                break;
            case POINTER_DOUBLE_LINK_POOL:
                iPtr = 0x550F82;
                break;
            case POINTER_SINGLE_LINK_POOL:
                iPtr = 0x550F46;
                break;
            case ENV_MAP_MATERIAL_POOL:
                iPtr = 0x5DA08E;
                break;
            case ENV_MAP_ATOMIC_POOL:
                iPtr = 0x5DA0CA;
                break;
            case SPEC_MAP_MATERIAL_POOL:
                iPtr = 0x5DA106;
                break;
        }
        if (iPtr != 0)
            return *reinterpret_cast<int*>(iPtr);

        if (cPtr != 0)
            return *reinterpret_cast<char*>(cPtr);

        return 0;
    }

    [[nodiscard]] int GetNumberOfUsedSpaces(ePools pool)
    {
        DWORD dwFunc = 0;
        DWORD dwThis = 0;
        switch (pool)
        {
            case BUILDING_POOL:
                dwFunc = FUNC_CBuildingPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CBuildingPool;
                break;
            case PED_POOL:
                dwFunc = FUNC_CPedPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CPedPool;
                break;
            case OBJECT_POOL:
                dwFunc = FUNC_CObjectPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CObjectPool;
                break;
            case DUMMY_POOL:
                dwFunc = FUNC_CDummyPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CDummyPool;
                break;
            case VEHICLE_POOL:
                dwFunc = FUNC_CVehiclePool_GetNoOfUsedSpaces;
                dwThis = CLASS_CVehiclePool;
                break;
            case COL_MODEL_POOL:
                dwFunc = FUNC_CColModelPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CColModelPool;
                break;
            case TASK_POOL:
                dwFunc = FUNC_CTaskPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CTaskPool;
                break;
            case EVENT_POOL:
                dwFunc = FUNC_CEventPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CEventPool;
                break;
            case TASK_ALLOCATOR_POOL:
                dwFunc = FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CTaskAllocatorPool;
                break;
            case PED_INTELLIGENCE_POOL:
                dwFunc = FUNC_CPedIntelligencePool_GetNoOfUsedSpaces;
                dwThis = CLASS_CPedIntelligencePool;
                break;
            case PED_ATTRACTOR_POOL:
                dwFunc = FUNC_CPedAttractorPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CPedAttractorPool;
                break;
            case ENTRY_INFO_NODE_POOL:
                dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces;
                dwThis = CLASS_CEntryInfoNodePool;
                break;
            case NODE_ROUTE_POOL:
                dwFunc = FUNC_CNodeRoutePool_GetNoOfUsedSpaces;
                dwThis = CLASS_CNodeRoutePool;
                break;
            case PATROL_ROUTE_POOL:
                dwFunc = FUNC_CPatrolRoutePool_GetNoOfUsedSpaces;
                dwThis = CLASS_CPatrolRoutePool;
                break;
            case POINT_ROUTE_POOL:
                dwFunc = FUNC_CPointRoutePool_GetNoOfUsedSpaces;
                dwThis = CLASS_CPointRoutePool;
                break;
            case POINTER_DOUBLE_LINK_POOL:
                dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CPtrNodeDoubleLinkPool;
                break;
            case POINTER_SINGLE_LINK_POOL:
                dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces;
                dwThis = CLASS_CPtrNodeSingleLinkPool;
                break;
            default:
                return -1;
        }

        int iOut = -2;
        if (*(DWORD*)dwThis != 0)
        {
            _asm
            {
                mov     ecx, dwThis
                mov     ecx, [ecx]
                call    dwFunc
                mov     iOut, eax
            }
        }

        return iOut;
    }
}            // namespace

void CCrashDumpWriter::GetPoolInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    stream.Write(POOL_INFO_VERSION);

    stream.Write(MAX_POOLS);

    for (int i = 0; i < MAX_POOLS; i++)
    {
        int iCapacity = GetPoolCapacity(static_cast<ePools>(i));
        int iUsedSpaces = GetNumberOfUsedSpaces(static_cast<ePools>(i));
        stream.Write(i);
        stream.Write(iCapacity);
        stream.Write(iUsedSpaces);
    }
}

void CCrashDumpWriter::GetD3DInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    stream.Write(D3D_INFO_VERSION);

    if (g_pDeviceState == nullptr)
        return;

    stream.Write(g_pDeviceState->CallState.callType);

    if (g_pDeviceState->CallState.callType == CProxyDirect3DDevice9::SCallState::NONE)
        return;

    stream.Write(g_pDeviceState->CallState.uiNumArgs);
    for (uint i = 0; i < g_pDeviceState->CallState.uiNumArgs; i++)
        stream.Write(g_pDeviceState->CallState.args[i]);

    CCore*       pCore = CCore::GetSingletonPtr();
    CGame*       pGame = pCore != nullptr ? pCore->GetGame() : nullptr;
    CRenderWare* pRenderWare = pGame != nullptr ? pGame->GetRenderWare() : nullptr;

    stream.Write(static_cast<uchar>(pCore != nullptr ? 1 : 0));
    stream.Write(static_cast<uchar>(pGame != nullptr ? 1 : 0));
    stream.Write(static_cast<uchar>(pRenderWare != nullptr ? 1 : 0));

    stream.Write(reinterpret_cast<uintptr_t>(g_pDeviceState->TextureState[0].Texture));

    SString strTextureName = "no name";
    if (pRenderWare != nullptr)
        strTextureName = pRenderWare->GetTextureName(reinterpret_cast<CD3DDUMMY*>(static_cast<void*>(g_pDeviceState->TextureState[0].Texture)));
    stream.WriteString(strTextureName);

    stream.WriteString("");
    stream.Write(false);
}

void CCrashDumpWriter::GetCrashAvertedStats(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    stream.Write(CRASH_AVERTED_STATS_VERSION);

    stream.Write(ms_CrashAvertedMap.size());

    for (const auto& [id, info] : ms_CrashAvertedMap)
    {
        stream.Write(ms_uiTickCountBase - info.uiTickCount);
        stream.Write(id);
        stream.Write(info.uiUsageCount);
    }
}

void CCrashDumpWriter::GetLogInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    stream.Write(LOG_INFO_VERSION);

    stream.Write(ms_LogEventList.size());

    for (const auto& eventInfo : ms_LogEventList)
    {
        stream.Write(ms_uiTickCountBase - eventInfo.uiTickCount);
        stream.WriteString(eventInfo.strType);
        stream.WriteString(eventInfo.strContext);
        stream.WriteString(eventInfo.strBody);
    }
}

void CCrashDumpWriter::GetDxInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    stream.Write(2);

    SDxStatus status;
    CGraphics::GetSingleton().GetRenderItemManager()->GetDxStatus(status);

    stream.Write(status.testMode);

    stream.WriteString(status.videoCard.strName);
    stream.Write(status.videoCard.iInstalledMemoryKB);
    stream.WriteString(status.videoCard.strPSVersion);

    stream.Write(status.videoMemoryKB.iFreeForMTA);
    stream.Write(status.videoMemoryKB.iUsedByFonts);
    stream.Write(status.videoMemoryKB.iUsedByTextures);
    stream.Write(status.videoMemoryKB.iUsedByRenderTargets);

    stream.Write(status.settings.bWindowed);
    stream.Write(status.settings.iFXQuality);
    stream.Write(status.settings.iDrawDistance);
    stream.Write(status.settings.bVolumetricShadows);
    stream.Write(status.settings.iStreamingMemory);
}

void CCrashDumpWriter::GetMiscInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    stream.Write(2);

    unsigned char ucA = 0;
    unsigned char ucB = 0;

    const bool bReadA = SafeReadGameByte(0x748ADD, ucA);
    const bool bReadB = SafeReadGameByte(0x748ADE, ucB);

    if (!bReadA || !bReadB)
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::GetMiscInfo - Failed to read GTA memory\n");

    stream.Write(ucA);
    stream.Write(ucB);

    stream.Write(ms_uiInCrashZone);
}

void CCrashDumpWriter::GetMemoryInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    stream.Write(1);

    SMemStatsInfo memStatsNow;
    GetMemStats()->SampleState(memStatsNow);

    static const CProxyDirect3DDevice9::SResourceMemory* const nowList[] = {
        &memStatsNow.d3dMemory.StaticVertexBuffer, &memStatsNow.d3dMemory.DynamicVertexBuffer, &memStatsNow.d3dMemory.StaticIndexBuffer,
        &memStatsNow.d3dMemory.DynamicIndexBuffer, &memStatsNow.d3dMemory.StaticTexture,       &memStatsNow.d3dMemory.DynamicTexture};
    int iNumLines = NUMELMS(nowList);
    stream.Write(iNumLines);
    for (int i = 0; i < iNumLines; i++)
    {
        stream.Write(nowList[i]->iLockedCount);
        stream.Write(nowList[i]->iCreatedCount);
        stream.Write(nowList[i]->iDestroyedCount);
        stream.Write(nowList[i]->iCurrentCount);
        stream.Write(nowList[i]->iCurrentBytes);
    }

    stream.Write(3);
    stream.Write(memStatsNow.iProcessMemSizeKB);
    stream.Write(memStatsNow.iStreamingMemoryUsed);
    stream.Write(memStatsNow.iStreamingMemoryAvailable);

    iNumLines = sizeof(memStatsNow.modelInfo) / sizeof(uint);
    stream.Write(iNumLines);
    for (int i = 0; i < iNumLines; i++)
    {
        stream.Write(memStatsNow.modelInfo.uiArray[i]);
    }
}

void CCrashDumpWriter::GetCurrentAnimTaskInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    stream.Write(1);
    stream.WriteString("-- ** Current Animation Task Info -- **\n\n");

    CMultiplayer* pMultiplayer = g_pCore->GetMultiplayer();
    if (pMultiplayer != nullptr)
    {
        stream.WriteString(SString("Last Animation Added: group ID = %u, animation ID = %u, CAnimManager::ms_aAnimAssocGroups = %#.8x\n",
                                   pMultiplayer->GetLastStaticAnimationGroupID(), pMultiplayer->GetLastStaticAnimationID(),
                                   pMultiplayer->GetLastAnimArrayAddress()));
    }
}

[[nodiscard]] SString CCrashDumpWriter::GetCrashAvertedStatsSoFar()
{
    SString strResult;
    ms_uiTickCountBase = GetTickCount32();

    int iIndex = 1;
    for (const auto& [id, info] : ms_CrashAvertedMap)
    {
        strResult += SString("%d) Age:%5d Type:%2d Count:%d\n", iIndex++, ms_uiTickCountBase - info.uiTickCount, id, info.uiUsageCount);
    }
    return strResult;
}
