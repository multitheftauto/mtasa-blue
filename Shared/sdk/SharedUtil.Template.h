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
using nth_element = typename nth_element_impl<I, Ts...>::type;
