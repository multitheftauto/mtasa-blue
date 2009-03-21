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

using namespace std;

extern CCore* g_pCore;

CServerList::CServerList ( void )
{
    m_bUpdated = false;
    m_iPass = 0;
    m_strStatus = "Idle";
    m_nScanned = 0;
}


CServerList::~CServerList ( void )
{
    Clear ();
}


void CServerList::Clear ( void )
{
    // Clear all entries
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
        delete *i;
    m_Servers.clear ();
    m_nScanned = 0;
}


void CServerList::Pulse ( void )
{
    unsigned int j = 0, n = m_Servers.size ();

    // Scan all servers in our list, and keep the value of scanned servers
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ ) {
        CServerListItem * pServer = *i;
        if ( pServer->Pulse () ) j++;
        if ( j >= SERVER_LIST_QUERIES_PER_PULSE ) break;
    }

    // If we queried any new servers, we should toggle the GUI update flag
    m_bUpdated = m_bUpdated || ( j > 0 );

    // Check whether we are done scanning
    std::stringstream ss;

    // Store the new number of scanned servers
    m_nScanned += j;
    if ( m_nScanned == n ) {
        ss << "Found " << m_nScanned << " servers";
        // We are no longer refreshing
        m_iPass = 0;
    } else {
        ss << "Scanned " << m_nScanned << " servers";
    }

    // Update our status message
    m_strStatus = ss.str ();
}


bool CServerList::Exists ( CServerListItem Server )
{
    // Look for a duplicate entry
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
        if ( **i == Server ) return true;
    return false;
}


void CServerList::Remove ( CServerListItem Server )
{
    // Look for a duplicate entry
    for ( CServerListIterator i = m_Servers.begin (); i != m_Servers.end (); i++ )
    {
        if ( **i == Server )
        {
            m_Servers.remove ( *i );
            return;
        }
    }
}


void CServerList::Refresh ( void )
{   // Assumes we already have a (saved) list of servers, so we just need to refresh
    m_iPass = 1;
}

void CServerListInternet::Refresh ( void )
{   // Gets the server list from the master server and refreshes
    m_ulStartTime = CClientTime::GetTime ();
    memset ( m_szBuffer, 0, SERVER_LIST_DATA_BUFFER );
    m_HTTP.Get ( SERVER_LIST_MASTER_URL, m_szBuffer, SERVER_LIST_DATA_BUFFER - 1 );
    m_iPass = 1;
    m_bUpdated = true;

    // Clear the previous server list
    Clear ();
}


void CServerListInternet::Pulse ( void )
{   // We also need to take care of the master server list here
    char *szBuffer;
    unsigned int nDataLength;
    unsigned long ulTime = CClientTime::GetTime () - m_ulStartTime;

    if ( m_iPass == 1 ) {
        // We are polling for the master server list (first pass)
        stringstream ss;
        ss << "Requesting master server list (" << ulTime << "ms elapsed)";
        m_strStatus = ss.str ();
        m_bUpdated = true;
        
        // Attempt to get the HTTP data
        if ( m_HTTP.GetData ( &szBuffer, nDataLength ) ) {
            // We got the data, parse it and switch pass
            if ( ParseList ( szBuffer, nDataLength ) ) {
                m_iPass++;
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
    } else if ( m_iPass == 2 ) {
        // We are scanning our known servers (second pass)
        CServerList::Pulse ();
    }
}


bool CServerListInternet::ParseList ( const char *szBuffer, unsigned int nLength )
{
    unsigned int i = 0, j = 0;

    // Read out the server count
    if ( nLength < 2 ) return false;
    unsigned int uiCount = htons ( *((unsigned short*)&szBuffer[0]) );
    i = 2;

    // Add all servers until we hit the count or nLength
    while ( i < ( nLength - 6 ) ) {
        CServerListItem item;

        // Read the IPv4-address
        item.Address.S_un.S_un_b.s_b1 = szBuffer[i];
        item.Address.S_un.S_un_b.s_b2 = szBuffer[i+1];
        item.Address.S_un.S_un_b.s_b3 = szBuffer[i+2];
        item.Address.S_un.S_un_b.s_b4 = szBuffer[i+3];

        // Read the query port
        item.usQueryPort = htons ( *((unsigned short*)(&szBuffer[i+4])) );

        // Add the server
        Add ( item );
        i += 6;
    }
    return true;
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
                CServerListItem Server ( m_Remote.sin_addr, usPort );
                // Add the server if doesn't already exist
                if ( !Exists ( Server ) ) Add ( Server );
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
    setsockopt ( m_Socket, SOL_SOCKET, SO_REUSEADDR, "1", sizeof ( "1" ) );
    if ( setsockopt ( m_Socket, SOL_SOCKET, SO_BROADCAST, "1", sizeof ( "1" ) ) != 0 ) {
        m_strStatus = "Cannot bind LAN-broadcast socket";
        return;
    }

    // Prepare the structures
    memset ( &m_Remote, 0, sizeof (m_Remote) );
	m_Remote.sin_family			= AF_INET;
	m_Remote.sin_port			= htons ( SERVER_LIST_BROADCAST_PORT ); 
	m_Remote.sin_addr.s_addr	= INADDR_BROADCAST;

    // Discover other servers by sending out the broadcast packet
    Discover ();
}


void CServerListLAN::Discover ( void )
{
    m_strStatus = "Attempting to discover LAN servers";

    // Send out the broadcast packet
    std::string strQuery = std::string ( SERVER_LIST_CLIENT_BROADCAST_STR ) + " " + std::string ( MTA_VERSION );
	sendto ( m_Socket, strQuery.c_str (), strQuery.length () + 1, 0, (sockaddr *)&m_Remote, sizeof (m_Remote) );
    
    // Keep the time
    m_ulStartTime = CClientTime::GetTime ();
}


bool CServerListItem::Pulse ( void )
{   // Queries the server on it's query port (ASE protocol)
    // and returns whether it is done scanning
    if ( bScanned ) return false;

    char szBuffer[SERVER_LIST_QUERY_BUFFER] = {0};

    if ( m_ulQueryStart == 0 ) {
        Query ();
    } else {
        // Poll the socket
        sockaddr_in clntAddr;
        int addrLen = sizeof ( clntAddr );
        int len = recvfrom ( m_Socket, szBuffer, SERVER_LIST_QUERY_BUFFER, MSG_PARTIAL, (sockaddr *) &clntAddr, &addrLen );
        int error = WSAGetLastError();
        if ( len >= 0 ) {
            // Parse data
            ParseQuery ( szBuffer, len );
            return true;
        }
    }

    return false;
}


void CServerListItem::Query ( void )
{   // Performs a query according to ASE protocol
    sockaddr_in addr;
    memset ( &addr, 0, sizeof(addr) );
    addr.sin_family = AF_INET;
	addr.sin_addr = Address;
	addr.sin_port = htons ( usQueryPort );

    int ret = sendto ( m_Socket, "s", 1, 0, (sockaddr *) &addr, sizeof(addr) );
	if ( ret == 1 )
        m_ulQueryStart = CClientTime::GetTime ();
}


inline std::string ReadNextString ( const char * szBuffer, unsigned int &i )
{
    unsigned char len = szBuffer[i++];
    const char *ptr = szBuffer + i;
    i += len - 1;
    return std::string ( ptr, len - 1 );
}

inline void SkipNextString ( const char * szBuffer, unsigned int &i )
{
    unsigned char len = szBuffer[i++];
    i += len - 1;
}

bool CServerListItem::ParseQuery ( const char * szBuffer, unsigned int nLength )
{
    unsigned int i = 0;
    #define _C    szBuffer[i++]
    #define _B(y) _C==y
    #define _STR ReadNextString(szBuffer,i)
    #define _SKP SkipNextString(szBuffer,i)

    // Check header
    if(!(_B('E')&&_B('Y')&&_B('E')&&_B('1'))) return false;
    
    // Get IP as string
    const char* szIP = inet_ntoa ( Address );

    g_pCore->GetConsole()->Printf ( "Got server %s", szIP );

    // Calculate the ping/latency
    nPing           = CClientTime::GetTime () - m_ulQueryStart;

    // Parse relevant data
    strGame         = _STR;
    usGamePort      = atoi ( _STR.c_str () );
    strHost         = szIP;
    strName         = _STR;
    strType         = _STR;
    strMap          = _STR;
    strVersion      = _STR;
    bPassworded     = ( _STR == "1" ) ? true : false;
    nPlayers        = atoi ( _STR.c_str () );
    nMaxPlayers     = atoi ( _STR.c_str () );

    // Skip variables (two strings each)
    while ( szBuffer[i] != 1 && i < nLength ) {
        if ( ( _STR.compare ( "SerialVerification" ) == 0 ) && ( _STR.compare ( "yes" ) == 0 ) )
            bSerials = true;
    }
    if ( i == nLength ) return false;
    i++;

    // Parse player data
    vecPlayers.clear ();
    while ( i < nLength ) {
        unsigned char playerFlags = _C;
        CServerListItemPlayer Player;

        if ( playerFlags & 1 )
            Player.strName = _STR;
        if ( playerFlags & 2 )
            Player.strTeam = _STR;
        if ( playerFlags & 4 )
            Player.strSkin = _STR;
        if ( playerFlags & 8 )
            Player.nScore = atoi ( _STR.c_str () );
        if ( playerFlags & 16 )
            Player.nPing = atoi ( _STR.c_str () );
        if ( playerFlags & 32 )
            Player.nTime = atoi ( _STR.c_str () );

        vecPlayers.push_back ( Player );
    }

    bScanned = true;
    return true;
}
