/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceManager.cpp
 *  PURPOSE:     Resource manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CResourceManager::CResourceManager()
{
}

CResourceManager::~CResourceManager()
{
    while (!m_resources.empty())
    {
        CResource* pResource = m_resources.back();
        Remove(pResource);

        // Force delete all objects in cache (see https://github.com/multitheftauto/mtasa-blue/issues/1840).
        g_pClientGame->GetElementDeleter()->DoDeleteAll();
    }
}

CResource* CResourceManager::Add(unsigned short usNetID, const char* szResourceName, CClientEntity* pResourceEntity, CClientEntity* pResourceDynamicEntity,
                                 const CMtaVersion& strMinServerReq, const CMtaVersion& strMinClientReq, bool bEnableOOP, ELuaVersion luaVersion)
{
    CResource* pResource = new CResource(usNetID, szResourceName, pResourceEntity, pResourceDynamicEntity, strMinServerReq, strMinClientReq, bEnableOOP, luaVersion);
    if (pResource)
    {
        m_resources.push_back(pResource);
        assert(!MapContains(m_NetIdResourceMap, pResource->GetNetID()));
        MapSet(m_NetIdResourceMap, usNetID, pResource);
        return pResource;
    }
    return NULL;
}

CResource* CResourceManager::GetResourceFromNetID(unsigned short usNetID)
{
    CResource* pResource = MapFindRef(m_NetIdResourceMap, usNetID);
    if (pResource)
    {
        assert(pResource->GetNetID() == usNetID);
        return pResource;
    }

    list<CResource*>::const_iterator iter = m_resources.begin();
    for (; iter != m_resources.end(); ++iter)
    {
        if ((*iter)->GetNetID() == usNetID)
        {
            assert(0);            // Should be in map
            return (*iter);
        }
    }
    return NULL;
}

CResource* CResourceManager::GetResourceFromScriptID(uint uiScriptID)
{
    CResource* pResource = (CResource*)CIdArray::FindEntry(uiScriptID, EIdClass::RESOURCE);
    dassert(!pResource || ListContains(m_resources, pResource));
    return pResource;
}

CResource* CResourceManager::GetResourceFromLuaState(lua_State* luaVM)
{
    CLuaMain*  pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
    CResource* pResource = pLuaMain ? pLuaMain->GetResource() : NULL;
    return pResource;
}

SString CResourceManager::GetResourceName(lua_State* luaVM)
{
    CResource* pResource = GetResourceFromLuaState(luaVM);
    if (pResource)
        return pResource->GetName();
    return "";
}

CResource* CResourceManager::GetResource(const char* szResourceName)
{
    list<CResource*>::const_iterator iter = m_resources.begin();
    for (; iter != m_resources.end(); ++iter)
    {
        if (stricmp((*iter)->GetName(), szResourceName) == 0)
            return (*iter);
    }
    return NULL;
}

void CResourceManager::OnDownloadGroupFinished()
{
    // Try to load newly ready resources
    for (std::list<CResource*>::const_iterator iter = m_resources.begin(); iter != m_resources.end(); ++iter)
    {
        CResource* pResource = *iter;
        if (!pResource->IsActive())
        {
            // Stop as soon as we hit a resource which hasn't downloaded yet (as per previous behaviour)
            if (pResource->IsWaitingForInitialDownloads())
                break;
            pResource->Load();
        }
    }
}

bool CResourceManager::RemoveResource(unsigned short usNetID)
{
    CResource* pResource = GetResourceFromNetID(usNetID);
    if (pResource)
    {
        Remove(pResource);
        return true;
    }
    return false;
}

void CResourceManager::Remove(CResource* pResource)
{
    // Triggger the onStop event, and set resource state to 'stopping'
    pResource->Stop();

    // Delete the resource
    m_resources.remove(pResource);
    assert(MapContains(m_NetIdResourceMap, pResource->GetNetID()));
    MapRemove(m_NetIdResourceMap, pResource->GetNetID());
    delete pResource;
}

bool CResourceManager::Exists(CResource* pResource)
{
    return m_resources.Contains(pResource);
}

void CResourceManager::StopAll()
{
    while (m_resources.size() > 0)
    {
        Remove(m_resources.front());
    }
}

// pResource may be changed on return, and it could be NULL if the function returns false.
bool CResourceManager::ParseResourcePathInput(std::string strInput, CResource*& pResource, std::string* pStrPath, std::string* pStrMetaPath)
{
    ReplaceOccurrencesInString(strInput, "\\", "/");

    // Disallow file paths with a directory separator at the end
    if (strInput.back() == '/')
        return false;

    eAccessType accessType = ACCESS_PUBLIC;
    std::string strMetaPath;

    if (strInput[0] == '@')
    {
        accessType = ACCESS_PRIVATE;
        strInput = strInput.substr(1);
    }

    if (strInput[0] == ':')
    {
        unsigned int iEnd = strInput.find_first_of("/");
        if (iEnd)
        {
            std::string strResourceName = strInput.substr(1, iEnd - 1);
            pResource = g_pClientGame->GetResourceManager()->GetResource(strResourceName.c_str());
            if (pResource && strInput[iEnd + 1])
            {
                strMetaPath = strInput.substr(iEnd + 1);

                if (pStrMetaPath)
                    *pStrMetaPath = strMetaPath;

                if (IsValidFilePath(strMetaPath.c_str()))
                {
                    if (pStrPath)
                        *pStrPath = pResource->GetResourceDirectoryPath(accessType, strMetaPath);
                    return true;
                }
            }
        }
    }
    else if (pResource && IsValidFilePath(strInput.c_str()))
    {
        if (pStrPath)
            *pStrPath = pResource->GetResourceDirectoryPath(accessType, strInput);
        if (pStrMetaPath)
            *pStrMetaPath = strInput;
        return true;
    }
    return false;
}

// When a resource file is created
void CResourceManager::OnAddResourceFile(CDownloadableResource* pResourceFile)
{
    SString strFilename = PathConform(pResourceFile->GetName()).ToLower();
    dassert(!MapContains(m_ResourceFileMap, strFilename));
    MapSet(m_ResourceFileMap, strFilename, pResourceFile);
}

// When a resource file is delected
void CResourceManager::OnRemoveResourceFile(CDownloadableResource* pResourceFile)
{
    SString strFilename = PathConform(pResourceFile->GetName()).ToLower();
    dassert(MapFindRef(m_ResourceFileMap, strFilename) == pResourceFile);
    MapRemove(m_ResourceFileMap, strFilename);
}

// Update downloaded flag for this file
void CResourceManager::OnDownloadedResourceFile(const SString& strInFilename)
{
    SString                strFilename = PathConform(strInFilename).ToLower();
    CDownloadableResource* pResourceFile = MapFindRef(m_ResourceFileMap, strFilename);
    if (pResourceFile)
        pResourceFile->SetDownloaded();
}

// Check given file name is a resource file
bool CResourceManager::IsResourceFile(const SString& strInFilename)
{
    SString strFilename = PathConform(strInFilename).ToLower();
    return MapContains(m_ResourceFileMap, strFilename);
}

// Remove this file from the checks as it has been changed by script actions
void CResourceManager::OnFileModifedByScript(const SString& strInFilename, const SString& strReason)
{
    SString                strFilename = PathConform(strInFilename).ToLower();
    CDownloadableResource* pResourceFile = MapFindRef(m_ResourceFileMap, strFilename);
    if (pResourceFile && !pResourceFile->IsModifedByScript())
    {
        pResourceFile->SetModifedByScript(true);
        SString strMessage("Resource file modifed by script (%s): %s ", *strReason, *ConformResourcePath(strInFilename));
        AddReportLog(7059, strMessage + g_pNet->GetConnectedServer(true), 10);
    }
}

// Check resource file data matches server checksum
void CResourceManager::ValidateResourceFile(const SString& strInFilename, const char* buffer, size_t bufferSize)
{
    SString                strFilename = PathConform(strInFilename).ToLower();
    CDownloadableResource* pResourceFile = MapFindRef(m_ResourceFileMap, strFilename);
    if (pResourceFile && !pResourceFile->IsModifedByScript())
    {
        if (!pResourceFile->IsAutoDownload() && !pResourceFile->IsDownloaded())
        {
            // Scripting error
            g_pClientGame->GetScriptDebugging()->LogWarning(NULL, "Attempt to load '%s' before onClientFileDownloadComplete event",
                                                            *ConformResourcePath(strInFilename));
        }
        else
        {
            CChecksum checksum;
            if (buffer)
                checksum = CChecksum::GenerateChecksumFromBuffer(buffer, bufferSize);
            else
                checksum = CChecksum::GenerateChecksumFromFileUnsafe(strInFilename);
            if (checksum != pResourceFile->GetServerChecksum())
            {
                char szMd5[33];
                CMD5Hasher::ConvertToHex(checksum.md5, szMd5);
                char szMd5Wanted[33];
                CMD5Hasher::ConvertToHex(pResourceFile->GetServerChecksum().md5, szMd5Wanted);
                SString strMessage("%s [Expected Size:%d MD5:%s][Got Size:%d MD5:%s] ", *ConformResourcePath(strInFilename), pResourceFile->GetDownloadSize(),
                                   szMd5Wanted, (int)FileSize(strInFilename), szMd5);
                if (pResourceFile->IsDownloaded())
                {
                    strMessage = "Resource file unexpected change: " + strMessage;
                    g_pClientGame->TellServerSomethingImportant(1007, strMessage);
                    g_pClientGame->GetScriptDebugging()->LogWarning(NULL, strMessage);
                }
                else if (pResourceFile->IsAutoDownload())
                {
                    strMessage = "Attempt to load resource file before it is ready: " + strMessage;
                    g_pClientGame->TellServerSomethingImportant(1008, strMessage);
                }
            }
        }
    }
}
