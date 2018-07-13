#pragma once
#include <tuple>
#include "utils/id.h"
#include "functor.h"
#include "signal.h"
#include <vector>

namespace signals
{
    struct Listener
    {
        using ID = utils::ID<Listener>;
        constexpr static typename ID::Container& all = ID::container;

        template<typename Event>
            Listener( const utils::ID<hidden::EventListener<Event>> pListener )
            : stater( hidden::EventListener<Event>::stat )
            , index( pListener.index )
        {}

        template<template<typename...> typename Op, typename... Signals>
            Listener( const utils::ID<hidden::SignalListener<Op, Signals...>> pListener )
            : stater( hidden::SignalListener<Op, Signals...>::stat )
            , index( pListener.index )
        {}
            Listener( const utils::ID<Listener> pListener )
            : stater( pListener->stater )
            , index( pListener->index )
        {}

        bool stat() const
        {
            stater(index);
        }

        bool(&stater)(const size_t);
        const size_t index;
    };

    template<typename Event>
        utils::ID<Listener> listen( const utils::ID<hidden::EventListener<Event>> pEvent )
        {
            return utils::ID<Listener>( pEvent );
        }

    template<template<typename...> typename Op, typename... Signals>
        utils::ID<Listener> listen( const utils::ID<hidden::SignalListener<Op, Signals...>> pSignal )
        {
            return utils::ID<Listener>( pSignal );
        }

    void processReactions();

    namespace hidden
    {
        struct Invoker
        {
            template<typename R, typename... Args>
                Invoker(R(&&pF)(Args...), Args&&... pArgs)
                : invoker(hidden::Functor<R, Args...>::invoke)
                  , index(functor(std::forward<R(Args...)>(pF), std::forward<Args>(pArgs)...).index)
            {}

            template<typename... Funcs>
                Invoker(utils::ID<Funcs>... pFuncs)
                : invoker(hidden::Procedure<Funcs...>::invoke)
                  , index(procedure(pFuncs...).index)
            {}

            template<typename R, typename... Args>
                Invoker(utils::ID<hidden::Functor<R, Args...>> pFunc)
                : invoker(hidden::Functor<R, Args...>::invoke)
                  , index(pFunc.index)
            {}

            template<typename... Funcs>
                Invoker(utils::ID<hidden::Procedure<Funcs...>> pProc)
                : invoker(hidden::Procedure<Funcs...>::invoke)
                  , index(pProc.index)
            {}

            void invoke() const
            {
                invoker(index);
            }

            void(&invoker)(const size_t);
            const size_t index;
        };

        extern std::vector<std::pair<Listener, Invoker>> reactions;
    }

    template<typename Signal, typename R, typename... Args>
        void reaction(utils::ID<Signal> pSignal, R(&&pF)(Args...), Args&&... pArgs)
        {
            hidden::reactions.push_back(std::make_pair(Listener(pSignal), Invoker(std::forward<R(Args...)>(pF), std::forward<Args>(pArgs)...)));
        }

    template<typename Signal, typename... Funcs>
        void reaction(utils::ID<Signal> pSignal, utils::ID<Funcs>... pFuncs)
        {
            hidden::reactions.push_back(std::make_pair(Listener(pSignal), hidden::Invoker(pFuncs...)));
        }
    template<typename Signal, typename R, typename... Args>
        void reaction(utils::ID<Signal> pSignal, utils::ID<hidden::Functor<R, Args...>> pFunc)
        {
            hidden::reactions.push_back(std::make_pair(Listener(pSignal), hidden::Invoker(pFunc)));
        }

    template<typename... Funcs>
        void reaction(utils::ID<Listener> pListener, utils::ID<hidden::Procedure<Funcs...>> pProc)
        {
            hidden::reactions.push_back(std::make_pair(pListener, hidden::Invoker(pProc)));
        }
}

