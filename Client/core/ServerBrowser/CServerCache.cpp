/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerCache.cpp
 *  PURPOSE:
 *
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVersionUpdater.Util.hpp"
#include "CServerCache.h"

namespace
{
    struct CCachedKey
    {
        ulong  ulIp;
        ushort usGamePort;
        bool   operator<(const CCachedKey& other) const { return ulIp < other.ulIp || (ulIp == other.ulIp && (usGamePort < other.usGamePort)); }
    };

    struct CCachedInfo
    {
        CValueInt nPlayers;               // Current players
        CValueInt nMaxPlayers;            // Maximum players
        CValueInt nPing;                  // Ping time
        CValueInt bPassworded;            // Password protected
        CValueInt bKeepFlag;
        CValueInt uiCacheNoReplyCount;
        CValueInt usHttpPort;
        CValueInt ucSpecialFlags;
        SString   strName;                // Server name
        SString   strGameMode;            // Game mode
        SString   strMap;                 // Map name
        SString   strVersion;             // Version
    };

    // Variables used for saving the server cache file on a separate thread
    static bool                              ms_bIsSaving = false;
    static std::map<CCachedKey, CCachedInfo> ms_ServerCachedMap;
}            // namespace

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
    ZERO_ON_NEW
    virtual void SaveServerCache(bool bWaitUntilFinished);
    virtual void GetServerCachedInfo(CServerListItem* pItem);
    virtual void SetServerCachedInfo(const CServerListItem* pItem);
    virtual void GetServerListCachedInfo(CServerList* pList);
    virtual bool GenerateServerList(CServerList* pList);

    CServerCache();
    ~CServerCache();

protected:
    bool         LoadServerCache();
    static DWORD StaticThreadProc(LPVOID lpdwThreadParam);
    static void  StaticSaveServerCache();

    bool                              m_bListChanged;
    std::map<CCachedKey, CCachedInfo> m_ServerCachedMap;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CServerCache* g_pServerCache = NULL;

CServerCacheInterface* GetServerCache()
{
    if (!g_pServerCache)
        g_pServerCache = new CServerCache();
    return g_pServerCache;
}

///////////////////////////////////////////////////////////////
//
// CServerCache::CServerCache
//
//
//
///////////////////////////////////////////////////////////////
CServerCache::CServerCache()
{
    LoadServerCache();
}

///////////////////////////////////////////////////////////////
//
// CServerCache::~CServerCache
//
//
//
///////////////////////////////////////////////////////////////
CServerCache::~CServerCache()
{
}

///////////////////////////////////////////////////////////////
//
// CServerCache::LoadServerCache
//
// Load cache data from config
//
///////////////////////////////////////////////////////////////
bool CServerCache::LoadServerCache()
{
    // Load config XML file
    CXMLFile* m_pConfigFile = CCore::GetSingleton().GetXML()->CreateXML(CalcMTASAPath(MTA_SERVER_CACHE_PATH));
    if (!m_pConfigFile)
        return false;
    m_pConfigFile->Parse();

    CXMLNode* pNode = m_pConfigFile->GetRootNode();
    if (!pNode)
        pNode = m_pConfigFile->CreateRootNode("root");

    if (!pNode)
        return false;

    m_ServerCachedMap.clear();

    // Load XML data into dataSet
    CDataInfoSet dataSet;

    CXMLAccess XMLAccess(pNode);
    XMLAccess.GetSubNodeValue(CONFIG_NODE_SERVER_INT "11", dataSet);

    // Transfer each item from dataSet into m_ServerCachedMap
    for (uint i = 0; i < dataSet.size(); i++)
    {
        const SDataInfoItem& item = dataSet[i];

        SString strNodeName = item.strName;
        SString strHost, strPort;
        strNodeName.Split(":", &strHost, &strPort);
        CCachedKey  key = {0, 0};
        CCachedInfo info;
        if (const SString* pString = MapFind(item.attributeMap, "ip"))
            key.ulIp = inet_addr(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "port"))
            key.usGamePort = atoi(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "nPlayers"))
            info.nPlayers.SetFromString(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "nMaxPlayers"))
            info.nMaxPlayers.SetFromString(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "nPing"))
            info.nPing.SetFromString(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "bPassworded"))
            info.bPassworded.SetFromString(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "bKeepFlag"))
            info.bKeepFlag.SetFromString(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "uiNoReplyCount"))
            info.uiCacheNoReplyCount.SetFromString(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "httpPort"))
            info.usHttpPort.SetFromString(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "flags"))
            info.ucSpecialFlags.SetFromString(*pString);
        if (const SString* pString = MapFind(item.attributeMap, "strName"))
            info.strName = *pString;
        if (const SString* pString = MapFind(item.attributeMap, "strGameMode"))
            info.strGameMode = *pString;
        if (const SString* pString = MapFind(item.attributeMap, "strMap"))
            info.strMap = *pString;
        if (const SString* pString = MapFind(item.attributeMap, "strVersion"))
            info.strVersion = *pString;

        if (key.ulIp == 0)
            continue;

        MapSet(m_ServerCachedMap, key, info);
    }

    delete pNode;
    delete m_pConfigFile;

    return true;
}

///////////////////////////////////////////////////////////////
//
// CServerCache::SaveServerCache
//
// Save cache data to config
//
///////////////////////////////////////////////////////////////
void CServerCache::SaveServerCache(bool bWaitUntilFinished)
{
    // Check if we need to save
    if (m_bListChanged && !ms_bIsSaving)
    {
        m_bListChanged = false;

        // Copy vars for save thread
        ms_ServerCachedMap = m_ServerCachedMap;

        // Start save thread
        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CServerCache::StaticThreadProc, NULL, CREATE_SUSPENDED, NULL);
        if (!hThread)
        {
            CCore::GetSingleton().GetConsole()->Printf("Could not create server cache thread.");
        }
        else
        {
            ms_bIsSaving = true;
            SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
            ResumeThread(hThread);
        }
    }

    // If required, wait until save thread is done
    while (bWaitUntilFinished && ms_bIsSaving)
    {
        Sleep(1);
    }
}

///////////////////////////////////////////////////////////////
//
// CServerCache::StaticThreadProc
//
// SaveServerCache thread
//
///////////////////////////////////////////////////////////////
DWORD CServerCache::StaticThreadProc(LPVOID lpdwThreadParam)
{
    StaticSaveServerCache();
    ms_bIsSaving = false;
    return 0;
}

///////////////////////////////////////////////////////////////
//
// CServerCache::StaticSaveServerCache
//
//
//
///////////////////////////////////////////////////////////////
void CServerCache::StaticSaveServerCache()
{
    CXMLFile* m_pConfigFile = CCore::GetSingleton().GetXML()->CreateXML(CalcMTASAPath(MTA_SERVER_CACHE_PATH));
    if (!m_pConfigFile)
        return;
    m_pConfigFile->Parse();

    CXMLNode* pNode = m_pConfigFile->GetRootNode();
    if (!pNode)
        pNode = m_pConfigFile->CreateRootNode("root");

    if (!pNode)
        return;

    // Start by clearing out all previous nodes
    pNode->DeleteAllSubNodes();

    // Transfer each item from m_ServerCachedMap into dataSet
    CDataInfoSet dataSet;
    for (std::map<CCachedKey, CCachedInfo>::iterator it = ms_ServerCachedMap.begin(); it != ms_ServerCachedMap.end(); ++it)
    {
        const CCachedKey&  key = it->first;
        const CCachedInfo& info = it->second;

        // Don't save cache of non responding servers
        if (info.nMaxPlayers == 0 || info.uiCacheNoReplyCount > 3)
            continue;

        SDataInfoItem item;
        item.strName = "server";
        item.strValue = inet_ntoa((in_addr&)key.ulIp);
        SString strIp = inet_ntoa((in_addr&)key.ulIp);
        MapSet(item.attributeMap, "ip", strIp);
        MapSet(item.attributeMap, "port", SString("%u", key.usGamePort));
        MapSet(item.attributeMap, "nPlayers", info.nPlayers.ToString());
        MapSet(item.attributeMap, "nMaxPlayers", info.nMaxPlayers.ToString());
        MapSet(item.attributeMap, "nPing", info.nPing.ToString());
        MapSet(item.attributeMap, "bPassworded", info.bPassworded.ToString());
        MapSet(item.attributeMap, "bKeepFlag", info.bKeepFlag.ToString());
        MapSet(item.attributeMap, "uiNoReplyCount", info.uiCacheNoReplyCount.ToString());
        MapSet(item.attributeMap, "httpPort", info.usHttpPort.ToString());
        MapSet(item.attributeMap, "flags", info.ucSpecialFlags.ToString());
        MapSet(item.attributeMap, "strName", info.strName);
        MapSet(item.attributeMap, "strGameMode", info.strGameMode);
        MapSet(item.attributeMap, "strMap", info.strMap);
        MapSet(item.attributeMap, "strVersion", info.strVersion);
        dataSet.push_back(item);
    }

    // Save XML data from dataSet
    CXMLAccess XMLAccess(pNode);
    XMLAccess.SetSubNodeValue(CONFIG_NODE_SERVER_INT "11", dataSet);

    m_pConfigFile->Write();

    delete pNode;
    delete m_pConfigFile;
}

///////////////////////////////////////////////////////////////
//
// CServerCache::GetServerCachedInfo
//
// Get cached info for a single server
//
///////////////////////////////////////////////////////////////
void CServerCache::GetServerCachedInfo(CServerListItem* pItem)
{
    CCachedKey key;
    key.ulIp = pItem->Address.s_addr;
    key.usGamePort = pItem->usGamePort;
    if (CCachedInfo* pInfo = MapFind(m_ServerCachedMap, key))
    {
        if (pItem->ShouldAllowDataQuality(SERVER_INFO_CACHE))
        {
            pItem->SetDataQuality(SERVER_INFO_CACHE);

            pItem->nPlayers = pInfo->nPlayers;
            pItem->nMaxPlayers = pInfo->nMaxPlayers;
            pItem->nPing = pInfo->nPing;
            pItem->bPassworded = (pInfo->bPassworded != 0);
            pItem->bKeepFlag = (pInfo->bKeepFlag != 0);
            pItem->strName = pInfo->strName;
            pItem->strGameMode = pInfo->strGameMode;
            pItem->strMap = pInfo->strMap;
            pItem->strVersion = pInfo->strVersion;
            pItem->uiCacheNoReplyCount = pInfo->uiCacheNoReplyCount;
            pItem->m_usHttpPort = pInfo->usHttpPort;
            pItem->m_ucSpecialFlags = pInfo->ucSpecialFlags;
            pItem->PostChange();
        }
        else if (pItem->GetDataQuality() < SERVER_INFO_QUERY)
        {
            // Allow cache to fill in certain missing data if query not done yet
            if (pItem->strGameMode.empty())
                pItem->strGameMode = pInfo->strGameMode;
            if (pItem->strMap.empty())
                pItem->strMap = pInfo->strMap;
            if (pItem->strVersion.empty())
                pItem->strVersion = pInfo->strVersion;
            if (pItem->nPing == 9999)
                pItem->nPing = pInfo->nPing;
            if (pItem->m_usHttpPort == 0)
                pItem->m_usHttpPort = pInfo->usHttpPort;
            if (pItem->m_ucSpecialFlags == 0)
                pItem->m_ucSpecialFlags = pInfo->ucSpecialFlags;
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CServerCache::SetServerCachedInfo
//
// Store cached info for a single server
//
///////////////////////////////////////////////////////////////
void CServerCache::SetServerCachedInfo(const CServerListItem* pItem)
{
    CCachedKey key;
    key.ulIp = pItem->Address.s_addr;
    key.usGamePort = pItem->usGamePort;

    CCachedInfo* pInfo = MapFind(m_ServerCachedMap, key);
    if (!pInfo)
    {
        MapSet(m_ServerCachedMap, key, CCachedInfo());
        pInfo = MapFind(m_ServerCachedMap, key);
    }

    // Check if changed
    if (pInfo->nPlayers == pItem->nPlayers && pInfo->nMaxPlayers == pItem->nMaxPlayers && pInfo->nPing == pItem->nPing &&
        (pInfo->bPassworded != 0) == pItem->bPassworded && (pInfo->bKeepFlag != 0) == pItem->bKeepFlag && pInfo->strName == pItem->strName &&
        pInfo->strGameMode == pItem->strGameMode
        //&& pInfo->strMap               == pItem->strMap
        && pInfo->strVersion == pItem->strVersion && pInfo->uiCacheNoReplyCount == pItem->uiCacheNoReplyCount && pInfo->usHttpPort == pItem->m_usHttpPort &&
        pInfo->ucSpecialFlags == pItem->m_ucSpecialFlags)
    {
        return;
    }

    m_bListChanged = true;
    pInfo->nPlayers = pItem->nPlayers;
    pInfo->nMaxPlayers = pItem->nMaxPlayers;
    pInfo->nPing = pItem->nPing;
    pInfo->bPassworded = pItem->bPassworded;
    pInfo->bKeepFlag = pItem->bKeepFlag;
    pInfo->strName = pItem->strName;
    pInfo->strGameMode = pItem->strGameMode;
    pInfo->strMap = pItem->strMap;
    pInfo->strVersion = pItem->strVersion;
    pInfo->uiCacheNoReplyCount = pItem->uiCacheNoReplyCount;
    pInfo->usHttpPort = pItem->m_usHttpPort;
    pInfo->ucSpecialFlags = pItem->m_ucSpecialFlags;
}

///////////////////////////////////////////////////////////////
//
// CServerCache::GetServerListCachedInfo
//
// Get cached info for each server in a server list
//
///////////////////////////////////////////////////////////////
void CServerCache::GetServerListCachedInfo(CServerList* pList)
{
    // Get cached info for each server
    for (CServerListIterator it = pList->IteratorBegin(); it != pList->IteratorEnd(); it++)
        GetServerCachedInfo(*it);

    // Remove servers not in serverlist from cache
    std::map<CCachedKey, CCachedInfo> nextServerCachedMap;
    for (CServerListIterator it = pList->IteratorBegin(); it != pList->IteratorEnd(); it++)
    {
        CServerListItem* pItem = *it;
        CCachedKey       key;
        key.ulIp = pItem->Address.s_addr;
        key.usGamePort = pItem->usGamePort;
        if (CCachedInfo* pInfo = MapFind(m_ServerCachedMap, key))
            MapSet(nextServerCachedMap, key, *pInfo);
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
bool CServerCache::GenerateServerList(CServerList* pList)
{
    for (std::map<CCachedKey, CCachedInfo>::iterator it = m_ServerCachedMap.begin(); it != m_ServerCachedMap.end(); ++it)
    {
        const CCachedKey&  key = it->first;
        const CCachedInfo& info = it->second;

        // Don't add non responding servers
        if (info.nMaxPlayers == 0 || info.uiCacheNoReplyCount > 3)
            continue;

        ushort usGamePort = key.usGamePort;
        if (usGamePort > 0)
        {
            pList->AddUnique((in_addr&)key.ulIp, usGamePort);
        }
    }

    pList->SetUpdated(true);
    return true;
}
