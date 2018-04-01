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


    //foreach

    template<typename rT, 
        typename TarT, 
        typename SrcT>
        void foreach( rT(&func)(TarT, SrcT), TarT tar, SrcT src);

    template<typename rT, 
        typename... FArgTs,
        typename... TarTs, 
        typename... SrcTs>
        void foreach( rT(&func)(FArgTs...), 
                Element<TarTs...> tar, Element<SrcTs...> src);
    template<size_t N, 
        typename rT, 
        typename... FArgTs,
        typename... TarTs, 
        typename... SrcTs>
        void foreach_n( rT(&func)(FArgTs...), 
                Element<TarTs...> tar, Element<SrcTs...> src, _index<N> i);

    template<typename rT, 
        typename... FArgTs,
        typename... TarTs, 
        typename... SrcTs>
        void foreach_n( rT(&func)(FArgTs...), 
                Element<TarTs...> tar, Element<SrcTs...> src, _index<0> i);
}    

    template<typename rT, 
        typename TarT, 
        typename SrcT>
        void utils::foreach( rT(&func)(TarT, SrcT), TarT tar, SrcT src)
        {
            func(tar, src);
        }

    template<typename rT, 
        typename... FArgTs,
        typename... TarTs, 
        typename... SrcTs>
        void utils::foreach( rT(&func)(FArgTs...), 
                Element<TarTs...> tar, Element<SrcTs...> src)
        {
            foreach_n(func, tar, src, _index<sizeof...(TarTs)>() );
        }
    template<size_t N, 
        typename rT, 
        typename... FArgTs,
        typename... TarTs, 
        typename... SrcTs>
        void utils::foreach_n( rT(&func)(FArgTs...), 
                Element<TarTs...> tar, Element<SrcTs...> src, _index<N> i)
        {
            utils::foreach(func, std::get<N-1>(tar.subelements), std::get<N-1>(src.subelements));
            foreach_n(func, tar, src, _index<N-1>() );
        }

    template<typename rT, 
        typename... FArgTs,
        typename... TarTs, 
        typename... SrcTs>
        void utils::foreach_n( rT(&func)(FArgTs...), 
                Element<TarTs...> tar, Element<SrcTs...> src, _index<0> i)
        {
        }

