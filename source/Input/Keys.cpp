#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Keys.h"
#include "Signal.h"
#include "Event.h"
#include "EventGates.h"
using namespace signals;
using namespace events;
app::Input::KeySignal::KeySignal(int pKey)
{
	Event press_evt = createEvent(KeyEvent(pKey, KeyCondition(1, 0)));
	Event release_evt = createEvent(KeyEvent(pKey, KeyCondition(0, 0)));
	press = createSignal(Event(press_evt));
	release = createSignal(Event(release_evt));
	auto togglers = xor_gate<Event, Event>(Event(press_evt), Event(release_evt));
	hold = createSignal(toggle_gate<decltype(togglers)>(togglers));
}

void app::Input::reserveKeySignals(size_t pCount) {
	allSignals.reserve(allSignals.size() + pCount);
	EventSlot<KeyEvent>::reserve_slots(pCount*3);
}
void app::Input::key_Callback(GLFWwindow * window, int pKey, int pScancode, int pAction, int pMods)
{
	pushEvent(KeyEvent(pKey, pAction, pMods));
}

void app::Input::char_Callback(GLFWwindow * window, size_t pCodepoint)
{
	//printf("char callBack! Char: %c \n", pCodepoint);
}