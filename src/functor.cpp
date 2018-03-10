#include "functor.h"
#include "event.h"

std::vector<void(*)(unsigned int)> functors::functorInvokers = std::vector<void(*)(unsigned int)>();
std::vector<void(*)()> functors::functorDestructors = std::vector<void(*)()>();
std::vector<std::pair<unsigned int, unsigned int>> functors::functorOrder = std::vector<std::pair<unsigned int, unsigned int>>();


void functors::clearFunctors() {
	for (void(*&destructor)() : functorDestructors) {
		destructor();
	}
}
void functors::callFunctors()
{
	for (std::pair<unsigned int, unsigned int>& funi : functorOrder) {
		functorInvokers[funi.first](funi.second);
	}
}
