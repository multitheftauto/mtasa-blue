/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResource.cpp
*  PURPOSE:     Resource handler class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               Jax <>
*               Cecill Etheredge <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class controls a single resource, being a zip file
// or a folder that contains a number of files

//#define RESOURCE_DEBUG_MESSAGES // show info about where the actual files are coming from

#include "StdInc.h"
#ifndef WIN32
#include <utime.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

int do_extract_currentfile(unzFile uf,const int* popt_extract_without_path,int*popt_overwrite,const char*password,const char* szFilePath);
unsigned long get_current_file_crc ( unzFile uf );

list < CResource* > CResource::m_StartedResources;

extern CServerInterface* g_pServerInterface;
extern CGame* g_pGame;

#ifdef NO_ERRNO_H
extern int errno;
#endif

// (IJs) This class contains very nasty unchecked and unproper code. Please revise.

CResource::CResource ( CResourceManager * resourceManager, const char * szResourceName, bool bLoad )
{
    m_bHandlingHTTPRequest = false;
    m_pDefaultElementGroup = NULL;
    m_pNodeSettings = NULL;
    m_pNodeStorage = NULL;
    m_resourceManager = resourceManager;
    m_zipfile = NULL;

    // store the name
    m_strResourceName = szResourceName ? szResourceName : "";

    // Initialize
    m_bActive = false;
    m_bIsPersistent = false;
    m_bLoaded = false;
    m_bStarting = false;
    m_bStopping = false;
    m_bLinked = false;
    m_pResourceElement = NULL;
    m_pResourceDynamicElementRoot = NULL;
    m_pVM = NULL;
    m_timeLoaded = 0;
    m_timeStarted = 0;
    m_bResourceIsZip = false;
    m_bProtected = false;
    m_bStartedManually = false;

    m_uiVersionMajor = 0;
    m_uiVersionMinor = 0;
    m_uiVersionRevision = 0;
    m_uiVersionState = 2; // release

    m_bClientConfigs = true;
    m_bClientScripts = true;
    m_bClientFiles = true;
    m_bHasStarted = false;

    pthread_mutex_init(&m_mutex, NULL);

    m_bDoneUpgradeWarnings = false;

    if ( bLoad )
        Load();
}

bool CResource::Load ( void )
{
    if ( !m_bLoaded )
    {
        // Initialize
        m_strCircularInclude = "";
        m_checksum = CChecksum ();
        m_bActive = false;
        m_bIsPersistent = false;
        m_bLoaded = false;
        m_bStarting = false;
        m_bStopping = false;
        m_bLinked = false;
        m_pResourceElement = NULL;
        m_pResourceDynamicElementRoot = NULL;
        m_bResourceIsZip = false;
        m_bProtected = false;
        m_bStartedManually = false;

        m_uiVersionMajor = 0;
        m_uiVersionMinor = 0;
        m_uiVersionRevision = 0;
        m_uiVersionState = 2; // release

        m_bClientConfigs = true;
        m_bClientScripts = true;
        m_bClientFiles = true;
        m_bHasStarted = false;

        m_pVM = NULL;
        // @@@@@ Set some type of HTTP access here

        // Register the time we loaded this resource and zero out the time we started it
        time ( &m_timeLoaded );
        m_timeStarted = 0;

        // Register us in the EHS stuff
        g_pGame->GetHTTPD()->RegisterEHS ( this, m_strResourceName.c_str () );
        this->m_oEHSServerParameters["norouterequest"] = true;
        this->RegisterEHS ( this, "call" );

        // Store the actual directory and zip paths for fast access
        const char* szServerModPath = g_pServerInterface->GetServerModPath ();

        char szBuffer[MAX_PATH];
        _snprintf ( szBuffer, MAX_PATH - 1, "%s/resources/%s/", szServerModPath, m_strResourceName.c_str () );
        m_strResourceDirectoryPath = szBuffer;
        _snprintf ( szBuffer, MAX_PATH - 1, "%s/resource-cache/unzipped/%s/", szServerModPath, m_strResourceName.c_str () );
        m_strResourceCachePath = szBuffer;
        _snprintf ( szBuffer, MAX_PATH - 1, "%s/resources/%s.zip", szServerModPath, m_strResourceName.c_str () );
        m_strResourceZip = szBuffer;

        // Check for our resource directory
        if ( DoesDirectoryExist ( m_strResourceDirectoryPath.c_str () ) )
        {
            m_bResourceIsZip = false;
        }
        else
        {
            // OK, we didn't have a resource directory, it must be a zip file instead
            m_zipfile = unzOpen ( m_strResourceZip.c_str () );
            if ( !m_zipfile )
            {
                //Unregister EHS stuff
                g_pGame->GetHTTPD()->UnregisterEHS ( m_strResourceName.c_str () );

                // Show error
                _snprintf ( szBuffer, MAX_PATH - 1, "Couldn't find resource archive or directory (%s) for resource '%s'.\n", m_strResourceDirectoryPath.c_str (), m_strResourceName.c_str () );
                m_strFailureReason = szBuffer;
                CLogger::ErrorPrintf ( szBuffer );
                return false;
            }

            // Close the zip file
            unzClose ( m_zipfile );
            m_zipfile = NULL;
            m_bResourceIsZip = true;

            // See if the dir already exists
            bool bDirExists = DoesDirectoryExist ( m_strResourceCachePath.c_str () );

            // If the folder doesn't exist, create it
            if ( !bDirExists )
            {
                // If we're using a zip file, we need a temp directory for extracting
                // 17 = already exists (on windows)

                if ( mymkdir ( m_strResourceCachePath.c_str () ) == -1 && errno != EEXIST ) // check this is the correct return for *NIX too
                {
                    // Unregister EHS stuff
                    g_pGame->GetHTTPD()->UnregisterEHS ( m_strResourceName.c_str () );

                    // Show error
                    _snprintf ( szBuffer, MAX_PATH - 1, "Couldn't create directory '%s' for resource '%s', check that the server has write access to the resources folder.\n", m_strResourceCachePath.c_str (), m_strResourceName.c_str () );
                    m_strFailureReason = szBuffer;
                    CLogger::ErrorPrintf ( szBuffer );
                    return false;
                }
            }
        }

        // Load the meta.xml file
        string strMeta;
        if ( !GetFilePath ( "meta.xml", strMeta ) )
        {
            // Unregister the EHS stuff
            g_pGame->GetHTTPD()->UnregisterEHS ( m_strResourceName.c_str () );

            // Show error
            _snprintf ( szBuffer, MAX_PATH - 1, "Couldn't find meta.xml file for resource '%s'\n", m_strResourceName.c_str () );
            m_strFailureReason = szBuffer;
            CLogger::ErrorPrintf ( szBuffer );
            return false;
        }

        // Load the XML file and parse it
        bool bParsedSuccessfully = false;
        CXMLFile * metaFile = g_pServerInterface->GetXML ()->CreateXML ( strMeta.c_str () );
        if ( metaFile )
        {
            bParsedSuccessfully = metaFile->Parse ();
        }

        // If we parsed it successfully
        if ( bParsedSuccessfully )
        {
            CXMLNode * root = metaFile->GetRootNode ();
            if ( root )
            {
                // Find the settings node and copy it (meta xml is deleted at the end of this function, to preserve memory)
                CXMLNode * pNodeSettings = root->FindSubNode ( "settings", 0 );
                if ( pNodeSettings )
                    m_pNodeSettings = pNodeSettings->CopyNode ( NULL );

                // disabled for now
                /*
                CXMLNode * update = root->FindSubNode ( "update", 0 );
                if ( update )
                {
                    CXMLAttributes * attributes = &(update->GetAttributes());
                    if ( attributes )
                    {
                        CXMLAttribute * urlAttr = attributes->Find ( "url" );
                        if ( urlAttr )
                        {
                            char szURL[512];
                            urlAttr->GetValue ( szURL, 511 );
                            szURL[511] = '\0';

                            //g_pGame->GetResourceDownloader()->AddUpdateSite ( szURL );
                        }
                    }
                }
                */

                CXMLNode * info = root->FindSubNode ( "info", 0 );
                if ( info )
                {
                    CXMLAttributes * attributes = & ( info->GetAttributes () );
                    if ( attributes )
                    {
                        for ( unsigned int i = 0; i < attributes->Count(); i++ )
                        {
                            CXMLAttribute * attribute = attributes->Get ( i );
                            m_infoValues.push_back ( new CInfoValue ( attribute->GetName (), attribute->GetValue() ) );
                        }

                        CXMLAttribute * version = attributes->Find ( "major" );
                        if ( version )
                        {
                            m_uiVersionMajor = atoi ( version->GetValue ().c_str () );
                        }

                        version = attributes->Find("minor");
                        if ( version )
                        {
                            m_uiVersionMinor = atoi ( version->GetValue ().c_str () );
                        }

                        version = attributes->Find("revision");
                        if ( version )
                        {
                            m_uiVersionRevision = atoi ( version->GetValue ().c_str () );
                        }

                        version = attributes->Find ( "state" );
                        if ( version )
                        {
                            const char *szVersion = version->GetValue ().c_str ();
                            if ( stricmp ( szVersion, "alpha" ) == 0 )
                                m_uiVersionState = 0;
                            else if ( stricmp ( szVersion, "beta" ) == 0 )
                                m_uiVersionState = 1;
                            else
                                m_uiVersionState = 2;
                        }
                    }
                }

                // Read everything that's included. If one of these fail, delete the XML we created and return
                if ( !ReadIncludedResources ( root ) ||
                    !ReadIncludedMaps ( root ) ||
                    !ReadIncludedFiles ( root ) ||
                    !ReadIncludedScripts ( root ) ||
                    !ReadIncludedHTML ( root ) ||
                    !ReadIncludedExports ( root ) ||
                    !ReadIncludedConfigs ( root ) )
                {
                    delete metaFile;
                    g_pGame->GetHTTPD()->UnregisterEHS ( m_strResourceName.c_str () );
                    return false;
                }
            }

            // Delete the XML we created to save memory
            delete metaFile;
        }
        else
        {
            char szBuffer[255] = {0};
            _snprintf ( szBuffer, 254, "Couldn't parse meta file for resource '%s'\n", m_strResourceName.c_str () );
            m_strFailureReason = szBuffer;
            CLogger::ErrorPrintf ( szBuffer );

            // Delete the XML file if we somehow got to load it halfway
            if ( metaFile )
                delete metaFile;

            g_pGame->GetHTTPD()->UnregisterEHS ( m_strResourceName.c_str () );

            return false;
        }

        // Generate a CRC for this resource
        m_checksum = GenerateChecksum();

        // copy client files to http holding directory
        {
            list < CResourceFile* > ::const_iterator iter = this->IterBegin ();
            for ( ; iter != this->IterEnd () ; iter++ )
            {
                CResourceFile* pResourceFile = *iter;
                switch ( pResourceFile->GetType () )
                {
                    case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT:
                    case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG:
                    case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE:
                    {
                        string clientFileShortPath = pResourceFile->GetName();
                        string strDstFilePath = string ( g_pServerInterface->GetServerModPath () ) + "/resource-cache/http-client-files/" + this->GetName() + "/" + clientFileShortPath;
                        string strSrcFilePath;
                        if ( GetFilePath ( clientFileShortPath.c_str (), strSrcFilePath ) )
                        {
                            unsigned long ulSrcFileCRC = CRCGenerator::GetCRCFromFile ( strSrcFilePath.c_str () );
                            unsigned long ulDstFileCRC = CRCGenerator::GetCRCFromFile ( strDstFilePath.c_str () );
                            if ( ulSrcFileCRC != ulDstFileCRC )
                            {
                                MakeSureDirExists( strDstFilePath.c_str () );

                                if ( !FileCopy ( strSrcFilePath.c_str (), strDstFilePath.c_str () ) )
                                {
                                    CLogger::LogPrintf ( "Could not copy Copy '%s' to '%s'\n", strSrcFilePath.c_str (), strDstFilePath.c_str () );
                                }
                            }
                        }
                    }
                    break;

                    default:
                        break;
               }
            }
        }

       // if  ( stricmp ( this->GetName(), "updtest" ) == 0 )
      //      printf ( "0x%X\n", m_ulCRC );
        m_bLoaded = true;

        /*CLuaArguments Arguments;
        Arguments.PushResource ( this );
        m_pRootElement->CallEvent ( "onResourceLoad", Arguments );*/
        //currently called before the events are initialised @Kevuwk

        m_bDoneUpgradeWarnings = false;
   }

    // Return if we successfully loaded
    return m_bLoaded;
}

void ReplaceSlashes ( string& strPath )
{
    ReplaceOccurrencesInString ( strPath, "\\", "/" );
}

void ReplaceSlashes ( char* szPath )
{
    size_t iLen = strlen ( szPath );
    for ( size_t i = 0; i < iLen; i++ )
    {
        if ( szPath [ i ] == '\\' )
            szPath [ i ] = '/';
    }
}

bool CResource::Unload ( void )
{
    Stop(true);

    TidyUp();
    m_bLoaded = false;
    m_bHasStarted = false;

    if ( m_pNodeStorage )
    {
        delete m_pNodeStorage;
        m_pNodeStorage = NULL;
    }

    m_strResourceZip = "";
    m_strResourceCachePath = "";
    m_strResourceDirectoryPath = "";

    return true;
}


void CResource::Reload ( void )
{
    Unload ();
    Load ();
}


CResource::~CResource ( )
{
    Unload ();

    m_strResourceName = "";

    m_bDestroyed = true;

    pthread_mutex_destroy(&m_mutex);
}


void CResource::TidyUp ( void )
{
    // Close the zipfile stuff
    if ( m_zipfile )
        unzClose ( m_zipfile );

    m_zipfile = NULL;

    // Go through each resource file and delete it
    list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
    for ( ; iterf != m_resourceFiles.end (); iterf++ )
    {
        delete (*iterf);
    }
    m_resourceFiles.clear();

    // Go through each included resource item and delete it
    list < CIncludedResources* > ::iterator iterr = m_includedResources.begin ();
    for ( ; iterr != m_includedResources.end (); iterr++ )
    {
        delete (*iterr);
    }
    m_includedResources.clear();

    // go through each of the dependent resources (those that include this one) and remove the reference to this
    list < CResource* > ::iterator iterc = m_dependents.begin();
    for ( ; iterc != m_dependents.end(); iterc++ )
    {
        (*iterc)->InvalidateIncludedResourceReference ( this );
    }

    this->UnregisterEHS("call");
    g_pGame->GetHTTPD()->UnregisterEHS ( m_strResourceName.c_str () );

}

bool CResource::GetInfoValue ( const char * szKey, std::string& strValue )
{
    // Loop through all the infovalues looking for the matching key. Return the value.
    list < CInfoValue* > ::iterator iterr = m_infoValues.begin ();
    for ( ; iterr != m_infoValues.end (); iterr++ )
    {
        if ( stricmp ( (*iterr)->GetName ().c_str (), szKey ) == 0 )
        {
            strValue = (*iterr)->GetValue ();
            return true;
        }
    }

    // No matching key
    return false;
}

void CResource::SetInfoValue ( const char * szKey, const char * szValue )
{
    // Try to find an existing value with a matching key
    CInfoValue* pValue = 0;
    list < CInfoValue* > ::iterator iter = m_infoValues.begin ();
    for ( ; iter != m_infoValues.end (); iter++ )
    {
        // If the key matches, set the value and return
        pValue = *iter;
        if ( stricmp ( pValue->GetName ().c_str (), szKey ) == 0 )
        {
            if ( !szValue )
            {
                delete pValue;
                iter = m_infoValues.erase ( iter );
            }
            else
                pValue->SetValue ( szValue );
            return;
        }
    }

    // There was no matching key.
    if ( !szValue )         // If we were going to delete the key, we are done at this point
        return;

    // If we were going to set a new value, create a new key and add it to our list
    pValue = new CInfoValue ( szKey, szValue );
    m_infoValues.push_back ( pValue );

    // Save to xml
    std::string strPath;
    if ( GetFilePath ( "meta.xml", strPath ) )
    {
        // Load the meta file
        CXMLFile* metaFile = g_pServerInterface->GetXML ()->CreateXML ( strPath.c_str() );
        if ( metaFile )
        {
            // Parse it
            if ( metaFile->Parse () )
            {
                // Grab its rootnode
                CXMLNode* pRootNode = metaFile->GetRootNode ();
                if ( pRootNode )
                {
                    // Create a new map subnode
                    CXMLNode* pInfoNode = pRootNode->FindSubNode ( "info" );
                    if ( pInfoNode )
                    {
                        CXMLAttribute* pAttr = pInfoNode->GetAttributes ().Find ( szKey );
                        if ( pAttr ) pAttr->SetValue ( szValue );
                        else pInfoNode->GetAttributes ().Create ( szKey )->SetValue ( szValue );
                        // Success, write and destroy XML
                    }
                    else
                    {
                        pInfoNode = pRootNode->CreateSubNode ( "info" );
                        if ( pInfoNode )
                        {
                            pInfoNode->GetAttributes ().Create ( szKey )->SetValue ( szValue );
                        }
                    }
                    metaFile->Write ();
                }
            }

            // Destroy it
            delete metaFile;
        }
    }
}


CChecksum CResource::GenerateChecksum ( void )
{
    // initialize all of the CRC variables
    m_checksum = CChecksum ();
    string strPath;

    list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
    for ( ; iterf != m_resourceFiles.end (); iterf++ )
    {
        if ( GetFilePath ( (*iterf)->GetName(), strPath ) )
        {
            CChecksum checksum = CChecksum::GenerateChecksumFromFile ( strPath );
            ( *iterf )->SetLastChecksum ( checksum );
        }
    }

    if ( GetFilePath ( "meta.xml", strPath ) )
    {
        m_checksum = CChecksum::GenerateChecksumFromFile ( strPath );
    }

    return m_checksum;
}


bool CResource::HasResourceChanged ()
{
    string strPath;

    list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
    for ( ; iterf != m_resourceFiles.end (); iterf++ )
    {
        if ( GetFilePath ( (*iterf)->GetName(), strPath ) )
        {
            CChecksum checksum = CChecksum::GenerateChecksumFromFile ( strPath );
            if ( ( *iterf )->GetLastChecksum() != checksum )
                return true;
        }
    }

    if ( GetFilePath ( "meta.xml", strPath ) )
    {
        CChecksum checksum = CChecksum::GenerateChecksumFromFile ( strPath );
        if ( checksum != m_checksum )
            return true;
    }
    return false;
}


void CResource::ApplyUpgradeModifications ( void )
{
    CResourceChecker ().ApplyUpgradeModifications ( this, m_strResourceZip );
}


bool CResource::Start ( list<CResource *> * dependents, bool bStartedManually, bool bStartIncludedResources, bool bConfigs, bool bMaps, bool bScripts, bool bHTML, bool bClientConfigs, bool bClientScripts, bool bClientFiles )
{
    if ( m_bLoaded && !m_bActive )
    {
        CLuaArguments PreStartArguments;
        PreStartArguments.PushResource ( this );
        if ( !g_pGame->GetMapManager()->GetRootElement()->CallEvent ( "onResourcePreStart", PreStartArguments ) )
        {
            // Start cancelled by another resource
            return false;
        }

        // CheckForIssues
        if ( !m_bDoneUpgradeWarnings )
        {
            m_bDoneUpgradeWarnings = true;
            CResourceChecker ().LogUpgradeWarnings ( this, m_strResourceZip );
        }

        m_bStarting = true;

        // Check the included resources are linked
        if ( !m_bLinked )
        {
            if ( !LinkToIncludedResources() )
            {
                m_bStarting = false;
                return false;
            }
        }
        m_bIsPersistent = false;

        // Create an element group for us
        m_pDefaultElementGroup = new CElementGroup ( this );
        m_elementGroups.push_back ( m_pDefaultElementGroup ); // for use by scripts

        // Grab the root element
        m_pRootElement = g_pGame->GetMapManager()->GetRootElement();

        // Create the temporary storage node
        m_pNodeStorage = g_pServerInterface->GetXML ()->CreateDummyNode ();

        // Create the Resource Element
        m_pResourceElement = new CDummy ( g_pGame->GetGroups(), m_pRootElement );
        m_pResourceElement->SetTypeName ( "resource" );

        // Contains elements created at runtime by scripts etc (i.e. not in maps)
        m_pResourceDynamicElementRoot = new CDummy ( g_pGame->GetGroups(), m_pResourceElement );
        m_pResourceDynamicElementRoot->SetTypeName ( "map" );
        m_pResourceDynamicElementRoot->SetName ( "dynamic" );

        // Set the Resource Element name
        m_pResourceElement->SetName ( m_strResourceName.c_str () );

        // Create the virtual machine for this resource
        CreateVM();

        // We're now active
        m_bActive = true;
        CLogger::LogPrintf ( LOGLEVEL_LOW, "Starting %s\n", m_strResourceName.c_str () );

        // Remember the time we started
        time ( &m_timeStarted );

        // Start all our sub resourcefiles
        list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
        for ( ; iterf != m_resourceFiles.end (); iterf++ )
        {
            if ( ( (*iterf)->GetType() == CResourceFile::RESOURCE_FILE_TYPE_MAP && bMaps ) ||
                 ( (*iterf)->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CONFIG && bConfigs ) ||
                 ( (*iterf)->GetType() == CResourceFile::RESOURCE_FILE_TYPE_SCRIPT && bScripts ) ||
                 ( (*iterf)->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML && bHTML ) )
            {
                // Start. Failed?
                if ( !(*iterf)->Start() )
                {
                    // Log it
                    char szBuffer[255] = {0};
                    CLogger::LogPrintf ( "Failed to start resource item %s in %s\n", (*iterf)->GetName(), m_strResourceName.c_str () );
                    _snprintf ( szBuffer, 254, "Failed to start resource item %s which is required\n", (*iterf)->GetName() );
                    m_strFailureReason = szBuffer;

                    // Stop all the resource items without any warnings
                    StopAllResourceItems();
                    DestroyVM ();

                    // Remove the temporary XML storage node
                    if ( m_pNodeStorage )
                    {
                        delete m_pNodeStorage;
                        m_pNodeStorage = NULL;
                    }

                    // Destroy all the element groups attached directly to this resource
                    list < CElementGroup* > ::iterator itere = m_elementGroups.begin ();
                    for ( ; itere != m_elementGroups.end (); itere++ )
                    {
                        delete (*itere);
                    }
                    m_elementGroups.clear();
                    m_pDefaultElementGroup = NULL;

                    // Make sure we remove the resource elements from the players that have joined
                    CEntityRemovePacket removePacket;
                    if ( m_pResourceElement )
                    {
                        removePacket.Add ( m_pResourceElement );
                        g_pGame->GetElementDeleter()->Delete ( m_pResourceElement );
                        m_pResourceElement = NULL;
                    }

                    if ( m_pResourceDynamicElementRoot )
                    {
                        removePacket.Add ( m_pResourceDynamicElementRoot );
                        g_pGame->GetElementDeleter()->Delete ( m_pResourceDynamicElementRoot );
                        m_pResourceDynamicElementRoot = NULL;
                    }
                    g_pGame->GetPlayerManager()->BroadcastOnlyJoined ( removePacket );

                    m_bActive = false;
                    m_bStarting = false;
                    return false;
                }
            }
        }

        if ( bStartIncludedResources )
        {
            // Copy the list over included resources because reloading them might change the list
            list < CIncludedResources* > CopyList = m_includedResources;
            // Start our included resources that haven't been started
            CResource* pIncluded;
            list < CIncludedResources* > ::iterator iterr = CopyList.begin ();
            for ( ; iterr != CopyList.end (); iterr++ )
            {
                // Has it already been loaded?
                pIncluded = (*iterr)->GetResource();
                if ( pIncluded )
                {
                    // Included resource has changed?
                    if ( pIncluded->HasResourceChanged () )
                    {
                        // Reload it if it's not already started
                        if ( !pIncluded->IsActive () )
                        {
                            m_resourceManager->Reload ( pIncluded );
                        }
                        else
                        {
                            CLogger::LogPrintf ( "WARNING: Included resource %s has changed but unable to reload due to resource already being in use\n", pIncluded->GetName ().c_str () );
                        }
                    }

                    // Make us dependant of it
                    pIncluded->AddDependent ( this );
                }
            }
        }

        // Add the resources depending on us
        if ( dependents )
        {
            list<CResource *>::iterator iterd = dependents->begin();
            for ( ; iterd != dependents->end(); iterd++ )
            {
                AddDependent ( (*iterd) );
            }
        }

        m_bStarting = false;

        // Call the onResourceStart event. If it returns false, cancel this script again
        CLuaArguments Arguments;
        Arguments.PushResource ( this );
        if ( !m_pResourceElement->CallEvent ( "onResourceStart", Arguments ) )
        {
            // We're no longer active. stop the resource
            char szBuffer[255] = {0};
            CLogger::LogPrintf ( "Start up of resource %s cancelled by script\n", m_strResourceName.c_str () );
            _snprintf ( szBuffer, 254, "Start up of resource cancelled by script\n" );
            m_strFailureReason = szBuffer;
            Stop ( true );
            m_bActive = false;
            return false;
        }

        m_bStartedManually = bStartedManually;

        // Remember the client files state
        m_bClientConfigs = bClientConfigs;
        m_bClientScripts = bClientScripts;
        m_bClientFiles = bClientFiles;

        m_bHasStarted = true;

        // Broadcast new resourceelement that is loaded and tell the players that a new resource was started
        g_pGame->GetMapManager()->BroadcastElements ( m_pResourceElement, true );
        g_pGame->GetPlayerManager ()->BroadcastOnlyJoined ( CResourceStartPacket ( m_strResourceName.c_str (), this ) );

        // HACK?: stops resources getting loaded twice when you change them then manually restart
        GenerateChecksum ();

        // Add us to the running resources list
        m_StartedResources.push_back ( this );


      //  if  ( stricmp ( this->GetName(), "updtest" ) == 0 )
        //    printf ( "0x%X\n", m_ulCRC );
    }
    return m_bActive;
}

bool CResource::StopAllResourceItems ( void )
{
    list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
    for ( ; iterf != m_resourceFiles.end (); iterf++ )
    {
        (*iterf)->Stop();
    }
    return true;
}

bool CResource::Stop ( bool bStopManually )
{
    // If we're loaded and active
    if ( !m_bStopping && m_bLoaded && m_bActive && ( !m_bStartedManually || bStopManually ) )
    {
        m_bHasStarted = false;
        m_bStopping = true;

        // Tell the log that we've stopped this resource
        CLogger::LogPrintf ( LOGLEVEL_LOW, "Stopping %s\n", m_strResourceName.c_str () );

        // Tell the modules we are stopping
        g_pGame->GetLuaManager ()->GetLuaModuleManager ()->_ResourceStopping ( m_pVM->GetVirtualMachine () );

        // Remove us from the running resources list
        m_StartedResources.remove ( this );

        // Tell all the players that have joined that this resource is stopped
        g_pGame->GetPlayerManager ()->BroadcastOnlyJoined ( CResourceStopPacket ( m_usID ) );

        // Call the onResourceStop event on this resource element
        CLuaArguments Arguments;
        Arguments.PushResource ( this );
        m_pResourceElement->CallEvent ( "onResourceStop", Arguments );

        // Remove us from the resources we depend on (they might unload too first)
        list < CIncludedResources* > ::iterator iterr = m_includedResources.begin ();
        for ( ; iterr != m_includedResources.end (); iterr++ )
        {
            CResource * resource = (*iterr)->GetResource();
            if ( resource )
            {
                resource->RemoveDependent ( this );
            }
        }

        // Temorary includes??
        list < CResource* > ::iterator iters = m_temporaryIncludes.begin ();
        for ( ; iters != m_temporaryIncludes.end (); iters++ )
        {
            (*iters)->RemoveDependent ( this );
        }

        m_temporaryIncludes.clear();

        // Stop all the resource files we have. The files we share with our clients we remove from the resource file list.
        list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
        for ( ; iterf != m_resourceFiles.end (); iterf++ )
        {
            // Stop it. If it fails, tell the console and return false
            // WARNING: If this is called from Start() and this fails it could get nasty
            if ( !(*iterf)->Stop() )
            {
                CLogger::LogPrintf ( "Failed to stop resource item %s in %s\n", (*iterf)->GetName(), m_strResourceName.c_str () );
                StopAllResourceItems(); // stop every other resource item without any warnings
                return false;
            }
        }

        // Tell the module manager we have stopped
        g_pGame->GetLuaManager ()->GetLuaModuleManager ()->_ResourceStopped ( m_pVM->GetVirtualMachine () );

        // Remove the temporary XML storage node
        if ( m_pNodeStorage )
        {
            delete m_pNodeStorage;
            m_pNodeStorage = NULL;
        }

        // Destroy all the element groups attached directly to this resource
        list < CElementGroup* > ::iterator itere = m_elementGroups.begin ();
        for ( ; itere != m_elementGroups.end (); itere++ )
        {
            delete (*itere);
        }
        m_elementGroups.clear();
        m_pDefaultElementGroup = NULL;

        // Destroy the virtual machine for this resource
        DestroyVM ();

        // We're no longer active
        m_bActive = false;
        m_bStopping = false;

        // Remove the resource element from the client
        CEntityRemovePacket removePacket;
        if ( m_pResourceElement )
        {
            removePacket.Add ( m_pResourceElement );
            g_pGame->GetElementDeleter()->Delete ( m_pResourceElement );
            m_pResourceElement = NULL;
        }

        // Remove the dynamic resource element from the client (???)
        if ( m_pResourceDynamicElementRoot )
        {
            removePacket.Add ( m_pResourceDynamicElementRoot );
            g_pGame->GetElementDeleter()->Delete ( m_pResourceDynamicElementRoot );
            m_pResourceDynamicElementRoot = NULL;
        }

        // Broadcast the packet to joined players
        g_pGame->GetPlayerManager()->BroadcastOnlyJoined ( removePacket );
    }
    return !m_bActive;
}

// Create a virtual machine for everything in this resource
bool CResource::CreateVM ( void )
{
    // Create the virtual machine
    if ( m_pVM == NULL )
        m_pVM = g_pGame->GetLuaManager ()->CreateVirtualMachine ( this );

    if ( m_pVM )
    {
        m_pVM->SetScriptName ( m_strResourceName.c_str () );
        return true;
    }

    return false;
}

bool CResource::DestroyVM ( void )
{
    // Remove all player keybinds on this VM
    list < CPlayer* > ::const_iterator iter = g_pGame->GetPlayerManager ()->IterBegin ();
    for ( ; iter != g_pGame->GetPlayerManager ()->IterEnd () ; iter++ )
    {
        CKeyBinds* pBinds = (*iter)->GetKeyBinds ();
        if ( pBinds )
            pBinds->RemoveAllKeys ( m_pVM );
    }

    // Delete the events on this VM
    m_pRootElement->DeleteEvents ( m_pVM, true );

    // Delete the virtual machine
    g_pGame->GetLuaManager ()->RemoveVirtualMachine ( m_pVM );
    m_pVM = NULL;
    return true;
}

void CResource::DisplayInfo ( void ) // duplicated for HTML
{
    CLogger::LogPrintf ( "== Details for resource '%s' ==\n", m_strResourceName.c_str () );
    if ( m_bActive )
    {
        // count the number of elements
        unsigned int uiMapElementCount = 0;
        unsigned int uiScriptElementCount = 0;
        list < CElementGroup* > ::iterator itere = m_elementGroups.begin ();
        for ( ; itere != m_elementGroups.end (); itere++ )
        {
            uiScriptElementCount += (*itere)->GetCount();
        }

        list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
        for ( ; iterf != m_resourceFiles.end (); iterf++ )
        {
            if ( (*iterf)->GetType() == CResourceFile::RESOURCE_FILE_TYPE_MAP )
            {
                CResourceMapItem * map = (CResourceMapItem *)(*iterf);
                CElementGroup * group = map->GetElementGroup();
                if ( group )
                    uiMapElementCount += group->GetCount();
            }
        }

        //CLogger::LogPrintf ( "Author: %s  Version: %d  Map elements: %d  Script elements: %d\n", m_szAuthor, m_uiVersion, uiMapElementCount, uiScriptElementCount );
    }
    //else
        //CLogger::LogPrintf ( "Author: %s  Version: %d\n", m_szAuthor, m_uiVersion );

    if ( m_bActive )
    {
        CLogger::LogPrintf ( "Status: Running    Dependents: %d\n", m_dependents.size() );
        list < CResource* > ::iterator iterr = m_dependents.begin ();
        for ( ; iterr != m_dependents.end (); iterr++ )
        {
            CLogger::LogPrintf ( "  %s\n", (*iterr)->GetName().c_str () );
        }
    }
    else if ( m_bLoaded )
        CLogger::LogPrintf ( "Status: Stopped\n" );
    else if ( !m_strCircularInclude.empty () )
    {
        CLogger::LogPrintf ( "Status: Circular include error: %s\n", m_strCircularInclude.c_str () );
    }
    else
        CLogger::LogPrintf ( "Status: Failed to load\n" );

    CLogger::LogPrintf ( "Included resources: %d\n", m_includedResources.size() );

    list < CIncludedResources* > ::iterator iterr = m_includedResources.begin ();
    for ( ; iterr != m_includedResources.end (); iterr++ )
    {
        CIncludedResources * includedResource = (*iterr);
        if ( includedResource->DoesExist() )
        {
            if ( includedResource->GetResource()->IsLoaded() )
                CLogger::LogPrintf ( "  %s .. OK\n", includedResource->GetName().c_str () );
            else
                CLogger::LogPrintf ( "  %s .. FAILED TO LOAD\n", includedResource->GetName().c_str () );
        }
        else
        {
            if ( includedResource->IsBadVersion() )
            {
                CLogger::LogPrintf ( "  %s .. BAD VERSION (not between %d and %d)\n", includedResource->GetMinimumVersion(), includedResource->GetMaximumVersion() );
            }
            else
            {
                CLogger::LogPrintf ( "  %s .. NOT FOUND\n", includedResource->GetName().c_str () );
            }
        }
    }

    CLogger::LogPrintf ( "Files: %d\n", m_resourceFiles.size() );
    CLogger::LogPrintf ( "== End ==\n" );
}

char * CResource::DisplayInfoHTML ( char * info, size_t length )
{
    #define LAZYNESS counter += _snprintf ( info + counter, length - counter,

    size_t counter = 0;
    counter = _snprintf ( info, length, "<font face='tahoma,arial' size=-1><h2>Details for resource '%s'</h2><br>\n", m_strResourceName.c_str () );
    if ( m_bActive )
    {
        // count the number of elements
        unsigned int uiMapElementCount = 0;
        unsigned int uiScriptElementCount = 0;
        list < CElementGroup* > ::iterator itere = m_elementGroups.begin ();
        for ( ; itere != m_elementGroups.end (); itere++ )
        {
            uiScriptElementCount += (*itere)->GetCount();
        }

        list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
        for ( ; iterf != m_resourceFiles.end (); iterf++ )
        {
            if ( (*iterf)->GetType() == CResourceFile::RESOURCE_FILE_TYPE_MAP )
            {
                CResourceMapItem * map = (CResourceMapItem *)(*iterf);
                CElementGroup * group = map->GetElementGroup();
                if ( group )
                    uiMapElementCount += group->GetCount();
            }
        }

        //LAZYNESS "Author: %s<BR>Version: %d<BR>Map elements: %d<BR>Script elements: %d<BR>", m_szAuthor, m_uiVersion, uiMapElementCount, uiScriptElementCount );
        LAZYNESS "Loaded: %s<BR>", asctime(localtime((const time_t *)&m_timeLoaded)) );
        LAZYNESS "Started: %s<BR>", asctime(localtime((const time_t *)&m_timeStarted)) );
    }
    else
    {
        //LAZYNESS "Author: %s<BR>Version: %d<BR>Loaded %s<BR>", m_szAuthor, m_uiVersion, asctime(localtime((const time_t *)&m_timeLoaded)) );
        if ( m_timeStarted != 0 )
            LAZYNESS "Last ran: %s<BR>", asctime(localtime((const time_t *)&m_timeStarted)) );
        LAZYNESS "Last ran: never<BR>" );
    }

    if ( m_bActive )
    {
        LAZYNESS "Status: Running<BR>" );
        LAZYNESS "Dependents: %d<br>", m_dependents.size() );
        if ( m_dependents.size() != 0 )
        {
            list < CResource* > ::iterator iterr = m_dependents.begin ();
            for ( ; iterr != m_dependents.end (); iterr++ )
            {
                LAZYNESS " <li><a href='/resources/%s/' style='color:black;'>%s</a> <sup><font size=-2><a href='/resource-info/%s/' style='color:black;'>Info</a></font></sup>", (*iterr)->GetName().c_str (), (*iterr)->GetName().c_str (), (*iterr)->GetName().c_str () );
            }
            LAZYNESS "<BR>" );
        }
    }
    else if ( m_bLoaded )
        LAZYNESS "Status: Stopped<BR>" );
    else if ( !m_strCircularInclude.empty () )
    {
        LAZYNESS "Status: Circular include error: %s<BR>", m_strCircularInclude.c_str () );
    }
    else
        LAZYNESS "Status: Failed to load<BR>" );

    LAZYNESS "Included resources: %d<BR>", m_includedResources.size() );

    list < CIncludedResources* > ::iterator iterr = m_includedResources.begin ();
    for ( ; iterr != m_includedResources.end (); iterr++ )
    {
        CIncludedResources * includedResource = (*iterr);
        LAZYNESS " <li><a href='/resources/%s/' style='color:black;'>%s</a> <sup><font size=-2><a href='/resource-info/%s/' style='color:black;'>Info</a></font></sup>", (*iterr)->GetName().c_str (), (*iterr)->GetName().c_str (), (*iterr)->GetName().c_str () );
        if ( includedResource->DoesExist() )
        {
            if ( includedResource->GetResource()->IsLoaded() )

                LAZYNESS " .. OK<BR>" );
            else
                LAZYNESS " .. FAILED TO LOAD<BR>" );
        }
        else
        {
            if ( includedResource->IsBadVersion() )
            {
                LAZYNESS " .. BAD VERSION (not between %d and %d)<BR>", includedResource->GetMinimumVersion(), includedResource->GetMaximumVersion() );
            }
            else
            {
                LAZYNESS " .. NOT FOUND<BR>");
            }
        }
    }

    LAZYNESS "Exported functions: %d<br>", m_exportedFunctions.size() );
    if ( m_exportedFunctions.size() != 0 )
    {
        list < CExportedFunction* > ::iterator iterf = m_exportedFunctions.begin ();
        for ( ; iterf != m_exportedFunctions.end (); iterf++ )
        {
            if ( (*iterf)->IsHTTPAccessible() )
            {
                LAZYNESS "<li> <a href='/call/%s/%s?' style='color: black;'>%s</a>", m_strResourceName.c_str (), (*iterf)->GetFunctionName ().c_str (), (*iterf)->GetFunctionName ().c_str () );
            }
            else
            {
                LAZYNESS "<li> %s", (*iterf)->GetFunctionName ().c_str () );
            }

        }
        LAZYNESS "<BR>" );
    }


    LAZYNESS "Files: %d<br>", m_resourceFiles.size() );
    if ( m_exportedFunctions.size() != 0 )
    {
        list < CResourceFile* > ::iterator iterf = m_resourceFiles.begin ();
        for ( ; iterf != m_resourceFiles.end (); iterf++ )
        {
            if ( (*iterf)->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML )
            {
                if ( ((CResourceHTMLItem *)(*iterf))->IsDefaultPage() )
                    LAZYNESS "<b>" );

                LAZYNESS "<li> <a href='/resources/%s/%s' style='color:black;'>%s</a>", m_strResourceName.c_str (), (*iterf)->GetName(), (*iterf)->GetName() );

                if ( ((CResourceHTMLItem *)(*iterf))->IsDefaultPage() )
                    LAZYNESS "</b>" );

            }
            else
                LAZYNESS "<li> %s", (*iterf)->GetName() );
        }
        LAZYNESS "<BR>" );
    }
    LAZYNESS "<BR><BR><a href='/resources/' style='color:black' />Resource List</a>" );
    return info;
}

bool CResource::ExtractFile ( const char * szFilename )
{
    if ( DoesFileExistInZip ( szFilename ) )
    {
        // Load the zip file if it isn't already loaded. Return false if it can't be loaded.
        if ( !m_zipfile )
            m_zipfile = unzOpen(m_strResourceZip.c_str ());

        if ( !m_zipfile ) return false;

        unzLocateFile ( m_zipfile, szFilename, false );

        // if the file exists, we'll be at the right place to extract from now
        int opt_extract_without_path = 0;
        int opt_overwrite = 1;

        int ires = do_extract_currentfile(m_zipfile,&opt_extract_without_path,
                                        &opt_overwrite,
                                        NULL, m_strResourceCachePath.c_str ());

        if ( ires == UNZ_OK)
            return true;
    }
    return false;
}

bool CResource::DoesFileExistInZip ( const char * szFilename )
{
    if ( !m_zipfile )
        m_zipfile = unzOpen(m_strResourceZip.c_str ());
    bool bRes = false;
    if ( m_zipfile )
    {
        bRes = unzLocateFile ( m_zipfile, szFilename, false ) != UNZ_END_OF_LIST_OF_FILE;
        unzClose ( m_zipfile );
        m_zipfile = NULL;
    }
    return bRes;
}

// gets the path of the file specified, may extract it from the zip
bool CResource::GetFilePath ( const char * szFilename, string& strPath )
{
    // first, check the resource folder, then check the zip file
    strPath = m_strResourceDirectoryPath + szFilename;
    FILE * temp = fopen ( strPath.c_str (), "r" );
    if ( temp )
    {
        fclose ( temp );
#ifdef RESOURCE_DEBUG_MESSAGES
        CLogger::LogPrintf("%s is in resource folder\n", szFilename );
#endif
        return true;
    }

    if ( !m_zipfile )
        m_zipfile = unzOpen(m_strResourceZip.c_str ());
    if ( m_zipfile )
    {
        if ( unzLocateFile ( m_zipfile, szFilename, false ) != UNZ_END_OF_LIST_OF_FILE )
        {
            strPath = m_strResourceCachePath + szFilename;
            temp = fopen ( strPath.c_str (), "r" );
            if ( temp )
            {
                fclose ( temp );

                // we've already got a cached copy of this file, check its still the same
                unsigned long ulFileInZipCRC = get_current_file_crc ( m_zipfile );
                unsigned long ulFileOnDiskCRC = CRCGenerator::GetCRCFromFile ( strPath.c_str () );

                if ( ulFileInZipCRC == ulFileOnDiskCRC )
                {
#ifdef RESOURCE_DEBUG_MESSAGES
                    CLogger::LogPrintf("Up to date %s already extracted from zip\n", szFilename );
#endif
                    unzClose ( m_zipfile );
                    m_zipfile = NULL;
                    return true; // we've already extracted EXACTLY this file before
                }
                else
                {
#ifdef RESOURCE_DEBUG_MESSAGES
                    CLogger::LogPrintf("Old version of %s already extracted, extracting again\n", szFilename );
#endif
                }
            }
            else
            {
#ifdef RESOURCE_DEBUG_MESSAGES
                CLogger::LogPrintf("Extracting %s from zip\n", szFilename );
#endif
            }

            // we've never extracted this EXACT file (maybe an old version), so do it again
            ExtractFile ( szFilename );
            unzClose ( m_zipfile );
            m_zipfile = NULL;
            return true;
        }
    }
#ifdef RESOURCE_DEBUG_MESSAGES
    CLogger::LogPrintf("Can't find %s in zip or in folder\n", szFilename );
#endif
    return false;
}


bool CResource::ReadIncludedHTML ( CXMLNode * root )
{
    int i = 0;
    bool bFoundDefault = false;
    CResourceHTMLItem* firstHTML = NULL;

    // Go trough each html subnode of the root
    for ( CXMLNode * inc = root->FindSubNode("html", i);
        inc != NULL; inc = root->FindSubNode("html", ++i ) )
    {
        // Get the attributelist
        CXMLAttributes * attributes = &(inc->GetAttributes());
        if ( attributes )
        {
            // See if this is the default page (default attribute)
            bool bIsDefault = false;
            CXMLAttribute * defaultfile = attributes->Find("default");
            if ( defaultfile )
            {
                const char * szDefault = defaultfile->GetValue ().c_str ();
                if ( stricmp ( szDefault, "yes" ) == 0 || stricmp ( szDefault, "true" ) == 0 )
                    bIsDefault = true;
                else
                    bIsDefault = false;
            }

            // See if this is a raw file (like an image)
            bool bIsRaw = false;
            CXMLAttribute * raw = attributes->Find("raw");
            if ( raw )
            {
                const char *szRaw = raw->GetValue ().c_str ();
                if ( stricmp ( szRaw, "yes" ) == 0 || stricmp ( szRaw, "true" ) == 0 )
                    bIsRaw = true;
                else
                    bIsRaw = false;
            }

            // See if this is a restricted file
            bool bIsRestricted = false;
            CXMLAttribute * restricted = attributes->Find("restricted");
            if ( restricted )
            {
                const char *szRestricted = restricted->GetValue ().c_str ();
                if ( stricmp ( szRestricted, "yes" ) == 0 || stricmp ( szRestricted, "true" ) == 0 )
                    bIsRestricted = true;
                else
                    bIsRestricted = false;
            }

            // Find the source attribute (the name of the file)
            CXMLAttribute * src = attributes->Find("src");
            if ( src )
            {
                // If we found it grab the value
                string strFilename = src->GetValue ();
                string strFullFilename;
                ReplaceSlashes ( strFilename );

                // Try to find the file
                if ( IsValidFilePath ( strFilename.c_str () ) && GetFilePath ( strFilename.c_str (), strFullFilename ) )
                {
                    // This one is supposed to be default, but there's already a default page
                    if ( bFoundDefault && bIsDefault )
                    {
                        CLogger::LogPrintf ( "Only one html item can be default per resource, ignoring %s in %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                        bIsDefault = false;
                    }

                    // If this is supposed to be default, we've now found our default page
                    if ( bIsDefault )
                        bFoundDefault = true;

                    // Create a new resource HTML file and add it to the list
                    CResourceFile * afile = new CResourceHTMLItem ( this, strFilename.c_str (), strFullFilename.c_str (), attributes, bIsDefault, bIsRaw, bIsRestricted );
                    m_resourceFiles.push_back ( afile );

                    // This is the first HTML file? Remember it
                    if ( firstHTML == NULL )
                        firstHTML = (CResourceHTMLItem*)afile;
                }
                else
                {
                    char szBuffer[512];
                    _snprintf ( szBuffer, 511, "Couldn't find html %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    m_strFailureReason = szBuffer;
                    CLogger::ErrorPrintf ( "Couldn't find html %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    return false;
                }
            }
            else
            {
                CLogger::LogPrintf ( "WARNING: Missing 'src' attribute from 'html' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
            }
        }
    }

    // If we haven't found a default html page, we put the first HTML as the default
    if ( firstHTML && !bFoundDefault )
        firstHTML->SetDefaultPage ( true );
    return true;
}

bool CResource::ReadIncludedConfigs ( CXMLNode * root )
{
    int i = 0;

    // Loop through the list of included configs
    for ( CXMLNode * inc = root->FindSubNode("config", i);
        inc != NULL; inc = root->FindSubNode("config", ++i ) )
    {
        // Grab the list over attributes
        CXMLAttributes * attributes = &(inc->GetAttributes());
        if ( attributes )
        {
            // Find the type attribute (server / client)
            int iType = CResourceScriptItem::RESOURCE_FILE_TYPE_CONFIG;
            CXMLAttribute * type = attributes->Find("type");
            if ( type )
            {
                const char *szType = type->GetValue ().c_str ();
                if ( stricmp ( szType, "server" ) == 0 )
                    iType = CResourceScriptItem::RESOURCE_FILE_TYPE_CONFIG;
                else if ( stricmp ( szType, "client" ) == 0 )
                    iType = CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_CONFIG;
                else
                    CLogger::LogPrintf ( "Unknown config type specified in %s. Assuming 'server'\n", m_strResourceName.c_str () );
            }

            // Find the source (file path)
            CXMLAttribute * src = attributes->Find("src");
            if ( src )
            {
                // Grab the filename
                string strFilename = src->GetValue ();
                string strFullFilename;
                ReplaceSlashes ( strFilename );

                // Extract / grab the filepath
                if ( IsValidFilePath ( strFilename.c_str () ) && GetFilePath ( strFilename.c_str (), strFullFilename ) )
                {
                    // Create it and push it to the list over resource files. Depending on if it's client or server type
                    if ( iType == CResourceScriptItem::RESOURCE_FILE_TYPE_CONFIG )
                        m_resourceFiles.push_back ( new CResourceConfigItem ( this, strFilename.c_str (), strFullFilename.c_str (), attributes ) );
                    else if ( iType == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_CONFIG )
                        m_resourceFiles.push_back ( new CResourceClientConfigItem ( this, strFilename.c_str (), strFullFilename.c_str (), attributes ) );
                }
                else
                {
                    char szBuffer[512];
                    _snprintf ( szBuffer, 511, "Couldn't find config %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    m_strFailureReason = szBuffer;
                    CLogger::ErrorPrintf ( "Couldn't find config %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    return false;
                }
            }
            else
            {
                CLogger::LogPrintf ( "WARNING: Missing 'src' attribute from 'config' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
            }
        }
    }

    return true;
}

bool CResource::ReadIncludedFiles ( CXMLNode * root )
{
    int i = 0;

    // Loop through the included files
    for ( CXMLNode * inc = root->FindSubNode("file", i);
        inc != NULL; inc = root->FindSubNode("file", ++i ) )
    {
        // Grab the attributelist
        CXMLAttributes * attributes = &(inc->GetAttributes ());
        if ( attributes )
        {
            // Grab the filepath attribute
            CXMLAttribute * src = attributes->Find("src");
            if ( src )
            {
                // Grab the value
                string strFilename = src->GetValue ();
                string strFullFilename;
                ReplaceSlashes ( strFilename );

                // Create a new resourcefile item
                if ( IsValidFilePath ( strFilename.c_str () ) && GetFilePath ( strFilename.c_str (), strFullFilename ) )
                    m_resourceFiles.push_back ( new CResourceClientFileItem ( this, strFilename.c_str (), strFullFilename.c_str (), attributes ) );
                else
                {
                    char szBuffer[512];
                    _snprintf ( szBuffer, 511, "Couldn't find file %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    m_strFailureReason = szBuffer;
                    CLogger::ErrorPrintf ( "Couldn't find file %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    return false;
                }
            }
            else
            {
                CLogger::LogPrintf ( "WARNING: Missing 'src' attribute from 'file' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
            }
        }
    }

    return true;
}

bool CResource::ReadIncludedExports ( CXMLNode * root )
{
    int i = 0;

    // Read our exportlist
    for ( CXMLNode * inc = root->FindSubNode("export", i);
        inc != NULL; inc = root->FindSubNode("export", ++i ) )
    {
        // Grab the attributelist for this node
        CXMLAttributes * attributes = &(inc->GetAttributes ());
        if ( attributes )
        {
            // Find the access attribute and grab its value
            std::string strAccess;
            CXMLAttribute * access = attributes->Find("access");
            if ( access )
            {
                strAccess = access->GetValue ();
            }

            // See if the http attribute is true or false
            bool bHTTP = false;
            CXMLAttribute * http = attributes->Find("http");
            if ( http )
            {
                const char *szHttp = http->GetValue ().c_str ();
                if ( stricmp ( szHttp, "yes" ) == 0 || stricmp ( szHttp, "true" ) == 0 )
                    bHTTP = true;
                else
                    bHTTP = false;
            }

            // See if the restricted attribute is true or false
            bool bRestricted = false;
            CXMLAttribute * restricted = attributes->Find("restricted");
            if ( restricted )
            {
                const char *szRestricted = restricted->GetValue ().c_str ();
                if ( stricmp ( szRestricted, "yes" ) == 0 || stricmp ( szRestricted, "true" ) == 0 )
                    bRestricted = true;
                else
                    bRestricted = false;
            }

            // Find the type attribute
            CExportedFunction::eExportedFunctionType ucType = CExportedFunction::EXPORTED_FUNCTION_TYPE_SERVER;
            CXMLAttribute * type = attributes->Find("type");
            if ( type )
            {
                // Grab the type. Client or server
                const char *szType = type->GetValue ().c_str ();
                if ( stricmp ( szType, "server" ) == 0 )
                {
                    ucType = CExportedFunction::EXPORTED_FUNCTION_TYPE_SERVER;
                }
                else if ( stricmp ( szType, "client" ) == 0 )
                {
                    ucType = CExportedFunction::EXPORTED_FUNCTION_TYPE_CLIENT;
                }
                else
                {
                    CLogger::LogPrintf ( "Unknown exported function type specified in %s. Assuming 'server'\n", m_strResourceName.c_str () );
                }
            }

            // Grab the functionname attribute
            CXMLAttribute * function = attributes->Find("function");
            if ( function )
            {
                // Grab the functionname from the attribute
                std::string strFunction = function->GetValue ();

                // Add it to the list if it wasn't zero long. Otherwize show a warning
                if ( !strFunction.empty () )
                {
                    m_exportedFunctions.push_back ( new CExportedFunction ( strFunction.c_str (), strAccess.c_str (), bHTTP, ucType, bRestricted ) );
                   /* if ( bHTTP && ucType == CExportedFunction::EXPORTED_FUNCTION_TYPE_SERVER )
                    {
                        char szExportCall[512];
                        __snprintf ( szExportCall, 511, "call/%s", szFunction );
                        szExportCall[511] = '\0';
                        this->RegisterEHS ( this, szExportCall );
                    }*/
                }
                else
                {
                    CLogger::ErrorPrintf ( "WARNING: Empty 'function' attribute of 'export' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
                }
            }
            else
            {
                CLogger::LogPrintf ( "WARNING: Missing 'function' attribute from 'export' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
            }
        }
    }

    return true;
}

bool CResource::ReadIncludedScripts ( CXMLNode * root )
{
    int i = 0;

    // Loop through all script nodes under the root
    for ( CXMLNode * inc = root->FindSubNode("script", i);
        inc != NULL; inc = root->FindSubNode("script", ++i ) )
    {
        // Grab the attribute list for this node
        CXMLAttributes * attributes = &(inc->GetAttributes ());
        if ( attributes )
        {
            // Grab the type attribute (server / client)
            int iType = CResourceScriptItem::RESOURCE_FILE_TYPE_SCRIPT;
            CXMLAttribute * type = attributes->Find("type");
            if ( type )
            {
                // Grab the text
                const char *szType = type->GetValue ().c_str ();

                // Check its content (server or client) and set the type accordingly
                if ( stricmp ( szType, "server" ) == 0 )
                    iType = CResourceScriptItem::RESOURCE_FILE_TYPE_SCRIPT;
                else if ( stricmp ( szType, "client" ) == 0 )
                    iType = CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_SCRIPT;
                else
                    CLogger::LogPrintf ( "Unknown script type specified in %s. Assuming 'server'\n", m_strResourceName.c_str () );
            }

            // Grab the source attribute
            CXMLAttribute * src = attributes->Find("src");
            if ( src )
            {
                // Grab the source value from the attribute
                string strFilename = src->GetValue ();
                string strFullFilename;
                ReplaceSlashes ( strFilename );

                // Extract / get the filepath of the file
                if ( IsValidFilePath ( strFilename.c_str () ) && GetFilePath ( strFilename.c_str (), strFullFilename ) )
                {
                    // Create it depending on the type (clietn or server) and add it to the list over resource files
                    if ( iType == CResourceScriptItem::RESOURCE_FILE_TYPE_SCRIPT )
                        m_resourceFiles.push_back ( new CResourceScriptItem ( this, strFilename.c_str (), strFullFilename.c_str (), attributes ) );
                    else if ( iType == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_SCRIPT )
                        m_resourceFiles.push_back ( new CResourceClientScriptItem ( this, strFilename.c_str (), strFullFilename.c_str (), attributes ) );
                }
                else
                {
                    char szBuffer[512];
                    _snprintf ( szBuffer, 511, "Couldn't find script %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    m_strFailureReason = szBuffer;
                    CLogger::ErrorPrintf ( "Couldn't find script %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    return false;
                }
            }
            else
            {
                CLogger::LogPrintf ( "WARNING: Missing 'src' attribute from 'script' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
            }
        }
    }

    return true;
}


bool CResource::ReadIncludedMaps ( CXMLNode * root )
{
    int i = 0;

    // Loop through the map nodes under the root
    for ( CXMLNode * inc = root->FindSubNode("map", i);
        inc != NULL; inc = root->FindSubNode("map", ++i ) )
    {
        // Grab the attributelist of this node
        CXMLAttributes * attributes = &(inc->GetAttributes());
        if ( attributes )
        {
            // Grab the dimension attribute
            int iDimension = 0;
            CXMLAttribute * dimension = attributes->Find("dimension");
            if ( dimension )
            {
                iDimension = atoi( dimension->GetValue ().c_str () );
                if ( iDimension < 0 || iDimension > 65535 )
                    iDimension = 0;
            }

            // Grab the source node
            CXMLAttribute * src = attributes->Find("src");
            if ( src )
            {
                // Grab the source text from the node
                string strFilename = src->GetValue ();
                string strFullFilename;
                ReplaceSlashes ( strFilename );
                // Grab the file (evt extract it). Make a map item resource and put it into the resourcefiles list
                if ( IsValidFilePath ( strFilename.c_str () ) && GetFilePath ( strFilename.c_str (), strFullFilename ) )
                    m_resourceFiles.push_back ( new CResourceMapItem ( this, strFilename.c_str (), strFullFilename.c_str (), attributes, iDimension ) );
                else
                {
                    char szBuffer[512];
                    _snprintf ( szBuffer, 511, "Couldn't find map %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    m_strFailureReason = szBuffer;
                    CLogger::ErrorPrintf ( "Couldn't find map %s for resource %s\n", strFilename.c_str (), m_strResourceName.c_str () );
                    return false;
                }
            }
            else
            {
                CLogger::LogPrintf ( "WARNING: Missing 'src' attribute from 'map' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
            }
        }
    }

    return true;
}

bool CResource::GetDefaultSetting ( const char* szName, char* szValue, size_t sizeBuffer )
{
    // Look through its subnodes for settings with a matching name
    unsigned int uiCount = m_pNodeSettings->GetSubNodeCount ();
    unsigned int i = 0;
    std::string strTagName;
    for ( ; i < uiCount; i++ )
    {
        // Grab its tag name
        CXMLNode* pTemp = m_pNodeSettings->GetSubNode ( i );
        strTagName = pTemp->GetTagName ();

        // Check that its "setting"
        if ( stricmp ( strTagName.c_str (), "setting" ) == 0 )
        {
            // Grab the name attribute and compare it to the name we look for
            CXMLAttribute* pName = m_pNodeSettings->GetAttributes ().Find ( "name" );
            if ( pName && strcmp ( szName, pName->GetValue ().c_str () ) == 0 )
            {
            }
        }
    }

    return false;
}


bool CResource::SetDefaultSetting ( const char* szName, const char* szValue )
{
    return false;
}


bool CResource::RemoveDefaultSetting ( const char* szName )
{
    return false;
}


bool CResource::AddMapFile ( const char* szName, const char* szFullFilename, int iDimension )
{
    // We can only do this if loaded
    if ( m_bLoaded && !m_bResourceIsZip )
    {
        // Find the meta file path
        char szMetaPath [MAX_PATH + 1];
        _snprintf ( szMetaPath, MAX_PATH, "%s%s", m_strResourceDirectoryPath.c_str (), "meta.xml" );

        // Load the meta file
        CXMLFile* metaFile = g_pServerInterface->GetXML ()->CreateXML ( szMetaPath );
        if ( metaFile )
        {
            // Parse it
            if ( metaFile->Parse () )
            {
                // Grab its rootnode
                CXMLNode* pRootNode = metaFile->GetRootNode ();
                if ( pRootNode )
                {
                    // Create a new map subnode
                    CXMLNode* pMapNode = pRootNode->CreateSubNode ( "map" );
                    if ( pMapNode )
                    {
                        // Set the src and dimension attributes
                        pMapNode->GetAttributes ().Create ( "src" )->SetValue ( szName );
                        pMapNode->GetAttributes ().Create ( "dimension" )->SetValue ( iDimension );

                        // If we're loaded, add it to the resourcefiles too
                        m_resourceFiles.push_back ( new CResourceMapItem ( this, szName, szFullFilename, &pMapNode->GetAttributes (), iDimension ) );

                        // Success, write and destroy XML
                        metaFile->Write ();
                        delete metaFile;
                        return true;
                    }
                }
            }

            // Destroy it
            delete metaFile;
        }
    }

    return false;
}


bool CResource::AddConfigFile ( const char* szName, const char* szFullFilepath, int iType )
{
    // We can only do this if loaded
    if ( m_bLoaded && !m_bResourceIsZip )
    {
        // Find the meta file path
        char szMetaPath [MAX_PATH + 1];
        _snprintf (szMetaPath, MAX_PATH, "%s%s", m_strResourceDirectoryPath.c_str (), "meta.xml" );

        // Load the meta file
        CXMLFile* metaFile = g_pServerInterface->GetXML ()->CreateXML ( szMetaPath );
        if ( metaFile )
        {
            // Parse it
            if ( metaFile->Parse () )
            {
                // Grab its rootnode
                CXMLNode* pRootNode = metaFile->GetRootNode ();
                if ( pRootNode )
                {
                    // Create a new map subnode
                    CXMLNode* pMapNode = pRootNode->CreateSubNode ( "config" );
                    if ( pMapNode )
                    {
                        // Set the src attributes
                        pMapNode->GetAttributes ().Create ( "src" )->SetValue ( szName );

                        // Also set the type attribute (server or client)
                        if ( iType == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_CONFIG )
                            pMapNode->GetAttributes ().Create ( "type" )->SetValue ( "client" );
                        else if ( iType == CResourceScriptItem::RESOURCE_FILE_TYPE_CONFIG )
                            pMapNode->GetAttributes ().Create ( "type" )->SetValue ( "server" );

                        // If we're loaded, add it to the resourcefiles too
                        m_resourceFiles.push_back ( new CResourceConfigItem ( this, szName, szFullFilepath, &pMapNode->GetAttributes () ) );

                        // Success, write and destroy XML
                        metaFile->Write ();
                        delete metaFile;
                        return true;
                    }
                }
            }

            // Destroy it
            delete metaFile;
        }
    }

    return false;
}


bool CResource::IncludedFileExists ( const char* szName, int iType )
{
    // Go through all our resource files
    std::list <CResourceFile*> ::iterator iter = m_resourceFiles.begin ();
    for ( ; iter != m_resourceFiles.end (); iter++ )
    {
        // Is it the required type?
        if ( ( iType == CResourceFile::RESOURCE_FILE_TYPE_NONE ) ||
             ( (*iter)->GetType () == iType ) )
        {
            // Check if the name compares equal (case independant)
            if ( stricmp ( (*iter)->GetName (), szName ) == 0 )
            {
                return true;
            }
        }
    }

    return false;
}


bool CResource::RemoveFile ( const char* szName )
{
    // We can only do this if loaded and we're not in a zip
    if ( m_bLoaded && !m_bResourceIsZip )
    {
        // Find the meta file path
        char szMetaPath [MAX_PATH + 1];
        _snprintf ( szMetaPath, MAX_PATH, "%s%s", m_strResourceDirectoryPath.c_str (), "meta.xml" );

        // Load the meta file
        CXMLFile* metaFile = g_pServerInterface->GetXML ()->CreateXML ( szMetaPath );
        if ( metaFile )
        {
            // Parse it
            if ( metaFile->Parse () )
            {
                // Grab its rootnode
                CXMLNode* pRootNode = metaFile->GetRootNode ();
                if ( pRootNode )
                {
                    int i = 0;
                    CXMLNode* pTemp;
                    CXMLNode* pNodeFound = NULL;
                    std::string strTempBuffer;

                    // Loop through the map nodes under the root
                    for ( pTemp = pRootNode->GetSubNode ( i );
                          pTemp != NULL;
                          pTemp = pRootNode->GetSubNode ( ++i ) )
                    {
                        // Grab the tag name
                        strTempBuffer = pTemp->GetTagName ();
                        if ( stricmp ( strTempBuffer.c_str (), "map" ) == 0 ||
                             stricmp ( strTempBuffer.c_str (), "config" ) == 0 ||
                             stricmp ( strTempBuffer.c_str (), "script" ) == 0 ||
                             stricmp ( strTempBuffer.c_str (), "html" ) == 0 )
                        {
                            // Grab the src attribute? Same name?
                            CXMLAttribute* pAttrib = pTemp->GetAttributes ().Find ( "src" );
                            if ( pAttrib && stricmp ( pAttrib->GetValue ().c_str (), szName ) == 0 )
                            {
                                pNodeFound = pTemp;
                                break;
                            }
                        }
                    }

                    // Found our node?
                    if ( pNodeFound )
                    {
                        // Delete the node from the XML, write it and then clean up
                        pRootNode->DeleteSubNode ( pNodeFound );

                        // Find the file in our filelist
                        CResourceFile* pFile;
                        CResourceFile* pFileFound = NULL;
                        std::list <CResourceFile*> ::iterator iter = m_resourceFiles.begin ();
                        for ( ; iter != m_resourceFiles.end (); iter++ )
                        {
                            pFile = *iter;

                            // Found a matching file? Remember its type
                            if ( stricmp ( szName, pFile->GetName () ) == 0 )
                                pFileFound = pFile;
                        }

                        // Found the resource file?
                        if ( pFileFound )
                        {
                            // Delete it from our list
                            delete pFileFound;
                            m_resourceFiles.remove ( pFileFound );
                        }
                        else
                            CLogger::LogPrintf ( "WARNING: Problems removing resource file from memory" );
                    }

                    // Delete the file
                    char szFullFilepath [MAX_PATH + 1];
                    _snprintf ( szFullFilepath, MAX_PATH, "%s%s", m_strResourceDirectoryPath.c_str (), szName );
                    if ( unlink ( szFullFilepath ) != 0 )
                        CLogger::LogPrintf ( "WARNING: Problems deleting the actual file, but was removed from resource" );

                    // Delete the metafile
                    metaFile->Write ();
                    delete metaFile;

                    // We succeeded
                    return true;
                }
            }

            // Destroy it
            delete metaFile;
        }
    }

    return false;
}


// read the included resources from the XML file and store it for future reference
bool CResource::ReadIncludedResources ( CXMLNode * root )
{
    int i = 0;

    // Loop through the included resources list
    for ( CXMLNode * inc = root->FindSubNode ( "include", i );
        inc != NULL; inc = root->FindSubNode ( "include", ++i ) )
    {
        // Grab the attributelist from this node
        CXMLAttributes& Attributes = inc->GetAttributes ();

        // Grab the minversion attribute (minimum version the included resource needs to be)
        SVersion svMinVersion;
        SVersion svMaxVersion;
        svMinVersion.m_uiMajor = 0;
        svMinVersion.m_uiMinor = 0;
        svMinVersion.m_uiRevision = 0;
        svMaxVersion.m_uiMajor = 0;
        svMaxVersion.m_uiMinor = 0;
        svMaxVersion.m_uiRevision = 0;
        unsigned int uiMinVersion = 0;
        unsigned int uiMaxVersion = 0;
        CXMLAttribute * minversion = Attributes.Find ( "minversion" ); // optional
        if ( minversion )
        {
            /* TODO: Convert this code into std::string */
            std::string strMinversion = minversion->GetValue ();
            char szMinversion[MAX_RESOURCE_VERSION_LENGTH];
            strncpy ( szMinversion, strMinversion.c_str (), MAX_RESOURCE_VERSION_LENGTH - 1 );
            uiMinVersion = atoi(szMinversion);
            char* sz1 = new char;
            char* sz2 = new char;
            char* sz3 = new char;
            if ( strlen ( szMinversion ) > 0 )
            {
                sz1 = strtok ( szMinversion, " " );
                if ( sz1 )
                    svMinVersion.m_uiMajor = atoi ( sz1 );
            }
            if ( strlen ( szMinversion ) > ( 1 + strlen ( sz1 ) ) )
            {
                sz2 = strtok ( NULL, " " );
                if ( sz2 )
                    svMinVersion.m_uiMinor = atoi ( sz2 );
            }
            if ( strlen ( szMinversion ) > ( 2 + strlen ( sz1 ) + strlen ( sz2 ) ) )
            {
                sz3 = strtok ( NULL, " " );
                if ( sz3 )
                    svMinVersion.m_uiRevision = atoi ( sz3 );
            }
        }

        // Grab the maxversion attribute (maximum version the included resource needs to be)
        CXMLAttribute * maxversion = Attributes.Find ( "maxversion" ); //optional
        if ( maxversion )
        {
            /* TODO: Convert this code into std::string */
            std::string strMaxversion = maxversion->GetValue ();
            char szMaxversion[MAX_RESOURCE_VERSION_LENGTH];
            strncpy ( szMaxversion, strMaxversion.c_str (), MAX_RESOURCE_VERSION_LENGTH - 1 );

            uiMaxVersion = atoi(szMaxversion);
            char* sz1 = new char;
            char* sz2 = new char;
            char* sz3 = new char;
            if ( strlen ( szMaxversion ) > 0 )
            {
                sz1 = strtok ( szMaxversion, " " );
                if ( sz1 )
                    svMaxVersion.m_uiMajor = atoi ( sz1 );
            }
            if ( strlen ( szMaxversion ) > ( 1 + strlen ( sz1 ) ) )
            {
                sz2 = strtok ( NULL, " " );
                if ( sz2 )
                    svMaxVersion.m_uiMinor = atoi ( sz2 );
            }
            if ( strlen ( szMaxversion ) > ( 2 + strlen ( sz1 ) + strlen ( sz2 ) ) )
            {
                sz3 = strtok ( NULL, " " );
                if ( sz3 )
                    svMaxVersion.m_uiRevision = atoi ( sz3 );
            }
        }

        // Grab the resource attribute
        CXMLAttribute * src = Attributes.Find ( "resource" );
        if ( src )
        {
            // Grab the value and add an included resource
            std::string strIncludedResource = src->GetValue ();

            // If there's text in the node
            if ( !strIncludedResource.empty () )
                m_includedResources.push_back ( new CIncludedResources ( m_resourceManager, strIncludedResource.c_str (), svMinVersion, svMaxVersion, uiMinVersion, uiMaxVersion, this ) );
            else
                CLogger::LogPrintf ( "WARNING: Empty 'resource' attribute from 'include' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
        }
        else
        {
            CLogger::LogPrintf ( "WARNING: Missing 'resource' attribute from 'include' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str () );
        }
    }

    return true;
}

bool CResource::LinkToIncludedResources ( void )
{
    m_bLinked = true;

    // Loop through the list of included resources
    list < CIncludedResources* > ::iterator iterr = m_includedResources.begin ();
    for ( ; iterr != m_includedResources.end (); iterr++ )
    {
        // If we failed creating the link
        if ( !(*iterr)->CreateLink() )
        {
            m_bLinked = false;
#ifdef RESOURCE_DEBUG_MESSAGES
            CLogger::LogPrintf ( "  Links to %s .. FAILED\n", (*iterr)->GetName().c_str () );
#endif
        }
#ifdef RESOURCE_DEBUG_MESSAGES
        else
            CLogger::LogPrintf ( "  Links to %s .. OK\n", (*iterr)->GetName().c_str () );
#endif
    }
    m_bLoaded = m_bLinked;
    return m_bLinked;
}

bool CResource::CheckIfStartable ( void )
{
    // return straight away if we know we've already got a circular include, otherwise
    // it spams it every few seconds
    if ( m_bLoaded == false ) return false;

    // Check that the included resources aren't circular
    m_strCircularInclude = "";
    vector < CResource * > vecCircular;
    if ( IsIncludedResourceRecursive ( &vecCircular ) )
    {
        char szOldString [ 512 ];
        char szTrail [ 512 ] = "";

        // Make a string telling what the circular path is
        vector < CResource* > ::iterator iterr = vecCircular.begin ();
        for ( ; iterr != vecCircular.end (); iterr++ )
        {
            CResource * resc = (*iterr);
            if ( resc )
            {
                strcpy ( szOldString, szTrail );
                _snprintf ( szTrail, 510, "-> %s %s", resc->GetName ().c_str (), szOldString );
            }
        }

        // Remember why we failed and return false
        char szBuffer[512] = {0};
        _snprintf ( szBuffer, 511, "%s %s", m_strResourceName.c_str (), szTrail );
        m_strCircularInclude = szBuffer;
        _snprintf ( szBuffer, 511, "Circular include error: %s", m_strCircularInclude.c_str () );
        m_strFailureReason = szBuffer;
        //CLogger::LogPrintf ( "%s\n", m_szCircularInclude );
        m_bLoaded = false;
        return false;
    }

    // Check if all the included resources are startable
    list < CIncludedResources* > ::iterator iterr = m_includedResources.begin ();
    for ( ; iterr != m_includedResources.end (); iterr++ )
    {
#ifdef RESOURCE_DEBUG_MESSAGES
        CLogger::LogPrintf ( "  Checking if %s is loaded\n", (*iterr)->GetName().c_str () );
#endif
        CResource* pResource = (*iterr)->GetResource();
        if ( !pResource || !pResource->CheckIfStartable() )
        {
            m_bLoaded = false;
            return false;
        }
    }

    return true;
}

//bool CResource::LoadResource ( char * szResourceName )
//{
//    if ( !IsChildResource(szResourceName) ) // don't bother trying to load it twice
//    {
//        if ( !IsDependentResourceRecursive(szResourceName) )
//        {
//            CResource * resource = m_resourceManager->Load ( szResourceName, this );
//            if ( resource )
//                m_children.push_back ( resource );
//            else
//                return false;
//        }
//        else
//        {
//            CLogger::LogPrintf ( "WARNING: Circular include, ignored include of %s by resource %s", szResourceName, m_szResourceName );
//            return true;
//        }
//    }
//
//    return true;
//}

//bool CResource::LoadResourceItems ( void )
//{
//    if ( !IsActive() ) // so we don't load this twice
//    {
//        // Go through all our included resources and make sure they're loaded
//        list < CResource* > ::iterator iter = m_children.begin ();
//        for ( ; iter != m_children.end (); iter++ )
//        {
//            if ( !(*iter)->LoadResourceItems() )
//            {
//                CLogger::ErrorPrintf("Failed to load items in resource %s required by %s", (*iter)->GetName().c_str (), m_szResourceName );
//                return false;
//            }
//        }
//
//        // everything is now loaded
//        m_bActive = true;
//    }
//    return true;
//}

//bool CResource::IsChildResource ( CResource * resource )
//{
//    list < CResource* > ::iterator iter = m_children.begin ();
//    for ( ; iter != m_children.end (); iter++ )
//    {
//        if ( (*iter) == resource )
//        {
//            return true;
//        }
//    }
//    return false;
//}
//
//bool CResource::IsChildResource ( char * szResourceName )
//{
//    list < CResource* > ::iterator iter = m_children.begin ();
//    for ( ; iter != m_children.end (); iter++ )
//    {
//        if ( strcmp ( (*iter)->GetName(), szResourceName ) == 0 )
//        {
//            return true;
//        }
//    }
//    return false;
//}

bool CResource::IsIncludedResourceRecursive ( vector<CResource *> * past )
{
    past->push_back ( this );

   // CLogger::LogPrintf ( "%s\n", this->GetName().c_str () );

    // Loop through the included resources
    list < CIncludedResources* > ::iterator iter = m_includedResources.begin ();
    for ( ; iter != m_includedResources.end (); iter++ )
    {
        // If one item from the past is equal to one of our dependencies, then there's circular dependencies
        vector < CResource* > ::iterator iterr = past->begin ();
        for ( ; iterr != past->end (); iterr++ )
        {
            if ( (*iterr) == (*iter)->GetResource() )
                return true;
        }

        // Check further down the tree. If this returns true, we pass that true backwards
        CResource * res = (*iter)->GetResource();
        if ( res && res->IsIncludedResourceRecursive ( past ) )
            return true;
    }

    // Remove us from the tree again because it wasn't circular this path
    past->pop_back ();
    return false;
}

bool CResource::IsDependentResource ( CResource * resource )
{
    // Check if the given resource is a resource we're depending on
    list < CResource* > ::iterator iter = m_dependents.begin ();
    for ( ; iter != m_dependents.end (); iter++ )
    {
        if ( (*iter) == resource )
        {
            return true;
        }
    }
    return false;
}

bool CResource::IsDependentResourceRecursive ( CResource * resource )
{
    // Check if the given resource is a resource we're depending on. Also
    // check our dependencies dependencies and etc.. further down the tree
    list < CResource* > ::iterator iter = m_dependents.begin ();
    for ( ; iter != m_dependents.end (); iter++ )
    {
        if ( (*iter) == resource || (*iter)->IsDependentResourceRecursive ( resource ) )
        {
            return true;
        }
    }
    return false;
}

bool CResource::IsDependentResource ( const char * szResourceName )
{
    // Check if the given resource is a resource we're depending on
    list < CResource* > ::iterator iter = m_dependents.begin ();
    for ( ; iter != m_dependents.end (); iter++ )
    {
        if ( strcmp ( (*iter)->GetName ().c_str (), szResourceName ) == 0 )
        {
            return true;
        }
    }
    return false;
}

bool CResource::IsDependentResourceRecursive ( const char * szResourceName )
{
    // Check if the given resource is a resource we're depending on. Also
    // check our dependencies dependencies and etc.. further down the tree
    list < CResource* > ::iterator iter = m_dependents.begin ();
    for ( ; iter != m_dependents.end (); iter++ )
    {
        if ( strcmp ( (*iter)->GetName ().c_str (), szResourceName ) == 0 || (*iter)->IsDependentResourceRecursive ( szResourceName ) )
        {
            return true;
        }
    }
    return false;
}

void CResource::AddTemporaryInclude ( CResource * resource )
{
    if ( !ListContains ( m_temporaryIncludes, resource ) )
        m_temporaryIncludes.push_back ( resource );
}

void CResource::AddDependent ( CResource * resource )
{
    if ( !ListContains ( m_dependents, resource ) )
    {
        m_dependents.push_back ( resource );
        CheckState();
    }
}

void CResource::RemoveDependent ( CResource * resource )
{
    m_dependents.remove ( resource );
    CheckState();
}

ResponseCode CResource::HandleRequest ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse )
{
    // if the mutex is already locked (i.e. the resource is being deleted), we return from this asap
    // otherwise, we get the mutex and can handle the request

  /*  if ( pthread_mutex_trylock(&m_mutex) == EBUSY ) {
        ipoHttpResponse->SetBody("Resource shutting down",strlen("Resource shutting down"));
        printf(" X\n");
        return HTTPRESPONSECODE_500_INTERNALSERVERERROR;
    }*/
    g_pGame->Lock(); // get the mutex (blocking)

    std::string strAccessType;
    const char* szRequest = ipoHttpRequest->sOriginalUri.c_str();
    if ( *szRequest )
    {
        const char* szSlash1 = strchr ( szRequest + 1, '/' );
        if ( szSlash1 )
        {
            const char* szSlash2 = strchr ( szSlash1 + 1, '/' );
            if ( szSlash2 )
                strAccessType.assign ( szSlash1 + 1, szSlash2 - (szSlash1 + 1) );
        }
    }

    CAccount * account = g_pGame->GetHTTPD()->CheckAuthentication ( ipoHttpRequest );
    if ( account )
    {
        ResponseCode response;
        if ( strAccessType == "call" )
            response = HandleRequestCall ( ipoHttpRequest, ipoHttpResponse, account );
        else
            response = HandleRequestActive ( ipoHttpRequest, ipoHttpResponse, account );

        g_pGame->Unlock();
        return response;
    }
   g_pGame->Unlock();
   return HTTPRESPONSECODE_200_OK;

}

void Unescape ( std::string& str )
{
    const char* pPercent = strchr ( str.c_str (), '%' );
    while ( pPercent )
    {
        if ( pPercent[1] && pPercent[2] )
        {
            int iCharCode = 0;
            sscanf ( &pPercent[1], "%02X", &iCharCode );
            str.replace ( pPercent - str.c_str (), 3, (char *)&iCharCode );
            pPercent = strchr ( pPercent + 3, '%' );
        }
        else
        {
            break;
        }
    }
}

ResponseCode CResource::HandleRequestCall ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, CAccount* account )
{
    static int bAlreadyCalling = false; // a mini-mutex flag, seems to work :)
    // This code runs multithreaded, we need to make sure multiple server requests don't overlap each other... (slows the server down quite a bit)

    // Check for http general and if we have access to this resource
    // if we're trying to return a http file. Otherwize it's the MTA
    // client trying to download files.
    CAccessControlListManager * aclManager = g_pGame->GetACLManager();
    if ( !aclManager->CanObjectUseRight ( account->GetName().c_str (),
                                            CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                            m_strResourceName.c_str (),
                                            CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                            true ) ||
         !aclManager->CanObjectUseRight ( account->GetName().c_str (),
                                            CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                            "http",
                                            CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                            true ) )
    {
        bAlreadyCalling = false;
        return g_pGame->GetHTTPD()->RequestLogin ( ipoHttpResponse );;
    }

    #define MAX_INPUT_VARIABLES       25

    if ( !m_bActive )
    {
        const char* szError = "error: resource not running";
        ipoHttpResponse->SetBody ( szError, strlen(szError) );
        return HTTPRESPONSECODE_200_OK;
    }

    const char * szQueryString = ipoHttpRequest->sUri.c_str();
    if ( !*szQueryString )
    {
        const char* szError = "error: invalid function name";
        ipoHttpResponse->SetBody ( szError, strlen(szError) );
        bAlreadyCalling = false;
        return HTTPRESPONSECODE_200_OK;
    }

    std::string strFuncName;
    std::vector < std::string > arguments;
    const char* pQueryArg = strchr ( szQueryString, '?' );
    if ( !pQueryArg )
    {
        strFuncName = szQueryString;
    }
    else
    {
        strFuncName.assign ( szQueryString, pQueryArg - szQueryString );
        pQueryArg++;

        const char* pEqual = NULL;
        const char* pAnd = NULL;

        while ( *pQueryArg )
        {
            pAnd = strchr ( pQueryArg, '&' );
            if ( !pAnd )
                pAnd = pQueryArg + strlen ( pQueryArg );

            pEqual = strchr ( pQueryArg, '=' );
            if ( pEqual && pEqual < pAnd )
            {
                std::string strKey ( pQueryArg, pEqual - pQueryArg );
                int iKey = atoi ( strKey.c_str () );
                if ( iKey >= 0 && iKey < MAX_INPUT_VARIABLES )
                {
                    std::string strValue ( pEqual + 1, pAnd - (pEqual + 1) );
                    Unescape ( strValue );
                    if ( iKey + 1 > static_cast < int > ( arguments.size () ) )
                        arguments.resize ( iKey + 1 );
                    arguments [ iKey ] = strValue;
                }
            }

            if ( *pAnd )
                pQueryArg = pAnd + 1;
            else
                break;
        }
    }
    Unescape ( strFuncName );

    list < CExportedFunction* > ::iterator iter =  m_exportedFunctions.begin ();
    for ( ; iter != m_exportedFunctions.end (); iter++ )
    {
        if ( strFuncName == (*iter)->GetFunctionName () )
        {
            if ( (*iter)->IsHTTPAccessible() )
            {
                CAccessControlListManager * aclManager = g_pGame->GetACLManager();
                SString strResourceFuncName ( "%s.function.%s", m_strResourceName.c_str (), strFuncName.c_str () );

                // @@@@@ Deal with this the new way
                if ( aclManager->CanObjectUseRight ( account->GetName().c_str (),
                                                    CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                    strResourceFuncName.c_str (),
                                                    CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                    true ) )
                {
                    CLuaArguments luaArgs;
                    if ( ipoHttpRequest->nRequestMethod == REQUESTMETHOD_GET )
                    {
                        std::vector < std::string >::iterator it;
                        for ( it = arguments.begin (); it != arguments.end (); it++ )
                        {
                            const char* szArg = (*it).c_str ();
                            if ( strlen(szArg) > 3 && szArg[0] == '^' && szArg[2] == '^' && szArg[1] != '^' )
                            {
                                switch ( szArg[1] )
                                {
                                    case 'E':   // element
                                    {
                                        int id = atoi ( szArg + 3 );
                                        CElement* pElement = NULL;
                                        if ( id != INT_MAX && id != INT_MIN && id != 0 )
                                            pElement = CElementIDs::GetElement ( id );

                                        if ( pElement )
                                        {
                                            luaArgs.PushElement ( pElement );
                                        }
                                        else
                                        {
                                            g_pGame->GetScriptDebugging()->LogError ( NULL, "HTTP Get - Invalid element specified." );
                                            luaArgs.PushNil ();
                                        }
                                        break;
                                    }
                                    case 'R':   // resource
                                    {
                                        CResource* pResource = g_pGame->GetResourceManager()->GetResource ( szArg + 3 );
                                        if ( pResource )
                                        {
                                            luaArgs.PushResource ( pResource );
                                        }
                                        else
                                        {
                                            g_pGame->GetScriptDebugging()->LogError ( NULL, "HTTP Get - Invalid resource specified." );
                                            luaArgs.PushNil ();
                                        }
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                luaArgs.PushString ( szArg );
                            }
                        }
                    }
                    else if ( ipoHttpRequest->nRequestMethod == REQUESTMETHOD_POST )
                    {
                        const char* szRequestBody = ipoHttpRequest->sBody.c_str();
                        luaArgs.ReadFromJSONString ( szRequestBody );
                    }

                    CLuaArguments luaReturns;

					CLuaArguments formData;
					for ( FormValueMap::iterator iter = ipoHttpRequest->oFormValueMap.begin(); iter != ipoHttpRequest->oFormValueMap.end(); iter++ )
					{
						formData.PushString ( (*iter).first.c_str() );
						formData.PushString ( ((FormValue)(*iter).second).sBody.c_str() );
					}

					CLuaArguments cookies;
					for ( CookieMap::iterator iter = ipoHttpRequest->oCookieMap.begin(); iter != ipoHttpRequest->oCookieMap.end(); iter++ )
					{
						cookies.PushString ( (*iter).first.c_str() );
						cookies.PushString ( (*iter).second.c_str() );
					}

					CLuaArguments headers;
					for ( StringMap::iterator iter = ipoHttpRequest->oRequestHeaders.begin(); iter != ipoHttpRequest->oRequestHeaders.end(); iter++ )
					{
						headers.PushString ( (*iter).first.c_str() );
						headers.PushString ( (*iter).second.c_str() );
					}

					// cache old data
					lua_getglobal ( m_pVM->GetVM(), "form" );
					CLuaArgument OldForm ( m_pVM->GetVM(), -1 );
                    lua_pop( m_pVM->GetVM(), 1 );

					lua_getglobal ( m_pVM->GetVM(), "cookies" );
					CLuaArgument OldCookies ( m_pVM->GetVM(), -1 );
                    lua_pop( m_pVM->GetVM(), 1 );

					lua_getglobal ( m_pVM->GetVM(), "requestHeaders" );
					CLuaArgument OldHeaders ( m_pVM->GetVM(), -1 );
                    lua_pop( m_pVM->GetVM(), 1 );

					lua_getglobal ( m_pVM->GetVM(), "hostname" );
					CLuaArgument OldHostname ( m_pVM->GetVM(), -1 );
                    lua_pop( m_pVM->GetVM(), 1 );

					lua_getglobal ( m_pVM->GetVM(), "url" );
					CLuaArgument OldURL ( m_pVM->GetVM(), -1 );
                    lua_pop( m_pVM->GetVM(), 1 );

					lua_getglobal ( m_pVM->GetVM(), "user" );
					CLuaArgument OldUser ( m_pVM->GetVM(), -1 );
                    lua_pop( m_pVM->GetVM(), 1 );

					// push new data
					formData.PushAsTable ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "form" );

					cookies.PushAsTable ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "cookies" );

					headers.PushAsTable ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "requestHeaders" );

					lua_pushstring ( m_pVM->GetVM(), ipoHttpRequest->GetAddress().c_str() );
					lua_setglobal ( m_pVM->GetVM(), "hostname" );

					lua_pushstring ( m_pVM->GetVM(), ipoHttpRequest->sOriginalUri.c_str() );
					lua_setglobal ( m_pVM->GetVM(), "url" );

					lua_pushaccount ( m_pVM->GetVM(), account );
					lua_setglobal ( m_pVM->GetVM(), "user" );

                    luaArgs.CallGlobal ( m_pVM, strFuncName.c_str (), &luaReturns );
                    //g_pGame->Unlock(); // release the mutex

					// restore old data
					OldForm.Push ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "form" );

					OldCookies.Push ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "cookies" );

					OldHeaders.Push ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "requestHeaders" );

					OldHostname.Push ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "hostname" );

					OldURL.Push ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "url" );

					OldUser.Push ( m_pVM->GetVM() );
					lua_setglobal ( m_pVM->GetVM(), "user" );

                    std::string strJSON;
                    luaReturns.WriteToJSONString ( strJSON, true );

                    ipoHttpResponse->SetBody ( strJSON.c_str (), strJSON.length () );
                    bAlreadyCalling = false;
                    return HTTPRESPONSECODE_200_OK;
                }
                else
                {
                    bAlreadyCalling = false;
                    return g_pGame->GetHTTPD()->RequestLogin ( ipoHttpResponse );;
                }
            }
            else
            {
                bAlreadyCalling = false;
                return g_pGame->GetHTTPD()->RequestLogin ( ipoHttpResponse );;
            }
        }
    }

    const char* szError = "error: not found";
    ipoHttpResponse->SetBody ( szError, strlen(szError) );
    bAlreadyCalling = false;
    return HTTPRESPONSECODE_200_OK;
}

ResponseCode CResource::HandleRequestInfo ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse )
{
    char szDebug[4024];
    DisplayInfoHTML ( szDebug, 4024 );
    ipoHttpResponse->SetBody ( szDebug, strlen(szDebug) );
    return HTTPRESPONSECODE_200_OK;
}

ResponseCode CResource::HandleRequestActive ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, CAccount* account )
{
    const char* szUrl = ipoHttpRequest->sOriginalUri.c_str ();
    std::string strFile;
    if ( szUrl[0] )
    {
        const char* pFileFrom = strchr ( szUrl[0] == '/' ? &szUrl[1] : szUrl, '/' );
        if ( pFileFrom )
        {
            pFileFrom++;
            const char* pFileTo = strchr ( pFileFrom, '?' );
            if ( pFileTo )
                strFile.assign ( pFileFrom, pFileTo - pFileFrom );
            else
                strFile = pFileFrom;
        }
    }

    Unescape ( strFile );

    list < CResourceFile* > ::iterator iter =  m_resourceFiles.begin ();
    for ( ; iter != m_resourceFiles.end (); iter++ )
    {
        CResourceFile* pFile = *iter;
        if ( !strFile.empty () )
        {
            if ( strcmp( pFile->GetName(), strFile.c_str () ) == 0 )
            {
                if ( pFile->GetType () == CResourceFile::RESOURCE_FILE_TYPE_HTML )
                {
                    CResourceHTMLItem* pHtml = (CResourceHTMLItem *) pFile;

                    // We need to be active if downloading a HTML file
                    if ( m_bActive )
                    {
                        // Check for http general and if we have access to this resource
                        // if we're trying to return a http file. Otherwise it's the MTA
                        // client trying to download files.
                        CAccessControlListManager * aclManager = g_pGame->GetACLManager();
                        if ( aclManager->CanObjectUseRight ( account->GetName().c_str (),
                                                            CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                            m_strResourceName.c_str (),
                                                            CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                            true ) &&
                            aclManager->CanObjectUseRight ( account->GetName().c_str (),
                                                            CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                            "http",
                                                            CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                            true ) )
                        {
                            SString strResourceFileName ( "%s.file.%s", m_strResourceName.c_str (), pHtml->GetName() );

                            if ( aclManager->CanObjectUseRight ( account->GetName().c_str (),
                                                                CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                                strResourceFileName.c_str (),
                                                                CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                                !pHtml->IsRestricted () ) )
                            {
                                return pHtml->Request ( ipoHttpRequest, ipoHttpResponse, account );
                            }
                            else
                            {
                                return g_pGame->GetHTTPD()->RequestLogin ( ipoHttpResponse );
                            }
                        }
                        else
                        {
                            return g_pGame->GetHTTPD()->RequestLogin ( ipoHttpResponse );
                        }
                    }
                    else
                    {
                        SString err ( "Resource %s is not running.", m_strResourceName.c_str () );
                        ipoHttpResponse->SetBody ( err.c_str (), err.size () );
                        return HTTPRESPONSECODE_401_UNAUTHORIZED;
                    }

                }
                // Send back any clientfile. Otherwise keep looking for server files matching
                // this filename. If none match, the file not found will be sent back.
                else if ( pFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG ||
                          pFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT ||
                          pFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE )
                {
                    return pFile->Request ( ipoHttpRequest, ipoHttpResponse ); // sends back any file in the resource
                }
            }
        }
        else // handle the default page
        {
            // Check for http general and if we have access to this resource
            // if we're trying to return a http file. Otherwize it's the MTA
            // client trying to download files.
            CAccessControlListManager * aclManager = g_pGame->GetACLManager();
            if ( !aclManager->CanObjectUseRight (   account->GetName().c_str (),
                                                    CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                    "http",
                                                    CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                    true ) )
            {
                return g_pGame->GetHTTPD()->RequestLogin ( ipoHttpResponse );
            }


            if ( pFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem* pHtml = (CResourceHTMLItem *) pFile;

                if ( pHtml->IsDefaultPage() )
                {
                    CAccessControlListManager * aclManager = g_pGame->GetACLManager();

                    SString strResourceFileName ( "%s.file.%s", m_strResourceName.c_str (), pFile->GetName() );
                    if ( aclManager->CanObjectUseRight ( account->GetName().c_str (),
                                                            CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                            m_strResourceName.c_str (),
                                                            CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                            true ) &&
                        aclManager->CanObjectUseRight ( account->GetName().c_str (),
                                                            CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                            strResourceFileName.c_str (),
                                                            CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                            !pHtml->IsRestricted () ) )
                    {
                        return pHtml->Request ( ipoHttpRequest, ipoHttpResponse, account );
                    }
                    else
                    {
                        return g_pGame->GetHTTPD()->RequestLogin ( ipoHttpResponse );
                    }
                }
            }
        }
    }

    SString err ( "Cannot find a resource file named '%s' in the resource %s.", strFile.c_str (), m_strResourceName.c_str () );
    ipoHttpResponse->SetBody ( err.c_str (), err.size () );
    return HTTPRESPONSECODE_404_NOTFOUND;
}

bool CResource::CallExportedFunction ( char * szFunctionName, CLuaArguments& args, CLuaArguments& returns, CResource& caller )
{
    if ( !m_bActive )
        return false;

    list < CExportedFunction* > ::iterator iter =  m_exportedFunctions.begin ();
    for ( ; iter != m_exportedFunctions.end (); iter++ )
    {
        // Verify that the exported function is marked as "Server" (since both Client and Server exported functions exist here)
        if ( ( *iter )->GetType () == CExportedFunction::EXPORTED_FUNCTION_TYPE_SERVER )
        {
            bool bRestricted = (*iter)->IsRestricted ();
            if ( strcmp ( (*iter)->GetFunctionName ().c_str (), szFunctionName ) == 0 )
            {
                char szFunctionRightName[512];
                _snprintf ( szFunctionRightName, 512, "%s.function.%s", m_strResourceName.c_str (), szFunctionName );
                CAccessControlListManager * pACLManager = g_pGame->GetACLManager();
                if ( pACLManager->CanObjectUseRight ( caller.GetName().c_str (),
                                                      CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                      m_strResourceName.c_str (),
                                                      CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                      !bRestricted ) &&
                    pACLManager->CanObjectUseRight ( caller.GetName().c_str (),
                                                     CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                     szFunctionRightName,
                                                     CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                     !bRestricted ) )
                {
                    if ( args.CallGlobal ( m_pVM, szFunctionName, &returns ) )
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool CResource::CheckState ( void )
{
    if ( m_dependents.size() == 0 && m_bIsPersistent == false )
    {
        Stop ( false );
        return false;
    }
    else
    {
        return Start ( );
    }
}


void CResource::OnPlayerJoin ( CPlayer& Player )
{
    // do the player join crap
    Player.Send ( CResourceStartPacket ( m_strResourceName.c_str (), this ) );
}


unsigned long get_current_file_crc ( unzFile uf )
{
    char filename_inzip[256];
    int err=UNZ_OK;


    unz_file_info file_info;
    err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
    if ( err == UNZ_OK )
        return file_info.crc;
    else
        return 0;
}

int makedir (char *newdir)
{
  char *buffer ;
  char *p;
  int  len = (int)strlen(newdir);

  if (len <= 0)
    return 0;

  buffer = (char*)malloc(len+1);
  strcpy(buffer,newdir);

  if (buffer[len-1] == '/') {
    buffer[len-1] = '\0';
  }
  if (mymkdir(buffer) == 0)
    {
      free(buffer);
      return 1;
    }

  p = buffer+1;
  while (1)
    {
      char hold;

      while(*p && *p != '\\' && *p != '/')
        p++;
      hold = *p;
      *p = 0;
      if ((mymkdir(buffer) == -1) && (errno == ENOENT))
        {
          //printf("couldn't create directory %s\n",buffer);
          free(buffer);
          return 0;
        }
      if (hold == 0)
        break;
      *p++ = hold;
    }
  free(buffer);
  return 1;
}

/* change_file_date : change the date/time of a file
    filename : the filename of the file where date/time must be modified
    dosdate : the new date at the MSDos format (4 bytes)
    tmu_date : the SAME new date at the tm_unz format */
void change_file_date(const char *filename,uLong dosdate,tm_unz tmu_date)
{
#ifdef WIN32
    HANDLE hFile;
    FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

    hFile = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,
                        0,NULL,OPEN_EXISTING,0,NULL);
    GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
    DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
    LocalFileTimeToFileTime(&ftLocal,&ftm);
    SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
    CloseHandle(hFile);
#else
#ifdef unix
    struct utimbuf ut;
    struct tm newdate;
    newdate.tm_sec = tmu_date.tm_sec;
    newdate.tm_min=tmu_date.tm_min;
    newdate.tm_hour=tmu_date.tm_hour;
    newdate.tm_mday=tmu_date.tm_mday;
    newdate.tm_mon=tmu_date.tm_mon;
    if (tmu_date.tm_year > 1900)
        newdate.tm_year=tmu_date.tm_year - 1900;
    else
        newdate.tm_year=tmu_date.tm_year ;
    newdate.tm_isdst=-1;

    ut.actime=ut.modtime=mktime(&newdate);
    utime(filename,&ut);
#endif
#endif
}

#define WRITEBUFFERSIZE (8192)
int do_extract_currentfile(unzFile uf,const int* popt_extract_without_path,int*popt_overwrite,const char*password, const char* szFilePath)
{
    char filename_inzip[256];
    char* filename_withoutpath;
    char* p;
    int err=UNZ_OK;
    FILE *fout=NULL;
    void* buf;
    uInt size_buf;

    unz_file_info file_info;
    err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

    if (err!=UNZ_OK)
    {
        //printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
        return err;
    }

    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf==NULL)
    {
        //printf("Error allocating memory\n");
        return UNZ_INTERNALERROR;
    }

    p = filename_withoutpath = filename_inzip;
    while ((*p) != '\0')
    {
        if (((*p)=='/') || ((*p)=='\\'))
            filename_withoutpath = p+1;
        p++;
    }

    if ((*filename_withoutpath)=='\0')
    {
        if ((*popt_extract_without_path)==0)
        {
            //printf("creating directory: %s\n",filename_inzip);
            mymkdir(filename_inzip);
        }
    }
    else
    {
        const char* write_filename;
        int skip=0;

        if ((*popt_extract_without_path)==0)
            write_filename = filename_inzip;
        else
            write_filename = filename_withoutpath;

        err = unzOpenCurrentFilePassword(uf,password);
        if (err!=UNZ_OK)
        {
            //printf("error %d with zipfile in unzOpenCurrentFilePassword\n",err);
        }

        // ChrML: We always overwrite and this stuff seems to leak files.
        /*
        if (((*popt_overwrite)==0) && (err==UNZ_OK))
        {
            FILE* ftestexist;
            ftestexist = fopen(write_filename,"rb");
            if (ftestexist!=NULL)
            {
                skip = 1;
            }
        }
        */

        // prepend the filepath to read from
        //_asm int 3;
        mymkdir ( szFilePath );
        char szOutFile [MAX_PATH];
        size_t lenFilePath = strlen ( szFilePath );
        if ( szFilePath [ lenFilePath - 1 ] == '\\' ||
             szFilePath [ lenFilePath - 1 ] == '/' )
        {
            _snprintf ( szOutFile, MAX_PATH, "%s%s", szFilePath, write_filename );
        }
        else
        {
            _snprintf ( szOutFile, MAX_PATH, "%s/%s", szFilePath, write_filename );
        }

        if ((skip==0) && (err==UNZ_OK))
        {
            fout=fopen(szOutFile,"wb");

            /* some zipfile don't contain directory alone before file */
            if ((fout==NULL) && ((*popt_extract_without_path)==0) &&
                                (filename_withoutpath!=(char*)filename_inzip))
            {
                MakeSureDirExists ( szOutFile );
                char c=*(filename_withoutpath-1);
                *(filename_withoutpath-1)='\0';
                //makedir((char *)write_filename);
                *(filename_withoutpath-1)=c;
                fout=fopen(szOutFile,"wb");
            }

            if (fout==NULL)
            {
                //printf("error opening %s\n",write_filename);
            }
        }

        if (fout!=NULL)
        {
            //printf(" extracting: %s\n",write_filename);

            do
            {
                err = unzReadCurrentFile(uf,buf,size_buf);
                if (err<0)
                {
                    //printf("error %d with zipfile in unzReadCurrentFile\n",err);
                    break;
                }
                if (err>0)
                    if (fwrite(buf,err,1,fout)!=1)
                    {
                        //printf("error in writing extracted file\n");
                        err=UNZ_ERRNO;
                        break;
                    }
            }
            while (err>0);
            if (fout)
                    fclose(fout);

            if (err==0)
                change_file_date(write_filename,file_info.dosDate,
                                 file_info.tmu_date);
        }

        if (err==UNZ_OK)
        {
            err = unzCloseCurrentFile (uf);
            if (err!=UNZ_OK)
            {
                //printf("error %d with zipfile in unzCloseCurrentFile\n",err);
            }
        }
        else
            unzCloseCurrentFile(uf); /* don't lose the error */
    }

    free(buf);
    return err;
}

bool CIncludedResources::CreateLink ( void ) // just a pointer to it
{
    // Grab the resource that we are
    m_resource = m_resourceManager->GetResource ( m_strResourceName.c_str () );

    m_bBadVersion = false;

    // Does it exist?
    if ( !m_resource )
        m_bExists = false;
    else
        m_bExists = true;

    if ( m_resource )
    {
        // Grab the version and check that it's in range
        /*unsigned int uiVersion = m_resource->GetVersion ();
        if ( uiVersion < m_uiMinimumVersion || uiVersion > m_uiMaximumVersion )
        {
            CLogger::ErrorPrintf ( "Incompatible version of %s for resource %s\n", m_szResourceName, m_owner->GetName().c_str () );
            CLogger::ErrorPrintf ( "Version between %u and %u needed, version %u installed\n", m_uiMinimumVersion, m_uiMaximumVersion, uiVersion );
            m_bExists = false;
            m_bBadVersion = true;
        }*/
    }

    return m_bExists;
}

void CResource::InvalidateIncludedResourceReference ( CResource * resource )
{
    list < CIncludedResources* > ::iterator iterr = m_includedResources.begin ();
    for ( ; iterr != m_includedResources.end (); iterr++ )
    {
        if ( (*iterr)->GetResource() == resource )
            (*iterr)->InvalidateReference();
    }

    m_temporaryIncludes.remove ( resource );
    assert ( this != resource );
    m_dependents.remove ( resource );
}
