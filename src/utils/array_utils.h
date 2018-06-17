#pragma once
#include <array>
#include <utility>
namespace utils 
{
    template<typename ToType, typename FromType, size_t N, size_t... Ns>
        std::array<ToType, N> convert_array_impl( const std::array<FromType, N>& arr, std::index_sequence<Ns...> ns )
        {
            return std::array<ToType, N>{ arr[Ns]... };
        }

    template<typename ToType, typename FromType, size_t N>
        std::array<ToType, N> convert_array( const std::array<FromType, N>& arr )
        {
            return convert_array_impl<ToType>( arr, std::make_index_sequence<N>() );
        }
}
