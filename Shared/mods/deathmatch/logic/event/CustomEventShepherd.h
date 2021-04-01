#pragma once

#include "Event.h"

#include <unordered_map>
#include <memory>
#include <string>

class CLuaMain;

// Handles `CustomEvent`s only
// For built-in events see namespace `BuiltInEvents`
// This class manages the lifetime of `CustomEvent`'s.
// You can create CustomEvent's here, get them by name, etc..
struct CustomEventShepherd
{
    // TODO: Implement CustomEventIDArray so we can eliminate the need of
    // always using Event* => HandlerCollection

    static CustomEvent* Get(const std::string& name)
    {
        if (auto it = m_events.find(name); it != m_events.end())
            return it->second.get();
        return nullptr;
    }

    static bool Exists(const std::string& name) { return Get(name); }

    template<typename... Ts>
    static bool Add(const std::string& name, Ts&&... ts)
    {
        // todo: clamp name to 100 chars (or whatever event max name is)
        return std::get<bool>(m_events.emplace(name, std::make_unique<CustomEvent>(name, std::forward<Ts>(ts)...));
    }

    static bool Remove(const std::string& event) { return m_events.erase(event); }

    static bool RemoveAllOf(CLuaMain* owner)
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
    static std::unordered_map<std::string, std::unique_ptr<CustomEvent>> m_events;
};

static inline CustomEventShepherd s_CustomEventShepherd;
