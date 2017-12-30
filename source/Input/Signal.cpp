#include "stdafx.h"
#include "Signal.h"

std::vector<bool> app::Input::allSignals;
std::vector<void(*)()> app::Input::signalCheckers;//checks all Signal templates
std::vector<void(*)()> app::Input::signalDestructors;//destroys all Signal templates

void app::Input::clearSignals() {
	allSignals.clear();
	for (void(*&destructor)() : signalDestructors) {
		destructor();
	}
}
void app::Input::resetSignals()
{
	for (bool&& signal : allSignals) {
		signal = 0;
	}
}

void app::Input::checkSignals() {
	for (void(*&checker)() : signalCheckers) {
		checker();
	}
}

