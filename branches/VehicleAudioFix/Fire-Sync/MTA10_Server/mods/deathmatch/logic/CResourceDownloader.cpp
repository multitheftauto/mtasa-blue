/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceDownloader.cpp
*  PURPOSE:     Resource download handler class
*  DEVELOPERS:  Ed Lyons <>
*               Kevin Whiteside <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

#define MAX_DOWNLOADABLE_FILE_TAGS  5
const char * downloadableFileTags[MAX_DOWNLOADABLE_FILE_TAGS]  = {"map", "config", "file", "html", "script"};

//////////////////////////////////////////////////////////
CResourceDownloadFile::CResourceDownloadFile ( const char * szURL, const char * szFileName, CResourceDownload * pResource )
{
    m_strURL = szURL ? szURL : "";
    m_strFileName = szFileName ? szFileName : "";

    m_bIsDownloaded = false;
    m_bHasFailed = false;

    m_pResource = pResource;
}

void CResourceDownloadFile::Download()
{
    CNetHTTPDownloadManagerInterface * manager = g_pNetServer->GetHTTPDownloadManager();
    manager->QueueFile ( m_strURL.c_str (), m_strFileName.c_str (), 0, NULL, this, ProgressCallback );
    if ( !manager->IsDownloading() )
        manager->StartDownloadingQueuedFiles();
}

void CResourceDownloadFile::ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error)
{
    if ( complete )
    {
        CResourceDownloadFile * resourceDownloadFile = (CResourceDownloadFile*)obj;
        resourceDownloadFile->m_bIsDownloaded = true;
        resourceDownloadFile->m_bHasFailed = false;

        resourceDownloadFile->m_pResource->FileDownloadComplete ( resourceDownloadFile );
    }
    else if ( error )
    {
        CLogger::LogPrintf ( "Error!");
    }
}

//////////////////////////////////////////////////////////

CResourceDownload::CResourceDownload ( const char * szName, const char * szURL, CUpdateResourceVersion * updateResourceVersion, bool bStartWhenDownloaded )
{
    m_strURL = szURL ? szURL : "";
    m_strName = szName ? szName : "";

    m_downloadManager = g_pNetServer->GetHTTPDownloadManager();

    m_XML = g_pServerInterface->GetXML();

    m_bStartWhenDownloaded = bStartWhenDownloaded;

    DownloadMeta();

    m_updateResourceVersion = updateResourceVersion;
}

void CResourceDownload::DownloadMeta (  )
{
    char szMetaDownloadPath[260];
    _snprintf ( szMetaDownloadPath, 259, "%s/%s/meta.xml", g_pGame->GetResourceManager()->GetResourceDirectory(), m_strName.c_str () );

    char szMetaURL[512];
    _snprintf ( szMetaURL, 511, "%s/meta.xml", m_strURL.c_str () );
    szMetaURL[511] = '\0';


    m_downloadManager->QueueFile ( szMetaURL, szMetaDownloadPath, 0, NULL, this, ProgressCallback );
    if ( !m_downloadManager->IsDownloading() )
        m_downloadManager->StartDownloadingQueuedFiles();
}

void CResourceDownload::ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error)
{
    if ( complete )
    {
        CResourceDownload * resourceDownload = (CResourceDownload*)obj;
        resourceDownload->ParseMeta();
    }
    else if ( error )
    {
        CLogger::LogPrintf ( "Error accessing resource meta.xml: %d\n", error );
        ((CResourceDownload*)obj)->m_updateResourceVersion->DownloadComplete(false);
    }
}

void CResourceDownload::FileDownloadComplete ( CResourceDownloadFile * file )
{
    if ( GetFilesRemainingToDownload() == 0 )
    {
        CResource * resource = g_pGame->GetResourceManager()->Load ( m_strName.c_str () );
        if ( resource )
        {
            if ( resource->GetIncludedResourcesCount() > 0 )
            {
                // We need a check on the versions of any included resources
                list < CIncludedResources* >::iterator iter = resource->GetIncludedResourcesBegin();
                for ( ; iter != resource->GetIncludedResourcesEnd(); iter++ )
                {
                    CIncludedResources* pIncludedResource = *iter;
                    CResource* pResource = g_pGame->GetResourceManager()->GetResource ( pIncludedResource->GetName ().c_str () );
                    if ( pResource )
                    {
                        // Compare the versions
                        if ( !IsVersionSafe ( pResource->GetVersionMajor(), pResource->GetVersionMinor(), pResource->GetVersionRevision(), pIncludedResource->GetMinVersion(), pIncludedResource->GetMaxVersion() ) )
                        {
                            // Version isn't safe, is it too low?
                            if ( IsVersionTooLow ( pResource->GetVersionMajor(), pResource->GetVersionMinor(), pResource->GetVersionRevision(), pIncludedResource->GetMinVersion() ) )
                            {
                                if ( g_pGame->GetConfig()->GetAutoUpdateIncludedResourcesEnabled() )
                                {
                                    // Version is too low, check for (and download) a higher version within the versions the resource can take
                                    g_pGame->GetResourceDownloader()->FindUpdates ( pIncludedResource->GetName ().c_str (), pIncludedResource->GetMinVersion (), pIncludedResource->GetMaxVersion () );

                                    g_pGame->GetResourceManager()->Unload ( pResource ); // remove it so resource just downloaded doesn't think it is still there

                                    if ( m_bStartWhenDownloaded )
                                    {
                                        CLogger::LogPrintf ( "'%s' should be started when the included resources are started\n", resource->GetName().c_str () );
                                        g_pGame->GetResourceDownloader()->AddWaitingResource ( resource ); // add it to the list of waiting resources
                                    }
                                    if ( m_updateResourceVersion )
                                        m_updateResourceVersion->DownloadComplete(true);
                                }
                                else
                                {
                                    if ( m_bStartWhenDownloaded )
                                    {
                                        CLogger::LogPrintf ( "Server doesn't have permission to auto-update resources, trying to start '%s' anyway\n", resource->GetName().c_str () );
                                        resource->Start ( NULL, true );
                                    }
                                    if ( m_updateResourceVersion )
                                        m_updateResourceVersion->DownloadComplete(true);
                                }
                            }
                            else
                            {
                                if ( m_bStartWhenDownloaded )
                                {
                                    CLogger::LogPrintf ( "Version of %s too high for '%s' attempting to start anyway\n", pIncludedResource->GetName().c_str (), resource->GetName().c_str () );
                                    resource->Start ( NULL, true );
                                }
                                if ( m_updateResourceVersion )
                                    m_updateResourceVersion->DownloadComplete(true);
                            }
                        }
                    }
                    // It obviously doesnt exist, download a version within the minimum and maximum that the resource can take
                    else
                    {
                        if ( g_pGame->GetConfig()->GetAutoUpdateIncludedResourcesEnabled() )
                        {
                            // find (and download) a suitable version
                            g_pGame->GetResourceDownloader()->FindUpdates ( pIncludedResource->GetName ().c_str (), pIncludedResource->GetMinVersion(), pIncludedResource->GetMaxVersion() );

                            if ( m_bStartWhenDownloaded )
                            {
                                CLogger::LogPrintf ( "'%s' should be started when the included resources are started\n", resource->GetName().c_str () );
                                g_pGame->GetResourceDownloader()->AddWaitingResource ( resource ); // add it to the list of waiting resources
                            }
                            if ( m_updateResourceVersion )
                                m_updateResourceVersion->DownloadComplete(true);
                        }
                        else
                        {
                            if ( m_bStartWhenDownloaded )
                            {
                                CLogger::LogPrintf ( "Server doesn't have permission to auto-update resources, trying to start '%s' anyway\n", resource->GetName().c_str () );
                                resource->Start ( NULL, true );
                            }
                            if ( m_updateResourceVersion )
                                m_updateResourceVersion->DownloadComplete(true);
                        }
                    }
                }
            }
            else
            {
                if ( m_bStartWhenDownloaded )
                    resource->Start ( NULL, true );
                if ( m_updateResourceVersion )
                    m_updateResourceVersion->DownloadComplete(true);
            }
            g_pGame->GetResourceDownloader()->CheckWaitingResources (); // check resources that are waiting to be downloaded as this resource is now available
        }
    }
    //m_files.remove ( file );
   // delete file;
}

// not sure when we call this...
void CResourceDownload::FileDownloadFailed ( CResourceDownloadFile * file )
{

}

unsigned int CResourceDownload::GetFilesRemainingToDownload()
{
    unsigned int count = 0;
    list < CResourceDownloadFile* > ::iterator iter = m_files.begin ();
    for ( ; iter != m_files.end (); iter++ )
    {
        if ( !(*iter)->IsDownloaded() )
            count++;
    }
    return count;
}

void CResourceDownload::ParseMeta()
{
    char szMetaDownloadPath[260];
    _snprintf ( szMetaDownloadPath, 259, "%s/%s/meta.xml", g_pGame->GetResourceManager()->GetResourceDirectory(), m_strName.c_str () );

    // Create the XML
    CXMLFile * xmlfile = m_XML->CreateXML ( szMetaDownloadPath );
    if ( xmlfile )
    {
        // Parse it
        if ( xmlfile->Parse () )
        {
            CXMLNode * root = xmlfile->GetRootNode();
            if ( root )
            {
                for ( int j = 0; j < MAX_DOWNLOADABLE_FILE_TAGS; j++ )
                {
                    int i = 0;
                    for ( CXMLNode * fileTag = root->FindSubNode(downloadableFileTags[j], i);
                        fileTag != NULL; fileTag = root->FindSubNode(downloadableFileTags[j], ++i ) )
                    {
                        CXMLAttribute * attrSrc = fileTag->GetAttributes().Find ( "src" );
                        if ( attrSrc )
                        {
                            std::string strFileName = attrSrc->GetValue ().c_str ();

                            char szURL[512];
                            _snprintf ( szURL, 511, "%s//%s", m_strURL.c_str (), strFileName.c_str () );
                            szURL[511] = '\0';

                            char szPath[260];
                            _snprintf ( szPath, 259, "%s/%s/%s", g_pGame->GetResourceManager()->GetResourceDirectory(), m_strName.c_str (), strFileName.c_str () );
                            szPath[259] = '\0';

                            CResourceDownloadFile * file = new CResourceDownloadFile ( szURL, szPath, this );
                            file->Download();
                            m_files.push_back ( file );
                        }
                    }
                }
            }
        }

        // Delete the XML again
        delete xmlfile;
    }
}


bool CResourceDownload::IsVersionSafe ( unsigned int uiMajor, unsigned int uiMinor, unsigned int uiRevision, SVersion* pMinVersion, SVersion* pMaxVersion )
{
    if ( ( pMinVersion->m_uiMajor < uiMajor ) && ( pMaxVersion->m_uiMajor > uiMajor ) )
        return true; // It's between the versions so return true
    else if ( ( pMinVersion->m_uiMajor <= uiMajor ) && ( pMaxVersion->m_uiMajor >= uiMajor ) )
    {
        if ( ( pMinVersion->m_uiMinor < uiMinor ) && ( pMaxVersion->m_uiMinor > uiMinor ) )
            return true; // It's between the versions so return true
        else if ( ( pMinVersion->m_uiMinor <= uiMinor ) && ( pMaxVersion->m_uiMinor >= uiMinor ) )
        {
            if ( ( pMinVersion->m_uiRevision <= uiRevision ) && ( pMaxVersion->m_uiRevision >= uiRevision ) )
                return true; // It's between the versions so return true
            else
                return false;
        }
    }
    return false;
}

bool CResourceDownload::IsVersionTooLow ( unsigned int uiMajor, unsigned int uiMinor, unsigned int uiRevision, SVersion* pMinVersion )
{
    if ( pMinVersion->m_uiMajor > uiMajor )
        return true;
    else if ( pMinVersion->m_uiMinor > uiMinor )
        return true;
    else if ( pMinVersion->m_uiRevision > uiRevision )
        return true;

    return false;
}

/////////////////////////////////////////////////////////////////////////
CUpdateResourceVersion::CUpdateResourceVersion ( const char * szName, unsigned int major, unsigned int minor, unsigned int revision, unsigned int state, const char * szURL, CUpdateResource * updateResource )
{
    m_major = major;
    m_minor = minor;
    m_revision = revision;
    m_state = state;
    m_updateResource = updateResource;

    m_strURL = szURL ? szURL : "";
    m_strName = szName ? szName : "";

    m_resourceDownload = NULL;

}

CUpdateResourceVersion::CUpdateResourceVersion ( const char * szName, CXMLNode * xmlNode, CUpdateResource * updateResource )
{
    std::string strTemp;

    //CLogger::LogPrintf("Added version for %s", szName);
    m_updateResource = updateResource;

    m_resourceDownload = NULL;

    m_strName = szName ? szName : "";

    CXMLAttributes * versionAttributes = &(xmlNode->GetAttributes());
    if ( versionAttributes )
    {
        CXMLAttribute * attr = versionAttributes->Find ( "major" );
        if ( attr )
        {
            m_major = atoi(attr->GetValue ().c_str ());
        }

        attr = versionAttributes->Find ( "minor" );
        if ( attr )
        {
            m_minor = atoi(attr->GetValue ().c_str ());
        }

        attr = versionAttributes->Find ( "revision" );
        if ( attr )
        {
            m_revision = atoi(attr->GetValue ().c_str ());
        }

        attr = versionAttributes->Find ( "state" );
        if ( attr )
        {
            strTemp = attr->GetValue ();

            if ( strTemp.compare ( "alpha" ) == 0 )
                m_state = 0;
            else if ( strTemp.compare ( "beta" ) == 0 )
                m_state = 1;
            else
                m_state = 2;
        }

        attr = versionAttributes->Find ( "url" );
        if ( attr )
        {
            m_strURL = attr->GetValue ();
        }

        attr = versionAttributes->Find ( "crc" );
        if ( attr )
        {
            m_checksum = CChecksum ();
            m_checksum.ulCRC = strtoul(attr->GetValue().c_str(), NULL, 0);
        }
        else
        {
            m_checksum = CChecksum ();
        }
    }
}

CUpdateResourceVersion::~CUpdateResourceVersion()
{
    if ( m_resourceDownload )
        delete m_resourceDownload;
}

bool CUpdateResourceVersion::Download ( bool bStartResource )
{
    if ( !m_resourceDownload )
    {
        m_resourceDownload = new CResourceDownload ( m_strName.c_str (), m_strURL.c_str (), this, bStartResource );
        return true;
    }
    else
    {
        return false;
    }
}

void CUpdateResourceVersion::DownloadComplete (bool bSuccess)
{
    if ( m_resourceDownload )
        delete m_resourceDownload;
    m_resourceDownload = NULL;
}

/////////////////////////////////////////////////////////////////////////
CUpdateResource::CUpdateResource ( const char * szName, CUpdateSite * site )
{
    m_strName = szName ? szName : "";
    m_pSite = site;
}

CUpdateResource::CUpdateResource ( CXMLNode * xmlNode, CUpdateSite * site )
{
    CXMLAttributes * attributes = &(xmlNode->GetAttributes());
    if ( attributes )
    {
        CXMLAttribute * name = attributes->Find ( "name" );
        if ( name )
        {
            m_strName = name->GetValue ();

            int i = 0;
            for ( CXMLNode * ver = xmlNode->FindSubNode("version", i);
                ver != NULL; ver = xmlNode->FindSubNode("version", ++i ) )
            {
                m_versions.push_back ( new CUpdateResourceVersion ( m_strName.c_str (), ver, this ) );
            }
        }
    }

    m_pSite = site;
}

CUpdateResource::~CUpdateResource()
{
    list < CUpdateResourceVersion* > ::iterator iter = m_versions.begin ();
    for ( ; iter != m_versions.end (); iter++ )
    {
        delete (*iter);
    }
}

void CUpdateResource::FindUpdates ( unsigned int majorMin, unsigned int minorMin, unsigned int revisionMin, unsigned int stateMin, list<CUpdateResourceVersion *> * updates )
{
    list < CUpdateResourceVersion* > ::iterator iter = m_versions.begin ();
    for ( ; iter != m_versions.end (); iter++ )
    {
        CUpdateResourceVersion * version = (*iter);
        if ( version->GetMajor() > majorMin ||
            ( version->GetMajor() == majorMin && version->GetMinor() > minorMin ) ||
            ( version->GetMajor() == majorMin && version->GetMinor() == minorMin && version->GetRevision() > revisionMin ) ||
            ( version->GetMajor() == majorMin && version->GetMinor() == minorMin && version->GetRevision() == revisionMin && version->GetState() >= stateMin ) )
        {
            bool bSkip = false;
            list < CUpdateResourceVersion* > ::iterator iter2 = updates->begin ();
            for ( ; iter2 != updates->end (); iter2++ )
            {
                CUpdateResourceVersion* ver1 = (*iter);
                CUpdateResourceVersion* ver2 = (*iter2);

                if ( ver1->GetUpdateResource()->GetName().compare ( ver2->GetUpdateResource()->GetName().c_str () ) == 0 &&
                    ver1->GetMajor() == ver2->GetMajor() &&
                    ver1->GetMinor() == ver2->GetMinor() &&
                    ver1->GetRevision() == ver2->GetRevision() &&
                    ver1->GetState() == ver2->GetState() )
                {
                    bSkip = true;
                    break;
                }
            }
            if ( !bSkip )
                updates->push_back ( version );
        }
    }
}


void CUpdateResource::FindUpdates ( SVersion* pMinVersion, SVersion* pMaxVersion, list<CUpdateResourceVersion*> * updates )
{
    list < CUpdateResourceVersion* > ::iterator iter = m_versions.begin ();
    for ( ; iter != m_versions.end (); iter++ )
    {
        CUpdateResourceVersion * version = (*iter);
        if ( CResourceDownload::IsVersionSafe ( version->GetMajor(), version->GetMinor(), version->GetRevision(), pMinVersion, pMaxVersion ) )
        {
            bool bSkip = false;
            list < CUpdateResourceVersion* > ::iterator iter2 = updates->begin ();
            for ( ; iter2 != updates->end (); iter2++ )
            {
                CUpdateResourceVersion* ver1 = (*iter);
                CUpdateResourceVersion* ver2 = (*iter2);

                if ( ver1->GetUpdateResource()->GetName().compare ( ver2->GetUpdateResource()->GetName().c_str () ) == 0 &&
                    ver1->GetMajor() == ver2->GetMajor() &&
                    ver1->GetMinor() == ver2->GetMinor() &&
                    ver1->GetRevision() == ver2->GetRevision() &&
                    ver1->GetState() == ver2->GetState() )
                {
                    bSkip = true;
                    break;
                }
            }
            if ( !bSkip )
                updates->push_back ( version );
        }
    }
}


void CUpdateResource::GetLatestVersion ( unsigned int state, CUpdateResourceVersion ** version )
{
    list < CUpdateResourceVersion* > ::iterator iter = m_versions.begin ();
    for ( ; iter != m_versions.end (); iter++ )
    {
        CUpdateResourceVersion * thisversion = (*iter);
        if ( state == thisversion->GetState() )
        {
            if  ( ( *version == NULL ) ||
                ( thisversion->GetMajor() > (*version)->GetMajor() ) ||
                ( thisversion->GetMajor() == (*version)->GetMajor() && thisversion->GetMinor() > (*version)->GetMinor() ) ||
                ( thisversion->GetMajor() == (*version)->GetMajor() && thisversion->GetMinor() == (*version)->GetMinor() && thisversion->GetRevision() > (*version)->GetRevision() ) )
            {
                *version = thisversion;
            }
        }
    }
}

CUpdateResourceVersion * CUpdateResource::Get ( unsigned int majorVersion, unsigned int minorVersion, unsigned int revisionVersion, unsigned int state )
{
    list < CUpdateResourceVersion* > ::iterator iter = m_versions.begin ();
    for ( ; iter != m_versions.end (); iter++ )
    {
        CUpdateResourceVersion * thisversion = (*iter);
        if ( state == thisversion->GetState() &&
            majorVersion == thisversion->GetMajor() &&
            minorVersion == thisversion->GetMinor() &&
            revisionVersion == thisversion->GetRevision() )
        {
            return thisversion;
        }
    }
    return NULL;
}

/////////////////////////////////////////////////////////////////////////

CUpdateSite::CUpdateSite ( const char * szURL )
{
    int urlLength = strlen ( szURL );
    int start = 0;

    if ( szURL[0] == 'h' && szURL[1] == 't' && szURL[2] == 't' && szURL[3] == 'p' && szURL[4] == ':' && szURL[5] == '/' && szURL[6] == '/' )
        start = 7;

    int j = 0;
    for ( int i = start; i < urlLength && szURL[i] != '/'; i++ )
    {
        m_strDomain[j] = szURL[i];
        j++;
    }

    CLogger::LogPrintf ( "Added update site: %s\n", szURL );

    m_strURL = szURL ? szURL : "";

    m_downloadManager = g_pNetServer->GetHTTPDownloadManager();

    m_XML = g_pServerInterface->GetXML();

    DownloadUpdateManifest();
}

CUpdateSite::~CUpdateSite()
{
    list < CUpdateResource* > ::iterator iter = m_updateResources.begin ();
    for ( ; iter != m_updateResources.end (); iter++ )
    {
        delete (*iter);
    }
}

void CUpdateSite::DownloadUpdateManifest()
{
    char szManifestURL[512], szDownload[512];
    _snprintf ( szManifestURL, 511, "%s", m_strURL.c_str () ); // pointless currently
    szManifestURL[511] = '\0';

    srand(GetTickCount());
    _snprintf ( szDownload, 511, "temp%d.xml", rand() );
    szDownload[511] = '\0';
    m_strManifestDownloadFileName = szDownload;

    m_downloadManager->QueueFile ( szManifestURL, szDownload, 0, 0, this, ProgressCallback );
    if ( !m_downloadManager->IsDownloading() )
        m_downloadManager->StartDownloadingQueuedFiles();
}

void CUpdateSite::ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error)
{
    if ( complete )
    {
        ((CUpdateSite*)obj)->ProcessUpdateManifest();
    }
}

void CUpdateSite::ProcessUpdateManifest ()
{
    // Create the XML
    CXMLFile* xmlFile = m_XML->CreateXML ( m_strManifestDownloadFileName.c_str () );
    if ( xmlFile )
    {
        // Try to parse it
        if ( xmlFile->Parse () )
        {
            CXMLNode* root = xmlFile->GetRootNode();
            if ( root )
            {
                int i = 0;
                for ( CXMLNode * inc = root->FindSubNode("resource", i);
                    inc != NULL; inc = root->FindSubNode("resource", ++i ) )
                {
                    m_updateResources.push_back ( new CUpdateResource ( inc, this ) );
                }
            }
        }

        // Delete the XML again
        delete xmlFile;
    }


    remove ( m_strManifestDownloadFileName.c_str () );

}

void CUpdateSite::FindUpdates ( const char * szResourceName, unsigned int majorMin, unsigned int minorMin, unsigned int revisionMin, unsigned int stateMin, list<CUpdateResourceVersion *> * updates )
{
    list < CUpdateResource* > ::iterator iter = m_updateResources.begin ();
    for ( ; iter != m_updateResources.end (); iter++ )
    {
        if ( !szResourceName || (*iter)->GetName().compare ( szResourceName ) == 0 )
        {
            (*iter)->FindUpdates ( majorMin, minorMin, revisionMin, stateMin, updates );
        }
    }
}


void CUpdateSite::FindUpdates ( const char * szResourceName, SVersion* pMinVersion, SVersion* pMaxVersion, list<CUpdateResourceVersion*> * updates )
{
    list < CUpdateResource* > ::iterator iter = m_updateResources.begin ();
    for ( ; iter != m_updateResources.end (); iter++ )
    {
        if ( !szResourceName || (*iter)->GetName().compare ( szResourceName ) == 0 )
        {
            (*iter)->FindUpdates ( pMinVersion, pMaxVersion, updates );
        }
    }
}


void CUpdateSite::GetAvailableResources ( list<CUpdateResource *> * updates )
{
    list < CUpdateResource* > ::iterator iter = m_updateResources.begin ();
    for ( ; iter != m_updateResources.end (); iter++ )
    {
        bool bSkip = false;
        list < CUpdateResource* > ::iterator iter2 = updates->begin ();
        for ( ; iter2 != updates->end (); iter2++ )
        {
            if ( stricmp ( (*iter2)->GetName().c_str (), (*iter)->GetName().c_str () ) == 0 )
            {
                bSkip = true;
                break;
            }
        }

        if ( !bSkip )
            updates->push_back ( (*iter) );
    }
}

void CUpdateSite::GetLatestVersion ( const char * szResourceName, unsigned int state, CUpdateResourceVersion ** version )
{
    list < CUpdateResource* > ::iterator iter = m_updateResources.begin ();
    for ( ; iter != m_updateResources.end (); iter++ )
    {
        if ( (*iter)->GetName().compare ( szResourceName ) == 0 )
        {
            (*iter)->GetLatestVersion ( state, version );
        }
    }
}

CUpdateResourceVersion * CUpdateSite::Get ( const char * szResourceName, unsigned int majorVersion, unsigned int minorVersion, unsigned int revisionVersion, unsigned int state )
{
    list < CUpdateResource* > ::iterator iter = m_updateResources.begin ();
    for ( ; iter != m_updateResources.end (); iter++ )
    {
        if ( (*iter)->GetName().compare ( szResourceName ) == 0 )
        {
            CUpdateResourceVersion * update = (*iter)->Get ( majorVersion, minorVersion, revisionVersion, state );
            if ( update )
                return update;
        }
    }
    return NULL;
}



/////////////////////////////////////////////////////////////////////////

CResourceDownloader::~CResourceDownloader()
{
    list < CUpdateSite* > ::iterator iter = m_updateSites.begin ();
    for ( ; iter != m_updateSites.end (); iter++ )
    {
        delete (*iter);
    }
}

bool CResourceDownloader::AddUpdateSite ( const char * szURL )
{
    list < CUpdateSite* > ::iterator iter = m_updateSites.begin ();
    for ( ; iter != m_updateSites.end (); iter++ )
    {
        if ( stricmp ( (*iter)->GetURL ().c_str (), szURL ) == 0 )
            return false;
    }

    m_updateSites.push_back ( new CUpdateSite ( szURL ) );
    return true;
}

bool compareUpdates ( CUpdateResourceVersion* first, CUpdateResourceVersion* second)
{
    if ( first->GetMajor() > second->GetMajor() ||
        ( first->GetMajor() == second->GetMajor() && first->GetMinor() > second->GetMinor() ) ||
        ( first->GetMajor() == second->GetMajor() && first->GetMinor() == second->GetMinor() && first->GetRevision() > second->GetRevision() ) ||
        ( first->GetMajor() == second->GetMajor() && first->GetMinor() == second->GetMinor() && first->GetRevision() == second->GetRevision() && first->GetState() >= second->GetState() ) )
        return true;
    return false;
}

void CResourceDownloader::FindUpdates ( const char * szResourceName, unsigned int majorMin, unsigned int minorMin, unsigned int revisionMin, unsigned int stateMin, list<CUpdateResourceVersion *> * updates )
{
    list < CUpdateSite* > ::iterator iter = m_updateSites.begin ();
    for ( ; iter != m_updateSites.end (); iter++ )
    {
        (*iter)->FindUpdates ( szResourceName, majorMin, minorMin, revisionMin, stateMin, updates );
    }

    updates->sort ( compareUpdates );
}


bool CResourceDownloader::FindUpdates ( const char * szResourceName, SVersion* pMinVersion, SVersion* pMaxVersion )
{
    list<CUpdateResourceVersion*> updates;
    list < CUpdateSite* > ::iterator iter = m_updateSites.begin ();
    for ( ; iter != m_updateSites.end (); iter++ )
    {
        (*iter)->FindUpdates ( szResourceName, pMinVersion, pMaxVersion, &updates );
    }
    if ( updates.size() != 0 )
    {
        list < CUpdateResourceVersion* > ::iterator iter = updates.begin ();
        CUpdateResourceVersion* pLatest = *iter;
        for ( iter++; iter != updates.end(); iter++ )
        {
            if ( (*iter)->GetState() == 2 &&
                    pLatest->GetState() !=2 )
            {
                pLatest = *iter;
            }
            else if ( (*iter)->GetMajor() > pLatest->GetMajor() )
            {
                pLatest = *iter;
            }
            else if ( (*iter)->GetMajor() == pLatest->GetMajor() )
            {
                if ( (*iter)->GetMinor() > pLatest->GetMajor() )
                {
                    pLatest = *iter;
                }
                else if ( (*iter)->GetMinor() == pLatest->GetMinor() )
                {
                    if ( (*iter)->GetRevision() > pLatest->GetRevision() )
                    {
                        pLatest = *iter;
                    }
                }
            }
        }
        if ( pLatest && pLatest->GetState() == 2 )
        {
            CUpdateResourceVersion * update = Get ( szResourceName, pLatest->GetMajor(), pLatest->GetMinor(), pLatest->GetRevision(), pLatest->GetState() );
            if ( update )
            {
                CLogger::LogPrintf ( "Downloading v%i.%i.%i of %s\n", pLatest->GetMajor(), pLatest->GetMinor(), pLatest->GetRevision(), szResourceName );
                update->Download ( false );
                return true;
            }
        }
    }
    return false;
}


CUpdateResourceVersion * CResourceDownloader::Get ( const char * szResourceName, unsigned int majorVersion, unsigned int minorVersion, unsigned int revisionVersion, unsigned int state )
{
    list < CUpdateSite* > ::iterator iter = m_updateSites.begin ();
    for ( ; iter != m_updateSites.end (); iter++ )
    {
        CUpdateResourceVersion * update = (*iter)->Get ( szResourceName, majorVersion, minorVersion, revisionVersion, state );
        if ( update )
            return update;
    }
    return NULL;
}

void CResourceDownloader::GetAvailableResources ( list<CUpdateResource *> * updates )
{
    list < CUpdateSite* > ::iterator iter = m_updateSites.begin ();
    for ( ; iter != m_updateSites.end (); iter++ )
    {
        (*iter)->GetAvailableResources ( updates );
    }
}

void CResourceDownloader::CheckWaitingResources ( void )
{
    if ( m_WaitingResources.size() == 0 )
        return ;
    list < CResource* > resourcesToRemove;
    list < CResource* >::iterator iter = m_WaitingResources.begin();
    for ( ; iter != m_WaitingResources.end(); iter++ )
    {
        bool bAllExist = true;
        CResource* pResource = NULL;
        list < CIncludedResources* >::iterator iterir = (*iter)->GetIncludedResourcesBegin();
        for ( ; iterir != (*iter)->GetIncludedResourcesEnd(); iterir++ )
        {
            pResource = g_pGame->GetResourceManager()->GetResource ( (*iterir)->GetName ().c_str () );
            if ( !pResource )
                bAllExist = false;
        }
        if ( bAllExist )
        {
            if ( (*iter)->Start ( NULL, true ) ) // start the resource
                resourcesToRemove.push_back ( *iter );
        }
    }
    iter = resourcesToRemove.begin();
    for ( ; iter != resourcesToRemove.end(); iter++ )
    {
        m_WaitingResources.remove ( *iter );
    }
    resourcesToRemove.clear();
}

// get the latest version - highest version number with the state specified
CUpdateResourceVersion * CResourceDownloader::GetLatestVersion ( const char * szResourceName, unsigned int state )
{
    CUpdateResourceVersion * version = NULL;
    list < CUpdateSite* > ::iterator iter = m_updateSites.begin ();
    for ( ; iter != m_updateSites.end (); iter++ )
    {
        (*iter)->GetLatestVersion ( szResourceName, state, &version );
    }
    return version;
}

