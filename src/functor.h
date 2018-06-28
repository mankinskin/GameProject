#pragma once
#include "signal.h"
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <tuple>
#include "utils/id.h"

namespace events
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
                static void func( R( &pF )( Args... ), std::tuple<Args...> pArgTuple, ExArgs&&... pExArgs )
                {
                    applier<N - 1, R, Args...>::func( pF, pArgTuple, std::get<N - 1>( pArgTuple ), pExArgs... );
                }
        };

    template<typename R, typename... Args>
        struct applier<0, R, Args...>
        {
            template<typename... ExArgs>
                static void func( R( &pF )( Args... ), std::tuple<Args...> pArgTuple, ExArgs&&... pExArgs )
                {
                    pF( pExArgs... );
                }
        };
    // used to extract parameter pack from tuple
    template<typename R, typename... Args>
        void function_caller( R( &pF )( Args... ), std::tuple<Args...> pArgTuple )
        {
            applier<sizeof...( Args ), R, Args...>::func( pF, pArgTuple );
        }
    //
    // Invoke functors using a functor-global array of invokers taking a local index
    extern std::vector<void(*)(const size_t)> invokers;
    extern std::vector<void(*)()> destructors;

    template<typename R, typename... Args>
        class Functor
        {
            public:
                Functor( R( &&pF )( Args... ), Args&&... pArgs )
                    : func( std::forward<R( Args... )>( pF ) )
                    , args( std::forward<Args>( pArgs )... )
                {
                    (void)init;
                }

                void invoke() const
                {
                    function_caller( func, args );
                }

                //invoke instance by index
                static void invoke( const size_t i )
                {
                    all[i].invoke();
                }
                static void clear()
                {
                    all.clear();
                }
                static std::vector<Functor<R, Args...>> all;
                static size_t invoker_index;

            private:
                R( &func )( Args... );
                std::tuple<Args...> args;
                struct At_Init
                {
                    At_Init()
                    {
                        Functor<R, Args...>::invoker_index = invokers.size();
                        invokers.push_back( Functor<R, Args...>::invoke );
                        destructors.push_back( Functor<R, Args...>::clear );
                    }
                };
                static At_Init init;
        };

    template<typename R, typename... Args>
        std::vector<Functor<R, Args...>> Functor<R, Args...>::all = std::vector<Functor<R, Args...>>();

    template<typename R, typename... Args>
        size_t Functor<R, Args...>::invoker_index = 0;

    template<typename R, typename... Args>
        typename Functor<R, Args...>::At_Init Functor<R, Args...>::init = Functor<R, Args...>::At_Init();

    struct FunctorID
    {
        template<typename R, typename... Args>
            FunctorID( R( &&pF )( Args... ), Args&&... pArgs )
            : invoker( Functor<R, Args...>::invoker_index )
            , index( Functor<R, Args...>::all.size() )
        {
            Functor<R, Args...>::all.push_back( Functor<R, Args...>( std::forward<R( Args... )>( pF ), std::forward<Args>( pArgs )... ) );
        }

        template<typename R, typename... Args>
            FunctorID( const Functor<R, Args...> pFunctor )
            : invoker( Functor<R, Args...>::invoker_index )
            , index( Functor<R, Args...>::all.size() )
        {
            Functor<R, Args...>::all.push_back( pFunctor );
        }

        void invoke() const
        {
            (*invokers[invoker])(index);
        }

        const size_t invoker; // invoker in functorInvokers
        const size_t index;   // local index for functor
    };

    template<typename R, typename... Args>
        FunctorID createFunctor( R( &&pF )( Args... ), Args&&... pArgs )
        {
            return FunctorID( std::forward<R( Args... )>( pF ), std::forward<Args>( pArgs )... );
        }

    void clearFunctors();
}
