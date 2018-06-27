#pragma once
#include "gates.h"
#include <vector>

namespace events 
{
    // Events set signals when their signature gets pushed into its queue ( with pushEvent )
    // Events are templated, thus events of any type can be tracked. 

    void checkEvents();
    void resetEvents();

    extern size_t template_count;
    extern std::vector<void(*)()> checkers;  // for checking all Events
    extern std::vector<bool(*)(size_t)> staters;  // for getting the status of all listeners
    extern std::vector<void(*)()> resetters; // resets all Event queues

    template<typename Event>
        class Listener
        {
            public:
                static std::vector<Listener<Event>> all;

                Listener( const Event pSignature )
                    : signature( pSignature )
                    , occurred( false )
                {
                    (void)init;
                }

                static void push( Event pEvent )
                {
                    if( all.size() ) {  // if listening for any events
                        eventBuffer.push_back( pEvent );
                    }
                }

                static void clearBuffer()
                {
                    eventBuffer.clear();
                }

                void checkEvent( Event pEvent )
                {
                    if ( !occurred && pEvent == signature ) {
                        occurred = true;
                    }
                }

                static void checkAll()
                {
                    for( Listener<Event>& instance : all ) {
                        instance.occurred = false;
                        for( const Event& event : eventBuffer ) {
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
                        resetters.push_back( clearBuffer );
                    }
                };
                static At_Init init;
        };

    template<typename Event>
        std::vector<Listener<Event>> Listener<Event>::all = std::vector<Listener<Event>>();

    template<typename Event>
        std::vector<Event> Listener<Event>::eventBuffer = std::vector<Event>();

    template<typename Event>
        size_t Listener<Event>::templateIndex = 0;

    template<typename Event>
        typename Listener<Event>::At_Init Listener<Event>::init = Listener<Event>::At_Init();

    template<typename Event>
        void pushEvent( Event pEvent )
        {
            Listener<Event>::push( pEvent );
        }

    struct ListenerID
    {
        ListenerID()
            : templateIndex( 0 )
            , instanceIndex( 0 )
        {}
        template<typename Event>
            ListenerID( const Listener<Event> pListener )
            : templateIndex( Listener<Event>::templateIndex )
            , instanceIndex( Listener<Event>::all.size() )
        {
            Listener<Event>::all.push_back( pListener );
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
            return ListenerID( Listener<Event>( pEvent ) );
        }
}
