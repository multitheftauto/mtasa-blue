/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Event.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <iostream> // TOOD remove
#include <event/Event.h>
#include <event/EventDispatcher.h>

const Event* ResolveEvent(const std::string& name)
{
    if (const Event* event = Event::Get(name))
        return event;
    throw std::invalid_argument("Event doesn't exist");
}

bool CLuaFunctionDefs::RemoveEventHandler(lua_State* L, std::string eventName, CClientEntity* attachedTo, CLuaFunctionRef handlerfn)
{
    return attachedTo->GetEventHandlerCallDispatcher().Remove(*ResolveEvent(eventName), m_pLuaManager->GetVirtualMachine(L), handlerfn);
}

bool CLuaFunctionDefs::AddEventHandler(lua_State* L, std::string eventName, CClientEntity* attachedTo,
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
    SString        strName;
    CClientEntity* pElement;

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
    CElement* pElement;
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

int CLuaFunctionDefs::TriggerServerEvent(lua_State* luaVM)
{
    //  bool triggerServerEvent ( string event, element theElement, [arguments...] )
    SString        strName;
    CClientEntity* pCallWithEntity;
    CLuaArguments  Arguments;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadUserData(pCallWithEntity);
    argStream.ReadLuaArguments(Arguments);

    if (!argStream.HasErrors())
    {
        if (!pCallWithEntity->IsLocalEntity())
        {
            if (CStaticFunctionDefinitions::TriggerServerEvent(strName, *pCallWithEntity, Arguments))
            {
                lua_pushboolean(luaVM, true);
            }
            else
            {
                lua_pushboolean(luaVM, false);

                // Show a warning for clientside elements in the argument chain
                for (uint i = 0; i < Arguments.Count(); ++i)
                {
                    CLuaArgument* pArgument = Arguments[i];

                    if (!pArgument)
                        continue;

                    if (pArgument->GetType() != LUA_TLIGHTUSERDATA && pArgument->GetType() != LUA_TUSERDATA)
                        continue;

                    CClientEntity* pEntity = pArgument->GetElement();

                    if (pEntity && !pEntity->IsLocalEntity())
                        continue;

                    // Extra arguments begin at argument 3
                    if (pEntity)
                    {
                        m_pScriptDebugging->LogError(luaVM, "clientside element '%s' at argument %u @ 'triggerServerEvent'",
                                                     pEntity->GetTypeName().c_str(), i + 3);
                    }
                    else
                    {
                        m_pScriptDebugging->LogError(luaVM, "userdata at argument %u @ 'triggerServerEvent'", i + 3);
                    }
                }
            }

            return 1;
        }

        argStream.SetCustomError("element is clientside", "Bad source element");
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaFunctionDefs::CancelEvent()
{
    s_EventDispatcher.CancelEvent(true, "");
    return true;
}

bool CLuaFunctionDefs::WasEventCancelled()
{
    return s_EventDispatcher.WasEventCancelled();
}

int CLuaFunctionDefs::TriggerLatentServerEvent(lua_State* luaVM)
{
    //  int triggerLatentServerEvent ( string event, [int bandwidth=5000,] [bool persist=false,] element theElement, [arguments...] )
    SString        strName;
    int            iBandwidth;
    bool           bPersist;
    CClientEntity* pCallWithEntity;
    CLuaArguments  Arguments;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);
    argStream.ReadIfNextCouldBeNumber(iBandwidth, 5000);
    argStream.ReadIfNextIsBool(bPersist, false);
    argStream.ReadUserData(pCallWithEntity);
    argStream.ReadLuaArguments(Arguments);

    if (!argStream.HasErrors())
    {
        // Get resource details if transfer should be stopped when resource stops
        CLuaMain* pLuaMain = NULL;
        ushort    usResourceNetId = 0xFFFF;
        if (!bPersist)
        {
            pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
            if (pLuaMain)
            {
                CResource* pResource = pLuaMain->GetResource();
                if (pResource)
                {
                    usResourceNetId = pResource->GetNetID();
                }
            }
        }

        // Trigger it
        if (CStaticFunctionDefinitions::TriggerLatentServerEvent(strName, *pCallWithEntity, Arguments, iBandwidth, pLuaMain, usResourceNetId))
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

int CLuaFunctionDefs::GetLatentEventHandles(lua_State* luaVM)
{
    //  table getLatentEventHandles ()
    CScriptArgReader argStream(luaVM);

    if (!argStream.HasErrors())
    {
        std::vector<uint> resultList;
        g_pClientGame->GetLatentTransferManager()->GetSendHandles(0, resultList);

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
    //  int start,end = getLatentEventStatus ( int handle )
    int iHandle;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iHandle);

    if (!argStream.HasErrors())
    {
        SSendStatus sendStatus;
        if (g_pClientGame->GetLatentTransferManager()->GetSendStatus(0, iHandle, &sendStatus))
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
    //  bool cancelLatentEvent ( int handle )
    int iHandle;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iHandle);

    if (!argStream.HasErrors())
    {
        if (g_pClientGame->GetLatentTransferManager()->CancelSend(0, iHandle))
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
