#include <StdInc.h>
#include "CustomEvents.h"

const Event* Event::Get(const std::string& name)
{
    dassert(CustomEvent::Get(name) && BuiltInEvent::Get(name)); // Event can't be custom and built-in at the same time

    if (auto* custom = CustomEvent::Get(name))
        return custom;
    return BuiltInEvent::Get(name);
}
