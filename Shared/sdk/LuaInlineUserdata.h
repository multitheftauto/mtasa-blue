/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/LuaInlineUserdata.h
 *  PURPOSE:     Inline userdata type traits and class naming helpers
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <type_traits>

class CLuaVector2D;
class CLuaVector3D;
class CLuaVector4D;
class CLuaMatrix;
struct lua_State;

bool lua_isclass(lua_State* luaVM, int index, const char* szName);

template <typename T>
struct is_inline_userdata_type : std::false_type
{
};

template <>
struct is_inline_userdata_type<CLuaVector2D> : std::true_type
{
};

template <>
struct is_inline_userdata_type<CLuaVector3D> : std::true_type
{
};

template <>
struct is_inline_userdata_type<CLuaVector4D> : std::true_type
{
};

template <>
struct is_inline_userdata_type<CLuaMatrix> : std::true_type
{
};

template <typename T>
inline const char* GetInlineUserdataClassName()
{
    if constexpr (std::is_same_v<T, CLuaVector2D>)
        return "Vector2";
    else if constexpr (std::is_same_v<T, CLuaVector3D>)
        return "Vector3";
    else if constexpr (std::is_same_v<T, CLuaVector4D>)
        return "Vector4";
    else if constexpr (std::is_same_v<T, CLuaMatrix>)
        return "Matrix";
    else
        return "";
}
