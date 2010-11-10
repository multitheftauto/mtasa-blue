/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerCache.cpp
*  PURPOSE:
*  DEVELOPERS:
*
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CVersionUpdater.Util.hpp"


namespace
{
    struct CCachedKey
    {
        ulong   ulIp;
        ushort  usGamePort;
        bool operator < ( const CCachedKey& other ) const
        {
            return ulIp < other.ulIp || 
                    ( ulIp == other.ulIp &&
                        ( usGamePort < other.usGamePort ) );
        }
    };

    struct CCachedInfo
    {
        CValueInt       nPlayers;       // Current players
        CValueInt       nMaxPlayers;    // Maximum players
        CValueInt       nPing;          // Ping time
        CValueInt       bPassworded;    // Password protected
        CValueInt       uiNoReplyCount;
        SString         strName;        // Server name
        SString         strType;        // Game type
        SString         strMap;         // Map name
    };
}


///////////////////////////////////////////////////////////////
//
//
// CServerCache
//
//
///////////////////////////////////////////////////////////////
class CServerCache : public CServerCacheInterface
{
public:
    virtual bool        SaveServerCache             ( void );
    virtual void        GetServerCachedInfo         ( CServerListItem* pItem );
    virtual void        SetServerCachedInfo         ( CServerListItem* pItem );
    virtual void        GetServerListCachedInfo     ( CServerList *pList );
    virtual bool        GenerateServerList          ( CServerList *pList );

                        CServerCache                ( void );
                        ~CServerCache               ( void );
protected:
    bool                LoadServerCache             ( void );

    std::map < CCachedKey, CCachedInfo >            m_ServerCachedMap;
};



///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CServerCache* g_pServerCache = NULL;

CServerCacheInterface* GetServerCache ()
{
    if ( !g_pServerCache )
        g_pServerCache = new CServerCache ();
    return g_pServerCache;
}


///////////////////////////////////////////////////////////////
//
// CServerCache::CServerCache
//
//
//
///////////////////////////////////////////////////////////////
CServerCache::CServerCache ( void )
{
    LoadServerCache ();
}


///////////////////////////////////////////////////////////////
//
// CServerCache::~CServerCache
//
//
//
///////////////////////////////////////////////////////////////
CServerCache::~CServerCache ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CServerCache::LoadServerCache
//
// Load cache data from config
//
///////////////////////////////////////////////////////////////
bool CServerCache::LoadServerCache ( void )
{
    // Load config XML file
    CXMLFile* m_pConfigFile = CCore::GetSingleton ().GetXML ()->CreateXML ( CalcMTASAPath ( MTA_SERVER_CACHE_PATH ) );
    if ( !m_pConfigFile )
        return false;
    m_pConfigFile->Parse ();

    CXMLNode* pNode = m_pConfigFile->GetRootNode ();
    if ( !pNode )
        pNode = m_pConfigFile->CreateRootNode ( "root" );

    if ( !pNode )
        return false;

    m_ServerCachedMap.clear ();

    // Load XML data into dataSet
    CDataInfoSet dataSet;

    CXMLAccess XMLAccess ( pNode );
    XMLAccess.GetSubNodeValue ( CONFIG_NODE_SERVER_INT "11", dataSet );

    // Transfer each item from dataSet into m_ServerCachedMap
    for ( uint i = 0 ; i < dataSet.size () ; i++ )
    {
        const SDataInfoItem& item = dataSet[i];

        SString strNodeName = item.strName;
        SString strHost, strPort;
        strNodeName.Split( ":", &strHost, &strPort );
        CCachedKey key = {0,0};
        CCachedInfo info;
        if ( const SString* pString = MapFind ( item.attributeMap, "ip" ) )             key.ulIp        = inet_addr ( *pString );
        if ( const SString* pString = MapFind ( item.attributeMap, "port" ) )           key.usGamePort  = atoi ( *pString );
        if ( const SString* pString = MapFind ( item.attributeMap, "nPlayers" ) )       info.nPlayers.SetFromString ( *pString );
        if ( const SString* pString = MapFind ( item.attributeMap, "nMaxPlayers" ) )    info.nMaxPlayers.SetFromString ( *pString );
        if ( const SString* pString = MapFind ( item.attributeMap, "nPing" ) )          info.nPing.SetFromString ( *pString );
        if ( const SString* pString = MapFind ( item.attributeMap, "bPassworded" ) )    info.bPassworded.SetFromString ( *pString );
        if ( const SString* pString = MapFind ( item.attributeMap, "uiNoReplyCount" ) ) info.uiNoReplyCount.SetFromString ( *pString );
        if ( const SString* pString = MapFind ( item.attributeMap, "strName" ) )        info.strName    = *pString;
        if ( const SString* pString = MapFind ( item.attributeMap, "strType" ) )        info.strType    = *pString;
        if ( const SString* pString = MapFind ( item.attributeMap, "strMap" ) )         info.strMap     = *pString;

        if ( key.ulIp == 0 )
            continue;

        MapSet ( m_ServerCachedMap, key, info );
    }
    return true;
}


///////////////////////////////////////////////////////////////
//
// CServerCache::SaveServerCache
//
// Save cache data to config
//
///////////////////////////////////////////////////////////////
bool CServerCache::SaveServerCache ( void )
{
    CXMLFile* m_pConfigFile = CCore::GetSingleton ().GetXML ()->CreateXML ( CalcMTASAPath ( MTA_SERVER_CACHE_PATH ) );
    if ( !m_pConfigFile )
        return false;
    m_pConfigFile->Parse ();

    CXMLNode* pNode = m_pConfigFile->GetRootNode ();
    if ( !pNode )
        pNode = m_pConfigFile->CreateRootNode ( "root" );

    if ( !pNode )
        return false;

    // Start by clearing out all previous nodes
    pNode->DeleteAllSubNodes ();

    // Transfer each item from m_ServerCachedMap into dataSet
    CDataInfoSet dataSet;
    for ( std::map < CCachedKey, CCachedInfo >::iterator it = m_ServerCachedMap.begin () ; it != m_ServerCachedMap.end () ; ++it )
    {
        const CCachedKey& key = it->first;
        const CCachedInfo& info = it->second;

        // Don't save cache of non responding servers
        if ( info.nMaxPlayers == 0 || info.uiNoReplyCount > 3 )
            continue;

        SDataInfoItem item;
        item.strName = "server";
        item.strValue = inet_ntoa ( (in_addr&)key.ulIp );
        SString strIp = inet_ntoa ( (in_addr&)key.ulIp );
        MapSet ( item.attributeMap, "ip",               strIp );
        MapSet ( item.attributeMap, "port",             SString ( "%u", key.usGamePort ) );
        MapSet ( item.attributeMap, "nPlayers",         info.nPlayers.ToString () );
        MapSet ( item.attributeMap, "nMaxPlayers",      info.nMaxPlayers.ToString () );
        MapSet ( item.attributeMap, "nPing",            info.nPing.ToString () );
        MapSet ( item.attributeMap, "bPassworded",      info.bPassworded.ToString () );
        MapSet ( item.attributeMap, "uiNoReplyCount",   info.uiNoReplyCount.ToString () );
        MapSet ( item.attributeMap, "strName",          info.strName );
        MapSet ( item.attributeMap, "strType",          info.strType );
        MapSet ( item.attributeMap, "strMap",           info.strMap );
        dataSet.push_back( item );
    }

    // Save XML data from dataSet
    CXMLAccess XMLAccess ( pNode );
    XMLAccess.SetSubNodeValue ( CONFIG_NODE_SERVER_INT "11", dataSet );

    m_pConfigFile->Write ();
    return true;
}


///////////////////////////////////////////////////////////////
//
// CServerCache::GetServerCachedInfo
//
// Get cached info for a single server
//
///////////////////////////////////////////////////////////////
void CServerCache::GetServerCachedInfo ( CServerListItem* pItem )
{
    CCachedKey key;
    key.ulIp = pItem->Address.s_addr;
    key.usGamePort = pItem->usGamePort;
    if ( CCachedInfo* pInfo = MapFind ( m_ServerCachedMap, key ) )
    {
        pItem->nPlayers         = pInfo->nPlayers;
        pItem->nMaxPlayers      = pInfo->nMaxPlayers;
        pItem->nPing            = pInfo->nPing;
        pItem->bPassworded      = pInfo->bPassworded ? true : false;
        pItem->strName          = pInfo->strName;
        pItem->strType          = pInfo->strType;
        pItem->strMap           = pInfo->strMap;
        pItem->uiNoReplyCount   = pInfo->uiNoReplyCount;
    }
}


///////////////////////////////////////////////////////////////
//
// CServerCache::SetServerCachedInfo
//
// Store cached info for a single server
//
///////////////////////////////////////////////////////////////
void CServerCache::SetServerCachedInfo ( CServerListItem* pItem )
{
    CCachedKey key;
    key.ulIp = pItem->Address.s_addr;
    key.usGamePort = pItem->usGamePort;

    CCachedInfo* pInfo = MapFind ( m_ServerCachedMap, key );
    if ( !pInfo )
    {
        MapSet ( m_ServerCachedMap, key, CCachedInfo () );
        pInfo = MapFind ( m_ServerCachedMap, key );
    }
    pInfo->nPlayers         = pItem->nPlayers;
    pInfo->nMaxPlayers      = pItem->nMaxPlayers;
    pInfo->nPing            = pItem->nPing;
    pInfo->bPassworded      = pItem->bPassworded;
    pInfo->strName          = pItem->strName;
    pInfo->strType          = pItem->strType;
    pInfo->strMap           = pItem->strMap;
    pInfo->uiNoReplyCount   = pItem->uiNoReplyCount;
}


///////////////////////////////////////////////////////////////
//
// CServerCache::GetServerListCachedInfo
//
// Get cached info for each server in a server list
//
///////////////////////////////////////////////////////////////
void CServerCache::GetServerListCachedInfo ( CServerList *pList )
{
    // Get cached info for each server
    for ( CServerListIterator it = pList->IteratorBegin (); it != pList->IteratorEnd (); it++ )
        GetServerCachedInfo ( *it );

    // Remove servers not in serverlist from cache
    std::map < CCachedKey, CCachedInfo > nextServerCachedMap;
    for ( CServerListIterator it = pList->IteratorBegin (); it != pList->IteratorEnd (); it++ )
    {
        CServerListItem* pItem = *it;
        CCachedKey key;
        key.ulIp = pItem->Address.s_addr;
        key.usGamePort = pItem->usGamePort;
        if ( CCachedInfo* pInfo = MapFind ( m_ServerCachedMap, key ) )
            MapSet ( nextServerCachedMap, key, *pInfo );
    }
    m_ServerCachedMap = nextServerCachedMap;
}


///////////////////////////////////////////////////////////////
//
// CServerCache::GenerateServerList
//
// Create serverlist content from the cache
//
///////////////////////////////////////////////////////////////
bool CServerCache::GenerateServerList ( CServerList *pList )
{
    for ( std::map < CCachedKey, CCachedInfo >::iterator it = m_ServerCachedMap.begin () ; it != m_ServerCachedMap.end () ; ++it )
    {
        const CCachedKey& key = it->first;
        const CCachedInfo& info = it->second;

        // Don't add non responding servers
        if ( info.nMaxPlayers == 0 || info.uiNoReplyCount > 3 )
            continue;

        ushort usGamePort = key.usGamePort + SERVER_LIST_QUERY_PORT_OFFSET;
        if ( usGamePort > 0 )
            pList->Add ( CServerListItem ( (in_addr&)key.ulIp, usGamePort ) );
    }

    pList->SetUpdated ( true );
    return true;
}
