#include <StdInc.h>

#include "EventHandlerCallDispatcher.h"
#include "EventHandlerCollection.h"
#include "Event.h"

EventHandlerCollection* EventHandlerCallDispatcher::GetHandlers(const Event& event, bool allowCreate)
{
    if (event.IsBuiltIn())
    {
        const auto& builtin = static_cast<const BuiltInEvent&>(event);
        return &(*GetHandlers(builtin));
    }
    else
    {
        const auto& custom = static_cast<const CustomEvent&>(event);
        if (allowCreate)
            return &m_custom[&custom]; // operator[] will construct object if not in map
        else
        {
            if (auto [it, success] = GetHandlers(custom); success)
                return &it->second;
            return nullptr;
        }
    }
}

bool EventHandlerCallDispatcher::Remove(const Event& event, CLuaMain* lmain, const CLuaFunctionRef& fn)
{
    if (auto handlers = GetHandlers(event); handlers)
        return handlers->Remove(lmain, fn);
    return false;
}

void EventHandlerCallDispatcher::Remove(CLuaMain* lmain)
{
    ForAll([lmain](EventHandlerCollection& c) {
        c.Remove(lmain);
    });
}

void EventHandlerCallDispatcher::Remove(const CustomEvent& event)
{
    if (auto [it, success] = GetHandlers(event); success)
        m_custom.erase(it);
}

void EventHandlerCallDispatcher::Clear()
{
    ForAll([](EventHandlerCollection& c) {
        c.Clear();
    });
}
