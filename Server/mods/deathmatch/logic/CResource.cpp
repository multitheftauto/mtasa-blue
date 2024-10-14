/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResource.cpp
 *  PURPOSE:     Resource handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// Show info about where the actual files are coming from
// #define RESOURCE_DEBUG_MESSAGES

#include "StdInc.h"
#include "CResource.h"
#include "CResourceManager.h"
#include "CResourceChecker.h"
#include "CResourceHTMLItem.h"
#include "CResourceConfigItem.h"
#include "CResourceClientConfigItem.h"
#include "CResourceClientFileItem.h"
#include "CResourceScriptItem.h"
#include "CResourceClientScriptItem.h"
#include "CAccessControlListManager.h"
#include "CScriptDebugging.h"
#include "CMapManager.h"
#include "CKeyBinds.h"
#include "CIdArray.h"
#include "CChecksum.h"
#include "CHTTPD.h"
#include "Utils.h"
#include "packets/CResourceClientScriptsPacket.h"
#include "lua/CLuaFunctionParseHelpers.h"
#include <net/SimHeaders.h>
#include <zip.h>
#include <glob/glob.h>

#ifdef WIN32
    #include <zip/iowin32.h>
#else
    #include <utime.h>
#endif

#ifndef MAX_PATH
    #define MAX_PATH 260
#endif

enum HttpRouterCheck
{
    HTTP_ROUTER_CHECK_PENDING,
    HTTP_ROUTER_CHECK_PASSED,
    HTTP_ROUTER_CHECK_FAILED,
};

int           do_extract_currentfile(unzFile uf, const int* popt_extract_without_path, int* popt_overwrite, const char* password, const char* szFilePath);
unsigned long get_current_file_crc(unzFile uf);

std::list<CResource*> CResource::m_StartedResources;

extern CServerInterface* g_pServerInterface;
extern CGame*            g_pGame;

#ifdef NO_ERRNO_H
extern int errno;
#endif

//
// Helper function to avoid fopen in Window builds
//
static unzFile unzOpenUtf8(const char* path)
{
#ifdef WIN32
    // This will use CreateFile instead of fopen
    zlib_filefunc_def ffunc;
    fill_win32_filefunc(&ffunc);
    return unzOpen2(path, &ffunc);
#else
    return unzOpen(path);
#endif
}

CResource::CResource(CResourceManager* pResourceManager, bool bIsZipped, const char* szAbsPath, const char* szResourceName)
    : m_pResourceManager(pResourceManager), m_bResourceIsZip(bIsZipped), m_strResourceName(SStringX(szResourceName)), m_strAbsPath(SStringX(szAbsPath))
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::RESOURCE);

    Load();
}

bool CResource::Load()
{
    if (m_eState != EResourceState::None)
        return true;

    m_strCircularInclude = "";
    m_metaChecksum = CChecksum();
    m_bIsPersistent = false;
    m_bLinked = false;
    m_pResourceElement = nullptr;
    m_pResourceDynamicElementRoot = nullptr;
    m_bProtected = false;
    m_bStartedManually = false;
    m_bDoneDbConnectMysqlScan = false;

    m_uiVersionMajor = 0;
    m_uiVersionMinor = 0;
    m_uiVersionRevision = 0;
    m_uiVersionState = 2;            // release

    m_bClientConfigs = true;
    m_bClientScripts = true;
    m_bClientFiles = true;

    m_bOOPEnabledInMetaXml = false;

    m_pVM = nullptr;
    // @@@@@ Set some type of HTTP access here

    // Register the time we loaded this resource and zero out the time we started it
    time(&m_timeLoaded);
    m_timeStarted = 0;

    // Register us in the EHS stuff
    g_pGame->GetHTTPD()->RegisterEHS(this, m_strResourceName.c_str());
    this->m_oEHSServerParameters["norouterequest"] = true;

    // Store the actual directory and zip paths for fast access
    m_strResourceDirectoryPath = PathJoin(m_strAbsPath, m_strResourceName, "/");
    m_strResourceCachePath = PathJoin(g_pServerInterface->GetServerModPath(), "resource-cache", "unzipped", m_strResourceName, "/");
    m_strResourceZip = PathJoin(m_strAbsPath, m_strResourceName + ".zip");

    if (m_bResourceIsZip)
    {
        if (!UnzipResource())
        {
            // Unregister EHS stuff
            g_pGame->GetHTTPD()->UnregisterEHS(m_strResourceName.c_str());

            return false;
        }
    }

    // Load the meta.xml file
    string strMeta;
    if (!GetFilePath("meta.xml", strMeta))
    {
        // Unregister the EHS stuff
        g_pGame->GetHTTPD()->UnregisterEHS(m_strResourceName.c_str());

        // Show error
        m_strFailureReason = SString("Couldn't find meta.xml file for resource '%s'\n", m_strResourceName.c_str());
        CLogger::ErrorPrintf(m_strFailureReason);
        return false;
    }

    // Load the XML file and parse it
    CXMLFile* pMetaFile = g_pServerInterface->GetXML()->CreateXML(strMeta.c_str());
    bool      bParsedSuccessfully = pMetaFile ? pMetaFile->Parse() : false;

    if (bParsedSuccessfully)
    {
        CXMLNode* pRoot = pMetaFile->GetRootNode();

        if (pRoot)
        {
            // Find the settings node and copy it (meta xml is deleted at the end of this function, to preserve memory)
            CXMLNode* pNodeSettings = pRoot->FindSubNode("settings", 0);

            if (pNodeSettings)
                m_pNodeSettings = pNodeSettings->CopyNode(nullptr);

            // Find the client and server version requirements
            m_strMinClientFromMetaXml = "";
            m_strMinServerFromMetaXml = "";
            CXMLNode* pNodeMinMtaVersion = pRoot->FindSubNode("min_mta_version", 0);

            if (pNodeMinMtaVersion)
            {
                if (CXMLAttribute* pAttr = pNodeMinMtaVersion->GetAttributes().Find("server"))
                    m_strMinServerFromMetaXml = pAttr->GetValue();
                if (CXMLAttribute* pAttr = pNodeMinMtaVersion->GetAttributes().Find("client"))
                    m_strMinClientFromMetaXml = pAttr->GetValue();
                if (CXMLAttribute* pAttr = pNodeMinMtaVersion->GetAttributes().Find("both"))
                    m_strMinServerFromMetaXml = m_strMinClientFromMetaXml = pAttr->GetValue();
            }
            m_strMinServerRequirement = m_strMinServerFromMetaXml;
            m_strMinClientRequirement = m_strMinClientFromMetaXml;

            // Find the acl requets
            CXMLNode* pNodeAclRequest = pRoot->FindSubNode("aclrequest", 0);

            if (pNodeAclRequest)
                RefreshAutoPermissions(pNodeAclRequest);
            else
                RemoveAutoPermissions();

            // Find any map sync option
            m_bSyncMapElementData = true;
            m_bSyncMapElementDataDefined = false;
            CXMLNode* pNodeSyncMapElementData = pRoot->FindSubNode("sync_map_element_data", 0);

            if (pNodeSyncMapElementData)
            {
                m_bSyncMapElementData = StringToBool(pNodeSyncMapElementData->GetTagContent().c_str());
                m_bSyncMapElementDataDefined = true;
            }

            m_bOOPEnabledInMetaXml = false;
            CXMLNode* pNodeClientOOP = pRoot->FindSubNode("oop", 0);

            if (pNodeClientOOP)
            {
                m_bOOPEnabledInMetaXml = StringToBool(pNodeClientOOP->GetTagContent().c_str());
            }

            m_iDownloadPriorityGroup = 0;
            CXMLNode* pNodeDownloadPriorityGroup = pRoot->FindSubNode("download_priority_group", 0);

            if (pNodeDownloadPriorityGroup)
            {
                m_iDownloadPriorityGroup = atoi(pNodeDownloadPriorityGroup->GetTagContent().c_str());
            }

            CXMLNode* pInfo = pRoot->FindSubNode("info", 0);

            if (pInfo)
            {
                CXMLAttributes& Attributes = pInfo->GetAttributes();

                m_Info.clear();

                for (unsigned int i = 0; i < Attributes.Count(); i++)
                {
                    CXMLAttribute* pAttribute = Attributes.Get(i);
                    MapSet(m_Info, pAttribute->GetName(), pAttribute->GetValue());
                }

                CXMLAttribute* pVersion = Attributes.Find("major");
                if (pVersion)
                {
                    m_uiVersionMajor = atoi(pVersion->GetValue().c_str());
                }

                pVersion = Attributes.Find("minor");
                if (pVersion)
                {
                    m_uiVersionMinor = atoi(pVersion->GetValue().c_str());
                }

                pVersion = Attributes.Find("revision");
                if (pVersion)
                {
                    m_uiVersionRevision = atoi(pVersion->GetValue().c_str());
                }

                pVersion = Attributes.Find("state");
                if (pVersion)
                {
                    const char* szVersion = pVersion->GetValue().c_str();

                    if (!stricmp(szVersion, "alpha"))
                        m_uiVersionState = 0;
                    else if (!stricmp(szVersion, "beta"))
                        m_uiVersionState = 1;
                    else
                        m_uiVersionState = 2;
                }
            }

            // Read everything that's included. If one of these fail, delete the XML we created and return
            if (!ReadIncludedResources(pRoot) || !ReadIncludedMaps(pRoot) || !ReadIncludedFiles(pRoot) || !ReadIncludedScripts(pRoot) ||
                !ReadIncludedHTML(pRoot) || !ReadIncludedExports(pRoot) || !ReadIncludedConfigs(pRoot))
            {
                delete pMetaFile;
                g_pGame->GetHTTPD()->UnregisterEHS(m_strResourceName.c_str());
                return false;
            }
        }

        // Delete the XML we created to save memory
        delete pMetaFile;
    }
    else
    {
        SString strError;
        pMetaFile->GetLastError(strError);

        if (strError.empty())
            m_strFailureReason = SString("Couldn't parse meta file for resource '%s'\n", m_strResourceName.c_str());
        else
            m_strFailureReason = SString("Couldn't parse meta file for resource '%s' [%s]\n", m_strResourceName.c_str(), strError.c_str());

        CLogger::ErrorPrintf(m_strFailureReason.c_str());

        // Delete the XML file if we somehow got to load it halfway
        if (pMetaFile)
            delete pMetaFile;

        g_pGame->GetHTTPD()->UnregisterEHS(m_strResourceName.c_str());
        return false;
    }

    // Generate a CRC for this resource
    if (!GenerateChecksums())
        return false;

    m_eState = EResourceState::Loaded;
    m_bDoneUpgradeWarnings = false;
    return true;
}

void ReplaceSlashes(string& strPath)
{
    ReplaceOccurrencesInString(strPath, "\\", "/");
}

void ReplaceSlashes(char* szPath)
{
    const size_t iLen = strlen(szPath);

    for (size_t i = 0; i < iLen; i++)
    {
        if (szPath[i] == '\\')
            szPath[i] = '/';
    }
}

bool CResource::Unload()
{
    if (m_eState == EResourceState::Running)
        Stop(true);

    TidyUp();

    if (m_pNodeStorage)
    {
        delete m_pNodeStorage;
        m_pNodeStorage = nullptr;
    }

    if (m_pNodeSettings)
    {
        delete m_pNodeSettings;
        m_pNodeSettings = nullptr;
    }

    OnResourceStateChange("unloaded");

    m_strResourceZip = "";
    m_strResourceCachePath = "";
    m_strResourceDirectoryPath = "";
    m_eState = EResourceState::None;

    return true;
}

void CResource::Reload()
{
    Unload();
    Load();
}

CResource::~CResource()
{
    CIdArray::PushUniqueId(this, EIdClass::RESOURCE, m_uiScriptID);

    m_bDestroyed = true;

    Unload();

    // Overkill, but easiest way to stop crashes:
    // Go through all other resources and make sure we are not in m_IncludedResources, m_Dependents and m_TemporaryIncludes
    auto iter = m_pResourceManager->IterBegin();

    for (; iter != m_pResourceManager->IterEnd(); ++iter)
    {
        if (*iter != this)
            (*iter)->InvalidateIncludedResourceReference(this);
    }

    m_strResourceName = "";
}

void CResource::TidyUp()
{
    // Close the zipfile stuff
    if (m_zipfile)
        unzClose(m_zipfile);

    m_zipfile = nullptr;

    // Go through each resource file and delete it
    for (CResourceFile* pResourceFile : m_ResourceFiles)
        delete pResourceFile;

    m_ResourceFiles.clear();
    m_ResourceFilesCountPerDir.clear();

    // Go through each included resource item and delete it
    for (CIncludedResources* pIncludedResources : m_IncludedResources)
        delete pIncludedResources;

    m_IncludedResources.clear();

    // Go through each of the dependent resources (those that include this one) and remove the reference to this
    for (CResource* pDependent : m_Dependents)
        pDependent->InvalidateIncludedResourceReference(this);

    g_pGame->GetHTTPD()->UnregisterEHS(m_strResourceName.c_str());
}

bool CResource::GetInfoValue(const char* szKey, std::string& strValue) const
{
    auto iter = m_Info.find(std::string(szKey));

    if (iter == m_Info.end())
        return false;

    strValue = iter->second;
    return true;
}

void CResource::SetInfoValue(const char* szKey, const char* szValue, bool bSave)
{
    bool bFoundExisting = false;

    // Try to find an existing value with a matching key
    auto iter = m_Info.find(std::string(szKey));

    if (iter != m_Info.end())
    {
        bFoundExisting = true;

        if (!szValue)
            m_Info.erase(iter);
        else
            iter->second = szValue;
    }

    // If there was no match and we were going to delete the key, we are done
    if (!bFoundExisting && !szValue)
        return;

    // If we were going to set a new value, create a new key and add it to our list
    if (!bFoundExisting)
    {
        MapSet(m_Info, std::string(szKey), std::string(szValue));
    }

    if (!bSave)
        return;

    // Save to xml
    std::string strPath;

    if (GetFilePath("meta.xml", strPath))
    {
        // Load the meta file
        CXMLFile* pMetaFile = g_pServerInterface->GetXML()->CreateXML(strPath.c_str());

        if (pMetaFile)
        {
            // Parse it
            if (pMetaFile->Parse())
            {
                // Grab its rootnode
                CXMLNode* pRootNode = pMetaFile->GetRootNode();

                if (pRootNode)
                {
                    // Get info attributes
                    CXMLNode* pInfoNode = pRootNode->FindSubNode("info");

                    if (!pInfoNode)
                        pInfoNode = pRootNode->CreateSubNode("info");

                    if (!szValue)
                    {
                        // Delete existing
                        pInfoNode->GetAttributes().Delete(szKey);
                    }
                    else
                    {
                        // Update/add
                        CXMLAttribute* pAttr = pInfoNode->GetAttributes().Find(szKey);

                        if (pAttr)
                            pAttr->SetValue(szValue);
                        else
                            pInfoNode->GetAttributes().Create(szKey)->SetValue(szValue);
                    }

                    pMetaFile->Write();
                }
            }

            // Destroy it
            delete pMetaFile;
        }
    }
}

std::future<SString> CResource::GenerateChecksumForFile(CResourceFile* pResourceFile)
{
    return SharedUtil::async([pResourceFile, this] {
        SString strPath;

        if (!GetFilePath(pResourceFile->GetName(), strPath))
            return SString();

        std::vector<char> buffer;
        FileLoad(strPath, buffer);
        uint        uiFileSize = buffer.size();
        const char* pFileContents = uiFileSize ? buffer.data() : "";
        CChecksum   Checksum = CChecksum::GenerateChecksumFromBuffer(pFileContents, uiFileSize);
        pResourceFile->SetLastChecksum(Checksum);
        pResourceFile->SetLastFileSize(uiFileSize);

        // Check if file is blocked
        char szHashResult[33];
        CMD5Hasher::ConvertToHex(pResourceFile->GetLastChecksum().md5, szHashResult);
        SString strBlockReason = m_pResourceManager->GetBlockedFileReason(szHashResult);

        if (!strBlockReason.empty())
        {
            return SString("file '%s' is blocked (%s)", pResourceFile->GetName(), *strBlockReason);
        }

        // Copy file to http holding directory
        switch (pResourceFile->GetType())
        {
            case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT:
            case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG:
            case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE:
            {
                SString strCachedFilePath = pResourceFile->GetCachedPathFilename();

                if (!g_pRealNetServer->ValidateHttpCacheFileName(strCachedFilePath))
                {
                    FileDelete(strCachedFilePath);
                    return SString("ERROR: Resource '%s' client filename '%s' not allowed\n", GetName().c_str(), *ExtractFilename(strCachedFilePath));
                }

                CChecksum cachedChecksum = CChecksum::GenerateChecksumFromFileUnsafe(strCachedFilePath);

                if (pResourceFile->GetLastChecksum() != cachedChecksum)
                {
                    if (!FileSave(strCachedFilePath, pFileContents, uiFileSize))
                    {
                        return SString("Could not copy '%s' to '%s'\n", *strPath, *strCachedFilePath);
                    }

                    // If script is 'no client cache', make sure there is no trace of it in the output dir
                    if (pResourceFile->IsNoClientCache())
                        FileDelete(pResourceFile->GetCachedPathFilename(true));
                }

                break;
            }
            default:
                break;
        }

        return SString();
    });
}

bool CResource::GenerateChecksums()
{
    std::vector<std::future<SString>> checksumTasks;
    checksumTasks.reserve(m_ResourceFiles.size());

    for (CResourceFile* pResourceFile : m_ResourceFiles)
    {
        checksumTasks.push_back(GenerateChecksumForFile(pResourceFile));
    }

    bool bOk = true;

    for (auto& task : checksumTasks)
    {
        const auto& result = task.get();
        if (!result.empty())
        {
            m_strFailureReason = result;
            CLogger::LogPrintf(result);
            bOk = false;
        }
    }

    SString strPath;

    if (GetFilePath("meta.xml", strPath))
        m_metaChecksum = CChecksum::GenerateChecksumFromFileUnsafe(strPath);
    else
        m_metaChecksum = CChecksum();

    return bOk;
}

bool CResource::HasResourceChanged()
{
    std::string strPath;
    std::string_view strDirPath = m_strResourceDirectoryPath;

    if (IsResourceZip())
    {
        strDirPath = m_strResourceCachePath;

        // Zip file might have changed
        CChecksum checksum = CChecksum::GenerateChecksumFromFileUnsafe(m_strResourceZip);
        if (checksum != m_zipHash)
            return true;
    }

    for (CResourceFile* pResourceFile : m_ResourceFiles)
    {
        if (!GetFilePath(pResourceFile->GetName(), strPath))
            return true;

        CChecksum checksum = CChecksum::GenerateChecksumFromFileUnsafe(strPath);

        if (pResourceFile->GetLastChecksum() != checksum)
            return true;

        // Also check if file in http cache has been externally altered
        switch (pResourceFile->GetType())
        {
            case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT:
            case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG:
            case CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE:
            {
                string    strCachedFilePath = pResourceFile->GetCachedPathFilename();
                CChecksum cachedChecksum = CChecksum::GenerateChecksumFromFileUnsafe(strCachedFilePath);

                if (cachedChecksum != checksum)
                    return true;

                break;
            }
            default:
                break;
        }
    }

    for (const auto& [strGlob, uiFileCount] : m_ResourceFilesCountPerDir)
    {
        std::vector<std::filesystem::path> files = glob::rglob(strDirPath.data() + strGlob);

        if (files.size() != uiFileCount)
            return true;
    }

    if (GetFilePath("meta.xml", strPath))
    {
        CChecksum checksum = CChecksum::GenerateChecksumFromFileUnsafe(strPath);
        if (checksum != m_metaChecksum)
            return true;
    }

    return false;
}

void CResource::ApplyUpgradeModifications()
{
    CResourceChecker().ApplyUpgradeModifications(this, m_strResourceZip);
}

//
// Output deprecated function usage and version issues
//
void CResource::LogUpgradeWarnings()
{
    CResourceChecker().LogUpgradeWarnings(this, m_strResourceZip, m_strMinClientReqFromSource, m_strMinServerReqFromSource, m_strMinClientReason,
                                          m_strMinServerReason);
    SString strStatus;

    if (!GetCompatibilityStatus(strStatus))
    {
        SString strReason = SString("WARNING: %s will not start as %s\n", m_strResourceName.c_str(), *strStatus);
        CLogger::LogPrint(strReason);
    }
    else if (!strStatus.empty())
    {
        SString strReason = SString("WARNING: %s requires upgrade as %s\n", m_strResourceName.c_str(), *strStatus);
        CLogger::LogPrint(strReason);
        CLogger::LogPrintf("Use the 'upgrade' command to perform a basic upgrade of resources.\n");
    }
}

//
// Determine outcome of version rules
//
// Fatal error: Returns false with error message in strOutStatus
// Warning:     Returns true with warning message in strOutStatus
// Ok:          Returns true with no message in strOutStatus
//
bool CResource::GetCompatibilityStatus(SString& strOutStatus)
{
    // Check declared version strings are valid
    if (!IsValidVersionString(m_strMinServerRequirement) || !IsValidVersionString(m_strMinClientRequirement))
    {
        strOutStatus = "<min_mta_version> section in the meta.xml contains invalid version strings";
        return false;
    }

    // Check this server can run this resource
#if MTASA_VERSION_BUILD != 0
    CMtaVersion strServerVersion = CStaticFunctionDefinitions::GetVersionSortable();
    if (m_strMinServerRequirement > strServerVersion)
    {
        strOutStatus = SString("this server version is too low (%s required)", *m_strMinServerRequirement);
        return false;
    }

    // This should not happen
    if (m_strMinServerReqFromSource > strServerVersion)
    {
        strOutStatus = "server has come back from the future";
        return false;
    }
#endif

    // Check if calculated version is higher than declared version
    if (m_strMinClientReqFromSource > m_strMinClientFromMetaXml)
    {
        strOutStatus = "<min_mta_version> section in the meta.xml is incorrect or missing (expected at least ";
        strOutStatus += SString("client %s because of '%s')", *m_strMinClientReqFromSource, *m_strMinClientReason);
        m_strMinClientRequirement = m_strMinClientReqFromSource;            // Apply higher version requirement
    }
    else if (m_strMinServerReqFromSource > m_strMinServerFromMetaXml)
    {
        strOutStatus = "<min_mta_version> section in the meta.xml is incorrect or missing (expected at least ";
        strOutStatus += SString("server %s because of '%s')", *m_strMinServerReqFromSource, *m_strMinServerReason);
    }

    // See if any connected client are below min requirements
    {
        uint uiNumIncompatiblePlayers = 0;
        for (std::list<CPlayer*>::const_iterator iter = g_pGame->GetPlayerManager()->IterBegin(); iter != g_pGame->GetPlayerManager()->IterEnd(); iter++)
            if ((*iter)->IsJoined() && m_strMinClientRequirement > (*iter)->GetPlayerVersion() && !(*iter)->ShouldIgnoreMinClientVersionChecks())
                uiNumIncompatiblePlayers++;

        if (uiNumIncompatiblePlayers > 0)
        {
            strOutStatus = SString("%d connected player(s) below required client version %s", uiNumIncompatiblePlayers, *m_strMinClientRequirement);
            return false;
        }
    }

    return true;
}

bool CResource::Start(std::list<CResource*>* pDependents, bool bManualStart, const SResourceStartOptions& StartOptions)
{
    if (m_eState == EResourceState::Running)
        return true;

    if (m_eState != EResourceState::Loaded)
        return false;

    if (m_bDestroyed)
        return false;

    OnResourceStateChange("starting");

    m_eState = EResourceState::Starting;

    CLuaArguments PreStartArguments;
    PreStartArguments.PushResource(this);

    if (!g_pGame->GetMapManager()->GetRootElement()->CallEvent("onResourcePreStart", PreStartArguments))
    {
        // Start cancelled by another resource
        m_strFailureReason = "Start cancelled by script\n";
        m_eState = EResourceState::Loaded;
        return false;
    }

    // CheckForIssues
    if (!m_bDoneUpgradeWarnings)
    {
        m_bDoneUpgradeWarnings = true;
        CResourceChecker().LogUpgradeWarnings(this, m_strResourceZip, m_strMinClientReqFromSource, m_strMinServerReqFromSource, m_strMinClientReason,
                                              m_strMinServerReason);
    }

    // MTA version check
    SString strStatus;

    if (!GetCompatibilityStatus(strStatus))
    {
        m_strFailureReason = SString("Not starting resource %s as %s\n", m_strResourceName.c_str(), strStatus.c_str());
        CLogger::LogPrint(m_strFailureReason);
        m_eState = EResourceState::Loaded;
        return false;
    }
    else if (!strStatus.empty())
    {
        SString strReason = SString("WARNING: %s requires upgrade as %s\n", m_strResourceName.c_str(), *strStatus);
        CLogger::LogPrint(strReason);
        CLogger::LogPrintf("Use the 'upgrade' command to perform a basic upgrade of resources.\n");
    }

    // Check the included resources are linked
    if (!m_bLinked)
    {
        if (!LinkToIncludedResources())
        {
            m_eState = EResourceState::Loaded;
            return false;
        }
    }

    m_bIsPersistent = false;

    // Create an element group for us
    m_pDefaultElementGroup = new CElementGroup();

    // Grab the root element
    m_pRootElement = g_pGame->GetMapManager()->GetRootElement();

    // Create the temporary storage node
    m_pNodeStorage = g_pServerInterface->GetXML()->CreateDummyNode();

    // Create the Resource Element
    m_pResourceElement = new CDummy(g_pGame->GetGroups(), m_pRootElement);
    m_pResourceElement->SetTypeName("resource");

    // Contains elements created at runtime by scripts etc (i.e. not in maps)
    m_pResourceDynamicElementRoot = new CDummy(g_pGame->GetGroups(), m_pResourceElement);
    m_pResourceDynamicElementRoot->SetTypeName("map");
    m_pResourceDynamicElementRoot->SetName("dynamic");

    // Verify resource element id and dynamic element root id
    if (m_pResourceElement->GetID() == INVALID_ELEMENT_ID || m_pResourceDynamicElementRoot->GetID() == INVALID_ELEMENT_ID)
    {
        // Destroy the dynamic element root
        g_pGame->GetElementDeleter()->Delete(m_pResourceDynamicElementRoot);
        m_pResourceDynamicElementRoot = nullptr;

        // Destroy the resource element
        g_pGame->GetElementDeleter()->Delete(m_pResourceElement);
        m_pResourceElement = nullptr;

        // Remove the temporary XML storage node
        if (m_pNodeStorage)
        {
            delete m_pNodeStorage;
            m_pNodeStorage = nullptr;
        }

        m_pRootElement = nullptr;

        // Destroy the element group attached directly to this resource
        delete m_pDefaultElementGroup;
        m_pDefaultElementGroup = nullptr;

        m_eState = EResourceState::Loaded;
        m_strFailureReason = SString("Start up of resource %s cancelled by element id starvation", m_strResourceName.c_str());
        CLogger::LogPrintf("%s\n", m_strFailureReason.c_str());
        return false;
    }

    // Set the Resource Element name
    m_pResourceElement->SetName(m_strResourceName.c_str());

    // Create the virtual machine for this resource
    CreateVM(m_bOOPEnabledInMetaXml);

    // We're now active
    CLogger::LogPrintf(LOGLEVEL_LOW, "Starting %s\n", m_strResourceName.c_str());

    // Remember the time we started
    time(&m_timeStarted);

    if (m_bSyncMapElementDataDefined)
        m_pResourceManager->ApplySyncMapElementDataOption(this, m_bSyncMapElementData);

    // Start all our sub resourcefiles
    for (CResourceFile* pResourceFile : m_ResourceFiles)
    {
        bool bAbortStart = false;

        // Check if file is blocked
        char szHashResult[33];
        CMD5Hasher::ConvertToHex(pResourceFile->GetLastChecksum().md5, szHashResult);
        SString strBlockReason = m_pResourceManager->GetBlockedFileReason(szHashResult);

        if (!strBlockReason.empty())
        {
            m_strFailureReason = SString("File '%s' is blocked (%s)", pResourceFile->GetName(), strBlockReason.c_str());
            CLogger::LogPrintf("Failed to start resource '%s' - %s\n", GetName().c_str(), m_strFailureReason.c_str());
            bAbortStart = true;
        }

        // Start if applicable
        if ((pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_MAP && StartOptions.bMaps) ||
            (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CONFIG && StartOptions.bConfigs) ||
            (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_SCRIPT && StartOptions.bScripts) ||
            (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT && StartOptions.bClientScripts) ||
            (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML && StartOptions.bHTML))
        {
            // Start. Failed?
            if (!pResourceFile->Start())
            {
                // Log it
                m_strFailureReason = SString("Failed to start resource item %s which is required\n", pResourceFile->GetName());
                CLogger::LogPrintf("Failed to start resource item %s in %s\n", pResourceFile->GetName(), m_strResourceName.c_str());
                bAbortStart = true;
            }
        }

        // Handle abort
        if (bAbortStart)
        {
            // Stop all the resource items without any warnings
            for (CResourceFile* pResourceFile : m_ResourceFiles)
                pResourceFile->Stop();

            DestroyVM();

            // Remove the temporary XML storage node
            if (m_pNodeStorage)
            {
                delete m_pNodeStorage;
                m_pNodeStorage = nullptr;
            }

            // Destroy the element group attached directly to this resource
            if (m_pDefaultElementGroup)
                delete m_pDefaultElementGroup;

            m_pDefaultElementGroup = nullptr;

            // Make sure we remove the resource elements from the players that have joined
            CEntityRemovePacket removePacket;

            if (m_pResourceElement)
            {
                removePacket.Add(m_pResourceElement);
                g_pGame->GetElementDeleter()->Delete(m_pResourceElement);
                m_pResourceElement = nullptr;
            }

            if (m_pResourceDynamicElementRoot)
            {
                removePacket.Add(m_pResourceDynamicElementRoot);
                g_pGame->GetElementDeleter()->Delete(m_pResourceDynamicElementRoot);
                m_pResourceDynamicElementRoot = nullptr;
            }

            g_pGame->GetPlayerManager()->BroadcastOnlyJoined(removePacket);
            m_eState = EResourceState::Loaded;
            return false;
        }
    }

    if (StartOptions.bIncludedResources)
    {
        // Copy the list over included resources because reloading them might change the list
        std::list<CIncludedResources*> includedResources = m_IncludedResources;

        // Start our included resources that haven't been started
        for (CIncludedResources* pIncludedResources : includedResources)
        {
            // Has it already been loaded?
            CResource* pIncluded = pIncludedResources->GetResource();

            if (pIncluded)
            {
                // Included resource has changed?
                if (pIncluded->HasResourceChanged())
                {
                    // Reload it if it's not already started
                    if (!pIncluded->IsActive())
                    {
                        m_pResourceManager->Reload(pIncluded);
                    }
                    else
                    {
                        CLogger::LogPrintf("WARNING: Included resource %s has changed but unable to reload due to resource already being in use\n",
                                           pIncluded->GetName().c_str());
                    }
                }

                // Make us dependant of it
                pIncluded->AddDependent(this);
            }
        }
    }

    // Add the resources depending on us
    if (pDependents)
    {
        for (CResource* pDependent : *pDependents)
            AddDependent(pDependent);
    }

    OnResourceStateChange("running");

    m_eState = EResourceState::Running;

    // Call the onResourceStart event. If it returns false, cancel this script again
    CLuaArguments Arguments;
    Arguments.PushResource(this);

    if (!m_pResourceElement->CallEvent("onResourceStart", Arguments))
    {
        // We're no longer active. stop the resource
        CLogger::LogPrintf("Start up of resource %s cancelled by script\n", m_strResourceName.c_str());
        m_strFailureReason = "Start up of resource cancelled by script\n";
        Stop(true);
        return false;
    }

    m_bStartedManually = bManualStart;

    // Remember the client files state
    m_bClientConfigs = StartOptions.bClientConfigs;
    m_bClientScripts = StartOptions.bClientScripts;
    m_bClientFiles = StartOptions.bClientFiles;

    m_pResourceManager->ApplyMinClientRequirement(this, m_strMinClientRequirement);

    // Broadcast new resourceelement that is loaded and tell the players that a new resource was started
    g_pGame->GetMapManager()->BroadcastResourceElements(m_pResourceElement, m_pDefaultElementGroup);
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CResourceStartPacket(m_strResourceName.c_str(), this));
    SendNoClientCacheScripts();
    m_bClientSync = true;

    // HACK?: stops resources getting loaded twice when you change them then manually restart
    GenerateChecksums();

    // Add us to the running resources list
    m_StartedResources.push_back(this);

    // Sort by priority, for start grouping on the client
    m_StartedResources.sort([](CResource* a, CResource* b) { return a->m_iDownloadPriorityGroup > b->m_iDownloadPriorityGroup; });

    return true;
}

void CResource::OnResourceStateChange(const char* state) noexcept
{
    if (!m_pResourceElement)
        return;

    CLuaArguments stateArgs;
    stateArgs.PushResource(this);
    switch (m_eState)
    {
        case EResourceState::Loaded: // When resource is stopped
            stateArgs.PushString("loaded");
            break;
        case EResourceState::Running: // When resource is running
            stateArgs.PushString("running");
            break;
        case EResourceState::Starting: // When resource is starting
            stateArgs.PushString("starting");
            break;
        case EResourceState::Stopping: // When resource is stopping
            stateArgs.PushString("stopping");
            break;
        case EResourceState::None: // When resource is not loaded
        default:
            stateArgs.PushString("unloaded");
            break;
    }
    stateArgs.PushString(state);

    m_pResourceElement->CallEvent("onResourceStateChange", stateArgs);
}

bool CResource::Stop(bool bManualStop)
{
    if (m_eState == EResourceState::Loaded)
        return true;

    if (m_eState != EResourceState::Running)
        return false;

    if (m_bStartedManually && !bManualStop)
        return false;

    OnResourceStateChange("stopping");

    m_eState = EResourceState::Stopping;
    m_pResourceManager->RemoveMinClientRequirement(this);
    m_pResourceManager->RemoveSyncMapElementDataOption(this);

    // Tell the log that we've stopped this resource
    CLogger::LogPrintf(LOGLEVEL_LOW, "Stopping %s\n", m_strResourceName.c_str());

    // Tell the modules we are stopping
    g_pGame->GetLuaManager()->GetLuaModuleManager()->ResourceStopping(m_pVM->GetVirtualMachine());

    // Remove us from the running resources list
    m_StartedResources.remove(this);

    // Tell all the players that have joined that this resource is stopped
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CResourceStopPacket(m_usNetID));
    m_bClientSync = false;

    // Call the onResourceStop event on this resource element
    CLuaArguments Arguments;
    Arguments.PushResource(this);
    Arguments.PushBoolean(m_bDestroyed);
    m_pResourceElement->CallEvent("onResourceStop", Arguments);

    // Remove us from the resources we depend on (they might unload too first)
    for (CIncludedResources* pIncludedResources : m_IncludedResources)
    {
        CResource* pResource = pIncludedResources->GetResource();

        if (pResource)
            pResource->RemoveDependent(this);
    }

    // Temorary includes??
    for (CResource* pResource : m_TemporaryIncludes)
        pResource->RemoveDependent(this);

    m_TemporaryIncludes.clear();

    // Stop all the resource files we have. The files we share with our clients we remove from the resource file list.
    for (CResourceFile* pResourceFile : m_ResourceFiles)
    {
        if (!pResourceFile->Stop())
            CLogger::LogPrintf("Failed to stop resource item %s in %s\n", pResourceFile->GetName(), m_strResourceName.c_str());
    }

    // Tell the module manager we have stopped
    g_pGame->GetLuaManager()->GetLuaModuleManager()->ResourceStopped(m_pVM->GetVirtualMachine());

    // Remove the temporary XML storage node
    if (m_pNodeStorage)
    {
        delete m_pNodeStorage;
        m_pNodeStorage = nullptr;
    }

    // Destroy the element group attached directly to this resource
    if (m_pDefaultElementGroup)
    {
        delete m_pDefaultElementGroup;
        m_pDefaultElementGroup = nullptr;
    }

    // Destroy the virtual machine for this resource
    DestroyVM();

    // Remove the resource element from the client
    CEntityRemovePacket removePacket;

    if (m_pResourceElement)
    {
        removePacket.Add(m_pResourceElement);
        g_pGame->GetElementDeleter()->Delete(m_pResourceElement);
        m_pResourceElement = nullptr;
    }

    // Remove the dynamic resource element from the client
    if (m_pResourceDynamicElementRoot)
    {
        removePacket.Add(m_pResourceDynamicElementRoot);
        g_pGame->GetElementDeleter()->Delete(m_pResourceDynamicElementRoot);
        m_pResourceDynamicElementRoot = nullptr;
    }

    // Broadcast the packet to joined players
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(removePacket);

    OnResourceStateChange("loaded");
    m_eState = EResourceState::Loaded;
    return true;
}

bool CResource::CreateVM(bool bEnableOOP)
{
    if (!m_pVM)
    {
        m_pVM = g_pGame->GetLuaManager()->CreateVirtualMachine(this, bEnableOOP);
        m_pResourceManager->NotifyResourceVMOpen(this, m_pVM);
    }

    if (!m_pVM)
        return false;

    m_pVM->SetScriptName(m_strResourceName.c_str());
    m_pVM->LoadEmbeddedScripts();
    m_pVM->RegisterModuleFunctions();
    return true;
}

bool CResource::DestroyVM()
{
    // Remove all player keybinds on this VM
    list<CPlayer*>::const_iterator iter = g_pGame->GetPlayerManager()->IterBegin();

    for (; iter != g_pGame->GetPlayerManager()->IterEnd(); iter++)
    {
        CKeyBinds* pBinds = (*iter)->GetKeyBinds();

        if (pBinds)
            pBinds->RemoveAllKeys(m_pVM);
    }

    // Delete the events on this VM
    m_pRootElement->DeleteEvents(m_pVM, true);
    g_pGame->GetElementDeleter()->CleanUpForVM(m_pVM);

    // Delete the virtual machine
    m_pResourceManager->NotifyResourceVMClose(this, m_pVM);
    g_pGame->GetLuaManager()->RemoveVirtualMachine(m_pVM);
    m_pVM = nullptr;
    return true;
}

void CResource::DisplayInfo()            // duplicated for HTML
{
    CLogger::LogPrintf("== Details for resource '%s' ==\n", m_strResourceName.c_str());

    switch (m_eState)
    {
        case EResourceState::Loaded:
        {
            CLogger::LogPrintf("Status: Stopped\n");
            break;
        }
        case EResourceState::Starting:
        {
            CLogger::LogPrintf("Status: Starting\n");
            break;
        }
        case EResourceState::Running:
        {
            CLogger::LogPrintf("Status: Running    Dependents: %d\n", m_Dependents.size());

            for (CResource* pDependent : m_Dependents)
                CLogger::LogPrintf("  %s\n", pDependent->GetName().c_str());

            break;
        }
        case EResourceState::Stopping:
        {
            CLogger::LogPrintf("Status: Stopping\n");
            break;
        }
        case EResourceState::None:
        default:
        {
            CLogger::LogPrintf("Status: Failed to load\n");
            break;
        }
    }

    if (!m_strCircularInclude.empty())
        CLogger::LogPrintf("Status: Circular include error: %s\n", m_strCircularInclude.c_str());

    CLogger::LogPrintf("Included resources: %d\n", m_IncludedResources.size());

    for (CIncludedResources* pIncludedResources : m_IncludedResources)
    {
        if (pIncludedResources->DoesExist())
        {
            if (pIncludedResources->GetResource()->IsLoaded())
                CLogger::LogPrintf("  %s .. OK\n", pIncludedResources->GetName().c_str());
            else
                CLogger::LogPrintf("  %s .. FAILED TO LOAD\n", pIncludedResources->GetName().c_str());
        }
        else
        {
            if (pIncludedResources->IsBadVersion())
            {
                CLogger::LogPrintf("  %s .. BAD VERSION (not between %d and %d)\n", pIncludedResources->GetMinimumVersion(),
                                   pIncludedResources->GetMaximumVersion());
            }
            else
            {
                CLogger::LogPrintf("  %s .. NOT FOUND\n", pIncludedResources->GetName().c_str());
            }
        }
    }

    CLogger::LogPrintf("Files: %d\n", m_ResourceFiles.size());
    CLogger::LogPrintf("== End ==\n");
}

bool CResource::ExtractFile(const char* szFilename)
{
    if (DoesFileExistInZip(szFilename))
    {
        // Load the zip file if it isn't already loaded. Return false if it can't be loaded.
        if (!m_zipfile)
            m_zipfile = unzOpenUtf8(m_strResourceZip.c_str());

        if (!m_zipfile)
            return false;

        unzLocateFile(m_zipfile, szFilename, false);

        // if the file exists, we'll be at the right place to extract from now
        int opt_extract_without_path = 0;
        int opt_overwrite = 1;

        int ires = do_extract_currentfile(m_zipfile, &opt_extract_without_path, &opt_overwrite, nullptr, m_strResourceCachePath.c_str());

        if (ires == UNZ_OK)
            return true;
    }
    return false;
}

bool CResource::DoesFileExistInZip(const char* szFilename)
{
    if (!m_zipfile)
        m_zipfile = unzOpenUtf8(m_strResourceZip.c_str());

    bool bRes = false;

    if (m_zipfile)
    {
        bRes = unzLocateFile(m_zipfile, szFilename, false) != UNZ_END_OF_LIST_OF_FILE;
        unzClose(m_zipfile);
        m_zipfile = nullptr;
    }

    return bRes;
}

// Return true if is looks like the resource files have been removed
bool CResource::HasGoneAway()
{
    if (!IsResourceZip())
    {
        return !FileExists(PathJoin(m_strResourceDirectoryPath, "meta.xml"));
    }
    else
    {
        return !FileExists(m_strResourceZip);
    }
}

// gets the path of the file specified
bool CResource::GetFilePath(const char* szFilename, string& strPath)
{
    // Always prefer the local resource directory, as scripts may
    // have added new files to the regular folder, rather than the zip
    strPath = m_strResourceDirectoryPath + szFilename;
    if (FileExists(strPath))
        return true;

    // If this is a zipped resource, try to use the unzipped file
    if (!IsResourceZip())
        return false;

    strPath = m_strResourceCachePath + szFilename;
    return FileExists(strPath);
}

std::vector<std::string> CResource::GetFilePaths(const char* szFilename)
{
    std::vector<std::string>    vecFiles;
    const std::string&          strDirectory = IsResourceZip() ? m_strResourceCachePath : m_strResourceDirectoryPath;
    const std::string           strFilePath = strDirectory + szFilename;

    for (const std::filesystem::path& path : glob::rglob(strFilePath))
    {
        std::string strPath = std::filesystem::relative(path, strDirectory).string();
        ReplaceSlashes(strPath);

        if (strPath == "meta.xml")
            continue;

        vecFiles.push_back(std::move(strPath));
    }

    return vecFiles;
}

// Return true if file name is used by this resource
bool CResource::IsFilenameUsed(const SString& strFilename, bool bClient)
{
    for (CResourceFile* pResourceFile : m_ResourceFiles)
    {
        if (strFilename.CompareI(pResourceFile->GetName()))
        {
            bool bIsClientFile = (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT ||
                                  pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG ||
                                  pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE);

            if (bIsClientFile == bClient)
            {
                return true;
            }
        }
    }

    return false;
}

bool CResource::ReadIncludedHTML(CXMLNode* pRoot)
{
    int                i = 0;
    bool               bFoundDefault = false;
    CResourceHTMLItem* pFirstHTML = nullptr;

    // Go trough each html subnode of the root
    for (CXMLNode* pHTML = pRoot->FindSubNode("html", i); pHTML != nullptr; pHTML = pRoot->FindSubNode("html", ++i))
    {
        // Get the attributelist
        CXMLAttributes& Attributes = pHTML->GetAttributes();

        // See if this is the default page (default attribute)
        bool           bIsDefault = false;
        CXMLAttribute* pDefaultfile = Attributes.Find("default");

        if (pDefaultfile)
        {
            const char* szDefault = pDefaultfile->GetValue().c_str();

            if (!stricmp(szDefault, "yes") || !stricmp(szDefault, "true"))
                bIsDefault = true;
            else
                bIsDefault = false;
        }

        // See if this is a raw file (like an image)
        bool           bIsRaw = false;
        CXMLAttribute* pRaw = Attributes.Find("raw");

        if (pRaw)
        {
            const char* szRaw = pRaw->GetValue().c_str();

            if (!stricmp(szRaw, "yes") || !stricmp(szRaw, "true"))
                bIsRaw = true;
            else
                bIsRaw = false;
        }

        // See if this is a restricted file
        bool           bIsRestricted = false;
        CXMLAttribute* pRestricted = Attributes.Find("restricted");

        if (pRestricted)
        {
            const char* szRestricted = pRestricted->GetValue().c_str();

            if (!stricmp(szRestricted, "yes") || !stricmp(szRestricted, "true"))
                bIsRestricted = true;
            else
                bIsRestricted = false;
        }

        // Find the source attribute (the name of the file)
        CXMLAttribute* pSrc = Attributes.Find("src");

        if (pSrc)
        {
            // If we found it grab the value
            std::string strFilename = pSrc->GetValue();

            if (!strFilename.empty())
            {
                ReplaceSlashes(strFilename);

                if (!IsValidFilePath(strFilename.c_str()))
                {
                    m_strFailureReason = SString("Couldn't find html %s for resource %s\n", strFilename.c_str(), m_strResourceName.c_str());
                    CLogger::ErrorPrintf(m_strFailureReason);
                    return false;
                }

                std::vector<std::string> vecFiles = GetFilePaths(strFilename.c_str());
                if (vecFiles.empty())
                {
                    if (glob::has_magic(strFilename))
                    {
                        m_ResourceFilesCountPerDir[strFilename] = vecFiles.size();
                        continue;
                    }

                    m_strFailureReason = SString("Couldn't find html %s for resource %s\n", strFilename.c_str(), m_strResourceName.c_str());
                    CLogger::ErrorPrintf(m_strFailureReason);
                    return false;
                }

                for (const std::string& strFilePath : vecFiles)
                {
                    std::string strFullFilename;

                    if (GetFilePath(strFilePath.c_str(), strFullFilename))
                    {
                        // This one is supposed to be default, but there's already a default page
                        if (bFoundDefault && bIsDefault)
                        {
                            CLogger::LogPrintf("Only one html item can be default per resource, ignoring %s in %s\n", strFilename.c_str(), m_strResourceName.c_str());
                            bIsDefault = false;
                        }

                        // If this is supposed to be default, we've now found our default page
                        if (bIsDefault)
                            bFoundDefault = true;

                        // Create a new resource HTML file and add it to the list
                        auto pResourceFile = new CResourceHTMLItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes, bIsDefault, bIsRaw, bIsRestricted, m_bOOPEnabledInMetaXml);
                        m_ResourceFiles.push_back(pResourceFile);

                        // This is the first HTML file? Remember it
                        if (!pFirstHTML)
                            pFirstHTML = pResourceFile;
                    }
                }
            }
            else
            {
                CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'html' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
            }
        }
        else
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'html' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
        }
    }

    // If we haven't found a default html page, we put the first HTML as the default
    if (pFirstHTML && !bFoundDefault)
        pFirstHTML->SetDefaultPage(true);

    return true;
}

bool CResource::ReadIncludedConfigs(CXMLNode* pRoot)
{
    int i = 0;

    // Loop through the list of included configs
    for (CXMLNode* pConfig = pRoot->FindSubNode("config", i); pConfig != nullptr; pConfig = pRoot->FindSubNode("config", ++i))
    {
        CXMLAttributes& Attributes = pConfig->GetAttributes();

        bool           bServer = true;
        bool           bClient = false;
        CXMLAttribute* pType = Attributes.Find("type");

        if (pType)
        {
            // Grab the type. Client or server or shared
            const char* szType = pType->GetValue().c_str();

            if (!stricmp(szType, "client"))
            {
                bServer = false;
                bClient = true;
            }
            else if (!stricmp(szType, "shared"))
                bClient = true;
            else if (stricmp(szType, "server") != 0)
                CLogger::LogPrintf("Unknown config type specified in %s. Assuming 'server'\n", m_strResourceName.c_str());
        }

        // Find the source (file path)
        CXMLAttribute* pSrc = Attributes.Find("src");

        if (pSrc)
        {
            // Grab the filename
            std::string strFilename = pSrc->GetValue();

            if (!strFilename.empty())
            {
                std::string strFullFilename;
                ReplaceSlashes(strFilename);

                if (bClient && IsFilenameUsed(strFilename, true))
                {
                    CLogger::LogPrintf("WARNING: Ignoring duplicate client config file in resource '%s': '%s'\n", m_strResourceName.c_str(),
                                       strFilename.c_str());
                    bClient = false;
                }
                if (bServer && IsFilenameUsed(strFilename, false))
                {
                    CLogger::LogPrintf("WARNING: Duplicate config file in resource '%s': '%s'\n", m_strResourceName.c_str(), strFilename.c_str());
                }

                // Extract / grab the filepath
                if (IsValidFilePath(strFilename.c_str()) && GetFilePath(strFilename.c_str(), strFullFilename))
                {
                    // Create it and push it to the list over resource files. Depending on if it's client or server type
                    if (bServer)
                        m_ResourceFiles.push_back(new CResourceConfigItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes));

                    if (bClient)
                        m_ResourceFiles.push_back(new CResourceClientConfigItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes));
                }
                else
                {
                    m_strFailureReason = SString("Couldn't find config %s for resource %s\n", strFilename.c_str(), m_strResourceName.c_str());
                    CLogger::ErrorPrintf(m_strFailureReason);
                    return false;
                }
            }
            else
            {
                CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'config' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
            }
        }
        else
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'config' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
        }
    }

    return true;
}

bool CResource::ReadIncludedFiles(CXMLNode* pRoot)
{
    int i = 0;

    // Loop through the included files
    for (CXMLNode* pFile = pRoot->FindSubNode("file", i); pFile != nullptr; pFile = pRoot->FindSubNode("file", ++i))
    {
        CXMLAttributes& Attributes = pFile->GetAttributes();

        // Grab the filepath attribute
        CXMLAttribute* pSrc = Attributes.Find("src");

        if (pSrc)
        {
            // Grab the value
            std::string strFilename = pSrc->GetValue();

            if (!strFilename.empty())
            {
                ReplaceSlashes(strFilename);

                if (!IsValidFilePath(strFilename.c_str()))
                {
                    m_strFailureReason = SString("Couldn't find file(s) %s for resource %s\n", strFilename.c_str(), m_strResourceName.c_str());
                    CLogger::ErrorPrintf(m_strFailureReason);
                    return false;
                }

                std::vector<std::string> vecFiles = GetFilePaths(strFilename.c_str());

                if (vecFiles.empty())
                {
                    if (glob::has_magic(strFilename))
                    {
                        m_ResourceFilesCountPerDir[strFilename] = vecFiles.size();
                        continue;
                    }

                    m_strFailureReason = SString("Couldn't find file(s) %s for resource %s\n", strFilename.c_str(), m_strResourceName.c_str());
                    CLogger::ErrorPrintf(m_strFailureReason);
                    return false;
                }

                bool           bDownload = true;
                CXMLAttribute* pDownload = Attributes.Find("download");

                if (pDownload)
                {
                    const std::string strDownload = pDownload->GetValue();

                    if (strDownload == "no" || strDownload == "false")
                        bDownload = false;
                }

                for (const std::string& strFilePath : vecFiles)
                {
                    std::string strFullFilename;

                    if (IsFilenameUsed(strFilePath, true))
                    {
                        CLogger::LogPrintf("WARNING: Ignoring duplicate client file in resource '%s': '%s'\n", m_strResourceName.c_str(), strFilePath.c_str());
                        continue;
                    }

                    if (GetFilePath(strFilePath.c_str(), strFullFilename))
                    {
                        m_ResourceFiles.push_back(new CResourceClientFileItem(this, strFilePath.c_str(), strFullFilename.c_str(), &Attributes, bDownload));
                    }
                }

                if (glob::has_magic(strFilename))
                    m_ResourceFilesCountPerDir[strFilename] = vecFiles.size();
            }
            else
            {
                CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'file' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
            }
        }
        else
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'file' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
        }
    }

    return true;
}

bool CResource::ReadIncludedExports(CXMLNode* pRoot)
{
    int i = 0;
    m_ExportedFunctions.clear();
    m_httpRouterFunction.clear();
    m_httpRouterAclRight.clear();
    m_httpRouterCheck = HTTP_ROUTER_CHECK_PENDING;

    // Read our exportlist
    for (CXMLNode* pExport = pRoot->FindSubNode("export", i); pExport != nullptr; pExport = pRoot->FindSubNode("export", ++i))
    {
        CXMLAttributes& Attributes = pExport->GetAttributes();

        CXMLAttribute* functionAttribute = Attributes.Find("function");

        if (functionAttribute == nullptr)
        {
            CLogger::LogPrintf("WARNING: Missing 'function' attribute from 'export' node of 'meta.xml' for resource '%s', ignoring\n",
                               m_strResourceName.c_str());
            continue;
        }

        const char* functionName = functionAttribute->GetValue().c_str();

        if (functionName[0] == '\0')
        {
            CLogger::ErrorPrintf("WARNING: Empty 'function' attribute of 'export' node of 'meta.xml' for resource '%s', ignoring\n",
                                 m_strResourceName.c_str());
            continue;
        }

        // See if the http attribute is true or false
        bool isHttpFunction = false;

        if (CXMLAttribute* attribute = Attributes.Find("http"); attribute != nullptr)
        {
            const char* value = attribute->GetValue().c_str();
            isHttpFunction = !stricmp(value, "true") || !stricmp(value, "yes");
        }

        // See if the http router attribute is true or false
        bool isHttpRouter = false;

        if (CXMLAttribute* attribute = Attributes.Find("router"); attribute != nullptr)
        {
            const char* value = attribute->GetValue().c_str();
            isHttpRouter = !stricmp(value, "true") || !stricmp(value, "yes");
        }

        if (!isHttpFunction && isHttpRouter)
        {
            isHttpRouter = false;
            CLogger::ErrorPrintf("WARNING: Regular function '%s' in resource '%s' uses HTTP router attribute\n",
                                 functionName, m_strResourceName.c_str());
        }

        if (isHttpRouter && !m_httpRouterFunction.empty())
        {
            isHttpRouter = false;
            CLogger::ErrorPrintf("WARNING: HTTP router function '%s' in resource '%s' ignored, using '%s'\n",
                                 functionName, m_strResourceName.c_str(), m_httpRouterFunction.c_str());
        }

        // See if the restricted attribute is true or false
        bool isRestricted = false;

        if (CXMLAttribute* attribute = Attributes.Find("restricted"); attribute != nullptr)
        {
            const char* value = attribute->GetValue().c_str();
            isRestricted = !stricmp(value, "true") || !stricmp(value, "yes");
        }

        // Find the type attribute
        bool isServerFunction = true;
        bool isClientFunction = false;

        if (CXMLAttribute* attribute = Attributes.Find("type"); attribute != nullptr)
        {
            const char* value = attribute->GetValue().c_str();

            if (!stricmp(value, "client"))
            {
                isServerFunction = false;
                isClientFunction = true;
            }
            else if (!stricmp(value, "shared"))
            {
                isClientFunction = true;
            }
            else if (stricmp(value, "server") != 0)
            {
                CLogger::LogPrintf("WARNING: Function '%s' in resource '%s' uses unknown function type, assuming 'server'\n",
                                   functionName, m_strResourceName.c_str());
            }
        }

        if (isHttpRouter)
        {
            if (isServerFunction)
            {
                m_httpRouterFunction = functionName;
                m_httpRouterAclRight = SString("%s.function.%s", m_strResourceName.c_str(), functionName);
                continue;
            }

            CLogger::LogPrintf("WARNING: HTTP router function '%s' in resource '%s' is not a server-sided function, ignoring\n",
                               functionName, m_strResourceName.c_str());
            continue;
        }

        if (isServerFunction)
        {
            m_ExportedFunctions.push_back(CExportedFunction(functionName, isHttpFunction, CExportedFunction::EXPORTED_FUNCTION_TYPE_SERVER,
                                                            isRestricted || GetName() == "webadmin" || GetName() == "runcode"));
        }

        if (isClientFunction)
        {
            m_ExportedFunctions.push_back(CExportedFunction(functionName, isHttpFunction, CExportedFunction::EXPORTED_FUNCTION_TYPE_CLIENT,
                                                            isRestricted || GetName() == "webadmin" || GetName() == "runcode"));
        }
    }

    return true;
}

bool CResource::ReadIncludedScripts(CXMLNode* pRoot)
{
    int i = 0;

    // Loop through all script nodes under the root
    for (CXMLNode* pScript = pRoot->FindSubNode("script", i); pScript != nullptr; pScript = pRoot->FindSubNode("script", ++i))
    {
        CXMLAttributes& Attributes = pScript->GetAttributes();

        // Grab the type attribute (server / client)
        bool           bServer = true;
        bool           bClient = false;
        CXMLAttribute* pType = Attributes.Find("type");

        if (pType)
        {
            const char* szType = pType->GetValue().c_str();

            // Check its content (server or client or shared) and set the type accordingly
            if (!stricmp(szType, "client"))
            {
                bServer = false;
                bClient = true;
            }
            else if (!stricmp(szType, "shared"))
                bClient = true;
            else if (stricmp(szType, "server") != 0)
                CLogger::LogPrintf("Unknown script type specified in %s. Assuming 'server'\n", m_strResourceName.c_str());
        }

        // Grab the source attribute
        CXMLAttribute* pSrc = Attributes.Find("src");

        if (pSrc)
        {
            // Grab the source value from the attribute
            std::string strFilename = pSrc->GetValue();

            if (!strFilename.empty())
            {
                ReplaceSlashes(strFilename);

                if (!IsValidFilePath(strFilename.c_str()))
                {
                    m_strFailureReason = SString("Couldn't find script(s) %s for resource %s\n", strFilename.c_str(), m_strResourceName.c_str());
                    CLogger::ErrorPrintf(m_strFailureReason);
                    return false;
                }

                std::vector<std::string> vecFiles = GetFilePaths(strFilename.c_str());

                if (vecFiles.empty())
                {
                    if (glob::has_magic(strFilename))
                    {
                        m_ResourceFilesCountPerDir[strFilename] = vecFiles.size();
                        continue;
                    }

                    m_strFailureReason = SString("Couldn't find script(s) %s for resource %s\n", strFilename.c_str(), m_strResourceName.c_str());
                    CLogger::ErrorPrintf(m_strFailureReason);
                    return false;
                }

                for (const std::string& strFilePath : vecFiles)
                {
                    std::string strFullFilename;
                    bool        bLoadClient = bClient;
                    bool        bLoadServer = bServer;

                    if (GetFilePath(strFilePath.c_str(), strFullFilename))
                    {
                        // Check if the filename is already used by this resource

                        if (bClient && IsFilenameUsed(strFilePath, true))
                        {
                            CLogger::LogPrintf("WARNING: Ignoring duplicate client script file in resource '%s': '%s'\n", m_strResourceName.c_str(),
                                               strFilePath.c_str());
                            bLoadClient = false;
                        }

                        if (bServer && IsFilenameUsed(strFilePath, false))
                        {
                            CLogger::LogPrintf("WARNING: Ignoring duplicate script file in resource '%s': '%s'\n", m_strResourceName.c_str(),
                                               strFilePath.c_str());

                            bLoadServer = false;
                        }

                        // Create it depending on the type (client or server or shared) and add it to the list of resource files

                        if (bLoadServer)
                            m_ResourceFiles.push_back(new CResourceScriptItem(this, strFilePath.c_str(), strFullFilename.c_str(), &Attributes));

                        if (bLoadClient)
                            m_ResourceFiles.push_back(new CResourceClientScriptItem(this, strFilePath.c_str(), strFullFilename.c_str(), &Attributes));
                    }
                }

                if (glob::has_magic(strFilename))
                    m_ResourceFilesCountPerDir[strFilename] = vecFiles.size();
            }
            else
            {
                CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'script' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
            }
        }
        else
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'script' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
        }
    }

    return true;
}

bool CResource::ReadIncludedMaps(CXMLNode* pRoot)
{
    int i = 0;

    // Loop through the map nodes under the root
    for (CXMLNode* pMap = pRoot->FindSubNode("map", i); pMap != nullptr; pMap = pRoot->FindSubNode("map", ++i))
    {
        CXMLAttributes& Attributes = pMap->GetAttributes();

        // Grab the dimension attribute
        int            iDimension = 0;
        CXMLAttribute* pDimension = Attributes.Find("dimension");

        if (pDimension)
        {
            iDimension = atoi(pDimension->GetValue().c_str());

            if (iDimension < 0 || iDimension > 65535)
                iDimension = 0;
        }

        // Grab the source node
        CXMLAttribute* pSrc = Attributes.Find("src");

        if (pSrc)
        {
            std::string strFilename = pSrc->GetValue();

            if (!strFilename.empty())
            {
                std::string strFullFilename;
                ReplaceSlashes(strFilename);

                if (IsFilenameUsed(strFilename, false))
                {
                    CLogger::LogPrintf("WARNING: Duplicate map file in resource '%s': '%s'\n", m_strResourceName.c_str(), strFilename.c_str());
                }

                // Grab the file (evt extract it). Make a map item resource and put it into the resourcefiles list
                if (IsValidFilePath(strFilename.c_str()) && GetFilePath(strFilename.c_str(), strFullFilename))
                {
                    m_ResourceFiles.push_back(new CResourceMapItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes, iDimension));
                }
                else
                {
                    m_strFailureReason = SString("Couldn't find map %s for resource %s\n", strFilename.c_str(), m_strResourceName.c_str());
                    CLogger::ErrorPrintf(m_strFailureReason);
                    return false;
                }
            }
            else
            {
                CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'map' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
            }
        }
        else
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'map' node of 'meta.xml' for resource '%s', ignoring\n", m_strResourceName.c_str());
        }
    }

    return true;
}

bool CResource::GetDefaultSetting(const char* szName, char* szValue, size_t sizeBuffer)
{
    // Look through its subnodes for settings with a matching name
    unsigned int uiCount = m_pNodeSettings->GetSubNodeCount();
    unsigned int i = 0;
    std::string  strTagName;

    for (; i < uiCount; i++)
    {
        // Grab its tag name
        CXMLNode* pTemp = m_pNodeSettings->GetSubNode(i);
        strTagName = pTemp->GetTagName();

        // Check that its "setting"
        if (!stricmp(strTagName.c_str(), "setting"))
        {
            // Grab the name attribute and compare it to the name we look for
            CXMLAttribute* pName = m_pNodeSettings->GetAttributes().Find("name");

            if (pName && !strcmp(szName, pName->GetValue().c_str()))
            {
                strncpy(szValue, pName->GetValue().c_str(), std::min(sizeBuffer, pName->GetValue().size()));
                szValue[sizeBuffer - 1] = '\0';
                return true;
            }
        }
    }

    return false;
}

bool CResource::SetDefaultSetting(const char* szName, const char* szValue)
{
    return false;
}

bool CResource::RemoveDefaultSetting(const char* szName)
{
    return false;
}

bool CResource::AddMapFile(const char* szName, const char* szFullFilename, int iDimension)
{
    if (!IsLoaded() || m_bResourceIsZip)
        return false;

    // Find the meta file path
    char szMetaPath[MAX_PATH + 1];
    snprintf(szMetaPath, MAX_PATH, "%s%s", m_strResourceDirectoryPath.c_str(), "meta.xml");

    // Load the meta file
    CXMLFile* pMetaFile = g_pServerInterface->GetXML()->CreateXML(szMetaPath);

    if (!pMetaFile)
        return false;

    if (!pMetaFile->Parse())
    {
        delete pMetaFile;
        return false;
    }

    // Grab its rootnode
    CXMLNode* pRootNode = pMetaFile->GetRootNode();

    if (pRootNode)
    {
        // Create a new map subnode
        CXMLNode* pMapNode = pRootNode->CreateSubNode("map");

        if (pMapNode)
        {
            // Set the src and dimension attributes
            pMapNode->GetAttributes().Create("src")->SetValue(szName);
            pMapNode->GetAttributes().Create("dimension")->SetValue(iDimension);

            // If we're loaded, add it to the resourcefiles too
            m_ResourceFiles.push_back(new CResourceMapItem(this, szName, szFullFilename, &pMapNode->GetAttributes(), iDimension));

            // Success, write and destroy XML
            pMetaFile->Write();
            delete pMetaFile;
            return true;
        }
    }

    delete pMetaFile;
    return false;
}

bool CResource::AddConfigFile(const char* szName, const char* szFullFilepath, int iType)
{
    if (!IsLoaded() || m_bResourceIsZip)
        return false;

    // Find the meta file path
    char szMetaPath[MAX_PATH + 1];
    snprintf(szMetaPath, MAX_PATH, "%s%s", m_strResourceDirectoryPath.c_str(), "meta.xml");

    // Load the meta file
    CXMLFile* pMetaFile = g_pServerInterface->GetXML()->CreateXML(szMetaPath);

    if (!pMetaFile)
        return false;

    if (!pMetaFile->Parse())
    {
        delete pMetaFile;
        return false;
    }

    // Grab its rootnode
    CXMLNode* pRootNode = pMetaFile->GetRootNode();

    if (pRootNode)
    {
        // Create a new map subnode
        CXMLNode* pMapNode = pRootNode->CreateSubNode("config");

        if (pMapNode)
        {
            // Set the src attributes
            pMapNode->GetAttributes().Create("src")->SetValue(szName);

            // Also set the type attribute (server or client)
            if (iType == CResourceScriptItem::RESOURCE_FILE_TYPE_CLIENT_CONFIG)
                pMapNode->GetAttributes().Create("type")->SetValue("client");
            else if (iType == CResourceScriptItem::RESOURCE_FILE_TYPE_CONFIG)
                pMapNode->GetAttributes().Create("type")->SetValue("server");

            // If we're loaded, add it to the resourcefiles too
            m_ResourceFiles.push_back(new CResourceConfigItem(this, szName, szFullFilepath, &pMapNode->GetAttributes()));

            // Success, write and destroy XML
            pMetaFile->Write();
            delete pMetaFile;
            return true;
        }
    }

    delete pMetaFile;
    return false;
}

bool CResource::IncludedFileExists(const char* szName, int iType)
{
    for (CResourceFile* pResourceFile : m_ResourceFiles)
    {
        // Is it the required type?
        if (iType == CResourceFile::RESOURCE_FILE_TYPE_NONE || pResourceFile->GetType() == iType)
        {
            // Check if the name compares equal (case independant)
            if (!stricmp(pResourceFile->GetName(), szName))
                return true;
        }
    }

    return false;
}

bool CResource::RemoveFile(const char* szName)
{
    if (!IsLoaded() || m_bResourceIsZip)
        return false;

    // Find the meta file path
    char szMetaPath[MAX_PATH + 1];
    snprintf(szMetaPath, MAX_PATH, "%s%s", m_strResourceDirectoryPath.c_str(), "meta.xml");

    // Load the meta file
    CXMLFile* pMetaFile = g_pServerInterface->GetXML()->CreateXML(szMetaPath);

    if (!pMetaFile)
        return false;

    if (!pMetaFile->Parse())
    {
        delete pMetaFile;
        return false;
    }

    // Grab its rootnode
    CXMLNode* pRootNode = pMetaFile->GetRootNode();

    if (pRootNode)
    {
        int       i = 0;
        CXMLNode* pNodeFound = nullptr;

        // Loop through the map nodes under the root
        for (CXMLNode* pNode = pRootNode->GetSubNode(i); pNode != nullptr; pNode = pRootNode->GetSubNode(++i))
        {
            // Grab the tag name
            const std::string& strTempBuffer = pNode->GetTagName();

            if (!stricmp(strTempBuffer.c_str(), "map") || !stricmp(strTempBuffer.c_str(), "config") || !stricmp(strTempBuffer.c_str(), "script") ||
                !stricmp(strTempBuffer.c_str(), "html"))
            {
                // Grab the src attribute? Same name?
                CXMLAttribute* pAttrib = pNode->GetAttributes().Find("src");

                if (pAttrib && !stricmp(pAttrib->GetValue().c_str(), szName))
                {
                    pNodeFound = pNode;
                    break;
                }
            }
        }

        // Found our node?
        if (pNodeFound)
        {
            // Delete the node from the XML, write it and then clean up
            pRootNode->DeleteSubNode(pNodeFound);

            // Find the file in our filelist
            CResourceFile* pFileFound = nullptr;

            for (CResourceFile* pResourceFile : m_ResourceFiles)
            {
                // Found a matching file? Remember its type
                if (!stricmp(szName, pResourceFile->GetName()))
                {
                    pFileFound = pResourceFile;
                    break;
                }
            }

            // Found the resource file?
            if (pFileFound)
            {
                // Delete it from our list
                delete pFileFound;
                m_ResourceFiles.remove(pFileFound);
            }
            else
                CLogger::LogPrintf("WARNING: Problems removing resource file from memory");
        }

        // Delete the file
        char szFullFilepath[MAX_PATH + 1];
        snprintf(szFullFilepath, MAX_PATH, "%s%s", m_strResourceDirectoryPath.c_str(), szName);

        if (File::Delete(szFullFilepath) != 0)
            CLogger::LogPrintf("WARNING: Problems deleting the actual file, but was removed from resource");

        // Delete the metafile
        pMetaFile->Write();
        delete pMetaFile;

        return true;
    }

    return false;
}

// read the included resources from the XML file and store it for future reference
bool CResource::ReadIncludedResources(CXMLNode* pRoot)
{
    int i = 0;

    // Loop through the included resources list
    for (CXMLNode* pInclude = pRoot->FindSubNode("include", i); pInclude != nullptr; pInclude = pRoot->FindSubNode("include", ++i))
    {
        CXMLAttributes& Attributes = pInclude->GetAttributes();

        // Grab the minversion attribute (minimum version the included resource needs to be)
        SVersion       svMinVersion;
        CXMLAttribute* pMinVersion = Attributes.Find("minversion");

        if (pMinVersion)
        {
            const std::string& strMinVersion = pMinVersion->GetValue();

            if (!strMinVersion.empty())
            {
                std::stringstream ss;
                ss << strMinVersion;
                ss >> svMinVersion.m_uiMajor;
                ss >> svMinVersion.m_uiMinor;
                ss >> svMinVersion.m_uiRevision;
            }
        }

        // Grab the maxversion attribute (maximum version the included resource needs to be)
        SVersion       svMaxVersion;
        CXMLAttribute* pMaxVersion = Attributes.Find("maxversion");

        if (pMaxVersion)
        {
            const std::string& strMaxVersion = pMaxVersion->GetValue();

            if (!strMaxVersion.empty())
            {
                std::stringstream ss;
                ss << strMaxVersion;
                ss >> svMaxVersion.m_uiMajor;
                ss >> svMaxVersion.m_uiMinor;
                ss >> svMaxVersion.m_uiRevision;
            }
        }

        // Grab the resource attribute
        CXMLAttribute* pSrc = Attributes.Find("resource");

        if (pSrc)
        {
            // Grab the value and add an included resource
            const std::string& strIncludedResource = pSrc->GetValue();

            // If there's text in the node
            if (!strIncludedResource.empty())
                m_IncludedResources.push_back(new CIncludedResources(m_pResourceManager, strIncludedResource.c_str(), svMinVersion, svMaxVersion, this));
            else
                CLogger::LogPrintf("WARNING: Empty 'resource' attribute from 'include' node of 'meta.xml' for resource '%s', ignoring\n",
                                   m_strResourceName.c_str());
        }
        else
        {
            CLogger::LogPrintf("WARNING: Missing 'resource' attribute from 'include' node of 'meta.xml' for resource '%s', ignoring\n",
                               m_strResourceName.c_str());
        }
    }

    return true;
}

bool CResource::LinkToIncludedResources()
{
    m_bLinked = true;

    // Loop through the list of included resources
    for (CIncludedResources* pIncludedResources : m_IncludedResources)
    {
        // If we failed creating the link
        if (!pIncludedResources->CreateLink())
        {
            m_bLinked = false;

            if (m_strFailureReason.empty())
                m_strFailureReason = SString("Failed to link to %s", pIncludedResources->GetName().c_str());
#ifdef RESOURCE_DEBUG_MESSAGES
            CLogger::LogPrintf("  Links to %s .. FAILED\n", pIncludedResources->GetName().c_str());
#endif
        }
#ifdef RESOURCE_DEBUG_MESSAGES
        else
            CLogger::LogPrintf("  Links to %s .. OK\n", pIncludedResources->GetName().c_str());
#endif
    }

    return m_bLinked;
}

bool CResource::CheckIfStartable()
{
    // return straight away if we know we've already got a circular include, otherwise
    // it spams it every few seconds
    if (m_eState == EResourceState::None)
        return false;

    // Check that the included resources aren't circular
    m_strCircularInclude = "";
    std::vector<CResource*> vecCircular;

    if (IsIncludedResourceRecursive(&vecCircular))
    {
        char szOldString[512];
        char szTrail[512] = "";

        // Make a string telling what the circular path is
        for (CResource* pResource : vecCircular)
        {
            if (pResource)
            {
                strcpy(szOldString, szTrail);
                snprintf(szTrail, 510, "-> %s %s", pResource->GetName().c_str(), szOldString);
            }
        }

        // Remember why we failed and return false
        m_strCircularInclude = SString("%s %s", m_strResourceName.c_str(), szTrail);
        m_strFailureReason = SString("Circular include error: %s", m_strCircularInclude.c_str());
        return false;
    }

    // Check if all the included resources are startable
    for (CIncludedResources* pIncludedResources : m_IncludedResources)
    {
#ifdef RESOURCE_DEBUG_MESSAGES
        CLogger::LogPrintf("  Checking if %s is loaded\n", (*iterr)->GetName().c_str());
#endif
        CResource* pResource = pIncludedResources->GetResource();

        if (!pResource || !pResource->CheckIfStartable())
            return false;
    }

    return true;
}

bool CResource::IsIncludedResourceRecursive(std::vector<CResource*>* past)
{
    past->push_back(this);

    // CLogger::LogPrintf ( "%s\n", this->GetName().c_str () );

    // Loop through the included resources
    for (CIncludedResources* pIncludedResources : m_IncludedResources)
    {
        // If one item from the past is equal to one of our dependencies, then there's circular dependencies
        for (CResource* pResource : *past)
        {
            if (pResource == pIncludedResources->GetResource())
                return true;
        }

        // Check further down the tree. If this returns true, we pass that true backwards
        CResource* pResource = pIncludedResources->GetResource();

        if (pResource && pResource->IsIncludedResourceRecursive(past))
            return true;
    }

    // Remove us from the tree again because it wasn't circular this path
    past->pop_back();
    return false;
}

bool CResource::IsDependentResource(CResource* pResource)
{
    // Check if the given resource is a resource we're depending on
    for (CResource* pDependent : m_Dependents)
    {
        if (pDependent == pResource)
            return true;
    }

    return false;
}

bool CResource::IsDependentResourceRecursive(CResource* pResource)
{
    // Check if the given resource is a resource we're depending on. Also
    // check our dependencies dependencies and etc.. further down the tree
    for (CResource* pDependent : m_Dependents)
    {
        if (pDependent == pResource || pDependent->IsDependentResourceRecursive(pResource))
            return true;
    }

    return false;
}

bool CResource::IsDependentResource(const char* szResourceName)
{
    // Check if the given resource is a resource we're depending on
    for (CResource* pDependent : m_Dependents)
    {
        if (!strcmp(pDependent->GetName().c_str(), szResourceName))
            return true;
    }

    return false;
}

bool CResource::IsDependentResourceRecursive(const char* szResourceName)
{
    // Check if the given resource is a resource we're depending on. Also
    // check our dependencies dependencies and etc.. further down the tree
    for (CResource* pDependent : m_Dependents)
    {
        if (!strcmp(pDependent->GetName().c_str(), szResourceName) || pDependent->IsDependentResourceRecursive(szResourceName))
            return true;
    }

    return false;
}

void CResource::AddTemporaryInclude(CResource* pResource)
{
    if (!ListContains(m_TemporaryIncludes, pResource))
        m_TemporaryIncludes.push_back(pResource);
}

void CResource::AddDependent(CResource* pResource)
{
    if (!ListContains(m_Dependents, pResource))
    {
        m_Dependents.push_back(pResource);
        CheckState();
    }
}

void CResource::RemoveDependent(CResource* pResource)
{
    m_Dependents.remove(pResource);
    CheckState();
}

// Called on another thread, but g_pGame->Lock() has been called, so everything is going to be OK
HttpStatusCode CResource::HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse)
{
    CAccount* account = g_pGame->GetHTTPD()->CheckAuthentication(ipoHttpRequest);

    if (!account)
    {
        return HTTP_STATUS_CODE_200_OK;
    }

    if (!m_httpRouterFunction.empty())
    {
        return HandleRequestRouter(ipoHttpRequest, ipoHttpResponse, account);
    }

    const auto path = std::string_view{ipoHttpRequest->sOriginalUri}.substr(1 /* first slash */ + m_strResourceName.size());
    const bool isCall = path.size() >= sizeof("/call") && !strnicmp(path.data(), "/call/", 6);

    if (isCall)
    {
        return HandleRequestCall(ipoHttpRequest, ipoHttpResponse, account);
    }

    return HandleRequestActive(ipoHttpRequest, ipoHttpResponse, account);
}

std::string Unescape(std::string_view sv)
{
    // Converts a character to a hexadecimal value
    auto toHex = [](char c) {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        return 0;
    };

    std::string out;
    // String can only shrink here
    // as %?? is collapsed to a single char
    out.reserve(sv.length());
    auto it = sv.begin();
    while (it != sv.end())
    {
        if (*it == '%')
        {
            // Avoid reading past the end
            if (std::distance(it, sv.end()) < 3)
            {
                out.push_back(*it++);
                continue;
            }
            // Skip %
            ++it;
            // Read two digits/letters and convert to char
            uint8_t digit1 = toHex(*it++);
            uint8_t digit2 = toHex(*it++);
            out.push_back(static_cast<char>(digit1 * 0x10 + digit2));
        }
        else
        {
            // Push normally
            out.push_back(*it++);
        }
    }
    return out;
}

HttpStatusCode CResource::HandleRequestCall(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, CAccount* pAccount)
{
    if (!IsHttpAccessAllowed(pAccount))
    {
        return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
    }

#define MAX_INPUT_VARIABLES 25

    if (m_eState != EResourceState::Running)
    {
        const char* szError = "error: resource not running";
        ipoHttpResponse->SetBody(szError, strlen(szError));
        return HTTP_STATUS_CODE_200_OK;
    }

    const char* szQueryString = ipoHttpRequest->sUri.c_str();

    if (!*szQueryString)
    {
        const char* szError = "error: invalid function name";
        ipoHttpResponse->SetBody(szError, strlen(szError));
        return HTTP_STATUS_CODE_200_OK;
    }

    std::string              strFuncName;
    std::vector<std::string> vecArguments;
    const char*              pQueryArg = strchr(szQueryString, '?');

    if (!pQueryArg)
    {
        strFuncName = szQueryString;
    }
    else
    {
        strFuncName.assign(szQueryString, pQueryArg - szQueryString);
        pQueryArg++;

        const char* pEqual = nullptr;
        const char* pAnd = nullptr;

        while (*pQueryArg)
        {
            pAnd = strchr(pQueryArg, '&');

            if (!pAnd)
                pAnd = pQueryArg + strlen(pQueryArg);

            pEqual = strchr(pQueryArg, '=');

            if (pEqual && pEqual < pAnd)
            {
                std::string strKey(pQueryArg, pEqual - pQueryArg);
                int         iKey = atoi(strKey.c_str());

                if (iKey >= 0 && iKey < MAX_INPUT_VARIABLES)
                {
                    std::string strValue(pEqual + 1, pAnd - (pEqual + 1));
                    strValue = Unescape(strValue);

                    if (iKey + 1 > static_cast<int>(vecArguments.size()))
                        vecArguments.resize(iKey + 1);

                    vecArguments[iKey] = strValue;
                }
            }

            if (*pAnd)
                pQueryArg = pAnd + 1;
            else
                break;
        }
    }

    strFuncName = Unescape(strFuncName);

    for (CExportedFunction& Exported : m_ExportedFunctions)
    {
        if (strFuncName != Exported.GetFunctionName())
            continue;

        if (!Exported.IsHTTPAccessible())
            return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);

        SString strResourceFuncName("%s.function.%s", m_strResourceName.c_str(), strFuncName.c_str());

        // @@@@@ Deal with this the new way
        if (!g_pGame->GetACLManager()->CanObjectUseRight(pAccount->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                         strResourceFuncName.c_str(), CAccessControlListRight::RIGHT_TYPE_RESOURCE, true))
        {
            return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
        }

        CLuaArguments Arguments;

        if (ipoHttpRequest->nRequestMethod == REQUESTMETHOD_GET)
        {
            for (const std::string& strArgument : vecArguments)
            {
                const char* szArg = strArgument.c_str();

                if (strlen(szArg) > 3 && szArg[0] == '^' && szArg[2] == '^' && szArg[1] != '^')
                {
                    switch (szArg[1])
                    {
                        case 'E':            // element
                        {
                            int       id = atoi(szArg + 3);
                            CElement* pElement = nullptr;

                            if (id != INT_MAX && id != INT_MIN && id != 0)
                                pElement = CElementIDs::GetElement(id);

                            if (pElement)
                            {
                                Arguments.PushElement(pElement);
                            }
                            else
                            {
                                g_pGame->GetScriptDebugging()->LogError(nullptr, "HTTP Get - Invalid element specified.");
                                Arguments.PushNil();
                            }

                            break;
                        }
                        case 'R':            // resource
                        {
                            CResource* pResource = g_pGame->GetResourceManager()->GetResource(szArg + 3);

                            if (pResource)
                            {
                                Arguments.PushResource(pResource);
                            }
                            else
                            {
                                g_pGame->GetScriptDebugging()->LogError(nullptr, "HTTP Get - Invalid resource specified.");
                                Arguments.PushNil();
                            }

                            break;
                        }
                    }
                }
                else
                {
                    Arguments.PushString(szArg);
                }
            }
        }
        else if (ipoHttpRequest->nRequestMethod == REQUESTMETHOD_POST)
        {
            const char* szRequestBody = ipoHttpRequest->sBody.c_str();
            Arguments.ReadFromJSONString(szRequestBody);
        }

        CLuaArguments FormData;
        for (const auto& pair : ipoHttpRequest->oFormValueMap)
        {
            FormData.PushString(pair.first.c_str());
            FormData.PushString(pair.second.sBody.c_str());
        }

        CLuaArguments Cookies;
        for (const auto& pair : ipoHttpRequest->oCookieMap)
        {
            Cookies.PushString(pair.first.c_str());
            Cookies.PushString(pair.second.c_str());
        }

        CLuaArguments Headers;
        for (const auto& pair : ipoHttpRequest->oRequestHeaders)
        {
            Headers.PushString(pair.first.c_str());
            Headers.PushString(pair.second.c_str());
        }

        LUA_CHECKSTACK(m_pVM->GetVM(), 1);            // Ensure some room

        // cache old data
        lua_getglobal(m_pVM->GetVM(), "form");
        CLuaArgument OldForm(m_pVM->GetVM(), -1);
        lua_pop(m_pVM->GetVM(), 1);

        lua_getglobal(m_pVM->GetVM(), "cookies");
        CLuaArgument OldCookies(m_pVM->GetVM(), -1);
        lua_pop(m_pVM->GetVM(), 1);

        lua_getglobal(m_pVM->GetVM(), "requestHeaders");
        CLuaArgument OldHeaders(m_pVM->GetVM(), -1);
        lua_pop(m_pVM->GetVM(), 1);

        lua_getglobal(m_pVM->GetVM(), "hostname");
        CLuaArgument OldHostname(m_pVM->GetVM(), -1);
        lua_pop(m_pVM->GetVM(), 1);

        lua_getglobal(m_pVM->GetVM(), "url");
        CLuaArgument OldURL(m_pVM->GetVM(), -1);
        lua_pop(m_pVM->GetVM(), 1);

        lua_getglobal(m_pVM->GetVM(), "user");
        CLuaArgument OldUser(m_pVM->GetVM(), -1);
        lua_pop(m_pVM->GetVM(), 1);

        // push new data
        FormData.PushAsTable(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "form");

        Cookies.PushAsTable(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "cookies");

        Headers.PushAsTable(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "requestHeaders");

        lua_pushstring(m_pVM->GetVM(), ipoHttpRequest->GetAddress().c_str());
        lua_setglobal(m_pVM->GetVM(), "hostname");

        lua_pushstring(m_pVM->GetVM(), ipoHttpRequest->sOriginalUri.c_str());
        lua_setglobal(m_pVM->GetVM(), "url");

        lua_pushaccount(m_pVM->GetVM(), pAccount);
        lua_setglobal(m_pVM->GetVM(), "user");

        CLuaArguments Returns;
        Arguments.CallGlobal(m_pVM, strFuncName.c_str(), &Returns);
        // g_pGame->Unlock(); // release the mutex

        // restore old data
        OldForm.Push(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "form");

        OldCookies.Push(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "cookies");

        OldHeaders.Push(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "requestHeaders");

        OldHostname.Push(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "hostname");

        OldURL.Push(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "url");

        OldUser.Push(m_pVM->GetVM());
        lua_setglobal(m_pVM->GetVM(), "user");

        // Set debug info in case error occurs in WriteToJSONString
        g_pGame->GetScriptDebugging()->SaveLuaDebugInfo(SLuaDebugInfo(m_strResourceName, INVALID_LINE_NUMBER, SString("[HTTP:%s]", strFuncName.c_str())));

        std::string strJSON;
        Returns.WriteToJSONString(strJSON, true);

        g_pGame->GetScriptDebugging()->SaveLuaDebugInfo(SLuaDebugInfo());

        ipoHttpResponse->SetBody(strJSON.c_str(), strJSON.length());
        return HTTP_STATUS_CODE_200_OK;
    }

    const char* szError = "error: not found";
    ipoHttpResponse->SetBody(szError, strlen(szError));
    return HTTP_STATUS_CODE_200_OK;
}

static HttpStatusCode ParseLuaHttpRouterResponse(CLuaArguments& luaResponse, HttpResponse& httpResponse)
{
    bool           hasBody = false;
    HttpStatusCode responseCode = HTTP_STATUS_CODE_200_OK;

    for (size_t i = 0; i < luaResponse.Count(); i += 2)
    {
        CLuaArgument* argName = luaResponse[i + 0];
        CLuaArgument* argValue = luaResponse[i + 1];

        std::string_view key;

        if (!argName->TryGetString(key))
            continue;

        if (key == "status")
        {
            if (lua_Number status; argValue->TryGetNumber(status))
            {
                responseCode = static_cast<HttpStatusCode>(status);
            }
        }
        else if (key == "body")
        {
            if (std::string_view body; argValue->TryGetString(body))
            {
                if (body.size() <= std::numeric_limits<int>::max())
                {
                    hasBody = true;
                    httpResponse.SetBody(body.data(), body.size());
                }
            }
        }
        else if (key == "headers")
        {
            if (CLuaArguments* headers; argValue->TryGetTable(headers))
            {
                for (size_t j = 0; j < headers->Count(); j += 2)
                {
                    argName = (*headers)[j + 0];
                    argValue = (*headers)[j + 1];

                    if (std::string_view n, v; argName->TryGetString(n) && argValue->TryGetString(v))
                    {
                        httpResponse.oResponseHeaders[std::string{n}] = std::string{v};
                    }
                }
            }
        }
        else if (key == "cookies")
        {
            if (CLuaArguments* cookies; argValue->TryGetTable(cookies))
            {
                for (size_t j = 0; j < cookies->Count(); j += 2)
                {
                    argName = (*cookies)[j + 0];
                    argValue = (*cookies)[j + 1];

                    std::string_view n, v;
                    
                    if (argName->TryGetString(n) && argValue->TryGetString(v))
                    {
                        CookieParameters cookie;
                        cookie["name"] = std::string{n};
                        cookie["value"] = std::string{v};
                        httpResponse.SetCookie(cookie);
                    }
                    else if (CLuaArguments* properties; argValue->TryGetTable(properties))
                    {
                        CookieParameters cookie;

                        for (size_t k = 0; k < properties->Count(); k += 2)
                        {
                            argName = (*properties)[k + 0];
                            argValue = (*properties)[k + 1];

                            if (argName->TryGetString(n) && argValue->TryGetString(v))
                            {
                                cookie[std::string{n}] = std::string{v};
                            }
                        }

                        httpResponse.SetCookie(cookie);
                    }
                }
            }
        }
    }

    if (!hasBody)
        httpResponse.SetBody("", 0);

    return responseCode;
}

HttpStatusCode CResource::HandleRequestRouter(HttpRequest* request, HttpResponse* response, CAccount* account)
{
    if (!IsHttpAccessAllowed(account))
    {
        return g_pGame->GetHTTPD()->RequestLogin(request, response);
    }

    if (!g_pGame->GetACLManager()->CanObjectUseRight(account->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER, m_httpRouterAclRight.c_str(),
                                                     CAccessControlListRight::RIGHT_TYPE_RESOURCE, true))
    {
        return g_pGame->GetHTTPD()->RequestLogin(request, response);
    }

    if (m_eState != EResourceState::Running)
    {
        const char* errorText = "error: resource not running";
        response->SetBody(errorText, strlen(errorText));
        return HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
    }

    if (m_httpRouterCheck == HTTP_ROUTER_CHECK_PENDING)
    {
        lua_State* L = m_pVM->GetVM();
        lua_pushlstring(L, m_httpRouterFunction.c_str(), m_httpRouterFunction.size());
        lua_gettable(L, LUA_GLOBALSINDEX);
        {
            m_httpRouterCheck = lua_isfunction(L, -1) ? HTTP_ROUTER_CHECK_PASSED : HTTP_ROUTER_CHECK_FAILED;
        }
        lua_pop(L, 1);
    }

    if (m_httpRouterCheck != HTTP_ROUTER_CHECK_PASSED)
    {
        const char* errorText = "error: router function unavailable";
        response->SetBody(errorText, strlen(errorText));
        return HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
    }

    std::string_view path = std::string_view{request->sOriginalUri}.substr(1 /* first slash */ + m_strResourceName.size());

    if (const auto index = path.find_first_of("?&"); index != std::string_view::npos)
    {
        path = path.substr(0, index);
    }

    CLuaArguments luaRequest;
    {
        luaRequest.PushString("account");
        luaRequest.PushAccount(account);

        luaRequest.PushString("method");
        switch (request->nRequestMethod)
        {
            case REQUESTMETHOD_OPTIONS:
                luaRequest.PushString("OPTIONS");
                break;
            case REQUESTMETHOD_GET:
                luaRequest.PushString("GET");
                break;
            case REQUESTMETHOD_HEAD:
                luaRequest.PushString("HEAD");
                break;
            case REQUESTMETHOD_POST:
                luaRequest.PushString("POST");
                break;
            case REQUESTMETHOD_PUT:
                luaRequest.PushString("PUT");
                break;
            case REQUESTMETHOD_DELETE:
                luaRequest.PushString("DELETE");
                break;
            case REQUESTMETHOD_TRACE:
                luaRequest.PushString("TRACE");
                break;
            case REQUESTMETHOD_CONNECT:
                luaRequest.PushString("CONNECT");
                break;
            case REQUESTMETHOD_PATCH:
                luaRequest.PushString("PATCH");
                break;
            default:
                luaRequest.PushString("*");
                break;
        }
        
        luaRequest.PushString("path");
        luaRequest.PushString(path);

        luaRequest.PushString("absolutePath");
        luaRequest.PushString(request->sOriginalUri);

        luaRequest.PushString("hostname");
        luaRequest.PushString(request->GetAddress());

        luaRequest.PushString("port");
        luaRequest.PushNumber(request->GetPort());

        luaRequest.PushString("body");
        luaRequest.PushString(request->sBody);

        CLuaArguments query;
        for (const auto& pair : request->oQueryValueMap)
        {
            query.PushString(pair.first);
            query.PushString(pair.second.sBody);
        }
        luaRequest.PushString("query");
        luaRequest.PushTable(&query);

        CLuaArguments formData;
        for (const auto& pair : request->oFormValueMap)
        {
            formData.PushString(pair.first);
            formData.PushString(pair.second.sBody);
        }
        luaRequest.PushString("formData");
        luaRequest.PushTable(&formData);
        
        CLuaArguments cookies;
        for (const auto& pair : request->oCookieMap)
        {
            cookies.PushString(pair.first);
            cookies.PushString(pair.second);
        }
        luaRequest.PushString("cookies");
        luaRequest.PushTable(&cookies);

        CLuaArguments headers;
        for (const auto& pair : request->oRequestHeaders)
        {
            headers.PushString(pair.first);
            headers.PushString(pair.second);
        }
        luaRequest.PushString("headers");
        luaRequest.PushTable(&headers);
    }

    CLuaArguments arguments;
    arguments.PushTable(&luaRequest);

    CLuaArguments results;

    if (!arguments.CallGlobal(m_pVM, m_httpRouterFunction.c_str(), &results))
    {
        const char* errorText = "error: router function error";
        response->SetBody(errorText, strlen(errorText));
        return HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
    }

    HttpStatusCode responseCode = HTTP_STATUS_CODE_200_OK;

    if (results.Count() >= 1)
    {
        if (lua_Number statusCode; results[0]->TryGetNumber(statusCode))
        {
            response->SetBody("", 0);
            responseCode = static_cast<HttpStatusCode>(statusCode);
        }
        else if (CLuaArguments* luaResponse; results[0]->TryGetTable(luaResponse))
        {
            responseCode = ParseLuaHttpRouterResponse(*luaResponse, *response);
        }
    }
    else
    {
        response->SetBody("", 0);
    }

    return responseCode;
}

HttpStatusCode CResource::HandleRequestActive(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, CAccount* pAccount)
{
    const char* szUrl = ipoHttpRequest->sOriginalUri.c_str();
    std::string strFile;

    if (szUrl[0])
    {
        const char* pFileFrom = strchr(szUrl[0] == '/' ? &szUrl[1] : szUrl, '/');

        if (pFileFrom)
        {
            pFileFrom++;
            const char* pFileTo = strchr(pFileFrom, '?');

            if (pFileTo)
                strFile.assign(pFileFrom, pFileTo - pFileFrom);
            else
                strFile = pFileFrom;
        }
    }

    Unescape(strFile);
    strFile = Unescape(strFile);

    for (CResourceFile* pResourceFile : m_ResourceFiles)
    {
        if (!strFile.empty())
        {
            if (strcmp(pResourceFile->GetName(), strFile.c_str()) != 0)
                continue;

            if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML)
            {
                CResourceHTMLItem* pHtml = (CResourceHTMLItem*)pResourceFile;

                // We need to be active if downloading a HTML file
                if (m_eState == EResourceState::Running)
                {
                    if (!IsHttpAccessAllowed(pAccount))
                    {
                        return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
                    }

                    SString strResourceFileName("%s.file.%s", m_strResourceName.c_str(), pHtml->GetName());
                    if (g_pGame->GetACLManager()->CanObjectUseRight(pAccount->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                                    strResourceFileName.c_str(), CAccessControlListRight::RIGHT_TYPE_RESOURCE,
                                                                    !pHtml->IsRestricted()))
                    {
                        return pHtml->Request(ipoHttpRequest, ipoHttpResponse, pAccount);
                    }

                    return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
                }
                else
                {
                    SString err = "That resource is not running.";
                    ipoHttpResponse->SetBody(err.c_str(), err.size());
                    return HTTP_STATUS_CODE_401_UNAUTHORIZED;
                }
            }
            // Send back any clientfile. Otherwise keep looking for server files matching
            // this filename. If none match, the file not found will be sent back.
            else if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG ||
                     pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT ||
                     pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_FILE)
            {
                return pResourceFile->Request(ipoHttpRequest, ipoHttpResponse);            // sends back any file in the resource
            }
        }
        else            // handle the default page
        {
            if (!IsHttpAccessAllowed(pAccount))
            {
                return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
            }

            if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML)
            {
                CResourceHTMLItem* pHtml = (CResourceHTMLItem*)pResourceFile;

                if (pHtml->IsDefaultPage())
                {
                    CAccessControlListManager* pACLManager = g_pGame->GetACLManager();

                    SString strResourceFileName("%s.file.%s", m_strResourceName.c_str(), pResourceFile->GetName());
                    if (pACLManager->CanObjectUseRight(pAccount->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER, m_strResourceName.c_str(),
                                                       CAccessControlListRight::RIGHT_TYPE_RESOURCE, true) &&
                        pACLManager->CanObjectUseRight(pAccount->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                       strResourceFileName.c_str(), CAccessControlListRight::RIGHT_TYPE_RESOURCE, !pHtml->IsRestricted()))
                    {
                        return pHtml->Request(ipoHttpRequest, ipoHttpResponse, pAccount);
                    }
                    else
                    {
                        return g_pGame->GetHTTPD()->RequestLogin(ipoHttpRequest, ipoHttpResponse);
                    }
                }
            }
        }
    }

    SString err = "That resource file could not be found in that resource.";
    ipoHttpResponse->SetBody(err.c_str(), err.size());
    return HTTP_STATUS_CODE_404_NOT_FOUND;
}

// Return true if http access allowed for the supplied account
bool CResource::IsHttpAccessAllowed(CAccount* pAccount)
{
    CAccessControlListManager* pACLManager = g_pGame->GetACLManager();

    // New way
    // Check for "resource.<name>.http" being explicitly allowed
    if (pACLManager->CanObjectUseRight(pAccount->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, m_strResourceName + ".http",
                                       CAccessControlListRight::RIGHT_TYPE_RESOURCE, false))
    {
        return true;
    }

    // Old way phase 1
    // Check for "general.http" being explicitly denied
    if (!pACLManager->CanObjectUseRight(pAccount->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, "http",
                                        CAccessControlListRight::RIGHT_TYPE_GENERAL, true))
    {
        return false;
    }
    // Check for "resource.<name>" being explicitly denied
    if (!pACLManager->CanObjectUseRight(pAccount->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, m_strResourceName,
                                        CAccessControlListRight::RIGHT_TYPE_RESOURCE, true))
    {
        return false;
    }

    // Old way phase 2
    // Check for "general.http" being explicitly allowed
    if (pACLManager->CanObjectUseRight(pAccount->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, "http",
                                       CAccessControlListRight::RIGHT_TYPE_GENERAL, false))
    {
        return true;
    }
    // Check for "resource.<name>" being explicitly allowed
    if (pACLManager->CanObjectUseRight(pAccount->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER, m_strResourceName,
                                       CAccessControlListRight::RIGHT_TYPE_RESOURCE, false))
    {
        return true;
    }

    // If nothing explicitly set, then default to denied
    return false;
}

bool CResource::CallExportedFunction(const char* szFunctionName, CLuaArguments& Arguments, CLuaArguments& Returns, CResource& Caller)
{
    if (m_eState != EResourceState::Running)
        return false;

    for (CExportedFunction& Exported : m_ExportedFunctions)
    {
        // Verify that the exported function is marked as "Server" (since both Client and Server exported functions exist here)
        if (Exported.GetType() == CExportedFunction::EXPORTED_FUNCTION_TYPE_SERVER)
        {
            bool bRestricted = Exported.IsRestricted();

            if (!strcmp(Exported.GetFunctionName().c_str(), szFunctionName))
            {
                char szFunctionRightName[512];
                snprintf(szFunctionRightName, 512, "%s.function.%s", m_strResourceName.c_str(), szFunctionName);

                CAccessControlListManager* pACLManager = g_pGame->GetACLManager();

                if (pACLManager->CanObjectUseRight(Caller.GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, m_strResourceName.c_str(),
                                                   CAccessControlListRight::RIGHT_TYPE_RESOURCE, !bRestricted) &&
                    pACLManager->CanObjectUseRight(Caller.GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, szFunctionRightName,
                                                   CAccessControlListRight::RIGHT_TYPE_RESOURCE, !bRestricted))
                {
                    if (Arguments.CallGlobal(m_pVM, szFunctionName, &Returns))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool CResource::CheckState()
{
    if (m_Dependents.empty() && !m_bIsPersistent)
    {
        Stop(false);
        return false;
    }
    else
    {
        return Start();
    }
}

void CResource::OnPlayerJoin(CPlayer& Player)
{
    // do the player join crap
    Player.Send(CResourceStartPacket(m_strResourceName.c_str(), this));
    SendNoClientCacheScripts(&Player);
}

void CResource::SendNoClientCacheScripts(CPlayer* pPlayer)
{
    if (!IsClientScriptsOn())
        return;

    std::vector<CPlayer*> vecPlayers;

    // Send it to either a single player or all the players in the server.
    if (pPlayer)
        vecPlayers.push_back(pPlayer);
    else
    {
        std::list<CPlayer*>::const_iterator iter = g_pGame->GetPlayerManager()->IterBegin();

        for (; iter != g_pGame->GetPlayerManager()->IterEnd(); ++iter)
        {
            vecPlayers.push_back(*iter);
        }
    }

    if (!vecPlayers.empty())
    {
        // Decide what scripts to send
        CResourceClientScriptsPacket Packet(this);
        bool                         bEmptyPacket = true;

        for (CResourceFile* pResourceFile : m_ResourceFiles)
        {
            if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CLIENT_SCRIPT)
            {
                CResourceClientScriptItem* pClientScript = static_cast<CResourceClientScriptItem*>(pResourceFile);

                if (pClientScript->IsNoClientCache() == true)
                {
                    Packet.AddItem(pClientScript);
                    bEmptyPacket = false;
                }
            }
        }

        // Send them!
        if (!bEmptyPacket)
        {
            for (std::vector<CPlayer*>::iterator iter = vecPlayers.begin(); iter != vecPlayers.end(); ++iter)
            {
                (*iter)->Send(Packet);
            }
        }
    }
}

bool CResource::UnzipResource()
{
    m_zipfile = unzOpenUtf8(m_strResourceZip.c_str());

    if (!m_zipfile)
        return false;

    // See if the dir already exists
    bool bDirExists = DoesDirectoryExist(m_strResourceCachePath.c_str());

    // If the folder doesn't exist, create it
    if (!bDirExists)
    {
        // If we're using a zip file, we need a temp directory for extracting
        // 17 = already exists (on windows)
        if (File::Mkdir(m_strResourceCachePath.c_str()) == -1 && errno != EEXIST)            // check this is the correct return for *NIX too
        {
            // Show error
            m_strFailureReason = SString("Couldn't create directory '%s' for resource '%s', check that the server has write access to the resources folder.\n",
                                         m_strResourceCachePath.c_str(), m_strResourceName.c_str());
            CLogger::ErrorPrintf(m_strFailureReason);
            return false;
        }
    }

    std::vector<char> strFileName;
    std::string       strPath;

    if (unzGoToFirstFile(m_zipfile) == UNZ_OK)
    {
        do
        {
            // Check if we have this file already extracted
            unz_file_info fileInfo = {0};

            if (unzGetCurrentFileInfo(m_zipfile, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK)
                return false;

            strFileName.reserve(fileInfo.size_filename + 1);
            unzGetCurrentFileInfo(m_zipfile, &fileInfo, strFileName.data(), strFileName.capacity() - 1, nullptr, 0, nullptr, 0);

            // Check if the current file is a directory path
            if (strFileName[fileInfo.size_filename - 1] == '/')
                continue;

            strFileName[fileInfo.size_filename] = '\0';
            strPath = m_strResourceCachePath + strFileName.data();

            if (FileExists(strPath))
            {
                // We've already got a cached copy of this file, check its still the same
                unsigned long ulFileInZipCRC = fileInfo.crc;
                unsigned long ulFileOnDiskCRC = CRCGenerator::GetCRCFromFile(strPath.c_str());

                if (ulFileInZipCRC == ulFileOnDiskCRC)
                    continue;            // we've already extracted EXACTLY this file before

                RemoveFile(strPath.c_str());
            }

            // Doesn't exist or bad crc
            int opt_extract_without_path = 0;
            int opt_overwrite = 1;
            int ires = do_extract_currentfile(m_zipfile, &opt_extract_without_path, &opt_overwrite, nullptr, m_strResourceCachePath.c_str());

            if (ires != UNZ_OK)
                return false;
        } while (unzGoToNextFile(m_zipfile) != UNZ_END_OF_LIST_OF_FILE);
    }

    // Close the zip file
    unzClose(m_zipfile);
    m_zipfile = nullptr;

    // Store the hash so we can figure out whether it has changed later
    m_zipHash = CChecksum::GenerateChecksumFromFileUnsafe(m_strResourceZip);
    return true;
}

unsigned long get_current_file_crc(unzFile uf)
{
    char filename_inzip[256];
    int  err = UNZ_OK;

    unz_file_info file_info;
    err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), nullptr, 0, nullptr, 0);

    if (err == UNZ_OK)
        return file_info.crc;
    else
        return 0;
}

int makedir(char* newdir)
{
    char* buffer;
    char* p;
    int   len = (int)strlen(newdir);

    if (len <= 0)
        return 0;

    buffer = (char*)malloc(len + 1);
    strcpy(buffer, newdir);

    if (buffer[len - 1] == '/')
    {
        buffer[len - 1] = '\0';
    }
    if (File::Mkdir(buffer) == 0)
    {
        free(buffer);
        return 1;
    }

    p = buffer + 1;
    while (1)
    {
        char hold;

        while (*p && *p != '\\' && *p != '/')
            p++;
        hold = *p;
        *p = 0;
        if ((File::Mkdir(buffer) == -1) && (errno == ENOENT))
        {
            // printf("couldn't create directory %s\n",buffer);
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
void change_file_date(const char* filename, uLong dosdate, tm_unz tmu_date)
{
#ifdef WIN32
    HANDLE   hFile;
    FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

    hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
    DosDateTimeToFileTime((WORD)(dosdate >> 16), (WORD)dosdate, &ftLocal);
    LocalFileTimeToFileTime(&ftLocal, &ftm);
    SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);
    CloseHandle(hFile);
#else
#ifdef unix
    struct utimbuf ut;
    struct tm      newdate;
    newdate.tm_sec = tmu_date.tm_sec;
    newdate.tm_min = tmu_date.tm_min;
    newdate.tm_hour = tmu_date.tm_hour;
    newdate.tm_mday = tmu_date.tm_mday;
    newdate.tm_mon = tmu_date.tm_mon;
    if (tmu_date.tm_year > 1900)
        newdate.tm_year = tmu_date.tm_year - 1900;
    else
        newdate.tm_year = tmu_date.tm_year;
    newdate.tm_isdst = -1;

    ut.actime = ut.modtime = mktime(&newdate);
    utime(filename, &ut);
#endif
#endif
}

#define WRITEBUFFERSIZE (8192)
int do_extract_currentfile(unzFile uf, const int* popt_extract_without_path, int* popt_overwrite, const char* password, const char* szFilePath)
{
    char  filename_inzip[256];
    char* filename_withoutpath;
    char* p;
    int   err = UNZ_OK;
    FILE* fout = nullptr;
    void* buf;
    uInt  size_buf;

    unz_file_info file_info;
    err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), nullptr, 0, nullptr, 0);

    if (err != UNZ_OK)
    {
        // printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
        return err;
    }

    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf == nullptr)
    {
        // printf("Error allocating memory\n");
        return UNZ_INTERNALERROR;
    }

    p = filename_withoutpath = filename_inzip;
    while ((*p) != '\0')
    {
        if (((*p) == '/') || ((*p) == '\\'))
            filename_withoutpath = p + 1;
        p++;
    }

    if ((*filename_withoutpath) == '\0')
    {
        if ((*popt_extract_without_path) == 0)
        {
            // printf("creating directory: %s\n",filename_inzip);
            File::Mkdir(filename_inzip);
        }
    }
    else
    {
        const char* write_filename;
        int         skip = 0;

        if ((*popt_extract_without_path) == 0)
            write_filename = filename_inzip;
        else
            write_filename = filename_withoutpath;

        err = unzOpenCurrentFilePassword(uf, password);
        if (err != UNZ_OK)
        {
            // printf("error %d with zipfile in unzOpenCurrentFilePassword\n",err);
        }

        // ChrML: We always overwrite and this stuff seems to leak files.
        /*
        if (((*popt_overwrite)==0) && (err==UNZ_OK))
        {
            FILE* ftestexist;
            ftestexist = fopen(write_filename,"rb");
            if (ftestexist!=nullptr)
            {
                skip = 1;
            }
        }
        */

        // prepend the filepath to read from
        File::Mkdir(szFilePath);
        char   szOutFile[MAX_PATH];
        size_t lenFilePath = strlen(szFilePath);
        if (szFilePath[lenFilePath - 1] == '\\' || szFilePath[lenFilePath - 1] == '/')
        {
            snprintf(szOutFile, MAX_PATH, "%s%s", szFilePath, write_filename);
        }
        else
        {
            snprintf(szOutFile, MAX_PATH, "%s/%s", szFilePath, write_filename);
        }

        if ((skip == 0) && (err == UNZ_OK))
        {
            fout = File::Fopen(szOutFile, "wb");

            /* some zipfile don't contain directory alone before file */
            if ((fout == nullptr) && ((*popt_extract_without_path) == 0) && (filename_withoutpath != (char*)filename_inzip))
            {
                MakeSureDirExists(szOutFile);
                char c = *(filename_withoutpath - 1);
                *(filename_withoutpath - 1) = '\0';
                // makedir((char *)write_filename);
                *(filename_withoutpath - 1) = c;
                fout = File::Fopen(szOutFile, "wb");
            }

            if (fout == nullptr)
            {
                // printf("error opening %s\n",write_filename);
            }
        }

        if (fout != nullptr)
        {
            // printf(" extracting: %s\n",write_filename);

            do
            {
                err = unzReadCurrentFile(uf, buf, size_buf);
                if (err < 0)
                {
                    // printf("error %d with zipfile in unzReadCurrentFile\n",err);
                    break;
                }
                if (err > 0)
                    if (fwrite(buf, err, 1, fout) != 1)
                    {
                        // printf("error in writing extracted file\n");
                        err = UNZ_ERRNO;
                        break;
                    }
            } while (err > 0);
            if (fout)
                fclose(fout);

            if (err == 0)
                change_file_date(write_filename, file_info.dosDate, file_info.tmu_date);
        }

        if (err == UNZ_OK)
        {
            err = unzCloseCurrentFile(uf);
            if (err != UNZ_OK)
            {
                // printf("error %d with zipfile in unzCloseCurrentFile\n",err);
            }
        }
        else
            unzCloseCurrentFile(uf); /* don't lose the error */
    }

    free(buf);
    return err;
}

bool CIncludedResources::CreateLink()            // just a pointer to it
{
    // Grab the resource that we are
    m_pResource = m_pResourceManager->GetResource(m_strResourceName.c_str());

    m_bBadVersion = false;

    // Does it exist?
    if (!m_pResource)
        m_bExists = false;
    else
        m_bExists = true;

    if (m_pResource)
    {
        // Grab the version and check that it's in range
        /*unsigned int uiVersion = m_pResource->GetVersion ();
        if ( uiVersion < m_uiMinimumVersion || uiVersion > m_uiMaximumVersion )
        {
            CLogger::ErrorPrintf ( "Incompatible version of %s for resource %s\n", m_szResourceName, m_pOwner->GetName().c_str () );
            CLogger::ErrorPrintf ( "Version between %u and %u needed, version %u installed\n", m_uiMinimumVersion, m_uiMaximumVersion, uiVersion );
            m_bExists = false;
            m_bBadVersion = true;
        }*/
    }

    return m_bExists;
}

void CResource::InvalidateIncludedResourceReference(CResource* pResource)
{
    for (CIncludedResources* pIncludedResources : m_IncludedResources)
    {
        if (pIncludedResources->GetResource() == pResource)
            pIncludedResources->InvalidateReference();
    }

    m_TemporaryIncludes.remove(pResource);
    assert(this != pResource);
    m_Dependents.remove(pResource);
}

//
// Check ACL 'function right' cache for this resource.
// Will automatically clear the cache if ACL has changed.
//
// Returns true if setting was found in cache
//
bool CResource::CheckFunctionRightCache(lua_CFunction f, bool* pbOutAllowed)
{
    uint uiACLRevision = g_pGame->GetACLManager()->GetGlobalRevision();

    // Check validity of cache
    if (m_uiFunctionRightCacheRevision != uiACLRevision)
    {
        m_FunctionRightCacheMap.clear();
        m_uiFunctionRightCacheRevision = uiACLRevision;
    }

    // Read cache
    bool* pResult = MapFind(m_FunctionRightCacheMap, f);

    if (!pResult)
        return false;

    *pbOutAllowed = *pResult;
    return true;
}

//
// Update ACL 'function right' cache for this resource.
//
void CResource::UpdateFunctionRightCache(lua_CFunction f, bool bAllowed)
{
    MapSet(m_FunctionRightCacheMap, f, bAllowed);
}

//
// Check resource files for probable use of dbConnect + mysql
//
bool CResource::IsUsingDbConnectMysql()
{
    if (!m_bDoneDbConnectMysqlScan)
    {
        m_bDoneDbConnectMysqlScan = true;

        for (CResourceFile* pResourceFile : m_ResourceFiles)
        {
            if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_SCRIPT)
            {
                SString strLuaSource;
                FileLoad(pResourceFile->GetFullName(), strLuaSource);

                for (size_t curPos = 0; curPos < strLuaSource.length(); curPos++)
                {
                    curPos = strLuaSource.find("dbConnect", curPos);

                    if (curPos == SString::npos)
                        break;

                    size_t foundPos = strLuaSource.find("mysql", curPos);

                    if (foundPos > curPos && foundPos < curPos + 40)
                    {
                        m_bUsingDbConnectMysql = true;
                    }
                }
            }
        }
    }

    return m_bUsingDbConnectMysql;
}

//
// Return true if file access should be denied to other resources
//
bool CResource::IsFileDbConnectMysqlProtected(const SString& strAbsFilename, bool bReadOnly)
{
    if (!IsUsingDbConnectMysql())
        return false;

    SString strFilename = ExtractFilename(strAbsFilename);

    if (strFilename.CompareI("meta.xml"))
    {
        if (!bReadOnly)
        {
            // No write access to meta.xml
            return true;
        }
    }

    for (CResourceFile* pResourceFile : m_ResourceFiles)
    {
        if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_SCRIPT)
        {
            SString strResourceFilename = ExtractFilename(pResourceFile->GetName());

            if (strFilename.CompareI(strResourceFilename))
            {
                // No read/write access to server script files
                return true;
            }
        }
    }

    return false;
}

CResourceFile* CResource::GetResourceFile(const SString& relativePath) const
{
    for (CResourceFile* resourceFile : m_ResourceFiles)
    {
        if (!stricmp(relativePath.c_str(), resourceFile->GetName()))
        {
            return resourceFile;
        }
    }

    return nullptr;
}
