/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CEventsManager.h
 *
 *****************************************************************************/

#pragma once

#include "lua/CLuaArguments.h"
#include "enums/BuiltInEvent.h"
#include <vector>

class CClientEntity;
class CLuaMain;
class CTimingBlock;

struct SEventHandler
{
    CLuaMain*                          luaMain{};
    CLuaFunctionRef                    luaFunctionRef{};
    bool                               isValid{true};
    bool                               propagate{true};
    EEventPriority::EEventPriorityType priority{EEventPriority::NORMAL};
    float                              priorityMod{0.0f};
    bool                               isInUse{false};
    eClientEntityType                  entityType{eClientEntityType::CCLIENTUNKNOWN};
    bool                               isRenderingEvent{false};
    CTimingBlock*                      eventTiming{nullptr};
    CTimingBlock*                      resourceTiming{nullptr};

    bool operator<(const SEventHandler& other) const
    {
        if (priority != other.priority)
            return static_cast<int>(priority) > static_cast<int>(other.priority);

        return priorityMod > other.priorityMod;
    }
};

using EventHandlersList = std::vector<SEventHandler>;
using EventHandlersTable = CFastHashMap<CClientEntity*, EventHandlersList>;

struct SCustomEvent
{
    std::uint32_t      eventNameHash{0};
    std::string        eventName{};
    bool               allowRemoteTrigger{false};
    EventHandlersTable handlersTable{};
};

class CEventsManager
{
public:
    CEventsManager();
    ~CEventsManager() = default;

    bool AddEvent(const std::string& eventName, bool allowRemoteTrigger);

    void AddHandler(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity, CLuaMain* luaMain,
                    const CLuaFunctionRef& luaFunctionRef, bool propagated, EEventPriority::EEventPriorityType priority, float priorityMod,
                    eClientEntityType entityType = CCLIENTUNKNOWN);

    bool RemoveHandler(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity, CLuaMain* luaMain,
                       const CLuaFunctionRef& luaFunctionRef);
    void RemoveAllHandlers(CLuaMain* luaMain);
    void RemoveHandlersForEntity(CClientEntity* entity);

    // Use for built-in events
    bool TriggerEvent(BuiltInEvent::Enum event, CClientEntity* sourceEntity, const CLuaArguments& args, bool callOnChildren = true);

    bool TriggerCustomEvent(std::uint32_t hash, CClientEntity* sourceEntity, const CLuaArguments& args, bool callOnChildren = true);

    void CancelEvent() noexcept { m_eventCancelled = true; }
    bool WasEventCancelled() const noexcept { return m_eventCancelled; }

    const SCustomEvent*          GetCustomEvent(std::uint32_t hash) const;
    std::vector<CLuaFunctionRef> GetEventHandlers(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity, CLuaMain* luaMain,
                                                  std::optional<eClientEntityType> elementType);

    bool IsEventHandlerAttached(const std::variant<std::uint32_t, BuiltInEvent::Enum>& event, CClientEntity* sourceEntity, CLuaMain* luaMain,
                                const CLuaFunctionRef& luaFunctionRef);
    bool EventExists(std::uint32_t hash) const noexcept { return m_customEvents.find(hash) != m_customEvents.end(); }

    bool CallingRenderEvent() const noexcept { return m_callingRenderEvent; }

    static BuiltInEvent::Enum getBuiltInEventIDFromName(const std::string_view& name);

private:
    void ExecuteHandlersForEntity(EventHandlersList& handlers, EventHandlersTable& handlersTable, EventHandlersTable::iterator mapIt,
                                  CClientEntity* sourceEntity, CClientEntity* entity, const CLuaArguments& args, const std::string_view& eventName,
                                  std::uint32_t eventIdOrHash);
    void TriggerEventOnChildren(EventHandlersTable& handlersTable, CClientEntity* sourceEntity, CClientEntity* entity, const CLuaArguments& args,
                                const std::string_view& eventName, std::uint32_t eventIdOrHash);

    void TryRemoveHandler(CClientEntity* entity, EventHandlersList& handlers, EventHandlersTable& handlersTable, EventHandlersTable::iterator mapIt,
                          std::uint32_t eventIdOrHash);

    std::string_view GetEventName(BuiltInEvent::Enum event) const { return m_eventNames[static_cast<std::size_t>(event)]; }
    std::string_view GetEventName(std::uint32_t hash) const;

private:
    std::unordered_map<std::uint32_t, SCustomEvent> m_customEvents{};

    EventHandlersTable m_eventsTable[static_cast<std::size_t>(BuiltInEvent::MAX_EVENTS)]{};
    bool               m_eventCancelled{false};

    inline static std::array<std::string_view, static_cast<std::size_t>(BuiltInEvent::MAX_EVENTS)> m_eventNames{};
    inline static std::unordered_map<std::string_view, BuiltInEvent::Enum>                         m_eventNameToId{};

    bool m_callingRenderEvent{false};
};
