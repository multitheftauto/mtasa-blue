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
#include <sdk/SharedUtil.File.h>
#include <sdk/SharedUtil.Defines.h>

#define MTA_API_VERSION 2
#define MTA_API_REVISION 0

CLuaModule::CLuaModule(CLuaModuleManager* pLuaModuleManager, IModuleInterface* pModuleInterface, const char* szFileName, const char* szShortFileName)
{
    // Set module manager
    m_pLuaModuleManager = pLuaModuleManager;
    // Set script debugging
    m_pModuleInterface = pModuleInterface;
    // set module path
    m_szFileName = szFileName;
    m_szShortFileName = szShortFileName;
    // set as uninitialised
    m_bInitialised = false;
}

CLuaModule::~CLuaModule()
{
    if (m_hModule)
    {
        if (m_bInitialised)
        {
            // Shutdown module
            m_FunctionInfo.ShutdownModule();

            // Unregister Functions
            _UnregisterFunctions();

            m_pModuleInterface->GetLogger()->Printf("MODULE: Unloaded \"%s\" (%.2f) by \"%s\"\n", m_FunctionInfo.szModuleName, m_FunctionInfo.fVersion, m_FunctionInfo.szAuthor);
        }

        // Unload Module
        _UnloadModule();
    }
}

#ifdef WIN32
bool IsModule32Bit(const std::string& strExpectedPathFilename)
{
    FILE* fh = SharedUtil::File::Fopen(strExpectedPathFilename.c_str(), "rb");
    fseek(fh, 60, SEEK_SET);
    int offset = 0;
    fread(&offset, sizeof(offset), 1, fh);
    fseek(fh, offset + 24, SEEK_SET);
    unsigned short type = 0;
    fread(&type, sizeof(type), 1, fh);
    fclose(fh);
    return type == 0x010b;
}
#endif

int CLuaModule::_LoadModule()
{
    InitModuleFunc pfnInitFunc;
    // Load Module
    std::string strError;
#ifdef WIN32
    // Search the mod path for dependencies
    std::string strSavedCurrentDirectory = SharedUtil::GetSystemCurrentDirectory();
    SetCurrentDirectory(SharedUtil::PathJoin(m_pModuleInterface->GetServerPath(), SERVER_BIN_PATH_MOD));
    m_hModule = LoadLibrary(m_szFileName.c_str());
    if (m_hModule == NULL)
        strError = std::string("%d", GetLastError());
    SetCurrentDirectory(strSavedCurrentDirectory.c_str());
#else
    m_hModule = dlopen(m_szFileName, RTLD_NOW);
    if (m_hModule == NULL)
        strError = dlerror();
#endif

    if (m_hModule == NULL)
    {
        // Module failed to load
        std::string strExpectedPathFilename = SharedUtil::PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName);
        if (!SharedUtil::FileExists(strExpectedPathFilename))
        {
            m_pModuleInterface->GetLogger()->Printf("MODULE: File not found - %s\n", strExpectedPathFilename);
        }
        else
    #ifdef WIN_x64
            if (IsModule32Bit(strExpectedPathFilename))
        {
            m_pModuleInterface->GetLogger()->Printf("MODULE: File not 64 bit - %s\n", strExpectedPathFilename);
        }
        else
    #endif
    #ifdef WIN_x86
            if (!IsModule32Bit(strExpectedPathFilename))
        {
            m_pModuleInterface->GetLogger()->Printf("MODULE: File not 32 bit - %s\n", strExpectedPathFilename);
        }
        else
    #endif
        {
            m_pModuleInterface->GetLogger()->Printf("MODULE: Unable to load %s (%s)\n", strExpectedPathFilename, strError);
        }
        return 1;
    }

    // Find the initialisation function
#ifdef WIN32
    pfnInitFunc = (InitModuleFunc)(GetProcAddress(m_hModule, "InitModule"));
    if (pfnInitFunc == NULL)
    {
        m_pModuleInterface->GetLogger()->Printf("MODULE: Unable to initialize %s!\n", *SharedUtil::PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName));
        return 2;
    }
#else
    pfnInitFunc = (InitModuleFunc)(dlsym(m_hModule, "InitModule"));
    if (dlerror() != NULL)
    {
        m_pModuleInterface->GetLogger()->Printf("MODULE: Unable to initialize %s (%s)!\n", *PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName), dlerror());
        return 2;
    }
#endif

    // Initialise
    m_FunctionInfo.szFileName = m_szShortFileName;
#ifdef WIN32
    m_FunctionInfo.DoPulse = (DefaultModuleFunc)(GetProcAddress(m_hModule, "DoPulse"));
    if (m_FunctionInfo.DoPulse == NULL)
        return 3;
    m_FunctionInfo.ShutdownModule = (DefaultModuleFunc)(GetProcAddress(m_hModule, "ShutdownModule"));
    if (m_FunctionInfo.ShutdownModule == NULL)
        return 4;
    m_FunctionInfo.RegisterFunctions = (RegisterModuleFunc)(GetProcAddress(m_hModule, "RegisterFunctions"));
    if (m_FunctionInfo.RegisterFunctions == NULL)
        return 5;

    m_FunctionInfo.ResourceStopping = (RegisterModuleFunc)(GetProcAddress(m_hModule, "ResourceStopping"));
    // No error for backward compatibility
    // if ( m_FunctionInfo.ResourceStopping == NULL ) return 6;
    m_FunctionInfo.ResourceStopped = (RegisterModuleFunc)(GetProcAddress(m_hModule, "ResourceStopped"));
    // if ( m_FunctionInfo.ResourceStopped == NULL ) return 7;
#else
    m_FunctionInfo.DoPulse = (DefaultModuleFunc)(dlsym(m_hModule, "DoPulse"));
    if (m_FunctionInfo.DoPulse == NULL)
        return 3;
    m_FunctionInfo.ShutdownModule = (DefaultModuleFunc)(dlsym(m_hModule, "ShutdownModule"));
    if (m_FunctionInfo.ShutdownModule == NULL)
        return 4;
    m_FunctionInfo.RegisterFunctions = (RegisterModuleFunc)(dlsym(m_hModule, "RegisterFunctions"));
    if (m_FunctionInfo.RegisterFunctions == NULL)
        return 5;

    m_FunctionInfo.ResourceStopping = (RegisterModuleFunc)(dlsym(m_hModule, "ResourceStopping"));
    // if ( m_FunctionInfo.ResourceStopping == NULL ) return 6;
    m_FunctionInfo.ResourceStopped = (RegisterModuleFunc)(dlsym(m_hModule, "ResourceStopped"));
    // if ( m_FunctionInfo.ResourceStopped == NULL ) return 7;
#endif
    // Run initialisation function
    if (!pfnInitFunc(this, &m_FunctionInfo.szModuleName[0], &m_FunctionInfo.szAuthor[0], &m_FunctionInfo.fVersion))
    {
        m_pModuleInterface->GetLogger()->Printf("MODULE: Unable to initialize %s!\n", *SharedUtil::PathJoin(SERVER_BIN_PATH_MOD, "modules", m_szShortFileName));
        return 2;
    }
    m_bInitialised = true;

    m_pModuleInterface->GetLogger()->Printf("MODULE: Loaded \"%s\" (%.2f) by \"%s\"\n", m_FunctionInfo.szModuleName, m_FunctionInfo.fVersion, m_FunctionInfo.szAuthor);

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

void CLuaModule::_RegisterFunctions(IResource* pResource)
{
    m_FunctionInfo.RegisterFunctions(pResource->GetLuaState());
}

void CLuaModule::_UnregisterFunctions()
{
    //list<CLuaMain*>::const_iterator liter = m_pLuaModuleManager->GetLuaManager()->IterBegin();
    //for (; liter != m_pLuaModuleManager->GetLuaManager()->IterEnd(); ++liter)
    //{
    //    lua_State*                luaVM = (*liter)->GetVM();
    //    vector<std::string>::iterator iter = m_Functions.begin();
    //    for (; iter != m_Functions.end(); ++iter)
    //    {
    //        // points function to nill
    //        lua_pushnil(luaVM);
    //        lua_setglobal(luaVM, iter->c_str());

    //        // Remove func from CLuaCFunctions
    //        CLuaCFunctions::RemoveFunction(*iter);
    //    }
    //}
}

void CLuaModule::_DoPulse()
{
    m_FunctionInfo.DoPulse();
}

void CLuaModule::_ResourceStopping(IResource* resource)
{
    if (m_FunctionInfo.ResourceStopping)
        m_FunctionInfo.ResourceStopping(resource->GetLuaState());
}

void CLuaModule::_ResourceStopped(IResource* resource)
{
    lua_State* luaVM = resource->GetLuaState();
    if (m_FunctionInfo.ResourceStopped)
        m_FunctionInfo.ResourceStopped(luaVM);

    std::vector<std::string>::iterator iter = m_Functions.begin();
    for (; iter != m_Functions.end(); ++iter)
    {
        // points function to nil
        lua_pushnil(luaVM);
        lua_setglobal(luaVM, (iter)->c_str());
    }
}

bool CLuaModule::_DoesFunctionExist(const char* szFunctionName)
{
    std::vector<std::string>::iterator iter = m_Functions.begin();
    for (; iter != m_Functions.end(); ++iter)
    {
        if (strcmp((iter)->c_str(), szFunctionName) == 0)
        {
            return true;
        }
    }
    return false;
}

// Module Functions
void CLuaModule::ErrorPrintf(const char* szFormat, ...)
{
    va_list args;
    va_start(args, szFormat);
    m_pModuleInterface->GetLogger()->ErrorPrintf(szFormat, args);
    va_end(args);
}

void CLuaModule::DebugPrintf(lua_State* luaVM, const char* szFormat, ...)
{
    va_list args;
    va_start(args, szFormat);
    m_pModuleInterface->GetLogger()->LogInformation(luaVM, szFormat, args);
    va_end(args);
}

void CLuaModule::Printf(const char* szFormat, ...)
{
    va_list args;
    va_start(args, szFormat);
    m_pModuleInterface->GetLogger()->Printf(szFormat, args);
    va_end(args);
}

bool CLuaModule::RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction function)
{
    if (luaVM)
    {
        if (szFunctionName)
        {
            m_pModuleInterface->AddFunction(luaVM, szFunctionName, function);

            if (!_DoesFunctionExist(szFunctionName))
            {
                // Check or it adds for each resource
                m_Functions.push_back(szFunctionName);
            }
        }
    }
    else
    {
        m_pModuleInterface->GetLogger()->Printf("MODULE: Lua is not initialised.\n");
    }
    return true;
}

bool CLuaModule::GetResourceName(lua_State* luaVM, std::string& strName)
{
    IResource* pResource = m_pModuleInterface->GetResourceFromLuaState(luaVM);

    if (pResource)
    {
        strName = pResource->GetName();
        return true;
    }
    return false;
}

CChecksum CLuaModule::GetResourceMetaChecksum(lua_State* luaVM)
{
    IResource* pResource = m_pModuleInterface->GetResourceFromLuaState(luaVM);

    if (pResource)
    {
        return pResource->GetLastMetaChecksum();
    }
    return CChecksum();
}

CChecksum CLuaModule::GetResourceFileChecksum(lua_State* luaVM, const char* szFile)
{
    IResource* pResource = m_pModuleInterface->GetResourceFromLuaState(luaVM);

    if (pResource)
    {
        return pResource->GetFileChecksum(szFile);
    }
    return CChecksum();
}

unsigned long CLuaModule::GetVersion()
{
    return m_pModuleInterface->GetVersion();
}

const char* CLuaModule::GetVersionString()
{
    return m_pModuleInterface->GetVersionString();
}

const char* CLuaModule::GetVersionName()
{
    return m_pModuleInterface->GetVersionName();
}

unsigned long CLuaModule::GetNetcodeVersion()
{
    return m_pModuleInterface->GetNetcodeVersion();
}

const char* CLuaModule::GetOperatingSystemName()
{
    return m_pModuleInterface->GetOperatingSystemName();
}

lua_State* CLuaModule::GetResourceFromName(const char* szResourceName)
{
    IResource* pResource = m_pModuleInterface->GetResourceFromName(szResourceName);
    if (pResource)
    {
        return pResource->GetLuaState();
    }
    return nullptr;
}

bool CLuaModule::GetResourceName(lua_State* luaVM, char* szName, size_t length)
{
    std::string resourceName;
    if (GetResourceName(luaVM, resourceName))
    {
        std::strncpy(szName, resourceName.c_str(), length);
        return true;
    }

    return false;
}

bool CLuaModule::GetResourceFilePath(lua_State* luaVM, const char* fileName, char* path, size_t length)
{
    if (!luaVM)
        return false;

    IResource* pResource = m_pModuleInterface->GetResourceFromLuaState(luaVM);

    if (!pResource)
        return false;

    std::string p;
    if (!pResource->GetFilePath(fileName, p))
        return false;

    std::strncpy(path, p.c_str(), length);
    return true;
}

IResource* CLuaModule::GetResourceFromNameV2(const char* szResourceName)
{
    return m_pModuleInterface->GetResourceFromName(szResourceName);
}

IResource* CLuaModule::GetResourceFromLuaState(lua_State* luaVM)
{
    return m_pModuleInterface->GetResourceFromLuaState(luaVM);
}
