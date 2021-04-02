#pragma once

#include "EventHandler.h"

#include <list>
#include <algorithm>

class Event;
struct lua_State;

// Collection of handler functions attached to the same event and element
class EventHandlerCollection
{
public:
    bool Add(EventHandler handler);

    bool Remove(CLuaMain* lmain, const CLuaFunctionRef& fn);
    void Remove(CLuaMain* lmain);

    bool HandleExists(CLuaMain* lmain, const CLuaFunctionRef& fn) const;
    bool HandleExists(const EventHandler& toFind) const;

    void PushToLua(CLuaMain* lmain, lua_State* L) const;
    bool Empty() const { return m_handlers.empty(); }
    void Emmit(const Event& event, const CLuaArguments& args, CElement* sourceElement, CElement* thisElement, CPlayer* client);
protected:
    template<typename Pred>
    bool EraseIf(Pred pred) // Returns if any handlers were erased / marked to be erased(deleted)
    {
        bool anyMatched = false;
        for (auto it = m_handlers.begin(); it != m_handlers.end();)
        { 
            if (auto& handler = *it; pred(handler))
            {
                anyMatched = true;
                if (handler.CanBeDeleted())
                {
                    it = m_handlers.erase(it);
                    continue; // Skip it++
                }
                else
                    handler.DoMarkToBeDeleted(); // Just mark it, don't erase it from the list yet as that may make us crash
            }
            it++;
        }
        return anyMatched;
    }

    // With this we can track when a handler was added.
    // Increased every time a handler is added.
    size_t m_listRev = 0;

    // TODO: If performance is crap, maybe we could use some kind of object pool allocator for better data locality?
    // NOTE: vector cant be used, because it invalidates iterators on reallocation (while we might be in the middle of iterating thru it)
    // TODO: Somehow move the `to-be-deleted` handler out of the list, because this way there has to be checks for `IsMarkedToBeDeleted`
    //
    // ACHTUNG!!: This list might contain `to-be-deleted` handler(s). This happens when the handler calls removeEventHandler.
    //
    std::list<EventHandler> m_handlers;
};
