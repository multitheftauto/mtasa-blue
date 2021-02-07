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
    // is a valid statement to construct a LuaMuliReturn<std::string, int>
    template <typename... Args>
    CLuaMultiReturn(Args... args) : values{args...}
    {
    }

    std::tuple<Ts...> values;
};
