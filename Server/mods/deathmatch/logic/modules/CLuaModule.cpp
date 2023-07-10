/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaModule.cpp
 *  PURPOSE:     Lua module extension class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaModule.h"
#include "CLuaModuleManager.h"
#include "CScriptDebugging.h"
#include "CStaticFunctionDefinitions.h"
#include "CGame.h"
#include "CResourceManager.h"

extern CGame* g_pGame;

CLuaModule::CLuaModule(CLuaModuleManager* pLuaModuleManager, CScriptDebugging* pScriptDebugging, const char* szFileName, const char* szShortFileName)
{
    // Set module manager
    m_pLuaModuleManager = pLuaModuleManager;
    // Set script debugging
    m_pScriptDebugging = pScriptDebugging;
    // set module path
    m_szFileName = SString("%s", szFileName);
    m_szShortFileName = SString("%s", szShortFileName);
    // set as uninitialised
    m_bInitialised = false;
}

CLuaModule::~CLuaModule()
{
    if (!m_hModule)
        return;

    if (m_bInitialised)
    {
        // Shutdown module
        m_FunctionInfo.ShutdownModule();

        // Unregister Functions
        _UnregisterFunctions();

        CLogger::LogPrintf("MODULE: Unloaded \"%s\" (%.2f) by \"%s\"\n", m_FunctionInfo.szModuleName, m_FunctionInfo.fVersion, m_FunctionInfo.szAuthor);
    }

    // Unload Module
    _UnloadModule();
}

#ifdef WIN32
bool IsModule32Bit(const SString& strExpectedPathFilename)
{
    FILE* fh = File::Fopen(strExpectedPathFilename, "rb");
    fseek(fh, 60, SEEK_SET);
    int offset = 0;
    fread(&offset, sizeof(offset), 1, fh);
    fseek(fh, offset + 24, SEEK_SET);
    ushort type = 0;
    fread(&type, sizeof(type), 1, fh);
    fclose(fh);
    return type == 0x010b;
}
#endif

int CLuaModule::_LoadModule()
{
    InitModuleFunc pfnInitFunc;
    // Load Module
    SString strError;
#ifdef WIN32
    // Search the mod path for dependencies
    SString strSavedCurrentDirectory = GetSystemCurrentDirectory();
    SetCurrentDirectory(PathJoin(g_pServerInterface->GetModManager()->GetServerPath(), SERVER_BIN_PATH_MOD));
    m_hModule = LoadLibrary(m_szFileName);
    if (!m_hModule)
        strError = SString("%d", GetLastError());
    SetCurrentDirectory(strSavedCurrentDirectory);
#else
    m_hModule = dlopen(m_szFileName, RTLD_NOW);
    if (!m_hModule)
        strError = dlerror();
#endif

    if (!m_hModule)
    {
        // Module failed to load
        SString strExpectedPathFilename = PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName);
        if (!FileExists(strExpectedPathFilename))
        {
            CLogger::LogPrintf("MODULE: File not found - %s\n", *strExpectedPathFilename);
        }
        else
    #ifdef WIN_x64
            if (IsModule32Bit(strExpectedPathFilename))
        {
            CLogger::LogPrintf("MODULE: File not 64 bit - %s\n", *strExpectedPathFilename);
        }
        else
    #endif
    #ifdef WIN_x86
            if (!IsModule32Bit(strExpectedPathFilename))
        {
            CLogger::LogPrintf("MODULE: File not 32 bit - %s\n", *strExpectedPathFilename);
        }
        else
    #endif
        {
            CLogger::LogPrintf("MODULE: Unable to load %s (%s)\n", *strExpectedPathFilename, *strError);
        }
        return 1;
    }

    // Find the initialisation function
#ifdef WIN32
    pfnInitFunc = (InitModuleFunc)(GetProcAddress(m_hModule, "InitModule"));
    if (!pfnInitFunc)
    {
        CLogger::LogPrintf("MODULE: Unable to initialize %s!\n", *PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName));
        return 2;
    }
#else
    pfnInitFunc = (InitModuleFunc)(dlsym(m_hModule, "InitModule"));
    if (dlerror() != NULL)
    {
        CLogger::LogPrintf("MODULE: Unable to initialize %s (%s)!\n", *PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName), dlerror());
        return 2;
    }
#endif

    // Initialise
    m_FunctionInfo.szFileName = m_szShortFileName;
#ifdef WIN32
    m_FunctionInfo.DoPulse = (DefaultModuleFunc)(GetProcAddress(m_hModule, "DoPulse"));
    if (!m_FunctionInfo.DoPulse)
        return 3;
    m_FunctionInfo.ShutdownModule = (DefaultModuleFunc)(GetProcAddress(m_hModule, "ShutdownModule"));
    if (!m_FunctionInfo.ShutdownModule)
        return 4;
    m_FunctionInfo.RegisterFunctions = (RegisterModuleFunc)(GetProcAddress(m_hModule, "RegisterFunctions"));
    if (!m_FunctionInfo.RegisterFunctions)
        return 5;

    m_FunctionInfo.ResourceStopping = (RegisterModuleFunc)(GetProcAddress(m_hModule, "ResourceStopping"));
    // No error for backward compatibility
    // if ( !m_FunctionInfo.ResourceStopping ) return 6;
    m_FunctionInfo.ResourceStopped = (RegisterModuleFunc)(GetProcAddress(m_hModule, "ResourceStopped"));
    // if ( !m_FunctionInfo.ResourceStopped ) return 7;
#else
    m_FunctionInfo.DoPulse = (DefaultModuleFunc)(dlsym(m_hModule, "DoPulse"));
    if (!m_FunctionInfo.DoPulse)
        return 3;
    m_FunctionInfo.ShutdownModule = (DefaultModuleFunc)(dlsym(m_hModule, "ShutdownModule"));
    if (!m_FunctionInfo.ShutdownModule)
        return 4;
    m_FunctionInfo.RegisterFunctions = (RegisterModuleFunc)(dlsym(m_hModule, "RegisterFunctions"));
    if (!m_FunctionInfo.RegisterFunctions)
        return 5;

    m_FunctionInfo.ResourceStopping = (RegisterModuleFunc)(dlsym(m_hModule, "ResourceStopping"));
    // if ( !m_FunctionInfo.ResourceStopping ) return 6;
    m_FunctionInfo.ResourceStopped = (RegisterModuleFunc)(dlsym(m_hModule, "ResourceStopped"));
    // if ( !m_FunctionInfo.ResourceStopped ) return 7;
#endif
    // Run initialisation function
    if (!pfnInitFunc(this, &m_FunctionInfo.szModuleName[0], &m_FunctionInfo.szAuthor[0], &m_FunctionInfo.fVersion))
    {
        CLogger::LogPrintf("MODULE: Unable to initialize %s!\n", *PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName));
        return 2;
    }
    m_bInitialised = true;

    CLogger::LogPrintf("MODULE: Loaded \"%s\" (%.2f) by \"%s\"\n", m_FunctionInfo.szModuleName, m_FunctionInfo.fVersion, m_FunctionInfo.szAuthor);

    return 0;
}

void CLuaModule::_UnloadModule()
{
    // Unload from memory
#ifdef WIN32
    FreeLibrary(m_hModule);
#else
    dlclose(m_hModule);
#endif
}

void CLuaModule::_RegisterFunctions(lua_State* luaVM)
{
    m_FunctionInfo.RegisterFunctions(luaVM);
}

void CLuaModule::_UnregisterFunctions()
{
    for (const auto& liter : *m_pLuaModuleManager->GetLuaManager())
    {
        lua_State* luaVM = liter->GetVM();

        for (const auto& function : m_Functions)
        {
            // points function to nil
            lua_pushnil(luaVM);
            lua_setglobal(luaVM, function.c_str());

            // Remove func from CLuaCFunctions
            CLuaCFunctions::RemoveFunction(function);
        }
    }
}

void CLuaModule::_DoPulse()
{
    m_FunctionInfo.DoPulse();
}

void CLuaModule::_ResourceStopping(lua_State* luaVM)
{
    if (m_FunctionInfo.ResourceStopping)
        m_FunctionInfo.ResourceStopping(luaVM);
}

void CLuaModule::_ResourceStopped(lua_State* luaVM)
{
    if (m_FunctionInfo.ResourceStopped)
        m_FunctionInfo.ResourceStopped(luaVM);

    for (const auto& iter : m_Functions)
    {
        lua_pushnil(luaVM);
        lua_setglobal(luaVM, iter.c_str());
    }
}

bool CLuaModule::_DoesFunctionExist(const char* szFunctionName)
{
    for (const auto& iter : m_Functions)
        if (!strcmp(iter.c_str(), szFunctionName))
            return true;

    return false;
}

// Module Functions
void CLuaModule::ErrorPrintf(const char* szFormat, ...) const noexcept
{
    va_list args;
    va_start(args, szFormat);
    CLogger::ErrorPrintvf(szFormat, args);
    va_end(args);
}

void CLuaModule::DebugPrintf(lua_State* luaVM, const char* szFormat, ...) const noexcept
{
    va_list args;
    va_start(args, szFormat);
    m_pScriptDebugging->LogInformationV(luaVM, szFormat, args);
    va_end(args);
}

void CLuaModule::Printf(const char* szFormat, ...) const noexcept
{
    va_list args;
    va_start(args, szFormat);
    CLogger::LogPrintvf(szFormat, args);
    va_end(args);
}

bool CLuaModule::RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction Func)
{
    if (luaVM)
    {
        if (!szFunctionName)
            return false; // if the name wasn't provided, why returning true (success)?

        CLuaCFunctions::AddFunction(szFunctionName, Func);
        lua_register(luaVM, szFunctionName, Func);
        if (!_DoesFunctionExist(szFunctionName))
        {
            // Check or it adds for each resource
            m_Functions.push_back(szFunctionName);
        }
    }
    else
    {
        CLogger::LogPrintf("MODULE: Lua is not initialised.\n");
    }
    return true;
}

std::optional<std::string> CLuaModule::GetResourceName(lua_State* luaVM) const noexcept
{
    if (!luaVM)
        return nullptr;

    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return nullptr;

    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return nullptr;

    return pResource->GetName();
}

std::optional<CChecksum> CLuaModule::GetResourceMetaChecksum(lua_State* luaVM) const noexcept
{
    if (!luaVM)
        return {};

    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return {};

    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return {};

    return pResource ? pResource->GetLastMetaChecksum() : CChecksum();
}

std::optional<CChecksum> CLuaModule::GetResourceFileChecksum(lua_State* luaVM, const char* szFile) const noexcept
{
    if (!luaVM)
        return {};

    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return {};

    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return {};

    auto iter = pResource->IterBegin();
    for (; iter != pResource->IterEnd(); ++iter)
        if (!strcmp((*iter)->GetName(), szFile))
            return (*iter)->GetLastChecksum();

    return CChecksum();
}

unsigned long CLuaModule::GetVersion() const noexcept
{
    return CStaticFunctionDefinitions::GetVersion();
}

const char* CLuaModule::GetVersionString() const noexcept
{
    return CStaticFunctionDefinitions::GetVersionString();
}

const char* CLuaModule::GetVersionName() const noexcept
{
    return CStaticFunctionDefinitions::GetVersionName();
}

unsigned long CLuaModule::GetNetcodeVersion() const noexcept
{
    return CStaticFunctionDefinitions::GetNetcodeVersion();
}

const char* CLuaModule::GetOperatingSystemName() const noexcept
{
    return CStaticFunctionDefinitions::GetOperatingSystemName();
}

lua_State* CLuaModule::GetResourceFromName(const char* szResourceName) const noexcept
{
    CResource* pResource = g_pGame->GetResourceManager()->GetResource(szResourceName);

    if (!pResource)
        return nullptr;

    CLuaMain* pLuaMain = pResource->GetVirtualMachine();

    return pLuaMain ? pLuaMain->GetVM() : nullptr;
}

std::optional<std::string> CLuaModule::GetResourceFilePath(lua_State* luaVM, const char* fileName) const noexcept
{
    if (!luaVM)
        return nullptr;

    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return nullptr;

    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return nullptr;

    std::string path;
    if (!pResource->GetFilePath(fileName, path))
        return nullptr;

    return path;
}

CVehicleManager* CLuaModule::GetVehicleManager() const noexcept
{
    return g_pGame->GetVehicleManager();
}

CMainConfig* CLuaModule::GetConfigManager() const noexcept
{
    return g_pGame->GetConfig();
}

CTeamManager* CLuaModule::GetTeamManager() const noexcept
{
    return g_pGame->GetTeamManager();
}
