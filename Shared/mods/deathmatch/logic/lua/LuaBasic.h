/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <optional>
#include <variant>

/*
    Basic Lua operations:
        int Push(L, T value)
        T PopTrivial(L, std::size_t stackIndex)
        bool TypeMatch(L, std::size_t stackIndex)
*/


namespace lua
{
    // PopTrival should read a simple value of type T from the stack without extra type checks
    // If whatever is at that point in the stack is not convertible to T, the behavior is undefined
    template <typename T>
    inline T PopTrivial(lua_State* L, std::size_t& index);


    // Push should push a value of type T to the Lua Stack
    // The return value must be the amount of items pushed to the stack, which should
    // be 1 for trivial types (e.g. Push<int>) but may be any number for special cases
    // like tuples
    
    int Push(lua_State* L, int val);
    int Push(lua_State* L, const std::string& val);
    int Push(lua_State* L, bool val);
    int Push(lua_State* L, std::nullptr_t);
    int Push(lua_State* L, float val);
    int Push(lua_State* L, double val);
    int Push(lua_State* L, unsigned int val);
    int Push(lua_State* L, unsigned short val);


    template <typename... Ts>
    int Push(lua_State* L, const std::variant<Ts...>&& val)
    {
        return std::visit([L](auto&& value) -> int { return Push(L, value); }, val);
    }

    template <typename T>
    int Push(lua_State* L, const std::optional<T>&& val)
    {
        if (val.has_value())
            return Push(L, val.value());
        else
            return Push(L, nullptr);
     }

    template <typename T>
    int Push(lua_State* L, const std::vector<T>&& val)
    {
        lua_newtable(L);
        int i = 1;
        for (auto&& v : val)
        {
            Push(L, i++);
            Push(L, v);
            lua_settable(L, -3);
        }
        return 1;
    }

    template <typename K, typename V>
    int Push(lua_State* L, const std::unordered_map<K, V>&& val)
    {
        lua_newtable(L);
        for (auto&& [k, v] : val)
        {
            Push(L, k);
            Push(L, v);
            lua_settable(L, -3);
        }
        return 1;
    }

    template <typename T>
    typename std::enable_if_t<std::is_enum_v<T>, int> Push(lua_State* L, const T&& val)
    {
        return Push(L, EnumToString(val));
    }

    template <typename T>
    typename std::enable_if_t<(std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>>), int> Push(lua_State* L, const T&& val)
    {
        lua_pushelement(L, val);
        return 1;
    }

}
