#pragma once
#include <tuple>
#include <utility>
#include "hierarchy.h"

namespace utils 
{

    template<size_t I> struct _index { };

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
        using gen_Element = typename element_generator<Count, Type>::type;



    template<typename T>
    struct OmniElement
    {
        
    };


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
    template<typename rT, 
        typename... FArgTs>
        void foreach( rT(&func)(FArgTs...), FArgTs... fArgs);

    template<typename rT, 
        typename... FArgTs,
        typename... Es>
        void foreach( rT(&func)(FArgTs...), 
                Es... es);

    template<typename rT, 
        typename... FArgTs,
        typename... Es, 
        size_t N>
        void foreach_n( rT(&func)(FArgTs...), 
                std::index_sequence<N> ns, Es... es );
    template<typename rT, 
        typename... FArgTs,
        typename... Es, 
        size_t N,
        size_t... Ns>
        void foreach_n( rT(&func)(FArgTs...), 
                std::index_sequence<N, Ns...> ns, Es... es);
}    

    template<typename rT, 
        typename... FArgTs>
        void utils::foreach( rT(&func)(FArgTs...), FArgTs... fArgs)
        {
            func(fArgs...);
        }

    template<typename rT, 
        typename... FArgTs,
        typename... Es> 
        void utils::foreach( rT(&func)(FArgTs...), 
                Es... es)
        {
            foreach_n(func, std::make_index_sequence<std::tuple_element<0, std::tuple<Es...>>::type::COUNT>(), es... );
        }
    template<typename rT, 
        typename... FArgTs,
        typename... Es,
        size_t N>
        void utils::foreach_n( rT(&func)(FArgTs...), 
                std::index_sequence<N> ns, Es... es )
        {
            foreach(func, std::get<N>(es.subelements)... );
        }
    template<typename rT, 
        typename... FArgTs,
        typename... Es,
        size_t N,
        size_t... Ns>
        void utils::foreach_n( rT(&func)(FArgTs...), 
                std::index_sequence<N, Ns...> ns, Es... es )
        {
            foreach(func, std::get<N>(es.subelements)... );
            foreach_n(func, std::index_sequence<Ns...>(), es... );
        }
