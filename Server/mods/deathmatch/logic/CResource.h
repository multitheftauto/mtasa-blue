/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResource.h
 *  PURPOSE:     Resource handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "packets/CResourceStartPacket.h"
#include "packets/CResourceStopPacket.h"
#include "packets/CEntityRemovePacket.h"
#include "SResourceStartOptions.h"
#include "CResourceFile.h"
#include "CAclRightName.h"
#include <unzip.h>
#include <list>
#include <vector>
#include <ehs/ehs.h>
#include <time.h>

#define MAX_AUTHOR_LENGTH           255
#define MAX_RESOURCE_NAME_LENGTH    255
#define MAX_FUNCTION_NAME_LENGTH    50

class CDummy;
class CElement;
class CElementGroup;
class CXMLNode;
class CAccount;
class CLuaMain;
class CResourceManager;
class CChecksum;

struct SVersion
{
    unsigned int m_uiMajor = 0;
    unsigned int m_uiMinor = 0;
    unsigned int m_uiRevision = 0;
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
    SString           m_strResourceName;
    unsigned int      m_uiMinimumVersion;
    unsigned int      m_uiMaximumVersion;
    SVersion          m_MinVersion;
    SVersion          m_MaxVersion;
    bool              m_bExists;
    bool              m_bBadVersion;
    CResource*        m_pResource;            // the resource this links to
    CResource*        m_pOwner;               // the resource this is inside
    CResourceManager* m_pResourceManager;

public:
    CIncludedResources(CResourceManager* pResourceManager, const std::string& strResourceName, SVersion svMinVersion, SVersion svMaxVersion, CResource* pOwner)
    {
        m_pResourceManager = pResourceManager;
        m_pOwner = pOwner;
        m_strResourceName = strResourceName;
        m_MinVersion = svMinVersion;
        m_MaxVersion = svMaxVersion;
        m_uiMinimumVersion = svMinVersion.m_uiMajor;
        m_uiMaximumVersion = svMaxVersion.m_uiMajor;
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

enum class EResourceState : unsigned char
{
    None,
    Loaded,              // its been loaded successfully (i.e. meta parsed ok), included resources loaded ok
    Starting,            // the resource is starting
    Running,             // resource items are running
    Stopping,            // the resource is stopping
};

// A resource is either a directory with files or a ZIP file which contains the content of such directory.
// The directory or ZIP file must contain a meta.xml file, which describes the required content by the resource.
// It's a process-like environment for scripts, maps, images and other files.
class CResource : public EHS
{
    using KeyValueMap = CFastHashMap<SString, SString>;

public:
    ZERO_ON_NEW
    CResource(CResourceManager* pResourceManager, bool bIsZipped, const char* szAbsPath, const char* szResourceName);
    ~CResource();

    // Load this resource if it's not already loaded.
    // It needs to be loaded before it can be started.
    bool Load();

    // Unload this resource.
    // It will be stopped if it's running before this can be called or it will fail.
    bool Unload();

    void Reload();

    // Get a resource default setting
    bool GetDefaultSetting(const char* szName, char* szValue, size_t sizeBuffer);

    // Set a resource default setting
    bool SetDefaultSetting(const char* szName, const char* szValue);

    // Remove a resource default setting
    bool RemoveDefaultSetting(const char* szName);

    // Add a map file to a loaded resource's meta file
    bool AddMapFile(const char* szName, const char* szFullFilepath, int iDimension);

    // Add a config file to a loaded resource's meta file
    bool AddConfigFile(const char* szName, const char* szFullFilepath, int iType);

    // To check if a file of given name and type are included in this resource
    bool IncludedFileExists(const char* szName, int iType = CResourceFile::RESOURCE_FILE_TYPE_NONE);

    // Remove any included file from this loaded resource
    bool RemoveFile(const char* szName);

    bool Start(std::list<CResource*>* pDependents = nullptr, bool bManualStart = false, const SResourceStartOptions& StartOptions = SResourceStartOptions());
    bool Stop(bool bManualStop = false);

    bool IsClientConfigsOn() const noexcept { return m_bClientConfigs; }
    bool IsClientScriptsOn() const noexcept { return m_bClientScripts; }
    bool IsClientFilesOn() const noexcept { return m_bClientFiles; }

    bool                 GenerateChecksums();
    std::future<SString> GenerateChecksumForFile(CResourceFile* pResourceFile);
    const CChecksum&     GetLastMetaChecksum() { return m_metaChecksum; }
    bool                 HasResourceChanged();
    void                 ApplyUpgradeModifications();
    void                 LogUpgradeWarnings();
    bool                 GetCompatibilityStatus(SString& strOutStatus);

    void      AddTemporaryInclude(CResource* resource);
    SString   GetFailureReason() { return m_strFailureReason.TrimEnd("\n"); }
    CXMLNode* GetSettingsNode() { return m_pNodeSettings; }
    CXMLNode* GetStorageNode() { return m_pNodeStorage; }

    bool CallExportedFunction(const char* szFunctionName, CLuaArguments& Arguments, CLuaArguments& Returns, CResource& Caller);

    std::list<CResource*>& GetDependents() { return m_Dependents; }
    int                    GetDependentCount() const noexcept { return m_Dependents.size(); }

    std::list<CIncludedResources*>::iterator       GetIncludedResourcesBegin() { return m_IncludedResources.begin(); }
    std::list<CIncludedResources*>::const_iterator GetIncludedResourcesBegin() const noexcept { return m_IncludedResources.begin(); }

    std::list<CIncludedResources*>::iterator       GetIncludedResourcesEnd() { return m_IncludedResources.end(); }
    std::list<CIncludedResources*>::const_iterator GetIncludedResourcesEnd() const noexcept { return m_IncludedResources.end(); }

    size_t GetIncludedResourcesCount() const noexcept { return m_IncludedResources.size(); }

    bool GetInfoValue(const char* szKey, std::string& strValue) const;
    void SetInfoValue(const char* szKey, const char* szValue, bool bSave = true);

    unsigned int GetVersionMajor() const noexcept { return m_uiVersionMajor; }
    unsigned int GetVersionMinor() const noexcept { return m_uiVersionMinor; }
    unsigned int GetVersionRevision() const noexcept { return m_uiVersionRevision; }
    unsigned int GetVersionState() const noexcept { return m_uiVersionState; }

    bool IsLoaded() const noexcept { return m_eState != EResourceState::None; }
    bool IsActive() const noexcept
    {
        return m_eState == EResourceState::Starting || m_eState == EResourceState::Running || m_eState == EResourceState::Stopping;
    }
    bool IsStarting() const noexcept { return m_eState == EResourceState::Starting; }
    bool IsStopping() const noexcept { return m_eState == EResourceState::Stopping; }

    bool IsClientSynced() const noexcept { return m_bClientSync; }

    const SString& GetName() const noexcept { return m_strResourceName; }

    CLuaMain*       GetVirtualMachine() { return m_pVM; }
    const CLuaMain* GetVirtualMachine() const { return m_pVM; }

    void AddDependent(CResource* pResource);
    void RemoveDependent(CResource* pResource);
    bool IsDependentResource(CResource* pResource);
    bool IsDependentResource(const char* szResourceName);
    bool IsDependentResourceRecursive(CResource* pResource);
    bool IsDependentResourceRecursive(const char* szResourceName);

    bool IsIncludedResourceRecursive(std::vector<CResource*>* past);
    void InvalidateIncludedResourceReference(CResource* pRresource);

    bool IsPersistent() const noexcept { return m_bIsPersistent; }
    void SetPersistent(bool bPersistent) { m_bIsPersistent = bPersistent; }

    bool ExtractFile(const char* szFilename);
    bool DoesFileExistInZip(const char* szFilename);
    bool HasGoneAway();
    bool LinkToIncludedResources();
    bool CheckIfStartable();
    void DisplayInfo();

    bool               GetFilePath(const char* szFilename, std::string& strPath);
    const std::string& GetResourceDirectoryPath() const { return m_strResourceDirectoryPath; }
    const std::string& GetResourceCacheDirectoryPath() const { return m_strResourceCachePath; }

    std::list<CResourceFile*>& GetFiles() { return m_ResourceFiles; }
    size_t                     GetFileCount() const noexcept { return m_ResourceFiles.size(); }

    time_t GetTimeStarted() const noexcept { return m_timeStarted; }
    time_t GetTimeLoaded() const noexcept { return m_timeLoaded; }

    void           SetNetID(unsigned short usNetID) { m_usNetID = usNetID; }
    unsigned short GetNetID() const noexcept { return m_usNetID; }

    uint GetScriptID() const noexcept { return m_uiScriptID; }

    void OnPlayerJoin(CPlayer& Player);
    void SendNoClientCacheScripts(CPlayer* pPlayer = nullptr);

    CDummy*       GetResourceRootElement() { return m_pResourceElement; }
    const CDummy* GetResourceRootElement() const noexcept { return m_pResourceElement; }

    CDummy*       GetDynamicElementRoot() { return m_pResourceDynamicElementRoot; }
    const CDummy* GetDynamicElementRoot() const noexcept { return m_pResourceDynamicElementRoot; }

    CElementGroup*       GetElementGroup() { return m_pDefaultElementGroup; }
    const CElementGroup* GetElementGroup() const noexcept { return m_pDefaultElementGroup; }

    void SetProtected(bool bProtected) { m_bProtected = bProtected; }
    bool IsProtected() const noexcept { return m_bProtected; }

    bool IsResourceZip() const noexcept { return m_bResourceIsZip; }
    bool UnzipResource();

    ResponseCode HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);

    std::list<CResourceFile*>::iterator       IterBegin() { return m_ResourceFiles.begin(); }
    std::list<CResourceFile*>::const_iterator IterBegin() const noexcept { return m_ResourceFiles.begin(); }

    std::list<CResourceFile*>::iterator       IterEnd() { return m_ResourceFiles.end(); }
    std::list<CResourceFile*>::const_iterator IterEnd() const noexcept { return m_ResourceFiles.end(); }

    size_t IterCount() const noexcept { return m_ResourceFiles.size(); }

    std::list<CExportedFunction>::iterator IterBeginExportedFunctions() { return m_ExportedFunctions.begin(); }
    std::list<CExportedFunction>::iterator IterEndExportedFunctions() { return m_ExportedFunctions.end(); }

    void GetAclRequests(std::vector<SAclRequest>& outResultList);
    bool HandleAclRequestListCommand(bool bDetail);
    bool HandleAclRequestChangeCommand(const SString& strRightName, bool bAccess, const SString& strWho);
    bool HandleAclRequestChange(const CAclRightName& strRightName, bool bAccess, const SString& strWho);

    const CMtaVersion& GetMinServerRequirement() const noexcept { return m_strMinServerRequirement; }
    const CMtaVersion& GetMinClientRequirement() const noexcept { return m_strMinClientRequirement; }
    const CMtaVersion& GetMinServerFromMetaXml() const noexcept { return m_strMinServerFromMetaXml; }
    const CMtaVersion& GetMinClientFromMetaXml() const noexcept { return m_strMinClientFromMetaXml; }

    bool IsOOPEnabledInMetaXml() const noexcept { return m_bOOPEnabledInMetaXml; }

    bool CheckFunctionRightCache(lua_CFunction f, bool* pbOutAllowed);
    void UpdateFunctionRightCache(lua_CFunction f, bool bAllowed);

    bool IsFilenameUsed(const SString& strFilename, bool bClient);

    int GetDownloadPriorityGroup() const noexcept { return m_iDownloadPriorityGroup; }

    void SetUsingDbConnectMysql(bool bUsingDbConnectMysql) { m_bUsingDbConnectMysql = bUsingDbConnectMysql; }
    bool IsUsingDbConnectMysql();
    bool IsFileDbConnectMysqlProtected(const SString& strFilename, bool bReadOnly);

    /**
     * @brief Searches for a CResourceFile with the given relative path.
     * @param relativePath Relative resource file path (from meta)
     * @return A pointer to CResourceFile on success, null otherwise
     */
    CResourceFile* GetResourceFile(const SString& relativePath) const;
    
    ELuaVersion GetLuaVersion() const { return m_LuaVersion; }
    ELuaVersion GetClientLuaVersion() const { return m_ClientLuaVersion; }

public:
    static std::list<CResource*> m_StartedResources;

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
    bool CheckState();            // if the resource has no Dependents, stop it, if it has, start it. returns true if the resource is started.
    bool ReadIncludedResources(CXMLNode* pRoot);
    bool ReadIncludedMaps(CXMLNode* pRoot);
    bool ReadIncludedScripts(CXMLNode* pRoot);
    bool ReadIncludedConfigs(CXMLNode* pRoot);
    bool ReadIncludedHTML(CXMLNode* pRoot);
    bool ReadIncludedExports(CXMLNode* pRoot);
    bool ReadIncludedFiles(CXMLNode* pRoot);
    bool CreateVM(bool bEnableOOP);
    bool DestroyVM();
    void TidyUp();

    ResponseCode HandleRequestActive(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, CAccount* pAccount);
    ResponseCode HandleRequestCall(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, CAccount* pAccount);
    bool         IsHttpAccessAllowed(CAccount* pAccount);

private:
    EResourceState m_eState = EResourceState::None;
    bool           m_bClientSync = false;

    unsigned short m_usNetID = -1;
    uint           m_uiScriptID = -1;

    CResourceManager* m_pResourceManager;

    SString     m_strResourceName;
    SString     m_strAbsPath;                          // Absolute path to containing directory        i.e. /server/mods/deathmatch/resources
    std::string m_strResourceZip;                      // Absolute path to zip file (if a zip)         i.e. m_strAbsPath/resource_name.zip
    std::string m_strResourceDirectoryPath;            // Absolute path to resource files (if a dir)   i.e. m_strAbsPath/resource_name
    std::string m_strResourceCachePath;            // Absolute path to unzipped cache (if a zip)   i.e. /server/mods/deathmatch/resources/cache/resource_name

    unsigned int m_uiVersionMajor = 0;
    unsigned int m_uiVersionMinor = 0;
    unsigned int m_uiVersionRevision = 0;
    unsigned int m_uiVersionState = 2;            // 2 = release

    int m_iDownloadPriorityGroup = 0;

    time_t m_timeLoaded = 0;
    time_t m_timeStarted = 0;

    CElement*      m_pRootElement = nullptr;
    CDummy*        m_pResourceElement = nullptr;
    CDummy*        m_pResourceDynamicElementRoot = nullptr;
    CElementGroup* m_pDefaultElementGroup = nullptr;            // stores elements created by scripts in this resource
    CLuaMain*      m_pVM = nullptr;

    KeyValueMap                    m_Info;
    std::list<CIncludedResources*> m_IncludedResources;            // we store them here temporarily, then read them once all the resources are loaded
    std::list<CResourceFile*>      m_ResourceFiles;
    std::list<CResource*>          m_Dependents;            // resources that have "included" or loaded this one
    std::list<CExportedFunction>   m_ExportedFunctions;
    std::list<CResource*>          m_TemporaryIncludes;            // started by startResource script command

    std::string m_strCircularInclude;
    SString     m_strFailureReason;
    unzFile     m_zipfile = nullptr;
    CChecksum   m_zipHash;

    bool m_bResourceIsZip;
    bool m_bClientConfigs = true;
    bool m_bClientScripts = true;
    bool m_bClientFiles = true;

    bool m_bProtected = false;
    bool m_bStartedManually = false;
    bool m_bSyncMapElementData = true;
    bool m_bSyncMapElementDataDefined = false;

    bool m_bHandlingHTTPRequest = false;
    bool m_bDoneUpgradeWarnings = false;
    bool m_bDoneDbConnectMysqlScan = false;
    bool m_bUsingDbConnectMysql = false;

    bool m_bOOPEnabledInMetaXml = false;
    bool m_bLinked = false;                  // if true, the included resources are already linked to this resource
    bool m_bIsPersistent = false;            // if true, the resource will remain even if it has no Dependents, mainly if started by the user or the startup
    bool m_bDestroyed = false;

    ELuaVersion m_LuaVersion = ELuaVersion::VLUA_5_1;
    ELuaVersion m_ClientLuaVersion = ELuaVersion::VLUA_5_1;

    CXMLNode* m_pNodeSettings = nullptr;            // Settings XML node, read from meta.xml and copied into it's own instance
    CXMLNode* m_pNodeStorage = nullptr;             // Dummy XML node used for temporary storage of stuff returned by CSettings::Get

    CMtaVersion m_strMinClientRequirement;              // Min MTA client version
    CMtaVersion m_strMinServerRequirement;              // Min MTA server version
    CMtaVersion m_strMinClientFromMetaXml;              // Min MTA client version as declared in meta.xml
    CMtaVersion m_strMinServerFromMetaXml;              // Min MTA server version as declared in meta.xml
    CMtaVersion m_strMinClientReqFromSource;            // Min MTA client version as calculated by scanning the script source
    CMtaVersion m_strMinServerReqFromSource;            // Min MTA server version as calculated by scanning the script source
    SString     m_strMinClientReason;
    SString     m_strMinServerReason;

    CChecksum m_metaChecksum;            // Checksum of meta.xml last time this was loaded, generated in GenerateChecksums()

    uint                              m_uiFunctionRightCacheRevision = 0;
    CFastHashMap<lua_CFunction, bool> m_FunctionRightCacheMap;
};
