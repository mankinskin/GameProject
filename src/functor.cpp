#include "functor.h"
#include "event.h"

std::vector<void(*)(const size_t)> signals::invokers = std::vector<void(*)(const size_t)>();
std::vector<void( * )()> signals::destructors = std::vector<void( * )()>();

