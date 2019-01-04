/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResource.h
 *  PURPOSE:     Resource handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class controls a single resource, being a zip file
// or a folder that contains a number of files

#pragma once

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
    // unsigned int m_uiState;
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
    std::string m_strFunctionName;
    bool        m_bHTTPAccess;
    bool        m_bRestricted;

    eExportedFunctionType m_ucType;

public:
    CExportedFunction(const std::string& strFunctionName, bool bHTTPAccess, eExportedFunctionType ucType, bool bRestricted)
    {
        m_ucType = ucType;
        m_strFunctionName = strFunctionName;
        m_bHTTPAccess = bHTTPAccess;
        m_bRestricted = bRestricted;
    }

    eExportedFunctionType GetType() { return m_ucType; }
    const std::string&    GetFunctionName() { return m_strFunctionName; }
    bool                  IsHTTPAccessible() { return m_bHTTPAccess; }
    bool                  IsRestricted() { return m_bRestricted; }
};

class CIncludedResources
{
private:
    SString                 m_strResourceName;
    unsigned int            m_uiMinimumVersion;
    unsigned int            m_uiMaximumVersion;
    SVersion                m_MinVersion;
    SVersion                m_MaxVersion;
    bool                    m_bExists;
    bool                    m_bBadVersion;
    class CResource*        m_pResource;            // the resource this links to
    class CResource*        m_pOwner;               // the resource this is inside
    class CResourceManager* m_pResourceManager;

public:
    CIncludedResources(CResourceManager* pResourceManager, const std::string& strResourceName, SVersion svMinVersion, SVersion svMaxVersion,
                       unsigned int uiMinVersion, unsigned int uiMaxVersion, CResource* pOwner)
    {
        m_pResourceManager = pResourceManager;
        m_pOwner = pOwner;
        m_strResourceName = strResourceName;
        m_MinVersion = svMinVersion;
        m_MaxVersion = svMaxVersion;
        m_uiMinimumVersion = uiMinVersion;
        m_uiMaximumVersion = uiMaxVersion;
        m_bExists = false;
        m_pResource = nullptr;
    }

    const std::string& GetName() const { return m_strResourceName; }
    int                GetMinimumVersion() const { return m_uiMinimumVersion; }
    int                GetMaximumVersion() const { return m_uiMaximumVersion; }
    SVersion&          GetMinVersion() { return m_MinVersion; }
    const SVersion&    GetMinVersion() const { return m_MinVersion; }
    SVersion&          GetMaxVersion() { return m_MaxVersion; }
    const SVersion&    GetMaxVersion() const { return m_MaxVersion; }
    bool               DoesExist() const { return m_bExists; }
    bool               IsBadVersion() const { return m_bBadVersion; }
    CResource*         GetResource() { return m_pResource; }
    const CResource*   GetResource() const { return m_pResource; }
    bool               CreateLink();

    void InvalidateReference()
    {
        m_pResource = nullptr;
        m_bExists = false;
    };
};

enum class EResourceStatus
{
    None,
    Loaded,              // its been loaded successfully (i.e. meta parsed ok), included resources loaded ok
    Starting,            // the resource is starting
    Running,             // resource items are running
    Stopping,            // the resource is stopping
};

class CResource : public EHS
{
    using KeyValueMap = CFastHashMap<SString, SString>;

public:
    ZERO_ON_NEW
    CResource(CResourceManager* pResourceManager, bool bIsZipped, const char* szAbsPath, const char* szResourceName);
    ~CResource();

    /* Load this resource if it's not already loaded. It needs to be loaded before it can be started. */
    bool Load();

    /* Unload this resource. It needs to be stopped if it's running before this can be called or it will fail. */
    bool Unload();

    void Reload();

    /* Get a resource default setting */
    bool GetDefaultSetting(const char* szName, char* szValue, size_t sizeBuffer);

    /* Set a resource default setting */
    bool SetDefaultSetting(const char* szName, const char* szValue);

    /* Remove a resource default setting */
    bool RemoveDefaultSetting(const char* szName);

    /* Add a map file to a loaded resource's meta file */
    bool AddMapFile(const char* szName, const char* szFullFilepath, int iDimension);

    /* Add a config file to a loaded resource's meta file */
    bool AddConfigFile(const char* szName, const char* szFullFilepath, int iType);

    /* To check if a file of given name and type are included in this resource */
    bool IncludedFileExists(const char* szName, int iType = CResourceFile::RESOURCE_FILE_TYPE_NONE);

    /* Remove any included file from this loaded resource */
    bool RemoveFile(const char* szName);

    bool Start(std::list<CResource*>* dependents = nullptr, bool bStartManually = false, bool bStartIncludedResources = true, bool bConfigs = true,
               bool bMaps = true, bool bScripts = true, bool bHTML = true, bool bClientConfigs = true, bool bClientScripts = true, bool bClientFiles = true);
    bool Stop(bool bStopManually = false);

    bool IsClientConfigsOn() { return m_bClientConfigs; }
    bool IsClientScriptsOn() { return m_bClientScripts; }
    bool IsClientFilesOn() { return m_bClientFiles; }

    bool             GenerateChecksums();
    const CChecksum& GetLastMetaChecksum() { return m_metaChecksum; }
    bool             HasResourceChanged();
    void             ApplyUpgradeModifications();
    void             LogUpgradeWarnings();
    bool             GetCompatibilityStatus(SString& strOutStatus);

    void      AddTemporaryInclude(CResource* resource);
    SString   GetFailureReason() { return m_strFailureReason.TrimEnd("\n"); }
    CXMLNode* GetSettingsNode() { return m_pNodeSettings; }
    CXMLNode* GetStorageNode() { return m_pNodeStorage; }

    bool CallExportedFunction(const char* szFunctionName, CLuaArguments& Arguments, CLuaArguments& Returns, CResource& Caller);

    std::list<CResource*>*                   GetDependents() { return &m_Dependents; }
    int                                      GetDependentCount() { return m_Dependents.size(); }
    std::list<CIncludedResources*>::iterator GetIncludedResourcesBegin() { return m_IncludedResources.begin(); }
    std::list<CIncludedResources*>::iterator GetIncludedResourcesEnd() { return m_IncludedResources.end(); }
    int                                      GetIncludedResourcesCount() { return m_IncludedResources.size(); }
    bool                                     GetInfoValue(const char* szKey, std::string& strValue);
    void                                     SetInfoValue(const char* szKey, const char* szValue, bool bSave = true);
    unsigned int                             GetVersionMajor() { return m_uiVersionMajor; }
    unsigned int                             GetVersionMinor() { return m_uiVersionMinor; }
    unsigned int                             GetVersionRevision() { return m_uiVersionRevision; }
    unsigned int                             GetVersionState() { return m_uiVersionState; }

    EResourceStatus GetStatus() const noexcept { return m_eStatus; }
    bool            IsLoaded() const noexcept { return m_eStatus != EResourceStatus::None; }
    bool            IsActive() const noexcept { return m_eStatus != EResourceStatus::None && m_eStatus != EResourceStatus::Loaded; }
    bool            IsStarting() const noexcept { return m_eStatus == EResourceStatus::Starting; }
    bool            IsStopping() const noexcept { return m_eStatus == EResourceStatus::Stopping; }
    bool            HasStarted() const noexcept { return m_eStatus == EResourceStatus::Running; }

    const SString& GetName() { return m_strResourceName; }
    CLuaMain*      GetVirtualMachine() { return m_pVM; }

    void AddDependent(CResource* pResource);
    void RemoveDependent(CResource* pResource);
    bool IsDependentResource(CResource* pResource);
    bool IsDependentResource(const char* szResourceName);
    bool IsDependentResourceRecursive(CResource* pResource);
    bool IsDependentResourceRecursive(const char* szResourceName);

    bool                       IsIncludedResourceRecursive(std::vector<CResource*>* past);
    void                       InvalidateIncludedResourceReference(CResource* pRresource);
    bool                       IsPersistent() { return m_bIsPersistent; }
    void                       SetPersistent(bool bPersistent) { m_bIsPersistent = bPersistent; }
    bool                       ExtractFile(const char* szFilename);
    bool                       DoesFileExistInZip(const char* szFilename);
    bool                       HasGoneAway();
    bool                       GetFilePath(const char* szFilename, std::string& strPath);
    const std::string&         GetResourceDirectoryPath() { return m_strResourceDirectoryPath; }
    const std::string&         GetResourceCacheDirectoryPath() { return m_strResourceCachePath; }
    bool                       LinkToIncludedResources();
    bool                       CheckIfStartable();
    unsigned int               GetFileCount() { return m_ResourceFiles.size(); }
    void                       DisplayInfo();
    std::list<CResourceFile*>* GetFiles() { return &m_ResourceFiles; }
    CElementGroup*             GetElementGroup() { return m_pDefaultElementGroup; }
    time_t                     GetTimeStarted() { return m_timeStarted; }
    time_t                     GetTimeLoaded() { return m_timeLoaded; }
    void                       SetNetID(unsigned short usNetID) { m_usNetID = usNetID; }
    unsigned short             GetNetID() { return m_usNetID; }
    uint                       GetScriptID() const { return m_uiScriptID; }
    void                       OnPlayerJoin(CPlayer& Player);
    void                       SendNoClientCacheScripts(CPlayer* pPlayer = nullptr);
    class CDummy*              GetResourceRootElement() { return m_pResourceElement; }
    CDummy*                    GetDynamicElementRoot() { return m_pResourceDynamicElementRoot; }

    void SetProtected(bool bProtected) { m_bProtected = bProtected; }
    bool IsProtected() { return m_bProtected; }

    bool IsResourceZip() { return m_bResourceIsZip; }

    ResponseCode HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);

    list<CResourceFile*>::iterator IterBegin() { return m_ResourceFiles.begin(); }
    list<CResourceFile*>::iterator IterEnd() { return m_ResourceFiles.end(); }
    size_t                         IterCount() { return m_ResourceFiles.size(); }

    list<CExportedFunction>::iterator IterBeginExportedFunctions() { return m_ExportedFunctions.begin(); }
    list<CExportedFunction>::iterator IterEndExportedFunctions() { return m_ExportedFunctions.end(); }

    static list<CResource*> m_StartedResources;

    void           GetAclRequests(std::vector<SAclRequest>& outResultList);
    bool           HandleAclRequestListCommand(bool bDetail);
    bool           HandleAclRequestChangeCommand(const SString& strRightName, bool bAccess, const SString& strWho);
    bool           HandleAclRequestChange(const CAclRightName& strRightName, bool bAccess, const SString& strWho);
    const SString& GetMinServerReqFromMetaXml() { return m_strMinServerReqFromMetaXml; }
    const SString& GetMinClientReqFromMetaXml() { return m_strMinClientReqFromMetaXml; }
    bool           IsOOPEnabledInMetaXml() { return m_bOOPEnabledInMetaXml; }
    bool           CheckFunctionRightCache(lua_CFunction f, bool* pbOutAllowed);
    void           UpdateFunctionRightCache(lua_CFunction f, bool bAllowed);
    bool           IsFilenameUsed(const SString& strFilename, bool bClient);
    int            GetDownloadPriorityGroup() { return m_iDownloadPriorityGroup; }
    void           SetUsingDbConnectMysql(bool bUsingDbConnectMysql) { m_bUsingDbConnectMysql = bUsingDbConnectMysql; }
    bool           IsUsingDbConnectMysql();
    bool           IsFileDbConnectMysqlProtected(const SString& strFilename, bool bReadOnly);

protected:
    SString             GetAutoGroupName();
    SString             GetAutoAclName();
    CAccessControlList* GetAutoAcl();
    CAccessControlList* FindAutoAcl();

    void RemoveAutoPermissions();
    bool HasAutoPermissions(CXMLNode* pNodeAclRequest);
    void RefreshAutoPermissions(CXMLNode* pNodeAclRequest);

    void CommitAclRequest(const SAclRequest& request);
    bool FindAclRequest(SAclRequest& request);

private:
    bool         CheckState();            // if the resource has no Dependents, stop it, if it has, start it. returns true if the resource is started.
    bool         ReadIncludedResources(class CXMLNode* pRoot);
    bool         ReadIncludedMaps(CXMLNode* pRoot);
    bool         ReadIncludedScripts(CXMLNode* pRoot);
    bool         ReadIncludedConfigs(CXMLNode* pRoot);
    bool         ReadIncludedHTML(CXMLNode* pRoot);
    bool         ReadIncludedExports(CXMLNode* pRoot);
    bool         ReadIncludedFiles(CXMLNode* pRoot);
    bool         CreateVM(bool bEnableOOP);
    bool         DestroyVM();
    void         TidyUp();
    ResponseCode HandleRequestActive(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, class CAccount* pAccount);
    ResponseCode HandleRequestCall(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, class CAccount* pAccount);
    void         Lock() { pthread_mutex_lock(&m_mutex); }
    void         Unlock() { pthread_mutex_unlock(&m_mutex); }

    bool        m_bHandlingHTTPRequest;
    bool        m_bResourceIsZip;
    SString     m_strResourceName;
    SString     m_strAbsPath;                          // Absolute path to containing directory        i.e. /server/mods/deathmatch/resources
    std::string m_strResourceZip;                      // Absolute path to zip file (if a zip)         i.e. m_strAbsPath/resource_name.zip
    std::string m_strResourceDirectoryPath;            // Absolute path to resource files (if a dir)   i.e. m_strAbsPath/resource_name
    std::string m_strResourceCachePath;            // Absolute path to unzipped cache (if a zip)   i.e. /server/mods/deathmatch/resources/cache/resource_name

    unsigned int m_uiVersionMajor;
    unsigned int m_uiVersionMinor;
    unsigned int m_uiVersionRevision;
    unsigned int m_uiVersionState;

    time_t m_timeLoaded;
    time_t m_timeStarted;

    class CElement* m_pRootElement;
    class CDummy*   m_pResourceElement;
    class CDummy*   m_pResourceDynamicElementRoot;

    KeyValueMap                    m_Info;
    std::list<CIncludedResources*> m_IncludedResources;            // we store them here temporarily, then read them once all the resources are loaded
    std::list<CResourceFile*>      m_ResourceFiles;
    std::list<CResource*>          m_Dependents;            // resources that have "included" or loaded this one
    std::list<CExportedFunction>   m_ExportedFunctions;
    std::list<CResource*>          m_TemporaryIncludes;            // started by startResource script command

    CElementGroup* m_pDefaultElementGroup;            // stores elements created by scripts in this resource

    std::string     m_strCircularInclude;
    EResourceStatus m_eStatus = EResourceStatus::None;
    bool            m_bIsPersistent;            // if true, the resource will remain even if it has no Dependents, mainly if started by the user or the startup
    bool            m_bLinked;                  // if true, the included resources are already linked to this resource
    unzFile         m_zipfile;
    SString         m_strFailureReason;

    bool m_bClientConfigs;
    bool m_bClientScripts;
    bool m_bClientFiles;
    bool m_bDoneUpgradeWarnings;

    class CResourceManager* m_pResourceManager;
    class CLuaMain*         m_pVM;

    CXMLNode* m_pNodeSettings;            // Settings XML node, read from meta.xml and copied into it's own instance
    CXMLNode* m_pNodeStorage;             // Dummy XML node used for temporary storage of stuff returned by CSettings::Get

    SString m_strMinClientReqFromMetaXml;            // Min MTA client version as declared in meta.xml
    SString m_strMinServerReqFromMetaXml;            // Min MTA server version as declared in meta.xml
    SString m_strMinClientReqFromSource;             // Min MTA client version as calculated by scanning the script source
    SString m_strMinServerReqFromSource;             // Min MTA server version as calculated by scanning the script source
    SString m_strMinClientReason;
    SString m_strMinServerReason;

    bool m_bSyncMapElementData;
    bool m_bSyncMapElementDataDefined;

    CChecksum m_metaChecksum;            // Checksum of meta.xml last time this was loaded, generated in GenerateChecksums()

    unsigned short m_usNetID;            // resource ID
    uint           m_uiScriptID;

    bool m_bProtected;
    bool m_bStartedManually;
    int  m_iDownloadPriorityGroup;

    bool                              m_bOOPEnabledInMetaXml;
    uint                              m_uiFunctionRightCacheRevision;
    CFastHashMap<lua_CFunction, bool> m_FunctionRightCacheMap;
    bool                              m_bDoneDbConnectMysqlScan;
    bool                              m_bUsingDbConnectMysql;

    pthread_mutex_t m_mutex;
    bool            m_bDestroyed;
};
