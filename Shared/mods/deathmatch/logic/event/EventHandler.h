#pragma once
#include <lua/CLuaFunctionRef.h>
#include <SharedUtil.Misc.h>
#include <string_view>
#include <SharedUtil.Template.h>

class CLuaMain;
class CPlayer;
class CElement;
class Event;
class CLuaArguments;

class EventHandler
{
// TODO Deal with CElement define at some point, and replace it with a `using CElement = CClientEntity` OR refactor CClientEntity to be CElement
#ifdef MTA_CLIENT
#ifndef CElement 
    using CElement = CClientEntity;
#endif
#endif

public:
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
                return l.m_mod < r.m_mod; // Same level, check modifiers
            return l.m_lvl < r.m_lvl;
        }
        friend bool operator>(const Priority& l, const Priority& r) { return r < l; }
        friend bool operator<=(const Priority& l, const Priority& r) { return !(l > r); }
        friend bool operator>=(const Priority& l, const Priority& r) { return !(l < r); }

        friend bool operator==(const Priority& l, const Priority& r) { return std::tie(l.m_lvl, l.m_mod) == std::tie(r.m_lvl, r.m_mod); }
        friend bool operator!=(const Priority& l, const Priority& r) { return !(l == r); }

        std::string ToString() const;
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

    void operator()(const Event& event, const CLuaArguments& args, CElement* source, CElement* us SERVER_ONLY_ARG(CPlayer* client));
    //bool CanBeCalled() const { return !IsBeingDeleted(); }  TODO: Add check if m_fn is valid (VERIFY_FUNCTION)

    void SetListRev(size_t rev) { m_listRevWhenInserted = rev; }
    size_t GetListRev() const { return m_listRevWhenInserted; }

    friend bool operator==(const EventHandler& l, const EventHandler& r)
    {
        const auto GetTie = [](const EventHandler& h) {
            return std::tie(h.m_priority, h.m_lmain, h.m_fn, h.m_handlesPropagated);
        };
        return GetTie(l) == GetTie(r);
    }
    friend bool operator!=(const EventHandler& l, const EventHandler& r) { return !(l == r); }

    void DoMarkToBeDeleted() { m_markedToBeDeleted = true; }
    bool IsMarkedToBeDeleted() const { return m_markedToBeDeleted; }

    bool CanBeDeleted() const { return m_canBeDeleted; }
protected:
    Priority m_priority{};

    CLuaMain* m_lmain = nullptr;
    CLuaFunctionRef m_fn{};

    size_t m_listRevWhenInserted = 0;

    bool m_handlesPropagated = false;

    bool m_canBeDeleted = true;
    bool m_markedToBeDeleted = false;
};

DECLARE_ENUM_CLASS(EventHandler::Priority::Level);
