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

    if (!inserted)
    {
        // Hash collision
        [[unlikely]]
        if (it->second.eventName != eventName)
            return false;

        // Update allowRemoteTrigger if it changed
        if (it->second.allowRemoteTrigger != allowRemoteTrigger)
        {
            it->second.allowRemoteTrigger = allowRemoteTrigger;
            return true;
        }
    }

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

    sourceEntity->IncrementEventHandlersCount();

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
    bool  isInUse = false;

    for (auto handlerIt = handlersList.begin(); handlerIt != handlersList.end(); ++handlerIt)
    {
        if (handlerIt->luaMain == luaMain && handlerIt->luaFunctionRef == luaFunctionRef && handlerIt->isValid)
        {
            handlerIt->isValid = false;
            removed = true;

            if (handlerIt->isInUse)
                isInUse = true;

            break;
        }
    }

    // If the event is currently being executed, it will be removed by the executing loop (ExecuteHandlersForEntity)
    if (!isInUse)
        TryRemoveHandler(sourceEntity, handlersList, *entityMapPtr, it);

    return removed;
}

void CEventsManager::RemoveAllHandlers(CLuaMain* luaMain)
{
    auto removeHandlersLambda = [luaMain](auto& eventMap)
    {
        for (auto it = eventMap.begin(); it != eventMap.end();)
        {
            auto& handlersList = it->second;

            handlersList.erase(std::remove_if(handlersList.begin(), handlersList.end(),
                                              [luaMain](SEventHandler& h)
                                              {
                                                  if (h.luaMain == luaMain)
                                                  {
                                                      if (h.isInUse)
                                                      {
                                                          h.isValid = false;
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
    };

    // Built-in events
    for (std::size_t eventIndex = 0; eventIndex < static_cast<std::size_t>(BuiltInEvent::MAX_EVENTS); ++eventIndex)
        removeHandlersLambda(m_eventsTable[eventIndex]);

    // Custom events
    for (auto& [hash, customEvent] : m_customEvents)
        removeHandlersLambda(customEvent.handlersTable);
}

void CEventsManager::RemoveHandlersForEntity(CClientEntity* entity)
{
    if (!entity || entity->GetEventHandlersCount() == 0)
        return;

    auto cleanEventTable = [entity](auto& eventMap)
    {
        auto it = eventMap.find(entity);
        if (it != eventMap.end())
        {
            bool inUse = false;
            for (auto& h : it->second)
            {
                if (h.isInUse)
                {
                    h.isValid = false;
                    inUse = true;
                }
            }

            // If none are in use, remove the entry from the map entirely
            if (!inUse)
                eventMap.erase(it);
            else
                // Otherwise, clear the vector contents to prevent keeping stale data
                it->second.clear();
        }
    };

    // Built-in events
    for (std::size_t eventIndex = 0; eventIndex < static_cast<std::size_t>(BuiltInEvent::MAX_EVENTS); ++eventIndex)
        cleanEventTable(m_eventsTable[eventIndex]);

    // Custom events
    for (auto& [hash, customEvent] : m_customEvents)
        cleanEventTable(customEvent.handlersTable);
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

bool CEventsManager::TriggerEvent(BuiltInEvent::Enum event, CClientEntity* sourceEntity, const CLuaArguments& args, bool callOnChildren)
{
    dassert(IsMainThread());

    const auto eventId = static_cast<std::size_t>(event);
    if (eventId >= m_eventsTable->size() || m_eventsTable[eventId].empty())
        return true;

    EventHandlersTable& handlersTable = m_eventsTable[eventId];
    std::string_view    eventName = GetEventName(event);

    // if (!g_pClientGame->GetDebugHookManager()->OnPreEvent(eventName.data(), args, sourceEntity, nullptr))
    //    return false;

    m_eventCancelled = false;

    // Call the event on our entity and its parents (up the tree)
    CClientEntity* currentEntity = sourceEntity;
    while (currentEntity)
    {
        if (!currentEntity->IsBeingDeleted() && currentEntity->GetEventHandlersCount() > 0)
        {
            auto it = handlersTable.find(currentEntity);
            if (it != handlersTable.end())
                ExecuteHandlersForEntity(it->second, handlersTable, it, sourceEntity, currentEntity, args, eventName);
        }

        currentEntity = currentEntity->GetParent();
    }

    // Call the event on the children (down the tree)
    if (callOnChildren && sourceEntity)
        TriggerEventOnChildren(handlersTable, sourceEntity, sourceEntity, args, eventName);

    // g_pClientGame->GetDebugHookManager()->OnPostEvent(eventName.data(), args, sourceEntity, nullptr);
    return !m_eventCancelled;
}

bool CEventsManager::TriggerCustomEvent(std::uint32_t hash, CClientEntity* sourceEntity, const CLuaArguments& args, bool callOnChildren)
{
    dassert(IsMainThread());

    auto it = m_customEvents.find(hash);
    if (it == m_customEvents.end())
        return false;

    EventHandlersTable& handlersTable = it->second.handlersTable;
    std::string_view    eventName = it->second.eventName;

    // if (!g_pClientGame->GetDebugHookManager()->OnPreEvent(eventName.data(), args, sourceEntity, nullptr))
    //    return false;

    m_eventCancelled = false;

    // Call the event on our entity and its parents (up the tree)
    CClientEntity* currentEntity = sourceEntity;
    while (currentEntity)
    {
        if (!currentEntity->IsBeingDeleted() && currentEntity->GetEventHandlersCount() > 0)
        {
            auto it = handlersTable.find(currentEntity);
            if (it != handlersTable.end())
                ExecuteHandlersForEntity(it->second, handlersTable, it, sourceEntity, currentEntity, args, eventName);
        }

        currentEntity = currentEntity->GetParent();
    }

    // Call the event on the children (down the tree)
    if (callOnChildren && sourceEntity)
        TriggerEventOnChildren(handlersTable, sourceEntity, sourceEntity, args, eventName);

    // g_pClientGame->GetDebugHookManager()->OnPostEvent(eventName.data(), args, sourceEntity, nullptr);
    return !m_eventCancelled;
}

void CEventsManager::TriggerEventOnChildren(EventHandlersTable& handlersTable, CClientEntity* sourceEntity, CClientEntity* entity, const CLuaArguments& args,
                                            const std::string_view& eventName)
{
    const auto& children = entity->GetChildren();
    for (CClientEntity* child : children)
    {
        if (!child || child->IsBeingDeleted() || child->GetEventHandlersCount() == 0)
            continue;

        auto it = handlersTable.find(child);
        if (it != handlersTable.end())
            ExecuteHandlersForEntity(it->second, handlersTable, it, sourceEntity, child, args, eventName);

        TriggerEventOnChildren(handlersTable, sourceEntity, child, args, eventName);
    }
}

void CEventsManager::TryRemoveHandler(CClientEntity* entity, EventHandlersList& handlers, EventHandlersTable& handlersTable, EventHandlersTable::iterator mapIt)
{
    std::size_t oldSize = handlers.size();
    handlers.erase(std::remove_if(handlers.begin(), handlers.end(), [](const SEventHandler& h) { return !h.isValid && !h.isInUse; }), handlers.end());
    std::size_t removedCount = oldSize - handlers.size();

    for (std::size_t i = 0; i < removedCount; ++i)
        entity->DecrementEventHandlersCount();

    if (handlers.empty())
        handlersTable.erase(mapIt);
}

void CEventsManager::ExecuteHandlersForEntity(EventHandlersList& handlers, EventHandlersTable& handlersTable, EventHandlersTable::iterator mapIt,
                                              CClientEntity* sourceEntity, CClientEntity* entity, const CLuaArguments& args, const std::string_view& eventName)
{
    if (handlers.empty())
        return;

    // TIMEUS  startTimeCall = GetTimeUs();
    // SString strStatus;

    const bool isDirectSource = sourceEntity == entity;
    const auto entityType = entity->GetType();
    bool       removedDuringCallback = false;

    TIMEUS startTimeCall = 0;
    if (IS_TIMING_CHECKPOINTS())
        startTimeCall = GetTimeUs();

    for (auto& handler : handlers)
    {
        if (!handler.isValid) [[unlikely]]
            continue;

        if (!isDirectSource && !handler.propagate) [[unlikely]]
            continue;

        if (handler.entityType != eClientEntityType::CCLIENTUNKNOWN && entityType != handler.entityType)
            continue;

        handler.isInUse = true;

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

        int result = luaMain->PCall(luaVM, 6 + args.Count(), 0, 0);
        if (result > 1 && result != LUA_ERRSYNTAX)
            g_pClientGame->GetScriptDebugging()->LogPCallError(luaVM, ConformResourcePath(lua_tostring(luaVM, -1)));
        else
            // CClientPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(luaMain, luaMain->GetFunctionTag(handler.luaFunctionRef.ToInt()),
            //                                                           GetTimeUs() - startTime);

            lua_settop(luaVM, preCallTop);

        // TODO g_pClientGame->GetDebugHookManager()->OnPostEventFunction

        // Aspect ratio adjustment bodges
        if (handler.isRenderingEvent)
        {
            g_pCore->GetGraphics()->SetAspectRatioAdjustmentEnabled(false);
            m_callingRenderEvent = false;
        }

        if (!handler.isValid)
            removedDuringCallback = true;

        handler.isInUse = false;
    }

    if (IS_TIMING_CHECKPOINTS())
    {
        TIMEUS deltaTimeUs = GetTimeUs() - startTimeCall;
        if (deltaTimeUs > 5000)
            TIMING_DETAIL(SString("CEventsManager::ExecuteHandlersForEntity for %s took %d ms", eventName.data(), deltaTimeUs / 1000));
    }

    // To avoid searching through the entire list with erase, we first check whether there is anything to remove
    if (removedDuringCallback)
        TryRemoveHandler(entity, handlers, handlersTable, mapIt);
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
    std::vector<CLuaFunctionRef> result{};
    EventHandlersTable*          handlersTable = nullptr;

    if (std::holds_alternative<BuiltInEvent::Enum>(event))
    {
        auto builtInEnum = std::get<BuiltInEvent::Enum>(event);
        handlersTable = &m_eventsTable[static_cast<std::size_t>(builtInEnum)];
    }
    else
    {
        auto hash = std::get<std::uint32_t>(event);
        auto it = m_customEvents.find(hash);
        if (it != m_customEvents.end())
            handlersTable = &it->second.handlersTable;
    }

    if (!handlersTable)
        return result;

    auto it = handlersTable->find(sourceEntity);
    if (it != handlersTable->end())
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

BuiltInEvent::Enum CEventsManager::getBuiltInEventIDFromName(const std::string_view& name)
{
    auto it = m_eventNameToId.find(name);
    return it != m_eventNameToId.end() ? it->second : BuiltInEvent::MAX_EVENTS;
}
