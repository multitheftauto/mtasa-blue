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
#include <array>

/*
    Basic Lua operations:
        int Push(L, T value)
        T PopPrimitive(L, std::size_t stackIndex)
*/

class CVector2D;
class CVector;
class CVector4D;

namespace lua
{
    // PopTrival should read a simple value of type T from the stack without extra type checks
    // If whatever is at that point in the stack is not convertible to T, the behavior is undefined
    template <typename T>
    inline T PopPrimitive(lua_State* L, std::size_t& index);

    // Push should push a value of type T to the Lua Stack
    // The return value must be the net amount of items pushed to the stack, which should
    // be 1 for most types (e.g. Push<int>) but may be any number for special cases
    // like tuples, in order to allow returning multiple values from a function
    inline int Push(lua_State* L, int value)
    {
        lua_pushnumber(L, value);
        return 1;
    }
    inline int Push(lua_State* L, unsigned int value)
    {
        lua_pushnumber(L, value);
        return 1;
    }
    inline int Push(lua_State* L, float value)
    {
        lua_pushnumber(L, value);
        return 1;
    }
    inline int Push(lua_State* L, double value)
    {
        lua_pushnumber(L, value);
        return 1;
    }
    inline int Push(lua_State* L, long long int value)
    {
        lua_pushnumber(L, value);
        return 1;
    }

    inline int Push(lua_State* L, bool value)
    {
        lua_pushboolean(L, value);
        return 1;
    }

    inline int Push(lua_State* L, nullptr_t)
    {
        lua_pushnil(L);
        return 1;
    }

    inline int Push(lua_State* L, const std::string& value)
    {
        lua_pushlstring(L, value.data(), value.length());
        return 1;
    }

    inline int Push(lua_State* L, const CLuaArgument& arg)
    {
        if (arg.GetType() == LUA_TNONE)
            return 0;

        arg.Push(L);
        return 1;
    }

    inline int Push(lua_State* L, const CLuaArguments& args)
    {
        args.PushAsTable(L);
        return 1;
    }

    inline int Push(lua_State* L, const CVector2D& value)
    {
        lua_pushvector(L, value);
        return 1;
    }

    inline int Push(lua_State* L, const CVector& value)
    {
        lua_pushvector(L, value);
        return 1;
    }

    inline int Push(lua_State* L, const CVector4D& value)
    {
        lua_pushvector(L, value);
        return 1;
    }

    inline int Push(lua_State* L, const CMatrix& value)
    {
        lua_pushmatrix(L, value);
        return 1;
    }
    
    inline int Push(lua_State* L, std::shared_ptr<CLuaPhysicsShape> value)
    {
        lua_pushshape(L, value);
        return 1;
    }
    
    inline int Push(lua_State* L, CLuaPhysicsShape* value)
    {
        lua_pushshape(L, value);
        return 1;
    }
    
    inline int Push(lua_State* L, CLuaPhysicsRigidBody* value)
    {
        lua_pushrigidbody(L, value);
        return 1;
    }
    
    inline int Push(lua_State* L, CLuaPhysicsStaticCollision* value)
    {
        lua_pushstaticcollision(L, value);
        return 1;
    }
    
    inline int Push(lua_State* L, CLuaPhysicsConstraint* value)
    {
        lua_pushconstraint(L, value);
        return 1;
    }

    // Overload for enum types only
    template <typename T>
    typename std::enable_if_t<std::is_enum_v<T>, int> Push(lua_State* L, const T& val)
    {
        // Push<string> must be defined before this function, otherwise it wont compile
        return Push(L, EnumToString(val));
    }

    // Overload for pointers to classes. We boldly assume that these are script entities
    template <typename T>
    std::enable_if_t<std::is_class_v<T>, int> Push(lua_State* L, T* val)
    {
        lua_pushelement(L, val);
        return 1;
    }

    template <typename T>
    int Push(lua_State* L, const std::shared_ptr<T>& ptr)
    {
        lua_pushelement(L, ptr.get());
        return 1;
    }

    template <typename T>
    int Push(lua_State* L, const std::unique_ptr<T>& ptr)
    {
        lua_pushelement(L, ptr.get());
        return 1;
    }

    /*****************************************************************\
    * The functions below may depend on each other, so they need to be
    * forward declared.
    * Please define functions that call `Push` after this line.
    \*****************************************************************/

    template <typename... Ts>
    int Push(lua_State* L, const std::variant<Ts...>& val);

    template <typename T>
    int Push(lua_State* L, const std::optional<T>& val);

    template <typename T, size_t N>
    int Push(lua_State* L, const std::array<T, N>& val);

    template <typename T>
    int Push(lua_State* L, const std::vector<T>& val);

    template <typename K, typename V>
    int Push(lua_State* L, const std::unordered_map<K, V>& val);

    template<typename... Ts>
    int Push(lua_State* L, const std::tuple<Ts...>& tuple);

    // Define after this line, declare above.

    template <typename... Ts>
    int Push(lua_State* L, const std::variant<Ts...>& val)
    {
        return std::visit([L](const auto& value) -> int { return Push(L, value); }, val);
    }

    template <typename T>
    int Push(lua_State* L, const std::optional<T>& val)
    {
        if (val.has_value())
            return Push(L, val.value());
        else
            return Push(L, nullptr);
    }

    template <typename T, size_t N>
    int Push(lua_State* L, const std::array<T, N>& val)
    {
        lua_createtable(L, N, 0);
        lua_Number i = 1;
        for (const auto& v : val)
        {
            Push(L, v);
            lua_rawseti(L, -2, i++);
        }

        // Only the table remains on the stack
        return 1;
    }

    template <typename T>
    int Push(lua_State* L, const std::vector<T>& val)
    {
        lua_newtable(L);
        int i = 1;
        for (const auto& v : val)
        {
            Push(L, i++);
            Push(L, v);
            lua_settable(L, -3);
        }

        // Only the table remains on the stack
        return 1;
    }

    template <typename K, typename V>
    int Push(lua_State* L, const std::unordered_map<K, V>& val)
    {
        lua_newtable(L);
        for (const auto& [k, v] : val)
        {
            Push(L, k);
            Push(L, v);
            lua_settable(L, -3);
        }

        // Only the table remains on the stack
        return 1;
    }

    // Tuples can be used to return multiple results
    template<typename... Ts>
    int Push(lua_State* L, const std::tuple<Ts...>& tuple)
    {
        // Call Push on each element of the tuple
        std::apply([L](const auto&... value) { (Push(L, value), ...); }, tuple);
        return sizeof...(Ts);
    }
}
