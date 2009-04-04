/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceManager.h
*  PURPOSE:     Resource manager class
*  DEVELOPERS:  Ed Lyons <>
*               Cecill Etheredge <>
*               Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*               Oliver Brown <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class controls all the resources that are loaded, and loads
// new resources on demand

#ifndef CRESOURCEMANAGER_H
#define CRESOURCEMANAGER_H

#include "CResource.h"
#include "CElement.h"
#include "ehs/ehs.h"
#include <list>

class CResource;

class CResourceManager 
{
public:
    enum eResourceQueue
    {
        QUEUE_STOP,
        QUEUE_STOPALL,
        QUEUE_RESTART,
    };

    struct sResourceStartFlags
    {
        bool bConfigs;
        bool bMaps;
        bool bScripts;
        bool bHTML;
        bool bClientConfigs;
        bool bClientScripts;
        bool bClientFiles;
    };

private:
    struct sResourceQueue
    {
        CResource*          pResource;
        eResourceQueue      eQueue;
        sResourceStartFlags Flags;
    };

public:
                                CResourceManager                ( void );
                                ~CResourceManager               ( void );

    CResource *                 Load                            ( const char * szResourceName );
    void                        Unload                          ( CResource * resource );
    CResource *                 GetResource                     ( const char * szResourceName );
    CResource *                 GetResource                     ( unsigned short usID );
    bool                        Exists                          ( CResource* pResource );
    void                        UnloadRemovedResources          ( void );
    void                        CheckResourceDependencies       ( void );
    void                        ListResourcesLoaded             ( void );
    list <CResource *>*         GetLoadedResources              ( void );
    bool                        Refresh                         ( void );
    void                        Upgrade                         ( void );
    inline unsigned int         GetResourceLoadedCount          ( void )            { return m_uiResourceLoadedCount; }
    inline unsigned int         GetResourceFailedCount          ( void )            { return m_uiResourceFailedCount; }
    void                        OnPlayerJoin                    ( CPlayer& Player );
    
    char *                      GetResourceDirectory            ( void );

	bool					    StartResource					( CResource* pResource, list < CResource* > * dependents = NULL, bool bStartedManually = false, bool bStartIncludedResources = true, bool bConfigs = true, bool bMaps = true, bool bScripts = true, bool bHTML = true, bool bClientConfigs = true, bool bClientScripts = true, bool bClientFiles = true );
    bool                        Reload                          ( CResource* pResource );
	bool						StopAllResources				( void );

    void                        QueueResource                   ( CResource* pResource, eResourceQueue eQueueTypebConfigs, const sResourceStartFlags* Flags );
    void                        ProcessQueue                    ( void );
    void                        RemoveFromQueue                 ( CResource* pResource );

    bool                        IsAResourceElement              ( CElement* pElement );

    unsigned short              GenerateID                      ( void );

    CResource*                  GetResourceFromLuaState         ( struct lua_State* luaVM );
    bool                        Install                         ( char * szURL, char * szName );

	CResource*					CreateResource					( char* szResourceName );
    CResource*                  CopyResource                    ( CResource* pSourceResource, const char* szNewResourceName );


private:
    char                        m_szResourceDirectory[260];
    list<CResource *>           m_resources;
    unsigned int                m_uiResourceLoadedCount;
    unsigned int                m_uiResourceFailedCount;
    bool                        m_bResourceListChanged;
    list<CResource *>           m_resourcesToStartAfterRefresh;

    list<sResourceQueue>        m_resourceQueue;
};

#endif
