#pragma once
#include <tuple>
#include <utility>
#include "hierarchy.h"

namespace utils 
{

    template<size_t I> struct _index { };
    template<typename T> struct _type { using type = T; };

    template<unsigned int N, typename Type, typename ...SubElements>
        struct element_generator 
        {
            using type = typename element_generator<N - 1, Type, Type, SubElements...>::type;
        };
    template<typename Type, typename ...SubElements>
        struct element_generator<0, Type, SubElements...> 
        {
            using type = Element<SubElements...>;
        };

    template<typename Type, unsigned int Count>
        using gen_Element_t = typename element_generator<Count, Type>::type;


    //foreach
    //
    // A function to call functions on hierarchies
    // The goal is to call the function with each of
    // the subelements in the given hierarchies.
    //
    // the function is specialized for multiple cases 
    // which get called respective to the element type
    // when it calls itself. 
    //
    // The specialization for the case where the arguments
    // are of type Element<...> calls a recursive function
    // which calls foreach with each of the subelement of 
    // its argument.
    //
    // The specialization for the case where the arguments
    // are of any other type simply calls the function with
    // its arguments.
    //
    template<size_t N, typename T>
        const T& select( const T& t)
        {
            return t;
        } 
    template<size_t N, typename... Elems>
        const auto& select( const Element<Elems...>& t)
        {
            return std::get<N>(t.subelements);
        } 

    template<typename Elem>
        constexpr size_t extract_count(const Elem& e)
        {
            return 1;
        };
    template<typename... Elems>
        constexpr size_t extract_count(const Element<Elems...>& e)
        {
            return sizeof...(Elems);
        };

    template<typename rT, 
        typename... FArgTs,
        typename... Es>
            constexpr void foreach( rT(*func)(FArgTs...), const Es... es);

    template<typename rT, 
        typename... Es>
            constexpr void foreach_imp( rT(*func)(Es...), const Es... es);

    template<typename rT, 
        typename... FArgTs,
        typename... Elems,
        typename... Es>
            constexpr void foreach_imp( rT(*func)(FArgTs...), 
                    const Element<Elems...>& first, const Es... es);

    template<typename rT, 
        typename... FArgTs,
        typename... Es> 
            constexpr void sub( rT(*func)(FArgTs...), 
                    _index<0> i, const Es... es);
    template<typename rT, 
        typename... FArgTs,
        typename... Es, 
        size_t N>
            constexpr void sub( rT(*func)(FArgTs...), 
                    _index<N> i, const Es... es);
}    

template<typename rT,
    typename... FArgTs,
    typename... Es>
constexpr void utils::foreach(rT(*func)(FArgTs...), const Es... es)
{
    foreach_imp(func, es...);
}
template<typename rT,
    typename... Es>
constexpr void utils::foreach_imp(rT(*func)(Es...), const Es... es)
{
    func(es...);
}

template<typename rT, 
    typename... FArgTs,
    typename... Elems,
    typename... Es> 
constexpr void utils::foreach_imp( rT(*func)(FArgTs...), 
        const Element<Elems...>& first, const Es... es)
{
    sub(func, _index<extract_count(first)>(), first, es... );
}


template<typename rT, 
    typename... FArgTs,
    typename... Es>
constexpr void utils::sub( rT(*func)(FArgTs...), 
        _index<0> i, const Es... es )
{
}

template<typename rT, 
    typename... FArgTs,
    typename... Es,
    size_t N>
constexpr void utils::sub( rT(*func)(FArgTs...), 
        _index<N> i, const Es... es )
{
    sub( func, _index<N-1>(), es...);
    foreach(func, select<N-1>(es)... );
}
