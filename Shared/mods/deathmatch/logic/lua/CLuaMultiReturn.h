/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <tuple>
// Wrapper around std::tuple to indicate that multiple values should be pushed to
// the Lua stack
template <typename... Ts>
struct CLuaMultiReturn
{
    // Note: We use a separate template for the constructor arguments
    // to allow type conversions. For example: return { "hello", 42 };
    // is a valid statement to construct a CLuaMultiReturn<const char*, int>
    template <typename... Args>
    CLuaMultiReturn(Args... args) : values{args...}
    {
    }

    std::tuple<Ts...> values;
};

template <typename T>
struct is_multireturn : std::false_type
{
};

template <typename Arg1, typename... Args>
struct is_multireturn<CLuaMultiReturn<Arg1, Args...>> : std::true_type
{
    using tupleType_t = std::tuple<Arg1, Args...>;
    static constexpr auto count = sizeof...(Args) + 1;
};
