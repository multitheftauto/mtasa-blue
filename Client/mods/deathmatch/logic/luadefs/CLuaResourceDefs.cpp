/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaResourceDefs.cpp
 *  PURPOSE:     Lua resource definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

using std::list;

void CLuaResourceDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"call", Call},
        {"getThisResource", GetThisResource},
        {"getResourceConfig", GetResourceConfig},
        {"getResourceName", ArgumentParserWarn<false, GetResourceName>},
        {"getResourceFromName", GetResourceFromName},
        {"getResourceRootElement", GetResourceRootElement},
        {"getResourceGUIElement", GetResourceGUIElement},
        {"getResourceDynamicElementRoot", GetResourceDynamicElementRoot},
        {"getResourceExportedFunctions", GetResourceExportedFunctions},
        {"getResourceState", GetResourceState},
        {"loadstring", LoadString},
        {"load", Load},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaResourceDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "getResourceFromName");
    lua_classfunction(luaVM, "getFromName", "getResourceFromName");

    lua_classfunction(luaVM, "getGuiElement", "getResourceGUIElement");
    lua_classfunction(luaVM, "getRootElement", "getResourceRootElement");
    lua_classfunction(luaVM, "getName", "getResourceName");
    lua_classfunction(luaVM, "getThis", "getThisResource");
    lua_classfunction(luaVM, "getConfig", "getResourceConfig");
    lua_classfunction(luaVM, "getConfig", "getResourceConfig");
    lua_classfunction(luaVM, "getDynamicElementRoot", "getResourceDynamicElementRoot");
    lua_classfunction(luaVM, "getExportedFunctions", "getResourceExportedFunctions");
    lua_classfunction(luaVM, "getState", "getResourceState");

    lua_classvariable(luaVM, "config", NULL, "getResourceConfig");
    lua_classvariable(luaVM, "dynamicElementRoot", NULL, "getResourceDynamicElementRoot");
    lua_classvariable(luaVM, "exportedFunctions", NULL, "getResourceExportedFunctions");
    lua_classvariable(luaVM, "guiElement", NULL, "getResourceGUIElement");
    lua_classvariable(luaVM, "state", NULL, "getResourceState");
    lua_classvariable(luaVM, "name", NULL, "getResourceName");
    lua_classvariable(luaVM, "rootElement", NULL, "getResourceRootElement");

    lua_registerclass(luaVM, "Resource");
}

int CLuaResourceDefs::Call(lua_State* luaVM)
{
    CResource*       pResource = NULL;
    SString          strFunctionName = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);
    argStream.ReadString(strFunctionName);
    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Grab this resource
            CResource* pThisResource = pLuaMain->GetResource();
            if (pThisResource)
            {
                // Get the target Lua VM
                lua_State* targetLuaVM = pResource->GetVM()->GetVM();

                // Read out the vargs
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
                lua_pushresource(targetLuaVM, pThisResource);
                lua_setglobal(targetLuaVM, "sourceResource");

                lua_pushelement(targetLuaVM, pThisResource->GetResourceEntity());
                lua_setglobal(targetLuaVM, "sourceResourceRoot");

                // Call the exported function with the given name and the args
                if (pResource->CallExportedFunction(strFunctionName, args, returns, *pThisResource))
                {
                    // Push return arguments
                    returns.PushArguments(luaVM);
                    // Restore the old variables
                    OldResource.Push(targetLuaVM);
                    lua_setglobal(targetLuaVM, "sourceResource");

                    OldResourceRoot.Push(targetLuaVM);
                    lua_setglobal(targetLuaVM, "sourceResourceRoot");

                    return returns.Count();
                }
                else
                {
                    // Restore the old variables
                    OldResource.Push(targetLuaVM);
                    lua_setglobal(targetLuaVM, "sourceResource");

                    OldResourceRoot.Push(targetLuaVM);
                    lua_setglobal(targetLuaVM, "sourceResourceRoot");
                    m_pScriptDebugging->LogError(luaVM, "call: failed to call '%s:%s'", pResource->GetName(), *strFunctionName);
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::GetThisResource(lua_State* luaVM)
{
    // Grab our virtual machine
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        // Return the resource if any
        CResource* pThisResource = pLuaMain->GetResource();
        if (pThisResource)
        {
            lua_pushresource(luaVM, pThisResource);
            return 1;
        }
    }

    // No this resource (heh that'd be strange)
    // Indeed it would. 08/04/2013
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::GetResourceConfig(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserData())
        m_pScriptDebugging->LogCustom(luaVM, "getResourceConfig may be using an outdated syntax. Please check and update.");

    // Resource and config name
    CResource* pResource = NULL;
    SString    strInput;
    SString    strAbsPath;
    SString    strMetaPath;

    argStream.ReadString(strInput);

    if (!argStream.HasErrors())
    {
        // Grab our lua main
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Grab resource and the config name from arg
            pResource = pLuaMain->GetResource();

            // We have both a resource file to grab the config from and a config name?
            if (pResource)
            {
                if (CResourceManager::ParseResourcePathInput(strInput, pResource, &strAbsPath, &strMetaPath))
                {
                    // Loop through the configs in that resource
                    list<CResourceConfigItem*>::iterator iter = pResource->ConfigIterBegin();
                    for (; iter != pResource->ConfigIterEnd(); iter++)
                    {
                        // Matching name?
                        if (strcmp((*iter)->GetShortName(), strMetaPath.c_str()) == 0)
                        {
                            // Return it
                            CResourceConfigItem* pConfig = (CResourceConfigItem*)(*iter);
                            CXMLNode*            pNode = pConfig->GetRoot();
                            if (pNode)
                            {
                                lua_pushxmlnode(luaVM, pNode);
                                return 1;
                            }
                        }
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadType(luaVM);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

std::string CLuaResourceDefs::GetResourceName(lua_State* luaVM, std::optional<CResource*> resourceElement)
{
    if (resourceElement.has_value())
        return (*resourceElement)->GetName();

    return lua_getownerresource(luaVM).GetName();
}

int CLuaResourceDefs::GetResourceFromName(lua_State* luaVM)
{
    // Verify arguments
    SString          strResourceName = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strResourceName);

    if (!argStream.HasErrors())
    {
        // Try to find a resource with that name
        CResource* pResource = m_pResourceManager->GetResource(strResourceName);
        if (pResource)
        {
            lua_pushresource(luaVM, pResource);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::GetResourceRootElement(lua_State* luaVM)
{
    // Verify arguments
    CResource*       pResource = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource, NULL);

    // No resource given, get this resource's root
    if (!argStream.HasErrors())
    {
        if (!pResource)
        {
            // Find our vm and get the root
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                pResource = pLuaMain->GetResource();
            }
        }

        // Did we find a resource?
        if (pResource)
        {
            // Grab the root element of it and return it if it existed
            CClientEntity* pEntity = pResource->GetResourceEntity();
            if (pEntity)
            {
                lua_pushelement(luaVM, pEntity);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::GetResourceGUIElement(lua_State* luaVM)
{
    // Verify arguments
    CResource*       pResource = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource, NULL);

    // No resource given, get this resource's root
    if (pResource == NULL)
    {
        // Find our vm and get the root
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            pResource = pLuaMain->GetResource();
        }
    }

    // Did we get a resource?
    if (pResource)
    {
        // Grab the gui entity. If it exists, return it
        CClientEntity* pEntity = pResource->GetResourceGUIEntity();
        if (pEntity)
        {
            lua_pushelement(luaVM, pEntity);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::GetResourceDynamicElementRoot(lua_State* luaVM)
{
    CResource*       pResource = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        CClientEntity* pEntity = pResource->GetResourceDynamicEntity();
        if (pEntity)
        {
            lua_pushelement(luaVM, pEntity);
            return 1;
        }
        else
            m_pScriptDebugging->LogError(luaVM, "getResourceDynamicElementRoot: Resource %s Is Not Currently Running", pResource->GetName());
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::GetResourceExportedFunctions(lua_State* luaVM)
{
    CResource*       pResource = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource, NULL);

    // No resource given, get this resource's root
    if (pResource == NULL)
    {
        // Find our vm and get the root
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            pResource = pLuaMain->GetResource();
        }
    }

    // Push all functions into a key-value pair tableif (pResource)
    {
        lua_createtable(luaVM, 0, pResource->GetExportedFunctions().size());

        unsigned int index = 1;
        for (const auto& strName : pResource->GetExportedFunctions())
        {
            lua_pushstring(luaVM, strName.c_str());
            lua_rawseti(luaVM, -2, index++);
        }
        return 1;
    }

    m_pScriptDebugging->LogBadType(luaVM);
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::GetResourceState(lua_State* luaVM)
{
    //  string getResourceState ( resource theResource )
    CResource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, pResource->GetState());
        return 1;
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
        if (!g_pNet->DeobfuscateScript(cpInBuffer, uiInSize, &cpBuffer, &uiSize, m_pResourceManager->GetResourceName(luaVM) + "/loadstring"))
        {
            SString strMessage("argument 1 is invalid. Please re-compile at http://luac.mtasa.com/", 0);
            argStream.SetCustomError(strMessage);
            cpBuffer = NULL;
            g_pClientGame->TellServerSomethingImportant(1004, argStream.GetFullErrorMessage(), 3);
        }

        if (!argStream.HasErrors())
        {
            CLuaShared::CheckUTF8BOMAndUpdate(&cpBuffer, &uiSize);
            if (!CLuaMain::LuaLoadBuffer(luaVM, cpBuffer, uiSize, szChunkname))
            {
                // Ok
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
        if (!g_pNet->DeobfuscateScript(cpInBuffer, uiInSize, &cpBuffer, &uiSize, m_pResourceManager->GetResourceName(luaVM) + "/load"))
        {
            SString strMessage("argument 2 is invalid. Please re-compile at http://luac.mtasa.com/", 0);
            argStream.SetCustomError(strMessage);
            cpBuffer = NULL;
            g_pClientGame->TellServerSomethingImportant(1005, argStream.GetFullErrorMessage(), 3);
        }

        if (!argStream.HasErrors())
        {
            CLuaShared::CheckUTF8BOMAndUpdate(&cpBuffer, &uiSize);
            if (!CLuaMain::LuaLoadBuffer(luaVM, cpBuffer, uiSize, szChunkname))
            {
                // Ok
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
