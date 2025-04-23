/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Event.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaFunctionDefs.h"
#include "CScriptArgReader.h"
#include "CStaticFunctionDefinitions.h"
#include "CPerfStatManager.h"
#include "CLatentTransferManager.h"
#include "CDebugHookManager.h"

#define MIN_SERVER_REQ_TRIGGERCLIENTEVENT_SENDLIST          "1.3.0-9.04570"

extern CTimeUsMarker<20> markerLatentEvent;

int CLuaFunctionDefs::AddEvent(lua_State* luaVM)
{
    //  bool addEvent ( string eventName [, bool allowRemoteTrigger = false ] )
    SString strName;
    bool    bAllowRemoteTrigger;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadBool(bAllowRemoteTrigger, false);

    if (!argStream.HasErrors())
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Do it
            if (CStaticFunctionDefinitions::AddEvent(pLuaMain, strName, "", bAllowRemoteTrigger))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::AddEventHandler(lua_State* luaVM)
{
    //  bool addEventHandler ( string eventName, element attachedTo, function handlerFunction [, bool getPropagated = true, string priority = "normal" ] )
    SString         strName;
    CElement*       pElement;
    CLuaFunctionRef iLuaFunction;
    bool            bPropagated;
    SString         strPriority;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadUserData(pElement);
    argStream.ReadFunction(iLuaFunction);
    argStream.ReadBool(bPropagated, true);
    argStream.ReadString(strPriority, "normal");
    argStream.ReadFunctionComplete();

    // Check if strPriority has a number as well. e.g. name+1 or name-1.32
    float              fPriorityMod = 0;
    EEventPriorityType eventPriority;
    {
        size_t iPos = strPriority.find_first_of("-+");
        if (iPos != SString::npos)
        {
            fPriorityMod = static_cast<float>(atof(strPriority.SubStr(iPos)));
            strPriority = strPriority.Left(iPos);
        }

        if (!StringToEnum(strPriority, eventPriority))
            argStream.SetTypeError(GetEnumTypeName(eventPriority), 5);            // priority is argument #5
    }

    if (!argStream.HasErrors())
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResourceFile* file = pLuaMain->GetResourceFile();
            if (file && file->GetType() == CResourceFile::RESOURCE_FILE_TYPE_HTML)
            {
                argStream.SetCustomError("You cannot have event handlers in HTML scripts");
            }
            else
            {
                // check if the handle is in use
                if (pElement->GetEventManager()->HandleExists(pLuaMain, strName, iLuaFunction))
                {
                    argStream.SetCustomError(SString("'%s' with this function is already handled", *strName));
                }
                else
                {
                    // Do it
                    if (CStaticFunctionDefinitions::AddEventHandler(pLuaMain, strName, pElement, iLuaFunction, bPropagated, eventPriority, fPriorityMod))
                    {
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

int CLuaFunctionDefs::RemoveEventHandler(lua_State* luaVM)
{
    //  bool removeEventHandler ( string eventName, element attachedTo, function functionVar )
    SString         strName;
    CElement*       pElement;
    CLuaFunctionRef iLuaFunction;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadUserData(pElement);
    argStream.ReadFunction(iLuaFunction);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (CStaticFunctionDefinitions::RemoveEventHandler(pLuaMain, strName, pElement, iLuaFunction))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetEventHandlers(lua_State* luaVM)
{
    //  table getEventHandlers ( string eventName, element attachedTo )
    SString   strName;
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        // Grab our virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Create a new table
            lua_newtable(luaVM);

            pElement->GetEventManager()->GetHandles(pLuaMain, (const char*)strName, luaVM);

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::TriggerEvent(lua_State* luaVM)
{
    //  bool triggerEvent ( string eventName, element baseElement, [ var argument1, ... ] )
    SString       strName;
    CElement*     pElement;
    CLuaArguments Arguments;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadUserData(pElement);
    argStream.ReadLuaArguments(Arguments);

    if (!argStream.HasErrors())
    {
        // Trigger it
        bool bWasCancelled;
        if (CStaticFunctionDefinitions::TriggerEvent(strName, pElement, Arguments, bWasCancelled))
        {
            lua_pushboolean(luaVM, !bWasCancelled);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaFunctionDefs::TriggerClientEvent(lua_State* luaVM)
{
    //  int triggerClientEvent ( [element/table triggerFor,] string name, element theElement, [arguments...] )
    std::vector<CPlayer*> sendList;
    SString               strName;
    CElement*             pCallWithElement;
    CLuaArguments         Arguments;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsTable())
    {
        MinServerReqCheck(argStream, MIN_SERVER_REQ_TRIGGERCLIENTEVENT_SENDLIST, "a send list is being used");
        argStream.ReadUserDataTable(sendList);
    }
    else
    {
        CElement* pElement;
        argStream.ReadIfNextIsUserData(pElement, CStaticFunctionDefinitions::GetRootElement());
        if (!argStream.HasErrors())
            pElement->GetDescendantsByType(sendList, CElement::PLAYER);
    }
    argStream.ReadString(strName);
    argStream.ReadUserData(pCallWithElement);
    argStream.ReadLuaArguments(Arguments);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::TriggerClientEvent(sendList, strName, pCallWithElement, Arguments))
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

int CLuaFunctionDefs::TriggerLatentClientEvent(lua_State* luaVM)
{
    markerLatentEvent = CTimeUsMarker<20>();
    markerLatentEvent.Set("Start");

    //  int triggerLatentClientEvent ( [element/table triggerFor,] string name, [int bandwidth=50000,] [bool persist=false,] element theElement, [arguments...]
    //  )
    std::vector<CPlayer*> sendList;
    SString               strName;
    int                   iBandwidth;
    bool                  bPersist;
    CElement*             pCallWithElement;
    CLuaArguments         Arguments;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsTable())
    {
        MinServerReqCheck(argStream, MIN_SERVER_REQ_TRIGGERCLIENTEVENT_SENDLIST, "a send list is being used");
        argStream.ReadUserDataTable(sendList);
    }
    else
    {
        CElement* pElement;
        argStream.ReadIfNextIsUserData(pElement, CStaticFunctionDefinitions::GetRootElement());
        if (!argStream.HasErrors())
            pElement->GetDescendantsByType(sendList, CElement::PLAYER);
        markerLatentEvent.Set("GetDescendantsByType");
    }
    argStream.ReadString(strName);
    argStream.ReadIfNextCouldBeNumber(iBandwidth, 50000);
    argStream.ReadIfNextIsBool(bPersist, false);
    argStream.ReadUserData(pCallWithElement);
    argStream.ReadLuaArguments(Arguments);

    if (!argStream.HasErrors())
    {
        // Get resource details if transfer should be stopped when resource stops
        CLuaMain* pLuaMain = NULL;
        ushort    usResourceNetId = 0xFFFF;
        if (!bPersist)
        {
            pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                CResource* pResource = pLuaMain->GetResource();
                if (pResource)
                {
                    usResourceNetId = pResource->GetNetID();
                }
            }
        }

        markerLatentEvent.SetAndStoreString(SString("Get args (%d,%s)", sendList.size(), *strName));

        // Trigger it
        if (CStaticFunctionDefinitions::TriggerLatentClientEvent(sendList, strName, pCallWithElement, Arguments, iBandwidth, pLuaMain, usResourceNetId))
        {
            markerLatentEvent.Set("End");

            // Add debug info if wanted
            if (CPerfStatDebugInfo::GetSingleton()->IsActive("TriggerLatentClientEvent"))
                CPerfStatDebugInfo::GetSingleton()->AddLine("TriggerLatentClientEvent", markerLatentEvent.GetString());

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetLatentEventHandles(lua_State* luaVM)
{
    //  table getLatentEventHandles ( element player )
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        std::vector<uint> resultList;
        g_pGame->GetLatentTransferManager()->GetSendHandles(pPlayer->GetSocket(), resultList);

        lua_createtable(luaVM, 0, resultList.size());
        for (uint i = 0; i < resultList.size(); i++)
        {
            lua_pushnumber(luaVM, i + 1);
            lua_pushnumber(luaVM, resultList[i]);
            lua_settable(luaVM, -3);
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetLatentEventStatus(lua_State* luaVM)
{
    //  int start,end = getLatentEventStatus ( element player, int handle )
    CPlayer* pPlayer;
    int      iHandle;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadNumber(iHandle);

    if (!argStream.HasErrors())
    {
        SSendStatus sendStatus;
        if (g_pGame->GetLatentTransferManager()->GetSendStatus(pPlayer->GetSocket(), iHandle, &sendStatus))
        {
            lua_createtable(luaVM, 0, 4);

            lua_pushstring(luaVM, "tickStart");
            lua_pushinteger(luaVM, sendStatus.iStartTimeMsOffset);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "tickEnd");
            lua_pushinteger(luaVM, sendStatus.iEndTimeMsOffset);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "totalSize");
            lua_pushinteger(luaVM, sendStatus.iTotalSize);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "percentComplete");
            lua_pushnumber(luaVM, sendStatus.dPercentComplete);
            lua_settable(luaVM, -3);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::CancelLatentEvent(lua_State* luaVM)
{
    //  bool cancelLatentEvent ( element player, int handle )
    CPlayer* pPlayer;
    int      iHandle;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadNumber(iHandle);

    if (!argStream.HasErrors())
    {
        if (g_pGame->GetLatentTransferManager()->CancelSend(pPlayer->GetSocket(), iHandle))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::AddDebugHook(lua_State* luaVM)
{
    //  bool AddDebugHook ( string hookType, function callback[, table allowedNames ] )
    EDebugHookType       hookType;
    CLuaFunctionRef      callBack;
    std::vector<SString> allowedNames;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(hookType);
    argStream.ReadFunction(callBack);
    if (argStream.NextIsTable())
        argStream.ReadStringTable(allowedNames);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        if (g_pGame->GetDebugHookManager()->AddDebugHook(hookType, callBack, allowedNames))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::RemoveDebugHook(lua_State* luaVM)
{
    //  bool RemoveDebugHook ( string hookType, function callback )
    EDebugHookType  hookType;
    CLuaFunctionRef callBack;

    CScriptArgReader argStream(luaVM);
    argStream.ReadEnumString(hookType);
    argStream.ReadFunction(callBack);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        if (g_pGame->GetDebugHookManager()->RemoveDebugHook(hookType, callBack))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::CancelEvent(lua_State* luaVM)
{
    //  bool cancelEvent ( [ bool cancel = true, string reason = "" ] )
    bool    bCancel;
    SString strReason;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bCancel, true);
    argStream.ReadString(strReason, "");

    if (!argStream.HasErrors())
    {
        CStaticFunctionDefinitions::CancelEvent(bCancel, strReason);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetCancelReason(lua_State* luaVM)
{
    const char* szReason = CStaticFunctionDefinitions::GetCancelReason();
    if (szReason)
    {
        lua_pushstring(luaVM, szReason);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::WasEventCancelled(lua_State* luaVM)
{
    lua_pushboolean(luaVM, CStaticFunctionDefinitions::WasEventCancelled());
    return 1;
}
