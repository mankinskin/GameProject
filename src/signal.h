#pragma once
#include "utils/id.h"

namespace signals 
{
    //Signals are simply a uniform way of transmitting signals between Events and functors
    //Functors store indices of signals to check whether they should invoke or not
    //Signals use Events ( or Gates ) as sources for their state

    void resetSignals();
    void clearSignals();

    struct Signal
    {
        using ID = utils::ID<Signal>;
        static constexpr typename ID::Container& all = ID::container;

        Signal()
            :state( false )
        {}
        Signal( bool s )
            : state( s )
        {}
        bool state;
    };
}
