/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerImpl.h
 *  PURPOSE:     Server class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CServerImpl;

#pragma once

#include "MTAPlatform.h"
#include <string>

#include <core/CServerInterface.h>
#include <net/CNetServer.h>

#include "CDynamicLibrary.h"
#include "CModManagerImpl.h"
#include <xml/CXML.h>
#include "CThreadCommandQueue.h"

#define SERVER_RESET_RETURN 500

typedef CXML* (*InitXMLInterface)(const char* szSaveFlagDirectory);
typedef CNetServer* (*InitNetServerInterface)();
typedef void (*ReleaseNetServerInterface)();

#ifdef WIN32
constexpr SHORT SCREEN_BUFFER_SIZE = 256;
#endif

class CServerImpl : public CServerInterface
{
public:
#ifdef WIN32
    CServerImpl(CThreadCommandQueue* pThreadCommandQueue);
#else
    CServerImpl();
#endif

    ~CServerImpl();

    CNetServer*  GetNetwork();
    CModManager* GetModManager();
    CXML*        GetXML();

    const char* GetServerModPath() { return m_strServerModPath; };
    SString     GetAbsolutePath(const char* szRelative);

    void Printf(const char* szText, ...);
    bool IsRequestingExit();

    // Clears input buffer
    bool ClearInput();
    // Prints current input buffer on a new line, clears the input buffer and resets history selection
    bool ResetInput();
    // Clears console screen viewport
    void ClearScreen() override;

    int Run(int iArgumentCount, char* szArguments[]);
#ifndef WIN32
    void Daemonize() const;
#else
    bool HasConsole();
#endif

    void SetExitCode(int exitCode) { m_exitCode = exitCode; }
    int  GetExitCode() const { return m_exitCode; }

private:
    void MainLoop();

    bool ParseArguments(int iArgumentCount, char* szArguments[]);

    void ShowInfoTag(char* szTag);
    void HandleInput();
    void SelectCommandHistoryEntry(uint uiEntry);
    void HandlePulseSleep();
    void ApplyFrameRateLimit(uint uiUseRate);

    void                InsertCharacter(wchar_t ch);
    void                InsertString(const std::wstring& text);
    void                HandleBackspace(bool word = false);
    void                HandleDelete(bool word = false);
    void                HandleLeftArrow(bool word = false);
    void                HandleRightArrow(bool word = false);
    void                HandleHome();
    void                HandleEnd();
    void                HandleTabCompletion();
    void                HandleClipboardPaste();
    void                RedrawInputLine();
    void                ExecuteCurrentCommand();
    void                LoadCommandHistory();
    void                SaveCommandHistory();
    static std::wstring CleanCommandHistoryLine(const std::wstring& line);

    void DestroyWindow();

    CDynamicLibrary  m_NetworkLibrary;
    CDynamicLibrary  m_XMLLibrary;
    CNetServer*      m_pNetwork;
    CModManagerImpl* m_pModManager;
    CXML*            m_pXML;

    SString m_strServerPath;
    SString m_strServerModPath;

    bool m_bRequestedQuit;
    bool m_bRequestedReset;

    std::wstring m_inputBuffer;
    size_t       m_cursorPos = 0;
    bool         m_insertMode = true;
    size_t       m_renderedLength = 0;
    bool         m_bPendingPromptRedraw = false;

    char m_szTag[128];

    double m_dLastTimeMs;
    double m_dPrevOverrun;

    int m_exitCode;

    std::vector<std::vector<std::wstring>> m_vecCommandHistory = {{L"", L""}};
    uint                                   m_uiSelectedCommandHistoryEntry = 0;

#ifdef WIN32
    HANDLE    m_hConsole;
    HANDLE    m_hConsoleInput;
    CHAR_INFO m_ScrnBuffer[SCREEN_BUFFER_SIZE];

    CThreadCommandQueue* m_pThreadCommandQueue;
#endif
};
