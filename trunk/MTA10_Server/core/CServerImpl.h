/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerImpl.h
*  PURPOSE:     Server class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Oli <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CServerImpl;

#ifndef __CSERVERIMPL_H
#define __CSERVERIMPL_H

#include "MTAPlatform.h"
#include <string>

#include <core/CServerInterface.h>
#include <net/CNetServer.h>

#include "CDynamicLibrary.h"
#include "CModManagerImpl.h"
#include <xml/CXML.h>
#include "CThreadCommandQueue.h"

#ifndef WIN32
#include <ncursesw/curses.h>
#endif

#define SERVER_RESET_RETURN 500

typedef CXML* (*InitXMLInterface) ( void );
typedef CNetServer* (*InitNetServerInterface) ( void );

#ifdef WIN32
typedef void (FClientFeedback) ( const char* szText );
#endif

class CServerImpl : public CServerInterface
{
public:
    #ifdef WIN32
                        CServerImpl         ( CThreadCommandQueue* pThreadCommandQueue );
    #else
                        CServerImpl         ( void );
    #endif

                        ~CServerImpl        ( void );

    CNetServer*         GetNetwork          ( void );
    CModManager*        GetModManager       ( void );
    CXML*               GetXML              ( void );

    const char*         GetServerModPath    ( void )                { return m_strServerModPath; };
    SString             GetAbsolutePath     ( const char* szRelative );

    void                Printf              ( const char* szText, ... );
    bool                IsRequestingExit    ( void );

    int                 Run                 ( int iArgumentCount, char* szArguments [] );
#ifndef WIN32
    void                Daemonize           () const;
#else
    bool                HasConsole          ( void )                { return m_hConsole != NULL; }
#endif

private:
    void                MainLoop            ( void );
    bool                CheckLibVersions    ( void );

    bool                ParseArguments      ( int iArgumentCount, char* szArguments [] );

    void                ShowInfoTag         ( char *szTag );
    void                HandleInput         ( void );

    void                DestroyWindow       ( void );

    CDynamicLibrary     m_NetworkLibrary;
    CDynamicLibrary     m_XMLLibrary;
    CNetServer*         m_pNetwork;
    CModManagerImpl*    m_pModManager;
    CXML*               m_pXML;

#ifdef WIN32
    FClientFeedback*    m_fClientFeedback;
#endif

    SString             m_strServerPath;
    SString             m_strServerModPath;

    bool                m_bRequestedQuit;
    bool                m_bRequestedReset;

    wchar_t             m_szInputBuffer[255];
    unsigned int        m_uiInputCount;
    
    char                m_szTag[80];

#ifdef WIN32
    HANDLE              m_hConsole;
    CHAR_INFO           m_ScrnBuffer[256];

    CThreadCommandQueue*    m_pThreadCommandQueue;
#else
    WINDOW*             m_wndMenu;
    WINDOW*             m_wndInput;
#endif
};

#endif
