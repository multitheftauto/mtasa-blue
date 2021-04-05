#include <StdInc.h>
#include "CustomEvents.h"

const CustomEvent* CustomEvent::Get(const std::string& name)
{
    return s_CustomEvents.Get(name);
}

bool CustomEvent::Add(std::string name, CLuaMain* lmain, bool allowRemoteTrigger)
{
    return s_CustomEvents.Add(std::move(name), lmain, allowRemoteTrigger);
}

void CustomEvents::OnEventRemove(const CustomEvent& event)
{
    // Remove all refs to this event
    SPECIFIC_CODE(g_pClientGame->GetRootEntity(), g_pGame->GetMapManager()->GetRootElement())->IterChildren([&](CElement* elem) {
        elem->GetEventHandlerCallDispatcher().Remove(event);
    });
}
