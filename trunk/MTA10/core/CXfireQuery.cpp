/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CXfireQuery.cpp
*  PURPOSE:     Class file for the XFireQuery Server Information Class
*  DEVELOPERS:  Cazomino05 <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CXfireServerInfo::ReadString ( std::string &strRead, const char * szBuffer, unsigned int &i, unsigned int nLength )
{
    if ( i <= nLength )
    {
        unsigned char len = szBuffer[i];
        if ( i + len <= nLength )
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

bool CXfireServerInfo::ParseQuery ( const char * szBuffer, unsigned int nLength )
{

    // Check header
    if ( strncmp ( szBuffer, "EYE3", 4 ) != 0 )
        return false;
    

    unsigned int i = 4;

    // Game
    if ( !ReadString ( strGameName, szBuffer, i, nLength ) )
        return false;

    // Server name
    if ( !ReadString ( strName, szBuffer, i, nLength ) )
        return false;

    // Game type
    if ( !ReadString ( strGameMode, szBuffer, i, nLength ) )
        return false;

    // Map name
    if ( !ReadString ( strMap, szBuffer, i, nLength ) )
        return false;

    // Version
    if ( !ReadString ( strVersion, szBuffer, i, nLength ) )
        return false;

    // Got space for password, player count, players max?
    if ( i + 3 > nLength )
    {
        return false;
    }

    bPassworded = ( szBuffer[i++] == 1 );

    if ( atof(strVersion) <= 1.0f )
    {
        nPlayers = (unsigned char)szBuffer[i++];
        nMaxPlayers = (unsigned char)szBuffer[i++];
    }
    else
    {
        nPlayers = ntohs ( *(unsigned short *)&szBuffer[i] );
        i += 2;
        nMaxPlayers = ntohs ( *(unsigned short *)&szBuffer[i] );
        i += 2;
    }

    return true;
}
std::string CXfireServerInfo::Pulse ( void )
{   // Queries the server on it's query port (ASE protocol)
    // and returns whether it is done scanning
    if ( bScanned || bSkipped ) return "Done";

    char szBuffer[SERVER_LIST_QUERY_BUFFER] = {0};

    if ( m_ulQueryStart == 0 ) {
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
            ParseQuery ( szBuffer, len );
            return "ParsedQuery";
        }

        if ( CClientTime::GetTime () - m_ulQueryStart > SERVER_LIST_ITEM_TIMEOUT )
        {
            bSkipped = true;
            return "NoReply";
        }

        return "WaitingReply";
    }
}


void CXfireServerInfo::Query ( void )
{   // Performs a query according to ASE protocol
    sockaddr_in addr;
    memset ( &addr, 0, sizeof(addr) );
    addr.sin_family = AF_INET;
    addr.sin_addr = Address;
    addr.sin_port = htons ( usQueryPort );

    int ret = sendto ( m_Socket, "x", 1, 0, (sockaddr *) &addr, sizeof(addr) );
    if ( ret == 1 )
        m_ulQueryStart = CClientTime::GetTime ();
}