#pragma once

#include "hierarchy_utils.h"
#include <tuple>

template<typename... Elems>
struct Hierarchy
{
    template<typename... Initers>
    Hierarchy( const Hierarchy<Initers...> initer )
        :elements( construct<Elems...>( initer.elements ) )
    {}

    template<typename... Initers>
    Hierarchy( const Initers... initers )
        :elements( construct<Elems...>( std::tuple<Initers...>(initers...) ) )
    {}

    Hierarchy( const Elems... elems )
        :elements( construct<Elems...>(std::tuple<Elems...>( elems... ) ) )
    {}
    const std::tuple<Elems...> elements;
};


