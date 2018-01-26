#include"Global\stdafx.h"
#include "Global\app.h"
#include <conio.h>

int main() {

	app::init();
	while (app::state != app::State::Exit) {
		app::gameloop();
	}

	return 0;
}
