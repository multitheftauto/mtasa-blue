/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerList.cpp
*  PURPOSE:     Master server list querying
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CServerBrowser.MasterServerManager.h"

using namespace std;

extern CCore* g_pCore;
std::set < CServerListItem* > CServerListItem::ms_ValidServerListItemMap;

CServerList::CServerList ( void )
{
    m_bUpdated = false;
    m_iPass = 0;
    m_strStatus = "Idle";
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision = 1;
}


CServerList::~CServerList ( void )
{
    Clear ();
}


void CServerList::Clear ( void )
{
#if 1
    // Clear all entries
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
        delete *i;
    m_Servers.clear ();
#else
    for ( std::list<CServerListItem*>::iterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
        (*i)->ResetForRefresh ();
#endif
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision++;
    m_llLastTickCount = 0;
}


void CServerList::Pulse ( void )
{
    // Get QueriesPerSecond setting
    int iQueriesPerSecond = 50;
    int iVar;
    CVARS_GET ( "browser_speed", iVar );
    if ( iVar == 0 ) iQueriesPerSecond = 4;
    else if ( iVar == 1 ) iQueriesPerSecond = 10;
    else if ( iVar == 2 ) iQueriesPerSecond = 50;

    long long llTickCount = GetTickCount64_ ();
    // Ensure m_llLastTickCount is initialized
    m_llLastTickCount = m_llLastTickCount ? m_llLastTickCount : llTickCount;
    // Ensure m_llLastTickCount is in range
    m_llLastTickCount = Clamp ( llTickCount - 1000, m_llLastTickCount, llTickCount );

    // Calc number of queries this pulse
    int iTicksPerQuery = 1000 / Max ( 1, iQueriesPerSecond );
    int iDeltaTicks = llTickCount - m_llLastTickCount;
    int iNumQueries = iDeltaTicks / Max ( 1, iTicksPerQuery );
    iNumQueries = Clamp ( 0, iNumQueries, SERVER_LIST_QUERIES_PER_PULSE );
    int iNumTicksUsed = iNumQueries * iTicksPerQuery;
    m_llLastTickCount += iNumTicksUsed;

    unsigned int uiQueriesSent = 0;
    unsigned int uiQueriesResent = 0;
    unsigned int uiRepliesParsed = 0;
    unsigned int uiNoReplies = 0;
    unsigned int uiActiveServers = 0;
    unsigned int uiTotalSlots = 0;
    unsigned int uiOccupiedSlots = 0;
    bool bRemoveNonResponding = RemoveNonResponding ();

    // If a query is going to be done this pass, try to find high priority items first
    if ( iNumQueries > 0 )
    {
        std::vector < SAddressPort > endpointList;
        CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetServerBrowser ()->GetVisibleEndPointList ( endpointList );

        for ( std::vector < SAddressPort >::iterator iter = endpointList.begin (); iter != endpointList.end (); ++iter )
        {
            CServerListItem* pServer = m_Servers.Find ( (in_addr&)iter->m_ulIp, iter->m_usPort );
            // ** Crash - pServer can sometimes be invalid **
            if ( pServer && pServer->WaitingToSendQuery () )
            {
                std::string strResult = pServer->Pulse ( (int)( uiQueriesSent /*+ uiQueriesResent*/ ) < iNumQueries, bRemoveNonResponding );
                assert ( strResult == "SentQuery" );
                uiQueriesSent++;
                if ( (int)uiQueriesSent >= iNumQueries )
                    break;
            }
        }
    }

    // Scan all servers in our list, and keep the value of scanned servers
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
    {
        CServerListItem * pServer = *i;
        uint uiPrevRevision = pServer->uiRevision;
        std::string strResult = pServer->Pulse ( (int)( uiQueriesSent /*+ uiQueriesResent*/ ) < iNumQueries, bRemoveNonResponding );
        if ( uiPrevRevision != pServer->uiRevision )
            m_bUpdated |= true;         // Flag GUI update
        if ( strResult == "SentQuery" )
            uiQueriesSent++;
        else
        if ( strResult == "ResentQuery" )
            uiQueriesResent++;
        else
        if ( strResult == "ParsedQuery" )
            uiRepliesParsed++;
        else
        if ( strResult == "NoReply" )
            uiNoReplies++;

        if ( pServer->nMaxPlayers && !pServer->bMaybeOffline && !pServer->MaybeWontRespond () )
        {
            uiActiveServers++;
            uiTotalSlots += pServer->nMaxPlayers;
            uiOccupiedSlots += pServer->nPlayers;
        }
    }

    // Check whether we are done scanning
    std::stringstream ss;

    // Store the new number of scanned servers
    m_nScanned += uiRepliesParsed;
    m_nSkipped += uiNoReplies;
#if MTA_DEBUG
    //OutputDebugLine ( SString ( "[Browser] %08x  Size: %d  m_nScanned:%d  m_nSkipped:%d [%d]  iNumQueries: %d", this, m_Servers.size(), m_nScanned, m_nSkipped, m_nScanned+m_nSkipped, iNumQueries ) );
#endif

    if ( m_nScanned + m_nSkipped == m_Servers.size () ) {
        // We are no longer refreshing
        m_iPass = 0;
    }

    ss << "   ";
    if ( uiTotalSlots > 0 )
    {
        ss << uiOccupiedSlots << " player";
        if ( uiOccupiedSlots != 1 )
            ss << "s";
        ss << " on ";
    }
    ss << uiActiveServers << " server";
    if ( uiActiveServers != 1 )
        ss << "s";
    if ( m_iPass )
        ss << "...";

    // Update our status message
    m_strStatus = ss.str () + m_strStatus2;
}


// Return true if did add
bool CServerList::AddUnique ( in_addr Address, ushort usGamePort, bool addAtFront )
{
    if ( m_Servers.Find ( Address, usGamePort ) )
        return false;
    m_Servers.Add ( Address, usGamePort, addAtFront );
    return true;
}

bool CServerList::Remove ( in_addr Address, ushort usGamePort )
{
    return m_Servers.Remove ( Address, usGamePort );
}


void CServerList::Refresh ( void )
{   // Assumes we already have a (saved) list of servers, so we just need to refresh

    // Reinitialize each server list item
    for ( std::list<CServerListItem*>::iterator iter = m_Servers.begin (); iter != m_Servers.end (); iter++ )
    {
        CServerListItem* pOldItem = *iter;
        *iter = new CServerListItem( *pOldItem );
        delete pOldItem;
    }

    m_iPass = 1;
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision++;
}


CServerListInternet::CServerListInternet ( void )
{
    m_ElapsedTime.SetMaxIncrement ( 500 );
    m_pMasterServerManager = NewMasterServerManager ();
}


CServerListInternet::~CServerListInternet ( void )
{
    delete m_pMasterServerManager;
    m_pMasterServerManager = NULL;
}


void CServerListInternet::Refresh ( void )
{   // Gets the server list from the master server and refreshes
    m_ElapsedTime.Reset ();
    m_pMasterServerManager->Refresh ();
    m_iPass = 1;
    m_bUpdated = true;

    // Clear the previous server list
    Clear ();
}



static bool SortByASEVersionCallback ( const CServerListItem* const d1, const CServerListItem* const d2 )
{
    return d1->strVersionSortKey < d2->strVersionSortKey;
};

//
// Ensure serverlist is sorted by MTA ASE version
//
void CServerList::SortByASEVersion ( void )
{
    m_Servers.GetList ().sort ( SortByASEVersionCallback );
}


void CServerListInternet::Pulse ( void )
{   // We also need to take care of the master server list here
    unsigned long ulTime = m_ElapsedTime.Get ();

    if ( m_iPass == 1 ) {
        // We are polling for the master server list (first pass)
        stringstream ss;
        ss << "Requesting master server list (" << ulTime << "ms elapsed)";
        m_strStatus = ss.str ();
        m_strStatus2 = "";
        m_bUpdated = true;
        
        // Attempt to get the data
        if ( m_pMasterServerManager->HasData () )
        {
            if ( m_pMasterServerManager->ParseList ( m_Servers ) )
            {
                m_iPass++;
                GetServerCache ()->GetServerListCachedInfo ( this );
                SortByASEVersion ();
            } else {
                // Abort
                m_strStatus = "Master server list could not be parsed.";
                m_iPass = 0;
            }
        } else {
            // Take care of timeouts
            if ( ulTime > SERVER_LIST_MASTER_TIMEOUT ) {
                // Abort
                m_strStatus = "Master server list could not be retrieved.";
                m_iPass = 0;
            }
        }
        if ( m_iPass == 0 )
        {
            // If query failed, load from backup
            GetServerCache ()->GenerateServerList ( this );
            GetServerCache ()->GetServerListCachedInfo ( this );
            SortByASEVersion ();
            m_strStatus2 = string ( "  (Backup server list)" );
            m_iPass = 2;
        }
    } else if ( m_iPass == 2 ) {
        // We are scanning our known servers (second pass)
        CServerList::Pulse ();
    }
}


void CServerListLAN::Pulse ( void )
{
    char szBuffer[32];

    // Broadcast the discover packet on a regular interval
    if ( (CClientTime::GetTime () - m_ulStartTime) > SERVER_LIST_BROADCAST_REFRESH )
        Discover ();

    // Poll our socket to discover any new servers
    timeval tm;
    tm.tv_sec = 0;
    tm.tv_usec = 0;
    fd_set readfds;
    readfds.fd_array[0] = m_Socket;
    readfds.fd_count = 1;
    int len = sizeof ( m_Remote );
    if ( select ( 1, &readfds, NULL, NULL, &tm ) > 0 )
        if ( recvfrom ( m_Socket, szBuffer, sizeof (szBuffer), 0, (sockaddr *) &m_Remote, &len ) > 10 )
            if ( strncmp ( szBuffer, SERVER_LIST_SERVER_BROADCAST_STR, strlen ( SERVER_LIST_SERVER_BROADCAST_STR ) ) == 0 ) {
                unsigned short usPort = ( unsigned short ) atoi ( &szBuffer[strlen ( SERVER_LIST_SERVER_BROADCAST_STR ) + 1] );
                // Add the server if doesn't already exist
                AddUnique ( m_Remote.sin_addr, usPort - SERVER_LIST_QUERY_PORT_OFFSET_LAN );
                CServerBrowser::GetSingleton ().AddKnownLanServer ( m_Remote.sin_addr );
            }

    // Scan our already known servers
    CServerList::Pulse ();
}


void CServerListLAN::Refresh ( void )
{   // Gets the server list from LAN-broadcasting servers
    m_iPass = 1;
    m_bUpdated = true;

    // Create the LAN-broadcast socket
    closesocket ( m_Socket );
    m_Socket = socket ( AF_INET, SOCK_DGRAM, 0 );
    const int Flags = 1;
    setsockopt ( m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&Flags, sizeof ( Flags ) );
    if ( setsockopt ( m_Socket, SOL_SOCKET, SO_BROADCAST, (const char *)&Flags, sizeof ( Flags ) ) != 0 ) {
        m_strStatus = "Cannot bind LAN-broadcast socket";
        return;
    }

    // Prepare the structures
    memset ( &m_Remote, 0, sizeof (m_Remote) );
    m_Remote.sin_family         = AF_INET;
    m_Remote.sin_port           = htons ( SERVER_LIST_BROADCAST_PORT ); 
    m_Remote.sin_addr.s_addr    = INADDR_BROADCAST;

    // Clear the previous server list
    Clear ();

    // Discover other servers by sending out the broadcast packet
    Discover ();
}


void CServerListLAN::Discover ( void )
{
    m_strStatus = "Attempting to discover LAN servers";

    // Send out the broadcast packet
    std::string strQuery = std::string ( SERVER_LIST_CLIENT_BROADCAST_STR ) + " " + std::string ( MTA_DM_ASE_VERSION );
    sendto ( m_Socket, strQuery.c_str (), strQuery.length () + 1, 0, (sockaddr *)&m_Remote, sizeof (m_Remote) );
    
    // Keep the time
    m_ulStartTime = CClientTime::GetTime ();
}


std::string CServerListItem::Pulse ( bool bCanSendQuery, bool bRemoveNonResponding )
{   // Queries the server on it's query port (ASE protocol)
    // and returns whether it is done scanning
    if ( bScanned || bSkipped ) return "Done";

    char szBuffer[SERVER_LIST_QUERY_BUFFER] = {0};

    if ( WaitingToSendQuery () ) {
        if ( !bCanSendQuery )
            return "NotSentQuery";
        Query ();
        return "SentQuery";
    } else {
        // Poll the socket
        sockaddr_in clntAddr;
        int addrLen = sizeof ( clntAddr );
        int len = recvfrom ( m_Socket, szBuffer, SERVER_LIST_QUERY_BUFFER, MSG_PARTIAL, (sockaddr *) &clntAddr, &addrLen );
        int error = WSAGetLastError();
        if ( len >= 0 ) {
            // Parse data
            if ( ParseQuery ( szBuffer, len ) )
            {
                CloseSocket ();
                bMaybeOffline = false;
                SetDataQuality ( SERVER_INFO_QUERY );
                uiCacheNoReplyCount = 0;
                uiRevision++;           // To flag browser gui update
                GetServerCache ()->SetServerCachedInfo ( this );    // Save parsed info in the cache
                return "ParsedQuery";
            }
        }

        if ( m_ElapsedTime.Get () > SERVER_LIST_ITEM_TIMEOUT )
        {
            if ( bKeepFlag )
                bRemoveNonResponding = false;

            if ( bRemoveNonResponding )
            {
                bMaybeOffline = true;       // Flag to help 'Include offline' browser option
                nPlayers = 0;               // We don't have player names, so zero this now
            }
            uiRevision++;               // To flag browser gui update
            uint uiMaxRetries = GetDataQuality () <= SERVER_INFO_ASE_0 || MaybeWontRespond () ? 0 : 1;

            if ( uiQueryRetryCount < uiMaxRetries )
            {
                // Try again
                uiQueryRetryCount++;
                uiRevision++;           // To flag browser gui update
                if ( GetDataQuality () > SERVER_INFO_ASE_0 )
                    GetServerCache ()->SetServerCachedInfo ( this );
                Query ();
                return "ResentQuery";
            }
            else
            {
                // Give up
                CloseSocket ();
                uiRevision++;           // To flag browser gui update

                if ( bRemoveNonResponding )
                {
                    uiCacheNoReplyCount++;  // Keep a persistent count of failures. (When uiCacheNoReplyCount gets to 3, the server is removed from the Server Cache)
                    bSkipped = true;
                    if ( GetDataQuality () > SERVER_INFO_ASE_0 )
                        GetServerCache ()->SetServerCachedInfo ( this );
                }
                else
                {
                    // Pretend everything is fine until network access is confirmed
                    bScanned = true;
                    bMaybeOffline = false;
                }
                return "NoReply";
            }
        }

        return "WaitingReply";
    }
}


unsigned short CServerListItem::GetQueryPort ( void )
{
    if ( CServerBrowser::GetSingleton ().IsKnownLanServer ( Address ) )
        return usGamePort + SERVER_LIST_QUERY_PORT_OFFSET_LAN;
    return usGamePort + SERVER_LIST_QUERY_PORT_OFFSET;
}

void CServerListItem::Query ( void )
{   // Performs a query according to ASE protocol
    sockaddr_in addr;
    memset ( &addr, 0, sizeof(addr) );
    addr.sin_family = AF_INET;
    addr.sin_addr = Address;
    addr.sin_port = htons ( GetQueryPort () );

    // Initialize socket on demand
    if ( m_Socket == INVALID_SOCKET )
    {
        m_Socket = socket ( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
        u_long flag = 1;
        ioctlsocket ( m_Socket, FIONBIO, &flag );
    }

    sendto ( m_Socket, "r", 1, 0, (sockaddr *) &addr, sizeof(addr) );
    m_ElapsedTime.Reset ();
}


bool ReadString ( std::string &strRead, const char * szBuffer, unsigned int &i, unsigned int nLength )
{
    if ( i <= nLength )
    {
        unsigned char len = szBuffer[i];
        if ( i + len <= nLength && len > 0 )
        {
            const char *ptr = &szBuffer[i + 1];
            i += len;
            strRead = std::string ( ptr, len - 1 );
            return true;
        }
        i++;
    }
    return false;
}


bool CServerListItem::ParseQuery ( const char * szBuffer, unsigned int nLength )
{
    // Check length
    if ( nLength < 15 )
        return false;

    // Check header
    if ( strncmp ( szBuffer, "EYE2", 4 ) != 0 )
        return false;
    
    // Get IP as string
    const char* szIP = inet_ntoa ( Address );

    // Calculate the ping/latency
    nPing = m_ElapsedTime.Get ();

    // Parse relevant data
    SString strTemp;
    SString strMapTemp;
    unsigned int i = 4;

    // IP
    strHost = szIP;

    // Game
    if ( !ReadString ( strTemp, szBuffer, i, nLength ) )
        return false;
    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_GAME_NAME ) == false )
        strGameName = strTemp;

    // Port
    if ( !ReadString ( strTemp, szBuffer, i, nLength ) )
        return false;
    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_PORT ) == false )
        usGamePort = atoi ( strTemp.c_str () );

    // Server name
    if ( !ReadString ( strTemp, szBuffer, i, nLength ) )
        return false;
    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_NAME ) == false )
        strName = strTemp;

    // Game type
    if ( !ReadString ( strTemp, szBuffer, i, nLength ) )
        return false;
    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_GAME_MODE ) == false )
        strGameMode = strTemp;

    // Map name
    if ( !ReadString ( strMapTemp, szBuffer, i, nLength ) )
        return false;
    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_MAP ) == false )
        strMap = strMapTemp;

    // Version
    if ( !ReadString ( strTemp, szBuffer, i, nLength ) )
        return false;
    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_VERSION ) == false )
        strVersion = strTemp;

    // Got space for password, serial verification, player count, players max?
    if ( i + 4 > nLength )
    {
        return false;
    }

    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_PASSWORDED ) == false )
        bPassworded = ( szBuffer[i++] == 1 );

    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_SERIALS ) == false )
        bSerials = ( szBuffer[i++] == 1 );

    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_PLAYER_COUNT ) == false )
        nPlayers = (unsigned char)szBuffer[i++];

    if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_MAX_PLAYER_COUNT ) == false )
        nMaxPlayers = (unsigned char)szBuffer[i++];

    // Recover large player count if present
    const SString strPlayerCount = strMapTemp.Right ( strMapTemp.length () - strlen ( strMapTemp ) - 1 );
    if ( !strPlayerCount.empty () )
    {
        SString strJoinedPlayers, strMaxPlayers;
        if ( strPlayerCount.Split ( "/", &strJoinedPlayers, &strMaxPlayers ) )
        {
            if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_PLAYER_COUNT ) == false )
                nPlayers = atoi ( strJoinedPlayers );
            if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_MAX_PLAYER_COUNT ) == false )
                nMaxPlayers = atoi ( strMaxPlayers );
        }
    }

    // Recover server build type if present
    const SString strBuildType = strPlayerCount.Right ( strPlayerCount.length () - strlen ( strPlayerCount ) - 1 );
    if ( !strBuildType.empty () )
        m_iBuildType = atoi ( strBuildType );
    else
        m_iBuildType = 1;

    // Recover server build number if present
    const SString strBuildNumber = strBuildType.Right ( strBuildType.length () - strlen ( strBuildType ) - 1 );
    if ( !strBuildNumber.empty () )
        m_iBuildNumber = atoi ( strBuildNumber );
    else
        m_iBuildNumber = 0;

    // Recover server ping status if present
    const SString strPingStatus = strBuildNumber.Right ( strBuildNumber.length () - strlen ( strBuildNumber ) - 1 );
    CCore::GetSingleton ().GetNetwork ()->UpdatePingStatus ( *strPingStatus, nPlayers );

    // Get player nicks
    vecPlayers.clear ();
    while ( i < nLength )
    {
        std::string strPlayer;
        try
        {
            if ( ReadString ( strPlayer, szBuffer, i, nLength ) )
            {
                // Remove color code, unless that results in an empty string
                std::string strResult = RemoveColorCode ( strPlayer.c_str () );
                if ( strResult.length () == 0 )
                    strResult = strPlayer;
                if ( ( uiMasterServerSaysRestrictions & ASE_FLAG_PLAYER_LIST ) == false )
                    vecPlayers.push_back ( strResult );
            }
        }
        catch ( ... )
        {
            // yeah that's what I thought.
            return false;
        }
    }

    bScanned = true;

    PostChange ();
    return true;
}


void CServerListItem::ResetForRefresh ( void )
{
    CloseSocket ();
    bScanned = false;
    bSkipped = false;
    m_ElapsedTime.Reset ();
    uiQueryRetryCount = 0;

    if ( m_iDataQuality >= SERVER_INFO_QUERY )
        m_iDataQuality = SERVER_INFO_QUERY - 1;
    bMaybeOffline = false;
}
