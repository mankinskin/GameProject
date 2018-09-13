#pragma once
#include <tuple>
#include <vector>
#include <utility>
#include "utils/id.h"
#include "utils/tuple_utils.h"
#include <type_traits>

namespace signals
{
  extern std::vector<void(*)()> funcClearFuncs;
  template<typename F, typename... Args>
	class Functor
	{
	  public:
		using Container = utils::Container<Functor<F, Args...>>;
		using ID = typename Container::ID;
		static Container all;

		constexpr Functor(F&& pF, Args&&... pArgs) noexcept
		  : func(std::forward<F&>(pF))
		  , args(std::forward<Args>(pArgs)...)
		  {
			initialize();
		  }
		Functor() = delete;
		Functor(const Functor&) = default;
		Functor(Functor&&) = default;
		Functor& operator=(const Functor&) = default;
		Functor& operator=(Functor&&) = default;
		~Functor() = default;

		template<size_t... Ns>
		  constexpr void invoke_unpacked(std::index_sequence<Ns...>)
		  {
			func(std::get<Ns>(args)...);
		  }
		constexpr void invoke()
		{
		  invoke_unpacked(std::make_index_sequence<sizeof...(Args)>());
		}

		static void invoke(const size_t i)
		{
		  printf("Invoking functor %lu\n", i);
		  all[i].invoke();
		}

		static void clear()
		{
		  all.clear();
		}

	  private:
		const F& func;
		std::tuple<Args...> args;
		struct At_Init
		{
		  At_Init()
		  {
			funcClearFuncs.push_back(&Functor<F, Args...>::clear);
		  }
		};
		static void initialize()
		{
		  static At_Init init;
		}
	};

  template<typename F, typename... Args>
	typename Functor<F, Args...>::Container Functor<F, Args...>::all = typename Functor<F, Args...>::Container();

  template<typename... Funcs>
	class Procedure
	{
	  public:
		using Container = utils::Container<Procedure<Funcs...>>;
		using ID = typename Container::ID;
		static Container all;

		Procedure(utils::ID<Funcs>... fs)
		  : funcs(fs...)
		{
		  initialize();
		}

		constexpr void invoke_n(utils::_index<0>) const
		{}

		template<size_t N>
		  constexpr void invoke_n(utils::_index<N>) const
		  {
			invoke_n(utils::_index<N-1>());
			std::get<N-1>(funcs)->invoke();
		  }

		constexpr void invoke()
		{
		  invoke_n(utils::_index<sizeof...(Funcs)>());
		}

		static void invoke(const size_t i)
		{
		  all[i].invoke();
		}

		static void clear()
		{
		  all.clear();
		}

	  private:
		std::tuple<utils::ID<Funcs>...> funcs;
		struct At_Init
		{
		  At_Init()
		  {
			funcClearFuncs.push_back(&Procedure<Funcs...>::clear);
		  }
		};
		static void initialize()
		{
		  static At_Init init;
		}
	};

  template<typename... Funcs>
	typename Procedure<Funcs...>::Container Procedure<Funcs...>::all = typename Procedure<Funcs...>::Container();

  struct Invoker
  {
	template<typename F, typename... Args>
	  Invoker(utils::ID<Functor<F, Args...>> pFunc)
	  : invoker(Functor<F, Args...>::invoke)
		, index(pFunc.index)
	{}

	template<typename... Funcs>
	  Invoker(utils::ID<Procedure<Funcs...>> pProc)
	  : invoker(Procedure<Funcs...>::invoke)
		, index(pProc.index)
	{}

	void invoke() const
	{
	  invoker(index);
	}

	void(&invoker)(const size_t);
	const size_t index;
  };

  // allocates a func and returns an ID for it
  template<typename F, typename... Args>
	constexpr utils::ID<Functor<F, Args...>> func(F&& pF, Args... pArgs)
	{
	  return Functor<F, Args...>::all.makeID(Functor<F, Args...>(std::forward<F>(pF), std::forward<Args>(pArgs)...));
	}
  template<typename F, typename... Args>
	constexpr utils::ID<Functor<F, Args...>> refFunc(F&& pF, Args&&... pArgs)
	{
	  return Functor<F, Args...>::all.makeID(Functor<F, Args...>(std::forward<F>(pF), std::forward<Args>(pArgs)...));
	}
  template<typename... Funcs>
	constexpr utils::ID<Procedure<Funcs...>> procedure(utils::ID<Funcs>... pFuncs)
	{
	  return Procedure<Funcs...>::all.makeID(Procedure<Funcs...>(pFuncs...));
	}

  void clearFunctors();
}

