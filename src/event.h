#pragma once
#include "gates.h"
#include "utils/id.h"
#include "utils/tuple_utils.h"
#include <vector>
#include <tuple>

namespace events
{
    // Events set signals when their signature gets pushed into its queue ( with pushEvent )
    // Events are templated, thus events of any type can be tracked.

    void checkEvents();
    void clearEvents();

    extern size_t template_count;
    extern std::vector<void(*)()> checkers;  // for checking all Events
    extern std::vector<bool(*)(size_t)> staters;  // for getting the status of all listeners
    extern std::vector<void(*)()> clearers; // resets all Event queues

    template<typename Event>
        class EventListener
        {
            public:
                static std::vector<EventListener<Event>> all;
                EventListener( const Event pSignature )
                    : signature( pSignature )
                    , occurred( false )
                {
                    (void)init;
                }

                static void push( Event pEvent )
                {
                    if ( all.size() ) {  // if listening for any events
                        eventBuffer.push_back( pEvent );
                    }
                }

                static void clearBuffer()
                {
                    eventBuffer.clear();
                }

                static void clear()
                {
                    all.clear();
                    clearBuffer();
                }

                void checkEvent( const Event pEvent )
                {
                    if ( !occurred && pEvent == signature ) {
                        occurred = true;
                    }
                }

                static void checkAll()
                {
                    for ( EventListener<Event>& instance : all ) {
                        instance.occurred = false;
                        for ( const Event& event : eventBuffer ) {
                            instance.checkEvent( event );
                        }
                    }
                    clearBuffer();
                }

                static bool stat( size_t i )
                {
                    return all[i].occurred;
                }

                const Event signature;
                bool occurred = false;

                static std::vector<Event> eventBuffer;
            private:
                friend struct ListenerID;
                static size_t templateIndex;  // the index of the template instantiation
                struct At_Init
                {
                    At_Init()
                    {
                        templateIndex = template_count++;
                        checkers.push_back( checkAll );
                        staters.push_back( stat );
                        clearers.push_back( clear );
                    }
                };
                static At_Init init;
        };

    template<typename Event>
        std::vector<Event> EventListener<Event>::eventBuffer = std::vector<Event>();

    template<typename Event>
        std::vector<EventListener<Event>> EventListener<Event>::all = std::vector<EventListener<Event>>();

    template<typename Event>
        size_t EventListener<Event>::templateIndex = 0;

    template<typename Event>
        typename EventListener<Event>::At_Init EventListener<Event>::init = EventListener<Event>::At_Init();

    template<typename Event>
        void pushEvent( Event pEvent )
        {
            EventListener<Event>::push( pEvent );
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

    template<typename... Signals>
        class SignalListener
        {
            public:
                SignalListener( Signals... pSignals )
                    : signals{ pSignals... }
                {
                    (void)init;
                }

                bool stat_n( utils::_index<0> i )
                {
                    return std::get<0>( signals );
                }

                template<size_t N>
                bool stat_n( utils::_index<N> i )
                {
                    return std::get<N>( signals ).stat() && stat_n( utils::_index<N-1>() );
                }

                bool stat()
                {
                    return stat_n( utils::_index<sizeof...(Signals)>() );
                }

                static bool stat( size_t i )
                {
                    return all[i].stat();
                }

                static void clear()
                {
                    all.clear();
                }

                std::tuple<Signals...> signals;
                static std::vector<SignalListener<Signals...>> all;
            private:
                static size_t templateIndex;  // the index of the template instantiation
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

    template<typename... Signals>
        std::vector<SignalListener<Signals...>> SignalListener<Signals...>::all = std::vector<SignalListener<Signals...>>();

    template<typename... Signals>
        size_t SignalListener<Signals...>::templateIndex = 0;

    template<typename... Signals>
        typename SignalListener<Signals...>::At_Init SignalListener<Signals...>::init = SignalListener<Signals...>::At_Init();

}
