#pragma once
#include "gates.h"
#include "utils/id.h"
#include <vector>
#include <tuple>

namespace signals
{
    // Events set signals when their signature gets pushed into its queue ( with pushEvent )
    // Events are templated, thus signals of any type can be tracked.

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
                using ID = utils::ID<EventListener<Event>>;
                static constexpr typename ID::Container& all = ID::container;

                EventListener( const Event pSignature )
                    : signature( pSignature )
                    , occurred( false )
                {
                    (void)init;
                }

                static void push( Event pEvent )
                {
                    if ( all.size() ) {  // if listening for any signals
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

                bool stat() const
                {
                    return occurred;
                }

                static bool stat( size_t i )
                {
                    return all[i].stat();
                }

                const Event signature;
                bool occurred = false;

                static std::vector<Event> eventBuffer;
                static size_t templateIndex;  // the index of the template instantiation
            private:
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
        size_t EventListener<Event>::templateIndex = 0;

    template<typename Event>
        typename EventListener<Event>::At_Init EventListener<Event>::init = EventListener<Event>::At_Init();

    template<typename Event>
        void pushEvent( Event pEvent )
        {
            EventListener<Event>::push( pEvent );
        }

    template<typename Event>
        constexpr utils::ID<EventListener<Event>> eventsignal( const Event pEvent )
        {
            return utils::ID<EventListener<Event>>( pEvent );
        }
}
