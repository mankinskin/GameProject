#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <tuple>
#include "Signal.h"
/*
Each functor holds a function pointer and a tuple for the arguments.
when a functor class is first instantited from the template, its static call(size_t) function is given to the functorInvokers.
this function will later be used to call a functor object by its index.
when a functor object is created, its template_index and its slot index is inserted into the order_queue, which ensures that all functors will always be called in the order in which they were created
when checking all functors, the functorOrder queue is used. the invoker_index is used to determine the class of the functor and the slot_index is used to find the object to be invoked.
*/
namespace app {
	namespace Input {

		struct AnyGate {
			bool func(std::vector<size_t>& pSet, bool(*check)(size_t)) {
				return std::any_of(pSet.begin(), pSet.end(), check);
			}
		};
		struct AllGate {
			bool func(std::vector<size_t>& pSet, bool(*check)(size_t)) {
				return std::all_of(pSet.begin(), pSet.end(), check);
			}
		};
		struct SequenceGate {
			bool func(std::vector<size_t>& pSet, bool(*check)(size_t)) {
				for (size_t s = 0; s < pSet.size(); ++s) {
					if (!check(pSet[s]) && s >= dropped) {
						dropped = s;
						return false;
					}
				}
				dropped = 0;
				return true;
			}
		private:
			int dropped = 0;
		};
		struct AlwaysGate {
			bool func(std::vector<size_t>& pSet, bool(*check)(size_t)) {
				return true;
			}
		};


		template<size_t N, typename... Args>
		struct applier {
			template<typename...ExArgs>
			static void func(void(*pF)(Args...), const std::tuple<Args...>& pArgTuple, ExArgs&&... pExArgs) {
				applier<N - 1, Args...>::func(pF, pArgTuple, std::get<N - 1>(pArgTuple), pExArgs...);
			}
		};
		template<typename... Args>
		struct applier<0, Args...> {
			template<typename...ExArgs>
			static void func(void(*pF)(Args...), const std::tuple<Args...>& pArgTuple, ExArgs&&... pExArgs) {
				pF(pExArgs...);
			}
		};

		template<typename... Args>
		void function_caller(void(*pF)(Args...), const std::tuple<Args...>& pArgTuple) {
			applier<sizeof...(Args), Args...>::func(pF, pArgTuple);
		}

		template<typename... Args>
		class Functor {
		public:
			Functor(size_t pIndex, void(*pF)(Args...), Args... pArgs)
				:func(pF), args(std::forward_as_tuple<Args...>(std::forward<Args>(pArgs)...)), slot_index(pIndex)
			{}
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
				if(slots[pFunc].check()) {
					slots[pFunc].invoke();
				}
			}
			bool check() {
				return std::any_of(signalSet.begin(), signalSet.end(), [](size_t s)->bool {return allSignals[s]; });
			}
			void setTriggers(std::initializer_list<size_t> pSet) {
				signalSet = pSet;
			}
			static size_t invoker_index;
			static std::vector<Functor<Args...>> slots;
			size_t slot_index;
		private:
			struct initializer {
				initializer() {
					functorDestructors.push_back(clear);
					invoker_index = functorInvokers.size();
					functorInvokers.push_back(call);
				}
			};
			const initializer ini = initializer();
			std::vector<size_t> signalSet;
			void(*func)(Args...);
			std::tuple<Args...> args;
			
		};

		template<typename... Args>
		std::vector<Functor<Args...>> Functor<Args...>::slots = std::vector<Functor<Args...>>();
		template<typename... Args>
		size_t Functor<Args...>::invoker_index = 0;

		extern std::vector<void(*)(size_t)> functorInvokers;//invokes all functor templates
		extern std::vector<void(*)()> functorDestructors;//destroys all functor templates
		extern std::vector<std::pair<size_t, size_t>> functorOrder;

		template<class...Args>
		struct FunctorRef {
			FunctorRef(size_t pIndex):index(pIndex){}
			size_t index;
			void setTriggers(std::initializer_list<size_t> pSet) {
				Functor<Args...>::slots[index].setTriggers(pSet);
			}
		};
		template<class...Args>
		FunctorRef<Args...> createFunctor(void(*pF)(Args...), Args... pArgs) {
			size_t ind = Functor<Args...>::slots.size();
			functorOrder.emplace_back(Functor<Args...>::invoker_index, ind);
			Functor<Args...>::slots.emplace_back(ind, pF, pArgs...);
			return ind;
		}
		void callFunctors();
		void clearFunctors();

		template<typename... Args>
		void reserve_functors(size_t pCount) {
			Functor<Args...>::reserve_slots(pCount);
		}
	}//end Input
}//end app