#include "functor.h"

std::vector<void(*)()> signals::functorClearFuncs;

void signals::clearFunctors()
{
    for (void(*&clear)() : functorClearFuncs) {
        clear();
    }
}
