#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <tuple>
#include "Signal.h"

namespace functors {
/*
Each functor holds a function pointer and a tuple for the arguments.
when a functor class is first instantited from the template, its static void call(size_t index) function is pushed onto the functorInvokers.
this function will later be used to call a functor object by its index.
when a functor object is created, its template_index and its slot index is inserted into the order_queue, which ensures that all functors will always be called in the order in which they were created.
when checking all functors, the functorOrder queue is used. the invoker_index is used to determine the class of the functor and the slot_index is used to find the functor object to be invoked.
*/

	template<size_t N, typename R, typename... Args>
	struct applier {
		template<typename...ExArgs>
		static void func(R(*pF)(Args...), const std::tuple<Args...>& pArgTuple, ExArgs&&... pExArgs) {
			applier<N - 1, R, Args...>::func(pF, pArgTuple, std::get<N - 1>(pArgTuple), pExArgs...);
		}
	};
	template<typename R, typename... Args>
	struct applier<0, R, Args...> {
		template<typename...ExArgs>
		static void func(R(*pF)(Args...), const std::tuple<Args...>& pArgTuple, ExArgs&&... pExArgs) {
			pF(pExArgs...);
		}
	};

	template<typename R, typename... Args>
	void function_caller(R(*pF)(Args...), const std::tuple<Args...>& pArgTuple) {
		applier<sizeof...(Args), R, Args...>::func(pF, pArgTuple);
	}

	template<typename R, typename... Args>
	class Functor {
	public:
		Functor(size_t pIndex, R(*pF)(Args...), Args... pArgs)
			:func(pF), args(std::forward_as_tuple<Args...>(std::forward<Args>(pArgs)...)), slot_index(pIndex)
		{
		}
		static void clear() {
			slots.clear();
		}
		static void reserve_slots(size_t pCount) {
			slots.reserve(pCount);
		}
		void invoke() const {
			function_caller(func, args);
		}
		static void call(size_t pFunc) {
			if (slots[pFunc].check()) {
				slots[pFunc].invoke();
			}
		}
		bool check() {
			for (unsigned int s = 0; s < signalSet.size(); ++s) {
				if (signals::allSignals[signalSet[s]]) {
					return true;
				}
			}
			return false;
		}
		void set_triggers(std::initializer_list<size_t> pSet) {
			signalSet = pSet;
		}
		static size_t invoker_index;
		static std::vector<Functor<R, Args...>> slots;
		size_t slot_index;
	private:
		std::vector<size_t> signalSet;
		R(*func)(Args...);
		std::tuple<Args...> args;

	};

	template<typename R, typename... Args>
	std::vector<Functor<R, Args...>> Functor<R, Args...>::slots = std::vector<Functor<R, Args...>>();
	template<typename R, typename... Args>
	size_t Functor<R, Args...>::invoker_index = 0;

	extern std::vector<void(*)(size_t)> functorInvokers;//invokes all functor templates
	extern std::vector<void(*)()> functorDestructors;//destroys all functor templates
	extern std::vector<std::pair<size_t, size_t>> functorOrder;

	template<typename R, class...Args>
	struct FunctorRef {
		FunctorRef(size_t pIndex) :index(pIndex) {}
		size_t index;
		void set_triggers(std::initializer_list<size_t> pSet) {
			Functor<R, Args...>::slots[index].set_triggers(pSet);
		}
	};
	template<typename R, class...Args>
	FunctorRef<R, Args...> createFunctor(R(*pF)(Args...), Args... pArgs) {
		size_t ind = Functor<R, Args...>::slots.size();
		if (!ind) {
			Functor<R, Args...>::invoker_index = functorInvokers.size();
			functorInvokers.push_back(Functor<R, Args...>::call);
			functorDestructors.push_back(Functor<R, Args...>::clear);
		}
		functorOrder.emplace_back(Functor<R, Args...>::invoker_index, ind);
		Functor<R, Args...>::slots.emplace_back(ind, pF, pArgs...);
		return ind;
	}
	void callFunctors();
	void clearFunctors();

	template<typename R, typename... Args>
	void reserve_functors(size_t pCount) {
		Functor<R, Args...>::reserve_slots(pCount);
	}
}