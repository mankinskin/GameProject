#ifdef WINDOWS
    #include <ncurses.h>
#else
    #include <curses.h>
#endif
#include "app.h"
#include <stdio.h>
int main() {
	app::init();
	return 0;
}
