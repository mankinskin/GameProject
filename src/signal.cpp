#include "signal.h"

std::vector<void(*)()> signals::hidden::signalClearFuncs;

void signals::clearSignals()
{
    for (void(*&clear)() : hidden::signalClearFuncs) {
        clear();
    }
}
