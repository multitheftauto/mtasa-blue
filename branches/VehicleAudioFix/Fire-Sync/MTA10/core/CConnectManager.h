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
#include "CServerQueue.h"

class CConnectManager
{
public:
                    CConnectManager     ( void );
                    ~CConnectManager    ( void );

    bool            Connect             ( const char* szHost, unsigned short usPort, const char* szNick, const char* szPassword );
    bool            Reconnect           ( const char* szHost, unsigned short usPort, const char* szPassword );

    bool            Abort               ( void );
    inline bool     IsConnecting        ( void )                                                                { return m_bIsConnecting; };

    void            DoPulse             ( void );

    static bool     StaticProcessPacket ( unsigned char ucPacketID, class NetBitStreamInterface& bitStream );

private:
    bool            Event_OnCancelClick ( CGUIElement * pElement );

    std::string     m_strHost;
    unsigned short  m_usPort;
    std::string     m_strNick;
    std::string     m_strPassword;
    bool            m_bIsConnecting;
    bool            m_bReconnect;
    time_t          m_tConnectStarted;

    GUI_CALLBACK*   m_pOnCancelClick;

    bool CheckNickProvided ( const char* szNick );
};

#endif
