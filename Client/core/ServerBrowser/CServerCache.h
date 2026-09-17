/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerCache.h
 *  PURPOSE:
 *
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CServerList;
class CServerListItem;

class CServerCacheInterface
{
public:
    virtual ~CServerCacheInterface() {}
    virtual void SaveServerCache(bool bWaitUntilFinished) = 0;
    virtual void GetServerCachedInfo(CServerListItem* pItem) = 0;
    virtual void SetServerCachedInfo(const CServerListItem* pItem) = 0;
    virtual void GetServerListCachedInfo(CServerList* pList) = 0;
    virtual bool GenerateServerList(CServerList* pList, bool bAllowNonResponding = false) = 0;
    virtual void SetServerListCachedInfo(CServerList* pList) = 0;  // Cache all servers in the list
};

CServerCacheInterface* GetServerCache();
