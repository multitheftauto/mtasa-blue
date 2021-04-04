#include <StdInc.h>

#include "EventDispatcher.h"

#include "Event.h"
#include <logic/CPlayer.h>
#include <logic/CElement.h>

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

bool EventDispatcher::Call(const Event& event, const CLuaArguments& args, CElement* source, CPlayer* client)
{
	if (!g_pGame->GetDebugHookManager()->OnPreEvent(event.GetName(), args, source, client))
	    return false;
	PreEventPulse();

    // Call all event handlers (us, ancestors, children)
    {
        const auto CallHandlers = [&](auto* elem) {
            elem->GetEventHandlerCallDispatcher().Emmit(event, args, source, elem, client);
        };
        CallHandlers(source);
        source->IterAncestorsThenChildren(CallHandlers);
    }

	PostEventPulse();
    g_pGame->GetDebugHookManager()->OnPostEvent(event.GetName(), args, source, client);

	return !WasEventCancelled();
}
