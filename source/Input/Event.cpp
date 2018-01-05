#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Event.h"

std::vector<bool> app::Input::allEventStates;
std::vector<void(*)()> app::Input::eventCheckers = std::vector<void(*)()>();
std::vector<void(*)()> app::Input::eventDestructors = std::vector<void(*)()>();
std::vector<void(*)()> app::Input::eventResetters = std::vector<void(*)()>();

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
	for (unsigned int i = 0; i < eventResetters.size(); ++i) {
		eventResetters[i]();
	}
	std::fill(allEventStates.begin(), allEventStates.end(), false);
}

