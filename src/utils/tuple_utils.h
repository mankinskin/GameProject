#pragma once
#include <tuple>
#include <utility>
#include <type_traits>
#include <algorithm>

namespace utils
{
  template<size_t> struct _index { };
  template<bool> struct _bool { };
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

  template<typename... To>
	struct tuple_converter
	{
	  template<size_t... Ns, typename Head, typename... Rest>
		static constexpr std::tuple<Rest...>
		drop_head_impl(const std::index_sequence<Ns...> ns, std::tuple<Head, Rest...> tup)
		{
		  return std::tuple<Rest...>(std::get<Ns + 1u>(tup)...);
		}

	  template<typename Head, typename... Rest>
		static constexpr std::tuple<Rest...>
		drop_head(std::tuple<Head, Rest...> tup)
		{
		  return drop_head_impl(std::make_index_sequence<sizeof...(Rest)>(), tup);
		}

	  template<typename Head, typename... From>
		static constexpr std::tuple<Head>
		func_impl(From&&... from)
		{
		  return std::tuple<Head>(std::move(Head(std::move(std::get<0>(from))...)));
		}

	  template<typename Head, typename Next, typename... Rest, typename... From>
		static constexpr std::tuple<Head, Next, Rest...>
		func_impl(From&&... from)
		{
		  return std::tuple_cat(func_impl<Head>(std::move(from)...), func_impl<Next, Rest...>(drop_head(std::move(from))...));
		}

	  template<typename... From>
		static constexpr std::tuple<To...>
		func(From&&... from)
		{
		  return func_impl<To...>(std::move(from)...);
		}
	};

  template<typename... To, typename... From>
	constexpr std::tuple<To...>
	convert_tuple(From&&... from)
	{
	  puts("Converting tuple");
	  return tuple_converter<To...>::func(std::move(from)...);
	}

  template<typename Arg>
	constexpr Arg sum(const Arg arg)
	{
	  return arg;
	}
  template<typename Head, typename... Args>
	constexpr auto sum(const Head h, const Args... args)
	{
	  return h + sum(args...);
	}
  template<typename Arg>
	constexpr Arg min(const Arg arg)
	{
	  return arg;
	}
  template<typename Head, typename... Args>
	constexpr auto min(const Head h, const Args... args)
	{
	  return std::min(h, min(args...));
	}
  template<typename Arg>
	constexpr Arg max(const Arg arg)
	{
	  return arg;
	}
  template<typename Head, typename... Args>
	constexpr auto max(const Head h, const Args... args)
	{
	  return std::max(h, max(args...));
	}
}
