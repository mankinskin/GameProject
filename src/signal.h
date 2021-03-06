#pragma once
#include "utils/id.h"
#include "utils/id.h"
#include "functor.h"
#include "event.h"
#include <tuple>

namespace signals
{
  // plug these into signal listeners
  template<typename... Signals>
	class And
	{
	  public:
		And(const Signals... sigs)
		  : signals(sigs...)
		{}

		constexpr bool stat_n(utils::_index<0>) const
		{
		  return std::get<sizeof...(Signals) - 1>(signals)->stat();
		}
		template<size_t N>
		  constexpr bool stat_n(utils::_index<N>) const
		  {
			return std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() && stat_n(utils::_index<N-1>());
		  }
		constexpr bool stat() const
		{
		  return stat_n(utils::_index<sizeof...(Signals) - 1>());
		}

		constexpr const And* operator->() const
		{
		  return this;
		}
	  private:
		const std::tuple<const Signals...> signals;
	};

  template<typename... Signals>
	class Or
	{
	  public:
		Or(const Signals... sigs)
		  : signals(sigs...)
		{}

		constexpr bool stat_n(utils::_index<0>) const
		{
		  return std::get<sizeof...(Signals) - 1>(signals)->stat();
		}
		template<size_t N>
		  constexpr bool stat_n(utils::_index<N>) const
		  {
			return std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() || stat_n(utils::_index<N-1>());
		  }
		constexpr bool stat() const
		{
		  return stat_n(utils::_index<sizeof...(Signals) - 1>());
		}
		constexpr const Or* operator->() const
		{
		  return this;
		}
	  private:
		const std::tuple<const Signals...> signals;
	};

  template<typename... Signals>
	class Xor
	{
	  public:
		Xor(const Signals... sigs)
		  : signals(sigs...)
		{}

		constexpr bool stat_n(utils::_index<0>) const
		{
		  return false;
		}
		template<size_t N>
		  constexpr bool stat_n(utils::_index<N>) const
		  {
			return !((std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() ==
				  std::get<(sizeof...(Signals) - 1) - (N - 1)>(signals)->stat())
				&& !stat_n(utils::_index<N-1>()));
		  }
		constexpr bool stat() const
		{
		  return stat_n(utils::_index<sizeof...(Signals) - 1>());
		}
		constexpr const Xor* operator->() const
		{
		  return this;
		}
	  private:
		const std::tuple<const Signals...> signals;
	};

  template<typename... Signals>
	class Nor
	{
	  public:
		Nor(const Signals... sigs)
		  : signals(sigs...)
		{}

		constexpr bool stat_n(utils::_index<0>) const
		{
		  return !std::get<sizeof...(Signals) - 1>(signals)->stat();
		}
		template<size_t N>
		  constexpr bool stat_n(utils::_index<N>) const
		  {
			return !std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() && stat_n(utils::_index<N-1>());
		  }
		constexpr bool stat() const
		{
		  return stat_n(utils::_index<sizeof...(Signals) - 1>());
		}

		constexpr const Nor* operator->() const
		{
		  return this;
		}
	  private:
		const std::tuple<const Signals...> signals;
	};

  template<typename... Signals>
	class Equal
	{
	  public:
		Equal(const Signals... sigs)
		  : signals(sigs...)
		{}

		constexpr bool stat_n(utils::_index<0>) const
		{
		  return std::get<sizeof...(Signals) - 1>(signals)->stat();
		}
		template<size_t N>
		  constexpr bool stat_n(utils::_index<N>) const
		  {
			return std::get<(sizeof...(Signals) - 1) - N>(signals)->stat() == stat_n(utils::_index<N-1>());
		  }
		constexpr bool stat() const
		{
		  return stat_n(utils::_index<sizeof...(Signals) - 1>());
		}
		constexpr const Equal* operator->() const
		{
		  return this;
		}
	  private:
		const std::tuple<const Signals...> signals;
	};

  template<template<typename...> class Op, typename... Signals>
	class SignalListener : public Op<Signals...>
  {
	public:
	  using Container = utils::Container<SignalListener<Op, Signals...>>;
	  using ID = typename Container::ID;
	  static Container all;
	  constexpr SignalListener(const Signals... sigs)
		: Op<Signals...>(sigs...)
	  {
		initialize();
	  }
	  constexpr SignalListener(const Op<Signals...> op)
		: Op<Signals...>(op)
	  {
		initialize();
	  }

	  constexpr Op<Signals...>* operator->() const
	  {
		return (Op<Signals...>*)(this);
	  }

	  static bool stat(size_t i)
	  {
		return all[i].stat();
	  }
	  using Op<Signals...>::stat;
	  static void clear()
	  {
		all.clear();
	  }

	private:
	  static void initialize()
	  {
		static At_Init<Op, Signals...> init;
	  }
	  template<template<typename...> class O, typename... S>
		struct At_Init
		{
		  constexpr At_Init()
		  {
			signalClearFuncs.push_back(&SignalListener<O, S...>::clear);
		  }
		};
  };
  template<template<typename...>class Op, typename... Signals>
	typename SignalListener<Op, Signals...>::Container SignalListener<Op, Signals...>::all = SignalListener<Op, Signals...>::Container();

  template<typename... Signals>
	constexpr const SignalListener<And, Signals...> ifAll(const Signals... sigs)
	{
	  return SignalListener<And, Signals...>(sigs...);
	}

  template<typename... Signals>
	constexpr const SignalListener<Or, Signals...> ifAny(const Signals... sigs)
	{
	  return SignalListener<Or, Signals...>(sigs...);
	}

  template<typename... Signals>
	constexpr const SignalListener<Xor, Signals...> ifNotEqual(const Signals... sigs)
	{
	  return SignalListener<Xor, Signals...>(sigs...);
	}

  template<typename... Signals>
	constexpr const SignalListener<Nor, Signals...> ifNone(const Signals... sigs)
	{
	  return SignalListener<Nor, Signals...>(sigs...);
	}

  template<typename... Signals>
	constexpr const SignalListener<Equal, Signals...> ifEqual(const Signals... sigs)
	{
	  return SignalListener<Equal, Signals...>(sigs...);
	}

  struct State
  {
	State(bool s = false)
	  : status(s)
	{}

	constexpr void set(bool s)
	{
	  status = s;
	}
	constexpr bool stat() const
	{
	  return status;
	}
	constexpr const State* operator->() const
	{
	  return this;
	}
	using Container = utils::Container<State>;
	using ID = typename Container::ID;
	static Container all;
	private:
	bool status;
  };

  struct Signal
  {
	Signal(bool s = false)
	  : status(s)
	{}

	constexpr void set(bool s)
	{
	  status = s;
	}
	constexpr bool stat() const
	{
	  return status;
	}
	constexpr const Signal* operator->() const
	{
	  return this;
	}
	static void reset()
	{
	  std::fill(all.begin(), all.end(), Signal(false));
	}
	using Container = utils::Container<Signal>;
	using ID = typename Container::ID;
	static Container all;
	private:
	bool status;
  };

  template<typename S, typename R>
	struct Flip
	{
	  using Set = S;
	  using Reset = R;
	  Flip(const S pS, const R pR, bool startAs = false)
		: state(State::all.makeID(State(startAs)))
		  , prevState(State::all.makeID(State(startAs)))
		  , processed(Signal::all.makeID(Signal(false)))
		  , set(pS)
		  , reset(pR)
	  {}

	  constexpr void process() const
	  {
		if (!processed->stat()) {
		  prevState->set(state->stat());
		  if (set->stat() != reset->stat())
			state->set(!reset->stat());
		  processed->set(true);
		}
	  }
	  constexpr bool stat() const
	  {
		process();
		return state->stat();
	  }
	  constexpr bool stat_prev() const
	  {
		process();
		return prevState->stat();
	  }
	  constexpr const Flip* operator->() const
	  {
		return this;
	  }
	  const utils::ID<State> state;
	  const utils::ID<State> prevState;
	  const utils::ID<Signal> processed;
	  const S set;
	  const R reset;
	};

  template<typename S, typename R>
	struct Clicker : public Flip<S, R>
  {
	using Flip<S, R>::state;
	using Flip<S, R>::prevState;
	using Flip<S, R>::processed;
	using Flip<S, R>::set;
	using Flip<S, R>::reset;
	using Flip<S, R>::process;
	Clicker(const Flip<S, R> f, const bool t)
	  : Flip<S, R>(f)
		, to(t)
	{}
	constexpr bool stat() const
	{
	  process();
	  bool p = prevState->stat();
	  bool n = state->stat();
	  return (p != n) && (n == to);
	}
	constexpr const Clicker* operator->() const
	{
	  return this;
	}
	private:
	const bool to;
  };

  template<typename Set, typename Reset>
	constexpr const SignalListener<Flip, Set, Reset> flip(const Set set, const Reset reset, bool startAs = false)
	{
	  return SignalListener<Flip, Set, Reset>(Flip<Set, Reset>(set, reset, startAs));
	}
  template<typename Set, typename Reset>
	constexpr const SignalListener<Clicker, Set, Reset> clicker(const Flip<Set, Reset> f, const bool to)
	{
	  return SignalListener<Clicker, Set, Reset>(Clicker<Set, Reset>(f, to));
	}

  struct Listener
  {
	template<typename E>
	  Listener(const utils::ID<EventListener<E>> pListener)
	  : stater(EventListener<E>::stat)
		, index(pListener.index)
	{}
	template<template<typename...> class Op, typename... Signals>
	  Listener(const utils::ID<SignalListener<Op, Signals...>> pListener)
	  : stater(SignalListener<Op, Signals...>::stat)
		, index(pListener.index)
	{}
	template<template<typename...> class Op, typename... Signals>
	  Listener(const SignalListener<Op, Signals...> pListener)
	  : stater(SignalListener<Op, Signals...>::stat)
		, index(SignalListener<Op, Signals...>::all.makeID(pListener).index)
	{}
	bool stat() const
	{
	  return stater(index);
	}

	bool(&stater)(const size_t);
	const size_t index;
  };

  template<typename E>
	const Listener listen(const utils::ID<EventListener<E>> pEvent)
	{
	  return Listener(pEvent);
	}
  template<typename E>
	const Listener listen(const E pEvent)
	{
	  return listen(typename EventListener<E>::ID(EventListener<E>(pEvent)));
	}

  template<template<typename...> class Op, typename... Signals>
	const Listener listen(SignalListener<Op, Signals...> pSignal)
	{
	  return Listener(typename SignalListener<Op, Signals...>::ID(pSignal));
	}

  void processLinks();

  extern std::vector<std::pair<const Listener, const Invoker>> links;
  void link(Listener pListener, Invoker pInvoker);

  template<typename Signal, typename R, typename... Args>
	void link(const Signal pListener, R(&&pF)(Args...), Args&&... pArgs)
	{
	  link(Listener(pListener), Invoker(std::forward<R(Args...)>(pF), std::forward<Args>(pArgs)...));
	}

  template<typename Signal, typename... Funcs>
	void link(const Signal pListener, utils::ID<Funcs>... pFuncs)
	{
	  link(Listener(pListener), Invoker(pFuncs...));
	}
  template<typename Signal, typename R, typename... Args>
	void link(const Signal pListener, utils::ID<Functor<R, Args...>> pFunc)
	{
	  link(Listener(pListener), Invoker(pFunc));
	}

  template<typename Signal, typename... Funcs>
	void link(const Signal pListener, utils::ID<Procedure<Funcs...>> pProc)
	{
	  link(Listener(pListener), Invoker(pProc));
	}
  void checkEvents();
  void clearSignals();
}

