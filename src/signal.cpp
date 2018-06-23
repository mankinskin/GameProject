#include "signal.h"

void signals::resetSignals()
{
    std::fill( Signal::all.begin(), Signal::all.end(), Signal() );
}

void signals::clearSignals() 
{
    Signal::all.clear();
}
