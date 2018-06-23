#pragma once
#include "signal.h"
#include "gates.h"
#include <vector>

namespace events 
{
    // Events set signals when their signature gets pushed into its queue ( with pushEvent )
    // Events are templated, thus events of any type can be tracked. 

    void checkEvents();
    void resetEvents();

    extern std::vector<void(*)()> checkers;  // checks all Event queues
    extern std::vector<void(*)()> resetters; // resets all Event queues

    template<typename T>
        class Event
        {
            public:
                using ID = utils::ID<Event<T>>;
                static constexpr typename ID::Container& all = ID::container;

                Event( T&& pSignature, signals::Signal::ID pSignal = signals::Signal::ID() )
                    : signature( pSignature )
                    , signal( pSignal )
                {}

                static void clearQueue()
                {
                    queue.clear();
                }

                // throw signal if sig matches
                void check( const T& sig )
                {
                    if ( sig == signature ) {
                        signal->state = true;
                    }
                }

                // throws the signals of the events in queue
                static void checkQueue()
                {
                    for ( size_t q = 0; q < queue.size(); ++q ) {
                        for ( size_t e = 0; e < all.size(); ++e ) {
                            all[e].check( queue[q] );
                        }
                    }
                }

                const signals::Signal::ID signal;
                T signature;
                static std::vector<T> queue;
            private:
                struct At_Init
                {
                    At_Init()
                    {
                        checkers.push_back( checkQueue );
                        resetters.push_back( clearQueue ); 
                    }
                };
                static At_Init init;
        };

    template<typename T>
        std::vector<T> Event<T>::queue = std::vector<T>();
    template<typename T>
        typename Event<T>::At_Init Event<T>::init = Event<T>::At_Init();

    template<typename T>
        typename Event<T>::ID createEvent( Event<T> pEvent )
        {
            return Event<T>::ID( pEvent );
        }
    template<typename T>
        void pushEvent( T pEvent )
        {
            Event<T>::queue.push_back( pEvent );
        }
}
