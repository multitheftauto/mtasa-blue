/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CConnectManager.h
 *  PURPOSE:     Header file for connect manager
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <ctime>
#include <gui/CGUI.h>
#include <ServerBrowser/CServerInfo.h>

class CConnectManager
{
public:
    CConnectManager();
    ~CConnectManager();

    bool Connect(const char* szHost, unsigned short usPort, const char* szNick, const char* szPassword, bool bNotifyServerBrowser = false);
    bool Reconnect(const char* szHost, unsigned short usPort, const char* szPassword, bool bSave = true);

    bool Abort();

    void DoPulse();

    void OnServerExists();

    static void OpenServerFirewall(in_addr Address, ushort usHttpPort = 80, bool bHighPriority = false);

    static bool StaticProcessPacket(unsigned char ucPacketID, class NetBitStreamInterface& bitStream);

    std::string    m_strLastHost;
    unsigned short m_usLastPort;
    std::string    m_strLastPassword;

private:
    bool Event_OnCancelClick(CGUIElement* pElement);

    in_addr        m_Address;
    std::string    m_strHost;
    unsigned short m_usPort;
    std::string    m_strNick;
    std::string    m_strPassword;
    bool           m_bIsDetectingVersion;
    bool           m_bIsConnecting;
    bool           m_bReconnect;
    bool           m_bSave;
    time_t         m_tConnectStarted;
    bool           m_bHasTriedSecondConnect;

    GUI_CALLBACK* m_pOnCancelClick;

    CServerListItem* m_pServerItem;
    bool             m_bNotifyServerBrowser;

    bool CheckNickProvided(const char* szNick);
};
