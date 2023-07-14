/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "lua/LuaCommon.h"
#include "lua/CLuaArgument.h"
#include "lua/CLuaArguments.h"
#include "lua/CLuaVector2.h"
#include "lua/CLuaVector3.h"
#include "lua/CLuaVector4.h"
#include "lua/CLuaMatrix.h"
#include <optional>
#include <variant>
#include <array>

/*
    Basic Lua operations:
        void Push(L, T value)
        T PopPrimitive(L, int& stackIndex)
*/

namespace lua
{
    // PopTrival should read a simple value of type T from the stack without extra type checks
    // If whatever is at that point in the stack is not convertible to T, the behavior is undefined
    template <typename T>
    inline T PopPrimitive(lua_State* L, int& index);

    // Push should push a value of type T to the Lua Stack
    // This will always increase the stack size by 1
    inline void Push(lua_State* L, int value) { lua_pushnumber(L, value); }
    inline void Push(lua_State* L, unsigned int value) { lua_pushnumber(L, value); }
    inline void Push(lua_State* L, float value) { lua_pushnumber(L, value); }
    inline void Push(lua_State* L, double value) { lua_pushnumber(L, value); }

    inline void Push(lua_State* L, bool value) { lua_pushboolean(L, value); }

    inline void Push(lua_State* L, std::nullptr_t) { lua_pushnil(L); }

    inline void Push(lua_State* L, const char* value) { lua_pushstring(L, value); }

    inline void Push(lua_State* L, const std::string& value) { lua_pushlstring(L, value.data(), value.length()); }

    inline void Push(lua_State* L, const std::string_view& value) { lua_pushlstring(L, value.data(), value.length()); }

    inline void Push(lua_State* L, const CLuaArgument& arg)
    {
        if (arg.GetType() == LUA_TNONE)
        {
            // Calling lua::Push with a LUA_TNONE type value is not allowed, since this is rather error-prone
            // as many callers would not check the stack position after pushing.
        }

        arg.Push(L);
    }

    inline void Push(lua_State* L, const CLuaArguments& args) { args.PushAsTable(L); }

    inline void Push(lua_State* L, const CVector2D& value) { lua_pushvector(L, value); }

    inline void Push(lua_State* L, const CVector& value) { lua_pushvector(L, value); }

    inline void Push(lua_State* L, const CVector4D& value) { lua_pushvector(L, value); }

    inline void Push(lua_State* L, const CMatrix& value) { lua_pushmatrix(L, value); }

    // Script entities
    #ifdef MTA_CLIENT
    inline void Push(lua_State* L, const CClientEntity* value) { lua_pushelement(L, const_cast<CClientEntity*>(value)); }
    #else
    inline void Push(lua_State* L, const CElement* value) { lua_pushelement(L, const_cast<CElement*>(value)); }
    inline void Push(lua_State* L, const CAccount* value) { lua_pushaccount(L, const_cast<CAccount*>(value)); }
    inline void Push(lua_State* L, const CAccessControlList* value) { lua_pushacl(L, const_cast<CAccessControlList*>(value)); }
    inline void Push(lua_State* L, const CAccessControlListGroup* value) { lua_pushaclgroup(L, const_cast<CAccessControlListGroup*>(value)); }
    inline void Push(lua_State* L, const CBan* value) { lua_pushban(L, const_cast<CBan*>(value)); }
    inline void Push(lua_State* L, const CTextDisplay* value) { lua_pushtextdisplay(L, const_cast<CTextDisplay*>(value)); }
    inline void Push(lua_State* L, const CTextItem* value) { lua_pushtextitem(L, const_cast<CTextItem*>(value)); }
    inline void Push(lua_State* L, const CDbJobData* value) { lua_pushquery(L, const_cast<CDbJobData*>(value)); }
    #endif
    inline void Push(lua_State* L, const CResource* value) { lua_pushresource(L, const_cast<CResource*>(value)); }
    inline void Push(lua_State* L, const CXMLNode* value) { lua_pushxmlnode(L, const_cast<CXMLNode*>(value)); }
    inline void Push(lua_State* L, const CLuaTimer* value) { lua_pushtimer(L, const_cast<CLuaTimer*>(value)); }
    inline void Push(lua_State* L, const CLuaVector2D* value) { lua_pushvector(L, *value); }
    inline void Push(lua_State* L, const CLuaVector3D* value) { lua_pushvector(L, *value); }
    inline void Push(lua_State* L, const CLuaVector4D* value) { lua_pushvector(L, *value); }
    inline void Push(lua_State* L, const CLuaMatrix* value) { lua_pushmatrix(L, *value); }

    // Overload for enum types only
    template <typename T>
    typename std::enable_if_t<std::is_enum_v<T>> Push(lua_State* L, const T& val)
    {
        // Push<string> must be defined before this function, otherwise it wont compile
        Push(L, EnumToString(val));
    }

    template <typename T>
    void Push(lua_State* L, const std::shared_ptr<T>& ptr)
    {
        Push(L, ptr.get());
    }

    template <typename T>
    void Push(lua_State* L, const std::unique_ptr<T>& ptr)
    {
        Push(L, ptr.get());
    }

    /*****************************************************************\
    * The functions below may depend on each other, so they need to be
    * forward declared.
    * Please declare functions that call `Push` after this line.
    \*****************************************************************/

    template <typename... Ts>
    void Push(lua_State* L, const std::variant<Ts...>& val);

    template <typename T>
    void Push(lua_State* L, const std::optional<T>& val);

    template <typename T, size_t N>
    void Push(lua_State* L, const std::array<T, N>& val);

    template <typename T>
    void Push(lua_State* L, const std::vector<T>& val);

    template <typename K, typename V>
    void Push(lua_State* L, const std::unordered_map<K, V>& val);

    template <typename... Ts>
    void Push(lua_State* L, const std::tuple<Ts...>& tuple);

    // Define after this line, declare above.

    template <typename... Ts>
    void Push(lua_State* L, const std::variant<Ts...>& val)
    {
        std::visit([L](const auto& value) { Push(L, value); }, val);
    }

    template <typename T>
    void Push(lua_State* L, const std::optional<T>& val)
    {
        if (val.has_value())
            Push(L, val.value());
        else
            Push(L, nullptr);
    }

    template <typename T, size_t N>
    void Push(lua_State* L, const std::array<T, N>& val)
    {
        lua_createtable(L, N, 0);
        int i = 1;
        for (const auto& v : val)
        {
            Push(L, v);
            lua_rawseti(L, -2, i++);
        }
    }

    template <typename T>
    void Push(lua_State* L, const std::vector<T>& val)
    {
        lua_newtable(L);
        int i = 1;
        for (const auto& v : val)
        {
            Push(L, i++);
            Push(L, v);
            lua_settable(L, -3);
        }
    }

    template <typename K, typename V>
    void Push(lua_State* L, const std::unordered_map<K, V>& val)
    {
        lua_newtable(L);
        for (const auto& [k, v] : val)
        {
            Push(L, k);
            Push(L, v);
            lua_settable(L, -3);
        }
    }

    // Tuples can be used to push fixed-size tables
    // e.g. `std::tuple<float, int, bool>` will be pushed as { float, int, bool }
    template <typename... Ts>
    void Push(lua_State* L, const std::tuple<Ts...>& tuple)
    {
        // Call Push on each element of the tuple
        lua_createtable(L, sizeof...(Ts), 0);
        std::apply(
            [L](const auto&... values) {
                int  key = 1;
                auto PushTable = [](lua_State* L, int& key, const auto& value) {
                    Push(L, value);
                    lua_rawseti(L, -2, key++);
                };

                (PushTable(L, key, values), ...);
            },
            tuple);
    }
}            // namespace lua
