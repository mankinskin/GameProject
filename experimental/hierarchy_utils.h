#pragma once
#include <tuple>
#include <utility>

template<typename... Elems>
struct element_constructor 
{
    template<size_t... Ns, typename Head, typename... Rest>
        static constexpr const std::tuple<Rest...> 
        drop_head_impl( const std::index_sequence<Ns...> ns, 
                const std::tuple<Head, Rest...> tup )
        {
            return std::tuple<Rest...>( std::get<Ns + 1u>( tup )... );
        }

    template<typename Head, typename... Rest>
        static constexpr const std::tuple<Rest...> 
        drop_head( const std::tuple<Head, Rest...> tup )
        {
            return drop_head_impl( std::make_index_sequence<sizeof...(Rest)>(), tup );
        }

    template<typename Head, typename SubIniter>
        static constexpr const std::tuple<Head> 
        func_impl( const std::tuple<SubIniter> initer )
        {
            return  std::tuple<Head>( { std::get<0>( initer ) } ); 
        }

    template<typename Head, typename Next, typename... Rest, typename... SubIniters>
        static constexpr const std::tuple<Head, Next, Rest...> 
        func_impl( const std::tuple<SubIniters...> initer )
        {
            std::tuple<Head> head( { std::get<0>( initer ) } ); 
            return std::tuple_cat( head, func_impl<Next, Rest...>( drop_head( initer ) ) );
        }

    template<typename... Initers>
    static constexpr const std::tuple<Elems...> 
        func( const std::tuple<Initers...> initer )
        {
            return func_impl<Elems...>( initer );
        }
};
    template<typename... Elems, typename... Initers>
constexpr std::tuple<Elems...> construct( const std::tuple<Initers...> initer )
{
    return element_constructor<Elems...>::func( initer );
}
