#include <StdInc.h>

#include "EventDispatcher.h"
#include "Event.h"

#include <SharedUtil.Template.h>

#ifdef MTA_CLIENT
#include <CClientEntity.h>
#else
#include <logic/CElement.h>
#include <logic/CPlayer.h>
#endif

void EventDispatcher::PreEventPulse()
{
	m_cancelStack.push_back(m_eventCancelled);
	m_eventCancelled = false;
	m_wasEventCancelled = false;
}

void EventDispatcher::PostEventPulse()
{
	m_wasEventCancelled = m_eventCancelled;
	m_eventCancelled = m_cancelStack.back();
	m_cancelStack.pop_back();
}

void EventDispatcher::CancelEvent(bool cancelled, std::string reason)
{
    m_eventCancelled = cancelled;
    m_cancelReason = std::move(reason);
}

// Server/Client code compatibility crap
auto GetGame()
{
#ifdef MTA_CLIENT
    return g_pClientGame;
#else
    return g_pGame;
#endif
}

bool EventDispatcher::Call(const Event& event, const CLuaArguments& args, CElement* source SERVER_ONLY_ARG(CPlayer* client))
{
	if (!GetGame()->GetDebugHookManager()->OnPreEvent(event.GetName(), args, source, SPECIFIC_CODE(nullptr, client)))
	    return false;
	PreEventPulse();

    // Call all event handlers (us, ancestors, children)
    {
        const auto CallHandlers = [&](auto* elem) {
            elem->GetEventHandlerCallDispatcher().Emmit(event, args, source, elem SERVER_ONLY_ARG(client));
        };
        CallHandlers(source);
        source->IterAncestorsThenChildren(CallHandlers);
    }

	PostEventPulse();
    GetGame()->GetDebugHookManager()->OnPostEvent(event.GetName(), args, source, SPECIFIC_CODE(nullptr, client));

	return !WasEventCancelled();
}
