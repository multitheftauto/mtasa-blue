/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerCache.h
*  PURPOSE:
*  DEVELOPERS:
*
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


class CServerCacheInterface
{
public:
    virtual             ~CServerCacheInterface      ( void ) {}
    virtual bool        SaveServerCache             ( void ) = 0;
    virtual void        GetServerCachedInfo         ( CServerListItem* pItem ) = 0;
    virtual void        SetServerCachedInfo         ( const CServerListItem* pItem ) = 0;
    virtual void        GetServerListCachedInfo     ( CServerList *pList ) = 0;
    virtual bool        GenerateServerList          ( CServerList *pList ) = 0;
};


CServerCacheInterface* GetServerCache ();