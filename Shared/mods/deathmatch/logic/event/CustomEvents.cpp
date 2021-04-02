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
