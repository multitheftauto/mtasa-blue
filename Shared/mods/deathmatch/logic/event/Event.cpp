#include <StdInc.h>
#include "CustomEvents.h"

const Event* Event::Get(const std::string& name)
{
    dassert(!CustomEvent::Get(name) || !BuiltInEvent::Get(name)); // Event with the same name can't "exist twice"
    if (auto* custom = CustomEvent::Get(name))
        return custom;
    return BuiltInEvent::Get(name);
}
