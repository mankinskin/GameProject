#pragma once
#include <array>
#include <utility>

namespace utils
{
    template<typename ToType, typename FromType, size_t N, typename Pred, size_t... Ns>
        constexpr std::array<ToType, N> convert_array_impl( const std::array<FromType, N>& arr, std::index_sequence<Ns...> ns, Pred&& predicate)
        {
            return std::array<ToType, N>{ predicate(arr[Ns])... };
        }

    template<typename ToType, typename FromType, size_t N, typename Pred>
        constexpr std::array<ToType, N> convert_array( const std::array<FromType, N>& arr, Pred&& predicate = [](const FromType& from) -> const ToType { return from; })
        {
            return convert_array_impl<ToType, FromType, N>( arr, std::make_index_sequence<N>(), predicate );
        }
}
