/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResource.cpp
 *  PURPOSE:     Resource object
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CResource
#include "profiler/SharedUtil.Profiler.h"
#include "CServerIdManager.h"

using namespace std;

extern CClientGame* g_pClientGame;

int CResource::m_iShowingCursor = 0;

CResource::CResource(unsigned short usNetID, const char* szResourceName, CClientEntity* pResourceEntity, CClientEntity* pResourceDynamicEntity,
                     const CMtaVersion& strMinServerReq, const CMtaVersion& strMinClientReq, bool bEnableOOP)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::RESOURCE);
    m_usNetID = usNetID;
    m_bActive = false;
    m_bStarting = true;
    m_bStopping = false;
    m_bShowingCursor = false;
    m_usRemainingNoClientCacheScripts = 0;
    m_bLoadAfterReceivingNoClientCacheScripts = false;
    m_strMinServerReq = strMinServerReq;
    m_strMinClientReq = strMinClientReq;

    if (szResourceName)
        m_strResourceName.AssignLeft(szResourceName, MAX_RESOURCE_NAME_LENGTH);

    m_pLuaManager = g_pClientGame->GetLuaManager();
    m_pRootEntity = g_pClientGame->GetRootEntity();
    m_pDefaultElementGroup = new CElementGroup();            // for use by scripts
    m_pResourceEntity = pResourceEntity;
    m_pResourceDynamicEntity = pResourceDynamicEntity;

    // Create our GUI root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceGUIEntity = new CClientDummy(g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "guiroot");
    m_pResourceGUIEntity->MakeSystemEntity();

    // Create our COL root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceCOLRoot = new CClientDummy(g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "colmodelroot");
    m_pResourceCOLRoot->MakeSystemEntity();

    // Create our DFF root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceDFFEntity = new CClientDummy(g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "dffroot");
    m_pResourceDFFEntity->MakeSystemEntity();

    // Create our TXD root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceTXDRoot = new CClientDummy(g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "txdroot");
    m_pResourceTXDRoot->MakeSystemEntity();

    // Create our IFP root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceIFPRoot = new CClientDummy(g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "ifproot");
    m_pResourceIFPRoot->MakeSystemEntity();

    // Create our IMG root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceIMGRoot = new CClientDummy(g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "imgroot");
    m_pResourceIMGRoot->MakeSystemEntity();

    m_strResourceDirectoryPath = SString("%s/resources/%s", g_pClientGame->GetFileCacheRoot(), *m_strResourceName);
    m_strResourcePrivateDirectoryPath = PathJoin(CServerIdManager::GetSingleton()->GetConnectionPrivateDirectory(), m_strResourceName);

    m_strResourcePrivateDirectoryPathOld = CServerIdManager::GetSingleton()->GetConnectionPrivateDirectory(true);
    if (!m_strResourcePrivateDirectoryPathOld.empty())
        m_strResourcePrivateDirectoryPathOld = PathJoin(m_strResourcePrivateDirectoryPathOld, m_strResourceName);

    // Move this after the CreateVirtualMachine line and heads will roll
    m_bOOPEnabled = bEnableOOP;
    m_iDownloadPriorityGroup = 0;

    m_pLuaVM = m_pLuaManager->CreateVirtualMachine(this, bEnableOOP);
    if (m_pLuaVM)
    {
        m_pLuaVM->SetScriptName(szResourceName);
        m_pLuaVM->LoadEmbeddedScripts();
    }
}

CResource::~CResource()
{
    // Remove refrences from requested models
    m_modelStreamer.ReleaseAll();

    // Deallocate all models that this resource allocated earlier
    g_pClientGame->GetManager()->GetModelManager()->DeallocateModelsAllocatedByResource(this);

    // Delete all the resource's locally created children (the server won't do that)
    DeleteClientChildren();

    CIdArray::PushUniqueId(this, EIdClass::RESOURCE, m_uiScriptID);
    // Make sure we don't force the cursor on
    ShowCursor(false);

    // Do this before we delete our elements.
    m_pRootEntity->CleanUpForVM(m_pLuaVM, true);
    g_pClientGame->GetElementDeleter()->CleanUpForVM(m_pLuaVM);
    m_pLuaManager->RemoveVirtualMachine(m_pLuaVM);

    // Remove all keybinds on this VM
    g_pClientGame->GetScriptKeyBinds()->RemoveAllKeys(m_pLuaVM);
    g_pCore->GetKeyBinds()->SetAllCommandsActive(m_strResourceName, false);

    // Destroy the txd root so all dff elements are deleted except those moved out
    g_pClientGame->GetElementDeleter()->DeleteRecursive(m_pResourceTXDRoot);
    m_pResourceTXDRoot = NULL;

    // Destroy the ifp root so all ifp elements are deleted except those moved out
    g_pClientGame->GetElementDeleter()->DeleteRecursive(m_pResourceIFPRoot);
    m_pResourceIFPRoot = NULL;

    // Destroy the img root so all img elements are deleted except those moved out
    g_pClientGame->GetElementDeleter()->DeleteRecursive(m_pResourceIMGRoot);
    m_pResourceIMGRoot = NULL;

    // Destroy the ddf root so all dff elements are deleted except those moved out
    g_pClientGame->GetElementDeleter()->DeleteRecursive(m_pResourceDFFEntity);
    m_pResourceDFFEntity = NULL;

    // Destroy the colmodel root so all colmodel elements are deleted except those moved out
    g_pClientGame->GetElementDeleter()->DeleteRecursive(m_pResourceCOLRoot);
    m_pResourceCOLRoot = NULL;

    // Destroy the gui root so all gui elements are deleted except those moved out
    g_pClientGame->GetElementDeleter()->DeleteRecursive(m_pResourceGUIEntity);
    m_pResourceGUIEntity = NULL;

    // Undo all changes to water
    g_pGame->GetWaterManager()->UndoChanges(this);

    // Cancel all downloads started by this resource
    if (g_pClientGame->GetSingularFileDownloadManager())
        g_pClientGame->GetSingularFileDownloadManager()->CancelResourceDownloads(this);

    // Destroy the element group attached directly to this resource
    if (m_pDefaultElementGroup)
        delete m_pDefaultElementGroup;
    m_pDefaultElementGroup = NULL;

    m_pRootEntity = NULL;
    m_pResourceEntity = NULL;

    list<CResourceFile*>::iterator iter = m_ResourceFiles.begin();
    for (; iter != m_ResourceFiles.end(); ++iter)
    {
        delete (*iter);
    }
    m_ResourceFiles.clear();

    list<CResourceConfigItem*>::iterator iterc = m_ConfigFiles.begin();
    for (; iterc != m_ConfigFiles.end(); ++iterc)
    {
        delete (*iterc);
    }
    m_ConfigFiles.clear();
}

CDownloadableResource* CResource::AddResourceFile(CDownloadableResource::eResourceType resourceType, const char* szFileName, uint uiDownloadSize,
                                                  CChecksum serverChecksum, bool bAutoDownload)
{
    // Create the resource file and add it to the list
    SString strBuffer("%s\\resources\\%s\\%s", g_pClientGame->GetFileCacheRoot(), *m_strResourceName, szFileName);

    // Reject duplicates
    if (g_pClientGame->GetResourceManager()->IsResourceFile(strBuffer))
    {
        g_pClientGame->GetScriptDebugging()->LogError(NULL, "Ignoring duplicate file in resource '%s': '%s'", *m_strResourceName, szFileName);
        return NULL;
    }

    CResourceFile* pResourceFile = new CResourceFile(this, resourceType, szFileName, strBuffer, uiDownloadSize, serverChecksum, bAutoDownload);
    if (pResourceFile)
    {
        m_ResourceFiles.push_back(pResourceFile);
    }

    return pResourceFile;
}

CDownloadableResource* CResource::AddConfigFile(const char* szFileName, uint uiDownloadSize, CChecksum serverChecksum)
{
    // Create the config file and add it to the list
    SString strBuffer("%s\\resources\\%s\\%s", g_pClientGame->GetFileCacheRoot(), *m_strResourceName, szFileName);

    // Reject duplicates
    if (g_pClientGame->GetResourceManager()->IsResourceFile(strBuffer))
    {
        g_pClientGame->GetScriptDebugging()->LogError(NULL, "Ignoring duplicate file in resource '%s': '%s'", *m_strResourceName, szFileName);
        return NULL;
    }

    CResourceConfigItem* pConfig = new CResourceConfigItem(this, szFileName, strBuffer, uiDownloadSize, serverChecksum);
    if (pConfig)
    {
        m_ConfigFiles.push_back(pConfig);
    }

    return pConfig;
}

bool CResource::CallExportedFunction(const SString& name, CLuaArguments& args, CLuaArguments& returns, CResource& caller)
{
    if (m_exportedFunctions.find(name) != m_exportedFunctions.end())
        return args.CallGlobal(m_pLuaVM, name.c_str(), &returns);
    return false;
}

bool CResource::CanBeLoaded()
{
    return !IsActive() && !IsWaitingForInitialDownloads();
}

bool CResource::IsWaitingForInitialDownloads()
{
    for (std::list<CResourceConfigItem*>::iterator iter = m_ConfigFiles.begin(); iter != m_ConfigFiles.end(); ++iter)
        if ((*iter)->IsWaitingForDownload())
            return true;

    for (std::list<CResourceFile*>::iterator iter = m_ResourceFiles.begin(); iter != m_ResourceFiles.end(); ++iter)
        if ((*iter)->IsAutoDownload())
            if ((*iter)->IsWaitingForDownload())
                return true;
    return false;
}

void CResource::Load()
{
    dassert(CanBeLoaded());
    m_pRootEntity = g_pClientGame->GetRootEntity();

    if (m_usRemainingNoClientCacheScripts > 0)
    {
        m_bLoadAfterReceivingNoClientCacheScripts = true;
        return;
    }

    if (m_pRootEntity)
    {
        // Set the GUI parent to the resource root entity
        m_pResourceCOLRoot->SetParent(m_pResourceEntity);
        m_pResourceDFFEntity->SetParent(m_pResourceEntity);
        m_pResourceGUIEntity->SetParent(m_pResourceEntity);
        m_pResourceTXDRoot->SetParent(m_pResourceEntity);
    }

    CLogger::LogPrintf("> Starting resource '%s'", *m_strResourceName);

    // Flag resource files as readable
    for (std::list<CResourceConfigItem*>::iterator iter = m_ConfigFiles.begin(); iter != m_ConfigFiles.end(); ++iter)
        (*iter)->SetDownloaded();

    for (std::list<CResourceFile*>::iterator iter = m_ResourceFiles.begin(); iter != m_ResourceFiles.end(); ++iter)
        if ((*iter)->IsAutoDownload())
            (*iter)->SetDownloaded();

    // Load config files
    list<CResourceConfigItem*>::iterator iterc = m_ConfigFiles.begin();
    for (; iterc != m_ConfigFiles.end(); ++iterc)
    {
        if (!(*iterc)->Start())
        {
            CLogger::LogPrintf("Failed to start resource item %s in %s\n", (*iterc)->GetName(), *m_strResourceName);
        }
    }

    for (auto& list = m_NoClientCacheScriptList; !list.empty(); list.pop_front())
    {
        DECLARE_PROFILER_SECTION(OnPreLoadNoClientCacheScript)

        auto& item = list.front();
        GetVM()->LoadScriptFromBuffer(item.buffer.GetData(), item.buffer.GetSize(), item.strFilename);
        item.buffer.ZeroClear();

        DECLARE_PROFILER_SECTION(OnPostLoadNoClientCacheScript)
    }

    // Load the files that are queued in the list "to be loaded"
    list<CResourceFile*>::iterator iter = m_ResourceFiles.begin();
    for (; iter != m_ResourceFiles.end(); ++iter)
    {
        CResourceFile* pResourceFile = *iter;
        // Only load the resource file if it is a client script
        if (pResourceFile->GetResourceType() == CDownloadableResource::RESOURCE_FILE_TYPE_CLIENT_SCRIPT)
        {
            // Load the file
            std::vector<char> buffer;
            FileLoad(pResourceFile->GetName(), buffer);
            const char* pBufferData = buffer.empty() ? nullptr : &buffer.at(0);

            DECLARE_PROFILER_SECTION(OnPreLoadScript)
            // Check the contents
            if (CChecksum::GenerateChecksumFromBuffer(pBufferData, buffer.size()) == pResourceFile->GetServerChecksum())
            {
                m_pLuaVM->LoadScriptFromBuffer(pBufferData, buffer.size(), pResourceFile->GetName());
            }
            else
            {
                HandleDownloadedFileTrouble(pResourceFile, true);
            }
            DECLARE_PROFILER_SECTION(OnPostLoadScript)
        }
        else if (pResourceFile->IsAutoDownload())
        {
            // Check the file contents
            if (CChecksum::GenerateChecksumFromFileUnsafe(pResourceFile->GetName()) != pResourceFile->GetServerChecksum())
            {
                HandleDownloadedFileTrouble(pResourceFile, false);
            }
        }
    }

    // Set active flag
    m_bActive = true;
    m_bStarting = false;

    // Did we get a resource root entity?
    if (m_pResourceEntity)
    {
        // Call the Lua "onClientResourceStart" event
        CLuaArguments Arguments;
        Arguments.PushResource(this);
        m_pResourceEntity->CallEvent("onClientResourceStart", Arguments, true);

        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (pBitStream)
        {
            if (pBitStream->Can(eBitStreamVersion::OnPlayerResourceStart))
            {
                // Write resource net ID
                pBitStream->Write(GetNetID());

                g_pNet->SendPacket(PACKET_ID_PLAYER_RESOURCE_START, pBitStream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
            }

            g_pNet->DeallocateNetBitStream(pBitStream);
        }
    }
    else
        assert(0);
}

void CResource::Stop()
{
    m_bStarting = false;
    m_bStopping = true;
    CLuaArguments Arguments;
    Arguments.PushResource(this);
    m_pResourceEntity->CallEvent("onClientResourceStop", Arguments, true);

    // When a custom application is used - reset discord stuff
    const auto discord = g_pCore->GetDiscord();
    if (discord && !discord->IsDiscordCustomDetailsDisallowed() && discord->GetDiscordResourceName() == m_strResourceName)
    {
        if (discord->IsDiscordRPCEnabled())
        {
            discord->ResetDiscordData();
            discord->SetPresenceState(_("In-game"), false);
            discord->SetPresenceStartTimestamp(time(nullptr));
            discord->UpdatePresence();
        }
    }
}

SString CResource::GetState()
{
    if (m_bStarting)
        return "starting";
    else if (m_bStopping)
        return "stopping";
    else if (m_bActive)
        return "running";
    else
        return "loaded";
}

void CResource::DeleteClientChildren()
{
    // Run this on our resource entity if we have one
    if (m_pResourceEntity)
        m_pResourceEntity->DeleteClientChildren();
}

void CResource::ShowCursor(bool bShow, bool bToggleControls)
{
    // Different cursor showing state than earlier?
    if (bShow != m_bShowingCursor)
    {
        // Going to show the cursor?
        if (bShow)
        {
            // Increase the cursor ref count
            m_iShowingCursor += 1;
        }
        else
        {
            // Decrease the cursor ref count
            m_iShowingCursor -= 1;
        }

        // Update our showing cursor state
        m_bShowingCursor = bShow;

        // Show cursor if more than 0 resources wanting the cursor on
        g_pCore->ForceCursorVisible(m_iShowingCursor > 0, bToggleControls);
        g_pClientGame->SetCursorEventsEnabled(m_iShowingCursor > 0);
    }
}

SString CResource::GetResourceDirectoryPath(eAccessType accessType, const SString& strMetaPath)
{
    // See if private files should be moved to a new directory
    if (accessType == ACCESS_PRIVATE)
    {
        if (!m_strResourcePrivateDirectoryPathOld.empty())
        {
            SString strNewFilePath = PathJoin(m_strResourcePrivateDirectoryPath, strMetaPath);
            SString strOldFilePath = PathJoin(m_strResourcePrivateDirectoryPathOld, strMetaPath);

            if (FileExists(strOldFilePath))
            {
                if (FileExists(strNewFilePath))
                {
                    // If file exists in old and new, delete from old
                    OutputDebugLine(SString("Deleting %s", *strOldFilePath));
                    FileDelete(strOldFilePath);
                }
                else
                {
                    // If file exists in old only, move from old to new
                    OutputDebugLine(SString("Moving %s to %s", *strOldFilePath, *strNewFilePath));
                    MakeSureDirExists(strNewFilePath);
                    FileRename(strOldFilePath, strNewFilePath);
                }
            }
        }
        return PathJoin(m_strResourcePrivateDirectoryPath, strMetaPath);
    }
    return PathJoin(m_strResourceDirectoryPath, strMetaPath);
}

CResourceFile* CResource::GetResourceFile(const SString& relativePath) const
{
    for (CResourceFile* resourceFile : m_ResourceFiles)
    {
        if (!stricmp(relativePath.c_str(), resourceFile->GetShortName()))
        {
            return resourceFile;
        }
    }

    return nullptr;
}

void CResource::LoadNoClientCacheScript(const char* chunk, unsigned int len, const SString& strFilename)
{
    if (m_usRemainingNoClientCacheScripts > 0)
    {
        --m_usRemainingNoClientCacheScripts;

        // Store for later
        m_NoClientCacheScriptList.push_back(SNoClientCacheScript());
        SNoClientCacheScript& item = m_NoClientCacheScriptList.back();
        item.buffer = CBuffer(chunk, len);
        item.strFilename = strFilename;

        if (m_usRemainingNoClientCacheScripts == 0 && m_bLoadAfterReceivingNoClientCacheScripts)
        {
            m_bLoadAfterReceivingNoClientCacheScripts = false;
            Load();
        }
    }
}

//
// Add element to the default element group
//
void CResource::AddToElementGroup(CClientEntity* pElement)
{
    if (m_pDefaultElementGroup)
    {
        m_pDefaultElementGroup->Add(pElement);
    }
}

//
// Handle when things go wrong
//
void CResource::HandleDownloadedFileTrouble(CResourceFile* pResourceFile, bool bScript)
{
    auto checksumResult = CChecksum::GenerateChecksumFromFile(pResourceFile->GetName());

    SString errorMessage;
    if (std::holds_alternative<std::string>(checksumResult))
        errorMessage = std::get<std::string>(checksumResult);
    else
    {
        CChecksum checksum = std::get<CChecksum>(checksumResult);

        // Compose message
        uint    uiGotFileSize = (uint)FileSize(pResourceFile->GetName());
        SString strGotMd5 = ConvertDataToHexString(checksum.md5.data, sizeof(MD5));
        SString strWantedMd5 = ConvertDataToHexString(pResourceFile->GetServerChecksum().md5.data, sizeof(MD5));
        errorMessage = SString("Got size:%d MD5:%s, wanted MD5:%s", uiGotFileSize, *strGotMd5, *strWantedMd5);
    }

    SString strFilename = ExtractFilename(PathConform(pResourceFile->GetShortName()));
    SString strMessage = SString("HTTP server file mismatch! (%s) %s [%s]", GetName(), *strFilename, *errorMessage);

    // Log to the server & client console
    g_pClientGame->TellServerSomethingImportant(bScript ? 1002 : 1013, strMessage, 4);
    g_pCore->GetConsole()->Printf("Download error: %s", *strMessage);
}
