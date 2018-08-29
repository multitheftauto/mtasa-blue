/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerImpl.h
 *  PURPOSE:     Server class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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

#ifndef WIN32
    #ifdef __APPLE__
        #include <ncurses.h>
    #else
        #include <ncursesw/curses.h>
    #endif
#endif

#define SERVER_RESET_RETURN 500

typedef CXML* (*InitXMLInterface)(const char* szSaveFlagDirectory);
typedef CNetServer* (*InitNetServerInterface)(void);

#ifdef WIN32
typedef void(FClientFeedback)(const char* szText);
constexpr SHORT SCREEN_BUFFER_SIZE = 256;
#endif

class CServerImpl : public CServerInterface
{
public:
    #ifdef WIN32
    CServerImpl(CThreadCommandQueue* pThreadCommandQueue);
    #else
    CServerImpl(void);
    #endif

    ~CServerImpl(void);

    CNetServer*  GetNetwork(void);
    CModManager* GetModManager(void);
    CXML*        GetXML(void);

    const char* GetServerModPath(void) { return m_strServerModPath; };
    SString     GetAbsolutePath(const char* szRelative);

    void Printf(const char* szText, ...);
    bool IsRequestingExit(void);

    // Clears input buffer
    bool ClearInput(void);
    // Prints current input buffer on a new line, clears the input buffer and resets history selection
    bool ResetInput(void);

    int Run(int iArgumentCount, char* szArguments[]);
#ifndef WIN32
    void Daemonize() const;
#else
    bool HasConsole();
#endif

private:
    void MainLoop(void);

    bool ParseArguments(int iArgumentCount, char* szArguments[]);

    void ShowInfoTag(char* szTag);
    void HandleInput(void);
    void SelectCommandHistoryEntry(uint uiEntry);
    void HandlePulseSleep(void);
    void ApplyFrameRateLimit(uint uiUseRate);

    void DestroyWindow(void);

    CDynamicLibrary  m_NetworkLibrary;
    CDynamicLibrary  m_XMLLibrary;
    CNetServer*      m_pNetwork;
    CModManagerImpl* m_pModManager;
    CXML*            m_pXML;

#ifdef WIN32
    FClientFeedback* m_fClientFeedback;
#endif

    SString m_strServerPath;
    SString m_strServerModPath;

    bool m_bRequestedQuit;
    bool m_bRequestedReset;

    wchar_t m_szInputBuffer[255];
    uint    m_uiInputCount;

    char m_szTag[80];

    double m_dLastTimeMs;
    double m_dPrevOverrun;

    std::vector<std::vector<SString>> m_vecCommandHistory = {{"", ""}};
    uint                              m_uiSelectedCommandHistoryEntry = 0;

#ifdef WIN32
    HANDLE    m_hConsole;
    HANDLE    m_hConsoleInput;
    CHAR_INFO m_ScrnBuffer[SCREEN_BUFFER_SIZE];

    CThreadCommandQueue* m_pThreadCommandQueue;
#endif
};
