/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceManager.cpp
*  PURPOSE:     Resource manager class
*  DEVELOPERS:  Ed Lyons <>
*               Cecill Etheredge <>
*               Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*               Oliver Brown <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class controls all the resources that are loaded, and loads
// new resources on demand

#include "StdInc.h"

extern CServerInterface* g_pServerInterface;

static bool s_bNotFirstTime = false; // if true, outputs "new resource loaded" messages, doesn't do it
                                     // first time to prevent console spam on startup

// SResInfo - Item in list of potential resources - Used in Refresh()
struct SResInfo
{
    SString strAbsPath;
    SString strName;
    bool bIsDir;
    bool bPathIssue;
    SString strAbsPathDup;
};


CResourceManager::CResourceManager ( void )
{
    m_bResourceListChanged = false;
    m_uiResourceLoadedCount = 0;
    m_uiResourceFailedCount = 0;

    m_strResourceDirectory.Format ( "%s/resources", g_pServerInterface->GetServerModPath () );
}

CResourceManager::~CResourceManager ( void )
{
    // First process the queue to make sure all queued up tasks are done
    ProcessQueue ();

    // Then kill all the running resources
    StopAllResources ();

    // Then start deleting them
    while ( m_resources.size () )
    {    
        CResource* pResource = m_resources.front ();
        delete pResource;
        m_resources.remove ( pResource );
    }
}


// Load the complete list of resources and create their objects
// DOES NOT reload already loaded resources, we need a special function for lua for that (e.g. reloadResource)
// Talidan: yes it did, noob.  Not as of r897 - use bRefreshAll to do already loaded resources.
bool CResourceManager::Refresh ( bool bRefreshAll )
{
    UnloadRemovedResources();

    // Make list of potential active resources
    std::map < SString, SResInfo > resInfoMap;

    // Initial search dir
    std::vector < SString > resourcesPathList;
    resourcesPathList.push_back ( "resources" );

    SString strModPath = g_pServerInterface->GetModManager ()->GetModPath ();
    for ( uint i = 0 ; i < resourcesPathList.size () ; i++ )
    {
        // Enumerate all files and directories
        SString strResourcesRelPath = resourcesPathList[i];
        SString strResourcesAbsPath = PathJoin ( strModPath, strResourcesRelPath, "/" );
        std::vector < SString > itemList = FindFiles ( strResourcesAbsPath, true, true );

        // Check each item
        for ( uint i = 0 ; i < itemList.size () ; i++ )
        {
            SString strName = itemList[i];

            // Ignore items that start with a dot
            if ( strName[0] == '.' )
                continue;

            bool bIsDir = DirectoryExists ( PathJoin ( strResourcesAbsPath, strName ) );

            // Recurse into [directories]
            if ( bIsDir && ( strName.BeginsWith( "#" ) || ( strName.BeginsWith( "[" ) && strName.EndsWith( "]" ) ) ) )
            {
                resourcesPathList.push_back ( PathJoin ( strResourcesRelPath, strName ) );
                continue;
            }

            // Extract file extention
            SString strExt;
            if ( !bIsDir )
                ExtractExtention ( strName, &strName, &strExt );

            // Ignore files that are not .zip
            if ( !bIsDir && strExt != "zip" )
                continue;

            // Ignore items that have dot or space in the name
            if ( strName.Contains ( "." ) || strName.Contains ( " " ) )
            {
                CLogger::LogPrintf ( "WARNING: Not loading resource '%s' as it contains illegal characters\n", *strName );
                continue;
            }

            // Ignore dir items with no meta.xml (assume it's the result of saved files from a zipped resource)
            if ( bIsDir && !FileExists ( PathJoin ( strResourcesAbsPath, strName, "meta.xml" ) ) )
                continue;

            // Add potential resource to list
            SResInfo newInfo;
            newInfo.strAbsPath = strResourcesAbsPath;
            newInfo.strName = strName;
            newInfo.bIsDir = bIsDir;
            newInfo.bPathIssue = false;

            // Check for duplicate
            if ( SResInfo* pDup = MapFind ( resInfoMap, strName ) )
            {
                // Is path the same?
                if ( newInfo.strAbsPath == pDup->strAbsPath )
                {
                    if ( newInfo.bIsDir )
                    {
                        // If non-zipped item, replace already existing zipped item on the same path
                        assert ( !pDup->bIsDir );
                        *pDup = newInfo;
                    }
                }
                else
                {
                    // Don't load resource if there are duplicates on different paths
                    pDup->bPathIssue = true;
                    pDup->strAbsPathDup = newInfo.strAbsPath;
                }
            }
            else
            {
                // No duplicate found
                MapSet ( resInfoMap, strName, newInfo );
            }
        }
    }

    // Process potential resource list
    for ( std::map < SString, SResInfo >::const_iterator iter = resInfoMap.begin () ; iter != resInfoMap.end () ; ++iter )
    {
        const SResInfo& info = iter->second;
        if ( !info.bPathIssue )
        {
            CResource* pResource = GetResource ( info.strName );

            if ( bRefreshAll || !pResource || !pResource->CheckIfStartable() )
            {
                // Add the resource
                Load ( !info.bIsDir, info.strAbsPath, info.strName );
            }
        }
    }


    CheckResourceDependencies();

    // Print important errors
    for ( std::map < SString, SResInfo >::const_iterator iter = resInfoMap.begin () ; iter != resInfoMap.end () ; ++iter )
    {
        const SResInfo& info = iter->second;
        if ( info.bPathIssue )
        {
            CLogger::ErrorPrintf ( "Not processing resource '%s' as it has duplicates on different paths:\n", *info.strName );
            CLogger::LogPrintfNoStamp ( "                  Path #1: \"%s\"\n", *PathJoin ( PathMakeRelative ( strModPath, info.strAbsPath ), info.strName ) );
            CLogger::LogPrintfNoStamp ( "                  Path #2: \"%s\"\n", *PathJoin ( PathMakeRelative ( strModPath, info.strAbsPathDup ), info.strName ) );
        }
        else
        {
            CResource* pResource = GetResource ( info.strName );
            if ( pResource && !pResource->CheckIfStartable() )
            {
                CLogger::ErrorPrintf ( "Problem with resource: %s; %s\n", *info.strName, pResource->GetFailureReason ().c_str () ); 
            }
        }
    }


    if ( m_bResourceListChanged )
    {
        m_bResourceListChanged = false;
        CLogger::LogPrintf ( "Resources: %d loaded, %d failed\n", m_uiResourceLoadedCount, m_uiResourceFailedCount );
    }

    list < CResource* > ::iterator iter = m_resourcesToStartAfterRefresh.begin ();
    for ( ; iter != m_resourcesToStartAfterRefresh.end (); iter++ )
    {
        (*iter)->Start();
    }
    m_resourcesToStartAfterRefresh.clear();

    s_bNotFirstTime = true;

    return true;
}

void CResourceManager::UpgradeAll ( void )
{
    // Modify source files if needed
    for ( list < CResource* > ::const_iterator iter = m_resources.begin () ; iter != m_resources.end (); iter++ )
        (*iter)->ApplyUpgradeModifications ();
}

void CResourceManager::CheckAll ( void )
{
    // Check all resources for deprecated functions and MTA version issues
    for ( list < CResource* > ::const_iterator iter = m_resources.begin () ; iter != m_resources.end (); iter++ )
        (*iter)->LogUpgradeWarnings ();
}

const char* CResourceManager::GetResourceDirectory ( void )
{
    return m_strResourceDirectory;
}

// first, go through each resource then link up to other resources, any that fail are noted
// then go through each resource and perform a recursive check
void CResourceManager::CheckResourceDependencies ( void )
{
    m_uiResourceLoadedCount = 0;
    m_uiResourceFailedCount = 0;
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        (*iter)->LinkToIncludedResources();
    }

    iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( (*iter)->CheckIfStartable() )
            m_uiResourceLoadedCount++;
        else
            m_uiResourceFailedCount++;
    }
}

void CResourceManager::ListResourcesLoaded ( void )
{
    unsigned int uiCount = 0;
    unsigned int uiFailedCount = 0;
    unsigned int uiRunningCount = 0;
    CLogger::LogPrintf ( "== Resource list ==\n" );
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        CResource * res = (*iter);
        if ( res->IsLoaded() )
        {
            if ( res->IsActive() )
            {
                CLogger::LogPrintf ( "%-20.20s   RUNNING   (%d dependents)\n", res->GetName().c_str(), res->GetDependentCount() );
                uiRunningCount++;
            }
            else
                CLogger::LogPrintf ( "%-20.20s   STOPPED   (%d files)\n", res->GetName().c_str(), res->GetFileCount() );
            uiCount ++;
        }
        else
        {
            CLogger::LogPrintf ( "%-20.20s   FAILED    (see info command for reason)\n", res->GetName().c_str () );
            uiFailedCount ++;
        }
    }
    CLogger::LogPrintf ( "Resources: %d loaded, %d failed, %d running\n", uiCount, uiFailedCount, uiRunningCount );
}

// check all loaded resources and see if they're still valid (i.e. have a meta.xml file)
void CResourceManager::UnloadRemovedResources ( void )
{
    list < CResource* > resourcesToDelete;
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    string strPath;
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( ! (*iter)->GetFilePath ( "meta.xml", strPath ) )
        {
            if ( (*iter)->IsActive() )
                CLogger::ErrorPrintf ( "Resource '%s' has been removed while running! Stopping resource.\n", (*iter)->GetName().c_str () );
            else
                CLogger::LogPrintf ( "Resource '%s' has been removed, unloading\n", (*iter)->GetName().c_str () );
            resourcesToDelete.push_back ( (*iter) );
            m_bResourceListChanged = true;
        }
    }

    iter = resourcesToDelete.begin ();
    for ( ; iter != resourcesToDelete.end (); iter++ )
    {
        Unload ( *iter );
    }
}

void CResourceManager::Unload ( CResource * resource )
{
    RemoveResourceFromLists ( resource );
    m_resourcesToStartAfterRefresh.remove ( resource );
    RemoveFromQueue ( resource );
    delete resource;
}

CResource * CResourceManager::Load ( bool bIsZipped, const char * szAbsPath, const char * szResourceName )
{
    CResource * loadedResource = NULL;
    bool bStartAfterLoading = false;

    // check to see if we've already loaded this resource - we can only
    // load each resource once

    list < CResource* > resourcesToDelete;
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( stricmp ( (*iter)->GetName().c_str (), szResourceName ) == 0 )
        {
            if ( (*iter)->HasResourceChanged() )
            {
                if ( (*iter)->IsActive() )
                {
                    CLogger::LogPrintf ( "Resource '%s' changed while running, reloading and restarting\n", szResourceName );
                    bStartAfterLoading = true;
                }
                else
                {
                    CLogger::LogPrintf ( "Resource '%s' changed, reloading\n", szResourceName );
                }

                resourcesToDelete.push_back ( (*iter) );
            }
            else
                return (*iter);
        }
    }

    iter = resourcesToDelete.begin ();
    for ( ; iter != resourcesToDelete.end (); iter++ )
    {
        // Stop it first. This fixes bug #3729 because it isn't removed from the list before it's stopped.
        // Removing it from the resources list first caused the resource pointer to be unverifyable, and
        // the pointer wouldn't work in resource LUA functions.
        (*iter)->Stop ( true );

        RemoveResourceFromLists ( *iter );
        m_resourcesToStartAfterRefresh.remove ( *iter );
        RemoveFromQueue ( *iter );
        delete *iter;

        m_bResourceListChanged = true;
    }

    loadedResource = new CResource ( this, bIsZipped, szAbsPath, szResourceName );
    if ( !loadedResource->IsLoaded() )
    {
        CLogger::LogPrintf("Loading of resource '%s' failed\n", szResourceName );
        m_resourcesToStartAfterRefresh.remove ( loadedResource );
        RemoveFromQueue ( loadedResource );
        delete loadedResource;
        loadedResource = NULL;
    }
    else
    {
        if ( bStartAfterLoading )
            m_resourcesToStartAfterRefresh.push_back ( loadedResource );
        if ( s_bNotFirstTime )
            CLogger::LogPrintf("New resource '%s' loaded\n", loadedResource->GetName().c_str () );
        unsigned short usID = GenerateID ();
        loadedResource->SetNetID ( usID );
        AddResourceToLists ( loadedResource );
        m_bResourceListChanged = true;
    }

    return loadedResource;
}

CResource * CResourceManager::GetResource ( const char * szResourceName )
{
    CResource** ppResource = MapFind ( m_NameResourceMap, SStringX ( szResourceName ).ToUpper () );
    if ( ppResource )
        return *ppResource;
    return NULL;
}


CResource* CResourceManager::GetResourceFromScriptID ( uint uiScriptID )
{
    CResource* pResource = (CResource*) CIdArray::FindEntry ( uiScriptID, EIdClass::RESOURCE );
    dassert ( !pResource || ListContains ( m_resources, pResource ) );
    return pResource;
}


unsigned short CResourceManager::GenerateID ( void )
{
    // Create a map of all used IDs
    map < unsigned short, bool > idMap;
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        idMap[ ( *iter )->GetNetID () ] = true;
    }

    // Find an unused ID
    for ( unsigned short i = 0 ; i < 0xFFFE ; i++ )
    {
        if ( idMap.find ( i ) == idMap.end () )
            return i;
    }
    return 0xFFFF;
}


void CResourceManager::OnPlayerJoin ( CPlayer& Player )
{
    // Loop through our started resources so they start in the correct order clientside
    list < CResource* > ::iterator iter = CResource::m_StartedResources.begin ();
    for ( ; iter != CResource::m_StartedResources.end (); iter++ )
    {
        ( *iter )->OnPlayerJoin ( Player );
    }
}

//
// Add resource <-> luaVM lookup mapping
//
void CResourceManager::NotifyResourceVMOpen ( CResource* pResource, CLuaMain* pVM )
{
    lua_State* luaVM = pVM->GetVirtualMachine ();
    assert ( luaVM );
    assert ( !MapContains ( m_ResourceLuaStateMap, pResource ) );
    assert ( !MapContains ( m_LuaStateResourceMap, luaVM ) );
    MapSet ( m_ResourceLuaStateMap, pResource, luaVM );
    MapSet ( m_LuaStateResourceMap, luaVM, pResource );
}

//
// Remove resource <-> luaVM lookup mapping
//
void CResourceManager::NotifyResourceVMClose ( CResource* pResource, CLuaMain* pVM )
{
    lua_State* luaVM = pVM->GetVirtualMachine ();
    assert ( luaVM );
    assert ( MapContains ( m_ResourceLuaStateMap, pResource ) );
    assert ( MapContains ( m_LuaStateResourceMap, luaVM ) );
    MapRemove ( m_ResourceLuaStateMap, pResource );
    MapRemove ( m_LuaStateResourceMap, luaVM );
}

//
// Add resource to the internal lists
//
void CResourceManager::AddResourceToLists ( CResource* pResource )
{
    SString strResourceNameKey = SString ( pResource->GetName () ).ToUpper ();

    assert ( !m_resources.Contains ( pResource ) );
    assert ( !MapContains ( m_NameResourceMap, strResourceNameKey ) );
    assert ( !MapContains ( m_ResourceLuaStateMap, pResource ) );

    m_resources.push_back ( pResource );

    CLuaMain* pLuaMain = pResource->GetVirtualMachine ();
    assert ( !pLuaMain );
    MapSet ( m_NameResourceMap, strResourceNameKey, pResource );
}

//
// Remove resource from the internal lists
//
void CResourceManager::RemoveResourceFromLists ( CResource* pResource )
{
    SString strResourceNameKey = SString ( pResource->GetName () ).ToUpper ();

    assert ( m_resources.Contains ( pResource ) );
    assert ( MapContains ( m_NameResourceMap, strResourceNameKey ) );
    m_resources.remove ( pResource );
    MapRemove ( m_NameResourceMap, strResourceNameKey );
}


CResource* CResourceManager::GetResourceFromLuaState ( lua_State* luaVM )
{
    luaVM = lua_getmainstate ( luaVM );

    // Use lookup map
    CResource** ppResource = MapFind ( m_LuaStateResourceMap, luaVM );
    if ( ppResource )
    {
        CResource* pResource = *ppResource;
        CLuaMain* pLuaMain = pResource->GetVirtualMachine ();
        if ( pLuaMain )
        {
            assert ( luaVM == pLuaMain->GetVirtualMachine () );
            return pResource;
        }
    }
    return NULL;
}


bool CResourceManager::IsAResourceElement ( CElement* pElement )
{
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        CResource* pResource = *iter;
        if ( pResource->IsActive () )
        {
            if ( pResource->GetResourceRootElement () == pElement || pResource->GetDynamicElementRoot () == pElement )
                return true;
            else
            {
                list < CResourceFile* > ::iterator fiter = pResource->IterBegin ();
                for ( ; fiter != pResource->IterEnd (); fiter++ )
                {
                    if ( (*fiter)->GetType () == CResourceFile::RESOURCE_FILE_TYPE_MAP )
                    {
                        CResourceMapItem* pMap = static_cast < CResourceMapItem* > ( *fiter );
                        if ( pMap->GetMapRootElement () == pElement )
                            return true;
                    }
                }
            }
        }
    }
    return false;
}


bool CResourceManager::StartResource ( CResource* pResource, list < CResource* > * dependents, bool bStartedManually, bool bStartIncludedResources, bool bConfigs, bool bMaps, bool bScripts, bool bHTML, bool bClientConfigs, bool bClientScripts, bool bClientFiles )
{
    // Has resurce changed since load?
    if ( pResource->HasResourceChanged() )
    {
        // Attempt to reload it
        if ( Reload ( pResource ) )
        {
            // Start the resource
            return pResource->Start( NULL, bStartedManually, bStartIncludedResources, bConfigs, bMaps, bScripts, bHTML, bClientConfigs, bClientScripts, bClientFiles );
        }
        else
            return false;
    }
    else
    {
        // If it's not running yet
        if ( !pResource->IsActive() )
        {
            // Start it
            return pResource->Start ( dependents, bStartedManually, bStartIncludedResources, bConfigs, bMaps, bScripts, bHTML, bClientConfigs, bClientScripts, bClientFiles );
        }
        return false;
    }

    // Stop it again if it failed starting
    pResource->Stop ();
    return false;
}


bool CResourceManager::Reload ( CResource* pResource )
{
    // Copy old resource's name
    std::string strResourceName = pResource->GetName ();

    // Output it was changed
    CLogger::LogPrintf ( "Resource '%s' changed, reloading and starting\n", strResourceName.c_str () );

    // Delete the old resource and create a new one
    pResource->Reload ();

    // Was it loaded successfully?
    if ( pResource->IsLoaded () )
    {
        // Add the resource back to the list
        m_bResourceListChanged = true;

        // Generate a new ID for it
        pResource->SetNetID ( GenerateID () );
    }
    else
    {
        CLogger::LogPrintf ( "Loading of resource '%s' failed\n", strResourceName.c_str () );
        return false;
    }

    // Success
    return true;
}


bool CResourceManager::StopAllResources ( void )
{
    CLogger::SetMinLogLevel ( LOGLEVEL_MEDIUM );
    CLogger::LogPrint ( "Stopping resources..." );
    CLogger::ProgressDotsBegin ();

    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        CResource* pResource = *iter;
        if ( pResource->IsActive () )
        {

            if ( pResource->IsPersistent () )
                pResource->SetPersistent ( false );

            pResource->Stop ( true );

            CLogger::ProgressDotsUpdate ();
        }
    }

    CLogger::ProgressDotsEnd ();
    CLogger::SetMinLogLevel ( LOGLEVEL_LOW );
    return true;
}


void CResourceManager::QueueResource ( CResource* pResource, eResourceQueue eQueueType, const sResourceStartFlags* Flags, list < CResource* > * dependents )
{
    // Make the queue item
    sResourceQueue sItem;
    sItem.pResource = pResource;
    sItem.eQueue = eQueueType;
    if ( dependents )
        for ( list < CResource* >::iterator it = dependents->begin () ; it != dependents->end () ; ++it )
           sItem.dependents.push_back ( (*it)->GetName () );

    if ( Flags )
    {
        sItem.Flags = *Flags;
    }
    else
    {
        sItem.Flags.bClientConfigs = true;
        sItem.Flags.bClientFiles = true;
        sItem.Flags.bClientScripts = true;
        sItem.Flags.bHTML = true;
        sItem.Flags.bScripts = true;
        sItem.Flags.bMaps = true;
        sItem.Flags.bConfigs = true;
    }

    // Push it to the back of the queue
    m_resourceQueue.push_back ( sItem );
}


void CResourceManager::ProcessQueue ( void )
{
    // While we have queuestuff to process
    while ( m_resourceQueue.size () > 0 )
    {
        // Pop the first item
        sResourceQueue sItem = m_resourceQueue.front ();
        m_resourceQueue.pop_front ();

        // Supposed to stop it?
        if ( sItem.eQueue == QUEUE_STOP )
        {
            // It must be active
            if ( sItem.pResource->IsActive () )
            {
                // So we can stop it incase it is persistent
                if ( sItem.pResource->IsPersistent () )
                {
                    sItem.pResource->SetPersistent ( false );
                }

                // Stop the resource
                if ( !sItem.pResource->Stop ( true ) )
                {
                    // Failed
                    CLogger::ErrorPrintf ( "Unable to stop resource %s\n", sItem.pResource->GetName ().c_str () );
                }
            }
        }

        // Supposed to stop all resources
        else if ( sItem.eQueue == QUEUE_STOPALL )
        {
            // Do so
            StopAllResources ();
        }

        // Supposed to restart it?
        else if ( sItem.eQueue == QUEUE_RESTART )
        {
            // It must be active
            if ( sItem.pResource->IsActive () )
            {
                // So we can stop it incase it is persistent
                if ( sItem.pResource->IsPersistent() )
                {
                    sItem.pResource->SetPersistent ( false );
                }

                // Copy the dependents
                list<CResource *> * resourceList = sItem.pResource->GetDependents();
                list<CResource *> resourceListCopy;
                list<CResource *>::iterator iterd = resourceList->begin();
                for ( ; iterd != resourceList->end(); iterd++ )
                {
                    resourceListCopy.push_back ( (*iterd) );
                }

                // Stop it
                if ( sItem.pResource->Stop ( true ) )
                {
                    // Continue after the rest of the queue is processed
                    QueueResource ( sItem.pResource, QUEUE_RESTART2, &sItem.Flags, &resourceListCopy );
                }
                else
                    CLogger::ErrorPrintf ( "Unable to stop resource %s for restart\n", sItem.pResource->GetName ().c_str () );
            }
        }
        // Restart part 2
        else if ( sItem.eQueue == QUEUE_RESTART2 )
        {
            list < CResource* > resourceListCopy;
            for ( vector < SString >::iterator it = sItem.dependents.begin () ; it != sItem.dependents.end () ; ++it )
            {
                CResource* pResource = GetResource ( *it );
                if ( pResource )
                    resourceListCopy.push_back ( pResource);
            }

            // Start it again
            if ( !StartResource ( sItem.pResource, &resourceListCopy, true, true,
                                 sItem.Flags.bConfigs, sItem.Flags.bMaps,
                                 sItem.Flags.bScripts, sItem.Flags.bHTML,
                                 sItem.Flags.bClientConfigs, sItem.Flags.bClientScripts,
                                 sItem.Flags.bClientFiles ) )
            {
                // Failed
                CLogger::ErrorPrintf ( "Unable to restart resource %s\n", sItem.pResource->GetName ().c_str () );
            }
            else
                CLogger::LogPrintf ( "%s restarted successfully\n", sItem.pResource->GetName ().c_str () );
        }
    }

}


void CResourceManager::RemoveFromQueue ( CResource* pResource )
{
    // Loop through our resourcequeue
    std::list < sResourceQueue > ::iterator iter = m_resourceQueue.begin ();
    while ( iter != m_resourceQueue.end () )
    {
        // Matching resource, erase it. Otherwize just increase the iterator
        if ( iter->pResource == pResource )
            iter = m_resourceQueue.erase ( iter );
        else
            iter++;
    }
}


bool CResourceManager::Install ( char * szURL, char * szName )
{
    if ( IsValidFilePath(szName) )
    {
        CTCPImpl * pTCP = new CTCPImpl;
        pTCP->Initialize ();

        CHTTPRequest * request = new CHTTPRequest ( szURL );
        CHTTPResponse * response = request->Send ( pTCP );
        if ( response )
        {
            size_t dataLength = response->GetDataLength();
            if ( dataLength != 0 )
            {
                const char* szBuffer = response->GetData ();

                SString strResourceRoot = g_pServerInterface->GetModManager ()->GetAbsolutePath ( "resources" );
                SString strResourceFileName ( "%s/%s.zip", strResourceRoot.c_str (), szName );

                FILE * file = fopen ( strResourceFileName, "wb" );
                if ( file )
                {
                    fwrite ( szBuffer, dataLength, 1, file );
                    fclose ( file );
                    delete pTCP;
                    return true;
                }
            }
        }
        delete request;
        delete pTCP;
    }
    return false;
}


/////////////////////////////////
//
// CreateResource
//
/////////////////////////////////
CResource* CResourceManager::CreateResource ( const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus )
{
    // Calculate destination location
    SString strDstAbsPath          = PathJoin ( g_pServerInterface->GetServerModPath (), "resources", strNewOrganizationalPath );
    SString strDstResourceLocation = PathJoin ( strDstAbsPath, strNewResourceName );

    // Does the resource name already exist?
    if ( GetResource ( strNewResourceName ) != NULL )
    {
        CLogger::LogPrintf ( "CreateResource - Could not create '%s' as the resource already exists\n", *strNewResourceName );
        return NULL;
    }

    // Create destination folder
    MakeSureDirExists( strDstResourceLocation + "/" );

    // Create an empty meta file for that resource
    SString strMetaPath = PathJoin ( strDstResourceLocation, "meta.xml" );
    CXMLFile* pXML = g_pServerInterface->GetXML ()->CreateXML ( strMetaPath );
    if ( !pXML )
    {
        CLogger::LogPrintf ( "CreateResource - Could not create '%s'\n", *strMetaPath );
        return NULL;
    }
    else
    {
        // If we got the rootnode created, write the XML
        pXML->CreateRootNode ( "meta" );
        if ( !pXML->Write () )
        {
            delete pXML;
            CLogger::LogPrintf ( "CreateResource - Could not save '%s'\n", *strMetaPath );
            return NULL;
        }

        // Delete the XML
        delete pXML;
        pXML = NULL;
    }

    // Add the resource and load it
    CResource* pResource = new CResource ( this, false, strDstAbsPath, strNewResourceName );
    AddResourceToLists ( pResource );
    return pResource;
}


/////////////////////////////////
//
// CopyResource
//
/////////////////////////////////
CResource* CResourceManager::CopyResource ( CResource* pSourceResource, const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus )
{
    // Get source resource information
    SString strSrcResourceName     = pSourceResource->GetName ();
    SString strSrcOrganizationalPath = GetResourceOrganizationalPath ( pSourceResource );

    // Calculate destination location
    SString strDstTemp             = PathJoin ( g_pServerInterface->GetServerModPath (), "resource-cache", "temp", strNewResourceName );
    SString strDstOrganizationalPath = strNewOrganizationalPath.empty () ? strSrcOrganizationalPath : strNewOrganizationalPath;
    SString strDstAbsPath          = PathJoin ( g_pServerInterface->GetServerModPath (), "resources", strDstOrganizationalPath );
    SString strDstResourceLocation = PathJoin ( strDstAbsPath, strNewResourceName );

    // Is the source resource loaded
    if ( !pSourceResource->IsLoaded () )
    {
        strOutStatus = SString ( "Could not copy '%s' as the resource is not loaded\n", *strSrcResourceName );
        return NULL;
    }

    // Does the resource name already exist?
    if ( GetResource ( strNewResourceName ) != NULL )
    {
        strOutStatus = SString ( "Could not copy '%s' as the resource '%s' already exists\n", *strSrcResourceName, *strNewResourceName );
        return NULL;
    }

    // Does the destination directory already exist?
    if ( FileExists ( strDstResourceLocation ) || DirectoryExists ( strDstResourceLocation ) )
    {
        strOutStatus = SString ( "Could not copy '%s' as the file/directory '%s' already exists\n", *strSrcResourceName, *strNewResourceName );
        return NULL;
    }

    // Clear temp directory
    // in temp:
    //    Create target directory
    //    Copy meta
    //    Copy resouce files
    //    If copy success, move directory

    // Make sure temp directory is clear
    MoveDirToTrash ( strDstTemp );

    // Copy meta.xml
    {
        SString strFileName = "meta.xml";
        SString strSrcFileName;
        pSourceResource->GetFilePath ( strFileName, strSrcFileName );

        SString strDstFileName = PathJoin ( strDstTemp, strFileName );

        if ( !FileCopy ( strSrcFileName, strDstFileName ) )
        {
            strOutStatus = SString ( "Could not copy '%s' to '%s'\n", *strSrcFileName, *strDstFileName );
            return NULL;
        }
    }

    // Go through all our resource files
    std::list <CResourceFile*> ::iterator iter = pSourceResource->IterBegin ();
    for ( ; iter != pSourceResource->IterEnd (); iter++ )
    {
        CResourceFile* pResourceFile = *iter;

        SString strFileName = pResourceFile->GetName ();
        SString strSrcFileName;
        pSourceResource->GetFilePath ( strFileName, strSrcFileName );
        SString strDstFileName = PathJoin ( strDstTemp, strFileName );

        if ( !FileCopy ( strSrcFileName, strDstFileName ) )
        {
            strOutStatus = SString ( "Could not copy '%s' to '%s'\n", *strSrcFileName, *strDstFileName );
            return NULL;
        }
    }

    // Now move the directory into place
    MakeSureDirExists( strDstResourceLocation );
    if ( !FileRename ( strDstTemp, strDstResourceLocation ) )
    {
        strOutStatus = SString ( "Could not rename '%s' to '%s'\n", *strDstTemp, *strDstResourceLocation );
        return NULL;
    }

    // Add the resource and load it
    CResource* pResource = Load ( false, strDstAbsPath, strNewResourceName );
    return pResource;
}


/////////////////////////////////
//
// RenameResource
//
/////////////////////////////////
CResource* CResourceManager::RenameResource ( CResource* pSourceResource, const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus )
{
    // Get source resource information
    SString strSrcResourceName     = pSourceResource->GetName ();
    bool bIsZip                    = pSourceResource->IsResourceZip ();
    SString strSrcAbsPath          = ExtractPath ( ExtractPath ( PathConform ( pSourceResource->GetResourceDirectoryPath () ) ) );
    SString strSrcOrganizationalPath = GetResourceOrganizationalPath ( pSourceResource );
    SString strSrcResourceLocation = pSourceResource->GetResourceDirectoryPath ();
    if ( bIsZip )
        strSrcResourceLocation = strSrcResourceLocation.TrimEnd ( "\\" ).TrimEnd ( "/" ) + ".zip";

    // Calculate destination location
    SString strDstOrganizationalPath = strNewOrganizationalPath.empty () ? strSrcOrganizationalPath : strNewOrganizationalPath;
    SString strDstAbsPath          = PathJoin ( g_pServerInterface->GetServerModPath (), "resources", strDstOrganizationalPath );
    SString strDstResourceLocation = PathJoin ( strDstAbsPath, strNewResourceName );
    if ( bIsZip )
        strDstResourceLocation = strDstResourceLocation.TrimEnd ( "\\" ).TrimEnd ( "/" ) + ".zip";

    // Is the source resource active
    if ( pSourceResource->IsActive () )
    {
        strOutStatus = SString ( "Could not rename '%s' as the resource is running\n", *strSrcResourceName );
        return NULL;
    }

    // Is the source resource loaded
    if ( !pSourceResource->IsLoaded () )
    {
        strOutStatus = SString ( "Could not rename '%s' as the resource is not loaded\n", *strSrcResourceName );
        return NULL;
    }

    // Does the destination already exist?
    if ( FileExists ( strDstResourceLocation ) || DirectoryExists ( strDstResourceLocation ) )
    {
        strOutStatus = SString ( "Could not rename to '%s' as the file/directory name already exists\n", *strNewResourceName );
        return NULL;
    }

    // Unload - this will also free the resource object
    Unload ( pSourceResource );
    pSourceResource = NULL;

    // Rename
    MakeSureDirExists( strDstResourceLocation );
    if ( !FileRename ( strSrcResourceLocation, strDstResourceLocation ) )
    {
        strOutStatus = SString ( "Could not rename '%s' to '%s'\n", *strSrcResourceLocation, *strDstResourceLocation );

        // Panic reload
        CResource* pResource = Load ( bIsZip, strSrcAbsPath, strSrcResourceName );
        return pResource;
    }

    // reload as new name
    CResource* pResource = Load ( bIsZip, strDstAbsPath, strNewResourceName );
    return pResource;
}


/////////////////////////////////
//
// DeleteResource
//
// TODO: Handle invalid resources by matching the directory name
//
/////////////////////////////////
bool CResourceManager::DeleteResource ( const SString& strResourceName, SString& strOutStatus )
{
    // See if it's a known resource first
    CResource* pSourceResource = g_pGame->GetResourceManager()->GetResource ( strResourceName );

    // Is the source resource present
    if ( !pSourceResource )
    {
        strOutStatus = SString ( "Could not delete '%s' as the resource could not be found\n", *strResourceName );
        return false;
    }

    // Get source resource information
    SString strSrcResourceName     = pSourceResource->GetName ();
    bool bIsZip                    = pSourceResource->IsResourceZip ();
    SString strSrcResourceLocation = pSourceResource->GetResourceDirectoryPath ();
    if ( bIsZip )
        strSrcResourceLocation = strSrcResourceLocation.TrimEnd ( "\\" ).TrimEnd ( "/" ) + ".zip";

    // Is the source resource active
    if ( pSourceResource->IsActive () )
    {
        strOutStatus = SString ( "Could not delete '%s' as the resource is running\n", *strSrcResourceName );
        return false;
    }

    // Unload - this will also free the resource object
    Unload ( pSourceResource );
    pSourceResource = NULL;

    // Move resource dir/zip to the trash
    return MoveDirToTrash ( strSrcResourceLocation );
}


/////////////////////////////////
//
// CResourceManager::GetResourceTrashDir
//
/////////////////////////////////
SString CResourceManager::GetResourceTrashDir ( void )
{
    return PathJoin ( g_pServerInterface->GetServerModPath (), "resource-cache", "trash" );
}


/////////////////////////////////
//
// CResourceManager::MoveDirToTrash
//
/////////////////////////////////
bool CResourceManager::MoveDirToTrash ( const SString& strPathDirName )
{
    // Get path to unique trash sub-directory
    SString strDestPathDirName = MakeUniquePath ( PathJoin ( GetResourceTrashDir (), ExtractFilename ( strPathDirName.TrimEnd ( "\\" ).TrimEnd ( "/" ) ) ) );
    // Make sure the trash directory exists and create if it does not
    MakeSureDirExists ( GetResourceTrashDir () + "/" );
    // Try move
    return FileRename ( strPathDirName, strDestPathDirName );
}


/////////////////////////////////
//
// CResourceManager::GetResourceOrganizationalPath
//
/////////////////////////////////
SString CResourceManager::GetResourceOrganizationalPath ( CResource* pResource )
{
    SString strBaseAbsPath        = PathJoin ( g_pServerInterface->GetServerModPath (), "resources" );
    SString strResourceAbsPath    = ExtractPath ( ExtractPath ( PathConform ( pResource->GetResourceDirectoryPath () ) ) );
    SString strOrganizationalPath = strResourceAbsPath.SubStr ( strBaseAbsPath.length () );
    return strOrganizationalPath;
}


// pResource may be changed on return, and it could be NULL if the function returns false.
bool CResourceManager::ParseResourcePathInput ( std::string strInput, CResource*& pResource, std::string* pstrPath, std::string* pstrMetaPath )
{
    ReplaceOccurrencesInString ( strInput, "\\", "/" );
    std::string strMetaPath;

    if ( strInput[0] == '@' )
    {
        // This isn't relevant on the server because all files are private
        // But let's skip the symbol anyway
        strInput = strInput.substr ( 1 );
    }

    if ( strInput[0] == ':' )
    {
        unsigned int iEnd = strInput.find_first_of ( "/" );
        if ( iEnd )
        {
            std::string strResourceName = strInput.substr ( 1, iEnd - 1 );
            pResource = g_pGame->GetResourceManager ()->GetResource ( strResourceName.c_str() );
            if ( pResource && strInput[iEnd + 1] )
            {
                strMetaPath = strInput.substr ( iEnd + 1 );
                if ( pstrMetaPath )
                    *pstrMetaPath = strMetaPath;
                if ( IsValidFilePath ( strMetaPath.c_str() ) )
                {
                    if ( pstrPath && !pResource->GetFilePath ( strMetaPath.c_str (), *pstrPath ) )
                        *pstrPath = pResource->GetResourceDirectoryPath () + strMetaPath;
                    return true;
                }
            }
        }
    }
    else if ( pResource && IsValidFilePath ( strInput.c_str() ) )
    {
        strMetaPath = strInput;
        if ( pstrMetaPath )
            *pstrMetaPath = strMetaPath;
        if ( pstrPath && !pResource->GetFilePath ( strMetaPath.c_str (), *pstrPath ) )
            *pstrPath = pResource->GetResourceDirectoryPath () + strMetaPath;
        return true;
    }
    return false;
}


/////////////////////////////////////////////////////////////////////////////
//
// CResourceManager::ApplyMinClientRequirement
//
// If resource has a client version requirement, add it to the list
//
/////////////////////////////////////////////////////////////////////////////
void CResourceManager::ApplyMinClientRequirement ( CResource* pResource, const SString& strMinClientRequirement )
{
    if ( !strMinClientRequirement.empty () )
    {
        MapSet ( m_MinClientRequirementMap, pResource, strMinClientRequirement );
        ReevaluateMinClientRequirement ();
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// CResourceManager::RemoveMinClientRequirement
//
// Remove any previous client version requirement associated with the resource
//
/////////////////////////////////////////////////////////////////////////////
void CResourceManager::RemoveMinClientRequirement ( CResource* pResource )
{
    if ( MapContains ( m_MinClientRequirementMap, pResource ) )
    {
        MapRemove ( m_MinClientRequirementMap, pResource );
        ReevaluateMinClientRequirement ();
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// CResourceManager::ReevaluateMinClientRequirement
//
// Recalculate highest client version requirement from all running resources
//  and get the config to apply it
//
/////////////////////////////////////////////////////////////////////////////
void CResourceManager::ReevaluateMinClientRequirement ( void )
{
    // Calc highest requirement
    SString strMinClientRequirement;
    for ( std::map < CResource*, SString >::iterator iter = m_MinClientRequirementMap.begin () ; iter != m_MinClientRequirementMap.end () ; ++iter )
        if ( iter->second > strMinClientRequirement )
            strMinClientRequirement = iter->second;

    // Apply
    SString strBefore = g_pGame->GetConfig ()->GetMinimumClientVersion ();
    g_pGame->GetConfig ()->SetMinimumClientVersionOverride ( strMinClientRequirement );
    SString strAfter = g_pGame->GetConfig ()->GetMinimumClientVersion ();

    // Log change
    if ( strBefore != strAfter && !strAfter.empty () )
        CLogger::LogPrintf ( SString ( "Server minclientversion is now %s\n", *strAfter ) );
}
