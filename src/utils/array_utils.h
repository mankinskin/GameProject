#pragma once
#include <array>
#include <utility>
#include <functional>

namespace utils
{
  template<typename ToType, typename FromType, size_t N, typename Conv, size_t... Ns>
	constexpr std::array<ToType, N> convert_array_impl(const std::array<FromType, N>& arr, std::index_sequence<Ns...> ns, Conv&& conversion)
	{
	  return std::array<ToType, N>{conversion(arr[Ns])...};
	}

  template<typename ToType, typename FromType, size_t N, typename Conv = std::function<ToType(const FromType&)>>
	constexpr std::array<ToType, N> convert_array(const std::array<FromType, N>& arr, Conv&& conversion = [](const FromType& from) { return ToType(from); })
	{
	  return convert_array_impl<ToType, FromType, N>(arr, std::make_index_sequence<N>(), conversion);
	}
}
