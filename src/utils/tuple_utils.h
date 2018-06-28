#pragma once
#include <tuple>
#include <utility>

namespace utils
{
    template<size_t I> struct _index { };
    template<typename T> struct _type { using type = T; };

    template<unsigned int N, typename Type, typename ...SubElements>
        struct tuple_generator
        {
            using type = typename tuple_generator<N - 1, Type, Type, SubElements...>::type;
        };
    template<typename Type, typename ...SubElements>
        struct tuple_generator<0, Type, SubElements...>
        {
            using type = std::tuple<SubElements...>;
        };

    template<typename Type, unsigned int Count>
        using gen_tuple_t = typename tuple_generator<Count, Type>::type;
}
