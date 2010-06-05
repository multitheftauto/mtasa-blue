/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/ASE.cpp
*  PURPOSE:     Game-Monitor server query protocol handler class
*  DEVELOPERS:  Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Artem Karimov <skybon@live.ru>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern "C"
{
    #include "ASEQuerySDK.h"
}

ASE* ASE::_instance = NULL;

ASE::ASE ( CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, unsigned short usPort, const char* szServerIP, bool bLan )
{
    _instance = this;

    m_bLan = bLan;
    m_usPort = usPort + ( ( m_bLan ) ? SERVER_LIST_QUERY_PORT_OFFSET_LAN : SERVER_LIST_QUERY_PORT_OFFSET );

    m_pMainConfig = pMainConfig;
    m_pPlayerManager = pPlayerManager;

    m_strGameType = "MTA:SA";
    m_strMapName = "None";
    if ( szServerIP )
        m_strIP = szServerIP;
    std::stringstream ss;
    ss << usPort;
    m_strPort = ss.str();

    // Set the sock addr
    m_SockAddr.sin_family = AF_INET;         
    m_SockAddr.sin_port = htons ( m_usPort );
    // If we are in lan broadcasting mode, only let local ips query us
    m_SockAddr.sin_addr.s_addr = INADDR_ANY;

    // Initialize socket
    m_Socket = socket ( AF_INET, SOCK_DGRAM, 0 );

    // If we are in lan only mode, reuse addr to avoid possible conflicts
    if ( m_bLan )
    {
        setsockopt ( m_Socket, SOL_SOCKET, SO_REUSEADDR, "1", sizeof ( "1" ) );
    }

    // Bind the socket
    if ( bind ( m_Socket, ( sockaddr* ) &m_SockAddr, sizeof ( m_SockAddr ) ) != 0 )
    {
        sockclose ( m_Socket );
        m_Socket = NULL;
        return;
    }

    // Set it to non blocking, so we dont have to wait for a packet
    #ifdef WIN32
    unsigned long ulNonBlock = 1;
    ioctlsocket ( m_Socket, FIONBIO, &ulNonBlock );
    #else
    fcntl(m_Socket, F_SETFL, O_NONBLOCK); 
    #endif
}


ASE::~ASE ( void )
{
    _instance = NULL;
    ClearRules ();
}


void ASE::DoPulse ( void )
{
    sockaddr_in SockAddr;
#ifndef WIN32
	socklen_t nLen = sizeof ( sockaddr );
#else
    int nLen = sizeof ( sockaddr );
#endif

    char szBuffer[2];
    int iBuffer = 0;

    // We set the socket to non-blocking so we can just keep reading
    iBuffer = recvfrom ( m_Socket, szBuffer, 1, 0, (sockaddr*)&SockAddr, &nLen );
    if ( iBuffer > 0 )
    {
        std::string strReply;

        switch ( szBuffer[0] )
        {
            case 's':
            { // ASE protocol query
                strReply = QueryFull ();
                break;
            }
            case 'b':
            { // Our own lighter query for ingame browser
                strReply = QueryLight ();
                break;
            }
            case 'r':
            { // Our own lighter query for ingame browser - Release version only
                strReply = QueryLight ();
                break;
            }
            case 'v':
            { // MTA Version (For further possibilities to quick ping, in case we do multiply master servers)
                strReply = MTA_DM_ASE_VERSION;
                break;
            }
            default:
                return;
        }

        // If our reply buffer isn't empty, send it
        if ( !strReply.empty() )
        {
            /*int sent =*/ sendto ( m_Socket,
                                strReply.c_str(),
                                strReply.length(),
						        0,
						        (sockaddr*)&SockAddr,
						        nLen );
        }
    }
}


std::string ASE::QueryFull ( void )
{
    std::stringstream reply;
    std::stringstream temp;

    reply << "EYE1";
    // game
    reply << ( unsigned char ) 4;
    reply << "mta";
    // port
    reply << ( unsigned char ) ( m_strPort.length() + 1 );
    reply << m_strPort;
    // server name
    reply << ( unsigned char ) ( m_pMainConfig->GetServerName ().length() + 1 );
    reply << m_pMainConfig->GetServerName ();
    // game type
    reply << ( unsigned char ) ( m_strGameType.length() + 1 );
    reply << m_strGameType;
    // map name
    reply << ( unsigned char ) ( m_strMapName.length() + 1 );
    reply << m_strMapName;
    // version
    temp << MTA_DM_ASE_VERSION;
    reply << ( unsigned char ) ( temp.str().length() + 1 );
    reply << temp.str();
    // passworded
    reply << ( unsigned char ) 2;
    reply << ( ( m_pMainConfig->HasPassword () ) ? 1 : 0 );
    // players count
    temp.str ( "" );
    temp << m_pPlayerManager->CountJoined ();
    reply << ( unsigned char ) ( temp.str().length () + 1 );
    reply << temp.str();
    // players max
    temp.str ( "" );
    temp << m_pMainConfig->GetMaxPlayers ();
    reply << ( unsigned char ) ( temp.str().length () + 1 );
    reply << temp.str();

    // rules
    list < CASERule* > ::iterator rIter = IterBegin ();
    for ( ; rIter != IterEnd () ; rIter++ )
    {
        // maybe use a map and std strings for rules?
        reply << ( unsigned char ) ( strlen ( (*rIter)->GetKey () ) + 1 );
        reply << (*rIter)->GetKey ();
        reply << ( unsigned char ) ( strlen ( (*rIter)->GetValue () ) + 1 );
        reply << (*rIter)->GetValue ();
    }
    reply << ( unsigned char ) 1;

    // players

    // the flags that tell what data we carry per player ( apparently we need all set cause of GM atm )
    unsigned char ucFlags = 0;
    ucFlags |= 0x01; // nick
    ucFlags |= 0x02; // team
    ucFlags |= 0x04; // skin
    ucFlags |= 0x08; // score
    ucFlags |= 0x16; // ping
    ucFlags |= 0x32; // time

    char szTemp[256] = { '\0' };
    CPlayer* pPlayer = NULL;

    list < CPlayer* > ::const_iterator pIter = m_pPlayerManager->IterBegin ();
    for ( ; pIter != m_pPlayerManager->IterEnd (); pIter++ )
    {
        pPlayer = *pIter;
        if ( pPlayer->IsJoined () )
        {
            reply << ucFlags;
            // nick
            _snprintf ( szTemp, 255, "%s", pPlayer->GetNick () );
            reply << ( unsigned char ) ( strlen ( szTemp ) + 1 );
            reply << szTemp;
            // team (skip)
            reply << ( unsigned char ) 1;
            // skin (skip)
            reply << ( unsigned char ) 1;
            // score (skip)
            reply << ( unsigned char ) 1;
            // ping
            _snprintf ( szTemp, 255, "%u", pPlayer->GetPing () );
            reply << ( unsigned char ) ( strlen ( szTemp ) + 1 );
            reply << szTemp;
            // time (skip)
            reply << ( unsigned char ) 1;
	    }
    }

    return reply.str();
}


std::string ASE::QueryLight ( void )
{
    std::stringstream reply;

    reply << "EYE2";
    // game
    reply << ( unsigned char ) 4;
    reply << "mta";
    // port
    reply << ( unsigned char ) ( m_strPort.length() + 1 );
    reply << m_strPort;
    // server name
    reply << ( unsigned char ) ( m_pMainConfig->GetServerName ().length() + 1 );
    reply << m_pMainConfig->GetServerName ();
    // game type
    reply << ( unsigned char ) ( m_strGameType.length() + 1 );
    reply << m_strGameType;
    // map name
    reply << ( unsigned char ) ( m_strMapName.length() + 1 );
    reply << m_strMapName;
    // version
    std::string temp = MTA_DM_ASE_VERSION;
    reply << ( unsigned char ) ( temp.length() + 1 );
    reply << temp;
    // passworded
    reply << ( unsigned char ) ( ( m_pMainConfig->HasPassword () ) ? 1 : 0 );
    // serial verification?
    reply << ( unsigned char ) ( ( m_pMainConfig->GetSerialVerificationEnabled() ) ? 1 : 0 );
    // players count
    reply << ( unsigned char ) m_pPlayerManager->CountJoined ();
    // players max
    reply << ( unsigned char ) m_pMainConfig->GetMaxPlayers ();

    // players
    char szTemp[256] = { '\0' };
    CPlayer* pPlayer = NULL;

    list < CPlayer* > ::const_iterator pIter = m_pPlayerManager->IterBegin ();
    for ( ; pIter != m_pPlayerManager->IterEnd (); pIter++ )
    {
        pPlayer = *pIter;
        if ( pPlayer->IsJoined () )
        {
            // nick
            _snprintf ( szTemp, 255, "%s", pPlayer->GetNick () );
            reply << ( unsigned char ) ( strlen ( szTemp ) + 1 );
            reply << szTemp;
	    }
    }

    return reply.str();
}


CLanBroadcast* ASE::InitLan ( void )
{
    return new CLanBroadcast ( m_usPort );
}


void ASE::SetGameType ( const char * szGameType )
{
    m_strGameType = szGameType;
}


void ASE::SetMapName ( const char * szMapName )
{
    m_strMapName = szMapName;
}


const char* ASE::GetRuleValue ( const char* szKey )
{
    list < CASERule* > ::iterator iter = m_Rules.begin ();
    for ( ; iter != m_Rules.end () ; iter++ )
    {
        if ( strcmp ( (*iter)->GetKey (), szKey ) == 0 )
        {
            return (*iter)->GetValue ();
        }
    }
    return NULL;
}


void ASE::SetRuleValue ( const char* szKey, const char* szValue )
{
    if ( szKey && szKey [ 0 ] )
    {
        list < CASERule* > ::iterator iter = m_Rules.begin ();
        for ( ; iter != m_Rules.end () ; iter++ )
        {
            CASERule* pRule = *iter;
            if ( strcmp ( (*iter)->GetKey (), szKey ) == 0 )
            {
                if ( szValue && szValue [ 0 ] )
                {
                    (*iter)->SetValue ( szValue );
                }
                else
                {
					// Remove from the list
                    delete pRule;
                    m_Rules.erase ( iter );
                }
				// And return
                return;
            }
        }
        m_Rules.push_back ( new CASERule ( szKey, szValue ) );
    }
}


bool ASE::RemoveRuleValue ( const char* szKey )
{
    list < CASERule* > ::iterator iter = m_Rules.begin ();
    for ( ; iter != m_Rules.end () ; iter++ )
    {
        CASERule* pRule = *iter;
        if ( strcmp ( pRule->GetKey (), szKey ) == 0 )
        {
            delete pRule;
            m_Rules.erase ( iter );
            return true;
        }
    }
    return false;
}


void ASE::ClearRules ( void )
{
    list < CASERule* > ::iterator iter = m_Rules.begin ();
    for ( ; iter != m_Rules.end () ; iter++ )
    {
        delete *iter;
    }
    m_Rules.clear ();
}
