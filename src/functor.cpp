#include "functor.h"
#include "event.h"

std::vector<void(*)(size_t)> functors::functorInvokers = std::vector<void(*)(size_t)>();
std::vector<void(*)()> functors::functorDestructors = std::vector<void(*)()>();
std::vector<std::pair<size_t, size_t>> functors::functorOrder = std::vector<std::pair<size_t, size_t>>();


void functors::clearFunctors() {
	for (void(*&destructor)() : functorDestructors) {
		destructor();
	}
}
void functors::callFunctors()
{
	for (std::pair<size_t, size_t>& funi : functorOrder) {
		functorInvokers[funi.first](funi.second);
	}
}
