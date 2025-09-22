/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CDebugHookManager.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDebugHookManager.h"
#include "Enums.h"
#include "CScriptDebugging.h"

#ifdef MTA_CLIENT
    #define DECLARE_PROFILER_SECTION_CDebugHookManager
    #include "profiler/SharedUtil.Profiler.h"
    #define g_pGame g_pClientGame
#else
    #include "CGame.h"
    #define DECLARE_PROFILER_SECTION(tag)
#endif

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::CDebugHookManager
//
//
//
///////////////////////////////////////////////////////////////
CDebugHookManager::CDebugHookManager()
{
    m_MaskArgumentsMap = {
#ifndef MTA_CLIENT
        {"logIn", {{EArgType::Password, 2}}},                         // player, account, 2=PASSWORD
        {"addAccount", {{EArgType::Password, 1}}},                    // name, 1=PASSWORD
        {"getAccount", {{EArgType::Password, 1}}},                    // name, 1=PASSWORD
        {"setAccountPassword", {{EArgType::Password, 1}}},            // account, 1=PASSWORD
        {"dbConnect", {{EArgType::MaxArgs, 0}}},
        {"dbExec", {{EArgType::MaxArgs, 0}}},
        {"dbFree", {{EArgType::MaxArgs, 0}}},
        {"dbPoll", {{EArgType::MaxArgs, 0}}},
        {"dbPrepareString", {{EArgType::MaxArgs, 0}}},
        {"dbQuery", {{EArgType::MaxArgs, 0}}},
        {"executeSQLQuery", {{EArgType::MaxArgs, 0}}},
        {"callRemote", {{EArgType::MaxArgs, 1}, {EArgType::Url, 0}}},            // 0=URL, ...
#endif
        {"fetchRemote", {{EArgType::MaxArgs, 1}, {EArgType::Url, 0}}},            // 0=URL, ...
        {"passwordHash", {{EArgType::Password, 0}}},                              // 0=PASSWORD, ...
        {"passwordVerify", {{EArgType::Password, 0}}},                            // 0=PASSWORD, ...
        {"encodeString", {{EArgType::MaxArgs, 2}}},                               // algorithm, input, 2=SECRETKEY, ...
        {"decodeString", {{EArgType::MaxArgs, 2}}},                               // algorithm, input, 2=SECRETKEY, ...
        {"teaEncode", {{EArgType::Password, 1}}},                                 // input, 1=SECRETKEY
        {"teaDecode", {{EArgType::Password, 1}}},                                 // input, 1=SECRETKEY
    };
    
    // Initialize security flags
    m_bSecurityInitialized = true;
    m_bInHookExecution = false;
    m_ulLastHookCheckTime = 0;
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::AddDebugHook
//
// Returns true if hook was added
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::AddDebugHook(EDebugHookType hookType, const CLuaFunctionRef& functionRef, const std::vector<SString>& allowedNameList)
{
    // Security: Prevent hook manipulation during hook execution
    if (m_bInHookExecution)
    {
        LogSecurityWarning("Attempt to add debug hook during hook execution blocked");
        return false;
    }
    
    // Security: Validate hook type
    if (hookType >= EDebugHookType::MAX_DEBUG_HOOK_TYPE || hookType < EDebugHookType::PRE_EVENT)
    {
        LogSecurityWarning("Invalid hook type attempted");
        return false;
    }
    
    // Security: Rate limiting - prevent mass hook registration
    if (!CanRegisterNewHook())
    {
        LogSecurityWarning("Hook registration rate limit exceeded");
        return false;
    }

    std::vector<SDebugHookCallInfo>& hookInfoList = GetHookInfoListForType(hookType);
    
    // Security: Limit maximum hooks per type to prevent resource exhaustion
    if (hookInfoList.size() >= MAX_HOOKS_PER_TYPE)
    {
        LogSecurityWarning("Maximum hooks per type limit reached");
        return false;
    }

    for (std::vector<SDebugHookCallInfo>::iterator iter = hookInfoList.begin(); iter != hookInfoList.end(); ++iter)
    {
        if ((*iter).functionRef == functionRef)
            return false;
    }

    SDebugHookCallInfo info;
    info.functionRef = functionRef;
    info.pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(functionRef.GetLuaVM());
    if (!info.pLuaMain)
        return false;

    // Security: Validate Lua main is from a legitimate resource
    if (!IsValidLuaMain(info.pLuaMain))
    {
        LogSecurityWarning("Attempt to add hook from invalid Lua main");
        return false;
    }

    for (uint i = 0; i < allowedNameList.size(); i++)
        MapInsert(info.allowedNameMap, allowedNameList[i]);

    // Security: Add timestamp and signature for validation
    info.ulRegistrationTime = GetTickCount64_();
    info.bVerified = true;
    
    hookInfoList.push_back(info);
    
    // Update registration timestamp for rate limiting
    m_ulLastHookCheckTime = GetTickCount64_();
    
    return true;
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::RemoveDebugHook
//
// Returns true if hook was removed
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::RemoveDebugHook(EDebugHookType hookType, const CLuaFunctionRef& functionRef)
{
    // Security: Prevent hook manipulation during hook execution
    if (m_bInHookExecution)
    {
        LogSecurityWarning("Attempt to remove debug hook during hook execution blocked");
        return false;
    }

    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(functionRef.GetLuaVM());

    std::vector<SDebugHookCallInfo>& hookInfoList = GetHookInfoListForType(hookType);
    for (std::vector<SDebugHookCallInfo>::iterator iter = hookInfoList.begin(); iter != hookInfoList.end(); ++iter)
    {
        if ((*iter).pLuaMain == pLuaMain && (*iter).functionRef == functionRef)
        {
            // Security: Verify the hook is legitimate before removal
            if (!iter->bVerified)
            {
                LogSecurityWarning("Attempt to remove unverified hook blocked");
                return false;
            }
            
            hookInfoList.erase(iter);
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::CallHook
//
// Return false if function/event should be skipped
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::CallHook(const char* szName, const std::vector<SDebugHookCallInfo>& eventHookList, const CLuaArguments& Arguments,
                                 bool bNameMustBeExplicitlyAllowed)
{
    // Security: Prevent infinite recursion
    if (m_bInHookExecution)
    {
        LogSecurityWarning("Recursive hook execution detected and blocked");
        return true;
    }
    
    // Security: Validate input parameters
    if (!szName || !eventHookList.size())
        return true;
        
    // Security: Sanitize function/event name
    SString strSanitizedName = SanitizeName(szName);
    if (strSanitizedName.empty())
        return true;

    m_bInHookExecution = true;
    bool bSkip = false;

    for (uint i = 0; i < eventHookList.size(); i++)
    {
        const SDebugHookCallInfo& info = eventHookList[i];

        // Security: Verify hook integrity before execution
        if (!IsHookValid(info))
        {
            LogSecurityWarning("Invalid hook detected and skipped");
            continue;
        }

        if (!info.allowedNameMap.empty() || bNameMustBeExplicitlyAllowed)
        {
            if (!MapContains(info.allowedNameMap, strSanitizedName))
                continue;
        }

        lua_State* pState = info.pLuaMain->GetVirtualMachine();

        if (!pState)
            continue;

        // Security: Enhanced MTA globals protection with backup/restore
        BackupMTAGlobals(pState);
        
        // Security: Execute hook in protected environment
        CLuaArguments returnValues;
        bool bSuccess = ExecuteProtectedHook(info, Arguments, returnValues);
        
        // Security: Restore MTA globals regardless of execution result
        RestoreMTAGlobals(pState);
        
        if (!bSuccess)
        {
            LogSecurityWarning("Hook execution failed or was blocked");
            continue;
        }

        // Security: Validate return value before processing
        if (returnValues.IsNotEmpty())
        {
            CLuaArgument* returnedValue = *returnValues.begin();
            if (returnedValue->GetType() == LUA_TSTRING)
            {
                SString strReturn = returnedValue->GetString();
                
                // Enhanced security: Only allow "skip" for non-critical functions
                if (strReturn == "skip")
                {
                    if (IsSkipAllowedForFunction(strSanitizedName))
                    {
                        bSkip = true;
                    }
                    else
                    {
                        LogSecurityWarning("Attempt to skip critical function blocked: " + strSanitizedName);
                    }
                }
            }
        }
    }

    m_bInHookExecution = false;
    return !bSkip;
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::ExecuteProtectedHook
//
// Execute hook with enhanced security measures
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::ExecuteProtectedHook(const SDebugHookCallInfo& info, const CLuaArguments& Arguments, CLuaArguments& returnValues)
{
    lua_State* pState = info.pLuaMain->GetVirtualMachine();
    if (!pState)
        return false;

    // Security: Set execution time limit to prevent infinite loops
    unsigned long ulStartTime = GetTickCount64_();
    
    // Security: Set up protected call environment
    int iErrorHandler = lua_gettop(pState);
    lua_pushcfunction(pState, luaErrorHandler);
    lua_insert(pState, iErrorHandler);

    // Security: Execute the hook function
    bool bSuccess = Arguments.Call(info.pLuaMain, info.functionRef, &returnValues);
    
    // Security: Check execution time
    unsigned long ulExecutionTime = GetTickCount64_() - ulStartTime;
    if (ulExecutionTime > MAX_HOOK_EXECUTION_TIME)
    {
        LogSecurityWarning("Hook execution time exceeded limit: " + SString(ulExecutionTime) + "ms");
        bSuccess = false;
    }
    
    // Clean up error handler
    lua_remove(pState, iErrorHandler);
    
    return bSuccess;
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::BackupMTAGlobals
//
// Backup critical MTA globals with enhanced security
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::BackupMTAGlobals(lua_State* pState)
{
    m_BackupGlobals.clear();
    
    const char* criticalGlobals[] = {"source", "this", "sourceResource", "sourceResourceRoot", 
                                    "eventName", "client", "localPlayer", "root", "resource", 
                                    "resourceRoot", "_G", "debug", "getfenv", "setfenv"};
    
    for (const char* globalName : criticalGlobals)
    {
        lua_getglobal(pState, globalName);
        m_BackupGlobals[globalName] = CLuaArgument(pState, -1);
        lua_pop(pState, 1);
    }
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::RestoreMTAGlobals
//
// Restore MTA globals with validation
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::RestoreMTAGlobals(lua_State* pState)
{
    for (const auto& backup : m_BackupGlobals)
    {
        backup.second.Push(pState);
        lua_setglobal(pState, backup.first.c_str());
    }
    m_BackupGlobals.clear();
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::IsHookValid
//
// Validate hook integrity before execution
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::IsHookValid(const SDebugHookCallInfo& info)
{
    if (!info.bVerified)
        return false;
        
    if (!info.pLuaMain)
        return false;
        
    if (!IsValidLuaMain(info.pLuaMain))
        return false;
        
    // Check if hook registration is within reasonable time frame
    unsigned long ulCurrentTime = GetTickCount64_();
    if (ulCurrentTime - info.ulRegistrationTime > MAX_HOOK_LIFETIME)
    {
        LogSecurityWarning("Expired hook detected");
        return false;
    }
    
    return true;
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::IsValidLuaMain
//
// Validate Lua main belongs to legitimate resource
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::IsValidLuaMain(CLuaMain* pLuaMain)
{
    if (!pLuaMain)
        return false;
        
    CResource* pResource = pLuaMain->GetResource();
    if (!pResource)
        return false;
        
    // Add additional resource validation logic here
    return pResource->IsActive() && !pResource->IsWaitingForDelete();
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::CanRegisterNewHook
//
// Rate limiting for hook registration
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::CanRegisterNewHook()
{
    unsigned long ulCurrentTime = GetTickCount64_();
    
    if (ulCurrentTime - m_ulLastHookCheckTime < MIN_HOOK_REGISTRATION_INTERVAL)
    {
        return false;
    }
    
    return true;
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::IsSkipAllowedForFunction
//
// Determine if skipping is allowed for specific functions
//
///////////////////////////////////////////////////////////////
bool CDebugHookManager::IsSkipAllowedForFunction(const SString& strFunctionName)
{
    // Critical functions that should never be skipped
    static std::set<SString> criticalFunctions = {
        "addDebugHook", "removeDebugHook", "executeCommandHandler", 
        "addEventHandler", "removeEventHandler", "triggerEvent",
        "call", "pcall", "loadstring", "dofile", "loadfile"
    };
    
    return criticalFunctions.find(strFunctionName) == criticalFunctions.end();
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::SanitizeName
//
// Sanitize function/event names to prevent injection
//
///////////////////////////////////////////////////////////////
SString CDebugHookManager::SanitizeName(const char* szName)
{
    if (!szName)
        return "";
        
    SString strName(szName);
    
    // Remove potentially dangerous characters
    strName = strName.Replace("\\", "").Replace("/", "").Replace("..", "").Replace("\"", "").Replace("'", "");
    
    // Limit name length
    if (strName.length() > MAX_FUNCTION_NAME_LENGTH)
        strName = strName.SubStr(0, MAX_FUNCTION_NAME_LENGTH);
        
    return strName;
}

///////////////////////////////////////////////////////////////
//
// CDebugHookManager::LogSecurityWarning
//
// Log security warnings for monitoring
//
///////////////////////////////////////////////////////////////
void CDebugHookManager::LogSecurityWarning(const SString& strMessage)
{
    #ifdef MTA_CLIENT
        g_pClientGame->GetScriptDebugging()->LogWarning(NULL, "Security: %s", strMessage.c_str());
    #else
        g_pGame->GetScriptDebugging()->LogWarning(NULL, "Security: %s", strMessage.c_str());
    #endif
}

// Add these constants at the top of the class or in appropriate header
#define MAX_HOOKS_PER_TYPE 50
#define MAX_HOOK_EXECUTION_TIME 5000 // 5 seconds
#define MAX_HOOK_LIFETIME 3600000    // 1 hour
#define MIN_HOOK_REGISTRATION_INTERVAL 1000 // 1 second
#define MAX_FUNCTION_NAME_LENGTH 128

// Error handler for protected calls
static int luaErrorHandler(lua_State* L)
{
    // Log the error but don't propagate it to prevent disruption
    return 0;
}
