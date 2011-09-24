/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResource.h
*  PURPOSE:     Resource handler class
*  DEVELOPERS:  Ed Lyons <>
*               Oliver Brown <>
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

#ifndef CRESOURCE_H
#define CRESOURCE_H

#include "packets/CResourceStartPacket.h"
#include "packets/CResourceStopPacket.h"
#include "packets/CEntityRemovePacket.h"

#include "CResourceFile.h"
#include "CResourceManager.h"
#include "CElementGroup.h"
#include <unzip.h>
#include <list>
#include <vector>
#include "ehs/ehs.h"
#include <time.h>
#include <pthread.h>

#define MAX_AUTHOR_LENGTH           255
#define MAX_RESOURCE_NAME_LENGTH    255
#define MAX_FUNCTION_NAME_LENGTH    50
#define MAX_RESOURCE_VERSION_LENGTH 100

struct SVersion
{
    unsigned int m_uiMajor;
    unsigned int m_uiMinor;
    unsigned int m_uiRevision;
    //unsigned int m_uiState;
};

class CExportedFunction
{
public:
    enum eExportedFunctionType
    {
        EXPORTED_FUNCTION_TYPE_CLIENT,
        EXPORTED_FUNCTION_TYPE_SERVER
    };

private:
    std::string                     m_strFunctionName;
    vector<std::string>             m_accessList;
    bool                            m_bHTTPAccess;
    bool                            m_bRestricted;

    eExportedFunctionType           m_ucType;

public:
                                    CExportedFunction ( const std::string& strFunctionName, const std::string& access, bool bHTTPAccess, eExportedFunctionType ucType, bool bRestricted )
                                    {
                                        m_ucType = ucType;
                                        m_strFunctionName = strFunctionName;
                                        m_bHTTPAccess = bHTTPAccess;
                                        m_bRestricted = bRestricted;
                                        size_t leng = access.length ();
                                        char szResourceName[MAX_RESOURCE_NAME_LENGTH] = {'\0'};
                                        size_t s = 0;
                                        for ( size_t i = 0; i < leng; i++ )
                                        {
                                            if ( access[i] != ',' )
                                                szResourceName[s] = access[i];
                                            else if ( strlen(szResourceName) != 0 )
                                            {
                                                m_accessList.push_back ( szResourceName );
                                                szResourceName[0] = '\0';
                                                s = 0;
                                            }
                                        }   
                                    }

    inline eExportedFunctionType    GetType ( void ) { return m_ucType; }
    inline const std::string&       GetFunctionName ( void ) { return m_strFunctionName; }
    inline bool                     IsHTTPAccessible ( void ) { return m_bHTTPAccess; }
    inline bool                     IsRestricted ( void ) { return m_bRestricted; };
    inline bool                     IsOnAccessList ( char * strResourceName )
    {
        if ( m_accessList.size() == 0 )
            return false;
        vector < std::string > ::iterator iter = m_accessList.begin ();
        for ( ; iter != m_accessList.end (); iter++ )
        {
            if ( (*iter).compare ( "*" ) == 0 || (*iter).compare ( strResourceName ) == 0 )
                return true;
        }
        return false;
    }
};

class CIncludedResources 
{
private:
    std::string             m_strResourceName;
    unsigned int            m_uiMinimumVersion;
    unsigned int            m_uiMaximumVersion;
    SVersion                m_MinVersion;
    SVersion                m_MaxVersion;
    bool                    m_bExists;
    bool                    m_bBadVersion;
    class CResource *       m_resource; // the resource this links to
    class CResource *       m_owner; // the resource this is inside
    class CResourceManager *      m_resourceManager;

public:
    CIncludedResources ( CResourceManager * manager, const std::string& strResourceName, SVersion svMinVersion, SVersion svMaxVersion, unsigned int uiMinVersion, unsigned int uiMaxVersion, CResource * owner )
    {
        m_resourceManager = manager;
        m_owner = owner;
        m_strResourceName = strResourceName;
        m_MinVersion = svMinVersion;
        m_MaxVersion = svMaxVersion;
        m_uiMinimumVersion = uiMinVersion;
        m_uiMaximumVersion = uiMaxVersion;
        m_bExists = false;
        m_resource = NULL;
    }

    inline const std::string&   GetName             ( void )    { return m_strResourceName; }
    int                         GetMinimumVersion   ( void )    { return m_uiMinimumVersion; }
    int                         GetMaximumVersion   ( void )    { return m_uiMaximumVersion; }
    inline SVersion*            GetMinVersion       ( void )    { return &m_MinVersion; };
    inline SVersion*            GetMaxVersion       ( void )    { return &m_MaxVersion; };
    inline bool                 DoesExist           ( void ) { return m_bExists; }
    inline bool                 IsBadVersion        ( void ) { return m_bBadVersion; }
    inline CResource *          GetResource         ( void ) { return m_resource; }
    bool                        CreateLink          ( void );
    inline void                 InvalidateReference ( void ) { m_resource = NULL; m_bExists = false; };
};

class CInfoValue
{
private:
    std::string m_strName;
    std::string m_strValue;
public:
    CInfoValue ( const std::string& strName, const std::string& strValue )
    {
        m_strName = strName;
        m_strValue = strValue;
    }
    ~CInfoValue ()
    {
    }
    inline const std::string&   GetValue ( void )                       { return m_strValue; }
    inline void                 SetValue ( const std::string& szValue ) { m_strValue = szValue; }
    inline const std::string&   GetName  ( void )                       { return m_strName; }
};

class CResource : public EHS
{
private:
    bool                    m_bHandlingHTTPRequest;
    bool                    m_bResourceIsZip;
    std::string             m_strResourceName;
    SString                 m_strAbsPath;               // Absolute path to containing directory        i.e. /server/mods/deathmatch/resources
    std::string             m_strResourceZip;           // Absolute path to zip file (if a zip)         i.e. m_strAbsPath/resource_name.zip
    std::string             m_strResourceDirectoryPath; // Absolute path to resource files (if a dir)   i.e. m_strAbsPath/resource_name
    std::string             m_strResourceCachePath;     // Absolute path to unzipped cache (if a zip)   i.e. /server/mods/deathmatch/resources/cache/resource_name 

    unsigned int            m_uiVersionMajor;
    unsigned int            m_uiVersionMinor;
    unsigned int            m_uiVersionRevision;
    unsigned int            m_uiVersionState;

    time_t                  m_timeLoaded;
    time_t                  m_timeStarted;

    class CElement *        m_pRootElement;
    class CDummy*           m_pResourceElement;
    class CDummy*           m_pResourceDynamicElementRoot;

    list<CInfoValue>        m_infoValues;
    list<CIncludedResources *>  m_includedResources; // we store them here temporarily, then read them once all the resources are loaded
    list<CResourceFile *>   m_resourceFiles;
    list<CResource *>       m_dependents; // resources that have "included" or loaded this one
    list<CElementGroup *>   m_elementGroups; // stores elements created by scripts in this resource
    list<CExportedFunction *>   m_exportedFunctions;
    list<CResource *>       m_temporaryIncludes; // started by startResource script command
    
    CElementGroup *         m_pDefaultElementGroup;
    
    std::string             m_strCircularInclude;
    bool                    m_bLoaded; // its been loaded successfully (i.e. meta parsed ok), included resources loaded ok
    bool                    m_bActive; // resource items are running
    bool                    m_bStarting; // the resource is starting
    bool                    m_bStopping; // the resource is stopping
    bool                    m_bIsPersistent; // if true, the resource will remain even if it has no Dependents, mainly if started by the user or the startup
    bool                    m_bLinked; // if true, the included resources are already linked to this resource
    unzFile                 m_zipfile;
    SString                 m_strFailureReason;

    bool                    m_bClientConfigs;
    bool                    m_bClientScripts;
    bool                    m_bClientFiles;
    bool                    m_bHasStarted;
    bool                    m_bDoneUpgradeWarnings;

    class CResourceManager *      m_resourceManager; 
    class CLuaMain *        m_pVM;

    CXMLNode *              m_pNodeSettings;        // Settings XML node, read from meta.xml and copied into it's own instance
    CXMLNode *              m_pNodeStorage;         // Dummy XML node used for temporary storage of stuff returned by CSettings::Get

    CChecksum               m_checksum; // Checksum last time this was loaded, generated by GenerateChecksum()

    unsigned short          m_usID; // resource ID    

    bool                    m_bProtected;
    bool                    m_bStartedManually;

    bool                    CheckState ( void ); // if the resource has no Dependents, stop it, if it has, start it. returns true if the resource is started.
    bool                    ReadIncludedResources ( class CXMLNode * root );
    bool                    ReadIncludedMaps ( CXMLNode * root );
    bool                    ReadIncludedScripts ( CXMLNode * root );
    bool                    ReadIncludedConfigs ( CXMLNode * root );
    bool                    ReadIncludedHTML ( CXMLNode * root );
    bool                    ReadIncludedExports ( CXMLNode * root );
    bool                    ReadIncludedFiles ( CXMLNode * root );
    bool                    CreateVM ( void );
    bool                    DestroyVM ( void );
    void                    TidyUp ( void );
    ResponseCode            HandleRequestActive ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, class CAccount* account );
    ResponseCode            HandleRequestCall ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, class CAccount* account );
    ResponseCode            HandleRequestInfo ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse );
    
    pthread_mutex_t         m_mutex;
    bool                    m_bDestroyed;

    inline void Lock ( void )
    {
        pthread_mutex_lock ( &m_mutex );
    }

    inline void Unlock ( void )
    {
        pthread_mutex_unlock ( &m_mutex );
    }
public:
                            CResource ( CResourceManager * resourceManager, bool bIsZipped, const char * szAbsPath, const char * szResourceName );
                            ~CResource ( );

    /* Load this resource if it's not already loaded. It needs to be loaded before it can be started. */ 
    bool                    Load ( void );

    /* Unload this resource. It needs to be stopped if it's running before this can be called or it will fail. */
    bool                    Unload ( void );

    void                    Reload ( void );

    /* Get a resource default setting */
    bool                    GetDefaultSetting           ( const char* szName, char* szValue, size_t sizeBuffer );

    /* Set a resource default setting */
    bool                    SetDefaultSetting           ( const char* szName, const char* szValue );

    /* Remove a resource default setting */
    bool                    RemoveDefaultSetting        ( const char* szName );


    /* Add a map file to a loaded resource's meta file */
    bool                    AddMapFile                  ( const char* szName, const char* szFullFilepath, int iDimension );

    /* Add a config file to a loaded resource's meta file */
    bool                    AddConfigFile               ( const char* szName, const char* szFullFilepath, int iType );

    /* To check if a file of given name and type are included in this resource */
    bool                    IncludedFileExists          ( const char* szName, int iType = CResourceFile::RESOURCE_FILE_TYPE_NONE );

    /* Remove any included file from this loaded resource */
    bool                    RemoveFile                  ( const char* szName );
    
    bool                    HasStarted                  ( void ) { return m_bHasStarted; };

    bool                    Start ( list<CResource *> * dependents = NULL, bool bStartManually = false, bool bStartIncludedResources = true, bool bConfigs = true, bool bMaps = true, bool bScripts = true, bool bHTML = true, bool bClientConfigs = true, bool bClientScripts = true, bool bClientFiles = true );
    bool                    Stop ( bool bStopManually = false );

    inline bool             IsClientConfigsOn           ( void ) { return m_bClientConfigs; };
    inline bool             IsClientScriptsOn           ( void ) { return m_bClientScripts; };
    inline bool             IsClientFilesOn             ( void ) { return m_bClientFiles; };

    bool                    StopAllResourceItems ( void );    

    CChecksum               GenerateChecksum ( void );
    inline CChecksum        GetLastChecksum ( void ) { return m_checksum; }
    bool                    HasResourceChanged ( void );
    void                    ApplyUpgradeModifications ( void );

    void                    AddTemporaryInclude ( CResource * resource );
    const std::string&      GetFailureReason ( void )
    { 
        return m_strFailureReason; 
    }
    inline CXMLNode *       GetSettingsNode ( void ) { return m_pNodeSettings; }
    inline CXMLNode *       GetStorageNode ( void ) { return m_pNodeStorage; }

    bool                    CallExportedFunction ( char * szFunctionName, CLuaArguments& args, CLuaArguments& returns, CResource& caller );

    inline list<CResource *> *  GetDependents ( void ) { return &m_dependents; }
    inline int              GetDependentCount ( void ) { return m_dependents.size(); }
    inline list<CIncludedResources *>::iterator GetIncludedResourcesBegin ( void )  { return m_includedResources.begin(); };
    inline list<CIncludedResources *>::iterator GetIncludedResourcesEnd ( void )    { return m_includedResources.end(); };
    inline int              GetIncludedResourcesCount ( void )  { return m_includedResources.size(); };
    bool                    GetInfoValue ( const char * szKey, std::string& strValue );
    void                    SetInfoValue ( const char * szKey, const char * szValue );
    inline unsigned int     GetVersionMajor ( void ) { return m_uiVersionMajor; }
    inline unsigned int     GetVersionMinor ( void ) { return m_uiVersionMinor; }
    inline unsigned int     GetVersionRevision ( void ) { return m_uiVersionRevision; }
    inline unsigned int     GetVersionState ( void ) { return m_uiVersionState; }
    inline bool             IsActive ( void ) { return m_bActive; }
    inline bool             IsLoaded ( void ) { return m_bLoaded; }
    inline bool             IsStarting ( void ) { return m_bStarting; }
    inline bool             IsStopping ( void ) { return m_bStopping; }
    inline const std::string&   GetName ( void ) { return m_strResourceName; }
    inline CLuaMain *       GetVirtualMachine ( void ) { return m_pVM; }
    void                    AddDependent ( CResource * resource );
    void                    RemoveDependent ( CResource * resource );
    bool                    IsDependentResource ( CResource * resource );
    bool                    IsDependentResource ( const char * szResourceName );
    bool                    IsDependentResourceRecursive ( CResource * resource );
    bool                    IsDependentResourceRecursive ( const char * szResourceName );
    bool                    IsIncludedResourceRecursive ( vector<CResource *> * past );
    void                    InvalidateIncludedResourceReference ( CResource * resource );
    inline bool             IsPersistent ( void ) { return m_bIsPersistent; }
    inline void             SetPersistent ( bool bPersistent ) { m_bIsPersistent = bPersistent; }
    bool                    ExtractFile ( const char * szFilename );
    bool                    DoesFileExistInZip ( const char * szFilename );
    bool                    GetFilePath ( const char * szFilename, string& strPath );
    inline const std::string&   GetResourceDirectoryPath () { return m_strResourceDirectoryPath; };
    inline const std::string&   GetResourceCacheDirectoryPath () { return m_strResourceCachePath; };
    bool                    LinkToIncludedResources ( void );
    bool                    CheckIfStartable ( void );
    inline unsigned int     GetFileCount ( void ) { return m_resourceFiles.size(); }
    void                    DisplayInfo ( void );
    char *                  DisplayInfoHTML ( char * info, size_t length );
    list<CResourceFile *>*  GetFiles ( void ) { return &m_resourceFiles; }
    CElementGroup *         GetElementGroup ( void ) { return m_pDefaultElementGroup; }
    inline time_t           GetTimeStarted ( void ) { return m_timeStarted; }
    inline time_t           GetTimeLoaded ( void ) { return m_timeLoaded; }
    void                    SetID ( unsigned short usID ) { m_usID = usID; }
    unsigned short          GetID ( void ) { return m_usID; }
    void                    OnPlayerJoin ( CPlayer& Player );
    CDummy*                 GetResourceRootElement ( void ) { return m_pResourceElement; };
    CDummy*                 GetDynamicElementRoot ( void ) { return m_pResourceDynamicElementRoot; };

    void                    SetProtected ( bool bProtected )    { m_bProtected = bProtected; };
    bool                    IsProtected ( void )                { return m_bProtected; };

    inline bool             IsResourceZip ( void )  { return m_bResourceIsZip; };

    ResponseCode            HandleRequest ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse );

    inline list < CResourceFile* >::iterator    IterBegin   ( void )        { return m_resourceFiles.begin(); }
    inline list < CResourceFile* >::iterator    IterEnd     ( void )        { return m_resourceFiles.end(); }
    inline unsigned short                       IterCount   ( void )        { return m_resourceFiles.size(); }

    inline list < CExportedFunction* >::iterator    IterBeginExportedFunctions   ( void )        { return m_exportedFunctions.begin(); }
    inline list < CExportedFunction* >::iterator    IterEndExportedFunctions     ( void )        { return m_exportedFunctions.end(); }

    static list < CResource* > m_StartedResources;

};


#endif
