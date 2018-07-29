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

    template<typename... To>
        struct tuple_converter
        {
            template<size_t... Ns, typename Head, typename... Rest>
                static constexpr const std::tuple<Rest...>
                drop_head_impl(const std::index_sequence<Ns...> ns,
                        const std::tuple<Head, Rest...> tup)
                {
                    return std::tuple<Rest...>(
                            std::get<Ns + 1u>(tup)...);
                }

            template<typename Head, typename... Rest>
                static constexpr const std::tuple<Rest...>
                drop_head(const std::tuple<Head, Rest...> tup)
                {
                    return drop_head_impl(
                            std::make_index_sequence<sizeof...(Rest)>(),
                            tup);
                }

            template<typename Head, typename From>
                static constexpr const std::tuple<Head>
                func_impl(const std::tuple<From> from)
                {
                    return  std::tuple<Head>({ Head(std::get<0>(from)) });
                }

            template<typename Head, typename Next, typename... Rest, typename... From>
                static constexpr const std::tuple<Head, Next, Rest...>
                func_impl(const std::tuple<From...> from)
                {
                    std::tuple<Head> head({ Head(std::get<0>(from)) });
                    return std::tuple_cat(head,
                            func_impl<Next, Rest...>(
                                drop_head(from)));
                }

            template<typename... From>
                static constexpr const std::tuple<To...>
                func(const std::tuple<From...> from)
                {
                    return func_impl<To...>(from);
                }
        };

    template<typename... To, typename... From>
        constexpr std::tuple<To...>
        convert_tuple(const std::tuple<From...> from)
        {
            return tuple_converter<To...>::func(from);
        }
}
