/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include <StdInc.h>
#include "LuaBasic.h"

namespace lua
{
    int Push(lua_State* L, int value)
    {
        lua_pushnumber(L, value);
        return 1;
    }

    int Push(lua_State* L, bool value)
    {
        lua_pushboolean(L, value);
        return 1;
    }

    int Push(lua_State* L, const std::string& value)
    {
        lua_pushlstring(L, value.data(), value.length());
        return 1;
    }


    template <>
    std::string PopTrivial<std::string>(lua_State* L, std::size_t& index)
    {
        uint        uiLength = lua_strlen(L, index);
        std::string outValue;
        outValue.assign(lua_tostring(L, index++), uiLength);
        return outValue;
    }

    template <>
    int PopTrivial<int>(lua_State* L, std::size_t& index)
    {
        return static_cast<int>(lua_tonumber(L, index++));
    }

    template <>
    void* PopTrivial<void*>(lua_State* L, std::size_t& index)
    {
        return lua_touserdata(L, index++);
    }
}            // namespace mta::impl
