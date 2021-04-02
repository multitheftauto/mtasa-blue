#include <StdInc.h>

#include "BuiltInEvents.h"
#include "BuiltInEventsListMacroApply.h"

const BuiltInEvent* BuiltInEvent::Get(BuiltInEvent::ID id)
{
    #define CASE(name, _id) case BUILTINEVENT_ID_EXPAND(_id): return &BuiltInEvents::name;

    switch (id)
    {
        BUILTINEVENT_LIST_APPLY_MACRO(CASE)
    default:
        return nullptr; // This should never be hit
    }
}

const BuiltInEvent* BuiltInEvent::Get(const std::string& name)
{
    #define KVPAIR(name, id) {#name, &BuiltInEvents::name},
    static const std::unordered_map<std::string, const BuiltInEvent*> events = {
        BUILTINEVENT_LIST_APPLY_MACRO(KVPAIR)
    };
    if (auto it = events.find(name); it != events.end())
        return it->second;
    return nullptr;
}
