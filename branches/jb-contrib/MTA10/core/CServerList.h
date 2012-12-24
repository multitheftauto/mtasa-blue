/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerList.h
*  PURPOSE:     Header file for master server/LAN querying list
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CServerList;
class CServerListItem;
class CMasterServerManagerInterface;

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

// Query response data buffer
#define SERVER_LIST_QUERY_BUFFER            4096

// Master server list timeout (in ms)
#define SERVER_LIST_MASTER_TIMEOUT          10000

// Maximum amount of server queries per pulse (so the list gradually streams in)
#define SERVER_LIST_QUERIES_PER_PULSE       2

// LAN packet broadcasting interval (in ms)
#define SERVER_LIST_BROADCAST_REFRESH       2000

// Timeout for one server in the server list to respond to a query (in ms)
#define SERVER_LIST_ITEM_TIMEOUT       8000


enum
{
    ASE_FLAG_ADDRESS            =   0x0001,
    ASE_FLAG_PORT               =   0x0002,
    ASE_FLAG_PLAYER_COUNT       =   0x0004,
    ASE_FLAG_MAX_PLAYER_COUNT   =   0x0008,
    ASE_FLAG_GAME_NAME          =   0x0010,
    ASE_FLAG_NAME               =   0x0020,
    ASE_FLAG_GAME_MODE          =   0x0040,
    ASE_FLAG_MAP                =   0x0080,
    ASE_FLAG_VERSION            =   0x0100,
    ASE_FLAG_PASSWORDED         =   0x0200,
    ASE_FLAG_SERIALS            =   0x0400,
    ASE_FLAG_PLAYER_LIST        =   0x0800,
    ASE_FLAG_RESPONDING         =   0x1000,
    ASE_FLAG_RESTRICTIONS       =   0x2000,
    ASE_FLAG_SEARCH_IGNORE_SECTIONS =   0x4000,
    ASE_FLAG_KEEP                   =   0x8000,
};

enum
{
    SERVER_INFO_NONE,
    SERVER_INFO_ASE_0,
    SERVER_INFO_CACHE,
    SERVER_INFO_ASE_2,
    SERVER_INFO_ASE_2b,
    SERVER_INFO_QUERY,
};


class CServerListItem
{
    friend class CServerList;

public:
    CServerListItem ( void )
    {
        Address.S_un.S_addr = 0;
        usGamePort = 0;
        Init ();
    }

    CServerListItem ( in_addr _Address, unsigned short _usGamePort )
    {
        Address = _Address;
        usGamePort = _usGamePort;
        Init ();
    }

    CServerListItem ( in_addr _Address, unsigned short _usGamePort, std::string _strHostName )
    {
        Address = _Address;
        usGamePort = _usGamePort;
        strHostName = _strHostName;
        Init ();
    }

    CServerListItem ( const CServerListItem & copy )
    {
        Address.S_un.S_addr = copy.Address.S_un.S_addr;
        usGamePort = copy.usGamePort;
        strHostName = copy.strHostName;
        Init ();
        uiTieBreakPosition = copy.uiTieBreakPosition;
    }

    ~CServerListItem ( void )
    {
        MapRemove ( ms_ValidServerListItemMap, this );
        CloseSocket ();
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
        return ( Address.S_un.S_addr == other.Address.S_un.S_addr && usGamePort == other.usGamePort );
    }

    void                Init            ( void )
    {
        MapInsert ( ms_ValidServerListItemMap, this );
        // Initialize variables
        bScanned = false;
        bSkipped = false;
        bSerials = false;
        bPassworded = false;
        bKeepFlag = false;
        nPlayers = 0;
        nMaxPlayers = 0;
        nPing = 9999;
        uiCacheNoReplyCount = 0;
        m_ElapsedTime.SetMaxIncrement ( 500 );
        m_ElapsedTime.Reset ();
        uiQueryRetryCount = 0;
        uiRevision = 1;
        bMaybeOffline = false;
        bMasterServerSaysNoResponse = false;
        uiMasterServerSaysRestrictions = 0;
        for ( int i = 0 ; i < SERVER_BROWSER_TYPE_COUNT ; i++ )
            revisionInList[i] = -1;

        strHost = inet_ntoa ( Address );
        strName = SString ( "%s:%d", inet_ntoa ( Address ), usGamePort );
        strEndpoint = strName;
        strEndpointSortKey = SString ( "%02x%02x%02x%02x-%04x",
                                        Address.S_un.S_un_b.s_b1, Address.S_un.S_un_b.s_b2,
                                        Address.S_un.S_un_b.s_b3, Address.S_un.S_un_b.s_b4,
                                        usGamePort );

        m_Socket = INVALID_SOCKET;
        strGameMode = "";
        strMap = "";
        vecPlayers.clear();
        uiTieBreakPosition = 1000;
        m_iDataQuality = SERVER_INFO_NONE;
        m_iTimeoutLength = SERVER_LIST_ITEM_TIMEOUT;
        m_iBuildType = 0;
        m_iBuildNumber = 0;
    }

    void                CloseSocket     ( void )
    {
        if ( m_Socket != INVALID_SOCKET )
        {
            closesocket ( m_Socket );
            m_Socket = INVALID_SOCKET;
        }
    }

    bool                ParseQuery      ( const char * szBuffer, unsigned int nLength );
    void                Query           ( void );
    std::string         Pulse           ( bool bCanSendQuery, bool bRemoveNonResponding = false );
    void                ResetForRefresh ( void );
    unsigned short      GetQueryPort    ( void );

    in_addr             Address;        // IP-address
    unsigned short      usGamePort;     // Game port
    unsigned short      nPlayers;      // Current players
    unsigned short      nMaxPlayers;   // Maximum players
    unsigned short      nPing;         // Ping time
    bool                bPassworded;    // Password protected
    bool                bSerials;       // Serial verification on
    bool                bScanned;
    bool                bSkipped;
    bool                bMaybeOffline;
    bool                bMasterServerSaysNoResponse;
    uint                uiMasterServerSaysRestrictions;
    uint                revisionInList[ SERVER_BROWSER_TYPE_COUNT ];
    uint                uiCacheNoReplyCount;
    uint                uiQueryRetryCount;
    uint                uiRevision;
    bool                bKeepFlag;

    SString             strGameName;    // Game name. Always 'mta'
    SString             strVersion;     // Game version
    SString             strName;        // Server name
    SString             strSearchableName;  // Server name to use for searches
    SString             strHost;        // Server host as IP
    SString             strHostName;    // Server host as name
    SString             strGameMode;    // Gamemode
    SString             strMap;         // Map name
    SString             strEndpoint;    // IP:port as a string

    int                 m_iBuildType;     // 9=release
    int                 m_iBuildNumber;   // 00000 and up

    SString             strNameSortKey;         // Server name as a sortable string
    SString             strVersionSortKey;      // Game version as a sortable string
    SString             strEndpointSortKey;     // IP:port as a sortable string
    uint                uiTieBreakPosition;
    SString             strTieBreakSortKey;

    std::vector < SString >
                        vecPlayers;


    bool WaitingToSendQuery ( void ) const
    {
        return !bScanned && !bSkipped && m_Socket == INVALID_SOCKET;
    }

    const SString& GetEndpoint ( void ) const
    {
        return strEndpoint;
    }

    const SString& GetEndpointSortKey ( void ) const
    {
        return strEndpointSortKey;
    }

    void PostChange ( void )
    {
        // Update tie break sort key
        strTieBreakSortKey = SString ( "%04d", uiTieBreakPosition );

        // Update version sort key
        strVersionSortKey = strVersion;

        if ( strVersionSortKey.empty () )
            strVersionSortKey = "0.0";

        if ( isdigit ( (uchar)strVersionSortKey.Right ( 1 )[0] ) )
            strVersionSortKey += 'z';

        strVersionSortKey = SString ( "%s_%05d_", *strVersionSortKey, m_iBuildNumber );

        SString strTemp;
        for ( uint i = 0 ; i < strVersionSortKey.length () ; i++ )
        {
            uchar c = strVersionSortKey[i];
            if ( isdigit ( c ) )
                c = '9' - c + '0';
            else
            if ( isalpha ( c ) )
                c = 'z' - c + 'a';
            strTemp += c;
        }
        strVersionSortKey = strTemp;

        // Update name sort key
        strNameSortKey = "";
        for ( uint i = 0 ; i < strName.length () ; i++ )
        {
            uchar c = strName[i];
            if ( isalnum(c) )
                strNameSortKey += toupper ( c );
        }

        // Ensure searchable name is set
        if ( strSearchableName.empty () )
            strSearchableName = strName;
    }

    const SString& GetVersionSortKey ( void ) const
    {
        return strVersionSortKey;
    }

    bool MaybeWontRespond ( void ) const
    {
        if ( bMasterServerSaysNoResponse || uiCacheNoReplyCount > 0 )
            if ( GetDataQuality () < SERVER_INFO_QUERY )
                return true;
        return false;
    }

    bool ShouldAllowDataQuality ( int iDataQuality )
    {
        return iDataQuality >= m_iDataQuality;
    }
    void SetDataQuality ( int iDataQuality )
    {
        m_iDataQuality = iDataQuality;
        uiRevision++;
    }
    int GetDataQuality ( void ) const
    {
        return m_iDataQuality;
    }

    static bool StaticIsValid ( CServerListItem* pServer )
    {
        return MapContains ( ms_ValidServerListItemMap, pServer );
    }

    int                 m_iTimeoutLength;
protected:
    int                 m_Socket;
    CElapsedTime        m_ElapsedTime;
    int                 m_iDataQuality;

    static std::set < CServerListItem* > ms_ValidServerListItemMap;
};

typedef std::list<CServerListItem*>::const_iterator CServerListIterator;
typedef std::list<CServerListItem*>::const_reverse_iterator CServerListReverseIterator;

// Address and port combo
struct SAddressPort
{
    ulong   m_ulIp;
    ushort  m_usPort;
    SAddressPort ( in_addr Address, ushort usPort ) : m_ulIp ( Address.s_addr ), m_usPort ( usPort ) {}
    bool operator < ( const SAddressPort& other ) const
    {
        return m_ulIp < other.m_ulIp || 
                ( m_ulIp == other.m_ulIp &&
                    ( m_usPort < other.m_usPort ) );
    }
};


////////////////////////////////////////////////
//
// class CServerListItemList
//
// Keeps track of list items in a map
// 
////////////////////////////////////////////////
class CServerListItemList
{
    std::list < CServerListItem* >                  m_List;
    std::map < SAddressPort, CServerListItem* >     m_Map;
public:

    std::list < CServerListItem* >& GetList ( void )
    {
        return m_List;
    }

    std::list<CServerListItem*>::iterator begin()
    {
        return m_List.begin ();
    }

    std::list<CServerListItem*>::iterator end()
    {
        return m_List.end ();
    }

    std::list<CServerListItem*>::reverse_iterator rbegin()
    {
        return m_List.rbegin ();
    }

    std::list<CServerListItem*>::reverse_iterator rend()
    {
        return m_List.rend ();
    }

    size_t size() const
    {
        return m_List.size ();
    }

    void clear()
    {
        m_List.clear ();
        m_Map.clear ();
    }

    CServerListItem* Find ( in_addr Address, ushort usGamePort )
    {
        SAddressPort key ( Address, usGamePort );
        if ( CServerListItem** ppItem = MapFind ( m_Map, key ) )
            return *ppItem;
        return NULL;
    }

    CServerListItem* Add ( in_addr Address, ushort usGamePort, bool bAtFront = false )
    {
        SAddressPort key ( Address, usGamePort );
        assert ( !MapContains ( m_Map, key ) );
        CServerListItem* pItem = new CServerListItem ( Address, usGamePort );
        MapSet ( m_Map, key, pItem );
        pItem->uiTieBreakPosition = 5000;
        if ( !m_List.empty () )
        {
            if ( bAtFront )
                pItem->uiTieBreakPosition = m_List.front ()->uiTieBreakPosition - 1;
            else
                pItem->uiTieBreakPosition = m_List.back ()->uiTieBreakPosition + 1;
        }
        if ( bAtFront )
            m_List.push_front ( pItem );
        else
            m_List.push_back ( pItem );
        return pItem;
    }

    bool Remove ( in_addr Address, ushort usGamePort )
    {
        SAddressPort key ( Address, usGamePort );
        if ( MapRemove ( m_Map, key ) )
        {
            for ( std::list<CServerListItem *>::iterator iter = m_List.begin (); iter != m_List.end (); iter++ )
            {
                CServerListItem* pItem = *iter;
                if ( pItem->Address.s_addr == Address.s_addr && pItem->usGamePort == usGamePort )
                {
                    m_List.erase ( iter );
                    delete pItem;
                    return true;
                }
            }
        }
        return false;
    }
};




class CServerList
{
public:
                                            CServerList             ( void );
    virtual                                 ~CServerList            ( void );

    // Base implementation scans all listed servers
    virtual void                            Pulse                   ( void );
    virtual void                            Refresh                 ( void );
    virtual bool                            RemoveNonResponding     ( void )                        { return true; }

    CServerListIterator                     IteratorBegin           ( void )                        { return m_Servers.begin (); };
    CServerListIterator                     IteratorEnd             ( void )                        { return m_Servers.end (); };
    CServerListReverseIterator              ReverseIteratorBegin    ( void )                        { return m_Servers.rbegin (); };
    CServerListReverseIterator              ReverseIteratorEnd      ( void )                        { return m_Servers.rend (); };
    unsigned int                            GetServerCount          ( void )                        { return m_Servers.size (); };

    bool                                    AddUnique               ( in_addr Address, ushort usGamePort, bool addAtFront = false );
    void                                    Clear                   ( void );
    bool                                    Remove                  ( in_addr Address, ushort usGamePort );

    std::string&                            GetStatus               ( void )                        { return m_strStatus; };
    bool                                    IsUpdated               ( void )                        { return m_bUpdated; };
    void                                    SetUpdated              ( bool bUpdated )               { m_bUpdated = bUpdated; };
    int                                     GetRevision             ( void )                        { return m_iRevision; }
    void                                    SortByASEVersion        ( void );

protected:
    bool                                    m_bUpdated;
    int                                     m_iPass;
    unsigned int                            m_nScanned;
    unsigned int                            m_nSkipped;
    int                                     m_iRevision;
    CServerListItemList                     m_Servers;
    std::string                             m_strStatus;
    std::string                             m_strStatus2;
    long long                               m_llLastTickCount;
};

// Internet list (grabs the master server list on refresh)
class CServerListInternet : public CServerList
{
public:
                                            CServerListInternet     ( void );
                                            ~CServerListInternet    ( void );
    void                                    Pulse                   ( void );
    void                                    Refresh                 ( void );
    bool                                    RemoveNonResponding     ( void )    { return m_nScanned > 10; }     // Don't remove until net access is confirmed

private:

    CMasterServerManagerInterface*          m_pMasterServerManager;
    CElapsedTime                            m_ElapsedTime;
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