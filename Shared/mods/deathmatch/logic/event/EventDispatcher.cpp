#include "EventDispatcher.h"

#include "Event.h"
//#include "CElement.h"


void DispatchParent(CElement* source, CElement* current, CPlayer* client)
{
	if (current)
	{
		// current call
		//DispatchParent()
	}
}

void DispatchChildren(CElement* source, CElement* current, CPlayer* client)
{
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

bool EventDispatcher::Call(const Event& event, const CLuaArguments& args, CElement* source)
{
	//if (!g_pGame->GetDebugHookManager()->OnPreEvent(szName, Arguments, this, pCaller))
	//	return false;
	PreEventPulse();



	PostEventPulse();
	return !WasEventCancelled();
}
