#include <StdInc.h>

#include "EventHandler.h"
#include "Event.h"

#include <stdexcept>
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
        m_mod = std::stof(std::string{ value.data(), at }); // TODO: replace this with from_chars when GCC is updated from 10.2
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

void EventHandler::operator()(const Event& event, const CLuaArguments& args, CElement* source, CElement* us, CPlayer* client)
{
    if (!m_handlesPropagated && source != us)
        return;

    if (!GetGame()->GetDebugHookManager()->OnPreEventFunction(event.GetName(), args, source, client, *this))
        return;

    const bool wasDeletable = m_canBeDeleted;
    m_canBeDeleted = false; // Keep this object alive (Lua callback could call removeEventHandler)

    const auto timeBeginUS = GetTimeUs();

    lua_State* L = GetLuaMain()->GetVM();
    LUA_CHECKSTACK(L, 1);
    LUA_STACK_EXPECT(0);

    if (m_lmain) // As per old code.. Pretty sure this can never happen
    {
        const auto PushFn = [L](int ref) {
            lua_getref(L, ref);
            assert(lua_type(L, -1) == LUA_TFUNCTION);
        };
        // Based on code from CLuaArguments::Call

        PushFn(m_lmain->GetDispatchEventFnRef());

        // DispatchEvent Push args
        PushFn(m_fn.ToInt()); // handlerfn
        lua::Push(L, source); // source
        lua::Push(L, us); // this
        lua::Push(L, client); // client

        // sourceResource and sourceResourceRoot
        if (auto topLuaMain = GetGame()->GetScriptDebugging()->GetTopLuaMain())
        {
            auto sourceResource = topLuaMain->GetResource();
            lua::Push(L, sourceResource);
            lua::Push(L, sourceResource->GetResourceRootElement());
        }
        else
        {
            lua::Push(L, nullptr);
            lua::Push(L, nullptr);
        }

        lua::Push(L, event.GetName()); // eventName
        args.PushArguments(L); // push handler variadic fn args

        m_lmain->ResetInstructionCount();

        switch (m_lmain->PCall(L, 7 + args.Count(), 0, 0)) // 7 args for DispatchEvent + variadic (args.Count())
        {
        case LUA_ERRRUN:
        case LUA_ERRMEM:
        {
            GetGame()->GetScriptDebugging()->LogPCallError(L, ConformResourcePath(lua_tostring(L, -1)));
            break;
        }
        default: // Only if successful record timing
            CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(m_lmain, m_lmain->GetFunctionTag(m_fn.ToInt()), GetTimeUs() - timeBeginUS); 
        }
    }
    GetGame()->GetDebugHookManager()->OnPostEventFunction(event.GetName(), args, source, client, *this);
    m_canBeDeleted = wasDeletable;
}
