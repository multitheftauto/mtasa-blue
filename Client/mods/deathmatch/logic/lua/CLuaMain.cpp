/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaMain.cpp
 *  PURPOSE:     Lua main
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CLuaMain
#include "profiler/SharedUtil.Profiler.h"

using std::list;

extern CClientGame* g_pClientGame;

static CLuaManager* m_pLuaManager;
SString             CLuaMain::ms_strExpectedUndumpHash;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000

#include "luascripts/coroutine_debug.lua.h"
#include "luascripts/exports.lua.h"
#include "luascripts/inspect.lua.h"

CLuaMain::CLuaMain(CLuaManager* pLuaManager, CResource* pResourceOwner, bool bEnableOOP, ELuaVersion luaVersion) :
    m_LuaVersion(luaVersion)
{
    // Initialise everything to be setup in the Start function
    m_pLuaManager = pLuaManager;
    m_luaVM = NULL;
    m_bBeingDeleted = false;
    m_pLuaTimerManager = new CLuaTimerManager;
    m_FunctionEnterTimer.SetMaxIncrement(500);

    m_pResource = pResourceOwner;

    m_bEnableOOP = bEnableOOP;

    CClientPerfStatLuaMemory::GetSingleton()->OnLuaMainCreate(this);
    CClientPerfStatLuaTiming::GetSingleton()->OnLuaMainCreate(this);
}

CLuaMain::~CLuaMain()
{
    g_pClientGame->GetRemoteCalls()->OnLuaMainDestroy(this);
    g_pClientGame->GetLatentTransferManager()->OnLuaMainDestroy(this);
    g_pClientGame->GetDebugHookManager()->OnLuaMainDestroy(this);
    g_pClientGame->GetScriptDebugging()->OnLuaMainDestroy(this);

    // Unload the current script
    UnloadScript();
    m_bBeingDeleted = true;

    // Delete the timer manager
    delete m_pLuaTimerManager;

    CClientPerfStatLuaMemory::GetSingleton()->OnLuaMainDestroy(this);
    CClientPerfStatLuaTiming::GetSingleton()->OnLuaMainDestroy(this);
}

bool CLuaMain::BeingDeleted()
{
    return m_bBeingDeleted;
}

void CLuaMain::ResetInstructionCount()
{
    m_FunctionEnterTimer.Reset();
}

void CLuaMain::InitSecurity()
{
    // Disable dangerous Lua Os library functions
    static const luaL_reg osfuncs[] = {{"execute", CLuaUtilDefs::DisabledFunction},   {"rename", CLuaUtilDefs::DisabledFunction},
                                       {"remove", CLuaUtilDefs::DisabledFunction},    {"exit", CLuaUtilDefs::DisabledFunction},
                                       {"getenv", CLuaUtilDefs::DisabledFunction},    {"tmpname", CLuaUtilDefs::DisabledFunction},
                                       {"setlocale", CLuaUtilDefs::DisabledFunction}, {NULL, NULL}};
    luaL_register(m_luaVM, "os", osfuncs);

    lua_register(m_luaVM, "dofile", CLuaUtilDefs::DisabledFunction);
    lua_register(m_luaVM, "loadfile", CLuaUtilDefs::DisabledFunction);
    lua_register(m_luaVM, "require", CLuaUtilDefs::DisabledFunction);
    lua_register(m_luaVM, "loadlib", CLuaUtilDefs::DisabledFunction);
    lua_register(m_luaVM, "getfenv", CLuaUtilDefs::DisabledFunction);
    lua_register(m_luaVM, "newproxy", CLuaUtilDefs::DisabledFunction);
}

void CLuaMain::InitClasses(lua_State* luaVM)
{
    lua_initclasses(luaVM);

    CLuaVector4Defs::AddClass(luaVM);
    CLuaVector3Defs::AddClass(luaVM);
    CLuaVector2Defs::AddClass(luaVM);
    CLuaMatrixDefs ::AddClass(luaVM);

    if (!m_bEnableOOP)
        return;

    CLuaElementDefs::AddClass(luaVM);

    CLuaAudioDefs::AddClass(luaVM);
    CLuaBlipDefs::AddClass(luaVM);
    CLuaCameraDefs::AddClass(luaVM);
    CLuaColShapeDefs::AddClass(luaVM);
    CLuaDrawingDefs::AddClass(luaVM);
    CLuaEngineDefs::AddClass(luaVM);
    CLuaEffectDefs::AddClass(luaVM);
    CLuaGUIDefs::AddClass(luaVM);
    CLuaBrowserDefs::AddClass(luaVM);            // browser must be after drawing/gui, since it extends DxTexture/GUIElement
    CLuaMarkerDefs::AddClass(luaVM);
    CLuaObjectDefs::AddClass(luaVM);
    CLuaPedDefs::AddClass(luaVM);
    CLuaPickupDefs::AddClass(luaVM);
    CLuaPlayerDefs::AddClass(luaVM);
    CLuaPointLightDefs::AddClass(luaVM);
    CLuaProjectileDefs::AddClass(luaVM);
    CLuaRadarAreaDefs::AddClass(luaVM);
    CLuaResourceDefs::AddClass(luaVM);
    CLuaSearchLightDefs::AddClass(luaVM);
    CLuaTeamDefs::AddClass(luaVM);
    CLuaTimerDefs::AddClass(luaVM);
    CLuaVehicleDefs::AddClass(luaVM);
    CLuaWaterDefs::AddClass(luaVM);
    CLuaWeaponDefs::AddClass(luaVM);

    CLuaShared::AddClasses(luaVM);
}

void CLuaMain::InitVM()
{
    assert(!m_luaVM);

    // Create a new VM
    m_luaVM = VluaL_newstate(this, m_LuaVersion);
    m_pLuaManager->OnLuaMainOpenVM(this, m_luaVM);

    // Set the instruction count hook
    lua_sethook(m_luaVM, InstructionCountHook, LUA_MASKCOUNT, HOOK_INSTRUCTION_COUNT);

    // Load LUA libraries
    luaopen_base(m_luaVM);
    luaopen_math(m_luaVM);
    luaopen_string(m_luaVM);
    luaopen_table(m_luaVM);
    luaopen_debug(m_luaVM);
    luaopen_utf8(m_luaVM);
    luaopen_os(m_luaVM);

    // Initialize security restrictions. Very important to prevent lua trojans and viruses!
    InitSecurity();

    // Register module functions
    CLuaCFunctions::RegisterFunctionsWithVM(m_luaVM);

    // Create class metatables
    InitClasses(m_luaVM);

    // Update global variables
    lua_pushelement(m_luaVM, g_pClientGame->GetRootEntity());
    lua_setglobal(m_luaVM, "root");

    lua_pushresource(m_luaVM, m_pResource);
    lua_setglobal(m_luaVM, "resource");

    lua_pushelement(m_luaVM, m_pResource->GetResourceEntity());
    lua_setglobal(m_luaVM, "resourceRoot");

    lua_pushelement(m_luaVM, m_pResource->GetResourceGUIEntity());
    lua_setglobal(m_luaVM, "guiRoot");

    lua_pushelement(m_luaVM, g_pClientGame->GetLocalPlayer());
    lua_setglobal(m_luaVM, "localPlayer");
}

void CLuaMain::LoadEmbeddedScripts()
{
    DECLARE_PROFILER_SECTION(OnPreLoadScript)
    LoadScript(EmbeddedLuaCode::exports);
    LoadScript(EmbeddedLuaCode::coroutine_debug);
    LoadScript(EmbeddedLuaCode::inspect);
    DECLARE_PROFILER_SECTION(OnPostLoadScript)
}

void CLuaMain::InstructionCountHook(lua_State* luaVM, lua_Debug* pDebug)
{
    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        // Above max time?
        if (pLuaMain->m_FunctionEnterTimer.Get() > HOOK_MAXIMUM_TIME)
        {
            // Print it in the console
            CLogger::ErrorPrintf("Infinite/too long execution (%s)", pLuaMain->GetScriptName());

            SString strAbortInf = "Aborting; infinite running script in ";
            strAbortInf += pLuaMain->GetScriptName();

            // Error out
            luaL_error(luaVM, strAbortInf);
        }
    }
}

bool CLuaMain::LoadScriptFromBuffer(const char* cpInBuffer, unsigned int uiInSize, const char* szFileName)
{
    SString strNiceFilename = ConformResourcePath(szFileName);

    // Deobfuscate if required
    const char* cpBuffer;
    uint        uiSize;
    if (!g_pNet->DeobfuscateScript(cpInBuffer, uiInSize, &cpBuffer, &uiSize, strNiceFilename))
    {
        SString strMessage("%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename);
        g_pClientGame->GetScriptDebugging()->LogError(m_luaVM, "Loading script failed: %s", *strMessage);
        g_pClientGame->TellServerSomethingImportant(1003, SString("CLIENT SCRIPT ERROR: %s", *strMessage));
        return false;
    }

    bool bUTF8 = CLuaShared::CheckUTF8BOMAndUpdate(&cpBuffer, &uiSize);

    // If compiled script, make sure correct chunkname is embedded
    CLuaShared::EmbedChunkName(strNiceFilename, &cpBuffer, &uiSize);

    if (m_luaVM)
    {
        // Are we not marked as UTF-8 already, and not precompiled?
        std::string strUTFScript;
        if (!bUTF8 && !IsLuaCompiledScript(cpBuffer, uiSize))
        {
            std::string strBuffer = std::string(cpBuffer, uiSize);
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16(strBuffer));
            if (uiSize != strUTFScript.size())
            {
                uiSize = strUTFScript.size();
                g_pClientGame->GetScriptDebugging()->LogWarning(m_luaVM, "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", strNiceFilename.c_str());
            }
        }
        else
        {
            strUTFScript = std::string(cpBuffer, uiSize);
        }

        // Run the script
        const bool loadFailed = CLuaMain::LuaLoadBuffer(m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString("@%s", *strNiceFilename));

        // Clear raw script from memory
        std::fill(strUTFScript.begin(), strUTFScript.end(), 0);

        if (loadFailed)
        {
            // Print the error
            std::string strRes = lua_tostring(m_luaVM, -1);
            if (strRes.length())
            {
                CLogger::LogPrintf("SCRIPT ERROR: %s\n", strRes.c_str());
                g_pClientGame->GetScriptDebugging()->LogError(m_luaVM, "Loading script failed: %s", strRes.c_str());
            }
            else
            {
                CLogger::LogPrint("SCRIPT ERROR: Unknown\n");
                g_pClientGame->GetScriptDebugging()->LogError(m_luaVM, "Loading script failed for unknown reason");
            }

            return false;
        }
        else
        {
            ResetInstructionCount();
            int luaSavedTop = lua_gettop(m_luaVM);
            int iret = this->PCall(m_luaVM, 0, LUA_MULTRET, 0);
            if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
            {
                SString strRes = lua_tostring(m_luaVM, -1);
                g_pClientGame->GetScriptDebugging()->LogPCallError(m_luaVM, strRes, true);
            }
            // Cleanup any return values
            if (lua_gettop(m_luaVM) > luaSavedTop)
                lua_settop(m_luaVM, luaSavedTop);

            return true;
        }
    }

    std::fill_n(const_cast<char*>(cpBuffer), uiSize, 0);
    return false;
}

bool CLuaMain::LoadScript(const char* szLUAScript)
{
    const auto sz = strlen(szLUAScript);
    if (m_luaVM && !IsLuaCompiledScript(szLUAScript, sz))
    {
        // Run the script
        if (!CLuaMain::LuaLoadBuffer(m_luaVM, szLUAScript, sz, NULL))
        {
            ResetInstructionCount();
            int luaSavedTop = lua_gettop(m_luaVM);
            int iret = this->PCall(m_luaVM, 0, LUA_MULTRET, 0);
            if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
            {
                std::string strRes = ConformResourcePath(lua_tostring(m_luaVM, -1));
                g_pClientGame->GetScriptDebugging()->LogPCallError(m_luaVM, strRes);
            }
            // Cleanup any return values
            if (lua_gettop(m_luaVM) > luaSavedTop)
                lua_settop(m_luaVM, luaSavedTop);
        }
        else
        {
            std::string strRes = ConformResourcePath(lua_tostring(m_luaVM, -1));
            g_pClientGame->GetScriptDebugging()->LogError(m_luaVM, "Loading in-line script failed: %s", strRes.c_str());
        }
    }
    else
        return false;

    return true;
}

void CLuaMain::Start()
{
}

void CLuaMain::UnloadScript()
{
    // ACHTUNG: UNLOAD MODULES!

    // Delete all timers and events
    m_pLuaTimerManager->RemoveAllTimers();

    // Delete all GUI elements
    // m_pLuaManager->m_pGUIManager->DeleteAll ( this );

    /*
    // done at server version:
        // Delete all keybinds
        list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
        for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
        {
            if ( (*iter)->IsJoined () )
                (*iter)->GetKeyBinds ()->RemoveAllKeys ( this );
        }
    */
    // End the lua vm
    if (m_luaVM)
    {
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM(m_luaVM);
        m_pLuaManager->OnLuaMainCloseVM(this, m_luaVM);
        m_pLuaManager->AddToPendingDeleteList(m_luaVM);
        m_luaVM = NULL;
    }
}

void CLuaMain::DoPulse()
{
    m_pLuaTimerManager->DoPulse(this);
}

CXMLFile* CLuaMain::CreateXML(const char* szFilename, bool bUseIDs, bool bReadOnly)
{
    CXMLFile* pFile = g_pCore->GetXML()->CreateXML(szFilename, bUseIDs, bReadOnly);
    if (pFile)
        m_XMLFiles.push_back(pFile);
    return pFile;
}

CXMLNode* CLuaMain::ParseString(const char* strXmlContent)
{
    auto xmlStringNode = g_pCore->GetXML()->ParseString(strXmlContent);
    if (!xmlStringNode)
        return nullptr;

    auto node = xmlStringNode->node;
    m_XMLStringNodes.emplace(std::move(xmlStringNode));
    return node;
}

bool CLuaMain::DestroyXML(CXMLFile* pFile)
{
    if (m_XMLFiles.empty())
        return false;
    m_XMLFiles.remove(pFile);
    delete pFile;
    return true;
}

bool CLuaMain::DestroyXML(CXMLNode* pRootNode)
{
    if (m_XMLFiles.empty())
        return false;
    for (CXMLFile* pFile : m_XMLFiles)
    {
        if (pFile)
        {
            if (pFile->GetRootNode() == pRootNode)
            {
                m_XMLFiles.remove(pFile);
                delete pFile;
                break;
            }
        }
    }
    return true;
}

bool CLuaMain::SaveXML(CXMLNode* pRootNode)
{
    for (CXMLFile* pFile : m_XMLFiles)
        if (pFile)
            if (pFile->GetRootNode() == pRootNode)
                return pFile->Write();
    if (m_pResource)
    {
        list<CResourceConfigItem*>::iterator iter = m_pResource->ConfigIterBegin();
        for (; iter != m_pResource->ConfigIterEnd(); iter++)
        {
            CResourceConfigItem* pConfigItem = *iter;
            if (pConfigItem->GetRoot() == pRootNode)
            {
                CXMLFile* pFile = pConfigItem->GetFile();
                if (pFile)
                    return pFile->Write();
                return false;
            }
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::GetElementCount
//
//
//
///////////////////////////////////////////////////////////////
unsigned long CLuaMain::GetElementCount() const
{
    if (m_pResource && m_pResource->GetElementGroup())
        return m_pResource->GetElementGroup()->GetCount();
    return 0;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::GetFunctionTag
//
// Turn iFunctionNumber into something human readable
//
///////////////////////////////////////////////////////////////
const SString& CLuaMain::GetFunctionTag(int iLuaFunction)
{
    // Find existing
    SString* pTag = MapFind(m_FunctionTagMap, iLuaFunction);
#ifndef CHECK_FUNCTION_TAG
    if (!pTag)
#endif
    {
        // Create if required
        SString strText;

        lua_Debug debugInfo;
        lua_getref(m_luaVM, iLuaFunction);
        if (lua_getinfo(m_luaVM, ">nlS", &debugInfo))
        {
            // Make sure this function isn't defined in a string
            if (debugInfo.source[0] == '@')
            {
                // std::string strFilename2 = ConformResourcePath ( debugInfo.source );
                SString strFilename = debugInfo.source;

                int iPos = strFilename.find_last_of("/\\");
                if (iPos >= 0)
                    strFilename = strFilename.substr(iPos + 1);

                strText = SString("@%s:%d", strFilename.c_str(), debugInfo.currentline != -1 ? debugInfo.currentline : debugInfo.linedefined, iLuaFunction);
            }
            else
            {
                strText = SString("@func_%d %s", iLuaFunction, debugInfo.short_src);
            }
        }
        else
        {
            strText = SString("@func_%d NULL", iLuaFunction);
        }

    #ifdef CHECK_FUNCTION_TAG
        if (pTag)
        {
            // Check tag remains unchanged
            assert(strText == *pTag);
            return *pTag;
        }
    #endif

        MapSet(m_FunctionTagMap, iLuaFunction, strText);
        pTag = MapFind(m_FunctionTagMap, iLuaFunction);
    }
    return *pTag;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::PCall
//
// lua_pcall call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::PCall(lua_State* L, int nargs, int nresults, int errfunc)
{
    TIMING_CHECKPOINT("+pcall");
    g_pClientGame->ChangeFloatPrecision(true);
    g_pClientGame->GetScriptDebugging()->PushLuaMain(this);
    const int iret = lua_pcall(L, nargs, nresults, errfunc);
    g_pClientGame->GetScriptDebugging()->PopLuaMain(this);
    g_pClientGame->ChangeFloatPrecision(false);
    TIMING_CHECKPOINT("-pcall");
    return iret;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::LuaLoadBuffer
//
// luaL_loadbuffer call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::LuaLoadBuffer(lua_State* L, const char* buff, size_t sz, const char* name)
{
    if (IsLuaCompiledScript(buff, sz))
    {
        ms_strExpectedUndumpHash = GenerateSha256HexString(buff, sz);
    }

    int iResult = luaL_loadbuffer(L, buff, sz, name);

    ms_strExpectedUndumpHash = "";
    return iResult;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::OnUndump
//
// Callback from Lua when loading compiled bytes
//
///////////////////////////////////////////////////////////////
int CLuaMain::OnUndump(const char* p, size_t n)
{
    SString strGotHash = GenerateSha256HexString(p, n);
    SString strExpectedHash = ms_strExpectedUndumpHash;
    ms_strExpectedUndumpHash = "";
    if (strExpectedHash != strGotHash)
    {
        // I was not expecting that
        AddReportLog(7555, SString("Unexpected undump hash for buffer size %d. Got:%s Expected:%s", n, *strExpectedHash, *strGotHash));
        return 0;
    }
    return 1;
}
