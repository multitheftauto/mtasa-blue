/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerList.h
 *  PURPOSE:     Header file for master server/LAN querying list
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CServerList;
class CServerListItem;
class CServerListItemList;
class CMasterServerManagerInterface;

#pragma once

#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "CSingleton.h"

// Master server list URL
#define SERVER_LIST_MASTER_URL              "http://master.multitheftauto.com/ase/mta/"

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
    ASE_HAS_PLAYER_COUNT = 0x0004,
    ASE_HAS_MAX_PLAYER_COUNT = 0x0008,
    ASE_HAS_GAME_NAME = 0x0010,
    ASE_HAS_SERVER_NAME = 0x0020,
    ASE_HAS_GAME_MODE = 0x0040,
    ASE_HAS_MAP_NAME = 0x0080,
    ASE_HAS_SERVER_VERSION = 0x0100,
    ASE_HAS_PASSWORDED_FLAG = 0x0200,
    ASE_HAS_SERIALS_FLAG = 0x0400,
    ASE_HAS_PLAYER_LIST = 0x0800,
    ASE_HAS_RESPONDING_FLAG = 0x1000,
    ASE_HAS_RESTRICTION_FLAGS = 0x2000,
    ASE_HAS_SEARCH_IGNORE_SECTIONS = 0x4000,
    ASE_HAS_KEEP_FLAG = 0x8000,
    ASE_HAS_HTTP_PORT = 0x080000,
    ASE_HAS_SPECIAL_FLAGS = 0x100000,
};

enum
{
    RESTRICTION_PLAYER_COUNT = 0x0004,
    RESTRICTION_MAX_PLAYER_COUNT = 0x0008,
    RESTRICTION_GAME_NAME = 0x0010,
    RESTRICTION_SERVER_NAME = 0x0020,
    RESTRICTION_GAME_MODE = 0x0040,
    RESTRICTION_MAP_NAME = 0x0080,
    RESTRICTION_SERVER_VERSION = 0x0100,
    RESTRICTION_PASSWORDED_FLAG = 0x0200,
    RESTRICTION_SERIALS_FLAG = 0x0400,
    RESTRICTION_PLAYER_LIST = 0x0800,
    RESTRICTION_BLOCK_SERVER = 0x2000,
};

enum
{
    ASE_SPECIAL_FLAG_VALID = 0x01,
    ASE_SPECIAL_FLAG_DENY_TCP_SEND = 0x02,
    ASE_SPECIAL_FLAG_FORCE_KEEP = 0x04,
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
    CServerListItem()
    {
        Address.S_un.S_addr = 0;
        usGamePort = 0;
        m_pItemList = NULL;
        m_bDoneTcpSend = false;
        m_bDoPostTcpQuery = false;
        m_usHttpPort = 0;
        m_ucSpecialFlags = 0;
        Init();
    }
    CServerListItem(in_addr _Address, unsigned short _usGamePort, CServerListItemList* pItemList = NULL, bool bAtFront = false);
    ~CServerListItem();
    void ChangeAddress(in_addr _Address, unsigned short _usGamePort);

    static bool Parse(const char* szAddress, in_addr& Address)
    {
        DWORD dwIP = inet_addr(szAddress);
        if (dwIP == INADDR_NONE)
        {
            hostent* pHostent = gethostbyname(szAddress);
            if (!pHostent)
                return false;
            DWORD* pIP = (DWORD*)pHostent->h_addr_list[0];
            if (!pIP)
                return false;
            dwIP = *pIP;
        }

        Address.S_un.S_addr = dwIP;
        return true;
    }

    bool operator==(const CServerListItem& other) const { return (Address.S_un.S_addr == other.Address.S_un.S_addr && usGamePort == other.usGamePort); }

    void Init()
    {
        MapInsert(ms_ValidServerListItemMap, this);
        // Initialize variables
        bScanned = false;
        bSkipped = false;
        bSerials = false;
        isStatusVerified = true;
        bPassworded = false;
        bKeepFlag = false;
        iRowIndex = -1;

        nPlayers = 0;
        nMaxPlayers = 0;
        nPing = 9999;
        uiCacheNoReplyCount = 0;
        uiQueryRetryCount = 0;
        uiRevision = 1;
        bMaybeOffline = false;
        bMasterServerSaysNoResponse = false;
        uiMasterServerSaysRestrictions = 0;
        for (int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++)
            revisionInList[i] = -1;

        strHost = inet_ntoa(Address);
        strName = SString("%s:%d", inet_ntoa(Address), usGamePort);
        strEndpoint = strName;
        strEndpointSortKey = SString("%02x%02x%02x%02x-%04x", Address.S_un.S_un_b.s_b1, Address.S_un.S_un_b.s_b2, Address.S_un.S_un_b.s_b3,
                                     Address.S_un.S_un_b.s_b4, usGamePort);

        strGameMode = "";
        strMap = "";
        vecPlayers.clear();
        uiTieBreakPosition = 1000;
        m_iDataQuality = SERVER_INFO_NONE;
        m_iTimeoutLength = SERVER_LIST_ITEM_TIMEOUT;
        m_iBuildType = 0;
        m_iBuildNumber = 0;
    }

    std::string    Pulse(bool bCanSendQuery, bool bRemoveNonResponding = false);
    void           ResetForRefresh();
    unsigned short GetQueryPort();

    in_addr        AddressCopy;            // Copy to ensure it doesn't get changed without us knowing
    unsigned short usGamePortCopy;
    in_addr        Address;                // IP-address
    unsigned short usGamePort;             // Game port
    unsigned short nPlayers;               // Current players
    unsigned short nMaxPlayers;            // Maximum players
    unsigned short nPing;                  // Ping time
    bool           isStatusVerified;       // Ping status verified
    bool           bPassworded;            // Password protected
    bool           bSerials;               // Serial verification on
    bool           bScanned;
    bool           bSkipped;
    bool           bMaybeOffline;
    bool           bMasterServerSaysNoResponse;
    uint           uiMasterServerSaysRestrictions;
    uint           revisionInList[SERVER_BROWSER_TYPE_COUNT];
    uint           uiCacheNoReplyCount;
    uint           uiQueryRetryCount;
    uint           uiRevision;
    bool           bKeepFlag;
    int            iRowIndex;

    SString strGameName;                  // Game name. Always 'mta'
    SString strVersion;                   // Game version
    SString strName;                      // Server name
    SString strSearchableName;            // Server name to use for searches
    SString strHost;                      // Server host as IP
    SString strHostName;                  // Server host as name
    SString strGameMode;                  // Gamemode
    SString strMap;                       // Map name
    SString strEndpoint;                  // IP:port as a string

    int    m_iBuildType;              // 9=release
    int    m_iBuildNumber;            // 00000 and up
    ushort m_usHttpPort;
    uchar  m_ucSpecialFlags;

    SString strNameSortKey;                // Server name as a sortable string
    SString strVersionSortKey;             // Game version as a sortable string
    SString strEndpointSortKey;            // IP:port as a sortable string
    uint    uiTieBreakPosition;
    SString strTieBreakSortKey;

    CQueryReceiver queryReceiver;

    std::vector<SString> vecPlayers;

    void Query();

    bool ParseQuery();

    bool WaitingToSendQuery() const { return !bScanned && !bSkipped && !queryReceiver.IsSocketValid(); }

    const SString& GetEndpoint() const { return strEndpoint; }

    const SString& GetEndpointSortKey() const { return strEndpointSortKey; }

    void PostChange()
    {
        // Update tie break sort key
        strTieBreakSortKey = SString("%04d", uiTieBreakPosition);

        // Update version sort key
        strVersionSortKey = strVersion;

        if (strVersionSortKey.empty())
            strVersionSortKey = "0.0";

        if (isdigit((uchar)strVersionSortKey.Right(1)[0]))
            strVersionSortKey += 'z';

        strVersionSortKey = SString("%s_%05d_", *strVersionSortKey, m_iBuildNumber);

        SString strTemp;
        for (uint i = 0; i < strVersionSortKey.length(); i++)
        {
            uchar c = strVersionSortKey[i];
            if (isdigit(c))
                c = '9' - c + '0';
            else if (isalpha(c))
                c = 'z' - c + 'a';
            strTemp += c;
        }
        strVersionSortKey = strTemp;

        // Update name sort key
        strNameSortKey = "";
        for (uint i = 0; i < strName.length(); i++)
        {
            uchar c = strName[i];
            if (isalnum(c))
                strNameSortKey += toupper(c);
        }

        // Ensure searchable name is set
        if (strSearchableName.empty())
            strSearchableName = strName;
    }

    const SString& GetVersionSortKey() const { return strVersionSortKey; }

    bool MaybeWontRespond() const
    {
        if (bMasterServerSaysNoResponse || uiCacheNoReplyCount > 0)
            if (GetDataQuality() < SERVER_INFO_QUERY)
                return true;
        return false;
    }

    bool ShouldAllowDataQuality(int iDataQuality) { return iDataQuality >= m_iDataQuality; }
    void SetDataQuality(int iDataQuality)
    {
        m_iDataQuality = iDataQuality;
        uiRevision++;
    }
    int GetDataQuality() const { return m_iDataQuality; }

    uint GetMaxRetries() const { return GetDataQuality() <= SERVER_INFO_ASE_0 || MaybeWontRespond() ? 0 : 1; }

    static bool StaticIsValid(CServerListItem* pServer) { return MapContains(ms_ValidServerListItemMap, pServer); }

    bool                 m_bDoneTcpSend;
    bool                 m_bDoPostTcpQuery;
    int                  m_iTimeoutLength;
    CServerListItemList* m_pItemList;

protected:
    int m_iDataQuality;

    static std::set<CServerListItem*> ms_ValidServerListItemMap;
};

typedef std::list<CServerListItem*>::const_iterator         CServerListIterator;
typedef std::list<CServerListItem*>::const_reverse_iterator CServerListReverseIterator;

// Address and port combo
struct SAddressPort
{
    ulong  m_ulIp;
    ushort m_usPort;
    SAddressPort(in_addr Address, ushort usPort) : m_ulIp(Address.s_addr), m_usPort(usPort) {}
    bool operator<(const SAddressPort& other) const { return m_ulIp < other.m_ulIp || (m_ulIp == other.m_ulIp && (m_usPort < other.m_usPort)); }
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
    std::list<CServerListItem*>              m_List;
    std::map<SAddressPort, CServerListItem*> m_AddressMap;

public:
    std::list<CServerListItem*>& GetList() { return m_List; }

    std::list<CServerListItem*>::iterator begin() { return m_List.begin(); }

    std::list<CServerListItem*>::iterator end() { return m_List.end(); }

    std::list<CServerListItem*>::reverse_iterator rbegin() { return m_List.rbegin(); }

    std::list<CServerListItem*>::reverse_iterator rend() { return m_List.rend(); }

    size_t size() const { return m_List.size(); }

    ~CServerListItemList();
    void             DeleteAll();
    CServerListItem* Find(in_addr Address, ushort usGamePort);
    CServerListItem* AddUnique(in_addr Address, ushort usGamePort, bool bAtFront = false);
    void             AddNewItem(CServerListItem* pItem, bool bAtFront);
    bool             Remove(in_addr Address, ushort usGamePort);
    void             RemoveItem(CServerListItem* pItem);
    void             OnItemChangeAddress(CServerListItem* pItem, in_addr Address, ushort usGamePort);
};

class CServerList
{
public:
    CServerList();
    virtual ~CServerList();

    // Base implementation scans all listed servers
    virtual void Pulse();
    virtual void Refresh();
    virtual bool RemoveNonResponding() { return true; }

    CServerListIterator        IteratorBegin() { return m_Servers.begin(); };
    CServerListIterator        IteratorEnd() { return m_Servers.end(); };
    CServerListReverseIterator ReverseIteratorBegin() { return m_Servers.rbegin(); };
    CServerListReverseIterator ReverseIteratorEnd() { return m_Servers.rend(); };
    unsigned int               GetServerCount() { return m_Servers.size(); };

    bool AddUnique(in_addr Address, ushort usGamePort, bool addAtFront = false);
    void Clear();
    bool Remove(in_addr Address, ushort usGamePort);

    std::string& GetStatus() { return m_strStatus; };
    bool         IsUpdated() { return m_bUpdated; };
    void         SetUpdated(bool bUpdated) { m_bUpdated = bUpdated; };
    int          GetRevision() { return m_iRevision; }
    void         SortByASEVersion();

protected:
    bool                m_bUpdated;
    int                 m_iPass;
    unsigned int        m_nScanned;
    unsigned int        m_nSkipped;
    int                 m_iRevision;
    CServerListItemList m_Servers;
    std::string         m_strStatus;
    std::string         m_strStatus2;
    long long           m_llLastTickCount;
};

// Internet list (grabs the master server list on refresh)
class CServerListInternet : public CServerList
{
public:
    CServerListInternet();
    ~CServerListInternet();
    void Pulse();
    void Refresh();
    bool RemoveNonResponding() { return m_nScanned > 10; }            // Don't remove until net access is confirmed

private:
    CMasterServerManagerInterface* m_pMasterServerManager;
    CElapsedTime                   m_ElapsedTime;
};

// LAN list (scans for LAN-broadcasted servers on refresh)
class CServerListLAN : public CServerList
{
public:
    void Pulse();
    void Refresh();

private:
    void Discover();

    int           m_Socket;
    sockaddr_in   m_Remote;
    unsigned long m_ulStartTime;
};
