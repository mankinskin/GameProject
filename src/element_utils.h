#pragma once
#include <tuple>
#include <utility>
#include "element.h"

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

}    

