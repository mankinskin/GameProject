#include "signal.h"

std::vector<bool> signals::allSignals;
std::vector<void(*)()> signals::signalCheckers;//checks all Signal templates
std::vector<void(*)()> signals::signalDestructors;//destroys all Signal templates

void signals::clearSignals() {
	allSignals.clear();
	for (void(*&destructor)() : signalDestructors) {
		destructor();
	}
}
void signals::resetSignals()
{
	std::fill(allSignals.begin(), allSignals.end(), false);
}

void signals::checkSignals() {
	for (void(*&checker)() : signalCheckers) {
		checker();
	}
}

