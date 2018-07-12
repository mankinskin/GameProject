#pragma once
#include "utils/id.h"
#include "event.h"

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

                    bool stat() const
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

                    bool stat() const
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

                    bool stat() const
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

                    bool stat() const
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

                    bool stat() const
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

                    bool stat() const
                    {
                        return !signal->stat();
                    }
                private:
                    const typename Signal::ID signal;
            };
        extern std::vector<void(*)()> signalClearFuncs;
        template<template<typename...> typename Op, typename... Signals>
            class SignalListener : public Op<Signals...>
        {
            public:
                using ID = utils::ID<SignalListener<Op, Signals...>>;
                static constexpr typename ID::Container& all = ID::container;

                SignalListener( const typename Signals::ID... sigs )
                    : Op<Signals...>( sigs... )
                {}

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
                struct At_Init
                {
                    At_Init()
                    {
                        signalClearFuncs.push_back(&SignalListener<Op, Signals...>::clear);
                    }
                };
                static At_Init init;
        };

        template<template<typename...> typename Op, typename... Signals>
            typename SignalListener<Op, Signals...>::At_Init SignalListener<Op, Signals...>::init;

    }
    template<typename SignalL, typename SignalR>
    utils::ID<hidden::SignalListener<hidden::And, SignalL, SignalR>> andsignal( utils::ID<SignalL> lhs, utils::ID<SignalR> rhs )
    {
        return utils::ID<hidden::SignalListener<hidden::And, SignalL, SignalR>>( hidden::SignalListener<hidden::And, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename SignalL, typename SignalR>
    utils::ID<hidden::SignalListener<hidden::Or, SignalL, SignalR>> orsignal( utils::ID<SignalL> lhs, utils::ID<SignalR> rhs )
    {
        return utils::ID<hidden::SignalListener<hidden::Or, SignalL, SignalR>>( hidden::SignalListener<hidden::Or, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename SignalL, typename SignalR>
    utils::ID<hidden::SignalListener<hidden::Xor, SignalL, SignalR>> xorsignal( utils::ID<SignalL> lhs, utils::ID<SignalR> rhs )
    {
        return utils::ID<hidden::SignalListener<hidden::Xor, SignalL, SignalR>>( hidden::SignalListener<hidden::Xor, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename SignalL, typename SignalR>
    utils::ID<hidden::SignalListener<hidden::Nor, SignalL, SignalR>> norsignal( utils::ID<SignalL> lhs, utils::ID<SignalR> rhs )
    {
        return utils::ID<hidden::SignalListener<hidden::Nor, SignalL, SignalR>>( hidden::SignalListener<hidden::Nor, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename SignalL, typename SignalR>
    utils::ID<hidden::SignalListener<hidden::Equal, SignalL, SignalR>> equalsignal( utils::ID<SignalL> lhs, utils::ID<SignalR> rhs )
    {
        return utils::ID<hidden::SignalListener<hidden::Equal, SignalL, SignalR>>( hidden::SignalListener<hidden::Equal, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename Signal>
    constexpr utils::ID<hidden::SignalListener<hidden::Not, Signal>> notsignal( utils::ID<Signal> s )
    {
        return utils::ID<hidden::SignalListener<hidden::Not, Signal>>( hidden::SignalListener<hidden::Not, Signal>( s ) );
    }

    void clearSignals();
}
