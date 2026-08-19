/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaEventDefs.h
 *  PURPOSE:     Lua event definitions class header
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaEventDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

private:
    static std::optional<bool> TriggerEvent(lua_State* luaVM, std::string name, CClientEntity* baseElement, std::optional<CLuaArguments> args);

    static bool AddEvent(lua_State* luaVM, std::string name, std::optional<bool> allowRemoteTrigger);
    static bool AddEventHandler(lua_State* luaVM, std::string name, CClientEntity* attachedTo, CLuaFunctionRef func, std::optional<bool> propagated,
                                std::optional<std::string> priority, std::optional<eClientEntityType> entityType);

    static bool RemoveEventHandler(lua_State* luaVM, std::string name, CClientEntity* attachedTo, CLuaFunctionRef func);

    static std::variant<bool, std::vector<CLuaFunctionRef>> GetEventHandlers(lua_State* luaVM, std::string name, CClientEntity* attachedTo,
                                                                             std::optional<eClientEntityType> entityType);

    static bool CancelEvent();
    static bool WasEventCancelled();
};
