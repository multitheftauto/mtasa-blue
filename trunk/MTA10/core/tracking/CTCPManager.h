/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/tracking/CTCPManager.h
*  PURPOSE:		Header file for TCP manager class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CTCPManager;

#ifndef __CTCPMANAGER_H
#define __CTCPMANAGER_H

#include <windows.h>
#include "CTCPClientSocketImpl.h"
#include "CTCPServerSocketImpl.h"
#include "CSingleton.h"

/* ACHTUNG: No linux support. (Unnecessary for client) */
/* ACHTUNG: No server support */

class CTCPManager : public CSingleton < CTCPManager >
{
	public:
						CTCPManager					( void );
						~CTCPManager				( void );

    CTCPClientSocket*   CreateClient				( void );
    CTCPServerSocket*   CreateServer				( void );

    bool                DestroyClient				( CTCPClientSocket* pClient );
    bool                DestroyServer				( CTCPServerSocket* pServer );

    const char*         GetLastError				( void );

    bool                Initialize					( void );
    void                Reset						( void );

	void				HandleEvent					( unsigned int uiID, LPARAM lType );

	private:

	unsigned int		m_uiID;
    bool                m_bInitialized;
    char                m_szLastError[128];

	// Array of CTCP's.. most likely faster than a list
	// ACHTUNG: Use TCPSocket?
	CTCPSocket* m_pSocket[255];
};

#endif
