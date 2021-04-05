#pragma once
#include <SharedUtil.Template.h>
#include <vector>
#include <string>

class Event;
class CLuaArguments;
class CElement;
class CPlayer;

class EventDispatcher
{
public:
    bool Call(const Event& event, const CLuaArguments& args, const bool callOnChildren, CElement* source SERVER_ONLY_ARG(CPlayer* client));

	void PreEventPulse();
	void PostEventPulse();

    void CancelEvent(bool cancelled) { m_eventCancelled = cancelled; }
    void CancelEvent(bool cancelled, std::string reason);
	bool WasEventCancelled() const { return m_wasEventCancelled; }

    const std::string& GetCancelReason() const { return m_cancelReason; }
protected:
	bool m_eventCancelled = false;
	bool m_wasEventCancelled = false;
	std::vector<bool> m_cancelStack;
    std::string m_cancelReason;
};

static inline EventDispatcher s_EventDispatcher;
