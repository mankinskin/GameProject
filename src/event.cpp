#include "event.h"

std::vector<bool> events::allEventStates;
std::vector<void( * )()> events::eventCheckers = std::vector<void( * )()>();
std::vector<void( * )()> events::eventDestructors = std::vector<void( * )()>();
std::vector<void( * )()> events::eventResetters = std::vector<void( * )()>();

void events::checkEvents() {
	for ( void( *&checker )() : eventCheckers ) {
		checker();
	}
}
void events::clearEvents() {
	for ( void( *&destructor )() : eventDestructors ) {
		destructor();
	}
}

void events::resetEvents() {
	for ( unsigned int i = 0; i < eventResetters.size(); ++i ) {
		eventResetters[i]();
	}
	std::fill( allEventStates.begin(), allEventStates.end(), false );
}

