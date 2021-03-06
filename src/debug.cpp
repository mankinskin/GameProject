#include "debug.h"
#include "app.h"
#include <ncurses.h>
#include <curses.h>
#include <cstdio>
#include <algorithm>

std::vector<debug::Error> debug::errorBuffer;
const std::array<const std::string, 3> debug::Error::severityStrings =
{
  "Warning",
  "Error",
  "Fatal Error"
};

void debug::setError(std::string pMessage, Error::Severity errorSeverity)
{
  errorBuffer.emplace_back(pMessage, errorSeverity);
}
void debug::error(std::string pMessage)
{
  setError(pMessage, Error::Severity::Trivial);
}
void debug::warning(std::string pMessage)
{
  setError(pMessage, Error::Severity::Warning);
}
void debug::fatal(std::string pMessage)
{
  setError(pMessage, Error::Severity::Fatal);
}
void debug::printErrors()
{
  if (errorBuffer.size()) {
	bool fatal = 0;
	for (Error& err : errorBuffer) {
	  if (err.severity == Error::Severity::Fatal) {
		fatal = true;
	  }
	  printf("%s: %s\n", Error::severityStrings[ err.severity ].c_str(), err.msg.c_str());
	}
	errorBuffer.clear();
	if (fatal) {
	  app::state = app::State::Exit;
	  puts("Fatal Errors occured.\nPress any key to quit...");
	  while (!getch()) {};
	}
  }
}
