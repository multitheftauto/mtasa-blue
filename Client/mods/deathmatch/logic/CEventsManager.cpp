/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CEventsManager.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEventsManager.h"

CEventsManager::CEventsManager()
{
    constexpr auto eventCount = static_cast<std::size_t>(BuiltInEvent::MAX_EVENTS);
    m_eventNameToId.reserve(eventCount);

    for (std::size_t i = 0; i < eventCount; ++i)
    {
        const auto event = static_cast<BuiltInEvent::Enum>(i);

        m_eventNames[i] = EnumToString(event);
        m_eventNameToId[m_eventNames[i]] = event;
    }
}

bool CEventsManager::AddEvent(const std::string& eventName, bool allowRemoteTrigger)
{
    std::uint32_t hash = HashString(eventName.c_str(), eventName.length());
    auto [it, inserted] =
        m_customEvents.try_emplace(hash, SCustomEvent{.eventNameHash = hash, .eventName = eventName, .allowRemoteTrigger = allowRemoteTrigger});
    return inserted;
}

void CEventsManager::AddHandler(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity, CLuaMain* luaMain,
                                const CLuaFunctionRef& luaFunctionRef, bool propagated, EEventPriority::EEventPriorityType priority, float priorityMod,
                                eClientEntityType entityType)
{
    std::vector<SEventHandler>* handlersListPtr = nullptr;
    bool                        isRenderingEvent = false;

    if (std::holds_alternative<BuiltInEvent::Enum>(event))
    {
        auto builtInEnum = std::get<BuiltInEvent::Enum>(event);
        handlersListPtr = &m_eventsTable[static_cast<std::size_t>(builtInEnum)][sourceEntity];
        SetEventActive(builtInEnum, true);

        isRenderingEvent = builtInEnum == BuiltInEvent::ON_CLIENT_RENDER || builtInEnum == BuiltInEvent::ON_CLIENT_PRE_RENDER ||
                           builtInEnum == BuiltInEvent::ON_CLIENT_HUD_RENDER;
    }
    else
    {
        auto hash = std::get<std::uint32_t>(event);
        auto it = m_customEvents.find(hash);
        if (it != m_customEvents.end())
            handlersListPtr = &it->second.handlersTable[sourceEntity];
    }

    if (!handlersListPtr)
        return;

    handlersListPtr->push_back(SEventHandler{.luaMain = luaMain,
                                             .luaFunctionRef = luaFunctionRef,
                                             .isValid = true,
                                             .propagate = propagated,
                                             .priority = priority,
                                             .priorityMod = priorityMod,
                                             .entityType = entityType,
                                             .isRenderingEvent = isRenderingEvent,
                                             .forceAspectRatioAdjustment = std::string(luaMain->GetScriptName()) == "customblips"});

    std::sort(handlersListPtr->begin(), handlersListPtr->end());
}

bool CEventsManager::RemoveHandler(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity, CLuaMain* luaMain,
                                   const CLuaFunctionRef& luaFunctionRef)
{
    CFastHashMap<CClientEntity*, std::vector<SEventHandler>>* entityMapPtr = nullptr;

    if (std::holds_alternative<BuiltInEvent::Enum>(event))
    {
        auto builtInEnum = std::get<BuiltInEvent::Enum>(event);
        entityMapPtr = &m_eventsTable[static_cast<std::size_t>(builtInEnum)];
    }
    else
    {
        auto hash = std::get<std::uint32_t>(event);
        auto it = m_customEvents.find(hash);
        if (it != m_customEvents.end())
            entityMapPtr = &it->second.handlersTable;
    }

    if (!entityMapPtr)
        return false;

    auto it = entityMapPtr->find(sourceEntity);
    if (it == entityMapPtr->end())
        return false;

    auto& handlersList = it->second;
    bool  removed = false;

    for (auto handlerIt = handlersList.begin(); handlerIt != handlersList.end(); ++handlerIt)
    {
        if (handlerIt->luaMain == luaMain && handlerIt->luaFunctionRef == luaFunctionRef && handlerIt->isValid)
        {
            handlerIt->isValid = false;
            removed = true;
        }
    }

    // If the event is currently being executed, it will be removed by the executing loop (ExecuteHandlersForEntity)
    handlersList.erase(std::remove_if(handlersList.begin(), handlersList.end(), [](const SEventHandler& h) { return !h.isValid && !h.isCurrentlyUsed; }),
                       handlersList.end());

    if (handlersList.empty())
        entityMapPtr->erase(it);

    return removed;
}

void CEventsManager::RemoveAllHandlers(CLuaMain* luaMain)
{
    // Built-in events
    for (std::size_t eventIndex = 0; eventIndex < static_cast<std::size_t>(BuiltInEvent::MAX_EVENTS); ++eventIndex)
    {
        auto& eventMap = m_eventsTable[eventIndex];
        for (auto it = eventMap.begin(); it != eventMap.end();)
        {
            auto& handlersList = it->second;

            handlersList.erase(std::remove_if(handlersList.begin(), handlersList.end(),
                                              [luaMain](const SEventHandler& h)
                                              {
                                                  if (h.luaMain == luaMain)
                                                  {
                                                      if (h.isCurrentlyUsed)
                                                      {
                                                          const_cast<SEventHandler&>(h).isValid = false;
                                                          return false;
                                                      }
                                                      return true;
                                                  }
                                                  return false;
                                              }),
                               handlersList.end());

            if (handlersList.empty())
                eventMap.erase(it++);
            else
                ++it;
        }
    }

    // Custom events
    for (auto& [hash, customEvent] : m_customEvents)
    {
        auto& eventMap = customEvent.handlersTable;
        for (auto it = eventMap.begin(); it != eventMap.end();)
        {
            auto& handlersList = it->second;

            handlersList.erase(std::remove_if(handlersList.begin(), handlersList.end(),
                                              [luaMain](const SEventHandler& h)
                                              {
                                                  if (h.luaMain == luaMain)
                                                  {
                                                      if (h.isCurrentlyUsed)
                                                      {
                                                          const_cast<SEventHandler&>(h).isValid = false;
                                                          return false;
                                                      }
                                                      return true;
                                                  }
                                                  return false;
                                              }),
                               handlersList.end());

            if (handlersList.empty())
                eventMap.erase(it++);
            else
                ++it;
        }
    }
}

bool CEventsManager::IsEventHandlerAttached(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity, CLuaMain* luaMain,
                                            const CLuaFunctionRef& luaFunctionRef)
{
    const CFastHashMap<CClientEntity*, std::vector<SEventHandler>>* entityMapPtr = nullptr;

    if (std::holds_alternative<BuiltInEvent::Enum>(event))
    {
        auto builtInEnum = std::get<BuiltInEvent::Enum>(event);
        entityMapPtr = &m_eventsTable[static_cast<std::size_t>(builtInEnum)];
    }
    else
    {
        auto hash = std::get<std::uint32_t>(event);
        auto it = m_customEvents.find(hash);
        if (it != m_customEvents.end())
            entityMapPtr = &it->second.handlersTable;
    }

    if (!entityMapPtr)
        return false;

    auto it = entityMapPtr->find(sourceEntity);
    if (it == entityMapPtr->end())
        return false;

    const auto& handlersList = it->second;
    for (const auto& handler : handlersList)
    {
        if (handler.isValid && handler.luaMain == luaMain && handler.luaFunctionRef == luaFunctionRef)
            return true;
    }

    return false;
}

bool CEventsManager::TriggerEvent(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity, const CLuaArguments& args,
                                  bool callOnChildren)
{
    // Check for multi-threading slipups
    assert(IsMainThread());

    EventHandlersTable* handlers = nullptr;
    std::string_view    eventName;

    if (std::holds_alternative<BuiltInEvent::Enum>(event))
    {
        const auto eventId = std::get<BuiltInEvent::Enum>(event);
        if (!IsEventActive(eventId))
            return false;

        eventName = GetEventName(eventId);
        handlers = &m_eventsTable[static_cast<std::size_t>(eventId)];
    }
    else
    {
        const auto hash = std::get<std::uint32_t>(event);
        auto       it = m_customEvents.find(hash);
        if (it == m_customEvents.end())
            return false;

        eventName = it->second.eventName;
        handlers = &it->second.handlersTable;
    }

     //if (!g_pClientGame->GetDebugHookManager()->OnPreEvent(eventName.data(), args, sourceEntity, nullptr))
     //    return false;

    TIMEUS startTime = GetTimeUs();

    m_eventCancelled = false;

    // Call the event on our entity and its parents(up the tree)
    CClientEntity* currentEntity = sourceEntity;
    while (currentEntity)
    {
        ExecuteHandlersForEntity(*handlers, sourceEntity, currentEntity, args, eventName);
        currentEntity = currentEntity->GetParent();
    }

    // Call the event on the children (down the tree)
    if (callOnChildren && sourceEntity)
        TriggerEventOnChildren(*handlers, sourceEntity, sourceEntity, args, eventName);

    if (IS_TIMING_CHECKPOINTS())
    {
        TIMEUS deltaTimeUs = GetTimeUs() - startTime;
        if (deltaTimeUs > 10000)
            TIMING_DETAIL(SString("Event: %s [%d ms]", eventName.data(), deltaTimeUs / 1000));
    }

    //g_pClientGame->GetDebugHookManager()->OnPostEvent(eventName.data(), args, sourceEntity, nullptr);
    return !m_eventCancelled;
}

void CEventsManager::TriggerEventOnChildren(EventHandlersTable& handlersTable, CClientEntity* sourceEntity, CClientEntity* entity, const CLuaArguments& args,
                                            const std::string_view& eventName)
{
    const auto& children = entity->GetChildren();
    for (CClientEntity* child : children)
    {
        if (!child || child->IsBeingDeleted())
            continue;

        ExecuteHandlersForEntity(handlersTable, sourceEntity, child, args, eventName);
        TriggerEventOnChildren(handlersTable, sourceEntity, child, args, eventName);
    }
}

void CEventsManager::ExecuteHandlersForEntity(EventHandlersTable& handlersTable, CClientEntity* sourceEntity, CClientEntity* entity, const CLuaArguments& args,
                                              const std::string_view& eventName)
{
     TIMEUS  startTimeCall = GetTimeUs();
     SString strStatus;

    auto it = handlersTable.find(entity);
    if (it == handlersTable.end())
        return;

    auto&      handlers = it->second;
    bool       isDirectSource = sourceEntity == entity;
    const auto entityType = entity->GetType();

    for (auto& handler : handlers)
    {
        if (!handler.isValid)
            continue;

        if (!isDirectSource && !handler.propagate)
            continue;

        if (handler.entityType != eClientEntityType::CCLIENTUNKNOWN && entityType != handler.entityType)
            continue;

        handler.isCurrentlyUsed = true;

        CLuaMain*  luaMain = handler.luaMain;
        lua_State* luaVM = luaMain->GetVM();

        LUA_CHECKSTACK(luaVM, 1);

        TIMEUS startTime = GetTimeUs();

        // Record event for the crash dump writer
        if (g_pCore->GetDiagnosticDebug() == EDiagnosticDebug::LUA_TRACE_0000)
            g_pCore->LogEvent(0, "Lua Event", luaMain->GetScriptName(), eventName.data());

        // Aspect ratio adjustment bodges
        if (handler.isRenderingEvent)
        {
            m_callingRenderEvent = true;
            if (handler.forceAspectRatioAdjustment)
                g_pCore->GetGraphics()->SetAspectRatioAdjustmentEnabled(true);
        }

        // TODO g_pClientGame->GetDebugHookManager()->OnPreEventFunction

        int preCallTop = lua_gettop(luaVM);

        lua_getref(luaVM, luaMain->GetEventHandlerGlobalsFuncRef());
        lua_getref(luaVM, handler.luaFunctionRef.ToInt());
        lua::Push(luaVM, sourceEntity);
        lua::Push(luaVM, entity);

        if (auto topLuaMain = g_pClientGame->GetScriptDebugging()->GetTopLuaMain())
        {
            CResource* sourceResource = topLuaMain->GetResource();
            lua::Push(luaVM, sourceResource);
            lua::Push(luaVM, sourceResource->GetResourceEntity());
        }
        else
        {
            lua::Push(luaVM, nullptr);
            lua::Push(luaVM, nullptr);
        }

        lua::Push(luaVM, eventName.data());
        args.PushArguments(luaVM);

        luaMain->ResetInstructionCount();

        switch (luaMain->PCall(luaVM, 6 + args.Count(), 0, 0))
        {
            case LUA_ERRRUN:
            case LUA_ERRMEM:
            {
                g_pClientGame->GetScriptDebugging()->LogPCallError(luaVM, ConformResourcePath(lua_tostring(luaVM, -1)));
                break;
            }
            default:
            {
                CClientPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(luaMain, luaMain->GetFunctionTag(handler.luaFunctionRef.ToInt()), GetTimeUs() - startTime);
                break;
            }
        }

        lua_settop(luaVM, preCallTop);

        // TODO g_pClientGame->GetDebugHookManager()->OnPostEventFunction
        
        handler.isCurrentlyUsed = false;

        // Aspect ratio adjustment bodges
        if (handler.isRenderingEvent)
        {
            g_pCore->GetGraphics()->SetAspectRatioAdjustmentEnabled(false);
            m_callingRenderEvent = false;
        }
    }

    if (IS_TIMING_CHECKPOINTS())
    {
         TIMEUS deltaTimeUs = GetTimeUs() - startTimeCall;
         if (deltaTimeUs > 5000)
             TIMING_DETAIL(SString("CEventsManager::ExecuteHandlersForEntity ( %s, ... ) took %d ms ( %s )", eventName, deltaTimeUs / 1000, *strStatus));
     }

    handlers.erase(std::remove_if(handlers.begin(), handlers.end(), [](const SEventHandler& h) { return !h.isValid && !h.isCurrentlyUsed; }), handlers.end());
}

const SCustomEvent* CEventsManager::GetCustomEvent(std::uint32_t hash) const
{
    auto it = m_customEvents.find(hash);
    if (it != m_customEvents.end())
        return &it->second;

    return nullptr;
}

std::vector<CLuaFunctionRef> CEventsManager::GetEventHandlers(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity,
                                                              CLuaMain* luaMain, std::optional<eClientEntityType> elementType)
{
    std::vector<CLuaFunctionRef>                              result{};
    CFastHashMap<CClientEntity*, std::vector<SEventHandler>>* entityMapPtr = nullptr;

    if (std::holds_alternative<BuiltInEvent::Enum>(event))
    {
        auto builtInEnum = std::get<BuiltInEvent::Enum>(event);
        entityMapPtr = &m_eventsTable[static_cast<std::size_t>(builtInEnum)];
    }
    else
    {
        auto hash = std::get<std::uint32_t>(event);
        auto it = m_customEvents.find(hash);
        if (it != m_customEvents.end())
            entityMapPtr = &it->second.handlersTable;
    }

    if (!entityMapPtr)
        return result;

    auto it = entityMapPtr->find(sourceEntity);
    if (it != entityMapPtr->end())
    {
        const auto& handlersList = it->second;
        for (const auto& handler : handlersList)
        {
            if (!handler.isValid || handler.luaMain != luaMain)
                continue;

            if (elementType.has_value() && sourceEntity->GetType() != elementType.value())
                continue;

            result.push_back(handler.luaFunctionRef);
        }
    }

    return result;
}

std::string_view CEventsManager::GetEventName(std::uint32_t hash) const
{
    if (const auto it = m_customEvents.find(hash); it != m_customEvents.end())
        return it->second.eventName;

    return "unknown";
}
