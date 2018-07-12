#include "functor.h"

std::vector<void(*)()> signals::hidden::functorClearFuncs;

void signals::clearFunctors()
{
    for (void(*&clear)() : hidden::functorClearFuncs) {
        clear();
    }
}
