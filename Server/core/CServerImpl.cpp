/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerImpl.cpp
 *  PURPOSE:     Server class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CServerImpl.h"
#include "CCrashHandler.h"
#include "MTAPlatform.h"
#include "version.h"
#include "ErrorCodes.h"
#include <clocale>
#include <cstdio>
#include <cwctype>
#include <fstream>
#include <signal.h>

#ifdef WIN32
    #include <Mmsystem.h>
    #include <io.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

void WaitForKey(int iKey);
void Print(const char* szFormat, ...);

// Define libraries
char szNetworkLibName[] = "net" MTA_LIB_SUFFIX MTA_LIB_EXTENSION;
char szXMLLibName[] = "xmll" MTA_LIB_SUFFIX MTA_LIB_EXTENSION;

using namespace std;

bool g_bSilent = false;
bool g_bNoCurses = false;
bool g_bNoTopBar = false;
bool g_bNoCrashHandler = false;
#ifndef WIN32
bool    g_bDaemonized = false;
WINDOW* m_wndMenu = NULL;
WINDOW* m_wndInput = NULL;
bool    IsCursesActive()
{
    return m_wndInput != NULL;
}
#else
bool   g_isChildProcess = false;
HANDLE g_readyEvent = nullptr;
#endif

#ifdef WIN32
CServerImpl::CServerImpl(CThreadCommandQueue* pThreadCommandQueue)
#else
CServerImpl::CServerImpl()
#endif
{
#ifdef WIN32
    m_pThreadCommandQueue = pThreadCommandQueue;
    m_hConsole = NULL;
#else
    m_wndMenu = NULL;
    m_wndInput = NULL;
#endif

    // Init
    m_pNetwork = nullptr;
    m_bRequestedQuit = false;
    m_bRequestedReset = false;
    m_exitCode = ERROR_NO_ERROR;
    m_inputBuffer.clear();
    m_cursorPos = 0;
    m_insertMode = true;
    m_renderedLength = 0;
    memset(&m_szTag, 0, sizeof(m_szTag) * sizeof(char));
    m_dLastTimeMs = 0;
    m_dPrevOverrun = 0;

    // Create our stuff
    m_pModManager = new CModManagerImpl(this);
}

CServerImpl::~CServerImpl()
{
    SaveCommandHistory();
    // Destroy our stuff
    delete m_pModManager;
}

CNetServer* CServerImpl::GetNetwork()
{
    return m_pNetwork;
}

CModManager* CServerImpl::GetModManager()
{
    return m_pModManager;
}

CXML* CServerImpl::GetXML()
{
    return m_pXML;
}

SString CServerImpl::GetAbsolutePath(const char* szRelative)
{
    return PathJoin(m_strServerPath, szRelative);
}

void CServerImpl::Printf(const char* szFormat, ...)
{
    va_list ap;
    va_start(ap, szFormat);

    if (!g_bSilent)
    {
#ifdef WIN32
        if (HasConsole())
        {
            char    stackBuffer[2048];
            va_list apCopy;
            va_copy(apCopy, ap);
            const int formattedLength = vsnprintf(stackBuffer, sizeof(stackBuffer), szFormat, apCopy);
            va_end(apCopy);

            std::wstring wideMessage;
            if (formattedLength > 0 && formattedLength < static_cast<int>(sizeof(stackBuffer)))
            {
                wideMessage = MbUTF8ToUTF16(stackBuffer);
            }
            else if (formattedLength >= static_cast<int>(sizeof(stackBuffer)))
            {
                std::vector<char> dynamicBuffer(formattedLength + 1);
                va_list           apRetry;
                va_copy(apRetry, ap);
                vsnprintf(dynamicBuffer.data(), dynamicBuffer.size(), szFormat, apRetry);
                va_end(apRetry);
                wideMessage = MbUTF8ToUTF16(dynamicBuffer.data());
            }

            if (!wideMessage.empty())
            {
                CONSOLE_CURSOR_INFO cursorInfo;
                const bool          cursorOk = GetConsoleCursorInfo(m_hConsole, &cursorInfo);
                if (cursorOk && cursorInfo.bVisible)
                {
                    CONSOLE_CURSOR_INFO hiddenCursor = cursorInfo;
                    hiddenCursor.bVisible = FALSE;
                    SetConsoleCursorInfo(m_hConsole, &hiddenCursor);
                }

                // If user has an active prompt drawn, erase it first
                if (m_renderedLength > 0)
                {
                    std::wstring clearStr(m_renderedLength + 2, L' ');
                    WriteConsoleW(m_hConsole, L"\r", 1, nullptr, nullptr);
                    WriteConsoleW(m_hConsole, clearStr.c_str(), static_cast<DWORD>(clearStr.length()), nullptr, nullptr);
                    WriteConsoleW(m_hConsole, L"\r", 1, nullptr, nullptr);
                    m_renderedLength = 0;
                    m_bPendingPromptRedraw = true;
                }

                // Output log message text
                SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                WriteConsoleW(m_hConsole, wideMessage.c_str(), static_cast<DWORD>(wideMessage.length()), nullptr, nullptr);

                // Only redraw the input prompt when the log line completes (ends with \n or \r\n)
                const bool hasNewline = (wideMessage.back() == L'\n' || wideMessage.back() == L'\r');
                if (hasNewline && m_bPendingPromptRedraw && !m_inputBuffer.empty())
                {
                    m_bPendingPromptRedraw = false;
                    RedrawInputLine();
                }

                if (cursorOk && cursorInfo.bVisible)
                {
                    SetConsoleCursorInfo(m_hConsole, &cursorInfo);
                }
            }
        }
        else
        {
            vprintf(szFormat, ap);
            fflush(stdout);
        }
#else

        if (IsCursesActive())
            vwprintw(stdscr, szFormat, ap);
        else
            vprintf(szFormat, ap);
#endif
    }

    va_end(ap);
}

bool CServerImpl::IsRequestingExit()
{
#ifdef WIN32
    m_pThreadCommandQueue->Process(m_bRequestedQuit, NULL);
#endif
    return m_bRequestedQuit;
}

#ifndef WIN32
void CServerImpl::Daemonize() const
{
    if (fork())
        exit(0);

    close(0);
    assert(open("/dev/null", O_RDONLY) == 0);

    close(1);
    assert(open("/dev/null", O_WRONLY) == 1);

    close(2);
    assert(open("/dev/null", O_WRONLY) == 2);
}

#endif

#ifdef WIN32
bool CServerImpl::HasConsole()
{
    // Getting it a single time is sufficient
    static bool isTTY = _isatty(_fileno(stdin));
    return isTTY;
}
#endif

int CServerImpl::Run(int iArgumentCount, char* szArguments[])
{
    // Parse our arguments
    if (!ParseArguments(iArgumentCount, szArguments))
        return 1;

#ifndef WIN32
    if (!g_bNoCrashHandler)
#endif
    {
        // Init our crashhandler if not being run within the client
        // (and enabled for Linux)
        CCrashHandler::Init(m_strServerPath);
    }

#ifndef WIN32
    // Daemonize?
    if (g_bDaemonized)
        Daemonize();
#endif

    if (!g_bSilent)
    {
        // Initialize the console handlers
#ifdef WIN32
        // Set our locale to the C locale, as Unicode output only functions in this locale
        std::setlocale(LC_ALL, "C");
        assert(strcoll("a", "B") > 0);

        // Get the console handles
        m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        m_hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

        if (!g_isChildProcess && HasConsole())
        {
            // Disable QuickEdit mode to prevent text selection causing server freeze,
            // and disable Mouse Input to let Windows Console Host handle mouse wheel scrolling naturally
            DWORD dwConInMode;
            if (GetConsoleMode(m_hConsoleInput, &dwConInMode))
            {
    #ifndef ENABLE_EXTENDED_FLAGS
        #define ENABLE_EXTENDED_FLAGS 0x0080
    #endif
    #ifndef ENABLE_QUICK_EDIT_MODE
        #define ENABLE_QUICK_EDIT_MODE 0x0040
    #endif
    #ifndef ENABLE_MOUSE_INPUT
        #define ENABLE_MOUSE_INPUT 0x0010
    #endif
                SetConsoleMode(m_hConsoleInput, ENABLE_EXTENDED_FLAGS | (dwConInMode & ~ENABLE_QUICK_EDIT_MODE & ~ENABLE_MOUSE_INPUT));
            }

            // Enable the default grey color with a black background

            SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

            // Get the console's width
            CONSOLE_SCREEN_BUFFER_INFO ScrnBufferInfo;
            if (!GetConsoleScreenBufferInfo(m_hConsole, &ScrnBufferInfo))
            {
                Print("ERROR: GetConsoleScreenBufferInfo failed (%08x)\n", GetLastError());
                Print("Press Q to shut down the server!\n");
                WaitForKey('q');
                DestroyWindow();
                return ERROR_OTHER;
            }

            // Adjust the console's screenbuffer to provide a generous scrollback buffer
            if (ScrnBufferInfo.dwSize.Y < 5000)
                ScrnBufferInfo.dwSize.Y = 5000;

            SetConsoleScreenBufferSize(m_hConsole, ScrnBufferInfo.dwSize);
            SetConsoleWindowInfo(m_hConsole, TRUE, &ScrnBufferInfo.srWindow);
            SetConsoleOutputCP(CP_UTF8);

    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
            // Enable virtual terminal processing for ANSI escape sequences if available
            DWORD dwConOutMode = 0;
            if (GetConsoleMode(m_hConsole, &dwConOutMode))
            {
                SetConsoleMode(m_hConsole, dwConOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
            }
        }

        else if (GetFileType(m_hConsoleInput) == FILE_TYPE_PIPE)
        {
            // Enable non-blocking read mode
            DWORD pipeState = PIPE_NOWAIT;
            SetNamedPipeHandleState(m_hConsoleInput, &pipeState, nullptr, nullptr);
        }

        if (g_isChildProcess)
        {
            // Read the ready-event handle value as uint64_t to correctly receive it from both
            // 32-bit and 64-bit parent processes (HANDLE is 4 bytes on x86, 8 bytes on x64).
            DWORD    bytesRead{};
            uint64_t handleValue = 0;

            if (!ReadFile(m_hConsoleInput, &handleValue, sizeof(handleValue), &bytesRead, nullptr) || bytesRead != sizeof(handleValue))
            {
                Print("ERROR: Failed to read ready-event handle from input (%08x)\n", GetLastError());
                return ERROR_OTHER;
            }

            g_readyEvent = reinterpret_cast<HANDLE>(handleValue);
        }
#else
        // support user locales
        std::setlocale(LC_ALL, "");
        std::setlocale(LC_NUMERIC, "C");
        std::setlocale(LC_COLLATE, "C");
        assert(strcoll("a", "B") > 0);

        // Initialize the window and any necessary curses options
        if (!g_bNoCurses)
        {
            initscr();
            keypad(stdscr, TRUE);
            nonl();
            cbreak();
            noecho();
            nodelay(stdscr, TRUE);
            idlok(stdscr, FALSE);
            scrollok(stdscr, TRUE);

            if (!g_bNoTopBar)
                setscrreg(1, LINES - 1);
            else
                setscrreg(0, LINES - 1);

            // Initialize the colors
            if (has_colors())
            {
                start_color();

                init_pair(1, COLOR_BLACK, COLOR_WHITE);
                init_pair(2, COLOR_BLACK, COLOR_GREEN);
                init_pair(3, COLOR_WHITE, COLOR_WHITE);
                init_pair(4, COLOR_RED, COLOR_WHITE);
                init_pair(5, COLOR_GREEN, COLOR_WHITE);
                init_pair(6, COLOR_BLUE, COLOR_WHITE);
            }
            // Create the input window
            m_wndInput = subwin(stdscr, 1, COLS, LINES - 1, 0);
            scrollok(m_wndInput, TRUE);
            wbkgd(m_wndInput, COLOR_PAIR(2));

            // Create the menu window
            if (!g_bNoTopBar)
            {
                m_wndMenu = subwin(stdscr, 1, COLS, 0, 0);
                wbkgd(m_wndMenu, COLOR_PAIR(1));
            }

            // Position the cursor and refresh the physical screen

            if (!g_bNoTopBar)
                move(1, 0);
            else
                move(0, 0);
            refresh();
        }
        else
        {
            int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
            fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        }

#endif
    }

    // Did we find the path? If not, assume our current
    if (m_strServerPath == "")
        m_strServerPath = GetSystemCurrentDirectory();

    // Convert all backslashes to forward slashes
    m_strServerPath = m_strServerPath.Replace("\\", "/");

    // Make sure it has no trailing slash
    m_strServerPath = m_strServerPath.TrimEnd("/");

    // Set the mod path
    m_strServerModPath = m_strServerPath + "/mods/deathmatch";

    // Load persistent console command history
    LoadCommandHistory();

    // Tell the mod manager the server path

    m_pModManager->SetServerPath(m_strServerPath);

    // Welcome text
    if (!g_bSilent)
        Print("MTA:BLUE Server for MTA:SA\n\n");

    // Load the network DLL
    if (m_NetworkLibrary.Load(PathJoin(m_strServerPath, SERVER_BIN_PATH, szNetworkLibName)))
    {
        // Network module compatibility check
        typedef unsigned long (*PFNCHECKCOMPATIBILITY)(unsigned long, unsigned long*);
        PFNCHECKCOMPATIBILITY pfnCheckCompatibility = nullptr;
        {
            const auto procAddr = m_NetworkLibrary.GetProcedureAddress("CheckCompatibility");
            static_assert(sizeof(pfnCheckCompatibility) == sizeof(procAddr), "Unexpected function pointer size");
            std::memcpy(&pfnCheckCompatibility, &procAddr, sizeof(pfnCheckCompatibility));
        }
        if (!pfnCheckCompatibility || !pfnCheckCompatibility(MTA_DM_SERVER_NET_MODULE_VERSION, (unsigned long*)MTASA_VERSION_TYPE))
        {
            // net.dll doesn't like our version number
            ulong ulNetModuleVersion = 0;
            if (pfnCheckCompatibility)
                pfnCheckCompatibility(1, &ulNetModuleVersion);
            Print("Network module not compatible! (Expected 0x%x, got 0x%x)\n", MTA_DM_SERVER_NET_MODULE_VERSION, (uint)ulNetModuleVersion);
            Print("Press Q to shut down the server!\n");
            Print("\n\n\n(If this is a custom build,\n");
            Print(" 1. Update source to latest\n");
            Print(" 2. Get latest net module (run install-data)\n");
            Print(" 3. Check MTASA_VERSION_TYPE in version.h is set correctly)\n");
            WaitForKey('q');
            DestroyWindow();
            return ERROR_NETWORK_LIBRARY_FAILED;
        }

        if (m_XMLLibrary.Load(PathJoin(m_strServerPath, SERVER_BIN_PATH, szXMLLibName)))
        {
            InitNetServerInterface    pfnInitNetServerInterface = nullptr;
            ReleaseNetServerInterface pfnReleaseNetServerInterface = nullptr;
            InitXMLInterface          pfnInitXMLInterface = nullptr;

            {
                const auto procAddr = m_NetworkLibrary.GetProcedureAddress("InitNetServerInterface");
                static_assert(sizeof(pfnInitNetServerInterface) == sizeof(procAddr), "Unexpected function pointer size");
                std::memcpy(&pfnInitNetServerInterface, &procAddr, sizeof(pfnInitNetServerInterface));
            }
            {
                const auto procAddr = m_NetworkLibrary.GetProcedureAddress("ReleaseNetServerInterface");
                static_assert(sizeof(pfnReleaseNetServerInterface) == sizeof(procAddr), "Unexpected function pointer size");
                std::memcpy(&pfnReleaseNetServerInterface, &procAddr, sizeof(pfnReleaseNetServerInterface));
            }
            {
                const auto procAddr = m_XMLLibrary.GetProcedureAddress("InitXMLInterface");
                static_assert(sizeof(pfnInitXMLInterface) == sizeof(procAddr), "Unexpected function pointer size");
                std::memcpy(&pfnInitXMLInterface, &procAddr, sizeof(pfnInitXMLInterface));
            }

            if (pfnInitNetServerInterface && pfnInitXMLInterface)
            {
                // Call it to grab the network interface class
                m_pNetwork = pfnInitNetServerInterface();
                m_pXML = pfnInitXMLInterface(*m_strServerModPath);

                if (m_pNetwork && m_pXML)
                {
                    if (m_pModManager->Load("deathmatch", iArgumentCount, szArguments))  // Hardcoded for now
                    {
                        LoadCommandHistory();

                        // Enter our mainloop
                        MainLoop();

                        SaveCommandHistory();

                        if (pfnReleaseNetServerInterface)
                            pfnReleaseNetServerInterface();
                    }

                    else
                    {
                        // Quit during startup?
                        if (m_bRequestedQuit)
                        {
                            DestroyWindow();
                            return ERROR_NO_ERROR;
                        }

                        // Couldn't load our mod
                        Print("Press Q to shut down the server!\n");
                        WaitForKey('q');
                        DestroyWindow();
                        return ERROR_LOADING_MOD;
                    }
                }
                else
                {
                    // Couldn't find the InitNetServerInterface func
                    Print("ERROR: Initialization functions failed!\n");
                    Print("Press Q to shut down the server!\n");
                    WaitForKey('q');
                    DestroyWindow();
                    return ERROR_NETWORK_LIBRARY_FAILED;
                }
            }
            else
            {
                // Couldn't find the InitNetServerInterface func
                Print("ERROR: No suitable initialization functions found!\n");
                Print("Press Q to shut down the server!\n");
                WaitForKey('q');
                DestroyWindow();
                return ERROR_NETWORK_LIBRARY_FAILED;
            }
        }
        else
        {
            // Couldn't load it
            Print("ERROR: Loading XML library (%s) failed!\n", szXMLLibName);
            Print("Press Q to shut down the server!\n");
            WaitForKey('q');
            DestroyWindow();
            return ERROR_NO_NETWORK_LIBRARY;
        }
    }
    else
    {
        // Couldn't load it
        Print("ERROR: Loading network library (%s) failed!\n", szNetworkLibName);
        Print("Press Q to shut down the server!\n");
        WaitForKey('q');
        DestroyWindow();
        return ERROR_NO_NETWORK_LIBRARY;
    }

    // Normal termination
    DestroyWindow();

    // If a reset was requested, tell the main that
    if (m_bRequestedReset)
    {
        m_bRequestedReset = false;
        m_bRequestedQuit = false;
        return SERVER_RESET_RETURN;
    }

    // Otherwize return no error
    return ERROR_NO_ERROR;
}

void CServerImpl::MainLoop()
{
#ifdef WIN32
    timeBeginPeriod(1);  // Change sleep resolution to 1ms
#endif

    // Loop until a termination is requested
    while (!m_bRequestedQuit)
    {
#ifndef WIN32
        if (!g_bSilent && !g_bNoCurses)
        {
            // Update all the windows, and the physical screen in one burst
            if (m_wndMenu)
                wnoutrefresh(m_wndMenu);
            wnoutrefresh(m_wndInput);
            doupdate();
            wbkgd(m_wndInput, COLOR_PAIR(2));
        }
#endif
        if (!g_bSilent && !g_bNoTopBar && !g_bNoCurses)
        {
            char szInfoTag[128] = {'\0'};
            m_pModManager->GetTag(&szInfoTag[0], sizeof(szInfoTag));
            ShowInfoTag(szInfoTag);
        }

        // Handle the interpreter input
        HandleInput();

// Handle input from the secondary thread
#ifdef WIN32
        m_pThreadCommandQueue->Process(m_bRequestedQuit, m_pModManager);
#endif

        // Pulse the modmanager
        m_pModManager->DoPulse();

        if (m_pModManager->IsFinished())
            m_bRequestedQuit = true;

#ifdef WIN32
        if (g_readyEvent != nullptr && m_pModManager->IsReadyToAcceptConnections())
        {
            SetEvent(g_readyEvent);
            CloseHandle(g_readyEvent);
            g_readyEvent = nullptr;
        }
#endif

        HandlePulseSleep();
    }

#ifdef WIN32
    timeEndPeriod(1);  // Restore previous sleep resolution
#endif

    // Unload the current mod
    m_pModManager->Unload();
}

//
// Limit the pulses to avoid heavy CPU usage
//
void CServerImpl::HandlePulseSleep()
{
    // Get settings
    int iSleepBusyMs;
    int iSleepIdleMs;
    int iLogicFpsLimit;
    m_pModManager->GetSleepIntervals(iSleepBusyMs, iSleepIdleMs, iLogicFpsLimit);

    // Apply logic FPS limit if set
    if (iLogicFpsLimit > 0)
    {
        ApplyFrameRateLimit(iLogicFpsLimit);
        return;
    }

    // Sleep up to idle_sleep_time in 1ms ticks, exiting the moment the sync
    // thread queues a packet. The previous code did a blind Sleep for
    // busy_sleep_time at the top of every pulse before checking the inbound
    // queue, which capped logic FPS near 1000/busy_sleep_time on busy servers
    // regardless of how full the queue already was (#4853). busy_sleep_time
    // is no longer consulted on this path; server_logic_fps_limit is the
    // existing knob for a hard cap.
    const int        iSleepMs = Clamp(0, iSleepIdleMs, 50);
    const CTickCount deadline = CTickCount::Now() + CTickCount((long long)iSleepMs);
    while (CTickCount::Now() < deadline)
    {
        if (m_pModManager->PendingWorkToDo())
            return;
        Sleep(1);
    }
}

//
// Do FPS limiting
//
void CServerImpl::ApplyFrameRateLimit(uint uiUseRate)
{
    if (uiUseRate < 1)
        return;

    // Calc required time in ms between frames
    const double dTargetTimeToUse = 1000.0 / uiUseRate;

    // Time now
    double dTimeMs = CTickCount::Now().ToDouble();  // GetTickCount32 ();

    // Get delta time in ms since last frame
    double dTimeUsed = dTimeMs - m_dLastTimeMs;

    // Apply any over/underrun carried over from the previous frame
    dTimeUsed += m_dPrevOverrun;

    if (dTimeUsed < dTargetTimeToUse)
    {
        // Have time spare - maybe eat some of that now
        double dSpare = dTargetTimeToUse - dTimeUsed;

        double dUseUpNow = dSpare - dTargetTimeToUse * 0.3f;
        if (dUseUpNow >= 1)
            Sleep(static_cast<DWORD>(floor(dUseUpNow)));

        // Redo timing calcs
        dTimeMs = CTickCount::Now().ToDouble();
        dTimeUsed = dTimeMs - m_dLastTimeMs;
        dTimeUsed += m_dPrevOverrun;
    }

    // Update over/underrun for next frame
    m_dPrevOverrun = dTimeUsed - dTargetTimeToUse;

    // Limit carry over
    m_dPrevOverrun = Clamp(dTargetTimeToUse * -0.9f, m_dPrevOverrun, dTargetTimeToUse * 0.1f);

    m_dLastTimeMs = dTimeMs;
}

/*************************/
/* Tag color interpreter */
/* --------------------- */
/* 128 | white           */
/* 129 | grey            */
/* 130 | red             */
/* 131 | green           */
/* 132 | blue            */
/* 133 | light red       */
/* 134 | light green     */
/* 135 | light blue      */
/*************************/
void CServerImpl::ShowInfoTag(char* szTag)
{
    if (g_bSilent || g_bNoTopBar || g_bNoCurses)
        return;
#ifdef WIN32
    // On Windows, dynamically update the Console Window Title with clean live server statistics.
    // This provides a continuous status header in the title bar without overwriting rows in the scrollback buffer.
    if (strcmp(szTag, m_szTag) != 0)
    {
        strcpy(m_szTag, szTag);

        std::wstring cleanTitle;
        for (int i = 0; szTag[i] != '\0'; ++i)
        {
            if (static_cast<unsigned char>(szTag[i]) > 127)
                continue;

            cleanTitle += static_cast<wchar_t>(szTag[i]);
        }

        if (!cleanTitle.empty())
        {
            SetConsoleTitleW(cleanTitle.c_str());
        }
    }
#else

    // Linux curses variant, so much easier :)
    int iAttr = COLOR_PAIR(1);

    // Position the cursor
    wmove(m_wndMenu, 0, 0);

    for (int i = 0; i < COLS; i++)
    {
        // Break if we reached szTag's end
        if (szTag[i] == '\0')
            break;

        // Apply the attributes
        switch ((unsigned char)(szTag[i]))
        {
            case 128:
                iAttr = COLOR_PAIR(3) | A_BOLD;
                break;
            case 129:
                iAttr = COLOR_PAIR(3);
                break;
            case 130:
                iAttr = COLOR_PAIR(4);
                break;
            case 131:
                iAttr = COLOR_PAIR(5);
                break;
            case 132:
                iAttr = COLOR_PAIR(6);
                break;
            case 133:
                iAttr = COLOR_PAIR(4) | A_BOLD;
                break;
            case 134:
                iAttr = COLOR_PAIR(5) | A_BOLD;
                break;
            case 135:
                iAttr = COLOR_PAIR(6) | A_BOLD;
                break;
            default:
                waddch(m_wndMenu, szTag[i] | iAttr);
                iAttr = COLOR_PAIR(1);
                break;
        }
    }
#endif
}

void CServerImpl::RedrawInputLine()
{
#ifdef WIN32
    if (!HasConsole() || g_bSilent)
        return;

    // Temporarily hide the hardware caret during redraw to eliminate cursor flicker at column 0
    CONSOLE_CURSOR_INFO cursorInfo;
    const bool          cursorInfoRetrieved = GetConsoleCursorInfo(m_hConsole, &cursorInfo);
    if (cursorInfoRetrieved && cursorInfo.bVisible)
    {
        CONSOLE_CURSOR_INFO hiddenCursor = cursorInfo;
        hiddenCursor.bVisible = FALSE;
        SetConsoleCursorInfo(m_hConsole, &hiddenCursor);
    }

    const size_t previousRenderedLength = m_renderedLength;
    const size_t currentLength = m_inputBuffer.length();

    // Move to start of line
    WriteConsoleW(m_hConsole, L"\r", 1, nullptr, nullptr);

    // Render active input line with distinct bright yellow text color
    SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    if (!m_inputBuffer.empty())
    {
        WriteConsoleW(m_hConsole, m_inputBuffer.c_str(), static_cast<DWORD>(m_inputBuffer.length()), nullptr, nullptr);
    }

    // Overwrite trailing characters with spaces if line got shorter
    if (previousRenderedLength > currentLength)
    {
        std::wstring padding(previousRenderedLength - currentLength, L' ');
        WriteConsoleW(m_hConsole, padding.c_str(), static_cast<DWORD>(padding.length()), nullptr, nullptr);
    }

    m_renderedLength = currentLength;

    // Restore standard console color
    SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    // Reposition cursor at active insertion point
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    if (GetConsoleScreenBufferInfo(m_hConsole, &screenBufferInfo) && screenBufferInfo.dwSize.X > 0)
    {
        COORD       targetPosition = screenBufferInfo.dwCursorPosition;
        const SHORT consoleWidth = screenBufferInfo.dwSize.X;
        targetPosition.X = static_cast<SHORT>(m_cursorPos < static_cast<size_t>(consoleWidth) ? m_cursorPos : consoleWidth - 1);
        SetConsoleCursorPosition(m_hConsole, targetPosition);
    }

    // Restore visible hardware caret at the final insertion coordinate
    if (cursorInfoRetrieved && cursorInfo.bVisible)
    {
        SetConsoleCursorInfo(m_hConsole, &cursorInfo);
    }
#else

    if (!g_bSilent && !g_bNoCurses && m_wndInput)
    {
        wclear(m_wndInput);
        if (!m_inputBuffer.empty())
            wprintw(m_wndInput, "%s", UTF16ToMbUTF8(m_inputBuffer).c_str());
        wmove(m_wndInput, 0, static_cast<int>(m_cursorPos));
        wnoutrefresh(m_wndInput);
    }
#endif
}

void CServerImpl::InsertCharacter(wchar_t ch)
{
    if (m_cursorPos == m_inputBuffer.length())
    {
        m_inputBuffer.push_back(ch);
        m_cursorPos++;
        m_renderedLength = m_inputBuffer.length();

#ifdef WIN32
        if (HasConsole() && !g_bSilent)
        {
            SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            WriteConsoleW(m_hConsole, &ch, 1, nullptr, nullptr);
            SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
#else
        if (!g_bSilent && !g_bNoCurses && m_wndInput)
        {
            waddch(m_wndInput, ch);
            wnoutrefresh(m_wndInput);
        }
#endif
    }
    else
    {
        if (m_insertMode)
        {
            m_inputBuffer.insert(m_cursorPos, 1, ch);
        }
        else
        {
            m_inputBuffer[m_cursorPos] = ch;
        }
        m_cursorPos++;
        RedrawInputLine();
    }
}

void CServerImpl::InsertString(const std::wstring& text)
{
    if (text.empty())
        return;

    m_inputBuffer.insert(m_cursorPos, text);
    m_cursorPos += text.length();
    RedrawInputLine();
}

void CServerImpl::HandleBackspace(bool word)
{
    if (m_cursorPos == 0 || m_inputBuffer.empty())
        return;

    if (!word)
    {
        m_inputBuffer.erase(m_cursorPos - 1, 1);
        m_cursorPos--;
    }
    else
    {
        size_t eraseStart = m_cursorPos;
        while (eraseStart > 0 && iswspace(m_inputBuffer[eraseStart - 1]))
            eraseStart--;
        while (eraseStart > 0 && !iswspace(m_inputBuffer[eraseStart - 1]))
            eraseStart--;

        m_inputBuffer.erase(eraseStart, m_cursorPos - eraseStart);
        m_cursorPos = eraseStart;
    }
    RedrawInputLine();
}

void CServerImpl::HandleDelete(bool word)
{
    if (m_cursorPos >= m_inputBuffer.length())
        return;

    if (!word)
    {
        m_inputBuffer.erase(m_cursorPos, 1);
    }
    else
    {
        size_t eraseEnd = m_cursorPos;
        while (eraseEnd < m_inputBuffer.length() && iswspace(m_inputBuffer[eraseEnd]))
            eraseEnd++;
        while (eraseEnd < m_inputBuffer.length() && !iswspace(m_inputBuffer[eraseEnd]))
            eraseEnd++;

        m_inputBuffer.erase(m_cursorPos, eraseEnd - m_cursorPos);
    }
    RedrawInputLine();
}

void CServerImpl::HandleLeftArrow(bool word)
{
    if (m_cursorPos == 0)
        return;

    if (!word)
    {
        m_cursorPos--;
    }
    else
    {
        while (m_cursorPos > 0 && iswspace(m_inputBuffer[m_cursorPos - 1]))
            m_cursorPos--;
        while (m_cursorPos > 0 && !iswspace(m_inputBuffer[m_cursorPos - 1]))
            m_cursorPos--;
    }
    RedrawInputLine();
}

void CServerImpl::HandleRightArrow(bool word)
{
    if (m_cursorPos >= m_inputBuffer.length())
        return;

    if (!word)
    {
        m_cursorPos++;
    }
    else
    {
        while (m_cursorPos < m_inputBuffer.length() && iswspace(m_inputBuffer[m_cursorPos]))
            m_cursorPos++;
        while (m_cursorPos < m_inputBuffer.length() && !iswspace(m_inputBuffer[m_cursorPos]))
            m_cursorPos++;
    }
    RedrawInputLine();
}

void CServerImpl::HandleHome()
{
    if (m_cursorPos == 0)
        return;
    m_cursorPos = 0;
    RedrawInputLine();
}

void CServerImpl::HandleEnd()
{
    if (m_cursorPos == m_inputBuffer.length())
        return;
    m_cursorPos = m_inputBuffer.length();
    RedrawInputLine();
}

void CServerImpl::HandleTabCompletion()
{
    if (m_inputBuffer.empty())
        return;

    // List of standard console commands for first-token completion
    static const std::vector<std::wstring> knownCommands = {
        L"start",     L"stop",   L"restart",       L"refresh",       L"refreshall", L"list",         L"info",
        L"upgrade",   L"check",  L"say",           L"teamsay",       L"msg",        L"me",           L"nick",
        L"login",     L"logout", L"chgmypass",     L"addaccount",    L"delaccount", L"chgpass",      L"shutdown",
        L"aexec",     L"whois",  L"debugscript",   L"help",          L"loadmodule", L"unloadmodule", L"reloadmodule",
        L"ver",       L"ase",    L"openportstest", L"setdbloglevel", L"reloadbans", L"aclrequest",   L"authserial",
        L"reloadacl", L"cls",    L"clear",         L"quit",          L"exit",       L"reset"};

    // Find token boundary before cursor
    size_t             tokenStart = m_inputBuffer.rfind(L' ', m_cursorPos > 0 ? m_cursorPos - 1 : 0);
    const bool         isFirstToken = (tokenStart == std::wstring::npos);
    const size_t       wordStart = isFirstToken ? 0 : tokenStart + 1;
    const std::wstring prefix = m_inputBuffer.substr(wordStart, m_cursorPos - wordStart);

    if (prefix.empty())
        return;

    if (isFirstToken)
    {
        for (const auto& cmd : knownCommands)
        {
            if (cmd.length() > prefix.length() && cmd.compare(0, prefix.length(), prefix) == 0)
            {
                std::wstring suffix = cmd.substr(prefix.length()) + L" ";
                InsertString(suffix);
                return;
            }
        }
    }
}

void CServerImpl::HandleClipboardPaste()
{
#ifdef WIN32
    if (!OpenClipboard(nullptr))
        return;

    HANDLE clipboardDataHandle = GetClipboardData(CF_UNICODETEXT);
    if (clipboardDataHandle)
    {
        if (const auto* clipboardText = static_cast<const wchar_t*>(GlobalLock(clipboardDataHandle)))
        {
            std::wstring sanitizedText;
            for (const wchar_t* p = clipboardText; *p != L'\0'; ++p)
            {
                if (*p != L'\r' && *p != L'\n')
                    sanitizedText += *p;
                else
                    sanitizedText += L' ';
            }
            GlobalUnlock(clipboardDataHandle);

            if (!sanitizedText.empty())
            {
                InsertString(sanitizedText);
            }
        }
    }
    CloseClipboard();
#endif
}

void CServerImpl::ClearScreen()
{
#ifdef WIN32
    if (!HasConsole() || g_bSilent)
        return;

    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    if (!GetConsoleScreenBufferInfo(m_hConsole, &screenBufferInfo))
        return;

    const DWORD totalCells = screenBufferInfo.dwSize.X * screenBufferInfo.dwSize.Y;
    DWORD       cellsWritten = 0;
    const COORD homeCoordinates = {0, 0};

    FillConsoleOutputCharacterW(m_hConsole, L' ', totalCells, homeCoordinates, &cellsWritten);
    FillConsoleOutputAttribute(m_hConsole, screenBufferInfo.wAttributes, totalCells, homeCoordinates, &cellsWritten);
    SetConsoleCursorPosition(m_hConsole, homeCoordinates);

    m_renderedLength = 0;
    RedrawInputLine();
#else
    if (!g_bSilent && !g_bNoCurses && m_wndInput)
    {
        clear();
        refresh();
        RedrawInputLine();
    }
#endif
}

void CServerImpl::ExecuteCurrentCommand()

{
    if (m_inputBuffer.empty())
        return;

    // Deduplicate and record non-empty unique command in history (limit to 128 items)
    const std::wstring cleanHistoryLine = CleanCommandHistoryLine(m_inputBuffer);
    if (!cleanHistoryLine.empty())
    {
        for (auto it = m_vecCommandHistory.begin() + 1; it != m_vecCommandHistory.end(); ++it)
        {
            if ((*it)[0] == cleanHistoryLine)
            {
                m_vecCommandHistory.erase(it);
                break;
            }
        }

        m_vecCommandHistory.push_back({cleanHistoryLine, cleanHistoryLine});

        constexpr size_t maxHistoryCount = 128;
        while (m_vecCommandHistory.size() > maxHistoryCount + 1)
        {
            m_vecCommandHistory.erase(m_vecCommandHistory.begin() + 1);
        }

        SaveCommandHistory();
    }

    const std::wstring fullCommand = m_inputBuffer;
    ClearInput();

    // Check for hardcoded core commands
    if (!_wcsicmp(fullCommand.c_str(), L"quit") || !_wcsicmp(fullCommand.c_str(), L"exit"))
    {
        m_bRequestedQuit = true;
        return;
    }
    if (!_wcsicmp(fullCommand.c_str(), L"reset"))
    {
        m_bRequestedReset = true;
        m_bRequestedQuit = true;
        return;
    }
    if (!_wcsicmp(fullCommand.c_str(), L"cls") || !_wcsicmp(fullCommand.c_str(), L"clear"))
    {
        ClearScreen();
        return;
    }

    // Split unquoted semicolons (;) for command chaining
    std::vector<std::wstring> commandList;
    std::wstring              currentCommand;
    bool                      insideQuotes = false;

    for (wchar_t ch : fullCommand)
    {
        if (ch == L'"')
            insideQuotes = !insideQuotes;

        if (ch == L';' && !insideQuotes)
        {
            if (!currentCommand.empty())
            {
                commandList.push_back(currentCommand);
                currentCommand.clear();
            }
        }
        else
        {
            currentCommand += ch;
        }
    }
    if (!currentCommand.empty())
        commandList.push_back(currentCommand);

    // Execute each command sequentially
    for (const auto& singleCommand : commandList)
    {
        const size_t start = singleCommand.find_first_not_of(L" \t");
        const size_t end = singleCommand.find_last_not_of(L" \t");
        if (start != std::wstring::npos && end != std::wstring::npos)
        {
            const std::wstring trimmedCommand = singleCommand.substr(start, end - start + 1);
            if (!trimmedCommand.empty())
            {
                m_pModManager->HandleInput(UTF16ToMbUTF8(trimmedCommand).c_str());
            }
        }
    }
}

void CServerImpl::HandleInput()
{
    wint_t inputCharacter = 0;

    // Get the STDIN input
#ifdef WIN32
    if (!HasConsole())
    {
        // Read from pipe instead of tty
        DWORD bytesRead = 0;
        if (!ReadFile(m_hConsoleInput, &inputCharacter, 1, &bytesRead, nullptr) || bytesRead == 0)
            inputCharacter = 0;
    }
    else if (kbhit())
    {
        inputCharacter = _getwch();
    }
#else
    if (!g_bNoCurses)
    {
        if (get_wch(&inputCharacter) == ERR)
            inputCharacter = 0;
    }
    else
    {
        inputCharacter = getwchar();
        if (inputCharacter == WEOF)
            inputCharacter = 0;
    }
#endif

    if (inputCharacter == 0)
        return;

    switch (inputCharacter)
    {
        case '\n':
        case '\r':
#ifdef WIN32
            Printf(" \n");
#else
            if (!g_bSilent && !g_bNoCurses)
            {
                wclear(m_wndInput);
                printw("%s\n", UTF16ToMbUTF8(m_inputBuffer).c_str());
            }
#endif
            ExecuteCurrentCommand();
            break;

        case KEY_BACKSPACE:
            HandleBackspace(false);
            break;

        case 0x7F:  // Ctrl+Backspace or Unix delete
            HandleBackspace(true);
            break;

        case 0x1B:  // Esc: clear line
            ClearInput();
            break;

        case 0x0C:  // Ctrl+L: clear screen
            ClearScreen();
            break;

        case 0x16:  // Ctrl+V: paste from clipboard
            HandleClipboardPaste();
            break;

        case 0x09:  // Tab: auto-completion
            HandleTabCompletion();
            break;

        case 0x01:  // Ctrl+A: jump to start
            HandleHome();
            break;

        case 0x05:  // Ctrl+E: jump to end
            HandleEnd();
            break;

#ifdef WIN32
        case 0x00:
        case KEY_EXTENDED:
        {
            const wint_t extendedKey = _getwch();
            switch (extendedKey)
            {
                case KEY_LEFT:
                    HandleLeftArrow(false);
                    break;

                case KEY_RIGHT:
                    HandleRightArrow(false);
                    break;

                case KEY_CTRL_LEFT:
                    HandleLeftArrow(true);
                    break;

                case KEY_CTRL_RIGHT:
                    HandleRightArrow(true);
                    break;

                case KEY_HOME:
                    HandleHome();
                    break;

                case KEY_END:
                    HandleEnd();
                    break;

                case KEY_DELETE:
                    HandleDelete(false);
                    break;

                case 0x93:  // Ctrl+Delete
                    HandleDelete(true);
                    break;

                case KEY_INSERT:
                    m_insertMode = !m_insertMode;
                    break;

                case KEY_UP:
                {
                    if (m_vecCommandHistory.size() <= 1 || m_uiSelectedCommandHistoryEntry == 1)
                        break;

                    int selectedEntry = m_uiSelectedCommandHistoryEntry;
                    if (selectedEntry == 0)
                        selectedEntry = static_cast<int>(m_vecCommandHistory.size() - 1);
                    else
                        selectedEntry--;

                    SelectCommandHistoryEntry(selectedEntry);
                    break;
                }

                case KEY_DOWN:
                {
                    if (m_vecCommandHistory.size() <= 1 || m_uiSelectedCommandHistoryEntry == 0)
                        break;

                    SelectCommandHistoryEntry(m_uiSelectedCommandHistoryEntry + 1);
                    break;
                }
            }
            break;
        }
#endif

        default:
            if (iswprint(inputCharacter))
            {
                InsertCharacter(static_cast<wchar_t>(inputCharacter));
            }
            break;
    }
}

void CServerImpl::SelectCommandHistoryEntry(uint uiEntry)
{
    const uint previouslySelectedEntry = m_uiSelectedCommandHistoryEntry;

    if (!m_vecCommandHistory.empty() && uiEntry > 0 && uiEntry < m_vecCommandHistory.size())
        m_uiSelectedCommandHistoryEntry = uiEntry;
    else
        m_uiSelectedCommandHistoryEntry = 0;

    m_vecCommandHistory[previouslySelectedEntry][1] = m_inputBuffer;

    ClearInput();

    const auto& selectedInput = m_vecCommandHistory[m_uiSelectedCommandHistoryEntry][1];
    if (selectedInput.empty())
        return;

    m_inputBuffer = selectedInput;
    m_cursorPos = m_inputBuffer.length();
    RedrawInputLine();
}

bool CServerImpl::ClearInput()
{
    if (!m_inputBuffer.empty() || m_renderedLength > 0)
    {
#ifdef WIN32
        if (HasConsole() && !g_bSilent && m_renderedLength > 0)
        {
            CONSOLE_CURSOR_INFO cursorInfo;
            const bool          cursorInfoRetrieved = GetConsoleCursorInfo(m_hConsole, &cursorInfo);
            if (cursorInfoRetrieved && cursorInfo.bVisible)
            {
                CONSOLE_CURSOR_INFO hiddenCursor = cursorInfo;
                hiddenCursor.bVisible = FALSE;
                SetConsoleCursorInfo(m_hConsole, &hiddenCursor);
            }

            std::wstring spaces(m_renderedLength + 2, L' ');
            WriteConsoleW(m_hConsole, L"\r", 1, nullptr, nullptr);
            WriteConsoleW(m_hConsole, spaces.c_str(), static_cast<DWORD>(spaces.length()), nullptr, nullptr);
            WriteConsoleW(m_hConsole, L"\r", 1, nullptr, nullptr);

            if (cursorInfoRetrieved && cursorInfo.bVisible)
            {
                SetConsoleCursorInfo(m_hConsole, &cursorInfo);
            }
        }
#else
        if (!g_bSilent && !g_bNoCurses && m_wndInput)
        {
            wclear(m_wndInput);
            wnoutrefresh(m_wndInput);
        }
#endif
        m_inputBuffer.clear();
        m_cursorPos = 0;
        m_renderedLength = 0;
        return true;
    }
    return false;
}

bool CServerImpl::ResetInput()
{
    if (!m_inputBuffer.empty())
    {
#ifdef WIN32
        Printf(" \n");
#else
        if (!g_bSilent && !g_bNoCurses && m_wndInput)
        {
            wclear(m_wndInput);
            printw("%s\n", UTF16ToMbUTF8(m_inputBuffer).c_str());
        }
#endif
        ClearInput();
        m_uiSelectedCommandHistoryEntry = 0;
        return true;
    }
    return false;
}

bool CServerImpl::ParseArguments(int iArgumentCount, char* szArguments[])
{
#ifndef WIN32
    // Default to a simple console if stdout is not a TTY (e.g. running under 'nohup')
    if (!isatty(STDOUT_FILENO))
    {
        g_bNoTopBar = true;
        g_bNoCurses = true;
    }
#endif

    // Iterate our arguments
    unsigned char ucNext = 0;
    for (int i = 0; i < iArgumentCount; i++)
    {
        switch (ucNext)
        {
            // Run from path?
            case 'D':
            {
                // Set it as our current path.
                m_strServerPath = szArguments[i];
                ucNext = 0;
                break;
            }

            // Nothing we know, proceed
            default:
            {
                if (strcmp(szArguments[i], "-D") == 0)
                {
                    ucNext = 'D';
                }
                else if (strcmp(szArguments[i], "-s") == 0)
                {
                    g_bSilent = true;
                }
#ifndef WIN32
                else if (strcmp(szArguments[i], "-d") == 0)
                {
                    g_bDaemonized = true;
                }
                else if (strcmp(szArguments[i], "-n") == 0)
                {
                    g_bNoTopBar = true;
                    g_bNoCurses = true;
                }
#endif
                else if (strcmp(szArguments[i], "-t") == 0)
                {
                    g_bNoTopBar = true;
                }
                else if (strcmp(szArguments[i], "-f") == 0)
                {
                    g_bNoTopBar = false;
#ifndef WIN32
                    g_bNoCurses = false;
#endif
                }
                else if (strcmp(szArguments[i], "-u") == 0)
                {
                    std::setbuf(stdout, nullptr);
                    std::setbuf(stderr, nullptr);
                }
                else if (strcmp(szArguments[i], "-x") == 0)
                {
                    g_bNoCrashHandler = true;
                }
#ifdef WIN32
                else if (!strcmp(szArguments[i], "--child-process"))
                {
                    g_isChildProcess = true;
                    g_bNoTopBar = true;
                    g_bNoCurses = true;
                    std::setbuf(stdout, nullptr);
                    std::setbuf(stderr, nullptr);
                }
#endif
            }
        }
    }

    return true;
}

bool IsKeyPressed(int iKey)
{
    // Is the key pressed?
#ifdef WIN32
    if (kbhit())
        return getch() == iKey;
#else
    refresh();
    return getchar() == iKey;
#endif

    // Not pressed
    return false;
}

std::wstring CServerImpl::CleanCommandHistoryLine(const std::wstring& line)
{
    std::wstring lower = line;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

    struct SensitiveWord
    {
        std::wstring prefix;
        int          keepWords;  // number of initial words to keep
    };

    static const SensitiveWord sensitivePatterns[] = {
        {L"login", 1},  // login <user> [pass] -> keeps "login"

        {L"register", 1},     // register <user> [pass] -> keeps "register"
        {L"addaccount", 1},   // addaccount <user> [pass] -> keeps "addaccount"
        {L"chgpass", 1},      // chgpass <user> [pass] -> keeps "chgpass"
        {L"chgmypass", 1},    // chgmypass [old] [new] -> keeps "chgmypass"
        {L"password", 1},     // password [pass] -> keeps "password"
        {L"setpassword", 1},  // setpassword [pass] -> keeps "setpassword"
    };

    for (const auto& entry : sensitivePatterns)
    {
        if (lower.rfind(entry.prefix, 0) == 0)
        {
            if (lower.length() == entry.prefix.length() || iswspace(lower[entry.prefix.length()]))
            {
                size_t pos = 0;
                int    wordsFound = 0;
                size_t keepEnd = 0;
                while (pos < line.length() && wordsFound < entry.keepWords)
                {
                    while (pos < line.length() && iswspace(line[pos]))
                        pos++;
                    if (pos >= line.length())
                        break;
                    while (pos < line.length() && !iswspace(line[pos]))
                        pos++;
                    wordsFound++;
                    keepEnd = pos;
                }
                return line.substr(0, keepEnd);
            }
        }
    }
    return line;
}

void CServerImpl::LoadCommandHistory()
{
    m_vecCommandHistory = {{L"", L""}};
    m_uiSelectedCommandHistoryEntry = 0;

    std::vector<std::string> candidatePaths = {
        PathJoin(m_strServerModPath, "priv/history.txt"),
        PathJoin(m_strServerModPath, "history.txt"),
    };

    std::ifstream inFile;
    for (const auto& path : candidatePaths)
    {
        inFile.open(FromUTF8(path));
        if (inFile.is_open())
            break;
    }

    if (!inFile.is_open())
        return;

    std::string line;
    while (std::getline(inFile, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty())
            continue;

        std::wstring wLine = MbUTF8ToUTF16(line);
        std::wstring cleanLine = CleanCommandHistoryLine(wLine);
        if (!cleanLine.empty())
        {
            bool exists = false;
            for (const auto& entry : m_vecCommandHistory)
            {
                if (entry[0] == cleanLine)
                {
                    exists = true;
                    break;
                }
            }
            if (!exists)
            {
                m_vecCommandHistory.push_back({cleanLine, cleanLine});
            }
        }
    }
    inFile.close();

    constexpr size_t maxHistoryCount = 128;
    while (m_vecCommandHistory.size() > maxHistoryCount + 1)
    {
        m_vecCommandHistory.erase(m_vecCommandHistory.begin() + 1);
    }
}

void CServerImpl::SaveCommandHistory()
{
    if (m_strServerModPath.empty())
        return;

    std::string privDir = PathJoin(m_strServerModPath, "priv");
    MakeSureDirExists(privDir.c_str());

    std::string   historyPath = PathJoin(privDir, "history.txt");
    std::ofstream outFile(FromUTF8(historyPath), std::ios::trunc);
    if (!outFile.is_open())
    {
        historyPath = PathJoin(m_strServerModPath, "history.txt");
        outFile.open(FromUTF8(historyPath), std::ios::trunc);
    }

    if (!outFile.is_open())
        return;

    for (size_t i = 1; i < m_vecCommandHistory.size(); ++i)
    {
        const std::wstring& cmd = m_vecCommandHistory[i][0];
        std::wstring        cleanCmd = CleanCommandHistoryLine(cmd);
        if (!cleanCmd.empty())
        {
            std::string mb = UTF16ToMbUTF8(cleanCmd);
            outFile << mb << "\n";
        }
    }
    outFile.close();
}

void CServerImpl::DestroyWindow()

{
#ifndef WIN32
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
#endif
}

void WaitForKey(int iKey)
{
    if (!g_bSilent && !g_bNoCurses)
    {
        for (;;)
        {
            // Is the key pressed?
            if (IsKeyPressed(iKey))
                return;

            // Limit the looping a little to prevent heavy CPU usage
            Sleep(10);
        }
    }
}

// Always print these messages
void Print(const char* szFormat, ...)
{
    va_list ap;
    va_start(ap, szFormat);

    SString str;
    str.vFormat(szFormat, ap);

#ifdef WIN32
    printf("%s", *str);
#else
    if (IsCursesActive())
        printw("%s", *str);
    else
        printf("%s", *str);
#endif

    va_end(ap);
}
