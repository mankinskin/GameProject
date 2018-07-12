#include "event.h"

std::vector<void(*)()> signals::hidden::eventCheckFuncs;
std::vector<void(*)()> signals::hidden::eventInstanceClearFuncs;

void signals::checkEvents()
{
    for (void(*&check)() : hidden::eventCheckFuncs) {
        check();
    }
}
void signals::clearEvents()
{
    for (void(*&clear)() : hidden::eventInstanceClearFuncs) {
        clear();
    }
}
