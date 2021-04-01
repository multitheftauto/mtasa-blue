#pragma once

#include "EventHandler.h"

#include <list>
#include <algorithm>

class Event;

// Collection of handler functions attached to the same event and element
struct EventHandlerCollection
{
    void Add(EventHandler handler);

    bool Remove(CLuaMain* lmain, const CLuaFunctionRef& fn);

    void Remove(CLuaMain* lmain)
    {
        EraseIf([lmain](const EventHandler& h) {
            return h.GetLuaMain() == lmain;
        });
    }

    bool HandleExists(CLuaMain* lmain, const CLuaFunctionRef& fn) const;

    void PushToLua(CLuaMain* lmain, lua_State* L) const; // Push handles to Lua

    bool Empty() const { return m_handlers.empty(); }

    void Emmit(const Event& event, const CLuaArguments& args, CElement* sourceElement, CElement* thisElement, CPlayer* client)
    {
        const size_t listRev = m_listRev;
        for (const auto& [revWhenAdded, handler] : m_handlers)
        {
            if (revWhenAdded > listRev) // Was it after we've started iterating?
                continue;

            // TODO
        }
    }

protected:
    template<typename Pred>
    size_t EraseIf(Pred pred) // Returns number of erased elements
    {
        const size_t size = m_handlers.size();
        m_handlers.erase(
            std::remove_if(m_handlers.begin(), m_handlers.end(), pred),
            m_handlers.end()
        );
        return m_handlers.size() - size;
    }
    size_t m_listRev = 0;

    // Pair of `m_listRev` when the handler was added + the actual handler
    std::list<std::pair<size_t, EventHandler>> m_handlers;
};
