#include "EventHandlerCallDispatcher.h"

#include "EventHandlerCollection.h"
#include "Event.h"

bool EventHandlerCallDispatcher::Remove(const Event& event, CLuaMain* lmain, const CLuaFunctionRef& fn)
{
    bool out = false;
    ForAll([&](EventHandlerCollection& c) {
        out |= c.Remove(lmain, fn);
    });
    return out;
}

// LuaFunctionRef's are automatically invalidated when a VM closes
// Then this function is called to remove all that
void EventHandlerCallDispatcher::Remove(CLuaMain* lmain)
{
    ForAll([lmain](EventHandlerCollection& c) {
        c.Remove(lmain);
    });
}

void EventHandlerCallDispatcher::Remove(const CustomEvent& event) // TODO
{

}
