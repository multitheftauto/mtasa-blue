#include <StdInc.h>

#include "EventHandler.h"
#include "Event.h"
using namespace SharedUtil;

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
    if (value.empty())
        return; // Variables are initalized at this to default values

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
    if (!StringToEnum(std::string{priority}, m_lvl)) // TODO: Use std::less<> in StringToEnum map, thus enabling transparent lookup
        throw std::invalid_argument("Unknown priority level"); // TODO(C++20): Include string with std::format
}

std::string EventHandler::Priority::ToString() const
{
    return SString("%s%+f", EnumToString(m_lvl).c_str(), m_mod);
}

IMPLEMENT_ENUM_CLASS_BEGIN(EventHandler::Priority::Level)
ADD_ENUM(EventHandler::Priority::Level::LOW, "low")
ADD_ENUM(EventHandler::Priority::Level::NORMAL, "normal")
ADD_ENUM(EventHandler::Priority::Level::HIGH, "high")
IMPLEMENT_ENUM_CLASS_END("EventHandlerPriority")

// TODO Benchmark this. Maybe use a Lua function to handle all this
void EventHandler::operator()(const Event& event, const CLuaArguments& args, CElement* source, CElement* us, CPlayer* client)
{
    if (!m_lmain)
        return;

    if (!m_handlesPropagated && source != us)
        return;

    const bool wasDeletable = m_canBeDeleted;
    m_canBeDeleted = false; // Keep this object alive (Lua callback could call removeEventHandler)

    lua_State* L = GetLuaMain()->GetVM();
    LUA_CHECKSTACK(L, 1);

    constexpr std::array<const char*, 6> globalsToSave = {
        "source", "this", "sourceResource", "sourceResourceRoot", "eventName", "client"
    };

    // Save globals
    std::array<CLuaArgument, globalsToSave.size()> globalValues = {};
    {
        size_t i = 0;
        for (auto name : globalsToSave)
        {
            lua_getglobal(L, name);
            globalValues[i] = { L, -1 };
        }
    }

    // Set new globals
    {
        const auto SetGlobal = [L](const auto& name, const auto& value) {
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
        for (auto& luaArg : globalValues)
        {
            luaArg.Push(L);
            lua_setglobal(L, globalsToSave[i]);
        }
    }
    m_canBeDeleted = wasDeletable;
}
