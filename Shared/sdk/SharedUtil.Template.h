#pragma once

#include <type_traits>
#include <variant>

template<typename T>
struct remove_cr
{
    using type = std::remove_const_t<std::remove_reference_t<T>>; // Must first remove ref, then const: https://stackoverflow.com/questions/15887144
};

template<typename T>
using remove_cr_t = typename remove_cr<T>::type;

/**
    nth_element

    Returns the nth element of a parameter pack by recursively
    discarding and counting down until the index is zero, at which
    point the type is returned
**/

// Recursive case, I is larger than 1, therefore
// we need to look at the I-1 case by discarding the
// first type.
template <std::size_t I, typename T, typename... Ts>
struct nth_element_impl
{
    using type = typename nth_element_impl<I - 1, Ts...>::type;
};

// Base case: If we ask for the 0th element, the current element
// is the one we're looking for
template <typename T, typename... Ts>
struct nth_element_impl<0, T, Ts...>
{
    using type = T;
};

template <std::size_t I, typename... Ts>
using nth_element_t = typename nth_element_impl<I, Ts...>::type;

/**
    is_specialization

    This struct allows testing whether a type is a specialization of a class template

    Usage can be the following:
    if constexpr(is_specialization<T, std::vector>::value)
    {
        // T is a vector!
        using param_t = typename is_specialization<T, std::vector>::param_t<0>
        // param_t is the type of the content of the vector
    }
**/
template <typename Test, template <typename...> class Ref>
struct is_specialization : std::false_type
{
};

/* 
Fix this if you need it (size and param_t). 
Param_t must be available in both structs under GCC.

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type
{
    template <std::size_t N>
    using param_t = nth_element_t<N, Args...>;
    static constexpr auto size = sizeof...(Args);
}; */

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type
{
};

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

// T + Variant = Variant or a new variant consisting of T + Variant
// This is done by checking if T is convertible to the variant
template <typename T, typename... Ts>
struct common_variant<T, std::variant<Ts...>>
{
    using type = std::conditional_t<std::is_convertible_v<T, std::variant<Ts...>>, std::variant<Ts...>, std::variant<T, Ts...>>;
};

// Variant + T = Variant or a new variant consisting of T + Variant
// Simply calls the above case
template <typename T, typename... Ts>
struct common_variant<std::variant<Ts...>, T>
{
    using type = typename common_variant<T, std::variant<Ts...>>::type;
};


// Variant + Variant = Combined Variant
// This recursively calls itself and deconstructs the first variant, while
// adding the first type in the first variant to the second variant (via the T + variant overload)
template <typename T, typename... Ts, typename... Us>
struct common_variant<std::variant<T, Ts...>, std::variant<Us...>>
{
    using type = typename common_variant<std::variant<Ts...>, typename common_variant<T, std::variant<Us...>>::type>::type;
};

// dummy_type
// generic dummy type
struct dummy_type
{
};


// n_tuple: Constructs a tuple of size N (with dummy_type as parameter types)
//  n_tuple<2>::type == std::tuple<dummy_type, dummy_type>
template <std::size_t, bool HasEnough = false, typename... Args>
struct n_tuple;

// Second parameter is true -> We have reached N types in Args
template <std::size_t N, typename... Args>
struct n_tuple<N, true, Args...>
{
    using type = std::tuple<Args...>;
};

// Second parameter is false -> Add a dummy type and
// decide if sizeof...(Args) + 1 is enough
template <std::size_t N, typename... Args>
struct n_tuple<N, false, Args...>
{
    using type = typename n_tuple<N, sizeof...(Args) + 1 >= N, Args..., dummy_type>::type;
};

// pad_func_with_func
// pads Func with as many dummy_type arguments as needed to
// have the same number of arguments as FuncB has
template <auto* Func, typename T>
struct pad_func_with_func_impl
{
};

// pad_func_with_func_impl takes a tuple of additional arguments, which are then
// taken as parameter. This allows us to discard exactly sizeof...(Ts) function
// arguments. Effectively this means Call is a function that is identical in behavior
// to Func, but takes additional sizeof...(Ts) dummy arguments that are discarded.
template <typename... Ts, typename Ret, typename... Args, auto (*Func)(Args...)->Ret>
struct pad_func_with_func_impl<Func, std::tuple<Ts...>>
{
    static inline Ret Call(Args... arg, Ts... args) { return Func(arg...); }
};

template <auto*, auto*>
struct pad_func_with_func
{
};

// pad_func_with_func initially needs to figure out the maximum number of parameters for the two functions
// It then determines how many parameters need to be added to Func, in order to have parity in argument count
// By using n_tuple, we build a Tuple of exactly the amout of parameters that need to be added to Func, which
// is then applied via the impl function above
template <typename Ret, typename... Args, auto (*Func)(Args...)->Ret, typename RetB, typename... ArgsB, auto (*FuncB)(ArgsB...)->RetB>
struct pad_func_with_func<Func, FuncB> : pad_func_with_func_impl<Func, typename n_tuple<std::max(sizeof...(Args), sizeof...(ArgsB)) - sizeof...(Args),
                                                                                        std::max(sizeof...(Args), sizeof...(ArgsB)) - sizeof...(Args) == 0>::type>
{
};