/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceManager.h
 *  PURPOSE:     Resource manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class controls all the resources that are loaded, and loads
// new resources on demand

#pragma once

#include "CResource.h"
#include "CElement.h"
#include "ehs/ehs.h"
#include <list>

class CResource;
#define INVALID_RESOURCE_NET_ID     0xFFFF

class CResourceManager
{
public:
    enum eResourceQueue
    {
        QUEUE_STOP,
        QUEUE_STOPALL,
        QUEUE_RESTART,
        QUEUE_RESTART2,
        QUEUE_REFRESH,
        QUEUE_REFRESHALL,
    };

private:
    struct sResourceQueue
    {
        CResource*            pResource;
        eResourceQueue        eQueue;
        SResourceStartOptions StartOptions;
        vector<SString>       dependents;
    };

public:
    ZERO_ON_NEW
    CResourceManager();
    ~CResourceManager();

    CResource*                            Load(bool bIsZipped, const char* szAbsPath, const char* szResourceName);
    void                                  UnloadAndDelete(CResource* pResource);
    CResource*                            GetResource(const char* szResourceName);
    CResource*                            GetResourceFromScriptID(uint uiScriptID);
    void                                  UnloadRemovedResources();
    void                                  CheckResourceDependencies();
    void                                  ListResourcesLoaded(const SString& strListType);
    std::list<CResource*>::const_iterator IterBegin() { return m_resources.begin(); };
    std::list<CResource*>::const_iterator IterEnd() { return m_resources.end(); };

    bool         Refresh(bool bRefreshAll = false, const SString strJustThisResource = "", bool bShowTiming = false);
    void         UpgradeResources(CResource* pResource = NULL);
    void         CheckResources(CResource* pResource = NULL);
    void         OnResourceLoadStateChange(CResource* pResource, const char* szOldState, const char* szNewState) const;
    unsigned int GetResourceLoadedCount() { return m_uiResourceLoadedCount; }
    unsigned int GetResourceFailedCount() { return m_uiResourceFailedCount; }
    void         OnPlayerJoin(CPlayer& Player);

    const char* GetResourceDirectory();

    bool StartResource(CResource* pResource, std::list<CResource*>* pDependents = nullptr, bool bManualStart = false,
                       const SResourceStartOptions& StartOptions = SResourceStartOptions());
    bool Reload(CResource* pResource);
    bool StopAllResources();

    void QueueResource(CResource* pResource, eResourceQueue eQueueType, const SResourceStartOptions* pStartOptions,
                       std::list<CResource*>* pDependents = nullptr);
    void ProcessQueue();
    void RemoveFromQueue(CResource* pResource);

    bool IsAResourceElement(CElement* pElement);

    unsigned short GenerateID();
    CResource*     GetResourceFromNetID(unsigned short usNetID);

    CResource* GetResourceFromLuaState(struct lua_State* luaVM);
    SString    GetResourceName(struct lua_State* luaVM);

    CResource* CreateResource(const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus);
    CResource* CopyResource(CResource* pSourceResource, const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus);
    CResource* RenameResource(CResource* pSourceResource, const SString& strNewResourceName, const SString& strNewOrganizationalPath, SString& strOutStatus);
    bool       DeleteResource(const SString& strResourceName, SString& strOutStatus);

    SString GetResourceTrashDir();
    bool    MoveDirToTrash(const SString& strPathDirName);
    SString GetResourceOrganizationalPath(CResource* pResource);

    static bool ParseResourcePathInput(std::string strInput, CResource*& pResource, std::string* pStrPath, std::string* pStrMetaPath = nullptr);

    void NotifyResourceVMOpen(CResource* pResource, CLuaMain* pVM);
    void NotifyResourceVMClose(CResource* pResource, CLuaMain* pVM);

    void AddResourceToLists(CResource* pResource);
    void RemoveResourceFromLists(CResource* pResource);

    void    ApplyMinClientRequirement(CResource* pResource, const CMtaVersion& strMinClientRequirement);
    void    RemoveMinClientRequirement(CResource* pResource);
    void    ReevaluateMinClientRequirement();
    CMtaVersion GetMinClientRequirement() { return m_strMinClientRequirement; }

    void ApplySyncMapElementDataOption(CResource* pResource, bool bSyncMapElementData);
    void RemoveSyncMapElementDataOption(CResource* pResource);
    void ReevaluateSyncMapElementDataOption();

    void    LoadBlockedFileReasons();
    void    SaveBlockedFileReasons();
    void    ClearBlockedFileReason(const SString& strFileHash);
    void    AddBlockedFileReason(const SString& strFileHash, const SString& strReason);
    SString GetBlockedFileReason(const SString& strFileHash);

private:
    SString                 m_strResourceDirectory;
    CMappedList<CResource*> m_resources;
    unsigned int            m_uiResourceLoadedCount;
    unsigned int            m_uiResourceFailedCount;
    bool                    m_bResourceListChanged;
    list<CResource*>        m_resourcesToStartAfterRefresh;

    // Maps to speed things up
    CFastHashMap<CResource*, lua_State*> m_ResourceLuaStateMap;
    CFastHashMap<lua_State*, CResource*> m_LuaStateResourceMap;
    CFastHashMap<SString, CResource*>    m_NameResourceMap;
    std::map<ushort, CResource*>         m_NetIdResourceMap;

    list<sResourceQueue> m_resourceQueue;

    CMtaVersion                           m_strMinClientRequirement;
    CFastHashMap<CResource*, CMtaVersion> m_MinClientRequirementMap;
    CFastHashMap<CResource*, bool>        m_SyncMapElementDataOptionMap;

    ushort                     m_usNextNetId;
    std::map<SString, SString> m_BlockedFileReasonMap;
};
