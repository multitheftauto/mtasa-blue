/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConnectManager.h
*  PURPOSE:     Header file for connect manager
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONNECTMANAGER_H
#define __CCONNECTMANAGER_H

#include <ctime>
#include <gui/CGUI.h>
#include <xfire.h>
#include "CServerInfo.h"

class CConnectManager
{
public:
                    CConnectManager     ( void );
                    ~CConnectManager    ( void );

    bool            Connect             ( const char* szHost, unsigned short usPort, const char* szNick, const char* szPassword, bool bNotifyServerBrowser = false, bool bForceInternalHTTPServer = false );
    bool            Reconnect           ( const char* szHost, unsigned short usPort, const char* szPassword, bool bSave = true, bool bForceInternalHTTPServer = false );

    bool            Abort               ( void );

    void            DoPulse             ( void );

    void            OnServerExists      ( void );

    bool            ShouldUseInternalHTTPServer ( void )    { return m_bForceInternalHTTPServer; }

    static bool     StaticProcessPacket ( unsigned char ucPacketID, class NetBitStreamInterface& bitStream );

    std::string     m_strLastHost;
    unsigned short  m_usLastPort;
    std::string     m_strLastPassword;
private:
    bool            Event_OnCancelClick ( CGUIElement * pElement );

    in_addr         m_Address;
    std::string     m_strHost;
    unsigned short  m_usPort;
    std::string     m_strNick;
    std::string     m_strPassword;
    bool            m_bIsDetectingVersion;
    bool            m_bIsConnecting;
    bool            m_bReconnect;
    bool            m_bSave;
    bool            m_bForceInternalHTTPServer;
    time_t          m_tConnectStarted;

    GUI_CALLBACK*   m_pOnCancelClick;

    CServerListItem* m_pServerItem;
    bool            m_bNotifyServerBrowser;

    bool CheckNickProvided ( const char* szNick );
};

#endif
