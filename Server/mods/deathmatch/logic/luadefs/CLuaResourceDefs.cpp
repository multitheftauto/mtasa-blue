/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaResourceDefs.cpp
 *  PURPOSE:     Lua resource function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaResourceDefs.h"
#include "lua/CLuaShared.h"
#include "../utils/CFunctionUseLogger.h"
#include "CAclRightName.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"
#include "CResourceConfigItem.h"
#include "CDummy.h"
#include "Utils.h"

extern CNetServer* g_pRealNetServer;

void CLuaResourceDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Create/edit functions
        {"createResource", createResource},
        {"copyResource", copyResource},
        {"renameResource", renameResource},
        {"deleteResource", deleteResource},

        {"addResourceMap", addResourceMap},
        {"addResourceConfig", addResourceConfig},
        {"removeResourceFile", removeResourceFile},

        {"setResourceDefaultSetting", setResourceDefaultSetting},
        {"removeResourceDefaultSetting", removeResourceDefaultSetting},

        // Start/stop management
        {"startResource", startResource},
        {"stopResource", stopResource},
        {"restartResource", restartResource},

        // Get stuff
        {"getThisResource", getThisResource},
        {"getResourceFromName", getResourceFromName},
        {"getResources", getResources},

        {"getResourceState", getResourceState},
        {"getResourceInfo", getResourceInfo},
        {"getResourceConfig", getResourceConfig},
        {"getResourceLoadFailureReason", getResourceLoadFailureReason},
        {"getResourceLastStartTime", getResourceLastStartTime},
        {"getResourceLoadTime", getResourceLoadTime},
        {"getResourceName", ArgumentParserWarn<false, GetResourceName>},
        {"getResourceRootElement", getResourceRootElement},
        {"getResourceDynamicElementRoot", getResourceDynamicElementRoot},
        {"getResourceMapRootElement", getResourceMapRootElement},
        {"getResourceExportedFunctions", getResourceExportedFunctions},
        {"getResourceOrganizationalPath", getResourceOrganizationalPath},
        {"isResourceArchived", isResourceArchived},
        {"isResourceProtected", ArgumentParser<isResourceProtected>},

        // Set stuff
        {"setResourceInfo", setResourceInfo},

        // Misc
        {"call", call},
        {"refreshResources", refreshResources},

        {"getResourceACLRequests", getResourceACLRequests},
        {"loadstring", LoadString},
        {"load", Load},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);

    CLuaCFunctions::AddFunction("updateResourceACLRequest", updateResourceACLRequest, true);
}

void CLuaResourceDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // These have been separated for their abnormal argument scheme
    // Their first arg take a path from which a resource is determined
    // For now, they are static-classes, and if the scheme is fixed
    // Then they will also be able to serve use when in an instance
    lua_classfunction(luaVM, "addConfig", "addResourceConfig");
    lua_classfunction(luaVM, "addMap", "addResourceMap");
    lua_classfunction(luaVM, "getConfig", "getResourceConfig");

    lua_classfunction(luaVM, "getFromName", "getResourceFromName");
    lua_classfunction(luaVM, "getAll", "getResources");
    lua_classfunction(luaVM, "getThis", "getThisResource");
    lua_classfunction(luaVM, "refresh", "refreshResources");            // Can't use "all" here because that's an argument

    lua_classfunction(luaVM, "create", "createResource");
    lua_classfunction(luaVM, "start", "startResource");
    lua_classfunction(luaVM, "stop", "stopResource");
    lua_classfunction(luaVM, "copy", "copyResource");
    lua_classfunction(luaVM, "rename", "renameResource");
    lua_classfunction(luaVM, "delete", "deleteResource");
    lua_classfunction(luaVM, "call", "call");
    lua_classfunction(luaVM, "removeDefaultSetting", "removeResourceDefaultSetting");
    lua_classfunction(luaVM, "removeFile", "removeResourceFile");
    lua_classfunction(luaVM, "restart", "restartResource");
    lua_classfunction(luaVM, "hasPermissionTo", "hasObjectPermissionTo");
    lua_classfunction(luaVM, "updateACLRequest", "updateResourceACLRequest");

    lua_classfunction(luaVM, "setInfo", "setResourceInfo");
    lua_classfunction(luaVM, "setDefaultSetting", "setResourceDefaultSetting");

    lua_classfunction(luaVM, "getDynamicElementRoot", "getResourceDynamicElementRoot");
    lua_classfunction(luaVM, "getRootElement", "getResourceRootElement");
    lua_classfunction(luaVM, "getExportedFunctions", "getResourceExportedFunctions");
    lua_classfunction(luaVM, "getOrganizationalPath", "getResourceOrganizationalPath");
    lua_classfunction(luaVM, "getLastStartTime", "getResourceLastStartTime");
    lua_classfunction(luaVM, "getLoadTime", "getResourceLoadTime");
    lua_classfunction(luaVM, "getInfo", "getResourceInfo");
    lua_classfunction(luaVM, "getLoadFailureReason", "getResourceLoadFailureReason");
    lua_classfunction(luaVM, "getMapRootElement", "getResourceMapRootElement");
    lua_classfunction(luaVM, "getName", "getResourceName");
    lua_classfunction(luaVM, "getState", "getResourceState");
    lua_classfunction(luaVM, "getACLRequests", "getResourceACLRequests");
    lua_classfunction(luaVM, "isArchived", "isResourceArchived");
    lua_classfunction(luaVM, "isProtected", "isResourceProtected");

    lua_classvariable(luaVM, "dynamicElementRoot", NULL, "getResourceDynamicElementRoot");
    lua_classvariable(luaVM, "exportedFunctions", NULL, "getResourceExportedFunctions");
    lua_classvariable(luaVM, "organizationalPath", nullptr, "getResourceOrganizationalPath");
    lua_classvariable(luaVM, "lastStartTime", NULL, "getResourceLastStartTime");
    lua_classvariable(luaVM, "aclRequests", NULL, "getResourceACLRequests");
    lua_classvariable(luaVM, "loadTime", NULL, "getResourceLoadTime");
    lua_classvariable(luaVM, "name", "renameResource", "getResourceName");
    lua_classvariable(luaVM, "rootElement", NULL, "getResourceRootElement");
    lua_classvariable(luaVM, "state", NULL, "getResourceState");
    lua_classvariable(luaVM, "archived", NULL, "isResourceArchived");
    lua_classvariable(luaVM, "protected", nullptr, "isResourceProtected");
    lua_classvariable(luaVM, "loadFailureReason", NULL, "getResourceLoadFailureReason");

    lua_registerclass(luaVM, "Resource");
}

int CLuaResourceDefs::createResource(lua_State* luaVM)
{
    //  resource createResource ( string toName[, string organizationalDir ] )
    SString strNewResourceName;
    SString strNewOrganizationalPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strNewResourceName);
    argStream.ReadString(strNewOrganizationalPath, "");

    if (!argStream.HasErrors())
    {
        SString    strStatus;
        CResource* pNewResource = m_pResourceManager->CreateResource(strNewResourceName, strNewOrganizationalPath, strStatus);

        if (!strStatus.empty())
            argStream.SetCustomError(strStatus);
        else if (pNewResource)
        {
            lua_pushresource(luaVM, pNewResource);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::copyResource(lua_State* luaVM)
{
    //  resource copyResource ( mixed fromResource, string toName[, string organizationalDir ] )
    CResource* pResource;
    SString    strNewResourceName;
    SString    strNewOrganizationalPath;

    CScriptArgReader argStream(luaVM);
    MixedReadResourceString(argStream, pResource);
    argStream.ReadString(strNewResourceName);
    argStream.ReadString(strNewOrganizationalPath, "");

    if (!argStream.HasErrors())
    {
        SString    strStatus;
        CResource* pNewResource = m_pResourceManager->CopyResource(pResource, strNewResourceName, strNewOrganizationalPath, strStatus);

        if (!strStatus.empty())
            argStream.SetCustomError(strStatus);
        else if (pNewResource)
        {
            lua_pushresource(luaVM, pNewResource);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::renameResource(lua_State* luaVM)
{
    //  resource renameResource ( mixed fromResource, string newName[, string organizationalDir ] )
    CResource* pResource;
    SString    strNewResourceName;
    SString    strNewOrganizationalPath;

    CScriptArgReader argStream(luaVM);
    MixedReadResourceString(argStream, pResource);
    argStream.ReadString(strNewResourceName);
    argStream.ReadString(strNewOrganizationalPath, "");

    if (!argStream.HasErrors())
    {
        SString    strStatus;
        CResource* pNewResource = m_pResourceManager->RenameResource(pResource, strNewResourceName, strNewOrganizationalPath, strStatus);

        if (!strStatus.empty())
            argStream.SetCustomError(strStatus);
        else if (pNewResource)
        {
            lua_pushresource(luaVM, pNewResource);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::deleteResource(lua_State* luaVM)
{
    //  bool deleteResource ( mixed resource )
    SString strResourceName;

    CScriptArgReader argStream(luaVM);
    MixedReadResourceString(argStream, strResourceName);

    if (!argStream.HasErrors())
    {
        SString strStatus;
        bool    bResult = m_pResourceManager->DeleteResource(strResourceName, strStatus);

        if (!strStatus.empty())
            argStream.SetCustomError(strStatus);
        else
        {
            lua_pushboolean(luaVM, bResult);
            return 1;
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::addResourceMap(lua_State* luaVM)
{
    if (lua_type(luaVM, 1) == LUA_TLIGHTUSERDATA)
        m_pScriptDebugging->LogCustom(luaVM, "addResourceMap may be using an outdated syntax. Please check and update.");

    SString        strMapName;
    unsigned short usDimension;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strMapName);
    argStream.ReadNumber(usDimension, 0);

    if (!argStream.HasErrors())
    {
        // Grab our LUA instance
        CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLUA)
        {
            // Read out the resource and make sure it exists
            CResource* pResource = pLUA->GetResource();
            CResource* pThisResource = pResource;
            if (pResource)
            {
                // Grab the mapname string
                SString strPath;
                SString strMetaName;

                if (CResourceManager::ParseResourcePathInput(strMapName, pResource, &strPath, NULL))
                {
                    CheckCanModifyOtherResource(argStream, pThisResource, pResource);
                    if (!argStream.HasErrors())
                    {
                        // Add the resource map and return it if we succeeded
                        CXMLNode* pXMLNode = CStaticFunctionDefinitions::AddResourceMap(pResource, strPath, strMetaName, usDimension, pLUA);
                        if (pXMLNode)
                        {
                            lua_pushxmlnode(luaVM, pXMLNode);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::addResourceConfig(lua_State* luaVM)
{
    if (lua_type(luaVM, 1) == LUA_TLIGHTUSERDATA)
        m_pScriptDebugging->LogCustom(luaVM, "addResourceConfig may be using an outdated syntax. Please check and update.");

    SString                      strMapName, strType;
    CResourceFile::eResourceType iType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strMapName);
    argStream.ReadString(strType, "server");

    if (strType == "client")
        iType = CResourceFile::RESOURCE_FILE_TYPE_CLIENT_CONFIG;
    else
    {
        iType = CResourceFile::RESOURCE_FILE_TYPE_CONFIG;
        if (strType != "server")
        {
            CLogger::LogPrintf("WARNING: Unknown config file type specified for %s. Defaulting to 'server'", lua_tostring(luaVM, lua_upvalueindex(1)));
        }
    }

    if (!argStream.HasErrors())
    {
        // Grab our LUA instance
        CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLUA)
        {
            // Read out the resource and make sure it exists
            CResource* pResource = pLUA->GetResource();
            CResource* pThisResource = pResource;
            if (pResource)
            {
                // Grab the mapname string
                std::string strPath;
                std::string strConfigName;

                if (CResourceManager::ParseResourcePathInput(strMapName, pResource, &strPath, &strConfigName))
                {
                    CheckCanModifyOtherResource(argStream, pThisResource, pResource);
                    if (!argStream.HasErrors())
                    {
                        // Add the resource map and return it if we succeeded
                        CXMLNode* pXMLNode = CStaticFunctionDefinitions::AddResourceConfig(pResource, strPath, strConfigName, iType, pLUA);
                        if (pXMLNode)
                        {
                            lua_pushxmlnode(luaVM, pXMLNode);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::removeResourceFile(lua_State* luaVM)
{
    CResource* pOtherResource;
    SString    strFileName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pOtherResource);
    argStream.ReadString(strFileName);

    if (!argStream.HasErrors())
    {
        CLuaMain*  pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        CResource* pThisResource = pLuaMain ? pLuaMain->GetResource() : nullptr;
        if (pThisResource)
        {
            CheckCanModifyOtherResource(argStream, pThisResource, pOtherResource);
            CheckCanAccessOtherResourceFile(argStream, pThisResource, pOtherResource, strFileName);
            if (!argStream.HasErrors())
            {
                if (CStaticFunctionDefinitions::RemoveResourceFile(pOtherResource, strFileName))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::setResourceDefaultSetting(lua_State* luaVM)
{
    CResource* pResource;
    SString    strSettingName, strSettingValue;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadString(strSettingName);
    argStream.ReadString(strSettingValue);

    if (!argStream.HasErrors())
    {
        if (pResource->SetDefaultSetting(strSettingName, strSettingValue))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::removeResourceDefaultSetting(lua_State* luaVM)
{
    CResource* pResource;
    SString    strSettingName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadString(strSettingName);

    if (!argStream.HasErrors())
    {
        if (pResource->RemoveDefaultSetting(strSettingName))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::startResource(lua_State* luaVM)
{
    CResource*            pResource = nullptr;
    bool                  bPersistent = false;
    SResourceStartOptions StartOptions;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadBool(bPersistent, false);
    argStream.ReadBool(StartOptions.bIncludedResources, true);
    argStream.ReadBool(StartOptions.bConfigs, true);
    argStream.ReadBool(StartOptions.bMaps, true);
    argStream.ReadBool(StartOptions.bScripts, true);
    argStream.ReadBool(StartOptions.bHTML, true);
    argStream.ReadBool(StartOptions.bClientConfigs, true);
    argStream.ReadBool(StartOptions.bClientScripts, true);
    argStream.ReadBool(StartOptions.bClientFiles, true);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    const SString& strResourceName = pResource->GetName();

    if (pResource->IsActive())
    {
        // The resource is either starting, running or stopping:
        // In the event 'onResourcePreStart' the resource will be in the 'starting' state
        // and in the event 'onResourceStart' the resource is already in the 'running' state
        // and you can't force-start resources in the 'stopping' state
        lua_pushboolean(luaVM, false);
    }
    else if (pResource->IsLoaded())
    {
        if (!m_pResourceManager->StartResource(pResource, nullptr, bPersistent, StartOptions))
        {
            CLogger::LogPrintf("%s: Failed to start resource '%s'\n", lua_tostring(luaVM, lua_upvalueindex(1)), strResourceName.c_str());
            lua_pushboolean(luaVM, false);
            return 1;
        }

        if (pResource->IsActive())
        {
            // If the resource is persistent, set that flag
            pResource->SetPersistent(bPersistent);

            if (!bPersistent)
            {
                // Add the new resource to the list of included resources so that when
                // we unload this resource, the new resource goes with it
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

                if (pLuaMain)
                {
                    CResource* pThisResource = pLuaMain->GetResource();

                    if (pThisResource)
                    {
                        pThisResource->AddTemporaryInclude(pResource);
                        // Make sure the new resource is dependent on this one
                        pResource->AddDependent(pThisResource);
                    }
                }
            }

            CLogger::LogPrintf("%s: Resource '%s' started\n", lua_tostring(luaVM, lua_upvalueindex(1)), pResource->GetName().c_str());
            lua_pushboolean(luaVM, true);
        }
        else
        {
            m_pScriptDebugging->LogWarning(luaVM, "Failed to start resource '%s'", strResourceName.c_str());
            lua_pushboolean(luaVM, false);
        }
    }
    else
    {
        // Resource has loading issues
        m_pScriptDebugging->LogWarning(luaVM, "Failed to start resource '%s': %s", strResourceName.c_str(), pResource->GetFailureReason().c_str());
        lua_pushboolean(luaVM, false);
    }

    return 1;
}

int CLuaResourceDefs::stopResource(lua_State* luaVM)
{
    CResource* pResource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    if (pResource->IsActive() && !pResource->IsStopping())
    {
        if (pResource->IsProtected())
        {
            CResource* pThisResource = m_pLuaManager->GetVirtualMachineResource(luaVM);

            if (!pThisResource || !m_pACLManager->CanObjectUseRight(pThisResource->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                                    "stopResource.protected", CAccessControlListRight::RIGHT_TYPE_FUNCTION, false))
            {
                m_pScriptDebugging->LogError(luaVM, "%s: Resource could not be stopped as it is protected", lua_tostring(luaVM, lua_upvalueindex(1)));
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }

        // Schedule it for a stop
        m_pResourceManager->QueueResource(pResource, CResourceManager::QUEUE_STOP, nullptr);
        lua_pushboolean(luaVM, true);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::restartResource(lua_State* luaVM)
{
    CResource*            pResource = nullptr;
    bool                  bPersistent = false;            // unused
    SResourceStartOptions StartOptions;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadBool(bPersistent, false);
    argStream.ReadBool(StartOptions.bConfigs, true);
    argStream.ReadBool(StartOptions.bMaps, true);
    argStream.ReadBool(StartOptions.bScripts, true);
    argStream.ReadBool(StartOptions.bHTML, true);
    argStream.ReadBool(StartOptions.bClientConfigs, true);
    argStream.ReadBool(StartOptions.bClientScripts, true);
    argStream.ReadBool(StartOptions.bClientFiles, true);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    if (pResource->IsActive() && !pResource->IsStopping())
    {
        m_pResourceManager->QueueResource(pResource, CResourceManager::QUEUE_RESTART, &StartOptions);
        lua_pushboolean(luaVM, true);
    }
    else
    {
        m_pScriptDebugging->LogWarning(luaVM, "Attempt to restart a stopped resource");
        lua_pushboolean(luaVM, false);
    }

    return 1;
}

int CLuaResourceDefs::getThisResource(lua_State* luaVM)
{
    CLuaMain* amain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (amain)
    {
        CResource* thisResource = amain->GetResource();
        lua_pushresource(luaVM, thisResource);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceFromName(lua_State* luaVM)
{
    SString strName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);

    if (!argStream.HasErrors())
    {
        CResource* pResource = m_pResourceManager->GetResource(strName);
        if (pResource)
        {
            lua_pushresource(luaVM, pResource);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResources(lua_State* luaVM)
{
    unsigned int uiIndex = 0;
    lua_newtable(luaVM);
    list<CResource*>::const_iterator iter = m_pResourceManager->IterBegin();
    for (; iter != m_pResourceManager->IterEnd(); ++iter)
    {
        lua_pushnumber(luaVM, ++uiIndex);
        lua_pushresource(luaVM, *iter);
        lua_settable(luaVM, -3);
    }
    return 1;
}

int CLuaResourceDefs::getResourceState(lua_State* luaVM)
{
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        if (pResource->IsStarting())
            lua_pushstring(luaVM, "starting");
        else if (pResource->IsStopping())
            lua_pushstring(luaVM, "stopping");
        else if (pResource->IsActive())
            lua_pushstring(luaVM, "running");
        else if (pResource->IsLoaded())
            lua_pushstring(luaVM, "loaded");
        else
            lua_pushstring(luaVM, "failed to load");
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceInfo(lua_State* luaVM)
{
    CResource* pResource;
    SString    strInfoValueKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadString(strInfoValueKey);

    if (!argStream.HasErrors())
    {
        if (pResource->IsLoaded())
        {
            SString strInfoValue;
            pResource->GetInfoValue(strInfoValueKey, strInfoValue);

            if (!strInfoValue.empty())
            {
                lua_pushstring(luaVM, strInfoValue);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::setResourceInfo(lua_State* luaVM)
{
    CResource*  pResource;
    SString     strInfoValueKey;
    SString     strTempInfoValue;
    const char* szInfoValue = NULL;
    bool        bSave;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadString(strInfoValueKey);

    if (argStream.NextIsString())
    {
        argStream.ReadString(strTempInfoValue);
        szInfoValue = strTempInfoValue.c_str();
    }

    argStream.ReadBool(bSave, true);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pThisResource = pLuaMain->GetResource();
            if (pResource)
            {
                CheckCanModifyOtherResource(argStream, pThisResource, pResource);
                if (!argStream.HasErrors())
                {
                    if (pResource->IsLoaded())
                    {
                        pResource->SetInfoValue(strInfoValueKey, szInfoValue, bSave);

                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                }
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceConfig(lua_State* luaVM)
{
    SString strConfigName, strMetaPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strConfigName);

    if (!argStream.HasErrors())
    {
        CResource* pThisResource = m_pLuaManager->GetVirtualMachineResource(luaVM);
        CResource* pResource = pThisResource;

        if (pThisResource && CResourceManager::ParseResourcePathInput(strConfigName, pResource, NULL, &strMetaPath))
        {
            CheckCanModifyOtherResource(argStream, pThisResource, pResource);
            if (!argStream.HasErrors())
            {
                for (CResourceFile* pResourceFile : pResource->GetFiles())
                {
                    if (pResourceFile->GetType() != CResourceFile::RESOURCE_FILE_TYPE_CONFIG)
                        continue;

                    if (strcmp(pResourceFile->GetName(), strMetaPath.c_str()) != 0)
                        continue;

                    auto      pConfigItem = static_cast<CResourceConfigItem*>(pResourceFile);
                    CXMLNode* pRootNode = pConfigItem->GetRoot();

                    if (!pRootNode)
                        continue;

                    lua_pushxmlnode(luaVM, pRootNode);
                    return 1;
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceLoadFailureReason(lua_State* luaVM)
{
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        SString strFailureReason = pResource->GetFailureReason();
        lua_pushstring(luaVM, strFailureReason);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceLastStartTime(lua_State* luaVM)
{
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        time_t timestarted = pResource->GetTimeStarted();
        if (timestarted)
            lua_pushnumber(luaVM, (double)timestarted);
        else
            lua_pushstring(luaVM, "never");
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceLoadTime(lua_State* luaVM)
{
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        time_t timeloaded = pResource->GetTimeLoaded();
        if (timeloaded)
            lua_pushnumber(luaVM, (double)timeloaded);
        else
            lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

std::string CLuaResourceDefs::GetResourceName(lua_State* luaVM, std::optional<CResource*> resourceElement)
{
    if (resourceElement.has_value())
        return (*resourceElement)->GetName();

    return lua_getownerresource(luaVM).GetName();
}

int CLuaResourceDefs::getResourceRootElement(lua_State* luaVM)
{
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource, NULL);

    if (!argStream.HasErrors())
    {
        if (!pResource)
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                pResource = pLuaMain->GetResource();
            }

            // No Lua VM or no assigned resource?
            if (!pResource)
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }

        if (pResource->IsActive())
        {
            // Grab the root element of it and return it if it existed
            CElement* pElement = pResource->GetResourceRootElement();
            if (pElement)
            {
                lua_pushelement(luaVM, pElement);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceDynamicElementRoot(lua_State* luaVM)
{
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource, NULL);

    if (!argStream.HasErrors())
    {
        if (!pResource)
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                pResource = pLuaMain->GetResource();
            }

            // No Lua VM or no assigned resource?
            if (!pResource)
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }

        if (pResource->IsActive())
        {
            CElement* pElement = pResource->GetDynamicElementRoot();
            if (pElement)
            {
                lua_pushelement(luaVM, pElement);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError(luaVM, "%s: Resource %s is not currently running", lua_tostring(luaVM, lua_upvalueindex(1)),
                                         pResource->GetName().c_str());
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceMapRootElement(lua_State* luaVM)
{
    CResource* pResource;
    SString    strMapName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource, NULL);
    argStream.ReadString(strMapName);

    if (!argStream.HasErrors())
    {
        if (pResource->IsActive())
        {
            CElement* pMapRoot = CStaticFunctionDefinitions::GetResourceMapRootElement(pResource, strMapName);
            if (pMapRoot)
            {
                lua_pushelement(luaVM, pMapRoot);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogError(luaVM, "%s: Resource %s is not currently running", lua_tostring(luaVM, lua_upvalueindex(1)),
                                         pResource->GetName().c_str());
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceExportedFunctions(lua_State* luaVM)
{
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource, NULL);

    if (!argStream.HasErrors())
    {
        if (!pResource)
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                pResource = pLuaMain->GetResource();
            }

            // No Lua VM or no assigned resource?
            if (!pResource)
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }

        lua_newtable(luaVM);
        unsigned int                      uiIndex = 0;
        list<CExportedFunction>::iterator iterd = pResource->IterBeginExportedFunctions();
        for (; iterd != pResource->IterEndExportedFunctions(); ++iterd)
        {
            lua_pushnumber(luaVM, ++uiIndex);
            lua_pushstring(luaVM, iterd->GetFunctionName().c_str());
            lua_settable(luaVM, -3);
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceOrganizationalPath(lua_State* luaVM)
{
    // string getResourceOrganizationalPath ( resource theResource )
    // Returns a string representing the resource organizational path, false if invalid resource was provided.

    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        SString strOrganizationalPath = m_pResourceManager->GetResourceOrganizationalPath(pResource);

        if (!strOrganizationalPath.empty())
        {
            // Normalize path separator, it is always slash on resources side
            ReplaceOccurrencesInString(strOrganizationalPath, "\\", "/");

            // The leading separator won't be needed
            strOrganizationalPath = strOrganizationalPath.TrimStart("/");
        }

        lua_pushstring(luaVM, strOrganizationalPath);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::call(lua_State* luaVM)
{
    CResource* pResource;
    SString    strFunctionName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadString(strFunctionName);

    if (!argStream.HasErrors())
    {
        // Is this an internal function that's restricted? To make sure you can't
        // call an MTA function in an external resource that's restricted and not
        // defined in ACL.
        bool           bRestricted = false;
        CLuaCFunction* pFunction = CLuaCFunctions::GetFunction(strFunctionName);
        if (pFunction)
            bRestricted = pFunction->IsRestricted();

        // Check this resource can use the function call to the called resource
        if (!CanUseFunction(strFunctionName, luaVM, bRestricted))
        {
            lua_pushboolean(luaVM, false);
            return 1;
        }

        if (pResource->IsActive())
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                // Get the target Lua VM
                lua_State* targetLuaVM = pResource->GetVirtualMachine()->GetVirtualMachine();

                CResource*    resourceThis = pLuaMain->GetResource();
                CLuaArguments args;
                args.ReadArguments(luaVM, 3);
                CLuaArguments returns;

                LUA_CHECKSTACK(targetLuaVM, 1);            // Ensure some room

                // Lets grab the original hidden variables so we can restore them later
                lua_getglobal(targetLuaVM, "sourceResource");
                CLuaArgument OldResource(luaVM, -1);
                lua_pop(targetLuaVM, 1);

                lua_getglobal(targetLuaVM, "sourceResourceRoot");
                CLuaArgument OldResourceRoot(luaVM, -1);
                lua_pop(targetLuaVM, 1);

                // Set the new values for the current sourceResource, and sourceResourceRoot
                lua_pushresource(targetLuaVM, resourceThis);
                lua_setglobal(targetLuaVM, "sourceResource");

                lua_pushelement(targetLuaVM, resourceThis->GetResourceRootElement());
                lua_setglobal(targetLuaVM, "sourceResourceRoot");

                if (pResource->CallExportedFunction(strFunctionName, args, returns, *resourceThis))
                {
                    returns.PushArguments(luaVM);
                    // Restore the old variables
                    OldResource.Push(targetLuaVM);
                    lua_setglobal(targetLuaVM, "sourceResource");
                    OldResourceRoot.Push(targetLuaVM);
                    lua_setglobal(targetLuaVM, "sourceResourceRoot");

                    return static_cast<int>(returns.Count());
                }
                else
                {
                    // Restore the old variables
                    OldResource.Push(targetLuaVM);
                    lua_setglobal(targetLuaVM, "sourceResource");
                    OldResourceRoot.Push(targetLuaVM);
                    lua_setglobal(targetLuaVM, "sourceResourceRoot");

                    m_pScriptDebugging->LogError(luaVM, "%s: failed to call '%s:%s'", lua_tostring(luaVM, lua_upvalueindex(1)), pResource->GetName().c_str(),
                                                 *strFunctionName);
                }
            }
        }
        else
            m_pScriptDebugging->LogError(luaVM, "%s: Failed, the resource %s isn't running", lua_tostring(luaVM, lua_upvalueindex(1)),
                                         pResource->GetName().c_str());
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::refreshResources(lua_State* luaVM)
{
    //  bool refreshResources ( [ bool refreshAll = false, resource onlyThisResource = nil ] )
    bool       bRefreshAll;
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bRefreshAll, false);
    argStream.ReadUserData(pResource, nullptr);

    if (!argStream.HasErrors())
    {
        if (bRefreshAll)
            m_pResourceManager->QueueResource(pResource, CResourceManager::QUEUE_REFRESHALL, NULL);
        else
            m_pResourceManager->QueueResource(pResource, CResourceManager::QUEUE_REFRESH, NULL);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceACLRequests(lua_State* luaVM)
{
    //  table getResourceACLRequests ( resource theResource )
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        std::vector<SAclRequest> Result;
        pResource->GetAclRequests(Result);

        // Make table!
        lua_newtable(luaVM);
        for (uint i = 0; i < Result.size(); i++)
        {
            lua_newtable(luaVM);                     // new table
            lua_pushnumber(luaVM, i + 1);            // row index number (starting at 1, not 0)
            lua_pushvalue(luaVM, -2);                // value
            lua_settable(luaVM, -4);                 // refer to the top level table

            const SAclRequest& request = Result[i];
            lua_pushstring(luaVM, "name");
            lua_pushstring(luaVM, request.rightName.GetFullName());
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "access");
            lua_pushboolean(luaVM, request.bAccess);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "pending");
            lua_pushboolean(luaVM, request.bPending);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "who");
            lua_pushstring(luaVM, request.strWho);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "date");
            lua_pushstring(luaVM, request.strDate);
            lua_settable(luaVM, -3);

            lua_pop(luaVM, 1);            // pop the inner table
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::updateResourceACLRequest(lua_State* luaVM)
{
    //  bool updateResourceACLRequest ( resource theResource, string rightName, bool access, string byWho )
    CResource* pResource;
    SString    strRightName;
    bool       bAccess;
    SString    strUserName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadString(strRightName);
    argStream.ReadBool(bAccess);
    argStream.ReadString(strUserName, "");

    if (!argStream.HasErrors())
    {
        CResource* pThisResource = m_pLuaManager->GetVirtualMachineResource(luaVM);
        if (strUserName.empty() && pThisResource)
            strUserName = pThisResource->GetName();

        if (pResource->HandleAclRequestChange(CAclRightName(strRightName), bAccess, strUserName))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::LoadString(lua_State* luaVM)
{
    //  func,err loadstring( string text[, string name] )
    SString strInput;
    SString strName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInput);
    argStream.ReadString(strName, "");

    if (!argStream.HasErrors())
    {
        const char* szChunkname = strName.empty() ? *strInput : *strName;
        const char* cpInBuffer = strInput;
        uint        uiInSize = strInput.length();

        // Deobfuscate if required
        const char* cpBuffer;
        uint        uiSize;
        if (!g_pRealNetServer->DeobfuscateScript(cpInBuffer, uiInSize, &cpBuffer, &uiSize, m_pResourceManager->GetResourceName(luaVM) + "/loadstring"))
        {
            SString strMessage("argument 1 is invalid. Please re-compile at http://luac.mtasa.com/", 0);
            argStream.SetCustomError(strMessage);
            cpBuffer = NULL;
        }

        if (!argStream.HasErrors())
        {
            CLuaShared::CheckUTF8BOMAndUpdate(&cpBuffer, &uiSize);
            if (!CLuaMain::LuaLoadBuffer(luaVM, cpBuffer, uiSize, szChunkname))
            {
                // Ok
                if (g_pGame->GetConfig()->GetLoadstringLogEnabled())
                    g_pGame->GetFunctionUseLogger()->OnFunctionUse(luaVM, "loadstring", cpBuffer, uiSize);
                return 1;
            }
            else
            {
                lua_pushnil(luaVM);
                lua_insert(luaVM, -2); /* put before error message */
                return 2;              /* return nil plus error message */
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::Load(lua_State* luaVM)
{
    //  func,err load( callback callbackFunction[, string name] )
    CLuaFunctionRef iLuaFunction;
    SString         strName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadFunction(iLuaFunction);
    argStream.ReadString(strName, "=(load)");
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Call supplied function to get all the bits
        // Should apply some limit here?
        SString       strInput;
        CLuaArguments callbackArguments;
        CLuaMain*     pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        while (pLuaMain)
        {
            CLuaArguments returnValues;
            callbackArguments.Call(pLuaMain, iLuaFunction, &returnValues);
            if (returnValues.IsNotEmpty())
            {
                CLuaArgument* returnedValue = *returnValues.begin();
                int           iType = returnedValue->GetType();
                if (iType == LUA_TNIL)
                    break;

                else if (iType == LUA_TSTRING)
                {
                    std::string str = returnedValue->GetString();
                    if (str.length() == 0)
                        break;

                    strInput += str;
                    continue;
                }
            }
            break;
        }

        const char* szChunkname = *strName;
        const char* cpInBuffer = strInput;
        uint        uiInSize = strInput.length();

        // Deobfuscate if required
        const char* cpBuffer;
        uint        uiSize;
        if (!g_pRealNetServer->DeobfuscateScript(cpInBuffer, uiInSize, &cpBuffer, &uiSize, m_pResourceManager->GetResourceName(luaVM) + "/load"))
        {
            SString strMessage("argument 2 is invalid. Please re-compile at http://luac.mtasa.com/", 0);
            argStream.SetCustomError(strMessage);
            cpBuffer = NULL;
        }

        if (!argStream.HasErrors())
        {
            CLuaShared::CheckUTF8BOMAndUpdate(&cpBuffer, &uiSize);
            if (!CLuaMain::LuaLoadBuffer(luaVM, cpBuffer, uiSize, szChunkname))
            {
                // Ok
                if (g_pGame->GetConfig()->GetLoadstringLogEnabled())
                    g_pGame->GetFunctionUseLogger()->OnFunctionUse(luaVM, "load", cpBuffer, uiSize);
                return 1;
            }
            else
            {
                lua_pushnil(luaVM);
                lua_insert(luaVM, -2); /* put before error message */
                return 2;              /* return nil plus error message */
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::isResourceArchived(lua_State* luaVM)
{
    //  bool isResourceArchived ( resource theResource )
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pResource->IsResourceZip());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

bool CLuaResourceDefs::isResourceProtected(CResource* const resource)
{
    return resource->IsProtected();
}
