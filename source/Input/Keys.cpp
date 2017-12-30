#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Keys.h"
#include "Signal.h"
app::Input::KeySignal::KeySignal(int pKey)
{
	press = createSignal(EventSource(createEvent(KeyEvent(pKey, KeyCondition(1, 0)))));
	release = createSignal(EventSource(createEvent(KeyEvent(pKey, KeyCondition(0, 0)))));
	hold = createSignal(toggle_gate<or_gate<EventSource, EventSource>>(or_gate<EventSource, EventSource>(EventSource(createEvent(KeyEvent(pKey, KeyCondition(1, 0)))), EventSource(createEvent(KeyEvent(pKey, KeyCondition(0, 0)))))));
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