#pragma once
#include <vector>

class Event;
class CLuaArguments;
class CElement;

struct EventDispatcher
{
	bool Call(const Event& event, const CLuaArguments& args, CElement* source);

	void PreEventPulse();
	void PostEventPulse();

	void CancelEvent(bool cancelled) { m_eventCancelled = cancelled; }
	bool WasEventCancelled() const { return m_wasEventCancelled; }

protected:
	bool m_eventCancelled = false;
	bool m_wasEventCancelled = false;
	std::vector<bool> m_cancelStack;
};

static inline EventDispatcher s_EventDispatcher;