/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Event.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <event/CustomEvents.h>
#include <event/EventDispatcher.h>

#define MIN_SERVER_REQ_TRIGGERCLIENTEVENT_SENDLIST          "1.3.0-9.04570"

const Event* ResolveEvent(const std::string& name)
{
    if (const Event* event = Event::Get(name))
        return event;
    throw std::invalid_argument("Event doesn't exist");
}

bool CLuaFunctionDefs::RemoveEventHandler(lua_State* L, std::string eventName, CElement* attachedTo, CLuaFunctionRef handlerfn)
{
    return attachedTo->GetEventHandlerCallDispatcher().Remove(*ResolveEvent(eventName), m_pLuaManager->GetVirtualMachine(L), handlerfn);
}

bool CLuaFunctionDefs::AddEventHandler(lua_State* L, std::string eventName, CElement* attachedTo,
    CLuaFunctionRef handlerfn, std::optional<bool> propagated, std::optional<std::string_view> priorityToParse)
{
    EventHandler::Priority priority{ priorityToParse.value_or("") }; // Might throw invalid_argument

    const Event* event = ResolveEvent(eventName);
    std::cout << "Add event " << eventName << "[Priority: " << priority.ToString() << "]\n"; // TODO Remove

    return attachedTo->GetEventHandlerCallDispatcher().Add(*event,
        { priority, m_pLuaManager->GetVirtualMachine(L), handlerfn, propagated.value_or(true) }
    );
}

bool CLuaFunctionDefs::AddEvent(lua_State* L, std::string name, std::optional<bool> allowRemoteTrigger)
{
    if (name.empty())
        return false; // Invalid name (maybe throw here?)
    return CustomEvent::Add(name, m_pLuaManager->GetVirtualMachine(L), allowRemoteTrigger.value_or(false));
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
        if (CLuaMain* lmain = m_pLuaManager->GetVirtualMachine(luaVM))
        {
            if (auto* event = Event::Get(strName))
            {
                if (auto* handlers = pElement->GetEventHandlerCallDispatcher().GetHandlers(*event))
                {
                    lua_newtable(luaVM);
                    handlers->PushToLua(lmain, luaVM);
                    return 1;
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
        if (auto* event = Event::Get(strName))
        {
            lua_pushboolean(luaVM, !pElement->CallEvent(*event, Arguments));
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

bool CLuaFunctionDefs::CancelEvent(std::optional<bool> cancel, std::optional<std::string_view> reason)
{
    s_EventDispatcher.CancelEvent(cancel.value_or(true), std::string{ reason.value_or("") });
    return true;
}

const std::string& CLuaFunctionDefs::GetCancelReason()
{
    return s_EventDispatcher.GetCancelReason();
}

bool CLuaFunctionDefs::WasEventCancelled()
{
    return s_EventDispatcher.WasEventCancelled();
}
