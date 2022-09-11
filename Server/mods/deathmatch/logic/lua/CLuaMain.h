/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaMain.h
 *  PURPOSE:     Lua virtual machine container class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaMain;

#pragma once
#include "CLuaTimerManager.h"
#include "lua/CLuaVector2.h"
#include "lua/CLuaVector3.h"
#include "lua/CLuaVector4.h"
#include "lua/CLuaMatrix.h"
#include "CLuaModuleManager.h"
#include "../CTextDisplay.h"
#include "ResourceTask.h"
#include <utility>

#define MAX_SCRIPTNAME_LENGTH 64

class CBlipManager;
class CObjectManager;
class CPlayerManager;
class CRadarAreaManager;
class CVehicleManager;
class CMapManager;

struct CRefInfo
{
    unsigned long int ulUseCount;
    int               iFunction;
};

class CLuaMain            //: public CClient
{
public:
    ZERO_ON_NEW
    CLuaMain(class CLuaManager* pLuaManager, CObjectManager* pObjectManager, CPlayerManager* pPlayerManager, CVehicleManager* pVehicleManager,
             CBlipManager* pBlipManager, CRadarAreaManager* pRadarAreaManager, CMapManager* pMapManager, CResource* pResourceOwner, bool bEnableOOP);

    ~CLuaMain();

    bool LoadScriptFromBuffer(const char* cpBuffer, unsigned int uiSize, const char* szFileName);
    bool LoadScript(const char* szLUAScript);
    void UnloadScript();

    void Start();

    void DoPulse();

    const char* GetScriptName() const { return m_strScriptName; }
    void        SetScriptName(const char* szName) { m_strScriptName.AssignLeft(szName, MAX_SCRIPTNAME_LENGTH); }

    lua_State*        GetVM() { return m_luaVM; };
    CLuaTimerManager* GetTimerManager() const { return m_pLuaTimerManager; };

    CBlipManager*    GetBlipManager() const { return m_pBlipManager; };
    CObjectManager*  GetObjectManager() const { return m_pObjectManager; };
    CPlayerManager*  GetPlayerManager() const { return m_pPlayerManager; };
    CVehicleManager* GetVehicleManager() const { return m_pVehicleManager; };
    CMapManager*     GetMapManager() const { return m_pMapManager; };

    CXMLFile*     CreateXML(const char* szFilename, bool bUseIDs = true, bool bReadOnly = false);
    CXMLNode*     ParseString(const char* strXmlContent);
    bool          DestroyXML(CXMLFile* pFile);
    bool          DestroyXML(CXMLNode* pRootNode);
    bool          SaveXML(CXMLNode* pRootNode);
    bool          XMLExists(CXMLFile* pFile);
    unsigned long GetXMLFileCount() const { return m_XMLFiles.size(); };
    unsigned long GetOpenFileCount() const { return m_OpenFilenameList.size(); };
    unsigned long GetTimerCount() const { return m_pLuaTimerManager ? m_pLuaTimerManager->GetTimerCount() : 0; };
    unsigned long GetElementCount() const;
    unsigned long GetTextDisplayCount() const { return m_Displays.size(); };
    unsigned long GetTextItemCount() const { return m_TextItems.size(); };
    void          OnOpenFile(const SString& strFilename);
    void          OnCloseFile(const SString& strFilename);

    void          OnTaskFinishedOrCancelled(BaseResourceTask& task);
    size_t        CLuaMain::GetNumOfActiveTasks();

    //
    // Pushes an async task.
    // These tasks have the same life-time as the resoure that created them.
    // This means that the ready callback function won't ever be called after the resource has stopped/restarted!
    // (Although the task function might be running when the resource was stopped)
    //
    template<typename... Args>
    auto PushTask(Args&&... args) {
        auto task = new ResourceTask{ *this, std::forward<Args>(args)... };
        task->AddToList(m_ActiveTasksHead);
        m_ActiveTasksHead = task;
        return CLuaShared::GetAsyncTaskScheduler()->PushTask(std::unique_ptr<struct CAsyncTaskScheduler::SBaseTask>(task));
    }

    CTextDisplay* CreateDisplay();
    void          DestroyDisplay(CTextDisplay* pDisplay);
    CTextItem*    CreateTextItem(const char* szText, float fX, float fY, eTextPriority priority = PRIORITY_LOW, const SColor color = -1, float fScale = 1.0f,
                                 unsigned char format = 0, unsigned char ucShadowAlpha = 0);
    void          DestroyTextItem(CTextItem* pTextItem);

    CTextDisplay* GetTextDisplayFromScriptID(uint uiScriptID);
    CTextItem*    GetTextItemFromScriptID(uint uiScriptID);

    bool       BeingDeleted();
    lua_State* GetVirtualMachine() const { return m_luaVM; };

    void ResetInstructionCount();

    CResource* GetResource() { return m_pResource; }

    void           SetResourceFile(class CResourceFile* resourceFile) { m_pResourceFile = resourceFile; }
    CResourceFile* GetResourceFile() { return m_pResourceFile; }

    void RegisterHTMLDFunctions();

    void           Initialize();
    void           LoadEmbeddedScripts();
    void           RegisterModuleFunctions();
    const SString& GetFunctionTag(int iFunctionNumber);
    int            PCall(lua_State* L, int nargs, int nresults, int errfunc);
    void           CheckExecutionTime();
    static int     LuaLoadBuffer(lua_State* L, const char* buff, size_t sz, const char* name);
    static int     OnUndump(const char* p, size_t n);

private:
    void InitSecurity();
    void InitClasses(lua_State* luaVM);

public:
    bool IsOOPEnabled() { return m_bEnableOOP; }

private:
    static void InstructionCountHook(lua_State* luaVM, lua_Debug* pDebug);

    SString m_strScriptName;

    lua_State*        m_luaVM;
    CLuaTimerManager* m_pLuaTimerManager;

    class CResource*     m_pResource;
    class CResourceFile* m_pResourceFile;
    CBlipManager*        m_pBlipManager;
    CObjectManager*      m_pObjectManager;
    CPlayerManager*      m_pPlayerManager;
    CRadarAreaManager*   m_pRadarAreaManager;
    CVehicleManager*     m_pVehicleManager;
    CMapManager*         m_pMapManager;

    // List head for active tasks - that is, all tasks not cancelled
    // Tasks cancelled or finished are removed by a call to `OnTaskFinishedOrCancelled`
    BaseResourceTask*    m_ActiveTasksHead{};

    list<CXMLFile*>                                 m_XMLFiles;
    std::unordered_set<std::unique_ptr<SXMLString>> m_XMLStringNodes;
    list<CTextDisplay*>                             m_Displays;
    list<CTextItem*>                                m_TextItems;

    bool m_bEnableOOP;

    bool m_bBeingDeleted;            // prevent it being deleted twice

    CElapsedTime         m_FunctionEnterTimer;
    CElapsedTimeApprox   m_WarningTimer;
    uint                 m_uiPCallDepth;
    std::vector<SString> m_OpenFilenameList;
    uint                 m_uiOpenFileCountWarnThresh;
    uint                 m_uiOpenXMLFileCountWarnThresh;
    static SString       ms_strExpectedUndumpHash;

public:
    CFastHashMap<const void*, CRefInfo> m_CallbackTable;
    std::map<int, SString>              m_FunctionTagMap;
};
