#include "EventHandlerCollection.h"
#include <algorithm>

void EventHandlerCollection::Add(EventHandler handler)
{
    const auto whereIt = std::lower_bound(m_handlers.begin(), m_handlers.end(), handler.GetPriority(),
        [](const auto& handler, const auto& priority) {
            return handler.second.GetPriority() < priority;
        }
    );
    const auto emplacedIt = m_handlers.emplace(whereIt, m_listRev++, std::move(handler));
}

bool EventHandlerCollection::Remove(CLuaMain* lmain, const CLuaFunctionRef& fn)
{
    return EraseIf([lmain, &fn](const EventHandler& h) {
        return h.GetLuaMain() == lmain && h.GetCallback() == fn;
    });
}

bool EventHandlerCollection::HandleExists(CLuaMain* lmain, const CLuaFunctionRef& fn) const
{
    return std::find_if(m_handlers.begin(), m_handlers.end(), [lmain, &fn](const EventHandler& h) {
        return h.GetLuaMain() == lmain && h.GetCallback() == fn;
    }) != m_handlers.end();
}

void EventHandlerCollection::PushToLua(CLuaMain* lmain, lua_State* L) const // Push handles to Lua
{

}

void EventHandlerCollection::Emmit(const Event& event, const CLuaArguments& args, CElement* sourceElement, CElement* thisElement, CPlayer* client)
{
    const size_t listRev = m_listRev;
    for (const auto& [revWhenAdded, handler] : m_handlers)
    {
        if (revWhenAdded > listRev) // Was it after we've started iterating?
            continue;

        // TODO
    }
}
