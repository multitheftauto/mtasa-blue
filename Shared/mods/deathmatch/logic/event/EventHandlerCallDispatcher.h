#pragma once

#include "BuiltInEvents.h"

#include <array>
#include <unordered_map>
#include <utility>

class EventHandlerCollection;
class CLuaMain;
class Event;
class CLuaFunctionRef;
class CustomEvent;
class BuiltInEvent;

struct EventHandlerCallDispatcher
{
    bool Remove(const Event& event, CLuaMain* lmain, const CLuaFunctionRef& fn);

    // LuaFunctionRef's are automatically invalidated when a VM closes
    // Then this function is called to remove all that
    void Remove(CLuaMain* lmain);

    void Remove(const CustomEvent& event);

    template<typename... Ts>
    void Emmit(const Event& event, Ts&&... ts) // CElement* source, CElement* us, CPlayer* client
    {
        if (auto handlers = GetHandlers(event))
            (*handlers)(event, std::forward<Ts>(ts)...);
    }
protected:
    EventHandlerCollection* GetHandlers(const Event& event, bool allowCreate = false);

    auto GetHandlers(const BuiltInEvent& event)
    {
        return std::next(m_builtins.begin(), static_cast<size_t>(event.GetID()));
    }

    auto GetHandlers(const CustomEvent& event)
    {
        if (auto it = m_custom.find(&event); it != m_custom.end())
            return std::make_pair(it, true);
        return std::make_pair(m_custom.end(), false);
    }

    template<typename Fn>
    void ForAll(Fn fn)
    {
        for (const auto& c : m_builtins)
            fn(c);
        for (const auto& [k, c] : m_custom)
            fn(c);
    }

    std::array<EventHandlerCollection, BuiltInEvents::COUNT> m_builtins{};
    std::unordered_map<const CustomEvent*, EventHandlerCollection> m_custom;
};

