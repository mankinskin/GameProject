#include "functor.h"
#include "event.h"
std::vector<void(*)(const size_t)> functors::invokers = std::vector<void(*)(const size_t)>();
std::vector<void( * )()> functors::destructors = std::vector<void( * )()>();

void functors::clearFunctors() 
{
    for ( void( *&destructor )() : destructors ) {
        destructor();
    }
}
