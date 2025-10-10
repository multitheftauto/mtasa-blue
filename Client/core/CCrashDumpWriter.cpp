// file: ..\Client\core\CCrashDumpWriter.cpp
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

#include <process.h>
#include <DbgHelp.h>

#include <atomic>
#include <array>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits>
#include <ctime>
#include <utility>


static constexpr DWORD  CRASH_EXIT_CODE = 3;
static constexpr size_t LOG_EVENT_SIZE = 200;
static constexpr size_t DEBUG_BUFFER_SIZE = 256;
static constexpr uint   INVALID_PARAMETER_WARNING_THRESHOLD = 5;
static constexpr size_t INVALID_PARAMETER_SAMPLE_LIMIT = 8;
static constexpr size_t MAX_FALLBACK_STACK_FRAMES = 32;
static constexpr int    MAX_WIDE_TO_UTF8_BYTES = 1 * 1024 * 1024;

inline void SafeDebugOutput(const char* message) noexcept
{
    if (message != nullptr)
    {
        OutputDebugStringA(message);
    }
}

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
 
class SymbolHandlerGuard
{
public:
    explicit SymbolHandlerGuard(HANDLE process) noexcept : m_process(process), m_initialized(false)
    {
        if (m_process != nullptr && SymInitialize(m_process, nullptr, TRUE) != FALSE)
            m_initialized = true;
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

static bool NormalizePathForValidation(const SString& inputPath, SString& outputPath)
{
    if (inputPath.empty())
        return false;

    DWORD required = GetFullPathNameA(inputPath.c_str(), 0, nullptr, nullptr);
    if (required == 0)
        return false;

    std::vector<char> buffer(static_cast<size_t>(required) + 1u, '\0');
    DWORD written = GetFullPathNameA(inputPath.c_str(), required + 1u, buffer.data(), nullptr);
    if (written == 0 || written > required)
        return false;

    outputPath = buffer.data();
    for (size_t i = 0; i < outputPath.length(); ++i)
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

static bool ValidatePathWithinBase(const SString& baseDir, const SString& candidatePath, DWORD logId)
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

    const size_t baseLength = normalizedBase.length();
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

static std::atomic<uint> ms_uiInvalidParameterCount{0};
static std::atomic<uint> ms_uiInvalidParameterCountLogged{0};
static std::atomic<bool> ms_bInCrashHandler{false};
static std::array<SString, INVALID_PARAMETER_SAMPLE_LIMIT> ms_InvalidParameterSamples;
static std::atomic<uint>                                    ms_uiInvalidParameterSampleCount{0};
static std::atomic<bool>                                    ms_bInvalidParameterWarningStored{false};
static std::atomic<bool>                                    ms_bFallbackStackLogged{false};

static std::array<SString, 2> BuildCrashDialogCandidates()
{
    const SString basePath = GetMTASABaseDir();
    std::array<SString, 2> candidates = {
        SharedUtil::PathJoin(basePath, "Multi Theft Auto.exe"),
        SharedUtil::PathJoin(basePath, "Multi Theft Auto_d.exe")
    };

#if defined(MTA_DEBUG)
    std::swap(candidates[0], candidates[1]);
#endif

    return candidates;
}

static void EnsureCrashReasonForDialog(CExceptionInformation* pExceptionInformation) noexcept
{
    if (pExceptionInformation == nullptr)
        return;

    SString strReason = GetApplicationSetting("diagnostics", "last-crash-reason");

    if (strReason.empty())
    {
        const char* moduleBaseName = pExceptionInformation->GetModuleBaseName();
        if (moduleBaseName == nullptr || moduleBaseName[0] == '\0')
            moduleBaseName = pExceptionInformation->GetModulePathName();
        if (moduleBaseName == nullptr || moduleBaseName[0] == '\0')
            moduleBaseName = "unknown";

        strReason = SString("\n\nReason: Exception 0x%08X at %s+0x%08X", pExceptionInformation->GetCode(), moduleBaseName,
                             pExceptionInformation->GetAddressModuleOffset());

        if (ms_uiInCrashZone != 0)
            strReason += SString(" (CrashZone=%u)", ms_uiInCrashZone);

        const uint invalidParameterCount = ms_uiInvalidParameterCount.load(std::memory_order_relaxed);
        if (invalidParameterCount > 0)
            strReason += SString(" (InvalidParameterCount=%u)", invalidParameterCount);

        strReason += SString(" (ThreadId=%u)", GetCurrentThreadId());
    }

    SString pendingWarning = GetApplicationSetting("diagnostics", "pending-invalid-parameter-warning");
    if (!pendingWarning.empty())
        strReason += pendingWarning;

    SetApplicationSetting("diagnostics", "last-crash-reason", strReason);
}

static SString WideToUtf8(const wchar_t* input)
{
    if (input == nullptr || input[0] == L'\0')
        return SString();

    int required = WideCharToMultiByte(CP_UTF8, 0, input, -1, nullptr, 0, nullptr, nullptr);
    if (required <= 0)
        return SString();

    if (required > MAX_WIDE_TO_UTF8_BYTES)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::WideToUtf8 - input too large for UTF-8 conversion\n");
        AddReportLog(9212, SString("CCrashDumpWriter::WideToUtf8 - conversion request exceeds %d bytes", MAX_WIDE_TO_UTF8_BYTES));
        return SString();
    }

    std::vector<char> buffer(static_cast<size_t>(required));
    int converted = WideCharToMultiByte(CP_UTF8, 0, input, -1, buffer.data(), required, nullptr, nullptr);
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

static bool CaptureStackTraceText(_EXCEPTION_POINTERS* pException, SString& outText) noexcept
{
    if (pException == nullptr || pException->ContextRecord == nullptr)
        return false;

    outText = "";
    outText.reserve(MAX_FALLBACK_STACK_FRAMES * 128u);

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    CONTEXT    context = *pException->ContextRecord;
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

    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_FAIL_CRITICAL_ERRORS);

    alignas(SYMBOL_INFO) unsigned char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
    PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;

    for (size_t frameIndex = 0; frameIndex < MAX_FALLBACK_STACK_FRAMES; ++frameIndex)
    {
        BOOL bWalked = StackWalk64(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &frame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr);
        if (bWalked == FALSE)
            break;

        if (frame.AddrPC.Offset == 0)
            break;

        DWORD64 address = frame.AddrPC.Offset;

        SString symbolName = SString("0x%llX", static_cast<unsigned long long>(address));
        if (SymFromAddr(hProcess, address, nullptr, pSymbol) != FALSE)
            symbolName = pSymbol->Name;

        IMAGEHLP_LINE64 lineInfo{};
        lineInfo.SizeOfStruct = sizeof(lineInfo);
        DWORD lineDisplacement = 0;
        SString lineDetail = "unknown";
        if (SymGetLineFromAddr64(hProcess, address, &lineDisplacement, &lineInfo) != FALSE)
        {
            const char* fileName = lineInfo.FileName != nullptr ? lineInfo.FileName : "unknown";
            lineDetail = SString("%s:%lu", fileName, static_cast<unsigned long>(lineInfo.LineNumber));
        }

        outText += SString("#%02u %s [0x%llX] (%s)\n", static_cast<unsigned int>(frameIndex), symbolName.c_str(),
                           static_cast<unsigned long long>(address), lineDetail.c_str());
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
void AppendDumpSection(const SString& targetPath,
                              const char* tryLabel,
                              const char* successLabel,
                              DWORD tagBegin,
                              DWORD tagEnd,
                              SectionGenerator&& generator)
{
    SetApplicationSetting("diagnostics", "last-dump-extra", tryLabel);
    CBuffer buffer;
    std::forward<SectionGenerator>(generator)(buffer);
    CCrashDumpWriter::AppendToDumpFile(targetPath, buffer, tagBegin, tagEnd);
    SetApplicationSetting("diagnostics", "last-dump-extra", successLabel);
}


typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::OnCrashAverted
//
// Static function. Called everytime a crash is averted
//
///////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::OnEnterCrashZone
//
// Static function. Called when entering possible crash zone
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::OnEnterCrashZone(uint uiId)
{
    ms_uiInCrashZone = uiId;
}

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::LogEvent
//
// Static function.
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::LogEvent(const char* szType, const char* szContext, const char* szBody)
{
    ms_LogEventFilter.AddLine({szBody, szType, szContext});

    SLogEventLine line;
    while (ms_LogEventFilter.PopOutputLine(line))
    {
        SLogEventInfo info;
        info.uiTickCount = GetTickCount32();
        info.strType = line.strType;
        info.strContext = line.strContext;
        info.strBody = line.strBody;
        ms_LogEventList.push_front(info);

        while (ms_LogEventList.size() > LOG_EVENT_SIZE)
            ms_LogEventList.pop_back();
    }
}

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::SetHandlers
//
// Static function. Initialize handlers for crash situations
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::SetHandlers()
{
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Setting up crash handlers\n");

    ms_uiInvalidParameterSampleCount.store(0, std::memory_order_relaxed);
    ms_bInvalidParameterWarningStored.store(false, std::memory_order_relaxed);
    for (SString& sample : ms_InvalidParameterSamples)
        sample = "";
    SetApplicationSetting("diagnostics", "pending-invalid-parameter-warning", "");

    // Set invalid parameter handler (C runtime errors)
    _set_invalid_parameter_handler(CCrashDumpWriter::HandleInvalidParameter);
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Invalid parameter handler installed\n");

    // Install our crash handler via CrashHandler.cpp's unified system
    // This includes VEH + SEH + C++ exception handlers
    if (!SetCrashHandlerFilter(CCrashDumpWriter::HandleExceptionGlobal))
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: WARNING - Failed to install crash handler filter\n");
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Crash handler filter installed successfully\n");
    }

    // Reserve memory for crash dump processing
    CCrashDumpWriter::ReserveMemoryKBForCrashDumpProcessing(3000);
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Reserved 3000KB for crash dump processing\n");
}

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::UpdateCounters
//
// Static function. Called every so often
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::UpdateCounters()
{
    const uint currentCount = ms_uiInvalidParameterCount.load(std::memory_order_relaxed);
    const uint loggedCount = ms_uiInvalidParameterCountLogged.load(std::memory_order_relaxed);

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
            const uint sampleCount = std::min<uint>(ms_uiInvalidParameterSampleCount.load(std::memory_order_relaxed),
                                                    static_cast<uint>(INVALID_PARAMETER_SAMPLE_LIMIT));

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

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::HandleInvalidParameter
//
// Static function. Called when an invalid parameter is detected
// Can be caused by problems with localized strings.
//
///////////////////////////////////////////////////////////////
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

        ms_InvalidParameterSamples[static_cast<size_t>(sampleIndex)] =
            SString("expr=\"%s\" func=\"%s\" file=\"%s\" line=%u reserved=0x%p", exprStr.c_str(), funcStr.c_str(), fileStr.c_str(), line,
                    reinterpret_cast<void*>(pReserved));
    }

    ms_uiInvalidParameterCount.fetch_add(1, std::memory_order_relaxed);
}

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::ReserveMemoryKBForCrashDumpProcessing
// CCrashDumpWriter::FreeMemoryForCrashDumpProcessing
//
// Static functions. Keep some RAM to help avoid mem problems during crash dump saving
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::ReserveMemoryKBForCrashDumpProcessing(uint uiMemoryKB)
{
    FreeMemoryForCrashDumpProcessing();

    constexpr size_t kMaxReserveMemoryKB = 64u * 1024u;

    if (uiMemoryKB > kMaxReserveMemoryKB)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Requested reserve exceeds safety ceiling, clamping to 64 MiB\n");
        uiMemoryKB = static_cast<uint>(kMaxReserveMemoryKB);
    }

    if (uiMemoryKB > SIZE_MAX / 1024)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Memory reservation size too large\n");
        return;
    }

    ms_pReservedMemory = malloc(static_cast<size_t>(uiMemoryKB) * 1024);
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

// Handles VEH, SEH, and C++ exceptions through single entry point
// Ensures crash dump generation and crash dialog display for ALL fatal exceptions
long WINAPI CCrashDumpWriter::HandleExceptionGlobal(_EXCEPTION_POINTERS* pException)
{
    // Prevent recursive crashes using flag
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

    // Validation exception pointers
    if (pException == nullptr || pException->ExceptionRecord == nullptr)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::HandleExceptionGlobal - NULL exception pointers\n");
        TerminateProcess(GetCurrentProcess(), CRASH_EXIT_CODE);
        _exit(CRASH_EXIT_CODE);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    CExceptionInformation_Impl* pExceptionInformation = nullptr;

    // We handle exceptions via the callback's own exception handling
    pExceptionInformation = new (std::nothrow) CExceptionInformation_Impl;
    if (pExceptionInformation == nullptr)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::HandleExceptionGlobal - Failed to allocate exception information\n");
        TerminateProcess(GetCurrentProcess(), CRASH_EXIT_CODE);
        _exit(CRASH_EXIT_CODE);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    pExceptionInformation->Set(pException->ExceptionRecord->ExceptionCode, pException);

    WriteDebugEvent("CCrashDumpWriter::HandleExceptionGlobal");

    // Grab the mod manager
    bool bCrashArtifactsGenerated = false;

    CModManager* pModManager = CModManager::GetSingletonPtr();
    if (pModManager != nullptr)
    {
        // Got a client?
        if (pModManager->IsLoaded())
        {
            bool bHandled = false;
            CClientBase* pClient = pModManager->GetClient();
            const bool   bHandledSafely = InvokeClientHandleExceptionSafe(pClient, pExceptionInformation, bHandled);

            if (bHandledSafely && bHandled)
            {
                // Delete the exception record and continue to search the exception stack
                delete pExceptionInformation;
                ms_bInCrashHandler.store(false, std::memory_order_release);
                return EXCEPTION_CONTINUE_SEARCH;
            }

            if (!bHandledSafely)
            {
                // Fall through to crash dump generation when client handler faulted
                SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Client exception handler failed, proceeding with crash dump\n");
            }

            // Save tick count now
            ms_uiTickCountBase = GetTickCount32();

            // Client wants to terminate the process
            DumpCoreLog(pExceptionInformation);
            DumpMiniDump(pException, pExceptionInformation);
            RunErrorTool(pExceptionInformation);
            bCrashArtifactsGenerated = true;
        }
        else
        {
        }
    }

    // Terminate the process
    SAFE_DEBUG_OUTPUT("CCrashDumpWriter: Terminating process after crash dump\n");
    if (!bCrashArtifactsGenerated)
    {
        DumpCoreLog(pExceptionInformation);
        DumpMiniDump(pException, pExceptionInformation);
        RunErrorTool(pExceptionInformation);
    }

    delete pExceptionInformation;

    if (!TerminateProcess(GetCurrentProcess(), CRASH_EXIT_CODE))
    {
        _exit(CRASH_EXIT_CODE);
    }

    // Unreachable - but satisfies static analysis
    return EXCEPTION_EXECUTE_HANDLER;
}

void CCrashDumpWriter::DumpCoreLog(CExceptionInformation* pExceptionInformation)
{
    // Write crash flag for next launch
    FILE* pFlagFile = File::Fopen(CalcMTASAPath("mta\\core.log.flag"), "w");
    if (pFlagFile != nullptr)
    {
        fclose(pFlagFile);
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - Failed to create crash flag file\n");
    }

    // Write a log with the generic exception information
    FILE* pFile = File::Fopen(CalcMTASAPath("mta\\core.log"), "a+");
    if (pFile != nullptr)
    {
        // Header
        fprintf(pFile, "%s", "** -- Unhandled exception -- **\n\n");

        // Write the time
        time_t timeTemp;
        time(&timeTemp);

        SString strMTAVersionFull = SString("%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting("mta-version-ext").SplitRight(".", nullptr, -2));

        SString strInfo;
        strInfo += SString("Version = %s\n", strMTAVersionFull.c_str());
        strInfo += SString("Time = %s", ctime(&timeTemp));
        strInfo += SString("Module = %s\n", pExceptionInformation->GetModulePathName());
        strInfo += SString("Code = 0x%08X\n", pExceptionInformation->GetCode());
        strInfo += SString("Offset = 0x%08X\n\n", pExceptionInformation->GetAddressModuleOffset());

        // Write the register info
        strInfo += SString(
            "EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\n"
            "EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\n"
            "CS=%04X   DS=%04X  SS=%04X  ES=%04X   "
            "FS=%04X  GS=%04X\n\n",
            pExceptionInformation->GetEAX(), pExceptionInformation->GetEBX(), pExceptionInformation->GetECX(), pExceptionInformation->GetEDX(),
            pExceptionInformation->GetESI(), pExceptionInformation->GetEDI(), pExceptionInformation->GetEBP(), pExceptionInformation->GetESP(),
            pExceptionInformation->GetEIP(), pExceptionInformation->GetEFlags(), pExceptionInformation->GetCS(), pExceptionInformation->GetDS(),
            pExceptionInformation->GetSS(), pExceptionInformation->GetES(), pExceptionInformation->GetFS(), pExceptionInformation->GetGS());

        const uint invalidParameterCount = ms_uiInvalidParameterCount.load(std::memory_order_relaxed);
        strInfo += SString("InvalidParameterCount = %u\n", invalidParameterCount);
        strInfo += SString("CrashZone = %u\n", ms_uiInCrashZone);
        strInfo += SString("ThreadId = %u\n\n", GetCurrentThreadId());

        SString pendingWarning = GetApplicationSetting("diagnostics", "pending-invalid-parameter-warning");
        if (!pendingWarning.empty())
        {
            strInfo += pendingWarning;
        }

        EnsureCrashReasonForDialog(pExceptionInformation);

        fprintf(pFile, "%s", strInfo.c_str());

        // End of unhandled exception
        fprintf(pFile, "%s", "** -- End of unhandled exception -- **\n\n\n");

        // Close the file
        fclose(pFile);

        if (strInfo.empty())
        {
            strInfo = "** Crash info unavailable **\n";
        }

        // For the crash dialog
        SetApplicationSetting("diagnostics", "last-crash-info", strInfo);
        SetApplicationSetting("diagnostics", "last-crash-module", pExceptionInformation->GetModulePathName());
        SetApplicationSettingInt("diagnostics", "last-crash-code", pExceptionInformation->GetCode());
        WriteDebugEvent(strInfo.Replace("\n", " "));
        SetApplicationSetting("diagnostics", "pending-invalid-parameter-warning", "");
    }
    else
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::DumpCoreLog - Failed to open core.log file\n");
    }
}

void CCrashDumpWriter::DumpMiniDump(_EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation)
{
    WriteDebugEvent("CCrashDumpWriter::DumpMiniDump");

    bool bMiniDumpSucceeded = false;

    constexpr const char* kDbgHelpDllName = "DBGHELP.DLL";

    // Try to load the DLL from our directory
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
            const size_t dirLength = static_cast<size_t>(pSlash - modulePath.data()) + 1;            // Include trailing backslash
            const size_t remainingSpace = modulePath.size() - dirLength;
            const size_t requiredSpace = std::strlen(kDbgHelpDllName) + 1;            // +1 for null terminator

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
        UINT                      systemLen = GetSystemDirectoryA(systemPath.data(), static_cast<UINT>(systemPath.size()));

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
            const size_t dllNameLen = std::strlen(kDbgHelpDllName);
            size_t       appendIndex = static_cast<size_t>(systemLen);

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

            if (appendIndex != std::numeric_limits<size_t>::max())
            {
                const size_t required = appendIndex + dllNameLen + 1;            // +1 for null terminator
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

    // We could load a dll?
    if (hDll != nullptr)
    {
        // Grab the MiniDumpWriteDump proc address
        if (MINIDUMPWRITEDUMP pDump = nullptr; SharedUtil::TryGetProcAddress(hDll, "MiniDumpWriteDump", pDump))
        {
            // Create the file
            HANDLE hFile = CreateFileA(CalcMTASAPath("mta\\core.dmp"), GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile == INVALID_HANDLE_VALUE)
                AddReportLog(9203, SString("CCrashDumpWriter::DumpMiniDump - Could not create '%s'", *CalcMTASAPath("mta\\core.dmp")));

            if (hFile != INVALID_HANDLE_VALUE)
            {
                // Create an exception information struct
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo{};
                ExInfo.ThreadId = GetCurrentThreadId();
                ExInfo.ExceptionPointers = pException;
                ExInfo.ClientPointers = FALSE;

                // Write the dump
                BOOL bResult = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
                                     static_cast<MINIDUMP_TYPE>(MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory), &ExInfo, nullptr, nullptr);

                if (!bResult)
                {
                    AddReportLog(9204, SString("CCrashDumpWriter::DumpMiniDump - MiniDumpWriteDump failed (%08x)", GetLastError()));
                }
                else
                {
                    WriteDebugEvent("CCrashDumpWriter::DumpMiniDump - MiniDumpWriteDump succeeded");
                    bMiniDumpSucceeded = true;
                }

                // Close the dumpfile
                CloseHandle(hFile);

                // Grab the current time
                SYSTEMTIME SystemTime;
                GetLocalTime(&SystemTime);

                // Create the dump directory
                if (!CreateDirectoryA(CalcMTASAPath("mta\\dumps"), nullptr))
                {
                    DWORD dwError = GetLastError();
                    if (dwError != ERROR_ALREADY_EXISTS)
                    {
                        AddReportLog(9207, SString("CCrashDumpWriter::DumpMiniDump - Failed to create dumps directory (%08x)", dwError));
                    }
                }

                if (!CreateDirectoryA(CalcMTASAPath("mta\\dumps\\private"), nullptr))
                {
                    DWORD dwError = GetLastError();
                    if (dwError != ERROR_ALREADY_EXISTS)
                    {
                        AddReportLog(9208, SString("CCrashDumpWriter::DumpMiniDump - Failed to create private dumps directory (%08x)", dwError));
                    }
                }

                SString strModuleName = pExceptionInformation->GetModuleBaseName();
                strModuleName = strModuleName.ReplaceI(".dll", "").Replace(".exe", "").Replace("_", "").Replace(".", "").Replace("-", "");
                if (strModuleName.length() == 0)
                    strModuleName = "unknown";

                SString strMTAVersionFull = SString("%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting("mta-version-ext").SplitRight(".", nullptr, -2));
                SString strSerialPart = GetApplicationSetting("serial").substr(0, 5);
                uint    uiServerIP = GetApplicationSettingInt("last-server-ip");
                uint    uiServerPort = GetApplicationSettingInt("last-server-port");
                int     uiServerTime = GetApplicationSettingInt("last-server-time");
                const __time64_t currentTime = _time64(nullptr);

                __int64 rawDuration = 0;
                if (currentTime != static_cast<__time64_t>(-1))
                {
                    const __time64_t lastServerTime = static_cast<__time64_t>(uiServerTime);
                    rawDuration = currentTime - lastServerTime;
                }

                if (rawDuration < 0)
                    rawDuration = 0;

                // Leave headroom for the +1 adjustment below to avoid overflow
                if (rawDuration > std::numeric_limits<__int64>::max() - 1)
                    rawDuration = std::numeric_limits<__int64>::max() - 1;

                // Uploader protocol expects values in the range [1, 0x0fff] where 0 encodes "no duration"
                // Keep the +1 offset to preserve compatibility while ensuring the arithmetic stays in range.
                __int64 adjustedDuration = rawDuration + 1;
                if (adjustedDuration > 0x0fff)
                    adjustedDuration = 0x0fff;

                const int uiServerDuration = static_cast<int>(adjustedDuration);

                // Get path to mta dir
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

                // Ensure filename parts match up with EDumpFileNameParts
                SString strFilename("mta\\dumps\\private\\client_%s_%s_%08x_%x_%s_%08X_%04X_%03X_%s_%04d%02d%02d_%02d%02d.dmp", strMTAVersionFull.c_str(),
                                    strModuleName.c_str(), pExceptionInformation->GetAddressModuleOffset(), pExceptionInformation->GetCode() & 0xffff,
                                    strPathCode.c_str(), uiServerIP, uiServerPort, uiServerDuration, strSerialPart.c_str(), SystemTime.wYear,
                                    SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute);

                const SString privateDumpDir = CalcMTASAPath("mta\\dumps\\private");
                const SString strPathFilename = CalcMTASAPath(strFilename);

                if (!ValidatePathWithinBase(privateDumpDir, strPathFilename, 9213))
                {
                    AddReportLog(9213, SString("CCrashDumpWriter::DumpMiniDump - rejected dump path (%s)", strPathFilename.c_str()));
                    SetApplicationSetting("diagnostics", "last-dump-extra", "invalid-path");
                    SetApplicationSetting("diagnostics", "last-dump-save", "");
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
                    }
                    else
                    {
                        // For the crashdump uploader
                        SetApplicationSetting("diagnostics", "last-dump-extra", "none");
                        SetApplicationSetting("diagnostics", "last-dump-save", strPathFilename);

                        AppendDumpSection(strPathFilename, "try-pools", "added-pools", 'POLs', 'POLe', [](CBuffer& buffer) {
                            CCrashDumpWriter::GetPoolInfo(buffer);
                        });

                        AppendDumpSection(strPathFilename, "try-d3d", "added-d3d", 'D3Ds', 'D3De', [](CBuffer& buffer) {
                            CCrashDumpWriter::GetD3DInfo(buffer);
                        });

                        AppendDumpSection(strPathFilename, "try-crash-averted", "added-crash-averted", 'CASs', 'CASe', [](CBuffer& buffer) {
                            CCrashDumpWriter::GetCrashAvertedStats(buffer);
                        });

                        AppendDumpSection(strPathFilename, "try-log", "added-log", 'LOGs', 'LOGe', [](CBuffer& buffer) {
                            CCrashDumpWriter::GetLogInfo(buffer);
                        });

                        AppendDumpSection(strPathFilename, "try-misc", "added-misc", 'DXIs', 'DXIe', [](CBuffer& buffer) {
                            CCrashDumpWriter::GetDxInfo(buffer);
                        });

                        AppendDumpSection(strPathFilename, "try-misc", "added-misc", 'MSCs', 'MSCe', [](CBuffer& buffer) {
                            CCrashDumpWriter::GetMiscInfo(buffer);
                        });

                        AppendDumpSection(strPathFilename, "try-mem", "added-mem", 'MEMs', 'MEMe', [](CBuffer& buffer) {
                            CCrashDumpWriter::GetMemoryInfo(buffer);
                        });

                        AppendDumpSection(strPathFilename, "try-logfile", "added-logfile", 'LOGs', 'LOGe', [](CBuffer& buffer) {
                            buffer.LoadFromFile(CalcMTASAPath(PathJoin("mta", "logs", "logfile.txt")));
                        });

                        AppendDumpSection(strPathFilename, "try-report", "added-report", 'REPs', 'REPe', [](CBuffer& buffer) {
                            buffer.LoadFromFile(PathJoin(GetMTADataPath(), "report.log"));
                        });

                        AppendDumpSection(strPathFilename, "try-anim-task", "added-anim-task", 'CATs', 'CATe', [](CBuffer& buffer) {
                            CCrashDumpWriter::GetCurrentAnimTaskInfo(buffer);
                        });
                    }
                }
            }
        }
        else
        {
            AddReportLog(9202, "CCrashDumpWriter::DumpMiniDump - Could not find MiniDumpWriteDump");
        }

        // Free the DLL again
        FreeLibrary(hDll);
    }

    if (!bMiniDumpSucceeded)
    {
        AppendFallbackStackTrace(pException);
    }

    // Auto-fixes

    // Check if crash was in volumetric shadow code
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
    // MTA Error Reporter is now integrated into the launcher

    // Only do once
    static std::atomic<bool> bDoneReport{false};
    bool                     expected = false;
    if (!bDoneReport.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed))
        return;

    // Log the basic exception information
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

    // Try relaunch with crashed flag to display crash dialog
    const SString basePath = GetMTASABaseDir();
    const WString basePathWide = basePath.empty() ? WString() : FromUTF8(basePath);

    auto RestoreBaseDirectories = [&]() {
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

    bool dialogLaunched = false;
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
            const DWORD attrs = GetFileAttributesW(candidateWide.c_str());
            candidateExists = (attrs != INVALID_FILE_ATTRIBUTES) && ((attrs & FILE_ATTRIBUTE_DIRECTORY) == 0);
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
            const DWORD candidateDirError = GetLastError();
            AddReportLog(3127, SString("RunErrorTool failed to set candidate current directory (%s) error %u", candidateDir.c_str(), candidateDirError));
            RestoreBaseDirectories();
            continue;
        }

        if (!SetDllDirectoryW(candidateDirWidePtr))
        {
            const DWORD candidateDllError = GetLastError();
            AddReportLog(3128, SString("RunErrorTool failed to set candidate DLL directory (%s) error %u", candidateDir.c_str(), candidateDllError));
            RestoreBaseDirectories();
            continue;
        }

        SString commandLine = SString("\"%s\" %s", candidate.c_str(), crashArgs);
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

        STARTUPINFOW        startupInfo{};
        PROCESS_INFORMATION processInfo{};
        startupInfo.cb = sizeof(startupInfo);

        if (candidateWide.empty())
        {
            AddReportLog(3123, SString("RunErrorTool failed to convert candidate path to wide characters (%s)", candidate.c_str()));
            RestoreBaseDirectories();
            continue;
        }

        BOOL bProcessCreated = CreateProcessW(candidateWide.c_str(), commandBuffer.data(), nullptr, nullptr, FALSE, 0, nullptr, candidateDirWidePtr, &startupInfo, &processInfo);
        if (bProcessCreated)
        {
            SString debugMsg = SString("CCrashDumpWriter::RunErrorTool - Relaunched crash dialog via CreateProcess (%s)\n", candidate.c_str());
            SAFE_DEBUG_OUTPUT(debugMsg.c_str());
            AddReportLog(3124, SString("RunErrorTool launched crash dialog via CreateProcess (%s)", candidate.c_str()));
            CloseHandle(processInfo.hThread);
            CloseHandle(processInfo.hProcess);
            dialogLaunched = true;
            break;
        }

        const DWORD dwError = GetLastError();
        AddReportLog(3121, SString("RunErrorTool CreateProcess failed with error %u for %s", dwError, candidate.c_str()));
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - CreateProcess failed, attempting ShellExecute fallback\n");

        if (crashArgsWide.empty())
        {
            AddReportLog(3131, "RunErrorTool failed to convert crash arguments to wide characters");
            RestoreBaseDirectories();
            continue;
        }

        HINSTANCE hResult = ShellExecuteW(nullptr, L"open", candidateWide.c_str(), crashArgsWide.c_str(), candidateDirWidePtr, SW_SHOWNORMAL);
        if (reinterpret_cast<ULONG_PTR>(hResult) > 32u)
        {
            SString debugMsg = SString("CCrashDumpWriter::RunErrorTool - Relaunched crash dialog via ShellExecute (%s)\n", candidate.c_str());
            SAFE_DEBUG_OUTPUT(debugMsg.c_str());
            AddReportLog(3124, SString("RunErrorTool launched crash dialog via ShellExecute (%s)", candidate.c_str()));
            dialogLaunched = true;
            break;
        }

        AddReportLog(3122, SString("RunErrorTool ShellExecute fallback failed with code %u for %s",
                                    static_cast<unsigned int>(reinterpret_cast<ULONG_PTR>(hResult)), candidate.c_str()));
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - ShellExecute fallback failed to launch crash dialog\n");
        RestoreBaseDirectories();
    }

    RestoreBaseDirectories();

    if (!dialogLaunched)
    {
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::RunErrorTool - Failed to relaunch crash dialog with any candidate\n");
    }
}

//
// Add extra data to the dump file and hope visual studio doesn't mind
//
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

//
// Helper functions for GetPoolInfo
//
namespace
{
    #define CLASS_CBuildingPool 0xb74498
    #define CLASS_CPedPool 0xb74490
    #define CLASS_CObjectPool 0xb7449c
    #define CLASS_CDummyPool 0xb744a0
    #define CLASS_CVehiclePool 0xb74494
    #define CLASS_CColModelPool 0xb744a4
    #define CLASS_CTaskPool 0xb744a8
    #define CLASS_CEventPool 0xb744ac
    #define CLASS_CTaskAllocatorPool 0xb744bc
    #define CLASS_CPedIntelligencePool 0xb744c0
    #define CLASS_CPedAttractorPool 0xb744c4
    #define CLASS_CEntryInfoNodePool 0xb7448c
    #define CLASS_CNodeRoutePool 0xb744b8
    #define CLASS_CPatrolRoutePool 0xb744b4
    #define CLASS_CPointRoutePool 0xb744b0
    #define CLASS_CPtrNodeDoubleLinkPool 0xB74488
    #define CLASS_CPtrNodeSingleLinkPool 0xB74484

    #define FUNC_CBuildingPool_GetNoOfUsedSpaces 0x550620
    #define FUNC_CPedPool_GetNoOfUsedSpaces 0x5504A0
    #define FUNC_CObjectPool_GetNoOfUsedSpaces 0x54F6B0
    #define FUNC_CDummyPool_GetNoOfUsedSpaces 0x5507A0
    #define FUNC_CVehiclePool_GetNoOfUsedSpaces 0x42CCF0
    #define FUNC_CColModelPool_GetNoOfUsedSpaces 0x550870
    #define FUNC_CTaskPool_GetNoOfUsedSpaces 0x550940
    #define FUNC_CEventPool_GetNoOfUsedSpaces 0x550A10
    #define FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces 0x550d50
    #define FUNC_CPedIntelligencePool_GetNoOfUsedSpaces 0x550E20
    #define FUNC_CPedAttractorPool_GetNoOfUsedSpaces 0x550ef0
    #define FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces 0x5503d0
    #define FUNC_CNodeRoutePool_GetNoOfUsedSpaces 0x550c80
    #define FUNC_CPatrolRoutePool_GetNoOfUsedSpaces 0x550bb0
    #define FUNC_CPointRoutePool_GetNoOfUsedSpaces 0x550ae0
    #define FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces 0x550230
    #define FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces 0x550300

    int GetPoolCapacity(ePools pool)
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
            return *(int*)iPtr;

        if (cPtr != 0)
            return *(char*)cPtr;

        return 0;
    }

    int GetNumberOfUsedSpaces(ePools pool)
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

//
// Grab the state of the memory pools
//
void CCrashDumpWriter::GetPoolInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    // Write PoolInfo version
    stream.Write(1);

    // Write number of pools we have info on
    stream.Write(MAX_POOLS);

    // For each pool
    for (int i = 0; i < MAX_POOLS; i++)
    {
        int iCapacity = GetPoolCapacity(static_cast<ePools>(i));
        int iUsedSpaces = GetNumberOfUsedSpaces(static_cast<ePools>(i));
        // Write pool info
        stream.Write(i);
        stream.Write(iCapacity);
        stream.Write(iUsedSpaces);
    }
}

//
// Grab the state of D3D
//
void CCrashDumpWriter::GetD3DInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    // Write D3DInfo version
    stream.Write(2);

    // Quit if device state pointer is not valid
    if (g_pDeviceState == nullptr)
        return;

    // Write D3D call type
    stream.Write(g_pDeviceState->CallState.callType);

    // Only record state if crash was inside D3D
    if (g_pDeviceState->CallState.callType == CProxyDirect3DDevice9::SCallState::NONE)
        return;

    // Write D3D call args
    stream.Write(g_pDeviceState->CallState.uiNumArgs);
    for (uint i = 0; i < g_pDeviceState->CallState.uiNumArgs; i++)
        stream.Write(g_pDeviceState->CallState.args[i]);

    // Try to get CRenderWare pointer
    CCore*       pCore = CCore::GetSingletonPtr();
    CGame*       pGame = pCore != nullptr ? pCore->GetGame() : nullptr;
    CRenderWare* pRenderWare = pGame != nullptr ? pGame->GetRenderWare() : nullptr;

    // Write on how we got on with doing that
    stream.Write(static_cast<uchar>(pCore != nullptr ? 1 : 0));
    stream.Write(static_cast<uchar>(pGame != nullptr ? 1 : 0));
    stream.Write(static_cast<uchar>(pRenderWare != nullptr ? 1 : 0));

    // Write last used texture D3D pointer without truncation
    stream.Write(reinterpret_cast<uintptr_t>(g_pDeviceState->TextureState[0].Texture));

    // Write last used texture name
    SString strTextureName = "no name";
    if (pRenderWare != nullptr)
        strTextureName = pRenderWare->GetTextureName(reinterpret_cast<CD3DDUMMY*>(static_cast<void*>(g_pDeviceState->TextureState[0].Texture)));
    stream.WriteString(strTextureName);

    // Write shader name if being used
    stream.WriteString("");
    stream.Write(false);
}

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetCrashAvertedStats
//
// Static function
// Grab the crash averted stats
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetCrashAvertedStats(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    // Write info version
    stream.Write(2);

    // Write number of stats
    stream.Write(ms_CrashAvertedMap.size());

    // Write stats
    for (std::map<int, SCrashAvertedInfo>::iterator iter = ms_CrashAvertedMap.begin(); iter != ms_CrashAvertedMap.end(); ++iter)
    {
        stream.Write(ms_uiTickCountBase - iter->second.uiTickCount);
        stream.Write(iter->first);
        stream.Write(iter->second.uiUsageCount);
    }
}

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetLogInfo
//
// Static function
// Grab log info
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetLogInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    // Write info version
    stream.Write(1);

    // Write number of stats
    stream.Write(ms_LogEventList.size());

    // Write stats
    for (std::list<SLogEventInfo>::iterator iter = ms_LogEventList.begin(); iter != ms_LogEventList.end(); ++iter)
    {
        stream.Write(ms_uiTickCountBase - iter->uiTickCount);
        stream.WriteString(iter->strType);
        stream.WriteString(iter->strContext);
        stream.WriteString(iter->strBody);
    }
}

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetDxInfo
//
// Static function
// Grab our dx datum
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetDxInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    // Write info version
    stream.Write(2);

    // video card name etc..
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

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetMiscInfo
//
// Static function
// Grab various bits 'n' bobs
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetMiscInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    // Write info version
    stream.Write(2);

    // Protect hardcoded memory reads
    unsigned char ucA = 0;
    unsigned char ucB = 0;

    const bool bReadA = SafeReadGameByte(0x748ADD, ucA);
    const bool bReadB = SafeReadGameByte(0x748ADE, ucB);

    if (!bReadA || !bReadB)
        SAFE_DEBUG_OUTPUT("CCrashDumpWriter::GetMiscInfo - Failed to read GTA memory\n");

    stream.Write(ucA);
    stream.Write(ucB);

    // Crash zone if any
    stream.Write(ms_uiInCrashZone);
}

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetMemoryInfo
//
// Static function
// Same stuff as from showmemstat command
//
///////////////////////////////////////////////////////////////
void CCrashDumpWriter::GetMemoryInfo(CBuffer& buffer)
{
    CBufferWriteStream stream(buffer);

    // Write info version
    stream.Write(1);

    SMemStatsInfo memStatsNow;
    GetMemStats()->SampleState(memStatsNow);

    // GTA video memory
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

    // Game memory
    stream.Write(3);
    stream.Write(memStatsNow.iProcessMemSizeKB);
    stream.Write(memStatsNow.iStreamingMemoryUsed);
    stream.Write(memStatsNow.iStreamingMemoryAvailable);

    // Model usage
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

    // Write info version
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

///////////////////////////////////////////////////////////////
//
// CCrashDumpWriter::GetCrashAvertedStatsSoFar
//
// Static function
// Grab the crash averted stats
//
///////////////////////////////////////////////////////////////
SString CCrashDumpWriter::GetCrashAvertedStatsSoFar()
{
    SString strResult;
    ms_uiTickCountBase = GetTickCount32();

    int iIndex = 1;
    for (std::map<int, SCrashAvertedInfo>::iterator iter = ms_CrashAvertedMap.begin(); iter != ms_CrashAvertedMap.end(); ++iter)
    {
        strResult +=
            SString("%d) Age:%5d Type:%2d Count:%d\n", iIndex++, ms_uiTickCountBase - iter->second.uiTickCount, iter->first, iter->second.uiUsageCount);
    }
    return strResult;
}