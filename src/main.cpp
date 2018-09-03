#include "app.h"
#ifdef WINDOWS
#include <conio.h>
#else
#include <curses.h>
#endif

int main()
{
  app::init();
  return 0;
}
