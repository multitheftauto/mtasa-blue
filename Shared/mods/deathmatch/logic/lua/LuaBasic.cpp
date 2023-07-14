/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "LuaBasic.h"

namespace lua
{
    template <>
    std::string PopPrimitive<std::string>(lua_State* L, int& index)
    {
        uint        uiLength = lua_strlen(L, index);
        std::string outValue;
        outValue.assign(lua_tostring(L, index++), uiLength);
        return outValue;
    }

    template <>
    std::string_view PopPrimitive<std::string_view>(lua_State* L, int& index)
    {
        uint             uiLength = lua_strlen(L, index);
        std::string_view outValue(lua_tostring(L, index++), uiLength);
        return outValue;
    }

    template <>
    bool PopPrimitive<bool>(lua_State* L, int& index)
    {
        return static_cast<bool>(lua_toboolean(L, index++));
    }

    template <>
    void* PopPrimitive<void*>(lua_State* L, int& index)
    {
        return lua_touserdata(L, index++);
    }

    template<>
    lua_Number PopPrimitive<lua_Number>(lua_State* L, int& index)
    {
        return lua_tonumber(L, index++);
    }

    // Int types aren't Poppable directly, because they need to be checked for under/overflow, NaN, inf
}            // namespace lua
