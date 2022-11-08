/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaMain.h
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

#include "CLuaFunctionDefs.h"

#include <xml/CXMLFile.h>

#define MAX_SCRIPTNAME_LENGTH 64

#include <list>

struct CRefInfo
{
    unsigned long int ulUseCount;
    int               iFunction;
};

class CLuaMain            //: public CClient
{
public:
    ZERO_ON_NEW
    CLuaMain(class CLuaManager* pLuaManager, CResource* pResourceOwner, bool bEnableOOP, ELuaVersion luaVersion);
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

    bool       BeingDeleted();
    lua_State* GetVirtualMachine() const { return m_luaVM; };

    void ResetInstructionCount();

    class CResource* GetResource() { return m_pResource; }

    CXMLFile*     CreateXML(const char* szFilename, bool bUseIDs = true, bool bReadOnly = false);
    CXMLNode*     ParseString(const char* strXmlContent);
    bool          DestroyXML(CXMLFile* pFile);
    bool          DestroyXML(CXMLNode* pRootNode);
    bool          SaveXML(CXMLNode* pRootNode);
    unsigned long GetXMLFileCount() const { return m_XMLFiles.size(); };
    unsigned long GetTimerCount() const { return m_pLuaTimerManager ? m_pLuaTimerManager->GetTimerCount() : 0; };
    unsigned long GetElementCount() const;

    void           InitClasses(lua_State* luaVM);
    void           InitVM();
    void           LoadEmbeddedScripts();
    const SString& GetFunctionTag(int iLuaFunction);
    int            PCall(lua_State* L, int nargs, int nresults, int errfunc);
    static int     LuaLoadBuffer(lua_State* L, const char* buff, size_t sz, const char* name);
    static int     OnUndump(const char* p, size_t n);

    bool IsOOPEnabled() { return m_bEnableOOP; }

private:
    void InitSecurity();

    static void InstructionCountHook(lua_State* luaVM, lua_Debug* pDebug);

    SString m_strScriptName;

    lua_State*        m_luaVM;
    CLuaTimerManager* m_pLuaTimerManager;

    bool m_bBeingDeleted;            // prevent it being deleted twice

    CElapsedTime m_FunctionEnterTimer;

    class CResource* m_pResource;

    std::list<CXMLFile*>                            m_XMLFiles;
    std::unordered_set<std::unique_ptr<SXMLString>> m_XMLStringNodes;
    static SString                                  ms_strExpectedUndumpHash;

    bool m_bEnableOOP;
    ELuaVersion m_LuaVersion = ELuaVersion::VLUA_5_1;

public:
    CFastHashMap<const void*, CRefInfo> m_CallbackTable;
    std::map<int, SString>              m_FunctionTagMap;
};
