#include "functor.h"
#include "event.h"

std::vector<void(*)(const size_t)> events::invokers = std::vector<void(*)(const size_t)>();
std::vector<void( * )()> events::destructors = std::vector<void( * )()>();

void events::clearFunctors()
{
    for ( void( *&destructor )() : destructors ) {
        destructor();
    }
}
