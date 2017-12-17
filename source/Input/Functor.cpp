#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Functor.h"
#include "Event.h"

std::vector<void(*)(size_t)> app::Input::functorInvokers = std::vector<void(*)(size_t)>();
std::vector<void(*)()> app::Input::functorDestructors = std::vector<void(*)()>();
std::vector<std::pair<size_t, size_t>> app::Input::functorOrder = std::vector<std::pair<size_t, size_t>>();

bool app::Input::is_on(size_t pSignalIndex)
{
	return allSignals[pSignalIndex].signaled;
}

void app::Input::clearFunctors() {
	for (void(*&destructor)() : functorDestructors) {
		destructor();
	}
}
void app::Input::callFunctors()
{
	for (std::pair<size_t, size_t>& funi : functorOrder) {
		functorInvokers[funi.first](funi.second);
	}
}
