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
    template <>
    std::string PopPrimitive<std::string>(lua_State* L, std::size_t& index)
    {
        uint        uiLength = lua_strlen(L, index);
        std::string outValue;
        outValue.assign(lua_tostring(L, index++), uiLength);
        return outValue;
    }


    template <>
    std::string_view PopPrimitive<std::string_view>(lua_State* L, std::size_t& index)
    {
        uint        uiLength = lua_strlen(L, index);
        std::string_view outValue(lua_tostring(L, index++), uiLength);
        return outValue;
    }

    template <>
    int PopPrimitive<int>(lua_State* L, std::size_t& index)
    {
        return static_cast<int>(lua_tonumber(L, index++));
    }

    template <>
    unsigned int PopPrimitive<unsigned int>(lua_State* L, std::size_t& index)
    {
        return static_cast<unsigned int>(lua_tonumber(L, index++));
    }

    template <>
    short PopPrimitive<short>(lua_State* L, std::size_t& index)
    {
        return static_cast<short>(lua_tonumber(L, index++));
    }

    template <>
    unsigned short PopPrimitive<unsigned short>(lua_State* L, std::size_t& index)
    {
        return static_cast<unsigned short>(lua_tonumber(L, index++));
    }

    template <>
    char PopPrimitive<char>(lua_State* L, std::size_t& index)
    {
        return static_cast<char>(lua_tonumber(L, index++));
    }

    template <>
    unsigned char PopPrimitive<unsigned char>(lua_State* L, std::size_t& index)
    {
        return static_cast<unsigned char>(lua_tonumber(L, index++));
    }

    template <>
    int64_t PopPrimitive<int64_t>(lua_State* L, std::size_t& index)
    {
        return static_cast<int64_t>(lua_tonumber(L, index++));
    }

    template <>
    uint64_t PopPrimitive<uint64_t>(lua_State* L, std::size_t& index)
    {
        return static_cast<uint64_t>(lua_tonumber(L, index++));
    }

    template <>
    float PopPrimitive<float>(lua_State* L, std::size_t& index)
    {
        return static_cast<float>(lua_tonumber(L, index++));
    }

    template <>
    double PopPrimitive<double>(lua_State* L, std::size_t& index)
    {
        return static_cast<double>(lua_tonumber(L, index++));
    }

    template <>
    bool PopPrimitive<bool>(lua_State* L, std::size_t& index)
    {
        return static_cast<bool>(lua_toboolean(L, index++));
    }

    template <>
    void* PopPrimitive<void*>(lua_State* L, std::size_t& index)
    {
        return lua_touserdata(L, index++);
    }
}            // namespace mta::impl
