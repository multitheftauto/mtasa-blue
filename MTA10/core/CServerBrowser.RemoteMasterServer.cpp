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
    virtual bool            ParseList                   ( CServerListItemList& itemList );

    // CRemoteMasterServer
    void                    Init                        ( const SString& strURL );
protected:
    bool                    ParseListVer0               ( CServerListItemList& itemList );
    bool                    ParseListVer2               ( CServerListItemList& itemList );
    CServerListItem*        GetServerListItem           ( CServerListItemList& itemList, in_addr Address, ushort usGamePort );

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
bool CRemoteMasterServer::ParseList ( CServerListItemList& itemList )
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
    if ( usVersion == 2 )
        return ParseListVer2 ( itemList );
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
bool CRemoteMasterServer::ParseListVer0 ( CServerListItemList& itemList )
{
    CBufferReadStream stream ( m_Data, true );

    if ( stream.GetSize () < 2 )
        return false;

    unsigned short usCount = 0;
    stream.Read ( usCount );

#if MTA_DEBUG
    uint uiNumServers = usCount;
    uint uiNumServersUpdated = 0;
    uint uiNumServersBefore = itemList.size ();
#endif

    while ( !stream.AtEnd ( 6 ) && usCount-- )
    {
        in_addr             Address;        // IP-address
        unsigned short      usQueryPort;    // Query port

        stream.Read ( Address.S_un.S_un_b.s_b1 );
        stream.Read ( Address.S_un.S_un_b.s_b2 );
        stream.Read ( Address.S_un.S_un_b.s_b3 );
        stream.Read ( Address.S_un.S_un_b.s_b4 );
        stream.Read ( usQueryPort );

        // Add or find item to update
        CServerListItem* pItem = GetServerListItem ( itemList, Address, usQueryPort - SERVER_LIST_QUERY_PORT_OFFSET );

        if ( pItem->ShouldAllowDataQuality ( SERVER_INFO_ASE_0 ) )
        {
            pItem->SetDataQuality ( SERVER_INFO_ASE_0 );
#if MTA_DEBUG
            uiNumServersUpdated++;
#endif
        }
    }

#if MTA_DEBUG
    OutputDebugLine ( SString ( "[Browser] %d servers (%d added, %d updated) from %s", uiNumServers, itemList.size () - uiNumServersBefore, uiNumServersUpdated, *m_strURL ) );
#endif

    return true;
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::ParseListVer2
//
// Extended reply
//
///////////////////////////////////////////////////////////////
bool CRemoteMasterServer::ParseListVer2 ( CServerListItemList& itemList )
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

    //bool bHasAddress        = ( uiFlags & ASE_FLAG_ADDRESS ) != 0;
    //bool bHasPort           = ( uiFlags & ASE_FLAG_PORT ) != 0;
    bool bHasPlayerCount    = ( uiFlags & ASE_FLAG_PLAYER_COUNT ) != 0;
    bool bHasMaxPlayerCount = ( uiFlags & ASE_FLAG_MAX_PLAYER_COUNT ) != 0;
    bool bHasGameName       = ( uiFlags & ASE_FLAG_GAME_NAME ) != 0;
    bool bHasName           = ( uiFlags & ASE_FLAG_NAME ) != 0;
    bool bHasGameMode       = ( uiFlags & ASE_FLAG_GAME_MODE ) != 0;
    bool bHasMap            = ( uiFlags & ASE_FLAG_MAP ) != 0;
    bool bHasVersion        = ( uiFlags & ASE_FLAG_VERSION ) != 0;
    bool bHasPassworded     = ( uiFlags & ASE_FLAG_PASSWORDED ) != 0;
    bool bHasSerials        = ( uiFlags & ASE_FLAG_SERIALS ) != 0;
    bool bHasPlayers        = ( uiFlags & ASE_FLAG_PLAYER_LIST ) != 0;
    bool bHasRespondingFlag = ( uiFlags & ASE_FLAG_RESPONDING ) != 0;
    bool bHasRestrictionFlags = ( uiFlags & ASE_FLAG_RESTRICTIONS ) != 0;
    bool bHasSearchIgnoreSections = ( uiFlags & ASE_FLAG_SEARCH_IGNORE_SECTIONS ) != 0;
    bool bHasKeepFlag       = ( uiFlags & ASE_FLAG_KEEP ) != 0;

    // Rate quality of data supplied here
    uint uiDataQuality = SERVER_INFO_ASE_2;
    if ( bHasSearchIgnoreSections )
        uiDataQuality = SERVER_INFO_ASE_2b;

    // Read sequence number
    uint uiSequenceNumber = 0;
    stream.Read ( uiSequenceNumber );

    // Read server count
    uint uiCount = 0;
    stream.Read ( uiCount );

#if MTA_DEBUG
    struct SItem
    {
        int iTotal;
        int iNoResponse;
    };
    std::map < SString, SItem > totalMap;

    uint uiNumServers = uiCount;
    uint uiNumServersUpdated = 0;
    uint uiNumServersBefore = itemList.size ();
#endif

    // Add all servers until we hit the count or run out of data
    while ( !stream.AtEnd ( 6 ) && uiCount-- )
    {
        ushort usLength = 0;        // Length of data for this server
        stream.Read ( usLength );
        uint uiSkipPos = stream.Tell () + usLength - 2;

        in_addr             Address;        // IP-address
        unsigned short      usGamePort;     // Game port

        stream.Read ( Address.S_un.S_addr );
        stream.Read ( usGamePort );

        // Add or find item to update
        CServerListItem* pItem = GetServerListItem ( itemList, Address, usGamePort );

        if ( pItem->ShouldAllowDataQuality ( uiDataQuality ) )
        {
            pItem->SetDataQuality ( uiDataQuality );

            if ( bHasPlayerCount )          stream.Read ( pItem->nPlayers );
            if ( bHasMaxPlayerCount )       stream.Read ( pItem->nMaxPlayers );
            if ( bHasGameName )             stream.ReadString ( pItem->strGameName, true );
            if ( bHasName )                 stream.ReadString ( pItem->strName, true );
            if ( bHasGameMode )             stream.ReadString ( pItem->strGameMode, true );
            if ( bHasMap )                  stream.ReadString ( pItem->strMap, true );
            if ( bHasVersion )              stream.ReadString ( pItem->strVersion, true );
            if ( bHasPassworded )           stream.Read ( pItem->bPassworded );
            if ( bHasSerials )              stream.Read ( pItem->bSerials );

            if ( bHasPlayers )
            {
                pItem->vecPlayers.clear ();

                ushort usPlayerListSize = 0;
                stream.Read ( usPlayerListSize );

                for ( uint i = 0 ; i < usPlayerListSize ; i++ )
                {
                    SString strPlayer;
                    stream.ReadString ( strPlayer, true );
                    pItem->vecPlayers.push_back ( strPlayer );
                }
            }

            if ( bHasRespondingFlag )
            {
                stream.Read ( pItem->bMasterServerSaysNoResponse );
            }

            if ( bHasRestrictionFlags )
            {
                stream.Read ( pItem->uiMasterServerSaysRestrictions );
            }

            if ( bHasSearchIgnoreSections )
            {
                // Construct searchable name
                pItem->strSearchableName = pItem->strName;
                uchar ucNumItems = 0;
                stream.Read ( ucNumItems );
                while ( ucNumItems-- )
                {
                    // Read section of name to remove
                    uchar ucOffset = 0;
                    uchar ucLength = 0;
                    stream.Read ( ucOffset );
                    stream.Read ( ucLength );
                    for ( uint i = ucOffset ; i < (uint)( ucOffset + ucLength ) ; i++ )
                        if ( i < pItem->strSearchableName.length () )
                            pItem->strSearchableName[i] = '\1';
                }
            }

            if ( bHasKeepFlag )
            {
                uchar ucKeepFlag = 0;
                stream.Read ( ucKeepFlag );
                pItem->bKeepFlag = ucKeepFlag ? true : false;
            }

            pItem->PostChange ();

#if MTA_DEBUG
            uiNumServersUpdated++;
            totalMap[ pItem->strVersion ].iTotal += 1;
            if ( pItem->bMasterServerSaysNoResponse )
                totalMap[ pItem->strVersion ].iNoResponse += 1;
#endif
        }

        // Make sure past all data for this server
        stream.Seek ( uiSkipPos );
    }

#if MTA_DEBUG
    OutputDebugLine ( SString ( "[Browser] %d servers (%d added, %d updated) from %s", uiNumServers, itemList.size () - uiNumServersBefore, uiNumServersUpdated, *m_strURL ) );
    for ( std::map < SString, SItem >::iterator iter = totalMap.begin () ; iter != totalMap.end () ; ++iter )
        OutputDebugLine ( SString ( "[Browser] version '%s' - %d total  %d noresponse", *iter->first, iter->second.iTotal, iter->second.iNoResponse ) );
#endif

    return true;
}


///////////////////////////////////////////////////////////////
//
// CRemoteMasterServer::GetServerListItem
//
// Find or add list item for the address and port
//
///////////////////////////////////////////////////////////////
CServerListItem* CRemoteMasterServer::GetServerListItem ( CServerListItemList& itemList, in_addr Address, ushort usGamePort )
{
    CServerListItem* pItem = itemList.Find ( Address, usGamePort );
    if ( pItem )
        return pItem;

    return itemList.Add ( Address, usGamePort );
}

