#include <StdInc.h>
#ifdef MTA_CLIENT
bool g_bAllowAspectRatioAdjustment = false;
#endif

#include "EventHandler.h"
#include "Event.h"

#include <lua/LuaBasic.h>
#include <lua/CLuaStackChecker.h>

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

        // Parse modifier
        m_mod = std::stof(std::string{value.substr(at)}); // TODO: replace this with from_chars when GCC is updated from 10.2
        // NOTE: from_chars might fail, in which case m_mod remains 0.0f
        // Ideally we would throw here but we have to remain backwards compatible.

        priority = value.substr(0, at);
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


void EventHandler::operator()(const Event& event, const CLuaArguments& args, CElement* source, CElement* us SERVER_ONLY_ARG(CPlayer* client))
{
    if (!m_handlesPropagated && source != us)
        return;

    const auto timeBeginUS = GetTimeUs();

#ifdef MTA_CLIENT
    const bool allowAspectRatioAdjustment = event.IsBuiltIn() && static_cast<const BuiltInEvent&>(event).AllowAspectRatioAdjustment();
    if (allowAspectRatioAdjustment)
    {
        g_bAllowAspectRatioAdjustment = true;
        using namespace std::string_view_literals;
        if (m_lmain->GetScriptName() == "customblips"sv) // customblips resource forces aspect ratio on
        {
            g_pCore->GetGraphics()->SetAspectRatioAdjustmentEnabled(true);
        }
    }

    static bool bEnabled = (g_pCore->GetDiagnosticDebug() == EDiagnosticDebug::LUA_TRACE_0000);
    if (bEnabled)
        g_pCore->LogEvent(0, "Lua Event", m_lmain->GetScriptName(), event.GetName().c_str());
#endif

    if (!GetGame()->GetDebugHookManager()->OnPreEventFunction(event.GetName(), args, source, SPECIFIC_CODE(nullptr, client), *this))
        return;

    const bool wasDeletable = m_canBeDeleted;
    m_canBeDeleted = false; // Keep this object alive (Lua callback could call removeEventHandler)

    lua_State* L = GetLuaMain()->GetVM();
    LUA_CHECKSTACK(L, 1);
    LUA_STACK_EXPECT(0);

#ifdef MTA_CLIENT
    using CPerfStatLuaTiming = CClientPerfStatLuaTiming;
#endif

    if (m_lmain) // As per old code.. Pretty sure this can never happen
    {
        // Call event handler with our wrapper function
        // the wrapper function is responsible for setting and restoring globals
        // see it's source code in EmbedLuaCode::DispatchEvent

        const int preCallTop = lua_gettop(L);

        const auto PushFn = [L](int ref) {
            lua_getref(L, ref);
            assert(lua_type(L, -1) == LUA_TFUNCTION);
        };
        // Based on code from CLuaArguments::Call

        PushFn(m_lmain->GetDispatchEventFnRef()); // Push wrapper

        // Push wrapper args
        PushFn(m_fn.ToInt()); // handlerfn
        lua::Push(L, source); // source
        lua::Push(L, us); // this
        SERVER_ONLY(lua::Push(L, client);) // client (server only)

        // sourceResource and sourceResourceRoot
        if (auto topLuaMain = GetGame()->GetScriptDebugging()->GetTopLuaMain())
        {
            auto sourceResource = topLuaMain->GetResource();
            lua::Push(L, sourceResource);
        #ifdef MTA_CLIENT
            lua::Push(L, sourceResource->GetResourceDynamicEntity());
        #else
            lua::Push(L, sourceResource->GetResourceRootElement());
        #endif
        }
        else
        {
            lua::Push(L, nullptr);
            lua::Push(L, nullptr);
        }

        lua::Push(L, event.GetName()); // eventName
        args.PushArguments(L); // push handler variadic fn args

        m_lmain->ResetInstructionCount();

    #ifdef MTA_CLIENT
        switch (m_lmain->PCall(L, 6 + args.Count(), 0, 0)) // 6 args for wrapper + variadic (args.Count())
    #else
        switch (m_lmain->PCall(L, 7 + args.Count(), 0, 0)) // 7 args for wrapper + variadic (args.Count())
    #endif
        {
        case LUA_ERRRUN:
        case LUA_ERRMEM:
        {
            GetGame()->GetScriptDebugging()->LogPCallError(L, ConformResourcePath(lua_tostring(L, -1)));
            break;
        }
        default: 
        {
            // Record timing of this function call
            CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(m_lmain, m_lmain->GetFunctionTag(m_fn.ToInt()), GetTimeUs() - timeBeginUS); 
        }
        }
        lua_settop(L, preCallTop);
    }

#ifdef MTA_CLIENT
    if (allowAspectRatioAdjustment)
    {
        g_pCore->GetGraphics()->SetAspectRatioAdjustmentEnabled(false);
        g_bAllowAspectRatioAdjustment = false;
    }
#endif

    // Record timing of this event (separatly from the function call)
    CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(m_lmain, event.GetName().c_str(), GetTimeUs() - timeBeginUS);

    GetGame()->GetDebugHookManager()->OnPostEventFunction(event.GetName(), args, source, SPECIFIC_CODE(nullptr, client), *this);
    m_canBeDeleted = wasDeletable;
}
