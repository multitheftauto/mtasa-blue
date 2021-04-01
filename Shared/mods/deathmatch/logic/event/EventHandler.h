#pragma once
#include <lua/CLuaFunctionRef.h>

#include <string_view>

class CLuaMain;
class CPlayer;
class CElement;
class Event;
class CLuaArguments;

struct EventHandler
{
    struct Priority
    {
        enum class Level
        {
            LOW,
            NORMAL,
            HIGH
        };

        Priority() = default;
        Priority(std::string_view value);
        Priority(Level l, float m) : m_lvl(l), m_mod(m) {}

        friend bool operator<(const Priority& l, const Priority& r)
        {
            if (l.m_lvl == r.m_lvl)
                return l.m_mod < r.m_mod;
            return l.m_lvl < r.m_lvl;
        }
        friend bool operator>(const Priority& l, const Priority& r) { return r < l; }
        friend bool operator<=(const Priority& l, const Priority& r) { return !(l > r); }
        friend bool operator>=(const Priority& l, const Priority& r) { return !(l < r); }

    protected:
        Level m_lvl = Level::LOW;
        float m_mod = 0.0f;
    };
    
    EventHandler(Priority priority, CLuaMain* lmain, const CLuaFunctionRef& fn, bool handlePropagated) :
        m_priority(priority),
        m_lmain(lmain),
        m_fn(fn),
        m_handlesPropagated(handlePropagated)
    {
    }

    CLuaMain* GetLuaMain() const { return m_lmain; }
    const auto& GetCallback() const { return m_fn; }

    const auto& GetPriority() const { return m_priority; }

    bool CanBeCalled() const;

    void operator()(const Event& event, const CLuaArguments& args, CElement* source, CElement* us, CPlayer* client) const;
    //bool CanBeCalled() const { return !IsBeingDeleted(); }  TODO: Add check if m_fn is valid (VERIFY_FUNCTION)
protected:
    Priority m_priority{};

    CLuaMain* m_lmain = nullptr;
    CLuaFunctionRef m_fn{};

    bool m_handlesPropagated = false;
};

