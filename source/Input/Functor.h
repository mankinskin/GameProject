#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <tuple>
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
				for (size_t s = 0 + (pSet.size() - 1)*passed; s < pSet.size(); ++s) {
					if (!check(pSet[s])) {//if 
						passed = 0;
						return false;
					}
				}
				passed = 1;
				return true;
			}
		private:
			int passed = 0;
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
			Functor(void(*pF)(Args...), Args... pArgs)
				:func(pF), args(std::forward_as_tuple<Args...>(std::forward<Args>(pArgs)...)), slot_index(slots.size())
			{
				slots.emplace_back(*this);
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

			static std::vector<Functor<Args...>> slots;
			size_t slot_index;
		private:
			struct initializer {
				initializer() {
					functorDestructors.push_back(clear);
				}
			};
			const initializer ini = initializer();

			void(*func)(Args...);
			std::tuple<Args...> args;
		};
		bool is_on(size_t pSignalIndex);

		template<class Gate, typename... Args>
		class FunctorTrigger {
		public:
			FunctorTrigger(Functor<Args...> pFunctor, std::vector<size_t> pTriggerSignals)
				:signalSet(pTriggerSignals), slot_index(pFunctor.slot_index)
			{
				functorOrder.emplace_back(invoker_index, triggers.size());
				triggers.emplace_back(*this);
			}
			static void call(size_t pTrigger) {
				if (triggers[pTrigger].check(is_on)) {
					Functor<Args...>::slots[triggers[pTrigger].slot_index].invoke();
				}
			}
			bool check(bool(*check)(size_t)) {
				return gate.func(signalSet, check);
			}

		private:
			struct initializer {
				initializer() {
					invoker_index = functorInvokers.size();
					functorInvokers.push_back(call);
				}
			};
			const initializer ini = initializer();
			size_t slot_index;
			static std::vector<FunctorTrigger<Gate, Args...>> triggers;
			static size_t invoker_index;
			std::vector<size_t> signalSet;
			Gate gate;
		};
		template<class Gate, typename... Args>
		size_t FunctorTrigger<Gate, Args...>::invoker_index = 0;

		template<class Gate, typename... Args>
		std::vector<FunctorTrigger<Gate, Args...>> FunctorTrigger<Gate, Args...>::triggers = std::vector<FunctorTrigger<Gate, Args...>>();

		template<typename... Args>
		std::vector<Functor<Args...>> Functor<Args...>::slots = std::vector<Functor<Args...>>();

		extern std::vector<void(*)(size_t)> functorInvokers;//invokes all functor templates
		extern std::vector<void(*)()> functorDestructors;//destroys all functor templates
		extern std::vector<std::pair<size_t, size_t>> functorOrder;



		void callFunctors();
		void clearFunctors();

		template<typename... Args>
		void reserve_functors(size_t pCount) {
			Functor<Args...>::reserve_slots(pCount);
		}
	}//end Input
}//end app


//ANOTHER APPROACH
// EXPRESSION FUNCTORS
//
//template<typename T>
//struct Min {
//	Min(T& LHS, T& RHS)
//		 :lhs(LHS), rhs(RHS) {}
//	
//		T& lhs;
//	T& rhs;
//	inline T& operator()() {
//		return lhs < rhs ? lhs : rhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pRhs) {
//		return pLhs < pRhs ? pLhs : pRhs;
//		
//	}
//	
//		
//};
//template<typename T>
//struct Max {
//	Max(T& LHS, T& RHS)
//		:lhs(LHS), rhs(RHS) {}
//	
//		
//		T& lhs;
//	T& rhs;
//	inline T& operator()() {
//		return lhs > rhs ? lhs : rhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pRhs) {
//		return pLhs > pRhs ? pLhs : pRhs;
//		
//	}
//};
//template<typename T>
//struct Add {
//	Add(T& LHS, T& RHS)
//		 :lhs(LHS), rhs(RHS) {}
//	
//		T& lhs;
//	T& rhs;
//	inline T operator()() {
//		return lhs + rhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pRhs) {
//		return pLhs + pRhs;
//		
//	}
//
//};
//template<typename T>
//struct Substract {
//	Substract(T& LHS, T& RHS)
//		 :lhs(LHS), rhs(RHS) {}
//	
//		T& lhs;
//	T& rhs;
//	inline T operator()() {
//		return lhs - rhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pRhs) {
//		return pLhs - pRhs;
//		
//	}
//};
//template<typename T>
//struct Pass {
//		Pass(T& VAL)
//		 :lhs(VAL) {}
//	
//		T& lhs;
//	inline T& operator()() {
//		return lhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pLhs, R&& pDummy) {
//		return pLhs;
//		
//	}
//};
//template<typename T>
//struct Set {
//	Set(T& LHS, T& RHS)
//		 :lhs(LHS), rhs(RHS) {}
//	
//		T& lhs;
//	T& rhs;
//	inline T& operator()() {
//		lhs = rhs;
//		return lhs;
//		
//	}
//	template<typename L, typename R>
//		static L func(L&& pTarget, R&& pSource) {
//			pTarget = pSource;
//			return pTarget;
//	}
//};
//template<typename T, template<typename> class FuncPolicy, class LhsSource, class RhsSource>
//struct Operation {
//	Operation(LhsSource pLhsSource, RhsSource pRhsSource)
//		 :lhs(pLhsSource), rhs(pRhsSource) {
//		}
//	T operator()() {
//		return FuncPolicy<T>::func(lhs(), rhs());
//	}
//	LhsSource lhs;
//	RhsSource rhs;
//};