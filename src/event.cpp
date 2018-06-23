#include "event.h"

std::vector<void(*)()> events::checkers = std::vector<void(*)()>();
std::vector<void(*)()> events::resetters = std::vector<void(*)()>();

void events::checkEvents() 
{
    for ( void( *&checker )() : checkers ) {
        checker();
    }
}

void events::resetEvents() 
{
    for ( void( *&resetter )() : resetters ) {
        resetter();
    }
}

