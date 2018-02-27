#ifdef WINDOWS
    #include <conio.h>
#else
    #include <curses.h>
#endif
#include "app.h"

int main() {
	app::init();
	return 0;
}
