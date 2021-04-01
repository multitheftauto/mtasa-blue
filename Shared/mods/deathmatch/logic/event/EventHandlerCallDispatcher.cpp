#include "EventHandlerCallDispatcher.h"

#include "EventHandlerCollection.h"
#include "Event.h"

EventHandlerCollection* EventHandlerCallDispatcher::GetHandlers(const Event& event, bool allowCreate = false)
{
    if (event.IsBuiltIn())
    {
        const auto& builtin = static_cast<const BuiltInEvent&>(event);
        return &(*GetHandlers(builtin));
    }
    else
    {
        const auto custom = static_cast<const CustomEvent&>(event);
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
    bool out = false;
    ForAll([&](EventHandlerCollection& c) {
        out |= c.Remove(lmain, fn);
    });
    return out;
}

// LuaFunctionRef's are automatically invalidated when a VM closes
// Then this function is called to remove all that
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
