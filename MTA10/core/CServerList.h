/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerList.h
*  PURPOSE:     Header file for master server/LAN querying list
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CServerList;
class CServerListItem;

#ifndef __CSERVERLIST_H
#define __CSERVERLIST_H

#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "CSingleton.h"
#include "tracking/CHTTPClient.h"

// Master server list URL
#define SERVER_LIST_MASTER_URL              "http://1mgg.com/affil/mta-1.0"

// Master server data buffer (16KB should be enough, but be warned)
#define SERVER_LIST_DATA_BUFFER             16384

// Query response data buffer
#define SERVER_LIST_QUERY_BUFFER            4096

// Master server list timeout (in ms)
#define SERVER_LIST_MASTER_TIMEOUT          10000

// Maximum amount of server queries per pulse (so the list gradually streams in)
#define SERVER_LIST_QUERIES_PER_PULSE       2

// LAN packet broadcasting interval (in ms)
#define SERVER_LIST_BROADCAST_REFRESH       2000

class CServerListItemPlayer
{
public:
    std::string         strName;
    std::string         strTeam;
    std::string         strSkin;
    unsigned short      nScore;
    unsigned short      nPing;
    unsigned short      nTime;
};

class CServerListItem
{
    friend class CServerList;

public:
    CServerListItem ( void )
    {
        Address.S_un.S_addr = 0;
        usQueryPort = 0;
        Init ();
    }

    CServerListItem ( in_addr _Address, unsigned short _usQueryPort )
    {
        Address = _Address;
        usQueryPort = _usQueryPort;
        Init ();
    }

    CServerListItem ( const CServerListItem & copy )
    {
        Address.S_un.S_addr = copy.Address.S_un.S_addr;
        usQueryPort = copy.usQueryPort;
        Init ();
    }

    static bool         Parse           ( const char* szAddress, in_addr& Address )
    {
        DWORD dwIP = inet_addr ( szAddress );
        if ( dwIP == INADDR_NONE )
        {
            hostent* pHostent = gethostbyname ( szAddress );
            if ( !pHostent )
                return false;
            DWORD* pIP = (DWORD *)pHostent->h_addr_list[0];
            if ( !pIP )
                return false;
            dwIP = *pIP;
        }

        Address.S_un.S_addr = dwIP;
        return true;
    }

    bool                operator==      ( const CServerListItem &other ) const
    {
        return ( Address.S_un.S_addr == other.Address.S_un.S_addr && usQueryPort == other.usQueryPort );
    }

    void                Init            ( void )
    {
        // Initialize variables
        bScanned = false;
        bSerials = false;
        m_ulQueryStart = 0;
        
        // Initialize sockets
        m_Socket = socket ( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
        u_long flag = 1;
        ioctlsocket ( m_Socket, FIONBIO, &flag );
    }

    bool                ParseQuery      ( const char * szBuffer, unsigned int nLength );
    void                Query           ( void );
    bool                Pulse           ( void );

    in_addr             Address;        // IP-address
    unsigned short      usQueryPort;    // Query port
    unsigned short      usGamePort;     // Game port
    unsigned short      nPlayers;      // Current players
    unsigned short      nMaxPlayers;   // Maximum players
    unsigned short      nPing;         // Ping time
    bool                bPassworded;    // Password protected
    bool                bSerials;       // Serial verification on
    bool                bScanned;

    std::string         strGame;        // Game name
    std::string         strVersion;     // Game version
    std::string         strName;        // Server name
    std::string         strHost;        // Server hostname
    std::string         strType;        // Game type
    std::string         strMap;         // Map name

    std::vector < CServerListItemPlayer >
                        vecPlayers;
private:
    int                 m_Socket;
    unsigned long       m_ulQueryStart;
};

typedef std::list<CServerListItem*>::const_iterator CServerListIterator;

class CServerList
{
public:
                                            CServerList             ( void );
                                            ~CServerList            ( void );

    // Base implementation scans all listed servers
    virtual void                            Pulse                   ( void );
    virtual void                            Refresh                 ( void );

    CServerListIterator                     IteratorBegin           ( void )                        { return m_Servers.begin (); };
    CServerListIterator                     IteratorEnd             ( void )                        { return m_Servers.end (); };
    unsigned int                            GetServerCount          ( void )                        { return m_Servers.size (); };

    void                                    Add                     ( CServerListItem Server )      { m_Servers.push_back ( new CServerListItem ( Server ) ); };
    void                                    Remove                  ( CServerListItem* pServer )    { m_Servers.remove ( pServer ); delete pServer; };
    void                                    Clear                   ( void );
    bool                                    Exists                  ( CServerListItem Server );
    void                                    Remove                  ( CServerListItem Server );

    std::string&                            GetStatus               ( void )                        { return m_strStatus; };
    bool                                    IsUpdated               ( void )                        { return m_bUpdated; };
    void                                    SetUpdated              ( bool bUpdated )               { m_bUpdated = bUpdated; };
protected:
    bool                                    m_bUpdated;
    int                                     m_iPass;
    unsigned int                            m_nScanned;
    std::list < CServerListItem* >          m_Servers;
    std::string                             m_strStatus;
};

// Internet list (grabs the master server list on refresh)
class CServerListInternet : public CServerList
{
public:
    void                                    Pulse                   ( void );
    void                                    Refresh                 ( void );

private:
    bool                                    ParseList               ( const char *szBuffer, unsigned int nLength );

    CHTTPClient                             m_HTTP;
    char                                    m_szBuffer[SERVER_LIST_DATA_BUFFER];
    unsigned long                           m_ulStartTime;
};

// LAN list (scans for LAN-broadcasted servers on refresh)
class CServerListLAN : public CServerList
{
public:
    void                                    Pulse                   ( void );
    void                                    Refresh                 ( void );

private:
    void                                    Discover                ( void );

    int                                     m_Socket;
    sockaddr_in                             m_Remote;
    unsigned long                           m_ulStartTime;
};

#endif