//
// CRemoteMasterServer.hpp
//

#include "StdInc.h"
#include "CServerBrowser.RemoteMasterServer.h"


///////////////////////////////////////////////////////////////
//
//
// CRemoteMasterServer class
//
//
///////////////////////////////////////////////////////////////
class CRemoteMasterServer : public CRemoteMasterServerInterface
{
public:
    ZERO_ON_NEW
                            CRemoteMasterServer        ( void );
                            ~CRemoteMasterServer       ( void );

    // CRemoteMasterServerInterface
    virtual void            Refresh                     ( void );
    virtual bool            HasData                     ( void );
    virtual bool            ParseList                   ( std::list < CServerListItem* >& itemList );

    // CRemoteMasterServer
    void                    Init                        ( const SString& strURL );
protected:
    bool                    ParseListVer0               ( std::list < CServerListItem* >& itemList );
    bool                    ParseListVer1               ( std::list < CServerListItem* >& itemList );
    CServerListItem*        GetServerListItem           ( std::list < CServerListItem* >& itemList, in_addr Address, ushort usQueryPort );

    long long               m_llLastRefreshTime;
    SString                 m_strStage;
    SString                 m_strURL;
    CHTTPClient             m_HTTP;
    CBuffer                 m_Data;
};


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer instantiation
//
//
//
///////////////////////////////////////////////////////////////
CRemoteMasterServerInterface* NewRemoteMasterServer ( const SString& strURL )
{
    CRemoteMasterServer* pMasterServer = new CRemoteMasterServer ();
    pMasterServer->Init ( strURL );
    return pMasterServer;
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer implementation
//
//
//
///////////////////////////////////////////////////////////////
CRemoteMasterServer::CRemoteMasterServer ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::~CRemoteMasterServer
//
//
//
///////////////////////////////////////////////////////////////
CRemoteMasterServer::~CRemoteMasterServer ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::Init
//
//
//
///////////////////////////////////////////////////////////////
void CRemoteMasterServer::Init ( const SString& strURL )
{
    m_strURL = strURL;
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::Refresh
//
//
//
///////////////////////////////////////////////////////////////
void CRemoteMasterServer::Refresh ( void )
{
    // If it's been less than a minute and we has data, don't send a new request
    if ( GetTickCount64_ () - m_llLastRefreshTime < 60000 && m_strStage == "hasdata" )
        return;

    // Send new request
    m_HTTP.Get ( m_strURL );
    m_llLastRefreshTime = GetTickCount64_ ();
    m_strStage = "waitingreply";
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::HasData
//
//
//
///////////////////////////////////////////////////////////////
bool CRemoteMasterServer::HasData ( void )
{
    if ( m_strStage == "waitingreply" )
    {
        // Attempt to get the HTTP data
        CHTTPBuffer buffer;
        if ( m_HTTP.GetData ( buffer ) )
        {
            m_strStage = "hasdata";
            m_Data = CBuffer ( buffer.GetData (), buffer.GetSize () );
        }
    }

    return m_strStage == "hasdata";
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::ParseList
//
// Parse reply from master server
//
///////////////////////////////////////////////////////////////
bool CRemoteMasterServer::ParseList ( std::list < CServerListItem* >& itemList )
{
    CBufferReadStream stream ( m_Data, true );

    // Figure out which type of list it is
    unsigned short usVersion = 0;
    unsigned short usCount = 0;
    stream.Read ( usCount );
    if ( usCount == 0 )
        stream.Read ( usVersion );

    if ( usVersion == 0 )
        return ParseListVer0 ( itemList );
    else
    if ( usVersion == 1 )
        return ParseListVer1 ( itemList );
    else
        return true;
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::ParseListVer0
//
// Standard Game Monitor reply
//
///////////////////////////////////////////////////////////////
bool CRemoteMasterServer::ParseListVer0 ( std::list < CServerListItem* >& itemList )
{
    CBufferReadStream stream ( m_Data, true );

    if ( stream.GetSize () < 2 )
        return false;

    unsigned short usCount = 0;
    stream.Read ( usCount );

    while ( !stream.AtEnd ( 6 ) && usCount-- )
    {
        in_addr             Address;        // IP-address
        unsigned short      usQueryPort;    // Query port

        stream.Read ( Address.S_un.S_un_b.s_b1 );
        stream.Read ( Address.S_un.S_un_b.s_b2 );
        stream.Read ( Address.S_un.S_un_b.s_b3 );
        stream.Read ( Address.S_un.S_un_b.s_b4 );
        stream.Read ( usQueryPort );

        // Add or update item
        GetServerListItem ( itemList, Address, usQueryPort );
    }

    return true;
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::ParseListVer1
//
// Extended reply
//
///////////////////////////////////////////////////////////////
bool CRemoteMasterServer::ParseListVer1 ( std::list < CServerListItem* >& itemList )
{
    CBufferReadStream stream ( m_Data, true );

    // Check EOF marker
    stream.Seek ( stream.GetSize () - 4 );
    uint uiEOFMarker = 0;
    stream.Read ( uiEOFMarker );
    if ( uiEOFMarker != 0x12345679 )
        return false;

    // Skip header
    stream.Seek ( 4 );

    // Read flags
    uint uiFlags = 0;
    stream.Read ( uiFlags );

    //bool bHasAddress        = ( uiFlags & 0x0001 ) != 0;
    //bool bHasPort           = ( uiFlags & 0x0002 ) != 0;
    bool bHasPlayerCount    = ( uiFlags & 0x0004 ) != 0;
    bool bHasMaxPlayerCount = ( uiFlags & 0x0008 ) != 0;
    bool bHasGame           = ( uiFlags & 0x0010 ) != 0;
    bool bHasName           = ( uiFlags & 0x0020 ) != 0;
    bool bHasType           = ( uiFlags & 0x0040 ) != 0;
    bool bHasMap            = ( uiFlags & 0x0080 ) != 0;
    bool bHasVersion        = ( uiFlags & 0x0100 ) != 0;
    bool bHasPassworded     = ( uiFlags & 0x0200 ) != 0;
    bool bHasSerials        = ( uiFlags & 0x0400 ) != 0;
    bool bHasPlayers        = ( uiFlags & 0x0800 ) != 0;

    // Read server count
    uint uiCount = 0;
    stream.Read ( uiCount );

    // Add all servers until we hit the count or run out of data
    while ( !stream.AtEnd ( 6 ) && uiCount-- )
    {
        in_addr             Address;        // IP-address
        unsigned short      usGamePort;     // Game port

        stream.Read ( Address.S_un.S_addr );
        stream.Read ( usGamePort );

        // Add or find item to update
        CServerListItem* pItem = GetServerListItem ( itemList, Address, usGamePort + 123 );

        if ( bHasPlayerCount )          stream.Read ( pItem->nPlayers );
        if ( bHasMaxPlayerCount )       stream.Read ( pItem->nMaxPlayers );
        if ( bHasGame )                 stream.ReadString ( pItem->strGame, true );
        if ( bHasName )                 stream.ReadString ( pItem->strName, true );
        if ( bHasType )                 stream.ReadString ( pItem->strType, true );
        if ( bHasMap )                  stream.ReadString ( pItem->strMap, true );
        if ( bHasVersion )              stream.ReadString ( pItem->strVersion, true );
        if ( bHasPassworded )           stream.Read ( pItem->bPassworded );
        if ( bHasSerials )              stream.Read ( pItem->bSerials );

        if ( bHasPlayers )
        {
            ushort usPlayerListSize = 0;
            stream.Read ( usPlayerListSize );

            for ( uint i = 0 ; i < usPlayerListSize ; i++ )
            {
                SString strPlayer;
                stream.ReadString ( strPlayer, true );
                pItem->vecPlayers.push_back ( strPlayer );
            }
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::GetServerListItem
//
// Find or add list item for the address and port
//
///////////////////////////////////////////////////////////////
CServerListItem* CRemoteMasterServer::GetServerListItem ( std::list < CServerListItem* >& itemList, in_addr Address, ushort usQueryPort )
{
    for ( std::list < CServerListItem* > ::iterator iter = itemList.begin () ; iter != itemList.end () ; ++iter )
    {
        CServerListItem* pItem = *iter;
        if ( pItem->Address.S_un.S_addr == Address.S_un.S_addr && pItem->usQueryPort == usQueryPort )
            return pItem;
    }
    CServerListItem* pItem = new CServerListItem ( Address, usQueryPort );
    itemList.push_back ( pItem );
    return pItem;
}

