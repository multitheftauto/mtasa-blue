#pragma once

#include "Event.h"

#include <unordered_map>
#include <memory>
#include <string_view>
#include <string>

class CLuaMain;

// Handles `CustomEvent`s only
// For built-in events see namespace `BuiltInEvents`
// This class manages the lifetime of `CustomEvent`'s.
// You can create CustomEvent's here, get them by name, etc..
class CustomEvents
{
public:
    // TODO: Implement CustomEventIDArray so we can eliminate the need of
    // always using Event* => HandlerCollection

    const CustomEvent* Get(const std::string& name)
    {
        if (auto it = m_events.find(name); it != m_events.end())
            return it->second.get();
        return nullptr;
    }

    bool Exists(const std::string& name) { return Get(name); }

    template<typename... Ts>
    bool Add(std::string name, Ts&&... ts)
    {
        if (BuiltInEvent::Get(name)) // Can't register a built in event
            return false;
        // TODO: maybe move this string resize stuff to the API side?
        if (name.length() > MAX_EVENT_NAME_LENGTH)
            name.resize(MAX_EVENT_NAME_LENGTH);
        if (auto [it, success] = m_events.emplace(name, nullptr); success)
        {
            it->second.reset(new CustomEvent(std::move(name), std::forward<Ts>(ts)...)); // Can't use make_unique because `CustomEvent`'s constructor is `protected`
            return true;
        }
        return false;
    }

    bool Remove(const std::string& event) { return m_events.erase(event); }

    bool RemoveAllOf(CLuaMain* owner)
    {
        for (auto it = m_events.begin(); it != m_events.end();)
        {
            if (it->second->GetCreatedBy() == owner)
                it = m_events.erase(it);
            else
                it++;
        }
    }

    // Since Event objects are interned we must ensure their addresses never change
    // so we must use unique_ptr here.
    std::unordered_map<std::string, std::unique_ptr<CustomEvent>> m_events;
};

static inline CustomEvents s_CustomEvents;
