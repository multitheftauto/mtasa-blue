#include <StdInc.h>

#include "EventHandlerCollection.h"
#include <algorithm>

bool EventHandlerCollection::Add(EventHandler handler)
{
    if (HandleExists(handler))
        return false;

    const auto whereIt = std::lower_bound(m_handlers.begin(), m_handlers.end(), handler.GetPriority(),
        [](const auto& handler, const auto& priority) {
            return handler.GetPriority() > priority;
        }
    );
    const auto emplacedIt = m_handlers.emplace(whereIt, std::move(handler));
    emplacedIt->SetListRev(++m_listRev);
    return true;
}

bool EventHandlerCollection::Remove(CLuaMain* lmain, const CLuaFunctionRef& fn)
{
    return EraseIf([lmain, &fn](const EventHandler& h) {
        return h.GetLuaMain() == lmain && h.GetCallback() == fn;
    });
}

// See note in EventHandlerCallDispatcher::Remove(CLuaMain*) (caller of this function)
void EventHandlerCollection::Remove(CLuaMain* lmain)
{
    EraseIf([lmain](const EventHandler& h) {
        return h.GetLuaMain() == lmain;
    }, false);
}

bool EventHandlerCollection::HandleExists(CLuaMain* lmain, const CLuaFunctionRef& fn) const
{
    return std::find_if(m_handlers.begin(), m_handlers.end(), [lmain, &fn](const EventHandler& h) {
        return !h.IsMarkedToBeDeleted() && h.GetLuaMain() == lmain && h.GetCallback() == fn;
    }) != m_handlers.end();
}

bool EventHandlerCollection::HandleExists(const EventHandler& toFind) const
{
    return std::find_if(m_handlers.begin(), m_handlers.end(), [&toFind](const EventHandler& h) {
        return !h.IsMarkedToBeDeleted() && h == toFind;
    }) != m_handlers.end();
}

void EventHandlerCollection::PushToLua(CLuaMain* lmain, lua_State* L) const
{
    int index = 1;
    for (const auto& handler : m_handlers)
    {
        if (handler.GetLuaMain() != lmain)
            continue;
        if (handler.IsMarkedToBeDeleted())
            continue;
        lua_getref(L, handler.GetCallback().ToInt());
        lua_rawseti(L, -2, index++);
    }
}

void EventHandlerCollection::Emmit(const Event& event, const CLuaArguments& args, CElement* sourceElement, CElement* thisElement SERVER_ONLY_ARG(CPlayer* client))
{
    CLIENT_ONLY(std::string timingStats;)
    CLIENT_ONLY(const auto emmitBeginUs = GetTimeUs();)

    const size_t listRev = m_listRev;
    for (auto it = m_handlers.begin(); it != m_handlers.end();
        (it->IsMarkedToBeDeleted() && it->CanBeDeleted()) ? it = m_handlers.erase(it) : it++)
    {
        auto& handler = *it;
        if (handler.IsMarkedToBeDeleted())
            continue;
        if (handler.GetListRev() > listRev) // Was it after we've started iterating?
            continue; // Yes, this can happen if a previously called handler added it

    #ifdef MTA_CLIENT
        const auto handlerBeginUs = GetTimeUs();
        handler(event, args, sourceElement, thisElement);
        if (IS_TIMING_CHECKPOINTS())
        {
            if (auto delta = GetTimeUs() - handlerBeginUs; delta >= 3000)
                timingStats += SString(" (%s %d ms)", handler.GetLuaMain()->GetScriptName(), delta / 1000);
        }
    #else
        handler(event, args, sourceElement, thisElement, client);
    #endif
    }

#ifdef MTA_CLIENT
    if (IS_TIMING_CHECKPOINTS())
    {
        TIMEUS deltaTimeUs = GetTimeUs() - emmitBeginUs;
        if (deltaTimeUs > 5000)
            TIMING_DETAIL(SString("CMapEventManager::Call ( %s, ... ) took %d ms ( %s )", event.GetName().c_str(), deltaTimeUs / 1000, timingStats.c_str()));
    }
#endif
}
