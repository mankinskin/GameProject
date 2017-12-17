#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Keys.h"

app::Input::KeySignal::KeySignal(int pKey)
:held(createSignal(1))
{
	press = createEvent(KeyEvent(pKey, KeyCondition(1, 0)));
	release = createEvent(KeyEvent(pKey, KeyCondition(0, 0)));
	createEvent(KeyEvent(pKey, KeyCondition(1, 0)), held, 1);
	createEvent(KeyEvent(pKey, KeyCondition(0, 0)), held, 0);
}

void app::Input::reserveKeySignals(size_t pCount)
{
	EventSignal<KeyEvent>::reserve(pCount * 2);
}


void app::Input::key_Callback(GLFWwindow * window, int pKey, int pScancode, int pAction, int pMods)
{
	pushEvent(KeyEvent(pKey, pAction, pMods));
}

void app::Input::char_Callback(GLFWwindow * window, size_t pCodepoint)
{
	//printf("char callBack! Char: %c \n", pCodepoint);
}