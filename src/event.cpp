#include "event.h"

std::vector<void(*)()> signals::checkers = std::vector<void(*)()>();
std::vector<bool(*)(size_t)> signals::staters = std::vector<bool(*)(size_t)>();
std::vector<void(*)()> signals::clearers = std::vector<void(*)()>();
size_t signals::template_count = 0;

void signals::checkEvents()
{
    for ( void( *&checker )() : checkers ) {
        checker();
    }
}

void signals::clearEvents()
{
    for ( void( *&clearer )() : clearers ) {
        clearer();
    }
}

