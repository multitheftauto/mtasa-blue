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

CResourceManager::CResourceManager ( void )
{
    m_bResourceListChanged = false;
    m_uiResourceLoadedCount = 0;
    m_uiResourceFailedCount = 0;

    sprintf ( m_szResourceDirectory, "%s/mods/deathmatch/resources", g_pServerInterface->GetServerPath () );
}

CResourceManager::~CResourceManager ( void )
{
    // First process the queue to make sure all queued up tasks are done
    ProcessQueue ();

    // Then kill all the running resources
    StopAllResources ();

    // Then start deleting them
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        delete (*iter);
    }
}


// Load the complete list of resources and create their objects
// DOES NOT reload already loaded resources, we need a special function for lua for that (e.g. reloadResource)
bool CResourceManager::Refresh ( void )
{
    
    char szBuffer [MAX_PATH];

    unsigned int uiCount = 0;

    UnloadRemovedResources();

    #ifdef WIN32

        // Find all .map files in the maps folder
        WIN32_FIND_DATA FindData;
        HANDLE hFind = FindFirstFile ( g_pServerInterface->GetModManager ()->GetAbsolutePath ( "resources/*", szBuffer, MAX_PATH ), &FindData );
        if ( hFind != INVALID_HANDLE_VALUE )
        {
            // Remove the extension and store the time
            FindData.cFileName [ strlen ( FindData.cFileName ) - 4 ] = 0;
            // Add each file
            do
            {
                if ( strcmp ( FindData.cFileName, ".." ) != 0 && strcmp ( FindData.cFileName, "." ) != 0 )
                {
                    char * extn = NULL;
                    if ( ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != FILE_ATTRIBUTE_DIRECTORY )
                    {
                        extn = &FindData.cFileName [ strlen ( FindData.cFileName ) - 3 ];
                        FindData.cFileName [ strlen ( FindData.cFileName ) - 4 ] = 0;
                    }

                    if ( extn == NULL || strcmp ( extn, "zip" ) == 0 )
                    {
                        // Add the resource
                        Load ( FindData.cFileName );

                        // Increment the counter
                        uiCount++;
                    }
                }
            } while ( FindNextFile ( hFind, &FindData ) );

            // Close the search
            FindClose ( hFind );

            CheckResourceDependencies();

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
    #else
        DIR *Dir;
		struct dirent *DirEntry;
		time_t llHighestTime = 0;
		char szPath[MAX_PATH] = {0};

		if ( ( Dir = opendir ( g_pServerInterface->GetModManager ()->GetAbsolutePath ( "resources/", szBuffer, MAX_PATH ) ) ) )
		{
			while ( ( DirEntry = readdir ( Dir ) ) != NULL )
			{
                // Skip . and .. entry
                if ( strcmp ( DirEntry->d_name, "." ) != 0 && 
                     strcmp ( DirEntry->d_name, ".." ) != 0 )
                {
				    struct stat Info;
				    bool bDir = false;

				    // Get the path
				    if ( strlen(szBuffer) + strlen(DirEntry->d_name) < MAX_PATH )
                    {
					    strcpy ( szPath, szBuffer );
					    unsigned long ulPathLength = strlen ( szPath );

					    if ( szPath [ ulPathLength-1 ] != '/') strcat ( szPath, "/" );

					    strcat ( szPath, DirEntry->d_name );

					    // Determine the file stats
					    if ( lstat ( szPath, &Info ) != -1 )
						    bDir = S_ISDIR ( Info.st_mode );
					    else
						    CLogger::ErrorPrintf ( "Unable to stat %s\n", szPath );

				        // Chop off the extension if it's not a dir
				        char * extn = NULL;
				        if ( !bDir )
                        {
					        extn = &(DirEntry->d_name [ strlen ( DirEntry->d_name ) - 3 ]);
					        DirEntry->d_name [ strlen ( DirEntry->d_name ) - 4 ] = 0;
				        }
                        if ( extn == NULL || strcmp ( extn, "zip" ) == 0 )
                        {
                            // Add the resource
                            Load ( DirEntry->d_name );

                            // Increment the counter
                            uiCount++;
                        }

				    }
                }


			}

            CheckResourceDependencies();

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

			// Close the directory handle
			closedir ( Dir );
		}
    #endif

    // The list hasn't changed
    return false;
}

char * CResourceManager::GetResourceDirectory ( void )
{
    return m_szResourceDirectory;
}

// first, go through each resource then link up to other resources, any that fail are noted
// then go through each resource and perform a recursive check
void CResourceManager::CheckResourceDependencies ( void )
{
    m_uiResourceLoadedCount = 0;
    m_uiResourceFailedCount = 0;
    list < CResource* > ::iterator iter = m_resources.begin ();
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

list<class CResource *>* CResourceManager::GetLoadedResources ( void )
{
    return &m_resources;
}

void CResourceManager::ListResourcesLoaded ( void )
{
    unsigned int uiCount = 0;
    unsigned int uiFailedCount = 0;
    unsigned int uiRunningCount = 0;
    CLogger::LogPrintf ( "== Resource list ==\n" );
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        CResource * res = (*iter);
        if ( res->IsLoaded() )
        {
            if ( res->IsActive() )
            {
                CLogger::LogPrintf ( "%-20.20s   RUNNING   (%d dependents)\n", res->GetName(), res->GetDependentCount() );
                uiRunningCount++;
            }
            else
                CLogger::LogPrintf ( "%-20.20s   STOPPED   (%d files)\n", res->GetName(), res->GetFileCount() );
            uiCount ++;
        }
        else
        {
            CLogger::LogPrintf ( "%-20.20s   FAILED    (see info command for reason)\n", (*iter)->GetName().c_str () );
            uiFailedCount ++;
        }
    }
    CLogger::LogPrintf ( "Resources: %d loaded, %d failed, %d running\n", uiCount, uiFailedCount, uiRunningCount );
}

// check all loaded resources and see if they're still valid (i.e. have a meta.xml file)
void CResourceManager::UnloadRemovedResources ( void )
{
    list < CResource* > resourcesToDelete;
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        char szPath [ MAX_PATH ] = {'\n'};
        if ( (*iter)->GetFilePath ( "meta.xml", szPath, MAX_PATH ) == NULL )
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
        if ( !m_resources.empty() ) m_resources.remove ( *iter );
        RemoveFromQueue ( *iter );
        m_resourcesToStartAfterRefresh.remove ( *iter );
        delete *iter;
    }

                
}

void CResourceManager::Unload ( CResource * resource )
{
    m_resources.remove ( resource );
    m_resourcesToStartAfterRefresh.remove ( resource );
    RemoveFromQueue ( resource );
    delete resource;
}

CResource * CResourceManager::Load ( const char * szResourceName )
{
    CResource * loadedResource = NULL;
    bool bStartAfterLoading = false;

    // check to see if we've already loaded this resource - we can only
    // load each resource once

    list < CResource* > resourcesToDelete;
    list < CResource* > ::iterator iter = m_resources.begin ();
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
        if ( !m_resources.empty() ) m_resources.remove ( *iter );
        m_resourcesToStartAfterRefresh.remove ( *iter );
        RemoveFromQueue ( *iter );
        delete *iter;

        m_bResourceListChanged = true;
    }
                   
    loadedResource = new CResource ( this, szResourceName );
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
        m_resources.push_back ( loadedResource );
        m_bResourceListChanged = true;
        unsigned short usID = GenerateID ();
        loadedResource->SetID ( usID );
    }

    return loadedResource;
}

CResource * CResourceManager::GetResource ( const char * szResourceName )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( stricmp ( (*iter)->GetName().c_str (), szResourceName ) == 0 )
        {
            return *iter;
        }
    }
    return NULL;
}

CResource * CResourceManager::GetResource ( unsigned short usID )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( (*iter)->GetID() == usID )
        {
            return *iter;
        }
    }
    return NULL;
}



bool CResourceManager::Exists ( CResource* pResource )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( pResource == *iter )
        {
            return true;
        }
    }
    return false;
}


/*ResponseCode CResourceManager::HandleRequest ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse )
{
    char szListBuffer[2048] = "<font face='tahoma,arial' size=-1><h2>Resource list</h2><br><table width=100%>";
    char szTemp[2048];
    list<CResource *>* resources = g_pGame->GetResourceManager()->GetLoadedResources();
    unsigned int uiCount = 0;
    unsigned int uiFailedCount = 0;
    unsigned int uiRunningCount = 0;
    list < CResource* > ::iterator iter = resources->begin ();
    for ( ; iter != resources->end (); iter++ )
    {
        CResource * res = (*iter);
        char szLink[256];
        if ( res->IsActive() )
            _snprintf ( szLink, 256, "<a href='/resources/%s/' style='color:black'>%s</a> <sup><font size=-2><a href='/resource-info/%s/' style='color:black;'>Info</a></font></sup>", res->GetName(), res->GetName(), res->GetName() );
        else
            _snprintf ( szLink, 256, "%s <sup><font size=-2><a href='/resource-info/%s/' style='color:black;'>Info</a></font></sup>", res->GetName(), res->GetName() );

        if ( res->IsLoaded() )
        {
            if ( res->IsActive() )
            {
                sprintf ( szTemp, "<tr><td>%s</td><td>RUNNING</td><td>%d dependents</td></tr>", szLink, res->GetDependentCount() );
                
                uiRunningCount++;
            }
            else
                sprintf ( szTemp, "<tr><td>%s</td><td>STOPPED</td><td>%d files</td></tr>", szLink, res->GetFileCount() );
            uiCount ++;
        }
        else
        {
            sprintf ( szTemp, "<tr><td>%s</td><td>FAILED</td><td>see info command for reason</td></tr>", szLink );
            uiFailedCount ++;
        }
        strcat ( szListBuffer, szTemp );
    }
    sprintf ( szTemp, "</table>Resources: %d loaded, %d failed, %d running\n", uiCount, uiFailedCount, uiRunningCount );
    strcat ( szListBuffer, szTemp );
    ipoHttpResponse->SetBody ( szListBuffer, strlen(szListBuffer) );
    return HTTPRESPONSECODE_200_OK;
}*/

unsigned short CResourceManager::GenerateID ( void )
{
	bool bMatch = false;
	for ( unsigned short i = 0 ; i < 0xFFFE ; i++ )
	{
		list < CResource* > ::iterator iter = m_resources.begin ();
		for ( ; iter != m_resources.end (); iter++ )
		{
			if ( ( *iter )->GetID () == i ) 
                bMatch = true;

		}
		if ( !bMatch ) return i;
		else bMatch = false;
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


CResource* CResourceManager::GetResourceFromLuaState ( lua_State* luaVM )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( (*iter)->GetVirtualMachine () != NULL )
        {
            if ( luaVM == (*iter)->GetVirtualMachine ()->GetVirtualMachine () )
            {
                return *iter;
            }
        }
    }

    return NULL;
}


bool CResourceManager::IsAResourceElement ( CElement* pElement )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
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
        pResource->SetID ( GenerateID () );
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
    CLogger::LogPrint ( "Stopping resources..." );

    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        CResource* pResource = *iter;
        if ( pResource->IsActive () )
        {
            CLogger::SetOutputEnabled ( false );

			if ( pResource->IsPersistent () )
				pResource->SetPersistent ( false );

			pResource->Stop ( true );

            CLogger::SetOutputEnabled ( true );
            CLogger::LogPrintNoStamp ( "." );
        }
    }

    CLogger::LogPrintNoStamp ( "\n" );
    return true;
}


void CResourceManager::QueueResource ( CResource* pResource, eResourceQueue eQueueType, const sResourceStartFlags* Flags )
{
    // Make the queue item
    sResourceQueue sItem;
    sItem.pResource = pResource;
    sItem.eQueue = eQueueType;

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
                    // Start it again
				    if ( !StartResource ( sItem.pResource, &resourceListCopy, true,
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
                else
                    CLogger::ErrorPrintf ( "Unable to stop resource %s for restart\n", sItem.pResource->GetName ().c_str () );
            }
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
                char * szBuffer = new char[dataLength];
                response->GetData ( szBuffer );

                char szResourceRoot[MAX_PATH];
                g_pServerInterface->GetModManager ()->GetAbsolutePath ( "resources", szResourceRoot, MAX_PATH );
                char szResourceFileName[MAX_PATH];
                szResourceFileName[MAX_PATH - 1] = '\0';
                _snprintf ( szResourceFileName, MAX_PATH - 1, "%s/%s.zip", szResourceRoot, szName );


                FILE * file = fopen ( szResourceFileName, "wb" );
                if ( file ) 
                {
                    fwrite ( szBuffer, dataLength, 1, file );
                    fclose ( file );
                    delete[] szBuffer;
                    delete pTCP;
                    return true;
                }
                delete[] szBuffer;
            }
        }
        delete request;
        delete pTCP;
    }
    return false;
}


CResource* CResourceManager::CreateResource ( char* szResourceName )
{
    // Does the resource name already exist?
    if ( GetResource ( szResourceName ) != NULL )
        return NULL;

    // Make a string with the full path to the resource directory
    const char* szCurrentDirectory = g_pServerInterface->GetServerPath ();
    char szResourceDirectoryPath [ MAX_PATH + 1 ];
    _snprintf ( szResourceDirectoryPath, MAX_PATH, "%s/mods/deathmatch/resources/%s/", szCurrentDirectory, szResourceName );

    // Create that folder. Return if we failed
    if ( mymkdir ( szResourceDirectoryPath ) != 0 )
        return NULL;

    // Create an empty meta file for that resource
    char szMetaPath [MAX_PATH + 1];
    _snprintf ( szMetaPath, MAX_PATH, "%s%s", szResourceDirectoryPath, "meta.xml" );
    CXMLFile* pXML = g_pServerInterface->GetXML ()->CreateXML ( szMetaPath );
    if ( pXML )
    {
        // If we got the rootnode created, write the XML
        CXMLNode* pRootNode = pXML->CreateRootNode ( "meta" );
        if ( !pRootNode || !pXML->Write () )
        {
            delete pXML;
            return NULL;
        }

        // Delete the XML
        delete pXML;
        pXML = NULL;
    }

    // Add the resource and load it
	CResource* pResource = new CResource ( this, szResourceName, true );
	if ( pResource )
	{
		m_resources.push_back ( pResource );
		return pResource;
	}

    // We failed
	return NULL;
}


CResource* CResourceManager::CopyResource ( CResource* pSourceResource, const char* szNewResourceName )
{
    return NULL;
}
