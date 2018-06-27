#include "event.h"

std::vector<void(*)()> events::checkers = std::vector<void(*)()>();
std::vector<bool(*)(size_t)> events::staters = std::vector<bool(*)(size_t)>();
std::vector<void(*)()> events::resetters = std::vector<void(*)()>();
size_t events::template_count = 0;

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

