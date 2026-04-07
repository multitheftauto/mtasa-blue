/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CCrashHandler.cpp
 *  PURPOSE:     Crash handler class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCrashHandler.h"
#include "version.h"
#ifdef WIN32
    #include "CExceptionInformation_Impl.h"
#else
    #include <execinfo.h>
#endif

// clang-format off
#ifndef WIN32
    #if __has_include(<ncursesw/curses.h>)
        #include <ncursesw/curses.h>
    #else
        #include <ncurses.h>
    #endif

extern "C" WINDOW* m_wndMenu;
extern "C" WINDOW* m_wndInput;
extern "C" bool    g_bNoCurses;
extern bool        g_bSilent;

    #ifdef __APPLE__
        #include <client/mac/handler/exception_handler.h>
bool DumpCallback(const char* dump_dir, const char* minidump_id, void* context, bool succeeded);
    #else
        #include <client/linux/handler/exception_handler.h>
bool DumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded);
    #endif

static SString ms_strDumpPathFilename;
#endif
// clang-format on

static SString ms_strDumpPath;

#ifdef WIN32
    #include <cstring>
    #include <ctime>
    #include <new>
    #include <tchar.h>
    #include <dbghelp.h>

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

static HMODULE          ms_hDbgHelp = NULL;
static MINIDUMPWRITEDUMP ms_pMiniDumpWriteDump = nullptr;

#endif

void CCrashHandler::Init(const SString& strInServerPath)
{
    SString strServerPath = strInServerPath;
    if (strServerPath == "")
        strServerPath = GetSystemCurrentDirectory();
    ms_strDumpPath = PathJoin(strServerPath, SERVER_DUMP_PATH);

// Set a global filter
#ifdef WIN32
    // Load dbghelp.dll now while the loader lock is not held. During a crash
    // the loader lock may already be owned, so calling LoadLibrary at that
    // point would deadlock.
    {
        char szDbgHelpPath[MAX_PATH];
        if (GetModuleFileNameA(NULL, szDbgHelpPath, MAX_PATH))
        {
            char* pSlash = _tcsrchr(szDbgHelpPath, '\\');
            if (pSlash)
            {
                _tcscpy(pSlash + 1, "DBGHELP.DLL");
                ms_hDbgHelp = LoadLibrary(szDbgHelpPath);
            }
        }
        if (!ms_hDbgHelp)
            ms_hDbgHelp = LoadLibrary("DBGHELP.DLL");

        if (ms_hDbgHelp)
        {
            const auto procAddr = GetProcAddress(ms_hDbgHelp, "MiniDumpWriteDump");
            static_assert(sizeof(ms_pMiniDumpWriteDump) == sizeof(procAddr), "Unexpected function pointer size");
            if (procAddr)
                std::memcpy(&ms_pMiniDumpWriteDump, &procAddr, sizeof(ms_pMiniDumpWriteDump));
        }
    }

    SetCrashHandlerFilter(HandleExceptionGlobal);
#else

    // Prepare initial dumpfile name
    time_t     pTime = time(NULL);
    struct tm* tm = localtime(&pTime);
    SString strFilename("server_%s_%04d%02d%02d_%02d%02d.dmp", MTA_DM_BUILDTAG_LONG, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
    ms_strDumpPathFilename = PathJoin(ms_strDumpPath, strFilename);
    MakeSureDirExists(ms_strDumpPathFilename);

    #ifdef WITH_BACKTRACE_ONLY
    signal(SIGSEGV, HandleExceptionGlobal);
    signal(SIGFPE, HandleExceptionGlobal);
    signal(SIGBUS, HandleExceptionGlobal);
    signal(SIGILL, HandleExceptionGlobal);
    signal(SIGABRT, HandleExceptionGlobal);
    #else
        #ifdef __APPLE__
    static google_breakpad::ExceptionHandler eh(ms_strDumpPath, NULL, DumpCallback, NULL, true, NULL);
        #else
    google_breakpad::MinidumpDescriptor      descriptor(ms_strDumpPath);
    static google_breakpad::ExceptionHandler eh(descriptor, NULL, DumpCallback, NULL, true, -1);
        #endif
    #endif
#endif
}

#ifndef WIN32

// Save basic backtrace info into a file. Forced inline to avoid backtrace pollution
inline __attribute__((always_inline)) static void SaveBacktraceSummary()
{
    // Collect backtrace information
    void* buffer[100];
    int   iAmount = backtrace(buffer, sizeof buffer);
    iAmount = std::min<int>(iAmount, NUMELMS(buffer));
    char** symbols = backtrace_symbols(buffer, iAmount);

    // Generate a .log file
    time_t     pTime = time(NULL);
    struct tm* tm = localtime(&pTime);

    SString sFileName;
    sFileName.Format("server_%s_%04d%02d%02d_%02d%02d.log", MTA_DM_BUILDTYPE, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);

    SString sContent;
    sContent += SString("MTA:SA Server v%s-r%d-%s crash report.\n", MTA_DM_VERSIONSTRING, MTASA_VERSION_BUILD, MTA_DM_BUILDTYPE);
    sContent += SString("%04d-%02d-%02d %02d:%02d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
    sContent += SString("Caught %d addresses ...\n\n", iAmount);
    sContent += "Backtrace:\n";

    if (symbols)
    {
        for (int i = 0; i < iAmount; i++)
        {
            if (symbols[i])
            {
                sContent += SString("#%d - %s\n", i, symbols[i]);
            }
        }
    }
    sContent += std::string(80, '-') + "\n";

    // Write the content to the file and close
    MakeSureDirExists(PathJoin(ms_strDumpPath, sFileName));
    FileAppend(PathJoin(ms_strDumpPath, sFileName), sContent);
    FileAppend(PathJoin(ms_strDumpPath, "server_pending_upload.log"), sContent);

    free(symbols);

    // Try to close window gracefully
    if (!g_bSilent && !g_bNoCurses && m_wndInput)
    {
        if (m_wndMenu)
        {
            delwin(m_wndMenu);
            m_wndMenu = NULL;
        }
        delwin(m_wndInput);
        m_wndInput = NULL;
        endwin();
    }
}

    // Linux/Mac crash callback when using google-breakpad
    #ifdef __APPLE__
bool DumpCallback(const char* dump_dir, const char* minidump_id, void* context, bool succeeded)
{
    auto path = PathJoin(dump_dir, SString("%s.dmp", minidump_id));
    #else
bool DumpCallback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded)
{
    auto path = descriptor.path();
    #endif

    SString strFinalDumpPathFilename;

    if (succeeded)
    {
        // Set inital dump file name (Safeish)
        File::Rename(path, ms_strDumpPathFilename);

        // Set final dump file name (Not so safe)
        time_t     pTime = time(NULL);
        struct tm* tm = localtime(&pTime);
        SString strFilename("server_%s_%04d%02d%02d_%02d%02d.dmp", MTA_DM_BUILDTAG_LONG, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
        strFinalDumpPathFilename = PathJoin(ms_strDumpPath, strFilename);
        File::Rename(ms_strDumpPathFilename, strFinalDumpPathFilename);
    }

    SaveBacktraceSummary();

    if (succeeded)
        FileSave(PathJoin(ms_strDumpPath, "server_pending_upload_filename"), strFinalDumpPathFilename);

    // Return false to indicate exception has not been handled (and allow core dump?)
    return false;
}

void CCrashHandler::HandleExceptionGlobal(int iSig)
{
    SaveBacktraceSummary();
    exit(EXIT_FAILURE);
}

#else

long WINAPI CCrashHandler::HandleExceptionGlobal(_EXCEPTION_POINTERS* pException)
{
    // Create the exception information class
    CExceptionInformation_Impl* pExceptionInformation = new (std::nothrow) CExceptionInformation_Impl;
    if (!pExceptionInformation)
    {
        TerminateProcess(GetCurrentProcess(), 1);
        return EXCEPTION_CONTINUE_SEARCH;
    }
    pExceptionInformation->Set(pException->ExceptionRecord->ExceptionCode, pException);

    // Write the dump
    DumpMiniDump(pException, pExceptionInformation);
    RunErrorTool();
    TerminateProcess(GetCurrentProcess(), 1);
    return EXCEPTION_CONTINUE_SEARCH;
}

// Call MiniDumpWriteDump under SEH. The dump writer can crash if the process
// heap is corrupt; catching that crash here lets the handler clean up its
// file handles and exit the process cleanly.
static bool TryWriteDump(MINIDUMPWRITEDUMP pDump, HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE dumpType, _MINIDUMP_EXCEPTION_INFORMATION* pExInfo)
{
#ifdef _MSC_VER
    BOOL bResult = FALSE;
    __try
    {
        bResult = pDump(hProcess, dwPid, hFile, dumpType, pExInfo, NULL, NULL);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
    return bResult != FALSE;
#else
    BOOL bResult = pDump(hProcess, dwPid, hFile, dumpType, pExInfo, NULL, NULL);
    return bResult != FALSE;
#endif
}

void CCrashHandler::DumpMiniDump(_EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation)
{
    MINIDUMPWRITEDUMP pDump = ms_pMiniDumpWriteDump;

    if (pDump)
    {
        // Grab the current time
        // Ask windows for the system time.
        SYSTEMTIME SystemTime;
        GetLocalTime(&SystemTime);

        // Create the dump directory
        CreateDirectory(ms_strDumpPath, 0);
        CreateDirectory(PathJoin(ms_strDumpPath, "private"), 0);

        SString strModuleName = pExceptionInformation->GetModuleBaseName();
        strModuleName = strModuleName.ReplaceI(".dll", "").Replace(".exe", "").Replace("_", "").Replace(".", "").Replace("-", "");
        if (strModuleName.length() == 0)
            strModuleName = "unknown";

    #ifdef _WIN64
        strModuleName += "64";
    #endif

        SString strFilename("server_%s_%s_%08x_%x_%04d%02d%02d_%02d%02d.dmp", MTA_DM_BUILDTAG_LONG, strModuleName.c_str(),
                            pExceptionInformation->GetAddressModuleOffset(), pExceptionInformation->GetCode() & 0xffff, SystemTime.wYear, SystemTime.wMonth,
                            SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute);

        SString strFinalDumpPathFilename = PathJoin(ms_strDumpPath, "private", strFilename);

        // Write the log before attempting the dump. If MiniDumpWriteDump crashes
        // below (which happens when the process heap is corrupt), the log is
        // already on disk and still captures the original exception info.
        {
            FILE* pFile = File::Fopen(PathJoin(ms_strDumpPath, "server_pending_upload.log"), "a+");
            if (pFile)
            {
                // Header
                fprintf(pFile, "%s", "** -- Unhandled exception -- **\n\n");

                // Write the time
                time_t timeTemp;
                time(&timeTemp);

                SString strMTAVersionFull = SString("%s", MTA_DM_BUILDTAG_LONG);

                SString strInfo;
                strInfo += SString("Version = %s\n", strMTAVersionFull.c_str());

                char szTimeBuf[64];
                struct tm tmLocal;
                localtime_s(&tmLocal, &timeTemp);
                strftime(szTimeBuf, sizeof(szTimeBuf), "%c\n", &tmLocal);
                strInfo += SString("Time = %s", szTimeBuf);

                const char* szModulePath = pExceptionInformation->GetModulePathName();
                strInfo += SString("Module = %s\n", szModulePath ? szModulePath : "");

                // Write the basic exception information
                strInfo += SString("Code = 0x%08X\n", pExceptionInformation->GetCode());
                strInfo += SString("Offset = 0x%08X\n\n", pExceptionInformation->GetAddressModuleOffset());

                // Write the register info
    #ifdef WIN_x64
                strInfo += SString(
                    "RAX=%016llX  RBX=%016llX  RCX=%016llX\n"
                    "RDX=%016llX  RSI=%016llX  RDI=%016llX\n"
                    "RBP=%016llX  RSP=%016llX  RIP=%016llX\n"
                    "R8 =%016llX  R9 =%016llX  R10=%016llX\n"
                    "R11=%016llX  R12=%016llX  R13=%016llX\n"
                    "R14=%016llX  R15=%016llX  FLG=%08X\n"
                    "CS=%04X   DS=%04X  SS=%04X  ES=%04X   "
                    "FS=%04X  GS=%04X\n\n",
                    pExceptionInformation->GetRAX(), pExceptionInformation->GetRBX(), pExceptionInformation->GetRCX(),
                    pExceptionInformation->GetRDX(), pExceptionInformation->GetRSI(), pExceptionInformation->GetRDI(),
                    pExceptionInformation->GetRBP(), pExceptionInformation->GetRSP(), pExceptionInformation->GetRIP(),
                    pExceptionInformation->GetR8(), pExceptionInformation->GetR9(), pExceptionInformation->GetR10(),
                    pExceptionInformation->GetR11(), pExceptionInformation->GetR12(), pExceptionInformation->GetR13(),
                    pExceptionInformation->GetR14(), pExceptionInformation->GetR15(), pExceptionInformation->GetEFlags(),
                    pExceptionInformation->GetCS(), pExceptionInformation->GetDS(),
                    pExceptionInformation->GetSS(), pExceptionInformation->GetES(), pExceptionInformation->GetFS(), pExceptionInformation->GetGS());
    #else
                strInfo += SString(
                    "EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\n"
                    "EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\n"
                    "CS=%04X   DS=%04X  SS=%04X  ES=%04X   "
                    "FS=%04X  GS=%04X\n\n",
                    pExceptionInformation->GetEAX(), pExceptionInformation->GetEBX(), pExceptionInformation->GetECX(), pExceptionInformation->GetEDX(),
                    pExceptionInformation->GetESI(), pExceptionInformation->GetEDI(), pExceptionInformation->GetEBP(), pExceptionInformation->GetESP(),
                    pExceptionInformation->GetEIP(), pExceptionInformation->GetEFlags(), pExceptionInformation->GetCS(), pExceptionInformation->GetDS(),
                    pExceptionInformation->GetSS(), pExceptionInformation->GetES(), pExceptionInformation->GetFS(), pExceptionInformation->GetGS());
    #endif

                fprintf(pFile, "%s", strInfo.c_str());

                // End of unhandled exception
                fprintf(pFile, "%s", "** -- End of unhandled exception -- **\n\n\n");

                // Flush before close. fflush writes directly to the OS (no heap
                // alloc), so it succeeds even if the heap is corrupt. fclose
                // may fault when freeing internal buffers in that case.
                fflush(pFile);

                // Close the file
                fclose(pFile);
            }
        }

        // Create the file
        HANDLE hFile = CreateFile(strFinalDumpPathFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
            ExInfo.ThreadId = GetCurrentThreadId();
            ExInfo.ExceptionPointers = pException;
            ExInfo.ClientPointers = FALSE;

            // MiniDumpWriteDump can crash if the process heap is corrupt at the time
            // of the fault. TryWriteDump catches the secondary exception so this
            // handler can finish cleanly.
            const bool bDumpWritten = TryWriteDump(pDump, GetCurrentProcess(), GetCurrentProcessId(), hFile,
                                                   (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory), &ExInfo);

            // Close the dumpfile
            CloseHandle(hFile);

            if (bDumpWritten)
                FileSave(PathJoin(ms_strDumpPath, "server_pending_upload_filename"), strFinalDumpPathFilename);
        }
    }
}

void CCrashHandler::RunErrorTool()
{
}

#endif
