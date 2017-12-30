#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Event.h"

std::vector<bool> app::Input::allEventStates;
std::vector<void(*)()> app::Input::eventCheckers;
std::vector<void(*)()> app::Input::eventDestructors;


void app::Input::checkEvents() {
	for (void(*&checker)() : eventCheckers) {
		checker();
	}
}
void app::Input::clearEvents() {
	for (void(*&destructor)() : eventDestructors) {
		destructor();
	}
}

void app::Input::resetEvents() {
	std::fill(allEventStates.begin(), allEventStates.end(), false);
}