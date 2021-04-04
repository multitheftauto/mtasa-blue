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

    // TODO: Fix name inconsistency (we clamp name length in `Add` but nowhere else)
    // Maybe move this crap to API side

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

    // If you decide to add removeEvent function to the API
    // remember that it might be called mid event iteration
    // and if you delete the event object it will crash
    // you will have to store the delete state, and also
    // check if the event is being delete in the EventHandlerCollection call loop
    // if it is, then stop the event iteration, and delte it
    //bool Remove(const std::string& event) { return m_events.erase(event); }

    // tl;dr; we dont have to worry about events getting deleted while they're in use
    // because resource stops are queued up and executed end frame
    void RemoveAllOf(CLuaMain* owner)
    {
        for (auto it = m_events.begin(); it != m_events.end();)
        {
            if (it->second->GetCreatedBy() == owner)
            {
                OnEventRemove(*it->second);
                it = m_events.erase(it);
            }
            else
                it++;
        }
    }

protected:
    void OnEventRemove(const CustomEvent& event);

    // Since Event objects are interned we must ensure their addresses never change
    // so we must use unique_ptr here.
    // TODO: Or std::map without unique_ptr
    std::unordered_map<std::string, std::unique_ptr<CustomEvent>> m_events;
};

static inline CustomEvents s_CustomEvents;
