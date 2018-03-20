#pragma once
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <tuple>
#include "signal.h"

namespace functors 
{
/*
Each functor holds a function pointer and a tuple for the arguments.
when a functor class is first instantited from the template, its static void call( unsigned int index ) function is pushed onto the functorInvokers.
this function will later be used to call a functor object by its index.
when a functor object is created, its template_index and its slot index is inserted into the order_queue, which ensures that all functors will always be called in the order in which they were created.
when checking all functors, the functorOrder queue is used. the invoker_index is used to determine the class of the functor and the slot_index is used to find the functor object to be invoked.
*/

	template<unsigned int N, typename R, typename... Args>
	struct applier 
    {
		template<typename...ExArgs>
		static void func( R( *pF )( Args... ), const std::tuple<Args...>& pArgTuple, ExArgs&&... pExArgs ) 
        {
			applier<N - 1, R, Args...>::func( pF, pArgTuple, std::get<N - 1>( pArgTuple ), pExArgs... );
		}
	};
	template<typename R, typename... Args>
	struct applier<0, R, Args...> 
    {
		template<typename...ExArgs>
		static void func( R( *pF )( Args... ), const std::tuple<Args...>& pArgTuple, ExArgs&&... pExArgs ) 
        {
			pF( pExArgs... );
		}
	};

	template<typename R, typename... Args>
	void function_caller( R( *pF )( Args... ), const std::tuple<Args...>& pArgTuple ) 
    {
		applier<sizeof...( Args ), R, Args...>::func( pF, pArgTuple );
	}

	template<typename R, typename... Args>
	class Functor 
    {
	public:
		Functor( unsigned int pIndex, R( *pF )( Args... ), Args... pArgs )
			:func( pF ), 
            args( std::forward_as_tuple<Args...>( std::forward<Args>( pArgs )... ) ), 
            slot_index( pIndex )
		{ }
		static void clear() 
        {
			slots.clear();
		}
		static void reserve_slots( unsigned int pCount ) 
        {
			slots.reserve( pCount );
		}
		void invoke() const 
        {
			function_caller( func, args );
		}
		static void call( unsigned int pFunc ) 
        {
			if ( slots[pFunc].check() ) 
            {
				slots[pFunc].invoke();
			}
		}
		bool check() 
        {
			for ( unsigned int s = 0; s < signalSet.size(); ++s ) 
            {
				if ( signals::allSignals[signalSet[s]] ) 
                {
					return true;
				}
			}
			return false;
		}
		void set_triggers( std::initializer_list<unsigned int> pSet ) 
        {
			signalSet = pSet;
		}
        void add_trigger( unsigned int pSignal )
        {

        }
		static unsigned int invoker_index;
		static std::vector<Functor<R, Args...>> slots;
		unsigned int slot_index;
	private:
		std::vector<unsigned int> signalSet;
		R( *func )( Args... );
		std::tuple<Args...> args;
	};

	template<typename R, typename... Args>
	std::vector<Functor<R, Args...>> Functor<R, Args...>::slots = std::vector<Functor<R, Args...>>();
	template<typename R, typename... Args>
	unsigned int Functor<R, Args...>::invoker_index = 0;

	extern std::vector<void( * )( unsigned int )> functorInvokers;//invokes all functor templates
	extern std::vector<void( * )()> functorDestructors;//destroys all functor templates
	extern std::vector<std::pair<unsigned int, unsigned int>> functorOrder;

	template<typename R, class...Args>
	struct FunctorRef 
    {
		FunctorRef( unsigned int pIndex ) :index( pIndex ) 
        {}
		void set_triggers( std::initializer_list<unsigned int> pSet ) 
        {
			Functor<R, Args...>::slots[index].set_triggers( pSet );
		}

		unsigned int index;
	};
	template<typename R, class...Args>
	FunctorRef<R, Args...> createFunctor( R( *pF )( Args... ), Args... pArgs ) 
    {
		unsigned int ind = Functor<R, Args...>::slots.size();
		if ( !ind ) 
        {
			Functor<R, Args...>::invoker_index = functorInvokers.size();
			functorInvokers.push_back( Functor<R, Args...>::call );
			functorDestructors.push_back( Functor<R, Args...>::clear );
		}
		functorOrder.emplace_back( Functor<R, Args...>::invoker_index, ind );
		Functor<R, Args...>::slots.emplace_back( ind, pF, pArgs... );
		return ind;
	}

	template<typename R, typename... Args>
	void reserve_functors( unsigned int pCount ) 
    {
		Functor<R, Args...>::reserve_slots( pCount );
	}

	void callFunctors();
	void clearFunctors();
}
