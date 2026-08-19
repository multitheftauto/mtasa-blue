/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaEventDefs.cpp
 *  PURPOSE:     Lua event definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaEventDefs.h"

void CLuaEventDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{{"addEvent", ArgumentParserWarn<false, AddEvent>},
                                                                             {"addEventHandler", ArgumentParserWarn<false, AddEventHandler>},
                                                                             {"removeEventHandler", ArgumentParserWarn<false, RemoveEventHandler>},
                                                                             {"cancelEvent", ArgumentParserWarn<false, CancelEvent>},

                                                                             {"triggerEvent", ArgumentParserWarn<nullptr, TriggerEvent>},

                                                                             {"getEventHandlers", ArgumentParserWarn<false, GetEventHandlers>},
                                                                             {"wasEventCancelled", ArgumentParserWarn<false, WasEventCancelled>}};

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

std::optional<bool> CLuaEventDefs::TriggerEvent(lua_State* luaVM, std::string name, CClientEntity* baseElement, std::optional<CLuaArguments> args)
{
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    std::uint32_t hash = HashString(name.c_str(), name.length());

    CEventsManager* eventManager = m_pClientGame->GetEventsManager();
    if (!eventManager->EventExists(hash))
        return false;

    return eventManager->TriggerEvent(hash, baseElement, args.value_or(CLuaArguments()), true);
}

bool CLuaEventDefs::AddEvent(lua_State* luaVM, std::string name, std::optional<bool> allowRemoteTrigger)
{
    // Check if this is built-in event
    if (auto it = eventNameToId.find(name); it != eventNameToId.end())
        return false;

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    return m_pClientGame->GetEventsManager()->AddEvent(name, allowRemoteTrigger.value_or(false));
}

bool CLuaEventDefs::AddEventHandler(lua_State* luaVM, std::string name, CClientEntity* attachedTo, CLuaFunctionRef func, std::optional<bool> propagated,
                                    std::optional<std::string> priority, std::optional<eClientEntityType> entityType)
{
    EEventPriority::EEventPriorityType priorityType = EEventPriority::NORMAL;
    float                              priorityMod = 0.0f;

    if (priority.has_value())
    {
        auto& priorityStr = priority.value();

        std::size_t iPos = priorityStr.find_first_of("-+");
        if (iPos != std::string::npos)
        {
            priorityMod = std::strtof(priorityStr.c_str() + iPos, nullptr);
            priorityStr = priorityStr.substr(0, iPos);
        }

        if (!StringToEnum(priorityStr, priorityType))
            throw std::invalid_argument("Invalid priority type");
    }

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    // Check if this is built-in event
    std::variant<std::uint32_t, BuiltInEvent::Enum> event;

    if (auto it = CEventsManager::getBuiltInEvent(name); it != CEventsManager::GetIterEnd())
        event = it->second;
    else
        event = HashString(name.c_str(), name.length());

    CEventsManager* eventManager = m_pClientGame->GetEventsManager();
    if (eventManager->IsEventHandlerAttached(event, attachedTo, luaMain, func))
        throw std::logic_error(name + " with this function is already handled");

    m_pClientGame->GetEventsManager()->AddHandler(event, attachedTo, luaMain, func, propagated.value_or(true), priorityType, priorityMod,
                                                        entityType.value_or(eClientEntityType::CCLIENTUNKNOWN));
    return true;
}

bool CLuaEventDefs::RemoveEventHandler(lua_State* luaVM, std::string name, CClientEntity* attachedTo, CLuaFunctionRef func)
{
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    std::variant<std::uint32_t, BuiltInEvent::Enum> event;

    // Check if this is built-in event
    if (auto it = CEventsManager::getBuiltInEvent(name); it != CEventsManager::GetIterEnd())
        event = it->second;
    else
        event = HashString(name.c_str(), name.length());

    CEventsManager* eventManager = m_pClientGame->GetEventsManager();
    if (!eventManager->IsEventHandlerAttached(event, attachedTo, luaMain, func))
        return false;

    return eventManager->RemoveHandler(event, attachedTo, luaMain, func);
}

std::variant<bool, std::vector<CLuaFunctionRef>> CLuaEventDefs::GetEventHandlers(lua_State* luaVM, std::string name, CClientEntity* attachedTo,
                                                                                 std::optional<eClientEntityType> entityType)
{
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    std::variant<std::uint32_t, BuiltInEvent::Enum> event;

    // Check if this is built-in event
    if (auto it = CEventsManager::getBuiltInEvent(name); it != CEventsManager::GetIterEnd())
        event = it->second;
    else
        event = HashString(name.c_str(), name.length());

    return m_pClientGame->GetEventsManager()->GetEventHandlers(event, attachedTo, luaMain, entityType);
}

bool CLuaEventDefs::CancelEvent()
{
    m_pClientGame->GetEventsManager()->CancelEvent();
    return true;  // backwards compatibility
}

bool CLuaEventDefs::WasEventCancelled()
{
    return m_pClientGame->GetEventsManager()->WasEventCancelled();
}
