/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResource.h
 *  PURPOSE:     Resource object class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "lua/CLuaManager.h"
#include "CClientEntity.h"
#include "CResourceConfigItem.h"
#include "CResourceFile.h"
#include "CElementGroup.h"
#include <list>

#define MAX_RESOURCE_NAME_LENGTH    255
#define MAX_FUNCTION_NAME_LENGTH    50

struct SNoClientCacheScript
{
    CBuffer buffer;
    SString strFilename;
};

struct SPendingFileDownload
{
    SString strUrl;
    SString strFilename;
    double  dDownloadSize;
};

class CResource
{
public:
    CResource(unsigned short usNetID, const char* szResourceName, CClientEntity* pResourceEntity, CClientEntity* pResourceDynamicEntity,
              const CMtaVersion& strMinServerReq, const CMtaVersion& strMinClientReq, bool bEnableOOP, ELuaVersion luaVersion);
    ~CResource();

    unsigned short GetNetID() { return m_usNetID; };
    uint           GetScriptID() const { return m_uiScriptID; };
    const char*    GetName() { return m_strResourceName; };
    CLuaMain*      GetVM() { return m_pLuaVM; };
    bool           IsActive() { return m_bActive; };
    bool           CanBeLoaded();

    void    Load();
    void    Stop();
    SString GetState();

    CDownloadableResource* AddResourceFile(CDownloadableResource::eResourceType resourceType, const char* szFileName, uint uiDownloadSize,
                                           CChecksum serverChecksum, bool bAutoDownload);
    CDownloadableResource* AddConfigFile(const char* szFileName, uint uiDownloadSize, CChecksum serverChecksum);

    std::list<class CResourceConfigItem*>::iterator ConfigIterBegin() { return m_ConfigFiles.begin(); }
    std::list<class CResourceConfigItem*>::iterator ConfigIterEnd() { return m_ConfigFiles.end(); }

    CElementGroup* GetElementGroup() { return m_pDefaultElementGroup; }
    void           AddToElementGroup(CClientEntity* pElement);

    void AddExportedFunction(const SString& name) { m_exportedFunctions.insert(name); }
    bool CallExportedFunction(const SString& name, CLuaArguments& args, CLuaArguments& returns, CResource& caller);

    class CClientEntity* GetResourceEntity() { return m_pResourceEntity; }
    void                 SetResourceEntity(CClientEntity* pEntity) { m_pResourceEntity = pEntity; }
    class CClientEntity* GetResourceDynamicEntity() { return m_pResourceDynamicEntity; }
    void                 SetResourceDynamicEntity(CClientEntity* pEntity) { m_pResourceDynamicEntity = pEntity; }
    SString              GetResourceDirectoryPath(eAccessType accessType, const SString& strMetaPath);
    class CClientEntity* GetResourceGUIEntity() { return m_pResourceGUIEntity; }
    void                 SetResourceGUIEntity(CClientEntity* pEntity) { m_pResourceGUIEntity = pEntity; }
    CClientEntity*       GetResourceCOLModelRoot() { return m_pResourceCOLRoot; };
    CClientEntity*       GetResourceDFFRoot() { return m_pResourceDFFEntity; };
    CClientEntity*       GetResourceTXDRoot() { return m_pResourceTXDRoot; };
    CClientEntity*       GetResourceIFPRoot() { return m_pResourceIFPRoot; };
    CClientEntity*       GetResourceIMGRoot() { return m_pResourceIMGRoot; };

    // This is to delete all the elements created in this resource that are created locally in this client
    void DeleteClientChildren();

    // Use this for cursor showing/hiding
    void ShowCursor(bool bShow, bool bToggleControls = true);

    const auto& GetExportedFunctions() const noexcept { return m_exportedFunctions; }

    std::list<CResourceFile*>::iterator IterBeginResourceFiles() { return m_ResourceFiles.begin(); }
    std::list<CResourceFile*>::iterator IterEndResourceFiles() { return m_ResourceFiles.end(); }

    /**
     * @brief Searches for a CResourceFile with the given relative path.
     * @param relativePath Relative resource file path (from meta)
     * @return A pointer to CResourceFile on success, null otherwise
     */
    CResourceFile* GetResourceFile(const SString& relativePath) const;

    void               SetRemainingNoClientCacheScripts(unsigned short usRemaining) { m_usRemainingNoClientCacheScripts = usRemaining; }
    void               LoadNoClientCacheScript(const char* chunk, unsigned int length, const SString& strFilename);
    const CMtaVersion& GetMinServerReq() const { return m_strMinServerReq; }
    const CMtaVersion& GetMinClientReq() const { return m_strMinClientReq; }
    bool               IsOOPEnabled() { return m_bOOPEnabled; }
    void               HandleDownloadedFileTrouble(CResourceFile* pResourceFile, bool bScript);
    bool               IsWaitingForInitialDownloads();
    int                GetDownloadPriorityGroup() { return m_iDownloadPriorityGroup; }
    void               SetDownloadPriorityGroup(int iDownloadPriorityGroup) { m_iDownloadPriorityGroup = iDownloadPriorityGroup; }

private:
    unsigned short       m_usNetID;
    uint                 m_uiScriptID;
    SString              m_strResourceName;
    CLuaMain*            m_pLuaVM;
    CLuaManager*         m_pLuaManager;
    class CClientEntity* m_pRootEntity;
    bool                 m_bActive;
    bool                 m_bStarting;
    bool                 m_bStopping;
    class CClientEntity* m_pResourceEntity;                   // no idea what this is used for anymore
    class CClientEntity* m_pResourceDynamicEntity;            // parent of elements created by the resource
    class CClientEntity* m_pResourceCOLRoot;
    class CClientEntity* m_pResourceDFFEntity;
    class CClientEntity* m_pResourceGUIEntity;
    class CClientEntity* m_pResourceTXDRoot;
    class CClientEntity* m_pResourceIFPRoot;
    class CClientEntity* m_pResourceIMGRoot;
    unsigned short       m_usRemainingNoClientCacheScripts;
    bool                 m_bLoadAfterReceivingNoClientCacheScripts;
    CMtaVersion          m_strMinServerReq;
    CMtaVersion          m_strMinClientReq;
    bool                 m_bOOPEnabled;
    int                  m_iDownloadPriorityGroup;

    ELuaVersion m_LuaVersion = ELuaVersion::VLUA_5_1;

    // To control cursor show/hide
    static int m_iShowingCursor;
    bool       m_bShowingCursor;

    SString m_strResourceDirectoryPath;                      // stores the path to /mods/deathmatch/resources/resource_name
    SString m_strResourcePrivateDirectoryPath;               // stores the path to /mods/deathmatch/priv/server-id/resource_name
    SString m_strResourcePrivateDirectoryPathOld;            // stores the path to /mods/deathmatch/priv/old-server-id/resource_name

    std::list<class CResourceFile*>       m_ResourceFiles;
    std::list<class CResourceConfigItem*> m_ConfigFiles;
    CFastHashSet<SString>                 m_exportedFunctions;
    CElementGroup*                        m_pDefaultElementGroup;            // stores elements created by scripts in this resource
    std::list<SNoClientCacheScript>       m_NoClientCacheScriptList;
};
