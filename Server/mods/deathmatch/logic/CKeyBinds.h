/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CKeyBinds.h
 *  PURPOSE:     Server keybind manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <set>
#include <string>


struct CKeyBind
{
    enum class BoundToState
    {
        DOWN,
        UP,
        BOTH
    };

public:
    CKeyBind(std::string val, BoundToState state, CLuaFunctionRef cbref, CLuaArguments cbargs) :
        m_boundTo(val, state),
        m_callback(cbref, cbargs)
    {}


    bool IsBeingDeleted() const noexcept { return m_isBeingDeleted; }

private:
    bool m_isBeingDeleted = false;

    struct 
    {
        std::string  value = "none"; // key, control, or whatever else
        BoundToState state = BoundToState::BOTH;
    } m_boundTo;


    // Lua things
    struct 
    {
        CLuaFunctionRef func;
        CLuaArguments   args;
    } m_callback;
};

struct CKeyBinds
{
    // Shared things
    static bool CKeyBinds::IsBindableKey(std::string_view key);

    static bool CKeyBinds::IsBindableControl(std::string_view control);

    static bool CKeyBinds::IsBindable(std::string_view value) { return IsBindableKey(value) || IsBindableControl(value); }

protected: 
    // Note std::less<> is for transparent lookup. Aka: std::string_view can be used
    // directly without converting it to an std::string
    static std::set<std::string, std::less<>> ms_bindableKeys;
    static std::set<std::string, std::less<>> ms_bindableControls;

    std::vector<>
};
