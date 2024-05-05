/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CServer.cpp
 *  PURPOSE:     Local server instancing class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include <array>
#include <string_view>

#ifdef MTA_DEBUG
    #define SERVER_EXE_PATH "MTA Server_d.exe"
#else
    #define SERVER_EXE_PATH "MTA Server.exe"
#endif

constexpr UINT PROCESS_FORCEFULLY_TERMINATED = 0x90804050u;

static const SFixedArray<const char*, 4> szServerErrors = {"Server stopped", "Could not load network library", "Loading network library failed",
                                                           "Error loading mod"};

static bool IsProcessRunning(HANDLE process)
{
    return WaitForSingleObject(process, 0) == WAIT_TIMEOUT;
}

struct PipeHandlePair
{
    bool   AutoClose{};
    HANDLE Read{};
    HANDLE Write{};

    ~PipeHandlePair()
    {
        if (AutoClose)
        {
            CloseHandle(Read);
            CloseHandle(Write);
        }
    }
};

bool CServer::Start(const char* configFileName)
{
    if (m_isRunning)
        return false;

    // Create and use an optional job object to kill the server process automatically when we close the job object.
    // This is pretty handy in case the game process unexpectedly crashes and we miss terminating the server process.
    HANDLE job = CreateJobObjectW(nullptr, nullptr);

    if (job != nullptr)
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
        limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

        if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits)))
        {
            g_pCore->GetConsole()->Printf("Server process warning [error: %08x]: failed to setup job object\n", GetLastError());

            CloseHandle(job);
            job = nullptr;
        }
    }

    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;

    // Create the input pipe for the server process.
    PipeHandlePair in;

    if (!CreatePipe(&in.Read, &in.Write, &securityAttributes, 0))
    {
        g_pCore->GetConsole()->Printf("Server process failed to start [error: %08x]: failed to create input pipe\n", GetLastError());
        return false;
    }

    in.AutoClose = true;

    // Only inherit the read handle to the input pipe.
    if (!SetHandleInformation(in.Write, HANDLE_FLAG_INHERIT, 0))
    {
        g_pCore->GetConsole()->Printf("Server process failed to start [error: %08x]: failed to modify input pipe\n", GetLastError());
        return false;
    }

    // Create the output pipe for the server process.
    PipeHandlePair out;

    if (!CreatePipe(&out.Read, &out.Write, &securityAttributes, 0))
    {
        g_pCore->GetConsole()->Printf("Server process failed to start [error: %08x]: failed to create output pipe\n", GetLastError());
        return false;
    }

    out.AutoClose = true;

    // Only inherit the write handle to the output pipe.
    if (!SetHandleInformation(out.Read, HANDLE_FLAG_INHERIT, 0))
    {
        g_pCore->GetConsole()->Printf("Server process failed to start [error: %08x]: failed to modify output pipe\n", GetLastError());
        return false;
    }

    // Create an anonymous event to signal the readyness of the server to accept connections.
    HANDLE readyEvent = CreateEventA(&securityAttributes, FALSE, FALSE, nullptr);

    if (readyEvent == nullptr)
    {
        g_pCore->GetConsole()->Printf("Server process failed to start [error: %08x]: failed to create ready event\n", GetLastError());
        return false;
    }

    // We write the event handle to standard input, which we then extract in CServerImpl::Run (Server Core).
    DWORD bytesWritten{};

    if (!WriteFile(in.Write, &readyEvent, sizeof(readyEvent), &bytesWritten, nullptr) || bytesWritten != sizeof(readyEvent))
    {
        g_pCore->GetConsole()->Printf("Server process failed to start [error: %08x]: failed to write ready event\n", GetLastError());
        CloseHandle(readyEvent);
        return false;
    }

    // Create the server process now.
    const SString serverRoot = CalcMTASAPath("server");
    const SString serverExePath = PathJoin(serverRoot, SERVER_EXE_PATH);
    const SString commandLine("\"%s\" --child-process --config \"%s\"", SERVER_EXE_PATH, configFileName);

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(STARTUPINFOW);
    startupInfo.hStdError = out.Write;
    startupInfo.hStdOutput = out.Write;
    startupInfo.hStdInput = in.Read;
    startupInfo.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION processInfo{};

    if (!CreateProcessW(*FromUTF8(serverExePath), const_cast<wchar_t*>(*FromUTF8(commandLine)), nullptr, nullptr, TRUE,
                        CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, nullptr, nullptr, &startupInfo, &processInfo))
    {
        g_pCore->GetConsole()->Printf("Server process failed to start [error: %08x]: failed to create process\n", GetLastError());
        CloseHandle(readyEvent);
        return false;
    }

    // Try to assign the project to our job object, if we have one.
    if (job != nullptr && !AssignProcessToJobObject(job, processInfo.hProcess))
    {
        CloseHandle(job);
        job = nullptr;
    }

    // Close the handles managed by the server process.
    CloseHandle(in.Read);
    CloseHandle(out.Write);

    // Close the thread handle, because we don't need it.
    CloseHandle(processInfo.hThread);

    in.AutoClose = false;
    out.AutoClose = false;

    m_stdin = in.Write;
    m_stdout = out.Read;
    m_isAcceptingConnections = false;
    m_isRunning = true;
    m_job = job;
    m_readyEvent = readyEvent;
    m_process = processInfo.hProcess;
    m_processId = processInfo.dwProcessId;

    g_pCore->GetConsole()->Printf("Server process has started [pid: %lu]\n", m_processId);
    return true;
}

void CServer::Stop(bool graceful)
{
    if (!m_isRunning)
        return;

    bool isRunning = IsProcessRunning(m_process);

    if (graceful && isRunning)
    {
        DWORD bytesWritten{};

        // Try to write a normal "exit" command to the server process.
        if (WriteFile(m_stdin, "exit\n", 5, &bytesWritten, nullptr) && bytesWritten == 5)
        {
            g_pCore->GetConsole()->Printf("Sent 'exit' command to the server to shut it down\n");
            WaitForSingleObject(m_process, 8000);
        }
        // Try to send a CTRl+C event to the process console.
        else if (AttachConsole(m_processId))
        {
            SetConsoleCtrlHandler(nullptr, true);
            GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
            FreeConsole();
            g_pCore->GetConsole()->Printf("Sent 'CTRL+C' event to the server to shut it down\n");
            WaitForSingleObject(m_process, 8000);
            SetConsoleCtrlHandler(nullptr, false);
        }

        isRunning = IsProcessRunning(m_process);
    }

    // We either forcefully terminating the server, or the graceful shutdown failed.
    if (isRunning)
    {
        g_pCore->GetConsole()->Printf("Terminating the server forcefully now\n");

        TerminateProcess(m_process, PROCESS_FORCEFULLY_TERMINATED);
        WaitForSingleObject(m_process, 3000);

        if (m_job != nullptr)
        {
            CloseHandle(m_job);
            m_job = nullptr;
        }
    }

    if (DWORD exitCode{}; GetExitCodeProcess(m_process, &exitCode))
    {
        const char* errorText = "Unknown exit code";

        if (exitCode == PROCESS_FORCEFULLY_TERMINATED)
            errorText = "Process was forcefully terminated";
        else if (exitCode < (sizeof(szServerErrors) / sizeof(const char*)))
            errorText = szServerErrors[exitCode];

        g_pCore->GetConsole()->Printf("Server process has been terminated [%08X]: %s\n", exitCode, errorText);
    }

    CloseHandle(m_readyEvent);
    CloseHandle(m_stdin);
    CloseHandle(m_stdout);
    CloseHandle(m_process);

    if (m_job != nullptr)
    {
        CloseHandle(m_job);
        m_job = nullptr;
    }

    m_isRunning = false;
}

void CServer::Pulse()
{
    if (!m_isRunning)
        return;

    // Try to read the process standard output and then write it to the console window.
    if (DWORD numBytes{}; PeekNamedPipe(m_stdout, nullptr, 0, nullptr, &numBytes, nullptr) && numBytes > 0)
    {
        std::array<char, 4096> buffer{};

        if (ReadFile(m_stdout, buffer.data(), std::min<DWORD>(buffer.size(), numBytes), &numBytes, nullptr) && numBytes > 0)
        {
            g_pCore->GetConsole()->Printf("%.*s", numBytes, buffer.data());
        }
    }

    // Check if the server process was terminated externally.
    if (!IsProcessRunning(m_process))
    {
        Stop();
        return;
    }

    // Check if the server process signalised readyness to accept connections.
    if (m_readyEvent != nullptr)
    {
        if (WaitForSingleObject(m_readyEvent, 0) != WAIT_TIMEOUT)
        {
            CloseHandle(m_readyEvent);
            m_readyEvent = nullptr;
            m_isAcceptingConnections = true;
        }
    }
}
