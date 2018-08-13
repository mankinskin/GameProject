#include "functor.h"

std::vector<void(*)()> signals::funcClearFuncs;

void signals::clearFunctors()
{
    for (void(*&clear)() : funcClearFuncs) {
        clear();
    }
}
