/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceDownloader.h
*  PURPOSE:     Resource download handler class
*  DEVELOPERS:  Ed Lyons <>
*               Kevin Whiteside <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRESOURCEDOWNLOADER_H
#define __CRESOURCEDOWNLOADER_H

#include <xml/CXMLFile.h>
#include <list>
#include "CResource.h"

// This represents one file that needs to be downloaded for a CResourceDownload
class CResourceDownloadFile
{
private:
    std::string         m_strURL;
    std::string         m_strFileName;
    bool                m_bIsDownloaded;
    bool                m_bHasFailed;
    class CResourceDownload * m_pResource;
    static bool         ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error);


public:
                        CResourceDownloadFile ( const char * szURL, const char * szFilename, CResourceDownload * pResource );
    void                Download ( );
    inline bool         IsDownloaded() { return m_bIsDownloaded; };
    inline bool         HasFailed() { return m_bHasFailed; };
};

// This class represents a single resource download (i.e. all the files in one resource)
// First it downloads the update.xml file that specifies the versions available. It checks
// to see if the desired (or latest) version is available. It then downloads the meta for that
// version. It will then download each file in the meta.
class CResourceDownload
{
private:
    list<CResourceDownloadFile *>   m_files;

    std::string         m_strURL;
    std::string         m_strName;
    bool                m_bStartWhenDownloaded;

    class CUpdateResourceVersion *      m_updateResourceVersion;
    
    class CNetHTTPDownloadManagerInterface *     m_downloadManager;
    class CXML *        m_XML;
    static bool         ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error);
    
    void                ParseMeta();
    void                DownloadMeta();

public:
                        CResourceDownload ( const char * szName, const char * szURL, class CUpdateResourceVersion * updateResourceVersion = NULL, bool bStartWhenDownloaded = false );
    void                FileDownloadComplete ( CResourceDownloadFile * file );
    void                FileDownloadFailed ( CResourceDownloadFile * file );
    unsigned int        GetFilesRemainingToDownload();
    static bool         IsVersionSafe ( unsigned int uiMajor, unsigned int uiMinor, unsigned int uiRevision, SVersion* pMinVersion, SVersion* pMaxVersion );
    static bool         IsVersionTooLow ( unsigned int uiMajor, unsigned int uiMinor, unsigned int uiRevision, SVersion* pMinVersion );
};

// This class represents a single available resource version update
class CUpdateResourceVersion
{
private:
    unsigned int        m_major;
    unsigned int        m_minor;
    unsigned int        m_revision;
    unsigned int        m_state; // 0 = alpha/development, 1 = beta, 2 = release
    CChecksum           m_checksum;

    std::string         m_strURL;
    std::string         m_strName;
    class CUpdateResource *   m_updateResource;
    
    CResourceDownload * m_resourceDownload;

public:
                        CUpdateResourceVersion ( const char * szName, unsigned int major, unsigned int minor, unsigned int revision, unsigned int state, const char * szURL, class CUpdateResource * updateResource );
                        CUpdateResourceVersion ( const char * szName, CXMLNode * xmlNode, class CUpdateResource * updateResource );
                        ~CUpdateResourceVersion ();
    inline unsigned int GetMajor() {return m_major; };
    inline unsigned int GetMinor() {return m_minor; };
    inline unsigned int GetRevision() {return m_revision; };
    inline unsigned int GetState() {return m_state; };
    inline CChecksum GetChecksum() {return m_checksum; };
    inline std::string& GetURL() {return m_strURL; };
    inline CUpdateResource * GetUpdateResource() { return m_updateResource; };

    bool                Download ( bool bStartResource );
    void                DownloadComplete (bool bSuccess);
};

// This class represents a single available resource update (or at least a potential update)
// It contains a list of CUpdateResourceVersions - each one representing a version of the 
// resource that is available
class CUpdateResource
{
private:
    std::string         m_strName;
    class CUpdateSite *       m_pSite;

    list<CUpdateResourceVersion *>      m_versions;
public:
                        CUpdateResource ( const char * szName, CUpdateSite * site );
                        CUpdateResource ( CXMLNode * xmlNode, CUpdateSite * site );
                        ~CUpdateResource ();
    inline class CUpdateSite *GetUpdateSite() {return m_pSite; };
    inline std::string& GetName() { return m_strName; };
    inline unsigned int GetVersionCount() { return m_versions.size(); };
    void                FindUpdates ( unsigned int majorMin, unsigned int minorMin, unsigned int revisionMin, unsigned int stateMin, list<CUpdateResourceVersion *> * updates );
    void                FindUpdates ( SVersion* pMinVersion, SVersion* pMaxVersion, list<CUpdateResourceVersion*> * updates );
    void                GetLatestVersion ( unsigned int state, CUpdateResourceVersion ** version );
    CUpdateResourceVersion * Get ( unsigned int majorVersion, unsigned int minorVersion, unsigned int revisionVersion, unsigned int state );
};

// This class represents a single update site. It essentially downloads
// and loads the site's site.xml and loads it and parses out all the 
// resources listed in it. It can then be queried to see what versions are
// available.
class CUpdateSite
{
private:
    list<CUpdateResource*>  m_updateResources;
    std::string         m_strURL;
    std::string         m_strDomain;
    std::string         m_strManifestDownloadFileName; // the name of the file we temporarily store the update manifest in

    class CNetHTTPDownloadManagerInterface *   m_downloadManager;
    class CXML *        m_XML;
    void                DownloadUpdateManifest();
    void                ProcessUpdateManifest();

    static bool         ProgressCallback(double sizeJustDownloaded, double totalDownloaded, char * data, size_t dataLength, void * obj, bool complete, int error);
    
public:
                        CUpdateSite ( const char * szURL );
                        ~CUpdateSite();
    inline std::string& GetURL() { return m_strURL; };

    void                FindUpdates ( const char * szResourceName, unsigned int majorMin, unsigned int minorMin, unsigned int revisionMin, unsigned int stateMin, list<CUpdateResourceVersion *> * updates );
    void                FindUpdates ( const char * szResourceName, SVersion* pMinVersion, SVersion* pMaxVersion, list<CUpdateResourceVersion*> * updates );
    void                GetAvailableResources ( list<CUpdateResource *> * updates );
    void                GetLatestVersion ( const char * szResourceName, unsigned int state, CUpdateResourceVersion ** version );
    CUpdateResourceVersion * Get ( const char * szResourceName, unsigned int majorVersion, unsigned int minorVersion, unsigned int revisionVersion, unsigned int state );
    inline std::string& GetDomain() { return m_strDomain; }
};

// this class manages downloads of resources. This is all async. It can optionally
// start the resource as soon as it has been downloaded.
class CResourceDownloader
{
private:
    list<CUpdateSite *> m_updateSites;
    list<CResource *> m_WaitingResources;
public:
                        ~CResourceDownloader();
    bool                AddUpdateSite ( const char * szURL );
    void                FindUpdates ( const char * szResourceName, unsigned int majorMin, unsigned int minorMin, unsigned int revisionMin, unsigned int stateMin, list<CUpdateResourceVersion *> * updates );
    bool                FindUpdates ( const char * szResourceName, SVersion* pMinVersion, SVersion* pMaxVersion );
    void                GetAvailableResources ( list<CUpdateResource *> * updates );
    CUpdateResourceVersion * GetLatestVersion ( const char * szResourceName, unsigned int state );
    CUpdateResourceVersion * Get ( const char * szResourceName, unsigned int majorVersion, unsigned int minorVersion, unsigned int revisionVersion, unsigned int state );

    void                AddWaitingResource ( CResource* pResource ) { m_WaitingResources.push_back ( pResource ); };
    void                CheckWaitingResources ( void );
    void                RemoveWaitingResource ( CResource* pResource ) { m_WaitingResources.remove ( pResource ); };
};

#endif
