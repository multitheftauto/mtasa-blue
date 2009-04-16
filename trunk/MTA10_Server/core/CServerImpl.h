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

#include "CCriticalSection.h"
#include "MTAPlatform.h"
#include <string>

#include <core/CServerInterface.h>
#include <net/CLanBroadcastInterface.h>
#include <net/CNetServer.h>

#include "CDynamicLibrary.h"
#include "CModManagerImpl.h"
#include "sockets/CTCPImpl.h"
#include "sockets/CTCPServerSocketImplManager.h"
#include <xml/CXML.h>
#include "CThreadCommandQueue.h"

#ifndef WIN32
#include <curses.h>
#endif

#define SERVER_RESET_RETURN 500

typedef CXML* (*InitXMLInterface) ( void );
typedef CNetServer* (*InitNetServerInterface) ( void );
typedef CLanBroadcast* (*InitLanBroadcastInterface) ( void );

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
    CTCP*               GetTCP              ( void );
    CXML*               GetXML              ( void );
	CLanBroadcast*		GetLanBroadcast		( void );

    inline const char*  GetServerPath       ( void )                { return m_szServerPath; };
    const char*         GetAbsolutePath     ( const char* szRelative, char* szBuffer, unsigned int uiBufferSize );

    void                Printf              ( const char* szText, ... );

    int                 Run                 ( int iArgumentCount, char* szArguments [] );

private:
    void                MainLoop            ( void );

    bool                ParseArguments      ( int iArgumentCount, char* szArguments [] );
    bool                IsKeyPressed        ( int iKey );
    void                WaitForKey          ( int iKey );

	void				ShowInfoTag			( char *szTag );
	void				HandleInput			( void );

    void                SleepMs             ( unsigned long ulMs );
	void				DestroyWindow		( void );

    CDynamicLibrary     m_NetworkLibrary;
    CDynamicLibrary     m_XMLLibrary;
    CNetServer*         m_pNetwork;
	CLanBroadcast*		m_pLanBroadcast;
    CModManagerImpl*    m_pModManager;
    CTCPImpl*           m_pTCP;
    CXML*               m_pXML;

#ifdef WIN32
    FClientFeedback*    m_fClientFeedback;
#endif

    char                m_szServerPath [MAX_PATH];

    bool                m_bRequestedQuit;
    bool                m_bRequestedReset;

	char				m_szInputBuffer[255];
	unsigned int		m_uiInputCount;
	
	char				m_szTag[80];

#ifdef WIN32
	HANDLE				m_hConsole;
	CHAR_INFO			m_ScrnBuffer[256];

    CThreadCommandQueue*    m_pThreadCommandQueue;
#else
	WINDOW*				m_wndMenu;
	WINDOW*				m_wndInput;
#endif
};

#endif
