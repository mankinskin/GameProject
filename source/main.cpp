#include <conio.h>
#include "app.h"


int main() {

	app::init();
	while (app::state != app::State::Exit) {
		app::gameloop();
	}

	return 0;
}
