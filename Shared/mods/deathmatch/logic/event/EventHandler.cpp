#include "EventHandler.h"
#include "Event.h"

#include "StdInc.h"

#include <stdexcept>
#include <charconv>
#include <chrono>
#include <array>

// Server/Client code compatibility crap
auto GetGame()
{
#ifdef MTA_CLIENT
    return g_pClientGame;
#else
    return g_pGame;
#endif
}

// Parse strings in format <level><[+-]modifier>. Eg.:
// Value     |  Priority    |   Modifier
// low+1     |  low         |   1
// high+1    |  high        |   1
// high+     |  high        |   0 (atof retruns 0)
// high-     |  high        |   same as above
// +1        |  error       |   error
// high      |  high        |   0
EventHandler::Priority::Priority(std::string_view value)
{
    std::string_view priority = value;
    if (auto at = value.find_first_of("+-"); at != std::string_view::npos) // find +-
    {
        if (at == 0)
            throw std::invalid_argument("Missing priority level in priority string");
        priority = value.substr(at);
        std::from_chars(value.data(), value.data() + at, m_mod);
        // NOTE: from_chars might fail, in which case m_mod remains 0.0f
        // ideally we would throw here but we have to remain backwards compatible.
    }

    //if (!StringToEnum(priorityStr, m_lvl))
    //    throw std::invalid_argument("Unknown priority level"); // TODO: Include string with std::format
}

void EventHandler::operator()(const Event& event, const CLuaArguments& args, CElement* source, CElement* us, CPlayer* client) const
{
    using namespace std::chrono;
    using namespace std::chrono_literals;
    using namespace std::string_view_literals;

    if (!m_lmain)
        return;

    const auto begin = high_resolution_clock::now();

    //if (!GetGame()->GetDebugHookManager()->OnPreEventFunction(event.GetName().c_str(), args, source, client, nullptr)) // TODO
        //return;

    lua_State* L = GetLuaMain()->GetVM();
    LUA_CHECKSTACK(L, 1);

    // Save globals
    constexpr std::array<const char*, 6> globalsToSave = {
        "source", "this", "sourceResource", "sourceResourceRoot", "eventName", "client"
    };
    std::array<int, globalsToSave.size()> savedGlobalRefs = {};

    {
        size_t i = 0;
        for (const char* name : globalsToSave)
        {
            lua_getglobal(L, name);
            savedGlobalRefs[i] = lua_ref(L, true);
        }
    }

    // Set new globals
    {
        const auto SetGlobal = [L](auto name, auto value) {
            lua::Push(L, value);
            lua_setglobal(L, name);
        };

        SetGlobal("source", source);
        SetGlobal("this", us);
        SetGlobal("client", client);
        SetGlobal("eventName", event.GetName());

        if (auto topLuaMain = GetGame()->GetScriptDebugging()->GetTopLuaMain())
        {
            auto sourceResource = topLuaMain->GetResource();
            SetGlobal("sourceResource", sourceResource);
            SetGlobal("sourceResourceRoot", sourceResource->GetResourceRootElement());
        }
        else
        {
            SetGlobal("sourceResource", nullptr);
            SetGlobal("sourceResourceRoot", nullptr);
        }
    }

    args.Call(m_lmain, m_fn);

    // Reset globals
    {
        size_t i = 0;
        for (int ref : savedGlobalRefs)
        {
            lua_getref(L, ref);
            lua_setglobal(L, globalsToSave[i]);
            lua_unref(L, ref);
        }
    }
}

bool EventHandler::CanBeCalled() const
{
    return true; // TODO call VERIFY_FUNCTION here
}

