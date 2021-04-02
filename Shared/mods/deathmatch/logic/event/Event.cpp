#include <StdInc.h>
#include "CustomEvents.h"

const Event* Event::Get(const std::string& name)
{
    if (auto builtin = BuiltInEvent::Get(name))
        return builtin;
    return CustomEvent::Get(name);
}
