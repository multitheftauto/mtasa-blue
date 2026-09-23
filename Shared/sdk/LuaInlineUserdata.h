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
#include "CVector2D.h"
#include "CVector.h"
#include "CVector4D.h"
#include "CMatrix.h"

struct lua_State;

bool lua_isclass(lua_State* luaVM, int index, const char* szName);

template <typename T>
struct is_inline_userdata_type : std::false_type
{
};

template <typename T>
struct is_inline_userdata_type<const T> : is_inline_userdata_type<T>
{
};

template <>
struct is_inline_userdata_type<CVector2D> : std::true_type
{
};

template <>
struct is_inline_userdata_type<CVector> : std::true_type
{
};

template <>
struct is_inline_userdata_type<CVector4D> : std::true_type
{
};

template <>
struct is_inline_userdata_type<CMatrix> : std::true_type
{
};

template <typename T>
inline const char* GetInlineUserdataClassName()
{
    using clean_type = std::remove_cv_t<T>;
    if constexpr (std::is_same_v<clean_type, CVector2D>)
        return "Vector2";
    else if constexpr (std::is_same_v<clean_type, CVector>)
        return "Vector3";
    else if constexpr (std::is_same_v<clean_type, CVector4D>)
        return "Vector4";
    else if constexpr (std::is_same_v<clean_type, CMatrix>)
        return "Matrix";
    else
        return "";
}
