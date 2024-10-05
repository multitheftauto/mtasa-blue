/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/gamesa_function_caller.h
 *  PURPOSE:     Game functions caller
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "StdInc.h"

namespace CallingConventions
{
    struct __THISCALL{};
    struct __CDECL{};
    struct __STDCALL{};
    struct __FASTCALL{};
    struct __VECTORCALL{};
} // namespace CallingConventions

template <typename ReturnType, typename CallingConvention, typename Func, typename... Args>
typename std::enable_if_t<std::is_same_v<CallingConvention, CallingConventions::__THISCALL>, ReturnType> CallGTAFunction(Func function, Args... params)
{
    return reinterpret_cast<ReturnType(__thiscall*)(Args...)>(function)(params...);
}

template <typename ReturnType, typename CallingConvention, typename Func, typename... Args>
typename std::enable_if_t<std::is_same_v<CallingConvention, CallingConventions::__CDECL>, ReturnType> CallGTAFunction(Func function, Args... params)
{
    return reinterpret_cast<ReturnType(__cdecl*)(Args...)>(function)(params...);
}

template <typename ReturnType, typename CallingConvention, typename Func, typename... Args>
typename std::enable_if_t<std::is_same_v<CallingConvention, CallingConventions::__STDCALL>, ReturnType> CallGTAFunction(Func function, Args... params)
{
    return reinterpret_cast<ReturnType(__stdcall*)(Args...)>(function)(params...);
}

template <typename ReturnType, typename CallingConvention, typename Func, typename... Args>
typename std::enable_if_t<std::is_same_v<CallingConvention, CallingConventions::__FASTCALL>, ReturnType> CallGTAFunction(Func function, Args... params)
{
    return reinterpret_cast<ReturnType(__fastcall*)(Args...)>(function)(params...);
}

template <typename ReturnType, typename CallingConvention, typename Func, typename... Args>
typename std::enable_if_t<std::is_same_v<CallingConvention, CallingConventions::__VECTORCALL>, ReturnType> CallGTAFunction(Func function, Args... params)
{
    return reinterpret_cast<ReturnType(__vectorcall*)(Args...)>(function)(params...);
}
