#pragma once

/**
    is_Nspecialization

    These structs allow testing whether a type is a specialization of a
    class template

    Note: Take care of optional parameters. For example std::unordered_map
    has 5 template arguments, thus is_5specialization needs to be used, rather
    than is_2specialization (Key, Value).
**/

template <typename Test, template <typename> class Ref>
struct is_specialization : std::false_type
{
};

template <template <typename> class Ref, typename Args>
struct is_specialization<Ref<Args>, Ref> : std::true_type
{
    using param_t = Args;
};

template <typename Test, template <typename, typename> class Ref>
struct is_2specialization : std::false_type
{
};

template <template <typename, typename> class Ref, typename Arg1, typename Arg2>
struct is_2specialization<Ref<Arg1, Arg2>, Ref> : std::true_type
{
    using param1_t = Arg1;
    using param2_t = Arg2;
};

template <typename Test, template <typename, typename, typename, typename, typename> class Ref>
struct is_5specialization : std::false_type
{
};

template <template <typename, typename, typename, typename, typename> class Ref, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
struct is_5specialization<Ref<Arg1, Arg2, Arg3, Arg4, Arg5>, Ref> : std::true_type
{
    using param1_t = Arg1;
    using param2_t = Arg2;
};

template <typename Test>
struct is_variant : std::false_type
{
};

template <typename Arg1, typename... Args>
struct is_variant<std::variant<Arg1, Args...>> : std::true_type
{
    using param1_t = Arg1;
    using rest_t = std::variant<Args...>;
    static constexpr auto count = sizeof...(Args) + 1;
};

/**
    nth_element

    Returns the nth element of a parameter pack
**/

template <std::size_t I, typename T, typename... Ts>
struct nth_element_impl
{
    using type = typename nth_element_impl<I - 1, Ts...>::type;
};

template <typename T, typename... Ts>
struct nth_element_impl<0, T, Ts...>
{
    using type = T;
};

template <std::size_t I, typename... Ts>
using nth_element_t = typename nth_element_impl<I, Ts...>::type;


// common_variant
// common_variant builds a single variant from types or variants
//  bool, bool -> variant<bool>
//  int, bool -> variant<int, bool>
//  variant<int, bool>, bool -> variant<int, bool>
//  variant<int, bool>, float -> variant<int, bool, float>
//  variant<int, bool>, variant<bool, float> -> variant<int, bool, float>
//  variant<int, bool>, variant<bool, float, double> -> variant<int, bool, float, double>

// Two different types
//  int, bool -> variant<int, bool>
template <typename T, typename U>
struct common_variant
{
    using type = std::variant<T, U>;
};

// Identical types
//  int, int -> variant<int>
template <typename T>
struct common_variant<T, T>
{
    using type = std::variant<T>;
};

// Type + Variant which starts with the Type
//  int, variant<int> -> variant<int>
template <typename T, typename... Ts>
struct common_variant<T, std::variant<T, Ts...>>
{
    using type = std::variant<T, Ts...>;
};

// Variant + Empty = Variant
template <typename... Ts>
struct common_variant<std::variant<Ts...>, std::variant<>>
{
    using type = std::variant<Ts...>;
};
// Empty + Variant = Variant
template <typename... Ts>
struct common_variant<std::variant<>, std::variant<Ts...>>
{
    using type = std::variant<Ts...>;
};

template <typename T, typename... Ts>
struct common_variant<T, std::variant<Ts...>>
{
    using type = std::conditional_t<std::is_convertible_v<T, std::variant<Ts...>>, std::variant<Ts...>, std::variant<T, Ts...>>;
};

template <typename T, typename... Ts>
struct common_variant<std::variant<Ts...>, T>
{
    using type = typename common_variant<T, std::variant<Ts...>>::type;
};

template <typename T, typename... Ts, typename... Us>
struct common_variant<std::variant<T, Ts...>, std::variant<Us...>>
{
    using type = typename common_variant<std::variant<Ts...>, typename common_variant<T, std::variant<Us...>>::type>::type;
};
