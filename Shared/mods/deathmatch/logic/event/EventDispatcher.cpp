#include <StdInc.h>

#include "EventDispatcher.h"

#include "Event.h"
#include <logic/CPlayer.h>
#include <logic/CElement.h>


void DispatchTo(CElement* to, const Event& event, const CLuaArguments& args, CElement* source, CPlayer* client)
{
    to->GetEventHandlerCallDispatcher().Emmit(event, args, source, to, client);
}

void DispatchToParents(CElement* current, const Event& event, const CLuaArguments& args, CElement* source, CPlayer* client)
{
    if (!current)
        return;
    DispatchTo(current, event, args, source, client);
    DispatchToParents(current->GetParentEntity(), event, args, source, client);
}

void DispatchToChildren(CElement* parent, const Event& event, const CLuaArguments& args, CElement* source, CPlayer* client)
{
    if (!parent->CountChildren()) 
        return; // Doesn't have any alive children

    auto snapshot = parent->GetChildrenListSnapshot();
    snapshot->AddRef();
    for (auto child : *snapshot)
    {
        if (parent->IsBeingDeleted())
            continue; // Event handler might've destroyed us
        if (child->IsBeingDeleted())
            continue; // ...or our children
        DispatchTo(child, event, args, source, client);
        DispatchToChildren(child, event, args, source, client);
    }
    snapshot->Release();
}

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
	//if (!g_pGame->GetDebugHookManager()->OnPreEvent(szName, Arguments, this, pCaller))
	//	return false;
	PreEventPulse();

    DispatchToParents(source, event, args, source, client); // Also calls event on source
    DispatchToChildren(source, event, args, source, client);

	PostEventPulse();
	return !WasEventCancelled();
}
