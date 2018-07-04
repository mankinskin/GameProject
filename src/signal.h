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

                SignalListener( typename Signals::ID... sigs )
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
        And( typename SignalL::ID lhs, typename SignalR::ID rhs )
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
        Or( typename SignalL::ID lhs, typename SignalR::ID rhs )
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
        Xor( typename SignalL::ID lhs, typename SignalR::ID rhs )
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
        Nor( typename SignalL::ID lhs, typename SignalR::ID rhs )
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
        Equal( typename SignalL::ID lhs, typename SignalR::ID rhs )
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
        Not( typename Signal::ID s )
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
    typename SignalListener<And, SignalL, SignalR>::ID andsignal( typename SignalL::ID lhs, typename SignalR::ID rhs )
    {
        return typename SignalListener<And, SignalL, SignalR>::ID( lhs, rhs );
    }

    template<typename SignalL, typename SignalR>
    typename SignalListener<Or, SignalL, SignalR>::ID orsignal( typename SignalL::ID lhs, typename SignalR::ID rhs )
    {
        return typename SignalListener<Or, SignalL, SignalR>::ID( lhs, rhs );
    }

    template<typename SignalL, typename SignalR>
    typename SignalListener<Xor, SignalL, SignalR>::ID xorsignal( typename SignalL::ID lhs, typename SignalR::ID rhs )
    {
        return typename SignalListener<Xor, SignalL, SignalR>::ID( lhs, rhs );
    }

    template<typename SignalL, typename SignalR>
    typename SignalListener<Nor, SignalL, SignalR>::ID norsignal( typename SignalL::ID lhs, typename SignalR::ID rhs )
    {
        return typename SignalListener<Nor, SignalL, SignalR>::ID( lhs, rhs );
    }

    template<typename SignalL, typename SignalR>
    typename SignalListener<Equal, SignalL, SignalR>::ID equalsignal( typename SignalL::ID lhs, typename SignalR::ID rhs )
    {
        return typename SignalListener<Equal, SignalL, SignalR>::ID( lhs, rhs );
    }

    template<typename Signal>
    typename SignalListener<Not, Signal>::ID notsignal( typename Signal::ID s )
    {
        return typename SignalListener<Not, Signal>::ID( s );
    }

    struct ListenerID
    {
        ListenerID()
            : templateIndex( 0 )
            , instanceIndex( 0 )
        {}
        template<typename Event>
            ListenerID( const EventListener<Event> pListener )
            : templateIndex( EventListener<Event>::templateIndex )
            , instanceIndex( EventListener<Event>::all.size() )
        {
            EventListener<Event>::all.push_back( pListener );
        }

        template<template<typename...> typename Op, typename... Signals>
            ListenerID( const SignalListener<Op, Signals...> pListener )
            : templateIndex( SignalListener<Op, Signals...>::templateIndex )
            , instanceIndex( SignalListener<Op, Signals...>::all.size() )
        {
            SignalListener<Op, Signals...>::all.push_back( pListener );
        }

        bool stat() const
        {
            return staters[templateIndex]( instanceIndex );
        }
        private:
        size_t templateIndex;
        size_t instanceIndex;
    };

    template<typename Event>
        ListenerID listenForEvent( Event pEvent )
        {
            return ListenerID( EventListener<Event>( pEvent ) );
        }
    template<template<typename...> typename Op, typename... Signals>
        ListenerID listenForSignals( typename Signals::ID... pSignals )
        {
            return ListenerID( SignalListener<Op, Signals...>( pSignals... ) );
        }
}
