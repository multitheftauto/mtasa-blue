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

CLuaModule::CLuaModule(CLuaModuleManager* pLuaModuleManager, CScriptDebugging* pScriptDebugging,
    const char* szFileName, const char* szShortFileName
) noexcept : m_pLuaModuleManager(pLuaModuleManager), m_pScriptDebugging(pScriptDebugging),
    m_szFileName(SString("%s", szFileName)), m_szShortFileName(SString("%s", szShortFileName)),
    m_bInitialised(false)
{}

CLuaModule::~CLuaModule() noexcept
{
    if (!m_hModule)
        return;
        
    if (m_bInitialised)
    {
        // Shutdown module
        m_FunctionInfo.ShutdownModule();

        // Unregister Functions
        UnregisterFunctions();

        CLogger::LogPrintf(
            "MODULE: Unloaded \"%s\" (%.2f) by \"%s\"\n",
            m_FunctionInfo.szModuleName, m_FunctionInfo.fVersion, m_FunctionInfo.szAuthor
        );
    }

    // Unload Module
    UnloadModule();
}

#ifdef _WIN32
bool IsModule32Bit(const SString& strExpectedPathFilename) noexcept
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

void* GetProcAddr(HMODULE module, const char* procName) noexcept {
#ifdef _WIN32
    return GetProcAddress(module, procName);
#else
    return dlsym(module, procName);
#endif
}

int CLuaModule::LoadModule() noexcept
{
    // Load Module
    SString strError;
#ifdef _WIN32
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
    m_FunctionInfo.InitModule = (InitModuleFunc)(GetProcAddr(m_hModule, "InitModule"));
#ifdef _WIN32
    if (!m_FunctionInfo.InitModule)
    {
        CLogger::LogPrintf("MODULE: Unable to initialize %s!\n",
            *PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName)
        );
        return 2;
    }
#else
    if (dlerror())
    {
        CLogger::LogPrintf("MODULE: Unable to initialize %s (%s)!\n",
            *PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName), dlerror()
        );
        return 2;
    }
#endif

    // Initialise
    m_FunctionInfo.szFileName = m_szShortFileName;
    m_FunctionInfo.DoPulse = (DefaultModuleFunc)(GetProcAddr(m_hModule, "DoPulse"));
    if (!m_FunctionInfo.DoPulse)
        return 3;
    m_FunctionInfo.ShutdownModule = (DefaultModuleFunc)(GetProcAddr(m_hModule, "ShutdownModule"));
    if (!m_FunctionInfo.ShutdownModule)
        return 4;
    m_FunctionInfo.RegisterFunctions = (RegisterModuleFunc)(GetProcAddr(m_hModule, "RegisterFunctions"));
    if (!m_FunctionInfo.RegisterFunctions)
        return 5;

    m_FunctionInfo.ResourceStarting = (RegisterModuleFunc)(GetProcAddr(m_hModule, "ResourceStarting"));
    m_FunctionInfo.ResourceStarted = (RegisterModuleFunc)(GetProcAddr(m_hModule, "ResourceStarted"));

    m_FunctionInfo.ResourceStopping = (RegisterModuleFunc)(GetProcAddr(m_hModule, "ResourceStopping"));
    m_FunctionInfo.ResourceStopped = (RegisterModuleFunc)(GetProcAddr(m_hModule, "ResourceStopped"));
    // Run initialisation function
    if (!m_FunctionInfo.InitModule(this, &m_FunctionInfo.szModuleName[0], &m_FunctionInfo.szAuthor[0], &m_FunctionInfo.fVersion))
    {
        CLogger::LogPrintf("MODULE: Unable to initialize %s!\n", *PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName));
        return 2;
    }
    m_bInitialised = true;

    CLogger::LogPrintf("MODULE: Loaded \"%s\" (%.2f) by \"%s\"\n", m_FunctionInfo.szModuleName, m_FunctionInfo.fVersion, m_FunctionInfo.szAuthor);

    return 0;
}

void CLuaModule::UnloadModule() noexcept
{
    // Unload from memory
#ifdef _WIN32
    FreeLibrary(m_hModule);
#else
    dlclose(m_hModule);
#endif
}

void CLuaModule::RegisterFunctions(lua_State* luaVM) noexcept
{
    m_FunctionInfo.RegisterFunctions(luaVM);
}

void CLuaModule::UnregisterFunctions() noexcept
{
    for (const auto& pVM : *m_pLuaModuleManager->GetLuaManager())
    {
        lua_State* luaVM = pVM->GetVM();
        for (const auto& pFunc : m_Functions)
        {
            // points function to nill
            lua_pushnil(luaVM);
            lua_setglobal(luaVM, pFunc.c_str());

            // Remove func from CLuaCFunctions
            CLuaCFunctions::RemoveFunction(pFunc);
        }
    }
}

void CLuaModule::DoPulse() noexcept
{
    m_FunctionInfo.DoPulse();
}

void CLuaModule::ResourceStopping(lua_State* luaVM) noexcept
{
    if (m_FunctionInfo.ResourceStopping)
        m_FunctionInfo.ResourceStopping(luaVM);
}

void CLuaModule::ResourceStopped(lua_State* luaVM) noexcept
{
    if (m_FunctionInfo.ResourceStopped)
        m_FunctionInfo.ResourceStopped(luaVM);

    for (const auto& pFunc : m_Functions)
    {
        // points function to nil
        lua_pushnil(luaVM);
        lua_setglobal(luaVM, pFunc.c_str());
    }
}

void CLuaModule::ResourceStarting(lua_State* luaVM) noexcept
{
    if (m_FunctionInfo.ResourceStarting)
        m_FunctionInfo.ResourceStarting(luaVM);
}

void CLuaModule::ResourceStarted(lua_State* luaVM) noexcept {
    if (m_FunctionInfo.ResourceStarted)
        m_FunctionInfo.ResourceStarted(luaVM);
}

bool CLuaModule::DoesFunctionExist(const char* szFunctionName) const noexcept
{
    for (const auto& pFunc : m_Functions)
    {
        if (!strcmp(pFunc.c_str(), szFunctionName))
            return true;
    }
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

bool CLuaModule::RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction Func) noexcept
{
    if (!luaVM)
    {
        CLogger::LogPrintf("MODULE: Lua is not initialised.\n");
        return false;
    }
    if (!szFunctionName)
        return false;

    if (DoesFunctionExist(szFunctionName)) 
        return false;
        
    CLuaCFunctions::AddFunction(szFunctionName, Func);
    lua_register(luaVM, szFunctionName, Func);
    m_Functions.push_back(szFunctionName);
    
    return true;
}

bool CLuaModule::GetResourceName(lua_State* luaVM, std::string& strName) const noexcept
{
    if (!luaVM)
        return false;
        
    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return false;
        
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return false;
        
    strName = pResource->GetName();
    return true;
}

std::optional<std::string> CLuaModule::GetResourceName(lua_State* luaVM) const noexcept {
    if (!luaVM)
        return std::nullopt;
        
    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return std::nullopt;
        
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return std::nullopt;
        
    return pResource->GetName();
}

CChecksum CLuaModule::GetResourceMetaChecksum(lua_State* luaVM) const noexcept
{
    if (!luaVM)
        return {};
        
    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return {};
    
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return {};
        
    return pResource->GetLastMetaChecksum();
}

CChecksum CLuaModule::GetResourceFileChecksum(lua_State* luaVM, const char* szFile) const noexcept
{
    if (!luaVM)
        return {};
        
    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return {};
        
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return {};
        
    for (const auto& pFile : pResource->GetFiles())
    {
        if (!strcmp(pFile->GetName(), szFile))
            return pFile->GetLastChecksum();
    }
    return {};
}

std::uint32_t CLuaModule::GetVersion() const noexcept
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

std::uint32_t CLuaModule::GetNetcodeVersion() const noexcept
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
    if (!pLuaMain)
        return nullptr;
        
    return pLuaMain->GetVM();
}

std::string CLuaModule::GetResourcePath(lua_State* luaVM) const noexcept
{
    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return "";

    CResource* pResource = pLuaMain->GetResource();

    if (!pResource)
        return "";

    return pResource->GetResourceDirectoryPath();
}

std::string CLuaModule::GetResourceFilePath(lua_State* luaVM, const char* fileName) const noexcept
{
    if (!luaVM)
        return "";

    CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine(luaVM);
    if (!pLuaMain)
        return "";

    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return "";

    std::string p;
    if (!pResource->GetFilePath(fileName, p))
        return "";

    return p;
}
