#pragma once
#include "utils/id.h"
#include "event.h"

namespace signals
{
    template<template<typename...> typename Op, typename... Signals>
        class SignalListener : public Op<Signals...>
        {
            public:
                using ID = utils::ID<SignalListener<Op, Signals...>>;
                static constexpr typename ID::Container& all = ID::container;

                SignalListener( const typename Signals::ID... sigs )
                    : Op<Signals...>( sigs... )
                {
                    (void)init;
                }

                //bool stat_n( utils::_index<0> i ) const
                //{
                //    return std::get<0>( signals )->stat();
                //}

                //template<size_t N>
                //bool stat_n( utils::_index<N> i ) const
                //{
                //    return Op::op( std::get<N>( signals )->stat(), stat_n( utils::_index<N-1>() ) );
                //}

                //bool stat()
                //{
                //    return stat_n( utils::_index<sizeof...(Signals)-1>() );
                //}

                bool stat() const
                {
                    return Op<Signals...>::stat();
                }

                static bool stat( size_t i )
                {
                    return all[i].stat();
                }

                static void clear()
                {
                    all.clear();
                }

                static size_t templateIndex;  // the index of the template instantiation
            private:
                struct At_Init
                {
                    At_Init()
                    {
                        templateIndex = template_count++;
                        staters.push_back( stat );
                        clearers.push_back( clear );
                    }
                };
                static At_Init init;
        };

    template<template<typename...> typename Op, typename... Signals>
        size_t SignalListener<Op, Signals...>::templateIndex = 0;

    template<template<typename...> typename Op, typename... Signals>
        typename SignalListener<Op, Signals...>::At_Init SignalListener<Op, Signals...>::init = SignalListener<Op, Signals...>::At_Init();

    //template<template<typename...> typename Op, typename Signals...>
    //using Signal = SignalListener<Op, Signals...>::ID;

    // plug these into signal listeners
    template<typename SignalL, typename SignalR>
    class And
    {
        public:
        And( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
            : signalL( lhs )
            , signalR( rhs )
        {}

        bool stat() const
        {
            return signalL->stat() && signalR->stat();
        }
        private:
        const typename SignalL::ID signalL;
        const typename SignalR::ID signalR;
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

    template<typename SignalL, typename SignalR>
    typename SignalListener<And, SignalL, SignalR>::ID andsignal( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
    {
        return utils::ID<SignalListener<And, SignalL, SignalR>>( SignalListener<Equal, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename SignalL, typename SignalR>
    typename SignalListener<Or, SignalL, SignalR>::ID orsignal( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
    {
        return utils::ID<SignalListener<Or, SignalL, SignalR>>( SignalListener<Equal, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename SignalL, typename SignalR>
    typename SignalListener<Xor, SignalL, SignalR>::ID xorsignal( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
    {
        return utils::ID<SignalListener<Xor, SignalL, SignalR>>( SignalListener<Equal, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename SignalL, typename SignalR>
    typename SignalListener<Nor, SignalL, SignalR>::ID norsignal( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
    {
        return utils::ID<SignalListener<Nor, SignalL, SignalR>>( SignalListener<Equal, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename SignalL, typename SignalR>
    typename SignalListener<Equal, SignalL, SignalR>::ID equalsignal( const typename SignalL::ID lhs, const typename SignalR::ID rhs )
    {
        return utils::ID<SignalListener<Equal, SignalL, SignalR>>( SignalListener<Equal, SignalL, SignalR>( lhs, rhs ) );
    }

    template<typename Signal>
    constexpr typename SignalListener<Not, Signal>::ID notsignal( utils::ID<Signal> s )
    {
        return utils::ID<SignalListener<Not, Signal>>( SignalListener<Not, Signal>( s ) );
    }

    struct ListenerID
    {
        ListenerID()
            : templateIndex( 0 )
            , instanceIndex( 0 )
        {}
        template<typename Event>
            ListenerID( const utils::ID<EventListener<Event>> pListener )
            : templateIndex( EventListener<Event>::templateIndex )
            , instanceIndex( pListener.index )
        {}

        template<template<typename...> typename Op, typename... Signals>
            ListenerID( const utils::ID<SignalListener<Op, Signals...>> pListener )
            : templateIndex( SignalListener<Op, Signals...>::templateIndex )
            , instanceIndex( pListener.index )
        {}

        bool stat() const
        {
            return staters[templateIndex]( instanceIndex );
        }

        private:
        size_t templateIndex;
        size_t instanceIndex;
    };

    template<typename Event>
        ListenerID listen( const utils::ID<EventListener<Event>> pEvent )
        {
            return ListenerID( pEvent );
        }

    template<template<typename...> typename Op, typename... Signals>
        ListenerID listen( const utils::ID<SignalListener<Op, Signals...>> pSignal )
        {
            return ListenerID( pSignal );
        }
}
