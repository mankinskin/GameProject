#include "debug.h"
#include "app.h"
#include <ncurses.h>
#include <curses.h>
#include <cstdio>
#include <algorithm>

bool debug::shouldPrintInfo = true;
std::vector<debug::Error> debug::errorBuffer;

void debug::pushError( std::string pMessage, Error::Severity errorSeverity )
{
	errorBuffer.emplace_back( pMessage, errorSeverity );
}

void debug::printErrors()
{
	int fatal = 0;
	unsigned int errorCount = errorBuffer.size();
	if ( errorCount ) {
		printf( "\n%i Error( s ) or Warning( s ) occured.\n", errorCount );
	}
	for ( Error& err : errorBuffer ) {
		std::string severityString;
		switch ( err.severity ) {
		case Error::Severity::Warning:
			severityString = "Warning";
			break;

		case Error::Severity::Trivial:
			severityString = "Trivial Error";
			break;

		case Error::Severity::Fatal:
			fatal = 1;
			severityString = "Fatal Error";
			break;
		}
		printf( "%s: %s\n", severityString.c_str(), err.msg.c_str() );
	}
	errorBuffer.clear();
	if ( fatal ) {
		app::state = app::State::Exit;
		puts( "Press any key to quit..." );
		while ( !getch() ) {};
	}

}

void debug::togglePrintInfo() {
	shouldPrintInfo = !shouldPrintInfo;
}

void debug::printInfo()
{
	if ( shouldPrintInfo ) {
		app::lastFrameMS = std::max( 1.0, app::lastFrameMS );
		printf( "\nlastFrameMS %i\n", ( int )app::lastFrameMS );
		printf( "FPS\nActual %i\nPotential %i\n", ( int )( 1000 / ( std::max( app::minFrameMS, app::lastFrameMS ) ) ), ( int )( 1000 / ( app::lastFrameMS ) ) );
	}
}
