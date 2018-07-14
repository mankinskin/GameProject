#pragma once
#include "utils/id.h"
#include "functor.h"
#include <tuple>

namespace signals
{
    namespace hidden
    {
        // plug these into signal listeners
        template<typename SignalL, typename SignalR>
            class And
            {
                public:
                    And( const utils::ID<SignalL> lhs, const utils::ID<SignalR> rhs )
                        : signalL( lhs )
                          , signalR( rhs )
                {}

                    constexpr bool stat()
                    {
                        return signalL->stat() && signalR->stat();
                    }
                private:
                    const utils::ID<SignalL> signalL;
                    const utils::ID<SignalR> signalR;
            };

        template<typename SignalL, typename SignalR>
            class Or
            {
                public:
                    Or( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
                        : signalL( lhs )
                          , signalR( rhs )
                {}

                    constexpr bool stat()
                    {
                        return signalL->stat() || signalR->stat();
                    }
                private:
                    const typename SignalL::ID signalL;
                    const typename SignalR::ID signalR;
            };

        template<typename SignalL, typename SignalR>
            class Xor
            {
                public:
                    Xor( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
                        : signalL( lhs )
                          , signalR( rhs )
                {}

                    constexpr bool stat()
                    {
                        return signalL->stat() != signalR->stat();
                    }
                private:
                    const typename SignalL::ID signalL;
                    const typename SignalR::ID signalR;
            };

        template<typename SignalL, typename SignalR>
            class Nor
            {
                public:
                    Nor( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
                        : signalL( lhs )
                          , signalR( rhs )
                {}

                    constexpr bool stat()
                    {
                        return !signalL->stat() && !signalR->stat();
                    }

                private:
                    const typename SignalL::ID signalL;
                    const typename SignalR::ID signalR;
            };

        template<typename SignalL, typename SignalR>
            class Equal
            {
                public:
                    Equal( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
                        : signalL( lhs )
                          , signalR( rhs )
                {}

                    constexpr bool stat()
                    {
                        return signalL->stat() == signalR->stat();
                    }
                private:
                    const typename SignalL::ID signalL;
                    const typename SignalR::ID signalR;
            };

        template<typename Signal>
            class Not
            {
                public:
                    Not( const typename Signal::ID s )
                        : signal( s )
                    {}

                    constexpr bool stat()
                    {
                        return !signal->stat();
                    }
                private:
                    const typename Signal::ID signal;
            };

        extern std::vector<void(*)()> signalClearFuncs;
        extern std::vector<void(*)()> eventCheckFuncs;
        extern std::vector<void(*)()> eventInstanceClearFuncs;

        template<template<typename...> typename Op, typename... Signals>
            class SignalListener : public Op<Signals...>
        {
            public:
                using ID = utils::ID<SignalListener<Op, Signals...>>;
                static constexpr typename ID::Container& all = ID::container;

                SignalListener( const Signals... sigs )
                    : Op<Signals...>( sigs... )
                {
                    initialize();
                }
                SignalListener( const utils::ID<Signals>... sigs )
                    : Op<Signals...>( sigs... )
                {
                    initialize();
                }

                static bool stat( size_t i )
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
                template<template<typename...> typename O, typename... S>
                    struct At_Init
                    {
                        At_Init()
                        {
                            signalClearFuncs.push_back(&SignalListener<O, S...>::clear);
                        }
                    };
        };

        template<typename Event>
            class EventSignal
            {
                public:
                    EventSignal(const Event pSignature)
                        : signature(pSignature)
                          , occurred(false)
                {
                    initialize();
                }

                    static void pushEvent(const Event pEvent)
                    {
                        eventBuffer.push_back( pEvent );
                    }

                    bool stat() const
                    {
                        return occurred;
                    }

                    const Event signature;
                    bool occurred = false;
                private:
                    static std::vector<Event> eventBuffer;
                    static void check()
                    {
                        for (SignalListener<EventSignal, Event>& instance : SignalListener<EventSignal, Event>::all) {
                            instance.occurred = false;
                            for (const Event& event : EventSignal<Event>::eventBuffer) {
                                if(instance.signature == event) {
                                    instance.occurred = true;
                                    break;
                                }
                            }
                        }
                        eventBuffer.clear();
                    }
                    static void initialize()
                    {
                        static At_Init init;
                    }

                    struct At_Init
                    {
                        At_Init()
                        {
                            eventCheckFuncs.push_back(&check);
                        }
                    };
            };

        template<typename Event>
            std::vector<Event> EventSignal<Event>::eventBuffer = std::vector<Event>();


    }
    template<typename Event>
        utils::ID<hidden::SignalListener<hidden::EventSignal, Event>> eventsignal(const Event pEvent)
        {
            return utils::makeID(hidden::SignalListener<hidden::EventSignal, Event>(pEvent));
        }


    template<typename SignalL, typename SignalR>
        utils::ID<hidden::SignalListener<hidden::And, SignalL, SignalR>> andsignal(utils::ID<SignalL> lhs, utils::ID<SignalR> rhs)
        {
            return utils::makeID(hidden::SignalListener<hidden::And, SignalL, SignalR>(lhs, rhs));
        }

    template<typename SignalL, typename SignalR>
        utils::ID<hidden::SignalListener<hidden::Or, SignalL, SignalR>> orsignal(utils::ID<SignalL> lhs, utils::ID<SignalR> rhs)
        {
            return utils::makeID(hidden::SignalListener<hidden::Or, SignalL, SignalR>(lhs, rhs));
        }

    template<typename SignalL, typename SignalR>
        utils::ID<hidden::SignalListener<hidden::Xor, SignalL, SignalR>> xorsignal(utils::ID<SignalL> lhs, utils::ID<SignalR> rhs)
        {
            return utils::makeID(hidden::SignalListener<hidden::Xor, SignalL, SignalR>(lhs, rhs));
        }

    template<typename SignalL, typename SignalR>
        utils::ID<hidden::SignalListener<hidden::Nor, SignalL, SignalR>> norsignal(utils::ID<SignalL> lhs, utils::ID<SignalR> rhs)
        {
            return utils::makeID(hidden::SignalListener<hidden::Nor, SignalL, SignalR>(lhs, rhs));
        }

    template<typename SignalL, typename SignalR>
        utils::ID<hidden::SignalListener<hidden::Equal, SignalL, SignalR>> equalsignal(utils::ID<SignalL> lhs, utils::ID<SignalR> rhs)
        {
            return utils::makeID(hidden::SignalListener<hidden::Equal, SignalL, SignalR>(lhs, rhs));
        }

    template<typename Signal>
        constexpr utils::ID<hidden::SignalListener<hidden::Not, Signal>> notsignal( utils::ID<Signal> s )
        {
            return utils::makeID(hidden::SignalListener<hidden::Not, Signal>(s));
        }

    struct Listener
    {
        using ID = utils::ID<Listener>;
        constexpr static typename ID::Container& all = ID::container;

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
        private:
    };

    template<typename Event>
        void pushEvent(const Event pEvent)
        {
            hidden::EventSignal<Event>::pushEvent(pEvent);
        }

    template<typename Event>
        utils::ID<Listener> listen( const utils::ID<hidden::SignalListener<hidden::EventSignal, Event>> pEvent )
        {
            return utils::ID<Listener>( pEvent );
        }
    template<typename Event>
        utils::ID<Listener> listen( const Event pEvent )
        {
            return listen( utils::makeID( hidden::SignalListener<hidden::EventSignal, Event>(pEvent)));
        }

    template<template<typename...> typename Op, typename... Signals>
        utils::ID<Listener> listen( const utils::ID<hidden::SignalListener<Op, Signals...>> pSignal )
        {
            return utils::ID<Listener>( pSignal );
        }

    void processLinks();

    namespace hidden
    {
        extern std::vector<std::pair<Listener, Invoker>> links;
        void link(Listener pListener, Invoker pInvoker);
    }

    template<typename Signal, typename R, typename... Args>
        void link(utils::ID<Signal> pSignal, R(&&pF)(Args...), Args&&... pArgs)
        {
            hidden::link(Listener(pSignal), Invoker(std::forward<R(Args...)>(pF), std::forward<Args>(pArgs)...));
        }

    template<typename Signal, typename... Funcs>
        void link(utils::ID<Signal> pSignal, utils::ID<Funcs>... pFuncs)
        {
            hidden::link(Listener(pSignal), Invoker(pFuncs...));
        }
    template<typename Signal, typename R, typename... Args>
        void link(utils::ID<Signal> pSignal, utils::ID<hidden::Functor<R, Args...>> pFunc)
        {
            hidden::link(Listener(pSignal), Invoker(pFunc));
        }

    template<typename Signal, typename... Funcs>
        void link(utils::ID<Signal> pSignal, utils::ID<hidden::Procedure<Funcs...>> pProc)
        {
            hidden::link(Listener(pSignal), Invoker(pProc));
        }
    void checkEvents();
    void clearSignals();
}
