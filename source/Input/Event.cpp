#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Event.h"

std::vector<app::Input::SignalSlot> app::Input::allSignals;
std::vector<void(*)()> app::Input::EventInternal::eventCheckers;
std::vector<void(*)()> app::Input::EventInternal::eventDestructors;

void app::Input::clearSignals() {
	allSignals.clear();
}
void app::Input::resetSignals()
{
	for (SignalSlot& signal : allSignals) {
		signal.signaled *= signal.remain;
	}
}
void app::Input::checkEvents() {
	for (void(*&checker)() : EventInternal::eventCheckers) {
		checker();
	}
}
void app::Input::clearEvents() {
	for (void(*&destructor)() : EventInternal::eventDestructors) {
		destructor();
	}
}

size_t app::Input::createSignal(int pRemain, int pStartWith) {
	allSignals.emplace_back(pRemain, pStartWith);
	return allSignals.size() - 1;
}