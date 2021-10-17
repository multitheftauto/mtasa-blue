#include "StdInc.h"

void CModuleLoggerImpl::Printf(const char* szFormat, ...)
{
    va_list args;
    va_start(args, szFormat);
    CLogger::LogPrintf(szFormat, va_pass(args));
    va_end(args);
}

void CModuleLoggerImpl::ErrorPrintf(const char* szFormat, ...)
{
    va_list args;
    va_start(args, szFormat);
    CLogger::ErrorPrintf(szFormat, va_pass(args));
    va_end(args);
}

void CModuleLoggerImpl::LogInformation(lua_State* luaVM, const char* szFormat, ...)
{
    va_list args;
    va_start(args, szFormat);
    g_pGame->GetScriptDebugging()->LogInformation(luaVM, szFormat, va_pass(args));
    va_end(args);
}

CModuleInterfaceImpl::CModuleInterfaceImpl(CLuaManager* pLuaManager, ILogger* pLoggerImpl) : m_pLuaManager(pLuaManager), m_pLogger(pLoggerImpl)
{
}

ILogger* CModuleInterfaceImpl::GetLogger()
{
    return m_pLogger;
}

IResource* CModuleInterfaceImpl::GetResourceFromName(const char* name)
{
    CResource* pResource = g_pGame->GetResourceManager()->GetResource(name);

    if (pResource)
    {
        CLuaMain* pLuaMain = pResource->GetVirtualMachine();
        if (pLuaMain)
        {
            return (IResource*)pLuaMain->GetResource();
        }
    }

    return nullptr;
}

const char* CModuleInterfaceImpl::GetServerPath()
{
    return g_pServerInterface->GetModManager()->GetServerPath();
}

const char* CModuleInterfaceImpl::GetModulesPath()
{
    return PathJoin(GetServerPath(), SERVER_BIN_PATH_MOD, "modules");
}

IResource* CModuleInterfaceImpl::GetResourceFromLuaState(lua_State* luaVM)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();
        if (pResource)
        {
            return pResource;
        }
    }
    return nullptr;
}

unsigned long CModuleInterfaceImpl::GetVersion()
{
    return CStaticFunctionDefinitions::GetVersion();
}

const char* CModuleInterfaceImpl::GetVersionString()
{
    return CStaticFunctionDefinitions::GetVersionString();
}

const char* CModuleInterfaceImpl::GetVersionName()
{
    return CStaticFunctionDefinitions::GetVersionName();
}

unsigned long CModuleInterfaceImpl::GetNetcodeVersion()
{
    return CStaticFunctionDefinitions::GetNetcodeVersion();
}

const char* CModuleInterfaceImpl::GetOperatingSystemName()
{
    return CStaticFunctionDefinitions::GetOperatingSystemName();
}

void CModuleInterfaceImpl::AddFunction(lua_State* luaVM, const char* szName, lua_CFunction function)
{
    CLuaCFunctions::AddFunction(szName, function);
    lua_register(luaVM, szName, function);
}

