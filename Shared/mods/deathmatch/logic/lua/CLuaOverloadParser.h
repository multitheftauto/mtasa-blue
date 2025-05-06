/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <SharedUtil.Template.h>
#include <type_traits>
#include <variant>

template <auto*, auto*>
struct CLuaOverloadParser
{
};

template <typename Ret, typename Ret2, typename... Args, typename... Args2, auto (*Func)(Args...)->Ret, auto (*Func2)(Args2...)->Ret2>
struct CLuaOverloadParser<Func, Func2>
{
private:
    enum class ChosenFunction
    {
        FUNCA,
        FUNCB,
        COMMON,
        FAIL
    };

    template <std::size_t N, typename... Ts, typename... Us>
    static ChosenFunction MakeAllChoice(std::variant<Ts...> var, Us... us)
    {
        ChosenFunction result = MakeChoice<N>(var);
        if constexpr (sizeof...(Us) == 0)
            return result;
        else
        {
            ChosenFunction rest = MakeAllChoice<N + 1>(us...);
            // If types match for overload X, use overload X
            if (result == rest)
                return result;
            // If types don't match, check if this variable matches
            // both. If so, use the other type
            else if (result == ChosenFunction::COMMON)
                return rest;
            // If the rest matches both overloads, use the current one
            else if (rest == ChosenFunction::COMMON)
                return result;
            // result != rest && result != 0 && rest != 0
            // Type error
            return ChosenFunction::FAIL;
        }
    }

    // Chose an overload that the Nth paramter matches
    template <std::size_t N, typename... T>
    static ChosenFunction MakeChoice(std::variant<T...> var)
    {
        return std::visit(
            [](auto&& f) {
                using ft = decltype(f);
                if constexpr (!std::is_convertible_v<ft, nth_element_t<N, Args...>>)
                    return ChosenFunction::FUNCB;            // if it cannot match A, B it is
                if constexpr (!std::is_convertible_v<ft, nth_element_t<N, Args2...>>)
                    return ChosenFunction::FUNCA;            // if it cannot match B, A it is
                return ChosenFunction::COMMON;               // Both match
            },
            var);
    }

public:
    typename common_variant<Ret, Ret2>::type static Call(typename common_variant<Args, Args2>::type... args)
    {
        ChosenFunction choice = MakeAllChoice<0>(args...);
        if (choice == ChosenFunction::FUNCA)
            return Func(std::get<Args>(args)...);
        else if (choice == ChosenFunction::FUNCB)
            return Func2(std::get<Args2>(args)...);
        else if (choice == ChosenFunction::FAIL)
        {
            // User provided incorrect arguments
            throw std::invalid_argument("Overload resolution failed.");
        }
        // If this exception is thrown, there is a bug in the overload, as they are not unique
        throw std::invalid_argument("MTA:SA Bug. Please file an issue at https://git.io/blue-issues with the parameters used for calling.");
    }
};
