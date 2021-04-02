#pragma once
#include <vector>

class Event;
class CLuaArguments;
class CElement;
class CPlayer;

class EventDispatcher
{
public:
	bool Call(const Event& event, const CLuaArguments& args, CElement* source, CPlayer* client);

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
